#pragma once
#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "VRSimulationManager.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_FourParams(FOnSimulationComplete,
    float, AlertResponseTime,
    float, ExtinguishTime,
    float, TotalTime,
    int32, FinalScore
);

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnPhaseChanged,
    int32, NewPhase
);

// Broadcast when fire spawns — timer widget listens to this
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnFireSpawned);

UCLASS()
class VRFIRETRAINING_API AVRSimulationManager : public AActor
{
    GENERATED_BODY()

public:
    AVRSimulationManager();

    // ----------------------------------------------------------------
    // Phase tracking
    // 0=Idle  1=FireActive  2=Alerted  3=Complete
    // ----------------------------------------------------------------

    UPROPERTY(BlueprintReadOnly, Category = "Simulation")
    int32 CurrentPhase = 0;

    // ----------------------------------------------------------------
    // Timestamps
    // ----------------------------------------------------------------

    UPROPERTY(BlueprintReadOnly, Category = "Simulation|Timestamps")
    float FireStartTime = 0.f;

    UPROPERTY(BlueprintReadOnly, Category = "Simulation|Timestamps")
    float BuzzerPressedTime = 0.f;

    UPROPERTY(BlueprintReadOnly, Category = "Simulation|Timestamps")
    float FireExtinguishedTime = 0.f;

    // ----------------------------------------------------------------
    // Fire spawn config
    // ----------------------------------------------------------------

    // Drag your AVRFire Blueprint class here in the editor
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Simulation|Fire")
    TSubclassOf<AActor> FireClass;

    // Place Empty Actors in the level, drag them all into this array
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Simulation|Fire")
    TArray<AActor*> FireSpawnPoints;

    // Seconds after game start before fire spawns
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Simulation|Fire")
    float FireSpawnDelay = 12.f;

    // ----------------------------------------------------------------
    // Scoring weights
    // ----------------------------------------------------------------

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Simulation|Scoring")
    float AlertScoreWeight = 40.f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Simulation|Scoring")
    float ExtinguishScoreWeight = 60.f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Simulation|Scoring")
    float IdealAlertTime = 10.f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Simulation|Scoring")
    float IdealExtinguishTime = 20.f;

    // ----------------------------------------------------------------
    // Delegates
    // ----------------------------------------------------------------

    UPROPERTY(BlueprintAssignable, Category = "Simulation")
    FOnSimulationComplete OnSimulationComplete;

    UPROPERTY(BlueprintAssignable, Category = "Simulation")
    FOnPhaseChanged OnPhaseChanged;

    // Timer widget binds to this to know when to appear
    UPROPERTY(BlueprintAssignable, Category = "Simulation")
    FOnFireSpawned OnFireSpawned;

    // ----------------------------------------------------------------
    // Public API
    // ----------------------------------------------------------------

    // Call from Level Blueprint on BeginPlay — starts the grace period countdown
    UFUNCTION(BlueprintCallable, Category = "Simulation")
    void StartSimulation();

    // Called by AVRBuzzer
    UFUNCTION(BlueprintCallable, Category = "Simulation")
    void OnBuzzerPressed();

    // Called by AVRFire when all fires out
    UFUNCTION(BlueprintCallable, Category = "Simulation")
    void OnAllFiresExtinguished();

    // Resets for replay
    UFUNCTION(BlueprintCallable, Category = "Simulation")
    void ResetSimulation();

    // HUD helpers
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
    // Called by timer after FireSpawnDelay seconds
    void SpawnFireAndBegin();

    int32 CalculateScore(float AlertTime, float ExtinguishTime) const;
    void SetPhase(int32 NewPhase);

    // Spawned fire actors — kept so we can destroy on reset
    TArray<AActor*> SpawnedFires;

    FTimerHandle FireSpawnTimerHandle;
};