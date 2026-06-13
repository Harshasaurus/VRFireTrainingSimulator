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

    // MeshComponent and GrabSphere are already created and configured
    // by AVRGrabbable's constructor.

    // Spray particle
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
    {
        SprayTick(DeltaTime);
    }
}

void AVRFireExtinguisher::Grab(USceneComponent* AttachTo)
{
    Super::Grab(AttachTo);
    UE_LOG(LogTemp, Warning, TEXT("Extinguisher grabbed!"));
}

void AVRFireExtinguisher::Release(FVector ThrowVelocity)
{
    bIsSpraying = false;
    SprayParticle->Deactivate();

    Super::Release(ThrowVelocity);

    UE_LOG(LogTemp, Warning, TEXT("Extinguisher released!"));
}

void AVRFireExtinguisher::StartSpray()
{
    UE_LOG(LogTemp, Warning, TEXT("StartSpray called! bIsSpraying: %d"), bIsSpraying);

    bIsSpraying = true;
    SprayParticle->Activate();
    UE_LOG(LogTemp, Warning, TEXT("Spraying!"));
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
            GetActorLocation(),
            Fire->GetActorLocation()
        );

        UE_LOG(LogTemp, Warning, TEXT("Distance to fire: %f"), Distance);

        // If within spray range, apply extinguisher
        if (Distance <= SprayRange)
        {
            Fire->ApplyExtinguisher(DeltaTime);
            UE_LOG(LogTemp, Warning, TEXT("Extinguishing! Fire health: %f"),
                Fire->FireHealth);
        }
    }
}