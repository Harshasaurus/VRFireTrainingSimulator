#include "VRBuzzer.h"
#include "VRSimulationManager.h"
#include "Components/PointLightComponent.h"
#include "Kismet/GameplayStatics.h"

AVRBuzzer::AVRBuzzer()
{
    PrimaryActorTick.bCanEverTick = false;

    // Buzzer is mounted on a wall — it should never fall or move.
    // AVRGrabbable's constructor turns physics ON by default, so we turn it back off here.
    MeshComponent->SetSimulatePhysics(false);
    MeshComponent->SetCollisionProfileName(TEXT("BlockAll"));

    // Alarm light — red when active, off by default
    AlarmLight = CreateDefaultSubobject<UPointLightComponent>(TEXT("AlarmLight"));
    AlarmLight->SetupAttachment(RootComponent);
    AlarmLight->SetLightColor(FLinearColor::Red);
    AlarmLight->SetIntensity(0.f); // off until activated
    AlarmLight->SetAttenuationRadius(300.f);
}

void AVRBuzzer::BeginPlay()
{
    // AVRGrabbable::BeginPlay() — keeps grab sphere setup intact,
    // even though Grab() itself is now disabled below
    Super::BeginPlay();
    FindSimulationManager();
}

// ----------------------------------------------------------------
// Overridden — buzzer cannot be grabbed/attached to hand.
// Intentionally left empty so calling Grab() on this actor does nothing.
// ----------------------------------------------------------------

void AVRBuzzer::Grab(USceneComponent* AttachTo)
{
    // Do nothing — buzzer stays fixed in place.
    // Interaction happens only through PressBuzzer(), called from key input.
}

// ----------------------------------------------------------------
// API
// ----------------------------------------------------------------

void AVRBuzzer::PressBuzzer()
{
    if (bIsActivated && bOneTimeUse)
    {
        UE_LOG(LogTemp, Warning, TEXT("VRBuzzer: Already activated."));
        return;
    }

    bIsActivated = true;

    // Turn alarm light on
    if (AlarmLight)
        AlarmLight->SetIntensity(5000.f);

    // Play alarm sound at buzzer location
    if (AlarmSound)
        UGameplayStatics::PlaySoundAtLocation(this, AlarmSound, GetActorLocation());

    UE_LOG(LogTemp, Warning, TEXT("VRBuzzer: Buzzer pressed!"));

    // Notify SimulationManager (lazy init in case BeginPlay order was wrong)
    FindSimulationManager();
    if (SimulationManager)
        SimulationManager->OnBuzzerPressed();

    // Broadcast delegate so Blueprints / NPCs can also react
    OnBuzzerActivated.Broadcast();
}

void AVRBuzzer::ResetBuzzer()
{
    bIsActivated = false;

    if (AlarmLight)
        AlarmLight->SetIntensity(0.f);

    UE_LOG(LogTemp, Warning, TEXT("VRBuzzer: Reset."));
}

// ----------------------------------------------------------------
// Private
// ----------------------------------------------------------------

void AVRBuzzer::FindSimulationManager()
{
    if (SimulationManager) return; // already cached

    TArray<AActor*> Found;
    UGameplayStatics::GetAllActorsOfClass(
        GetWorld(), AVRSimulationManager::StaticClass(), Found);

    if (Found.Num() > 0)
        SimulationManager = Cast<AVRSimulationManager>(Found[0]);

    if (!SimulationManager)
        UE_LOG(LogTemp, Error, TEXT("VRBuzzer: SimulationManager not found!"));
}