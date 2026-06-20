#pragma once
#include "CoreMinimal.h"
#include "VRGrabbable.h"
#include "VRInstructionSystem.h"
#include "VRFireExtinguisher.generated.h"

UCLASS()
class VRFIRETRAINING_API AVRFireExtinguisher : public AVRGrabbable
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

protected:
    virtual void BeginPlay() override;

public:
    virtual void Tick(float DeltaTime) override;

private:
    void SprayTick(float DeltaTime);

    AVRInstructionSystem* InstructionSystem = nullptr;
    bool bHasStartedSprayBefore = false;
    bool bHasGrabbedBefore = false;

    void FindInstructionSystem();
};