#include "VRInstructionSystem.h"
#include "VRFireExtinguisher.h"
#include "Kismet/GameplayStatics.h"

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
    FindAndBindExtinguisher();
    StartTraining();
}

void AVRInstructionSystem::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);
}

void AVRInstructionSystem::FindAndBindExtinguisher()
{
    if (Extinguisher) return;

    TArray<AActor*> Found;
    UGameplayStatics::GetAllActorsOfClass(GetWorld(),
        AVRFireExtinguisher::StaticClass(), Found);

    if (Found.Num() > 0)
        Extinguisher = Cast<AVRFireExtinguisher>(Found[0]);

    if (Extinguisher)
    {
        Extinguisher->OnExtinguisherGrabbed.AddDynamic(this, &AVRInstructionSystem::NextInstruction);
        Extinguisher->OnPinPulled.AddDynamic(this, &AVRInstructionSystem::NextInstruction);
        Extinguisher->OnSprayStarted.AddDynamic(this, &AVRInstructionSystem::NextInstruction);
        Extinguisher->OnExtinguisherReleased.AddDynamic(this, &AVRInstructionSystem::StartTraining);

        UE_LOG(LogTemp, Warning, TEXT("VRInstructionSystem: Bound to extinguisher delegates."));
    }
    else
    {
        UE_LOG(LogTemp, Error, TEXT("VRInstructionSystem: Could not find extinguisher to bind!"));
    }
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