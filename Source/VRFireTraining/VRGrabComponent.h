#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "MotionControllerComponent.h"
#include "VRGrabComponent.generated.h"

UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class VRFIRETRAINING_API UVRGrabComponent : public UActorComponent
{
    GENERATED_BODY()

public:
    UVRGrabComponent();

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "VR Grab")
    UMotionControllerComponent* MotionControllerComponent;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "VR Grab")
    float GrabRadius = 10.0f;

    // Which hand is this? true = Right, false = Left
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "VR Grab")
    bool bIsRightHand = true;

    UFUNCTION(BlueprintCallable, Category = "VR Grab")
    void TryGrab();

    UFUNCTION(BlueprintCallable, Category = "VR Grab")
    void TryRelease();

    UPROPERTY(BlueprintReadOnly, Category = "VR Grab")
    bool bIsHolding;

protected:
    virtual void BeginPlay() override;

public:
    virtual void TickComponent(float DeltaTime, ELevelTick TickType,
        FActorComponentTickFunction* ThisTickFunction) override;

private:
    class AVRGrabbable* HeldObject;
    TArray<FVector> PositionHistory;
    AVRGrabbable* FindNearestGrabbable();
    FVector CalculateThrowVelocity();

    // Input binding functions
    void OnGrabPressed();
    void OnGrabReleased();

    // Enhanced Input references
    void SetupInputBindings();
};