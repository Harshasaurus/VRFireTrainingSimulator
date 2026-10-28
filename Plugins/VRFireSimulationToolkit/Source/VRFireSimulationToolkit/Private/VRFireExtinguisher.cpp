#include "VRFireExtinguisher.h"
#include "VRFire.h"
#include "VRSimulationManager.h"
#include "VRAimWidget.h"
#include "Components/SphereComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Components/WidgetComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Camera/PlayerCameraManager.h"
#include "Particles/ParticleSystemComponent.h"
#include "DrawDebugHelpers.h"

AVRFireExtinguisher::AVRFireExtinguisher()
{
    PrimaryActorTick.bCanEverTick = true;

    SprayParticle = CreateDefaultSubobject<UParticleSystemComponent>(TEXT("SprayParticle"));
    SprayParticle->SetupAttachment(RootComponent);
    SprayParticle->SetAutoActivate(false);

    AimWidgetComp = CreateDefaultSubobject<UWidgetComponent>(TEXT("AimWidgetComp"));
    AimWidgetComp->SetupAttachment(RootComponent);
    AimWidgetComp->SetWidgetClass(UVRAimWidget::StaticClass());
    AimWidgetComp->SetWidgetSpace(EWidgetSpace::World);
    AimWidgetComp->SetDrawSize(FVector2D(400, 200));
    AimWidgetComp->SetRelativeLocation(FVector(0, 0, 40));
    AimWidgetComp->SetRelativeScale3D(FVector(0.15f));
    AimWidgetComp->SetCollisionEnabled(ECollisionEnabled::NoCollision);
    AimWidgetComp->SetVisibility(false);

    FireAimHeightOffset = 60.0f;
    PitchToleranceAngle = 30.0f;
    PitchFalloffAngle = 60.0f;
    bIgnorePitch = false;
    FirePitchBias = 0.0f;
    bDrawDebugAim = true;
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

    UpdateAimWidget();
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
    UpdateSprayColor(1.0f, false);
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

    float BestEffectivenessThisTick = 0.0f;
    bool bFireInRange = false;

    for (AActor* Actor : FireActors)
    {
        AVRFire* Fire = Cast<AVRFire>(Actor);
        if (!Fire) continue;

        float Distance = FVector::Dist(
            GetActorLocation(), Fire->GetActorLocation());

        if (Distance <= SprayRange)
        {
            bFireInRange = true;

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
                LogAimDebug(Fire->GetActorLocation(), AimEffectiveness);
            }
        }
    }

    CurrentAimEffectiveness = BestEffectivenessThisTick;
    OnAimEffectivenessChanged.Broadcast(CurrentAimEffectiveness);
    UpdateSprayColor(CurrentAimEffectiveness, bFireInRange);

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

FLinearColor AVRFireExtinguisher::GetAimColor(float Effectiveness) const
{
    if (Effectiveness >= GoodAimThreshold)
    {
        return FLinearColor::White;
    }

    if (Effectiveness >= PartialAimThreshold)
    {
        return FLinearColor(1.f, 1.f, 0.f);
    }

    return FLinearColor::Red;
}

