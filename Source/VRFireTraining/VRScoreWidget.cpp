#include "VRScoreWidget.h"
#include "Components/TextBlock.h"
#include "Kismet/GameplayStatics.h"

void UVRScoreWidget::NativeConstruct()
{
    Super::NativeConstruct();

    // Hide by default — only shown when simulation completes
    SetVisibility(ESlateVisibility::Hidden);

    // Auto-bind to SimulationManager if one exists in the level
    RegisterWithSimulationManager();
}

// ----------------------------------------------------------------
// API
// ----------------------------------------------------------------

void UVRScoreWidget::RegisterWithSimulationManager()
{
    TArray<AActor*> Found;
    UGameplayStatics::GetAllActorsOfClass(
        GetWorld(), AVRSimulationManager::StaticClass(), Found);

    if (Found.Num() == 0)
    {
        UE_LOG(LogTemp, Error,
            TEXT("VRScoreWidget: No SimulationManager in level!"));
        return;
    }

    AVRSimulationManager* SimManager = Cast<AVRSimulationManager>(Found[0]);
    if (SimManager)
    {
        // Bind delegate — fires when all fires are out and score is ready
        SimManager->OnSimulationComplete.AddDynamic(
            this, &UVRScoreWidget::HandleSimulationComplete);

        UE_LOG(LogTemp, Warning,
            TEXT("VRScoreWidget: Bound to SimulationManager successfully."));
    }
}

void UVRScoreWidget::ShowScore(float AlertTime, float ExtinguishTime,
    float TotalTime, int32 Score)
{
    // Populate each text block
    if (TitleText)
        TitleText->SetText(FText::FromString(TEXT("SIMULATION COMPLETE")));

    if (AlertTimeText)
        AlertTimeText->SetText(FText::FromString(
            FString::Printf(TEXT("Alert Response:  %.1f s"), AlertTime)));

    if (ExtinguishTimeText)
        ExtinguishTimeText->SetText(FText::FromString(
            FString::Printf(TEXT("Extinguish Time:  %.1f s"), ExtinguishTime)));

    if (TotalTimeText)
        TotalTimeText->SetText(FText::FromString(
            FString::Printf(TEXT("Total Time:  %.1f s"), TotalTime)));

    if (ScoreText)
        ScoreText->SetText(FText::FromString(
            FString::Printf(TEXT("Score:  %d / 100"), Score)));

    if (GradeText)
        GradeText->SetText(FText::FromString(
            FString::Printf(TEXT("Grade:  %s"), *GetGradeString(Score))));

    // Make widget visible
    SetVisibility(ESlateVisibility::Visible);

    UE_LOG(LogTemp, Warning,
        TEXT("VRScoreWidget: Score shown — %d/100  Grade: %s"),
        Score, *GetGradeString(Score));
}

void UVRScoreWidget::HideScore()
{
    SetVisibility(ESlateVisibility::Hidden);
}

// ----------------------------------------------------------------
// Private
// ----------------------------------------------------------------

void UVRScoreWidget::HandleSimulationComplete(float AlertTime,
    float ExtinguishTime, float TotalTime, int32 FinalScore)
{
    ShowScore(AlertTime, ExtinguishTime, TotalTime, FinalScore);
}

FString UVRScoreWidget::GetGradeString(int32 Score) const
{
    if (Score >= 90) return TEXT("S");
    if (Score >= 80) return TEXT("A");
    if (Score >= 70) return TEXT("B+");
    if (Score >= 60) return TEXT("B");
    if (Score >= 50) return TEXT("C");
    if (Score >= 40) return TEXT("D");
    return TEXT("F");
}