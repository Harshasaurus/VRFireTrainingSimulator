#include "VRFireExtinguisher.h"
#include "VRFire.h"
#include "VRSimulationManager.h"
#include "Components/SphereComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Particles/ParticleSystemComponent.h"
#include "DrawDebugHelpers.h"

AVRFireExtinguisher::AVRFireExtinguisher()
{
    PrimaryActorTick.bCanEverTick = true;

    SprayParticle = CreateDefaultSubobject<UParticleSystemComponent>(TEXT("SprayParticle"));
    SprayParticle->SetupAttachment(RootComponent);
    SprayParticle->SetAutoActivate(false);
}

void AVRFireExtinguisher::BeginPlay()
{
    Super::BeginPlay();
}

void AVRFireExtinguisher::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);

    if (bIsSpraying)
        SprayTick(DeltaTime);
}

void AVRFireExtinguisher::Grab(USceneComponent* AttachTo)
{
    Super::Grab(AttachTo);
    UE_LOG(LogTemp, Warning, TEXT("Extinguisher grabbed!"));

    if (!bHasGrabbedBefore)
    {
        bHasGrabbedBefore = true;
        OnExtinguisherGrabbed.Broadcast();
    }
}

void AVRFireExtinguisher::Release(FVector ThrowVelocity)
{
    bIsSpraying = false;
    bPinPulled = false;
    bHasStartedSprayBefore = false;
    bHasGrabbedBefore = false;
    SprayParticle->Deactivate();

    Super::Release(ThrowVelocity);
    UE_LOG(LogTemp, Warning, TEXT("Extinguisher released!"));
    OnExtinguisherReleased.Broadcast();
}

void AVRFireExtinguisher::PullPin()
{
    if (!bIsGrabbed || bPinPulled) return;

    bPinPulled = true;
    UE_LOG(LogTemp, Warning, TEXT("Pin pulled!"));
    OnPinPulled.Broadcast();
}

void AVRFireExtinguisher::StartSpray()
{
    if (!bPinPulled) return;

    bIsSpraying = true;
    SprayParticle->Activate();
    UE_LOG(LogTemp, Warning, TEXT("Spraying!"));

    if (!bHasStartedSprayBefore)
    {
        bHasStartedSprayBefore = true;
        OnSprayStarted.Broadcast();
    }
}

void AVRFireExtinguisher::StopSpray()
{
    bIsSpraying = false;
    SprayParticle->Deactivate();
    UE_LOG(LogTemp, Warning, TEXT("Stopped spraying!"));
}

void AVRFireExtinguisher::SprayTick(float DeltaTime)
{
    TArray<AActor*> FireActors;
    UGameplayStatics::GetAllActorsOfClass(GetWorld(),
        AVRFire::StaticClass(), FireActors);

    // NEW: track the best aim this tick, across every fire in range —
    // this is what gets reported back for player feedback
    float BestEffectivenessThisTick = 0.0f;

    for (AActor* Actor : FireActors)
    {
        AVRFire* Fire = Cast<AVRFire>(Actor);
        if (!Fire) continue;

        float Distance = FVector::Dist(
            GetActorLocation(), Fire->GetActorLocation());

        if (Distance <= SprayRange)
        {
            // Scale extinguish effect by how well the nozzle is aimed.
            // Perfectly aimed = 1.0 (full rate), badly aimed = fades to 0.0.
            float AimEffectiveness = GetAimEffectiveness(Fire->GetActorLocation());

            if (AimEffectiveness > 0.0f)
            {
                Fire->ApplyExtinguisher(DeltaTime * AimEffectiveness);
            }

            BestEffectivenessThisTick = FMath::Max(BestEffectivenessThisTick, AimEffectiveness);

            if (bDrawDebugAim)
            {
                FColor LineColor = FColor::MakeRedToGreenColorFromScalar(AimEffectiveness);
                DrawDebugLine(GetWorld(), GetActorLocation(), Fire->GetActorLocation(),
                    LineColor, false, -1.f, 0, 2.f);

                // NEW: direct text readout, since debug lines don't always
                // render through the VR/HMD viewport. Shows up in Output Log.
                FVector NozzleForward = GetActorForwardVector();
                FVector DirectionToFire = (Fire->GetActorLocation() - GetActorLocation()).GetSafeNormal();
                float Dot = FVector::DotProduct(NozzleForward, DirectionToFire);
                float AngleDeg = FMath::RadiansToDegrees(FMath::Acos(FMath::Clamp(Dot, -1.0f, 1.0f)));

                UE_LOG(LogTemp, Warning,
                    TEXT("AIM DEBUG | Angle: %.1f deg | Effectiveness: %.2f | NozzleFwd: %s | ToFire: %s"),
                    AngleDeg, AimEffectiveness,
                    *NozzleForward.ToString(), *DirectionToFire.ToString());
            }
        }
    }

    // NEW: report aim quality back every tick while spraying, even if no
    // fire is currently in range (effectiveness just reads 0.0 in that case)
    CurrentAimEffectiveness = BestEffectivenessThisTick;
    OnAimEffectivenessChanged.Broadcast(CurrentAimEffectiveness);

    TArray<AActor*> Managers;
    UGameplayStatics::GetAllActorsOfClass(
        GetWorld(), AVRSimulationManager::StaticClass(), Managers);
    if (Managers.Num() > 0)
    {
        AVRSimulationManager* SimulationManager = Cast<AVRSimulationManager>(Managers[0]);
        if (SimulationManager)
            SimulationManager->RecordAimAccuracySample(CurrentAimEffectiveness);
    }
}

// ----------------------------------------------------------------
// NEW: Aim accuracy check
// ----------------------------------------------------------------

float AVRFireExtinguisher::GetAimEffectiveness(const FVector& FireLocation) const
{
    // Direction the nozzle is actually pointing.
    // Assumes the extinguisher mesh's forward (+X) axis points out the
    // nozzle — if your mesh's forward axis is different, rotate the mesh
    // in the editor or adjust this to use a dedicated nozzle socket instead.
    FVector NozzleForward = GetActorForwardVector();

    // Direction from the nozzle to the fire
    FVector DirectionToFire = (FireLocation - GetActorLocation()).GetSafeNormal();

    float Dot = FVector::DotProduct(NozzleForward, DirectionToFire);
    Dot = FMath::Clamp(Dot, -1.0f, 1.0f); // guard against float precision issues
    float AngleDegrees = FMath::RadiansToDegrees(FMath::Acos(Dot));

    if (AngleDegrees <= AimToleranceAngle)
    {
        return 1.0f; // dead on target — full effectiveness
    }
    else if (AngleDegrees >= AimFalloffAngle)
    {
        return 0.0f; // too far off — no effect at all
    }
    else
    {
        // Linear fade between tolerance and falloff angles
        float Range = AimFalloffAngle - AimToleranceAngle;
        float Progress = (AngleDegrees - AimToleranceAngle) / Range;
        return 1.0f - Progress;
    }
}

