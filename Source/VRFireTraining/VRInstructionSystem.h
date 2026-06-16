#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "VRInstructionSystem.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FOnInstructionChanged,
    const FString&, NewInstruction, int32, StepNumber, int32, TotalSteps);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnTrainingComplete);

UCLASS()
class VRFIRETRAINING_API AVRInstructionSystem : public AActor
{
    GENERATED_BODY()

public:
    AVRInstructionSystem();

    // List of instructions in order
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Instructions")
    TArray<FString> Instructions;

    // Current instruction index
    UPROPERTY(BlueprintReadOnly, Category = "Instructions")
    int32 CurrentInstructionIndex = 0;

    // Is training complete?
    UPROPERTY(BlueprintReadOnly, Category = "Instructions")
    bool bTrainingComplete = false;

    // Get current instruction text
    UFUNCTION(BlueprintCallable, Category = "Instructions")
    FString GetCurrentInstruction();

    // Move to next instruction
    UFUNCTION(BlueprintCallable, Category = "Instructions")
    void NextInstruction();

    // Called when instruction changes
    UPROPERTY(BlueprintAssignable, Category = "Instructions")
    FOnInstructionChanged OnInstructionChanged;

    // Called when all instructions complete
    UPROPERTY(BlueprintAssignable, Category = "Instructions")
    FOnTrainingComplete OnTrainingComplete;

    // Start the training
    UFUNCTION(BlueprintCallable, Category = "Instructions")
    void StartTraining();

protected:
    virtual void BeginPlay() override;

public:
    virtual void Tick(float DeltaTime) override;
};