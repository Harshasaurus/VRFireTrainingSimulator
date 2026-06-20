#include "VRBuzzer.h"
#include "VRSimulationManager.h"
#include "Components/PointLightComponent.h"
#include "Kismet/GameplayStatics.h"

AVRBuzzer::AVRBuzzer()
{
    PrimaryActorTick.bCanEverTick = false;

    // Alarm light — red when active, off by default
    AlarmLight = CreateDefaultSubobject<UPointLightComponent>(TEXT("AlarmLight"));
    AlarmLight->SetupAttachment(RootComponent);
    AlarmLight->SetLightColor(FLinearColor::Red);
    AlarmLight->SetIntensity(0.f); // off until activated
    AlarmLight->SetAttenuationRadius(300.f);
}

void AVRBuzzer::BeginPlay()
{
    // AVRGrabbable::BeginPlay() — keeps physics + grab sphere setup intact
    Super::BeginPlay();
    FindSimulationManager();
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