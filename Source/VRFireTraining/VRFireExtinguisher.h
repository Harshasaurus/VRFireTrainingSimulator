#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "VRFireExtinguisher.generated.h"

UCLASS()
class VRFIRETRAINING_API AVRFireExtinguisher : public AActor
{
    GENERATED_BODY()

public:
    AVRFireExtinguisher();

    // Main mesh
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Extinguisher")
    UStaticMeshComponent* MeshComponent;

    // Grab sphere — hand must overlap to grab
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Extinguisher")
    class USphereComponent* GrabSphere;

    // Spray particle — shows when spraying
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Extinguisher")
    class UParticleSystemComponent* SprayParticle;

    // How far spray reaches in cm
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Extinguisher")
    float SprayRange = 200.0f;

    // Is extinguisher currently grabbed?
    UPROPERTY(BlueprintReadOnly, Category = "Extinguisher")
    bool bIsGrabbed = false;

    // Is currently spraying?
    UPROPERTY(BlueprintReadOnly, Category = "Extinguisher")
    bool bIsSpraying = false;

    // Called by grab component to grab
    UFUNCTION(BlueprintCallable, Category = "Extinguisher")
    void Grab(USceneComponent* AttachTo);

    // Called by grab component to release
    UFUNCTION(BlueprintCallable, Category = "Extinguisher")
    void Release(FVector ThrowVelocity);

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