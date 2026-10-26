#pragma once
#include "CoreMinimal.h"
#include "VRGrabbable.h"
class UWidgetComponent;
#include "VRFireExtinguisher.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnExtinguisherGrabbed);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnPinPulled);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnSprayStarted);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnExtinguisherReleased);

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnAimEffectivenessChanged, float, Effectiveness);

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

    // Within this angle (degrees) of the fire, spray is at full effectiveness
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Extinguisher|Aim")
    float AimToleranceAngle = 15.0f;

    // Beyond this angle (degrees), spray has zero effect on the fire.
    // Between AimToleranceAngle and this, effectiveness fades linearly.
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Extinguisher|Aim")
    float AimFalloffAngle = 40.0f;

    // The particle system needs a Color parameter with this name.
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Extinguisher|Aim")
    FName SprayColorParameterName = TEXT("SprayColor");

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Extinguisher|Aim")
    float GoodAimThreshold = 0.85f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Extinguisher|Aim")
    float PartialAimThreshold = 0.4f;

    // Pitch-based aim uses a 45-degree downward target while preserving horizontal yaw alignment.
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Extinguisher|Aim")
    bool bUsePitchAim = true;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Extinguisher|Aim")
    float TargetPitchAngle = 45.0f;

    // If true, use the actual fire angle as the ideal pitch instead of TargetPitchAngle.
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Extinguisher|Aim")
    bool bTargetFirePitch = true;

    // Extra degrees added to the ideal fire pitch when bTargetFirePitch is true.
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Extinguisher|Aim")
    float FirePitchBias = 0.0f;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Extinguisher|Aim")
    UWidgetComponent* AimWidgetComp;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Extinguisher|Aim")
    bool bShowAimWidget = true;

    UPROPERTY(BlueprintReadOnly, Category = "Extinguisher|Aim")
    float CurrentAimAngle = -1.0f;

    // Current best aim quality this tick (0.0 = way off, 1.0 = dead on).
    // Read this directly, or bind OnAimEffectivenessChanged below.
    UPROPERTY(BlueprintReadOnly, Category = "Extinguisher|Aim")
    float CurrentAimEffectiveness = 0.0f;

    // Broadcasts every tick while spraying — bind this in your VR Pawn
    // Blueprint to drive haptic feedback, particle color, crosshair UI, etc.
    UPROPERTY(BlueprintAssignable, Category = "Extinguisher|Aim")
    FOnAimEffectivenessChanged OnAimEffectivenessChanged;

    virtual void Grab(USceneComponent* AttachTo) override;
    virtual void Release(FVector ThrowVelocity) override;

    UFUNCTION(BlueprintCallable, Category = "Extinguisher")
    void PullPin();

    UFUNCTION(BlueprintCallable, Category = "Extinguisher")
    void StartSpray();

    UFUNCTION(BlueprintCallable, Category = "Extinguisher")
    void StopSpray();

    UPROPERTY(BlueprintAssignable, Category = "Extinguisher")
    FOnExtinguisherGrabbed OnExtinguisherGrabbed;

    UPROPERTY(BlueprintAssignable, Category = "Extinguisher")
    FOnExtinguisherReleased OnExtinguisherReleased;

    UPROPERTY(BlueprintAssignable, Category = "Extinguisher")
    FOnPinPulled OnPinPulled;

    UPROPERTY(BlueprintAssignable, Category = "Extinguisher")
    FOnSprayStarted OnSprayStarted;

protected:
    virtual void BeginPlay() override;
public:
    virtual void Tick(float DeltaTime) override;

private:
    void SprayTick(float DeltaTime);
    void UpdateSprayColor(float Effectiveness, bool bFireInRange);
    void UpdateAimWidget();
    FLinearColor GetAimColor(float Effectiveness) const;
    void GetAimErrors(const FVector& FireLocation, float& OutPitch, float& OutPitchError, float& OutYawError, float& OutYawSigned) const;
    float GetTargetPitch(const FVector& FireLocation) const;
    float GetAimEffectiveness(const FVector& FireLocation) const;

    bool bHasStartedSprayBefore = false;
    bool bHasGrabbedBefore = false;
};
