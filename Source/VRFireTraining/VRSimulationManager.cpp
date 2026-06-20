#include "VRSimulationManager.h"
#include "Kismet/GameplayStatics.h"

AVRSimulationManager::AVRSimulationManager()
{
    PrimaryActorTick.bCanEverTick = false; // no per-frame work needed
}

void AVRSimulationManager::BeginPlay()
{
    Super::BeginPlay();
    // Start idle — call StartSimulation() from Blueprint or level BP
    // when you're ready to kick off the scenario
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
            TEXT("SimulationManager: StartSimulation called but not in Idle phase. Ignoring."));
        return;
    }

    FireStartTime = GetWorld()->GetTimeSeconds();
    BuzzerPressedTime = 0.f;
    FireExtinguishedTime = 0.f;

    SetPhase(1); // FireActive

    UE_LOG(LogTemp, Warning,
        TEXT("SimulationManager: Simulation started at %.2f"), FireStartTime);
}

void AVRSimulationManager::OnBuzzerPressed()
{
    // Guard: only valid during FireActive phase
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
        BuzzerPressedTime,
        BuzzerPressedTime - FireStartTime);
}

void AVRSimulationManager::OnAllFiresExtinguished()
{
    // Guard: only valid during FireActive or Alerted phase
    // (edge case: player extinguishes before hitting buzzer — still record it)
    if (CurrentPhase != 1 && CurrentPhase != 2)
    {
        UE_LOG(LogTemp, Warning,
            TEXT("SimulationManager: OnAllFiresExtinguished called in wrong phase. Ignoring."));
        return;
    }

    // If player never hit the buzzer, record buzzer time = extinguish time
    // so alert score is 0 (worst penalty) but game still ends cleanly
    if (BuzzerPressedTime <= 0.f)
    {
        BuzzerPressedTime = GetWorld()->GetTimeSeconds();
        UE_LOG(LogTemp, Warning,
            TEXT("SimulationManager: Buzzer was never pressed — marking at extinguish time."));
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
    FireStartTime = 0.f;
    BuzzerPressedTime = 0.f;
    FireExtinguishedTime = 0.f;

    SetPhase(0); // Back to Idle

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


// Private helpers


int32 AVRSimulationManager::CalculateScore(float AlertTime, float ExtinguishTime) const
{
    // Alert score: full marks if <= IdealAlertTime, drops linearly to 0 at 2x IdealAlertTime
    float AlertRatio = FMath::Clamp(1.f - (AlertTime / (IdealAlertTime * 2.f)), 0.f, 1.f);
    float ExtRatio = FMath::Clamp(1.f - (ExtinguishTime / (IdealExtinguishTime * 2.f)), 0.f, 1.f);

    float RawScore = (AlertRatio * AlertScoreWeight) + (ExtRatio * ExtinguishScoreWeight);
    return FMath::RoundToInt(RawScore);
}

void AVRSimulationManager::SetPhase(int32 NewPhase)
{
    CurrentPhase = NewPhase;
    OnPhaseChanged.Broadcast(NewPhase);

    static const TCHAR* PhaseNames[] = {
        TEXT("Idle"), TEXT("FireActive"), TEXT("Alerted"), TEXT("Complete")
    };
    if (NewPhase >= 0 && NewPhase <= 3)
    {
        UE_LOG(LogTemp, Warning,
            TEXT("SimulationManager: Phase -> %s"), PhaseNames[NewPhase]);
    }
}