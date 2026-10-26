#include "VRMenuManager.h"
#include "VRMenuWidget.h"
#include "VRSimulationManager.h"
#include "Components/WidgetComponent.h"
#include "Components/InputComponent.h"
#include "Camera/PlayerCameraManager.h"
#include "GameFramework/PlayerController.h"
#include "Kismet/GameplayStatics.h"
#include "TimerManager.h"
#include "Components/WidgetInteractionComponent.h"
#include "MotionControllerComponent.h"

AVRMenuManager::AVRMenuManager()
{
    PrimaryActorTick.bCanEverTick = false;
    InstructionPages = UVRMenuWidget::MakeDefaultPages();

    MenuWidgetComp = CreateDefaultSubobject<UWidgetComponent>(TEXT("MenuWidgetComp"));
    RootComponent = MenuWidgetComp;
    MenuWidgetComp->SetWidgetClass(UVRMenuWidget::StaticClass());
    MenuWidgetComp->SetWidgetSpace(EWidgetSpace::World);
    MenuWidgetComp->SetDrawSize(FVector2D(1000.f, 700.f));
    MenuWidgetComp->SetRelativeScale3D(FVector(0.13f));
    MenuWidgetComp->SetTwoSided(true);
    MenuWidgetComp->SetTickMode(ETickMode::Enabled);
    // Needed so the menu keeps redrawing (hover/click) while the game is paused
    MenuWidgetComp->PrimaryComponentTick.bTickEvenWhenPaused = true;
    MenuWidgetComp->SetVisibility(false);
    MenuWidgetComp->SetCollisionEnabled(ECollisionEnabled::NoCollision);
}

void AVRMenuManager::BeginPlay()
{
    Super::BeginPlay();

    MenuWidgetComp->InitWidget();
    MenuWidget = Cast<UVRMenuWidget>(MenuWidgetComp->GetUserWidgetObject());
    if (MenuWidget)
    {
        MenuWidget->OnStartClicked.AddDynamic(this, &AVRMenuManager::HandleStart);
        MenuWidget->OnResumeClicked.AddDynamic(this, &AVRMenuManager::HandleResume);
        MenuWidget->OnRestartClicked.AddDynamic(this, &AVRMenuManager::HandleRestart);
        MenuWidget->SetInstructionPages(InstructionPages);
    }

    if (bShowStartMenu)
    {
        if (AVRSimulationManager* Sim = GetSimulationManager())
            Sim->bRequireManualStart = true;
    }

    if (APlayerController* PC = UGameplayStatics::GetPlayerController(this, 0))
    {
        EnableInput(PC);
        if (InputComponent)
        {
            InputComponent->BindKey(PauseKey, IE_Pressed, this, &AVRMenuManager::TogglePause).bExecuteWhenPaused = true;
            InputComponent->BindKey(VRPauseKey, IE_Pressed, this, &AVRMenuManager::TogglePause).bExecuteWhenPaused = true;
        }
    }

    // Make the VR pointer + controllers keep updating while the game is paused,
    // so the Pause menu stays clickable (no Blueprint setting needed)
    if (APawn* Pawn = UGameplayStatics::GetPlayerPawn(this, 0))
    {
        TArray<UActorComponent*> Comps;
        Pawn->GetComponents(Comps);
        for (UActorComponent* C : Comps)
        {
            if (C && (C->IsA<UWidgetInteractionComponent>() || C->IsA<UMotionControllerComponent>()))
                C->PrimaryComponentTick.bTickEvenWhenPaused = true;

            if (UWidgetInteractionComponent* WI = Cast<UWidgetInteractionComponent>(C))
                WI->InteractionDistance = FMath::Max(WI->InteractionDistance, MinInteractionDistance);
        }
    }

    if (bShowStartMenu)
    {
        // Short delay so the HMD camera has a valid position before we place the menu
        GetWorldTimerManager().SetTimer(ShowStartHandle,
            FTimerDelegate::CreateLambda([this]()
                {
                    // Cancel any auto-start (e.g. a StartSimulation node in the Level Blueprint)
                    // so the scenario only begins when START is pressed
                    if (AVRSimulationManager* Sim = GetSimulationManager())
                        Sim->ResetSimulation();
                    ShowMenu((uint8)EVRMenuMode::Start);
                }), 0.5f, false);
    }
    else
    {
        bScenarioStarted = true;
    }
}

AVRSimulationManager* AVRMenuManager::GetSimulationManager()
{
    if (!SimulationManager)
    {
        TArray<AActor*> Found;
        UGameplayStatics::GetAllActorsOfClass(GetWorld(), AVRSimulationManager::StaticClass(), Found);
        if (Found.Num() > 0)
            SimulationManager = Cast<AVRSimulationManager>(Found[0]);
    }
    return SimulationManager;
}

void AVRMenuManager::PlaceInFrontOfPlayer()
{
    APlayerCameraManager* Cam = UGameplayStatics::GetPlayerCameraManager(this, 0);
    if (!Cam) return;

    const FVector CamLoc = Cam->GetCameraLocation();
    const FRotator YawOnly(0.f, Cam->GetCameraRotation().Yaw, 0.f);
    const FVector Pos = CamLoc + YawOnly.Vector() * MenuDistance + FVector(0.f, 0.f, MenuHeightOffset);

    FRotator Face = (CamLoc - Pos).Rotation();
    Face.Pitch = 0.f;
    Face.Roll = 0.f;

    SetActorLocationAndRotation(Pos, Face);
}

void AVRMenuManager::ShowMenu(uint8 ModeAsByte)
{
    const EVRMenuMode Mode = (EVRMenuMode)ModeAsByte;

    if (MenuWidget)
        MenuWidget->SetMode(Mode);

    PlaceInFrontOfPlayer();
    MenuWidgetComp->SetVisibility(true);
    MenuWidgetComp->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
    MenuWidgetComp->SetCollisionResponseToAllChannels(ECR_Block);
    MenuWidgetComp->SetCollisionObjectType(ECC_WorldDynamic);
    bMenuOpen = true;

    if (Mode == EVRMenuMode::Pause)
        UGameplayStatics::SetGamePaused(this, true);
}

void AVRMenuManager::HideMenu()
{
    MenuWidgetComp->SetVisibility(false);
    MenuWidgetComp->SetCollisionEnabled(ECollisionEnabled::NoCollision);
    bMenuOpen = false;
    UGameplayStatics::SetGamePaused(this, false);
}

void AVRMenuManager::TogglePause()
{
    // Ignore while the Start menu is still showing
    if (!bScenarioStarted) return;

    if (bMenuOpen)
        HideMenu();
    else
        ShowMenu((uint8)EVRMenuMode::Pause);
}

void AVRMenuManager::HandleStart()
{
    HideMenu();
    bScenarioStarted = true;

    if (AVRSimulationManager* Sim = GetSimulationManager())
        Sim->BeginScenarioFromMenu();
    else
        UE_LOG(LogTemp, Error, TEXT("VRMenuManager: SimulationManager not found, cannot start."));
}

void AVRMenuManager::HandleResume()
{
    HideMenu();
}

void AVRMenuManager::HandleRestart()
{
    UGameplayStatics::SetGamePaused(this, false);

    if (bReloadLevelOnRestart)
    {
        UGameplayStatics::OpenLevel(this, FName(*UGameplayStatics::GetCurrentLevelName(this, true)));
        return;
    }

    HideMenu();
    if (AVRSimulationManager* Sim = GetSimulationManager())
    {
        Sim->ResetSimulation();
        Sim->BeginScenarioFromMenu();
    }
}
