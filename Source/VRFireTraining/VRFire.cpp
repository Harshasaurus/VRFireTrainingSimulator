#include "VRFire.h"
#include "VRSimulationManager.h"          // NEW
#include "Particles/ParticleSystemComponent.h"
#include "Kismet/GameplayStatics.h"
#include "VRInstructionSystem.h"
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

// ----------------------------------------------------------------
// Unchanged from original
// ----------------------------------------------------------------

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

// ----------------------------------------------------------------
// Modified: Extinguish() keeps original InstructionSystem call
//           and ADDS SimulationManager notification
// ----------------------------------------------------------------

void AVRFire::Extinguish()
{
    bIsExtinguished = true;
    FireParticle->Deactivate();
    FireParticle->SetVisibility(false);
    UE_LOG(LogTemp, Warning, TEXT("Fire Extinguished!"));

    // Broadcast per-fire delegate (unchanged)
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
        UE_LOG(LogTemp, Warning, TEXT("All fires out! Training Complete!"));

        // ORIGINAL: Keep InstructionSystem call intact so existing Blueprint
        //           HUD widgets that bind OnTrainingComplete still work
        TArray<AActor*> Found;
        UGameplayStatics::GetAllActorsOfClass(
            GetWorld(), AVRInstructionSystem::StaticClass(), Found);
        if (Found.Num() > 0)
        {
            AVRInstructionSystem* IS = Cast<AVRInstructionSystem>(Found[0]);
            if (IS)
                IS->NextInstruction();
        }

        // NEW: Also notify SimulationManager for scoring
        //      Lazy init here — safe even if SimManager isn't in the level
        FindSimulationManager();
        if (SimulationManager)
            SimulationManager->OnAllFiresExtinguished();
    }
}

// ----------------------------------------------------------------
// NEW: lazy init helper (same pattern as rest of codebase)
// ----------------------------------------------------------------

void AVRFire::FindSimulationManager()
{
    if (SimulationManager) return;

    TArray<AActor*> Found;
    UGameplayStatics::GetAllActorsOfClass(
        GetWorld(), AVRSimulationManager::StaticClass(), Found);

    if (Found.Num() > 0)
        SimulationManager = Cast<AVRSimulationManager>(Found[0]);

    // Not logging an error here — SimulationManager is optional.
    // If it's not in the level, the old InstructionSystem flow still works fine.
    if (SimulationManager)
        UE_LOG(LogTemp, Warning, TEXT("VRFire: SimulationManager found and cached."));
}