void AVRFireExtinguisher::UpdateAimWidget()
{
    if (!AimWidgetComp)
    {
        return;
    }

    const bool bShow = bShowAimWidget && bIsGrabbed && bPinPulled;
    AimWidgetComp->SetVisibility(bShow);
    if (!bShow)
    {
        return;
    }

    TArray<AActor*> FireActors;
    UGameplayStatics::GetAllActorsOfClass(GetWorld(), AVRFire::StaticClass(), FireActors);

    AVRFire* NearestFire = nullptr;
    float BestDistanceSquared = MAX_FLT;
    for (AActor* Actor : FireActors)
    {
        AVRFire* Fire = Cast<AVRFire>(Actor);
        if (!Fire)
        {
            continue;
        }

        const float DistanceSquared = FVector::DistSquared(GetActorLocation(), Fire->GetActorLocation());
        if (DistanceSquared < BestDistanceSquared)
        {
            BestDistanceSquared = DistanceSquared;
            NearestFire = Fire;
        }
    }

    UVRAimWidget* AimWidget = Cast<UVRAimWidget>(AimWidgetComp->GetUserWidgetObject());
    if (!AimWidget)
    {
        return;
    }

    if (!NearestFire)
    {
        CurrentAimAngle = -1.0f;
        AimWidget->SetAim(-1, FLinearColor::White, TEXT("No fire"));
    }
    else
    {
        const FVector FireLocation = NearestFire->GetActorLocation();
        const float Effectiveness = GetAimEffectiveness(FireLocation);
        float Pitch = 0.0f;
        float PitchError = 0.0f;
        float YawError = 0.0f;
        float YawSigned = 0.0f;
        FString Hint;

        if (bUsePitchAim)
        {
            GetAimErrors(FireLocation, Pitch, PitchError, YawError, YawSigned);
            CurrentAimAngle = Pitch;

            if (YawError > AimToleranceAngle)
            {
                Hint = YawSigned > 0.0f ? TEXT("Turn RIGHT") : TEXT("Turn LEFT");
            }
            else if (!bIgnorePitch && PitchError > PitchToleranceAngle)
            {
                Hint = Pitch < GetTargetPitch(FireLocation) ? TEXT("Aim DOWN") : TEXT("Aim UP");
            }
            else
            {
                Hint = TEXT("ON TARGET");
            }
        }
        else
        {
            const FVector Local = GetActorTransform()
                .InverseTransformVectorNoScale(FireLocation - GetActorLocation()).GetSafeNormal();
            CurrentAimAngle = FMath::RadiansToDegrees(
                FMath::Acos(FMath::Clamp(Local.X, -1.0f, 1.0f)));

            if (CurrentAimAngle <= AimToleranceAngle)
            {
                Hint = TEXT("ON TARGET");
            }
            else if (FMath::Abs(Local.Y) > FMath::Abs(Local.Z))
            {
                Hint = Local.Y > 0.0f ? TEXT("Turn RIGHT") : TEXT("Turn LEFT");
            }
            else
            {
                Hint = Local.Z > 0.0f ? TEXT("Aim UP") : TEXT("Aim DOWN");
            }
        }

        AimWidget->SetAim(FMath::Max(0, FMath::RoundToInt(CurrentAimAngle)), GetAimColor(Effectiveness), Hint);
    }

    if (APlayerCameraManager* CameraManager = UGameplayStatics::GetPlayerCameraManager(this, 0))
    {
        AimWidgetComp->SetWorldRotation(
            (CameraManager->GetCameraLocation() - AimWidgetComp->GetComponentLocation()).Rotation());
    }
}

void AVRFireExtinguisher::UpdateSprayColor(float Effectiveness, bool bFireInRange)
{
    if (!SprayParticle) return;

    FLinearColor Color = FLinearColor::White;
    if (bFireInRange)
    {
        if (Effectiveness >= GoodAimThreshold)
        {
            Color = FLinearColor::White;
        }
        else if (Effectiveness >= PartialAimThreshold)
        {
            Color = FLinearColor(1.f, 1.f, 0.f);
        }
        else
        {
            Color = FLinearColor::Red;
        }
    }

    SprayParticle->SetColorParameter(SprayColorParameterName, Color);
}

float AVRFireExtinguisher::GetTargetPitch(const FVector& FireLocation) const
{
    if (!bTargetFirePitch)
    {
        return TargetPitchAngle;
    }

    const FVector TargetLocation = FireLocation + FVector(0.0f, 0.0f, FireAimHeightOffset);
    const FVector Dir = (TargetLocation - GetActorLocation()).GetSafeNormal();
    return FMath::RadiansToDegrees(
        FMath::Asin(FMath::Clamp(-Dir.Z, -1.0f, 1.0f))) + FirePitchBias;
}

void AVRFireExtinguisher::GetAimErrors(const FVector& FireLocation, float& OutPitch, float& OutPitchError, float& OutYawError, float& OutYawSigned) const
{
    const FVector Fwd = GetActorForwardVector();
    OutPitch = FMath::RadiansToDegrees(FMath::Asin(FMath::Clamp(-Fwd.Z, -1.0f, 1.0f)));
    OutPitchError = FMath::Abs(OutPitch - GetTargetPitch(FireLocation));

    const FVector ActorLocation = GetActorLocation();
    const FVector F2D = FVector(Fwd.X, Fwd.Y, 0.0f).GetSafeNormal();
    const FVector T2D = FVector(FireLocation.X - ActorLocation.X, FireLocation.Y - ActorLocation.Y, 0.0f).GetSafeNormal();
    if (F2D.IsNearlyZero() || T2D.IsNearlyZero())
    {
        OutYawError = 0.0f;
        OutYawSigned = 0.0f;
        return;
    }

    OutYawError = FMath::RadiansToDegrees(
        FMath::Acos(FMath::Clamp(FVector::DotProduct(F2D, T2D), -1.0f, 1.0f)));
    OutYawSigned = FVector::CrossProduct(F2D, T2D).Z >= 0.0f ? OutYawError : -OutYawError;
}

