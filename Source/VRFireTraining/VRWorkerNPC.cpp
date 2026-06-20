#include "VRWorkerNPC.h"
#include "AIController.h"
#include "Navigation/PathFollowingComponent.h"         // FPathFollowingRequestResult
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/GameplayStatics.h"

AVRWorkerNPC::AVRWorkerNPC()
{
    PrimaryActorTick.bCanEverTick = true;
    AutoPossessAI = EAutoPossessAI::PlacedInWorldOrSpawned;
}

void AVRWorkerNPC::BeginPlay()
{
    Super::BeginPlay();

    SpawnTransform = GetActorTransform();

    AIController = Cast<AAIController>(GetController());
    if (!AIController)
        UE_LOG(LogTemp, Error, TEXT("VRWorkerNPC: No AIController found on %s"), *GetName());

    GetCharacterMovement()->MaxWalkSpeed = IdleWalkSpeed;
    SetState(EWorkerState::Idle);
}

void AVRWorkerNPC::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);

    if (CurrentState == EWorkerState::Panicking)
        CheckIfReachedExit();
}

void AVRWorkerNPC::OnAlarmTriggered()
{
    if (CurrentState != EWorkerState::Idle)
    {
        UE_LOG(LogTemp, Warning,
            TEXT("VRWorkerNPC: %s alarm received but not Idle — ignoring."), *GetName());
        return;
    }

    SetState(EWorkerState::Panicking);
    MoveToExit();
}

void AVRWorkerNPC::ResetNPC()
{
    if (AIController)
        AIController->StopMovement();

    SetActorTransform(SpawnTransform);
    GetCharacterMovement()->MaxWalkSpeed = IdleWalkSpeed;
    SetState(EWorkerState::Idle);

    UE_LOG(LogTemp, Warning, TEXT("VRWorkerNPC: %s reset to Idle."), *GetName());
}

void AVRWorkerNPC::SetState(EWorkerState NewState)
{
    CurrentState = NewState;

    switch (NewState)
    {
    case EWorkerState::Idle:
        GetCharacterMovement()->MaxWalkSpeed = IdleWalkSpeed;
        UE_LOG(LogTemp, Warning, TEXT("VRWorkerNPC: %s -> Idle"), *GetName());
        break;

    case EWorkerState::Panicking:
        GetCharacterMovement()->MaxWalkSpeed = PanicRunSpeed;
        UE_LOG(LogTemp, Warning, TEXT("VRWorkerNPC: %s -> Panicking"), *GetName());
        break;

    case EWorkerState::Evacuated:
        GetCharacterMovement()->MaxWalkSpeed = 0.f;
        UE_LOG(LogTemp, Warning, TEXT("VRWorkerNPC: %s -> Evacuated"), *GetName());
        break;
    }
}

void AVRWorkerNPC::MoveToExit()
{
    if (!AIController)
    {
        UE_LOG(LogTemp, Error,
            TEXT("VRWorkerNPC: %s has no AIController."), *GetName());
        return;
    }

    if (!ExitPoint)
    {
        UE_LOG(LogTemp, Error,
            TEXT("VRWorkerNPC: %s has no ExitPoint assigned."), *GetName());
        return;
    }

    FAIMoveRequest MoveRequest;
    MoveRequest.SetGoalActor(ExitPoint);
    MoveRequest.SetAcceptanceRadius(AcceptanceRadius);

    AIController->MoveTo(MoveRequest);

    UE_LOG(LogTemp, Warning,
        TEXT("VRWorkerNPC: %s moving to exit: %s"),
        *GetName(), *ExitPoint->GetName());
}

void AVRWorkerNPC::CheckIfReachedExit()
{
    if (!ExitPoint) return;

    float Distance = FVector::Dist(GetActorLocation(), ExitPoint->GetActorLocation());
    if (Distance <= AcceptanceRadius)
    {
        SetState(EWorkerState::Evacuated);

        if (AIController)
            AIController->StopMovement();
    }
}