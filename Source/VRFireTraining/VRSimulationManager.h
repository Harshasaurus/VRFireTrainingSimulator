#pragma once
#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "VRSimulationManager.generated.h"

// Broadcast when simulation ends with final score data
DECLARE_DYNAMIC_MULTICAST_DELEGATE_FourParams(FOnSimulationComplete,
    float, AlertResponseTime,       // seconds from fire start to buzzer press
    float, ExtinguishTime,          // seconds from buzzer press to all fires out
    float, TotalTime,               // total elapsed seconds
    int32, FinalScore               // 0-100
);

// Broadcast when simulation phase changes (for HUD updates)
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnPhaseChanged,
    int32, NewPhase                 // 0=Idle, 1=FireActive, 2=Alerted, 3=Complete
);

UCLASS()
class VRFIRETRAINING_API AVRSimulationManager : public AActor
{
    GENERATED_BODY()

public:
    AVRSimulationManager();

    // ----------------------------------------------------------------
    // Phase tracking
    // ----------------------------------------------------------------

    UPROPERTY(BlueprintReadOnly, Category = "Simulation")
    int32 CurrentPhase = 0;
    // 0 = Idle       (waiting to start)
    // 1 = FireActive (fire burning, player hasn't hit buzzer yet)
    // 2 = Alerted    (buzzer pressed, extinguishing in progress)
    // 3 = Complete   (all fires out)

    // ----------------------------------------------------------------
    // Timestamps  (world time in seconds, set by GetWorld()->GetTimeSeconds())
    // ----------------------------------------------------------------

    UPROPERTY(BlueprintReadOnly, Category = "Simulation|Timestamps")
    float FireStartTime = 0.f;

    UPROPERTY(BlueprintReadOnly, Category = "Simulation|Timestamps")
    float BuzzerPressedTime = 0.f;

    UPROPERTY(BlueprintReadOnly, Category = "Simulation|Timestamps")
    float FireExtinguishedTime = 0.f;

    // ----------------------------------------------------------------
    // Scoring weights  (editable in editor so you can tune without recompile)
    // ----------------------------------------------------------------

    // Max score points awarded for alert speed  (out of 100)
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Simulation|Scoring")
    float AlertScoreWeight = 40.f;

    // Max score points awarded for extinguish speed  (out of 100)
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Simulation|Scoring")
    float ExtinguishScoreWeight = 60.f;

    // If player alerts within this many seconds they get full alert score
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Simulation|Scoring")
    float IdealAlertTime = 10.f;

    // If player extinguishes within this many seconds they get full extinguish score
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Simulation|Scoring")
    float IdealExtinguishTime = 20.f;

    // ----------------------------------------------------------------
    // Delegates
    // ----------------------------------------------------------------

    UPROPERTY(BlueprintAssignable, Category = "Simulation")
    FOnSimulationComplete OnSimulationComplete;

    UPROPERTY(BlueprintAssignable, Category = "Simulation")
    FOnPhaseChanged OnPhaseChanged;

    // ----------------------------------------------------------------
    // Public API  (called by Buzzer, Fire, or Blueprint)
    // ----------------------------------------------------------------

    // Call once to start the scenario (fire is now burning)
    UFUNCTION(BlueprintCallable, Category = "Simulation")
    void StartSimulation();

    // Called by AVRBuzzer when player presses the alarm
    UFUNCTION(BlueprintCallable, Category = "Simulation")
    void OnBuzzerPressed();

    // Called by AVRFire when all fires are extinguished
    UFUNCTION(BlueprintCallable, Category = "Simulation")
    void OnAllFiresExtinguished();

    // Resets everything back to Idle so the scenario can be replayed
    UFUNCTION(BlueprintCallable, Category = "Simulation")
    void ResetSimulation();

    // Read-only helpers for HUD
    UFUNCTION(BlueprintCallable, Category = "Simulation")
    float GetElapsedSinceFireStart() const;

    UFUNCTION(BlueprintCallable, Category = "Simulation")
    float GetElapsedSinceBuzzer() const;

    UFUNCTION(BlueprintCallable, Category = "Simulation")
    bool IsSimulationActive() const { return CurrentPhase == 1 || CurrentPhase == 2; }

protected:
    virtual void BeginPlay() override;

public:
    virtual void Tick(float DeltaTime) override;

private:
    // Computes 0-100 score from timestamps
    int32 CalculateScore(float AlertTime, float ExtinguishTime) const;

    // Moves to a new phase and broadcasts OnPhaseChanged
    void SetPhase(int32 NewPhase);
};