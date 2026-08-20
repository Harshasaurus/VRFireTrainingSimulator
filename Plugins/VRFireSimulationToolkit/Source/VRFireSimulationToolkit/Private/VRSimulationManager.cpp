#include "VRSimulationManager.h"
#include "VRFire.h"
#include "Kismet/GameplayStatics.h"
#include "Engine/World.h"

AVRSimulationManager::AVRSimulationManager()
{
    PrimaryActorTick.bCanEverTick = false;
}

void AVRSimulationManager::BeginPlay()
{
    Super::BeginPlay();
    // Call StartSimulation() from Level Blueprint on BeginPlay
}

void AVRSimulationManager::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);
}

// ----------------------------------------------------------------
// Public API
// ----------------------------------------------------------------

void AVRSimulationManager::StartSimulation()
{
    if (CurrentPhase != 0)
    {
        UE_LOG(LogTemp, Warning,
            TEXT("SimulationManager: Already running. Ignoring StartSimulation."));
        return;
    }

    if (FireSpawnPoints.Num() == 0)
    {
        UE_LOG(LogTemp, Error,
            TEXT("SimulationManager: No FireSpawnPoints assigned! Add them in the Details panel."));
        return;
    }

    if (!FireClass)
    {
        UE_LOG(LogTemp, Error,
            TEXT("SimulationManager: No FireClass assigned! Drag your BP_VRFire into the Details panel."));
        return;
    }

    UE_LOG(LogTemp, Warning,
        TEXT("SimulationManager: Grace period started. Fire spawns in %.1f seconds."),
        FireSpawnDelay);

    // Start countdown — fire spawns after FireSpawnDelay seconds
    GetWorld()->GetTimerManager().SetTimer(
        FireSpawnTimerHandle,
        this,
        &AVRSimulationManager::SpawnFireAndBegin,
        FireSpawnDelay,
        false  // don't loop
    );
}

void AVRSimulationManager::OnBuzzerPressed()
{
    if (CurrentPhase != 1)
    {
        UE_LOG(LogTemp, Warning,
            TEXT("SimulationManager: OnBuzzerPressed called outside FireActive phase. Ignoring."));
        return;
    }

    BuzzerPressedTime = GetWorld()->GetTimeSeconds();
    SetPhase(2); // Alerted

    UE_LOG(LogTemp, Warning,
        TEXT("SimulationManager: Buzzer pressed at %.2f  (%.2f s after fire start)"),
        BuzzerPressedTime, BuzzerPressedTime - FireStartTime);
}

void AVRSimulationManager::OnAllFiresExtinguished()
{
    if (CurrentPhase != 1 && CurrentPhase != 2)
    {
        UE_LOG(LogTemp, Warning,
            TEXT("SimulationManager: OnAllFiresExtinguished called in wrong phase. Ignoring."));
        return;
    }

    // If buzzer was never pressed, stamp it now so score is penalised but game ends cleanly
    if (BuzzerPressedTime <= 0.f)
    {
        BuzzerPressedTime = GetWorld()->GetTimeSeconds();
        UE_LOG(LogTemp, Warning,
            TEXT("SimulationManager: Buzzer never pressed — stamping at extinguish time."));
    }

    FireExtinguishedTime = GetWorld()->GetTimeSeconds();

    float AlertTime = BuzzerPressedTime - FireStartTime;
    float ExtinguishTime = FireExtinguishedTime - BuzzerPressedTime;
    float TotalTime = FireExtinguishedTime - FireStartTime;
    int32 Score = CalculateScore(AlertTime, ExtinguishTime);

    SetPhase(3); // Complete

    UE_LOG(LogTemp, Warning,
        TEXT("SimulationManager: Complete! Alert=%.1fs  Extinguish=%.1fs  Total=%.1fs  Score=%d"),
        AlertTime, ExtinguishTime, TotalTime, Score);

    OnSimulationComplete.Broadcast(AlertTime, ExtinguishTime, TotalTime, Score);
}

void AVRSimulationManager::ResetSimulation()
{
    // Cancel spawn timer if still counting down
    GetWorld()->GetTimerManager().ClearTimer(FireSpawnTimerHandle);

    // Destroy any spawned fires
    for (AActor* Fire : SpawnedFires)
    {
        if (Fire) Fire->Destroy();
    }
    SpawnedFires.Empty();

    FireStartTime = 0.f;
    BuzzerPressedTime = 0.f;
    FireExtinguishedTime = 0.f;

    SetPhase(0);

    UE_LOG(LogTemp, Warning, TEXT("SimulationManager: Reset to Idle."));
}

// ----------------------------------------------------------------
// HUD helpers
// ----------------------------------------------------------------

float AVRSimulationManager::GetElapsedSinceFireStart() const
{
    if (CurrentPhase < 1) return 0.f;
    return GetWorld()->GetTimeSeconds() - FireStartTime;
}

float AVRSimulationManager::GetElapsedSinceBuzzer() const
{
    if (CurrentPhase < 2 || BuzzerPressedTime <= 0.f) return 0.f;
    return GetWorld()->GetTimeSeconds() - BuzzerPressedTime;
}

// ----------------------------------------------------------------
// Private
// ----------------------------------------------------------------

void AVRSimulationManager::SpawnFireAndBegin()
{
    UWorld* World = GetWorld();
    if (!World) return;

    // Spawn a fire actor at every assigned spawn point
    for (AActor* SpawnPoint : FireSpawnPoints)
    {
        if (!SpawnPoint) continue;

        FActorSpawnParameters Params;
        Params.SpawnCollisionHandlingOverride =
            ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

        AActor* Fire = World->SpawnActor<AActor>(
            FireClass,
            SpawnPoint->GetActorLocation(),
            SpawnPoint->GetActorRotation(),
            Params
        );

        if (Fire)
        {
            SpawnedFires.Add(Fire);
            UE_LOG(LogTemp, Warning,
                TEXT("SimulationManager: Fire spawned at %s"), *SpawnPoint->GetName());
        }
    }

    // Record fire start time and move to FireActive phase
    FireStartTime = World->GetTimeSeconds();
    SetPhase(1);

    // Tell timer widget to appear
    OnFireSpawned.Broadcast();

    UE_LOG(LogTemp, Warning,
        TEXT("SimulationManager: Fire is live at %.2f — simulation active!"), FireStartTime);
}

int32 AVRSimulationManager::CalculateScore(float AlertTime, float ExtinguishTime) const
{
    float AlertRatio = FMath::Clamp(
        1.f - (AlertTime / (IdealAlertTime * 2.f)), 0.f, 1.f);
    float ExtRatio = FMath::Clamp(
        1.f - (ExtinguishTime / (IdealExtinguishTime * 2.f)), 0.f, 1.f);

    return FMath::RoundToInt(
        (AlertRatio * AlertScoreWeight) + (ExtRatio * ExtinguishScoreWeight));
}

void AVRSimulationManager::SetPhase(int32 NewPhase)
{
    CurrentPhase = NewPhase;
    OnPhaseChanged.Broadcast(NewPhase);

    static const TCHAR* PhaseNames[] = {
        TEXT("Idle"), TEXT("FireActive"), TEXT("Alerted"), TEXT("Complete")
    };
    if (NewPhase >= 0 && NewPhase <= 3)
        UE_LOG(LogTemp, Warning,
            TEXT("SimulationManager: Phase -> %s"), PhaseNames[NewPhase]);
}