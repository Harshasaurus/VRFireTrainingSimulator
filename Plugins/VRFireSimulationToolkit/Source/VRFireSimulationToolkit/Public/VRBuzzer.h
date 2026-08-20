#pragma once
#include "CoreMinimal.h"
#include "VRGrabbable.h"
#include "VRBuzzer.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnBuzzerActivated);

UCLASS()
class VRFIRESIMULATIONTOOLKIT_API AVRBuzzer : public AVRGrabbable
{
    GENERATED_BODY()

public:
    AVRBuzzer();

    // ----------------------------------------------------------------
    // Components
    // ----------------------------------------------------------------

    // Light that turns red when alarm is active
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Buzzer")
    class UPointLightComponent* AlarmLight;

    // ----------------------------------------------------------------
    // Config
    // ----------------------------------------------------------------

    // Sound to play when buzzer is pressed (assign in Blueprint)
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Buzzer")
    class USoundBase* AlarmSound;

    // If true, buzzer can only be activated once per simulation
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Buzzer")
    bool bOneTimeUse = true;

    // ----------------------------------------------------------------
    // State
    // ----------------------------------------------------------------

    UPROPERTY(BlueprintReadOnly, Category = "Buzzer")
    bool bIsActivated = false;

    // ----------------------------------------------------------------
    // Delegate
    // ----------------------------------------------------------------

    // Bind this in SimulationManager or Blueprint to react to alarm
    UPROPERTY(BlueprintAssignable, Category = "Buzzer")
    FOnBuzzerActivated OnBuzzerActivated;

    // ----------------------------------------------------------------
    // API
    // ----------------------------------------------------------------

    // Call this from Blueprint input (e.g. right trigger while near buzzer)
    // or from an overlap event on the buzzer mesh
    UFUNCTION(BlueprintCallable, Category = "Buzzer")
    void PressBuzzer();

    // Resets buzzer for replay
    UFUNCTION(BlueprintCallable, Category = "Buzzer")
    void ResetBuzzer();

    // NEW: Overridden so the buzzer is fixed to the wall/panel and
    // never gets attached to the player's hand like other grabbables.
    // PressBuzzer() (via key input) is the only way to interact with it.
    virtual void Grab(USceneComponent* AttachTo) override;

protected:
    virtual void BeginPlay() override;

private:
    class AVRSimulationManager* SimulationManager = nullptr;
    void FindSimulationManager();
};