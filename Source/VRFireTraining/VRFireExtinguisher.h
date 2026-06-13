#pragma once
#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "VRGrabbable.h"
#include "VRFireExtinguisher.generated.h"

UCLASS()
class VRFIRETRAINING_API AVRFireExtinguisher : public AVRGrabbable
{
    GENERATED_BODY()
public:
    AVRFireExtinguisher();

    // Spray particle — shows when spraying
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Extinguisher")
    class UParticleSystemComponent* SprayParticle;

    // How far spray reaches in cm
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Extinguisher")
    float SprayRange = 200.0f;

    // Is currently spraying?
    UPROPERTY(BlueprintReadOnly, Category = "Extinguisher")
    bool bIsSpraying = false;

    // Called by grab component to grab
    // Called by grab component to grab
    virtual void Grab(USceneComponent* AttachTo) override;
    // Called by grab component to release
    virtual void Release(FVector ThrowVelocity) override;

    // Start spraying
    UFUNCTION(BlueprintCallable, Category = "Extinguisher")
    void StartSpray();

    // Stop spraying
    UFUNCTION(BlueprintCallable, Category = "Extinguisher")
    void StopSpray();

protected:
    virtual void BeginPlay() override;
public:
    virtual void Tick(float DeltaTime) override;
private:
    // Raycast forward to find fire
    void SprayTick(float DeltaTime);
};