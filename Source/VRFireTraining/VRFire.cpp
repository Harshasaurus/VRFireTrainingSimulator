#include "VRFire.h"
#include "Particles/ParticleSystemComponent.h"
#include "Components/SphereComponent.h"

AVRFire::AVRFire()
{
    PrimaryActorTick.bCanEverTick = true;

    // Collision root
    FireCollision = CreateDefaultSubobject<USphereComponent>(TEXT("FireCollision"));
    RootComponent = FireCollision;
    FireCollision->SetSphereRadius(50.0f);
    FireCollision->SetCollisionProfileName(TEXT("OverlapAll"));

    // Fire particle
    FireParticle = CreateDefaultSubobject<UParticleSystemComponent>(
        TEXT("FireParticle"));
    FireParticle->SetupAttachment(RootComponent);
}

void AVRFire::BeginPlay()
{
    Super::BeginPlay();
}

void AVRFire::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);
}

void AVRFire::ApplyExtinguisher(float DeltaTime)
{
    if (bIsExtinguished) return;

    FireHealth -= ExtinguishRate * DeltaTime;
    FireHealth = FMath::Clamp(FireHealth, 0.0f, 100.0f);

    UE_LOG(LogTemp, Warning, TEXT("Fire health: %.1f"), FireHealth);

    // Scale particle down as fire dies
    float HealthPercent = FireHealth / 100.0f;
    FireParticle->SetWorldScale3D(FVector(HealthPercent));

    if (FireHealth <= 0.0f)
    {
        Extinguish();
    }
}

void AVRFire::Extinguish()
{
    bIsExtinguished = true;

    // Hide fire
    FireParticle->Deactivate();
    FireParticle->SetVisibility(false);

    OnFireExtinguished.Broadcast();

    UE_LOG(LogTemp, Warning, TEXT("Fire Extinguished! Great job!"));
}