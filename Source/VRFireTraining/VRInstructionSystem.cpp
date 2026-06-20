#include "VRInstructionSystem.h"

AVRInstructionSystem::AVRInstructionSystem()
{
    PrimaryActorTick.bCanEverTick = false;

    // Default fire extinguisher training instructions
    Instructions.Add(TEXT("Step 1: Locate and grab the fire extinguisher"));
    Instructions.Add(TEXT("Step 2: Pull the safety pin by pressing P"));
    Instructions.Add(TEXT("Step 3: Squeeze the handle by pressing T to spray"));
    Instructions.Add(TEXT("Step 4: Spray until the fire is totally gone"));
    
}

void AVRInstructionSystem::BeginPlay()
{
    Super::BeginPlay();
    StartTraining();
}

void AVRInstructionSystem::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);
}

void AVRInstructionSystem::StartTraining()
{
    CurrentInstructionIndex = 0;
    bTrainingComplete = false;

    if (Instructions.Num() > 0)
    {
        OnInstructionChanged.Broadcast(Instructions[0], CurrentInstructionIndex + 1, Instructions.Num());
        UE_LOG(LogTemp, Warning, TEXT("Training Started: %s"),
            *Instructions[0]);
    }
}

FString AVRInstructionSystem::GetCurrentInstruction()
{
    if (Instructions.IsValidIndex(CurrentInstructionIndex))
    {
        return Instructions[CurrentInstructionIndex];
    }
    return TEXT("Training Complete!");
}

void AVRInstructionSystem::NextInstruction()
{
    if (bTrainingComplete) return;

    CurrentInstructionIndex++;

    if (CurrentInstructionIndex >= Instructions.Num())
    {
        bTrainingComplete = true;
        OnInstructionChanged.Broadcast(
            TEXT("Training Complete! Well done!"),
            Instructions.Num(),
            Instructions.Num()
        );
        OnTrainingComplete.Broadcast();
        return;
    }

    FString NewInstruction = Instructions[CurrentInstructionIndex];
    OnInstructionChanged.Broadcast(NewInstruction,
        CurrentInstructionIndex + 1, Instructions.Num());
    UE_LOG(LogTemp, Warning, TEXT("Next instruction: %s"), *NewInstruction);
}