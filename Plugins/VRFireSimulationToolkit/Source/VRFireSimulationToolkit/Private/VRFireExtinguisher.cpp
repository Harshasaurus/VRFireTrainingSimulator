#include "VRFireExtinguisher.h"
#include "VRFire.h"
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

    for (AActor* Actor : FireActors)
    {
        AVRFire* Fire = Cast<AVRFire>(Actor);
        if (!Fire) continue;

        float Distance = FVector::Dist(
            GetActorLocation(), Fire->GetActorLocation());

        if (Distance <= SprayRange)
        {
            Fire->ApplyExtinguisher(DeltaTime);
        }
    }
}
