#include "VRWorkerNPC.h"
#include "AIController.h"
#include "Navigation/PathFollowingComponent.h"
#include "NavigationSystem.h"
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
    SpawnLocation = GetActorLocation();

    GetCharacterMovement()->MaxWalkSpeed = IdleWalkSpeed;
    SetState(EWorkerState::Idle);

    // Delay allows AIController to fully possess the character before wander starts
    FTimerHandle TimerHandle;
    GetWorldTimerManager().SetTimer(TimerHandle, [this]()
        {
            AIController = Cast<AAIController>(GetController());
            if (!AIController)
            {
                UE_LOG(LogTemp, Error, TEXT("VRWorkerNPC: %s — no AIController after delay!"), *GetName());
                return;
            }
            StartWander();
        }, 2.0f, false);
}

void AVRWorkerNPC::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);

    if (CurrentState == EWorkerState::Panicking)
        CheckIfReachedExit();

    if (CurrentState == EWorkerState::Idle)
        WanderTick(DeltaTime);
}

// ??? Wander ??????????????????????????????????????????????????????????????????

void AVRWorkerNPC::StartWander()
{
    if (!AIController || CurrentState != EWorkerState::Idle) return;

    FVector RandomPoint = SpawnLocation + FVector(
        FMath::RandRange(-WanderRadius, WanderRadius),
        FMath::RandRange(-WanderRadius, WanderRadius),
        0.f);

    EPathFollowingRequestResult::Type Result =
        AIController->MoveToLocation(RandomPoint, AcceptanceRadius,
            true, true, false, true);

    (void)Result;
    bIsWaiting = false;
}

void AVRWorkerNPC::WanderTick(float DeltaTime)
{
    if (!AIController || CurrentState != EWorkerState::Idle) return;

    if (bIsWaiting)
    {
        WanderWaitTimer -= DeltaTime;
        if (WanderWaitTimer <= 0.f)
        {
            bIsWaiting = false;
            StartWander();
        }
        return;
    }

    if (AIController->GetMoveStatus() == EPathFollowingStatus::Idle)
    {
        bIsWaiting = true;
        WanderWaitTimer = WanderWaitTime;
    }
}

// ??? Alarm & Evacuation ???????????????????????????????????????????????????????

void AVRWorkerNPC::OnAlarmTriggered()
{
    if (CurrentState != EWorkerState::Idle)
    {
        return;
    }

    SetState(EWorkerState::Panicking);
    MoveToExit();
}

void AVRWorkerNPC::MoveToExit()
{
    if (!AIController)
    {
        UE_LOG(LogTemp, Error, TEXT("VRWorkerNPC: %s has no AIController."), *GetName());
        return;
    }

    if (!ExitPoint)
    {
        UE_LOG(LogTemp, Error, TEXT("VRWorkerNPC: %s has no ExitPoint assigned."), *GetName());
        return;
    }

    FAIMoveRequest MoveRequest;
    MoveRequest.SetGoalActor(ExitPoint);
    MoveRequest.SetAcceptanceRadius(AcceptanceRadius);
    AIController->MoveTo(MoveRequest);

}

void AVRWorkerNPC::CheckIfReachedExit()
{
    if (!ExitPoint) return;

    if (FVector::Dist(GetActorLocation(), ExitPoint->GetActorLocation()) <= AcceptanceRadius)
    {
        SetState(EWorkerState::Evacuated);
        if (AIController)
            AIController->StopMovement();
    }
}

// ??? State & Reset ?

void AVRWorkerNPC::SetState(EWorkerState NewState)
{
    CurrentState = NewState;

    switch (NewState)
    {
    case EWorkerState::Idle:
        GetCharacterMovement()->MaxWalkSpeed = IdleWalkSpeed;
        break;

    case EWorkerState::Panicking:
        GetCharacterMovement()->MaxWalkSpeed = PanicRunSpeed;
        break;

    case EWorkerState::Evacuated:
        GetCharacterMovement()->MaxWalkSpeed = 0.f;
        break;
    }
}

void AVRWorkerNPC::ResetNPC()
{
    if (AIController)
        AIController->StopMovement();

    SetActorTransform(SpawnTransform);
    bIsWaiting = false;
    WanderWaitTimer = 0.f;

    GetCharacterMovement()->MaxWalkSpeed = IdleWalkSpeed;
    SetState(EWorkerState::Idle);
    StartWander();

}