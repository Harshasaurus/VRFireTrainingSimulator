#include "VRTimerWidget.h"
#include "Components/TextBlock.h"
#include "Kismet/GameplayStatics.h"

void UVRTimerWidget::NativeConstruct()
{
    Super::NativeConstruct();

    // Hidden until fire spawns
    SetVisibility(ESlateVisibility::Hidden);

    RegisterWithSimulationManager();
}

void UVRTimerWidget::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
    Super::NativeTick(MyGeometry, InDeltaTime);

    if (bShouldTick)
        UpdateTimerDisplay();
}

// ----------------------------------------------------------------
// Setup
// ----------------------------------------------------------------

void UVRTimerWidget::RegisterWithSimulationManager()
{
    TArray<AActor*> Found;
    UGameplayStatics::GetAllActorsOfClass(
        GetWorld(), AVRSimulationManager::StaticClass(), Found);

    if (Found.Num() == 0)
    {
        UE_LOG(LogTemp, Error, TEXT("VRTimerWidget: No SimulationManager found!"));
        return;
    }

    SimulationManager = Cast<AVRSimulationManager>(Found[0]);
    if (!SimulationManager) return;

    // Show widget when fire spawns
    SimulationManager->OnFireSpawned.AddDynamic(
        this, &UVRTimerWidget::HandleFireSpawned);

    // Update phase label when player hits buzzer
    SimulationManager->OnPhaseChanged.AddDynamic(
        this, &UVRTimerWidget::HandlePhaseChanged);

    // Hide timer when score screen appears
    SimulationManager->OnSimulationComplete.AddDynamic(
        this, &UVRTimerWidget::HandleSimulationComplete);

    UE_LOG(LogTemp, Warning, TEXT("VRTimerWidget: Bound to SimulationManager."));
}

// ----------------------------------------------------------------
// Delegate handlers
// ----------------------------------------------------------------

void UVRTimerWidget::HandleFireSpawned()
{
    // Fire just spawned — show widget and start ticking
    SetVisibility(ESlateVisibility::Visible);
    bShouldTick = true;

    if (PhaseText)
        PhaseText->SetText(
            FText::FromString(TEXT("FIRE DETECTED PRESS B near THE BUZZER!")));

    UE_LOG(LogTemp, Warning, TEXT("VRTimerWidget: Visible — timer started."));
}

void UVRTimerWidget::HandlePhaseChanged(int32 NewPhase)
{
    if (!PhaseText) return;

    switch (NewPhase)
    {
    case 2: // Alerted — buzzer pressed
        PhaseText->SetText(
            FText::FromString(TEXT("ALARM TRIGGERED — EXTINGUISH THE FIRE!")));
        break;

    case 3: // Complete
        bShouldTick = false;
        break;

    default:
        break;
    }
}

void UVRTimerWidget::HandleSimulationComplete(float AlertTime,
    float ExtinguishTime, float TotalTime, int32 FinalScore)
{
    // Hide timer widget — score widget takes over
    bShouldTick = false;
    SetVisibility(ESlateVisibility::Hidden);

    UE_LOG(LogTemp, Warning, TEXT("VRTimerWidget: Hidden — score widget active."));
}

// ----------------------------------------------------------------
// Private
// ----------------------------------------------------------------

void UVRTimerWidget::UpdateTimerDisplay()
{
    if (!SimulationManager || !TimerText) return;

    float Elapsed = SimulationManager->GetElapsedSinceFireStart();

    TimerText->SetText(
        FText::FromString(FString::Printf(TEXT("Time: %s"), *FormatTime(Elapsed))));
}

FString UVRTimerWidget::FormatTime(float Seconds) const
{
    int32 Mins = FMath::FloorToInt(Seconds / 60.f);
    int32 Secs = FMath::FloorToInt(Seconds) % 60;
    return FString::Printf(TEXT("%02d:%02d"), Mins, Secs);
}