#pragma once
#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "VRSimulationManager.h"
#include "VRTimerWidget.generated.h"

UCLASS()
class VRFIRETRAINING_API UVRTimerWidget : public UUserWidget
{
    GENERATED_BODY()

public:

    // Name this exactly "TimerText" in UMG designer
    // Shows "Time: 00:14"
    UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
    class UTextBlock* TimerText;

    // Name this exactly "PhaseText" in UMG designer
    // Shows "FIRE DETECTED — PRESS BUZZER!" or "EXTINGUISHING..."
    UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
    class UTextBlock* PhaseText;

    // Auto-binds to SimulationManager delegates
    UFUNCTION(BlueprintCallable, Category = "TimerHUD")
    void RegisterWithSimulationManager();

protected:
    virtual void NativeConstruct() override;
    virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;

private:
    AVRSimulationManager* SimulationManager = nullptr;

    // Delegate handlers
    UFUNCTION()
    void HandleFireSpawned();

    UFUNCTION()
    void HandlePhaseChanged(int32 NewPhase);

    UFUNCTION()
    void HandleSimulationComplete(float AlertTime, float ExtinguishTime,
        float TotalTime, int32 FinalScore);

    // Updates TimerText every tick while simulation is active
    void UpdateTimerDisplay();

    // Formats seconds into "01:23"
    FString FormatTime(float Seconds) const;

    bool bShouldTick = false;
};