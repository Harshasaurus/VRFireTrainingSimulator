#include "VRGrabComponent.h"
#include "VRGrabbable.h"
#include "DrawDebugHelpers.h"
#include "Kismet/GameplayStatics.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "InputAction.h"
#include "GameFramework/Pawn.h"
#include "GameFramework/PlayerController.h"

UVRGrabComponent::UVRGrabComponent()
{
    PrimaryComponentTick.bCanEverTick = true;
    bIsHolding = false;
    HeldObject = nullptr;
}

void UVRGrabComponent::BeginPlay()
{
    Super::BeginPlay();

    // Find motion controller on owner pawn at runtime
    AActor* Owner = GetOwner();
    if (Owner)
    {
        FName SocketName = bIsRightHand ?
            FName("MotionControllerRightAim") :
            FName("MotionControllerLeftAim");

        TArray<UActorComponent*> Components;
        Owner->GetComponents(UMotionControllerComponent::StaticClass(), Components);

        for (UActorComponent* Comp : Components)
        {
            UMotionControllerComponent* MC = Cast<UMotionControllerComponent>(Comp);
            if (MC && MC->GetName().Contains(bIsRightHand ? "Right" : "Left"))
            {
                MotionControllerComponent = MC;
                UE_LOG(LogTemp, Warning, TEXT("GrabComponent: Found controller: %s"),
                    *MC->GetName());
                break;
            }
        }

        if (!MotionControllerComponent)
        {
            UE_LOG(LogTemp, Error, TEXT("GrabComponent: Could not find motion controller!"));
        }
    }

    SetupInputBindings();
}
void UVRGrabComponent::SetupInputBindings()
{
    APawn* OwnerPawn = Cast<APawn>(GetOwner());
    if (!OwnerPawn) return;

    APlayerController* PC = Cast<APlayerController>(OwnerPawn->GetController());
    if (!PC) return;

    UEnhancedInputComponent* EIC = Cast<UEnhancedInputComponent>(OwnerPawn->InputComponent);
    if (!EIC) return;

    if (!GrabAction)
    {
        UE_LOG(LogTemp, Error, TEXT("VRGrabComponent: No GrabAction assigned!"));
        return;
    }

    EIC->BindAction(GrabAction, ETriggerEvent::Triggered,
        this, &UVRGrabComponent::OnGrabPressed);
}

void UVRGrabComponent::OnGrabPressed()
{
    if (bIsHolding)
    {
        TryRelease();
    }
    else
    {
        TryGrab();
    }
}

void UVRGrabComponent::OnGrabReleased()
{
    TryRelease();
}

void UVRGrabComponent::TickComponent(float DeltaTime, ELevelTick TickType,
    FActorComponentTickFunction* ThisTickFunction)
{
    Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

    if (MotionControllerComponent)
    {
        FVector CurrentPos = MotionControllerComponent->GetComponentLocation();
        PositionHistory.Add(CurrentPos);

        if (PositionHistory.Num() > 5)
            PositionHistory.RemoveAt(0);

        // Debug sphere ? green = empty hand, red = holding
        DrawDebugSphere(GetWorld(), CurrentPos, GrabRadius, 8,
            bIsHolding ? FColor::Red : FColor::Green, false, -1.0f);
    }
}

void UVRGrabComponent::TryGrab()
{
   
    if (bIsHolding) return;

    AVRGrabbable* Nearest = FindNearestGrabbable();
    if (Nearest && MotionControllerComponent && !Nearest->bIsGrabbed)
    {
        HeldObject = Nearest;
        bIsHolding = true;
        HeldObject->Grab(MotionControllerComponent);
        UE_LOG(LogTemp, Warning, TEXT("Grabbed: %s"), *HeldObject->GetName());
    }
}
void UVRGrabComponent::TryRelease()
{
    if (!bIsHolding || !HeldObject) return;

    FVector ThrowVelocity = CalculateThrowVelocity();
    HeldObject->Release(ThrowVelocity);
    UE_LOG(LogTemp, Warning, TEXT("Released with velocity: %s"),
        *ThrowVelocity.ToString());

    HeldObject = nullptr;
    bIsHolding = false;
    PositionHistory.Empty();
}

AVRGrabbable* UVRGrabComponent::FindNearestGrabbable()
{
    if (!MotionControllerComponent)
    {
        UE_LOG(LogTemp, Error, TEXT("No MotionControllerComponent set!"));
        return nullptr;
    }

    FVector HandLocation = MotionControllerComponent->GetComponentLocation();

    TArray<AActor*> FoundActors;
    UGameplayStatics::GetAllActorsOfClass(GetWorld(),
        AVRGrabbable::StaticClass(), FoundActors);

    UE_LOG(LogTemp, Warning, TEXT("Found %d grabbable actors. Hand at: %s. Radius: %f"),
        FoundActors.Num(), *HandLocation.ToString(), GrabRadius);

    AVRGrabbable* Nearest = nullptr;
    float NearestDistance = GrabRadius;

    for (AActor* Actor : FoundActors)
    {
        float Distance = FVector::Dist(HandLocation, Actor->GetActorLocation());
        UE_LOG(LogTemp, Warning, TEXT("Actor %s distance: %f"),
            *Actor->GetName(), Distance);

        if (Distance < NearestDistance)
        {
            NearestDistance = Distance;
            Nearest = Cast<AVRGrabbable>(Actor);
        }
    }

    return Nearest;
}

FVector UVRGrabComponent::CalculateThrowVelocity()
{
    if (PositionHistory.Num() < 2) return FVector::ZeroVector;

    FVector OldestPos = PositionHistory[0];
    FVector NewestPos = PositionHistory.Last();
    return (NewestPos - OldestPos) * 1000.0f;
}