float AVRFireExtinguisher::GetAimEffectiveness(const FVector& FireLocation) const
{
    if (bUsePitchAim)
    {
        float Pitch = 0.0f;
        float PitchError = 0.0f;
        float YawError = 0.0f;
        float YawSigned = 0.0f;
        GetAimErrors(FireLocation, Pitch, PitchError, YawError, YawSigned);

        const auto ComputeFade = [](float Error, float Tolerance, float Falloff) -> float
        {
            if (Error <= Tolerance)
            {
                return 1.0f;
            }
            if (Error >= Falloff)
            {
                return 0.0f;
            }
            const float Range = Falloff - Tolerance;
            if (Range <= 0.0f)
            {
                return 0.0f;
            }
            return 1.0f - (Error - Tolerance) / Range;
        };

        const float YawFade = ComputeFade(YawError, AimToleranceAngle, AimFalloffAngle);
        const float PitchFade = bIgnorePitch ? 1.0f : ComputeFade(PitchError, PitchToleranceAngle, PitchFalloffAngle);

        return FMath::Min(PitchFade, YawFade);
    }

    // Direction the nozzle is actually pointing.
    // Assumes the extinguisher mesh's forward (+X) axis points out the nozzle.
    FVector NozzleForward = GetActorForwardVector();

    // Direction from the nozzle to the fire
    FVector DirectionToFire = (FireLocation - GetActorLocation()).GetSafeNormal();

    float Dot = FVector::DotProduct(NozzleForward, DirectionToFire);
    Dot = FMath::Clamp(Dot, -1.0f, 1.0f); // guard against float precision issues
    float AngleDegrees = FMath::RadiansToDegrees(FMath::Acos(Dot));

    if (AngleDegrees <= AimToleranceAngle)
    {
        return 1.0f; // dead on target - full effectiveness
    }
    else if (AngleDegrees >= AimFalloffAngle)
    {
        return 0.0f; // too far off - no effect at all
    }
    else
    {
        // Linear fade between tolerance and falloff angles
        float Range = AimFalloffAngle - AimToleranceAngle;
        if (Range <= 0.0f)
        {
            return 0.0f;
        }
        float Progress = (AngleDegrees - AimToleranceAngle) / Range;
        return 1.0f - Progress;
    }
}

void AVRFireExtinguisher::LogAimDebug(const FVector& FireLocation, float Effectiveness) const
{
    float Pitch = 0.0f;
    float PitchError = 0.0f;
    float YawError = 0.0f;
    float YawSigned = 0.0f;
    GetAimErrors(FireLocation, Pitch, PitchError, YawError, YawSigned);

    const float TargetPitch = GetTargetPitch(FireLocation);

    const auto ComputeFade = [](float Error, float Tolerance, float Falloff) -> float
    {
        if (Error <= Tolerance) return 1.0f;
        if (Error >= Falloff) return 0.0f;
        const float Range = Falloff - Tolerance;
        return Range > 0.0f ? (1.0f - (Error - Tolerance) / Range) : 0.0f;
    };

    const float PitchFade = bIgnorePitch ? 1.0f : ComputeFade(PitchError, PitchToleranceAngle, PitchFalloffAngle);
    const float YawFade = ComputeFade(YawError, AimToleranceAngle, AimFalloffAngle);

    const FVector NozzleForward = GetActorForwardVector();
    const FVector TargetLocation = FireLocation + FVector(0.0f, 0.0f, FireAimHeightOffset);
    const FVector DirectionToTarget = (TargetLocation - GetActorLocation()).GetSafeNormal();
    const float Dot = FMath::Clamp(FVector::DotProduct(NozzleForward, DirectionToTarget), -1.0f, 1.0f);
    const float AngleDeg = FMath::RadiansToDegrees(FMath::Acos(Dot));
    const float Distance = FVector::Dist(GetActorLocation(), FireLocation);

    // Existing AIM DEBUG format augmented with height offset, pitch tolerance, and pitch fade
    UE_LOG(LogTemp, Warning,
        TEXT("AIM DEBUG | Angle: %.1f deg | Effectiveness: %.2f | NozzleFwd: %s | ToFire: %s | HeightOffset: %.1f cm | PitchTol: %.1f deg | PitchFade: %.2f"),
        AngleDeg, Effectiveness, *NozzleForward.ToString(), *DirectionToTarget.ToString(),
        FireAimHeightOffset, PitchToleranceAngle, PitchFade);

    // Extended AIM2 diagnostic line
    UE_LOG(LogTemp, Warning,
        TEXT("AIM2 | Dist: %.1f cm | FwdZ: %.3f | Pitch: %.1f deg | TargetPitch: %.1f deg | PitchError: %.1f deg | PitchTol: %.1f deg | PitchFade: %.2f | YawError: %.1f deg | YawTol: %.1f deg | YawFade: %.2f | HeightOffset: %.1f cm | IgnorePitch: %d | Effectiveness: %.2f"),
        Distance, NozzleForward.Z, Pitch, TargetPitch, PitchError, PitchToleranceAngle, PitchFade,
        YawError, AimToleranceAngle, YawFade, FireAimHeightOffset, bIgnorePitch ? 1 : 0, Effectiveness);

    if (UWorld* World = GetWorld())
    {
        const FColor LineColor = FColor::MakeRedToGreenColorFromScalar(Effectiveness);
        DrawDebugLine(World, GetActorLocation(), TargetLocation, LineColor, false, -1.f, 0, 2.f);
    }
}
