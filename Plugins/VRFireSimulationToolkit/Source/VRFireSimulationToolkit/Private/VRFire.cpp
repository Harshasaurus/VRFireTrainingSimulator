#include "VRFire.h"
#include "VRSimulationManager.h"
#include "Particles/ParticleSystemComponent.h"
#include "Kismet/GameplayStatics.h"
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
    FireParticle = CreateDefaultSubobject<UParticleSystemComponent>(TEXT("FireParticle"));
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
    FireParticle->Deactivate();
    FireParticle->SetVisibility(false);
    UE_LOG(LogTemp, Warning, TEXT("Fire Extinguished!"));

    // Broadcast per-fire delegate - host project (instruction system, HUD, etc.)
    // binds to this to react to an individual fire going out.
    OnFireExtinguished.Broadcast();

    // Check if ALL fires in the level are out
    TArray<AActor*> AllFires;
    UGameplayStatics::GetAllActorsOfClass(GetWorld(), AVRFire::StaticClass(), AllFires);

    bool bAllExtinguished = true;
    for (AActor* Actor : AllFires)
    {
        AVRFire* Fire = Cast<AVRFire>(Actor);
        if (Fire && !Fire->bIsExtinguished)
        {
            bAllExtinguished = false;
            break;
        }
    }

    if (bAllExtinguished)
    {
        UE_LOG(LogTemp, Warning, TEXT("All fires out! Notifying SimulationManager."));

        // Notify SimulationManager for scoring/phase progression.
        // The host project's instruction system should bind to
        // AVRSimulationManager::OnSimulationComplete instead of this class
        // knowing about any specific instruction/tutorial system.
        FindSimulationManager();
        if (SimulationManager)
            SimulationManager->OnAllFiresExtinguished();
    }
}

void AVRFire::FindSimulationManager()
{
    if (SimulationManager) return;

    TArray<AActor*> Found;
    UGameplayStatics::GetAllActorsOfClass(
        GetWorld(), AVRSimulationManager::StaticClass(), Found);

    if (Found.Num() > 0)
        SimulationManager = Cast<AVRSimulationManager>(Found[0]);

    // Not logging an error here - SimulationManager is optional.
    if (SimulationManager)
        UE_LOG(LogTemp, Warning, TEXT("VRFire: SimulationManager found and cached."));
}
