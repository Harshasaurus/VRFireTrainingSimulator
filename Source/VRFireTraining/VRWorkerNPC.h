#pragma once
#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "VRWorkerNPC.generated.h"

UENUM(BlueprintType)
enum class EWorkerState : uint8
{
    Idle        UMETA(DisplayName = "Idle"),       // standing around working
    Panicking   UMETA(DisplayName = "Panicking"),  // alarm triggered, running to exit
    Evacuated   UMETA(DisplayName = "Evacuated")   // reached exit point safely
};

UCLASS()
class VRFIRETRAINING_API AVRWorkerNPC : public ACharacter
{
    GENERATED_BODY()

public:
    AVRWorkerNPC();

    // ----------------------------------------------------------------
    // Config  (set in Blueprint or Details panel per NPC)
    // ----------------------------------------------------------------

    // Where this NPC runs to when alarm fires
    // Place an Empty Actor in the level and assign it here
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "NPC")
    AActor* ExitPoint = nullptr;

    // How fast NPC walks when idle (cm/s)
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "NPC")
    float IdleWalkSpeed = 100.f;

    // How fast NPC runs when panicking (cm/s)
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "NPC")
    float PanicRunSpeed = 400.f;

    // ----------------------------------------------------------------
    // State
    // ----------------------------------------------------------------

    UPROPERTY(BlueprintReadOnly, Category = "NPC")
    EWorkerState CurrentState = EWorkerState::Idle;

    // ----------------------------------------------------------------
    // API
    // ----------------------------------------------------------------

    // Called by AVRBuzzer delegate or Blueprint when alarm fires
    UFUNCTION(BlueprintCallable, Category = "NPC")
    void OnAlarmTriggered();

    // Called by SimulationManager->ResetSimulation() for replay
    UFUNCTION(BlueprintCallable, Category = "NPC")
    void ResetNPC();

    // Read by SimulationManager to count safe evacuees
    UFUNCTION(BlueprintCallable, Category = "NPC")
    bool IsEvacuated() const { return CurrentState == EWorkerState::Evacuated; }

protected:
    virtual void BeginPlay() override;

public:
    virtual void Tick(float DeltaTime) override;

private:
    class AAIController* AIController = nullptr;

    void SetState(EWorkerState NewState);
    void MoveToExit();

    // Checks every tick if NPC reached exit
    void CheckIfReachedExit();

    // How close NPC needs to be to ExitPoint to count as evacuated (cm)
    float AcceptanceRadius = 100.f;

    // Starting transform so we can reset position on replay
    FTransform SpawnTransform;
};