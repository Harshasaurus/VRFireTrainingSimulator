#pragma once
#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "InputCoreTypes.h"
#include "VRMenuWidget.h"
#include "VRMenuManager.generated.h"

// Place one in the level. Shows a world-space Start menu, holds the scenario until
// START is pressed, and toggles a Pause (Resume / Restart) menu with a key or button.
UCLASS()
class VRFIRESIMULATIONTOOLKIT_API AVRMenuManager : public AActor
{
    GENERATED_BODY()
public:
    AVRMenuManager();

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Menu")
    class UWidgetComponent* MenuWidgetComp;

    // Show the Start menu at game start and hold the scenario until START is pressed.
    // (Remove the StartSimulation call from the Level Blueprint when this is on.)
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Menu")
    bool bShowStartMenu = true;

    // Restart reloads the level (cleanest reset). If false, calls ResetSimulation + StartSimulation.
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Menu")
    bool bReloadLevelOnRestart = true;

    // Distance in front of the player's head where the menu appears (cm)
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Menu")
    float MenuDistance = 150.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Menu")
    float MenuHeightOffset = 0.0f;

    // The pawn's Widget Interaction laser is raised to at least this reach (cm)
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Menu")
    float MinInteractionDistance = 1000.0f;

    // Pages of the instruction manual shown from the Start menu. Defaults are set in the constructor.
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Menu")
    TArray<FVRInstructionPage> InstructionPages;

    // Keys that toggle the pause menu (keyboard / simulator, and left Touch menu button)
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Menu")
    FKey PauseKey = EKeys::N;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Menu")
    FKey VRPauseKey = FKey(TEXT("OculusTouch_Left_Menu_Click"));

    // Also callable from the VR pawn Blueprint (e.g. from an input action)
    UFUNCTION(BlueprintCallable, Category = "Menu")
    void TogglePause();

    UFUNCTION(BlueprintCallable, Category = "Menu")
    bool IsMenuOpen() const { return bMenuOpen; }

protected:
    virtual void BeginPlay() override;

private:
    void ShowMenu(uint8 ModeAsByte);
    void HideMenu();
    void PlaceInFrontOfPlayer();
    class AVRSimulationManager* GetSimulationManager();

    UFUNCTION() void HandleStart();
    UFUNCTION() void HandleResume();
    UFUNCTION() void HandleRestart();

    UPROPERTY(Transient) class UVRMenuWidget* MenuWidget = nullptr;
    UPROPERTY(Transient) class AVRSimulationManager* SimulationManager = nullptr;

    bool bMenuOpen = false;
    bool bScenarioStarted = false;
    FTimerHandle ShowStartHandle;
};
