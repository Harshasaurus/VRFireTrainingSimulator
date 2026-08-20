#pragma once
#include "CoreMinimal.h"
#include "VRGrabbable.h"
#include "VRFireExtinguisher.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnExtinguisherGrabbed);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnPinPulled);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnSprayStarted);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnExtinguisherReleased);

UCLASS()
class VRFIRESIMULATIONTOOLKIT_API AVRFireExtinguisher : public AVRGrabbable
{
    GENERATED_BODY()
public:
    AVRFireExtinguisher();

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Extinguisher")
    class UParticleSystemComponent* SprayParticle;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Extinguisher")
    float SprayRange = 200.0f;

    UPROPERTY(BlueprintReadOnly, Category = "Extinguisher")
    bool bIsSpraying = false;

    UPROPERTY(BlueprintReadOnly, Category = "Extinguisher")
    bool bPinPulled = false;

    virtual void Grab(USceneComponent* AttachTo) override;
    virtual void Release(FVector ThrowVelocity) override;

    UFUNCTION(BlueprintCallable, Category = "Extinguisher")
    void PullPin();

    UFUNCTION(BlueprintCallable, Category = "Extinguisher")
    void StartSpray();

    UFUNCTION(BlueprintCallable, Category = "Extinguisher")
    void StopSpray();

    // ----------------------------------------------------------------
    // Delegates - external systems (e.g. an instruction/tutorial system
    // in the host project) bind to these instead of this class knowing
    // about any specific instruction system.
    // ----------------------------------------------------------------

    // Broadcast the first time this extinguisher is grabbed and released
    UPROPERTY(BlueprintAssignable, Category = "Extinguisher")
    FOnExtinguisherGrabbed OnExtinguisherGrabbed;

    UPROPERTY(BlueprintAssignable, Category = "Extinguisher")
    FOnExtinguisherReleased OnExtinguisherReleased;

    // Broadcast when the safety pin is pulled
    UPROPERTY(BlueprintAssignable, Category = "Extinguisher")
    FOnPinPulled OnPinPulled;

    // Broadcast the first time spray starts
    UPROPERTY(BlueprintAssignable, Category = "Extinguisher")
    FOnSprayStarted OnSprayStarted;

protected:
    virtual void BeginPlay() override;

public:
    virtual void Tick(float DeltaTime) override;

private:
    void SprayTick(float DeltaTime);

    bool bHasStartedSprayBefore = false;
    bool bHasGrabbedBefore = false;
};
