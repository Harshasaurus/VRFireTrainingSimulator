#pragma once
#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "VRSimulationManager.h"
#include "VRScoreWidget.generated.h"

UCLASS()
class VRFIRETRAINING_API UVRScoreWidget : public UUserWidget
{
    GENERATED_BODY()

public:

    // ----------------------------------------------------------------
    // Text blocks — bind these to UMG text elements by name
    // ----------------------------------------------------------------

    // Shows "SIMULATION COMPLETE" or "TRAINING FAILED" etc.
    UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
    class UTextBlock* TitleText;

    // e.g. "Alert Response:   8.3s"
    UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
    class UTextBlock* AlertTimeText;

    // e.g. "Extinguish Time:  14.2s"
    UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
    class UTextBlock* ExtinguishTimeText;

    // e.g. "Total Time:       22.5s"
    UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
    class UTextBlock* TotalTimeText;

    // e.g. "Score:  78 / 100"
    UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
    class UTextBlock* ScoreText;

    // Grade label e.g. "Grade: B+"
    UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
    class UTextBlock* GradeText;

    // ----------------------------------------------------------------
    // API
    // ----------------------------------------------------------------

    // Call this to populate all text fields and make widget visible
    UFUNCTION(BlueprintCallable, Category = "ScoreHUD")
    void ShowScore(float AlertTime, float ExtinguishTime, float TotalTime, int32 Score);

    // Hides the widget (call at simulation start / reset)
    UFUNCTION(BlueprintCallable, Category = "ScoreHUD")
    void HideScore();

    // Binds to SimulationManager->OnSimulationComplete automatically
    UFUNCTION(BlueprintCallable, Category = "ScoreHUD")
    void RegisterWithSimulationManager();

protected:
    virtual void NativeConstruct() override;

private:
    // Delegate handler wired to OnSimulationComplete
    UFUNCTION()
    void HandleSimulationComplete(float AlertTime, float ExtinguishTime,
        float TotalTime, int32 FinalScore);

    // Returns "A", "B+", "C" etc. from 0-100 score
    FString GetGradeString(int32 Score) const;
};