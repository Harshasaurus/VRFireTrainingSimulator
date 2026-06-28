#pragma once
#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "VRWorkerNPC.generated.h"

UENUM(BlueprintType)
enum class EWorkerState : uint8
{
    Idle        UMETA(DisplayName = "Idle"),
    Panicking   UMETA(DisplayName = "Panicking"),
    Evacuated   UMETA(DisplayName = "Evacuated")
};

UCLASS()
class VRFIRETRAINING_API AVRWorkerNPC : public ACharacter
{
    GENERATED_BODY()

public:
    AVRWorkerNPC();

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "NPC")
    AActor* ExitPoint = nullptr;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "NPC")
    float IdleWalkSpeed = 100.f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "NPC")
    float PanicRunSpeed = 400.f;

    // How far from current position the NPC can wander (cm)
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "NPC")
    float WanderRadius = 500.f;

    // How long NPC waits at each wander point before moving again (seconds)
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "NPC")
    float WanderWaitTime = 2.f;

    UPROPERTY(BlueprintReadOnly, Category = "NPC")
    EWorkerState CurrentState = EWorkerState::Idle;

    UFUNCTION(BlueprintCallable, Category = "NPC")
    void OnAlarmTriggered();

    UFUNCTION(BlueprintCallable, Category = "NPC")
    void ResetNPC();

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
    void CheckIfReachedExit();

    // Wander logic
    void StartWander();
    void WanderTick(float DeltaTime);
    bool bIsWaiting = false;
    float WanderWaitTimer = 0.f;

    float AcceptanceRadius = 100.f;
    FTransform SpawnTransform;
    FVector SpawnLocation;
};