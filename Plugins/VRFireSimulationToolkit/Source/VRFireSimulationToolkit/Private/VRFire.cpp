#include "VRFire.h"
#include "VRSimulationManager.h"
#include "Particles/ParticleSystemComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Components/SphereComponent.h"

AVRFire::AVRFire()
{
    PrimaryActorTick.bCanEverTick = true;

    FireCollision = CreateDefaultSubobject<USphereComponent>(TEXT("FireCollision"));
    RootComponent = FireCollision;
    FireCollision->SetSphereRadius(50.0f);
    FireCollision->SetCollisionProfileName(TEXT("OverlapAll"));

    FireParticle = CreateDefaultSubobject<UParticleSystemComponent>(TEXT("FireParticle"));
    FireParticle->SetupAttachment(RootComponent);
}

void AVRFire::BeginPlay()
{
    Super::BeginPlay();

    if (bCanSpread && SpreadGeneration < MaxSpreadGenerations)
    {
        GetWorldTimerManager().SetTimer(
            SpreadTimerHandle,
            this,
            &AVRFire::TrySpread,
            SpreadInterval,
            false
        );
    }
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

    GetWorldTimerManager().ClearTimer(SpreadTimerHandle);

    OnFireExtinguished.Broadcast();

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
        UE_LOG(LogTemp, Warning, TEXT("All fires out!"));

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

    if (SimulationManager)
        UE_LOG(LogTemp, Warning, TEXT("VRFire: SimulationManager found and cached."));
}

void AVRFire::TrySpread()
{
    if (bIsExtinguished) return;

    UWorld* World = GetWorld();
    if (!World) return;

    // NEW: pick a random direction, then a distance between
    // MinSpreadDistance and MaxSpreadDistance — keeps the new fire
    // close and adjacent instead of anywhere in a wide circle
    float RandomAngle = FMath::RandRange(0.f, 2.f * PI);
    float RandomDistance = FMath::RandRange(MinSpreadDistance, MaxSpreadDistance);

    FVector Offset(
        FMath::Cos(RandomAngle) * RandomDistance,
        FMath::Sin(RandomAngle) * RandomDistance,
        0.f
    );

    FVector SpawnLocation = GetActorLocation() + Offset;

    FActorSpawnParameters Params;
    Params.SpawnCollisionHandlingOverride =
        ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

    // Spawn using GetClass() (this actor's actual class — e.g. BP_VRFire)
    // instead of AVRFire::StaticClass() (the raw C++ base). This keeps
    // whatever particle system, sound, and mesh you set in the Blueprint,
    // so spread fires actually render instead of being invisible.
    AVRFire* NewFire = World->SpawnActor<AVRFire>(
        GetClass(), SpawnLocation, GetActorRotation(), Params);

    if (NewFire)
    {
        NewFire->SpreadGeneration = SpreadGeneration + 1;
        UE_LOG(LogTemp, Warning,
            TEXT("VRFire: %s spread! New fire at generation %d, %.0f units away."),
            *GetName(), NewFire->SpreadGeneration, RandomDistance);
    }
}