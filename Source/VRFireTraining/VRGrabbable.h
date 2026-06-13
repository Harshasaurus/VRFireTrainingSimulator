#pragma once
#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "VRGrabbable.generated.h"

UCLASS()
class VRFIRETRAINING_API AVRGrabbable : public AActor
{
    GENERATED_BODY()
public:
    AVRGrabbable();

    // Visible mesh and physics body for the grabbable actor.
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "VR Grab")
    UStaticMeshComponent* MeshComponent;

    // Overlap volume used by VR hands to detect nearby grabbable objects.
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "VR Grab")
    class USphereComponent* GrabSphere;

    // Attaches the object to the supplied hand or motion controller component.
    UFUNCTION(BlueprintCallable, Category = "VR Grab")
    virtual void Grab(USceneComponent* AttachTo);

    // Detaches the object and applies the hand velocity as throw momentum.
    UFUNCTION(BlueprintCallable, Category = "VR Grab")
    virtual void Release(FVector ThrowVelocity);

    // True while the object is attached to a grabbing hand.
    UPROPERTY(BlueprintReadOnly, Category = "VR Grab")
    bool bIsGrabbed;

protected:
    virtual void BeginPlay() override;
public:
    virtual void Tick(float DeltaTime) override;
};