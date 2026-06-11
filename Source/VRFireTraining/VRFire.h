#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "VRFire.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnFireExtinguished);

UCLASS()
class VRFIRETRAINING_API AVRFire : public AActor
{
    GENERATED_BODY()

public:
    AVRFire();

    // Fire particle system
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Fire")
    class UParticleSystemComponent* FireParticle;

    // Collision sphere
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Fire")
    class USphereComponent* FireCollision;

    // Fire health 100 = fully burning, 0 = extinguished
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Fire")
    float FireHealth = 100.0f;

    // How fast fire dies when sprayed
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Fire")
    float ExtinguishRate = 20.0f;

    // Is fire already out?
    UPROPERTY(BlueprintReadOnly, Category = "Fire")
    bool bIsExtinguished = false;

    // Called every frame while being sprayed
    UFUNCTION(BlueprintCallable, Category = "Fire")
    void ApplyExtinguisher(float DeltaTime);

    // Delegate — notifies instruction system when fire out
    UPROPERTY(BlueprintAssignable, Category = "Fire")
    FOnFireExtinguished OnFireExtinguished;

protected:
    virtual void BeginPlay() override;

public:
    virtual void Tick(float DeltaTime) override;

private:
    void Extinguish();
};