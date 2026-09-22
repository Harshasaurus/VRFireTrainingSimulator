#pragma once
#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "VRFire.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnFireExtinguished);

UCLASS()
class VRFIRESIMULATIONTOOLKIT_API AVRFire : public AActor
{
    GENERATED_BODY()

public:
    AVRFire();

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Fire")
    class UParticleSystemComponent* FireParticle;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Fire")
    class USphereComponent* FireCollision;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Fire")
    float FireHealth = 100.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Fire")
    float ExtinguishRate = 20.0f;

    UPROPERTY(BlueprintReadOnly, Category = "Fire")
    bool bIsExtinguished = false;

    UFUNCTION(BlueprintCallable, Category = "Fire")
    void ApplyExtinguisher(float DeltaTime);

    UPROPERTY(BlueprintAssignable, Category = "Fire")
    FOnFireExtinguished OnFireExtinguished;

    // ----------------------------------------------------------------
    // Fire Spread
    // ----------------------------------------------------------------

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Fire|Spread")
    bool bCanSpread = true;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Fire|Spread")
    float SpreadInterval = 20.0f;

    // NEW: closest a new fire can spawn (cm) — keeps it out of the
    // original fire's own collision/particle radius
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Fire|Spread")
    float MinSpreadDistance = 60.0f;

    // Furthest a new fire can spawn (cm) — reduced so it stays
    // adjacent to the original instead of jumping across the room
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Fire|Spread")
    float MaxSpreadDistance = 120.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Fire|Spread")
    int32 MaxSpreadGenerations = 2;

    UPROPERTY(BlueprintReadOnly, Category = "Fire|Spread")
    int32 SpreadGeneration = 0;

protected:
    virtual void BeginPlay() override;

public:
    virtual void Tick(float DeltaTime) override;

private:
    void Extinguish();

    class AVRSimulationManager* SimulationManager = nullptr;
    void FindSimulationManager();

    FTimerHandle SpreadTimerHandle;
    void TrySpread();
};