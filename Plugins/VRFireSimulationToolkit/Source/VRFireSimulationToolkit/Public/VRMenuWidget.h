#pragma once
#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "VRMenuWidget.generated.h"

UENUM(BlueprintType)
enum class EVRMenuMode : uint8
{
    Start,
    Pause,
    Instructions
};

// One page of the instruction manual
USTRUCT(BlueprintType)
struct FVRInstructionPage
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Menu")
    FText Title;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Menu", meta = (MultiLine = "true"))
    FText Body;
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnMenuButtonClicked);

// Self-building menu (no Blueprint needed): Start screen, Pause screen and
// a multi-page Instructions manual opened from the Start screen.
UCLASS()
class VRFIRESIMULATIONTOOLKIT_API UVRMenuWidget : public UUserWidget
{
    GENERATED_BODY()
public:
    UFUNCTION(BlueprintCallable, Category = "Menu")
    void SetMode(EVRMenuMode NewMode);

    // Replace the manual's pages (defaults are used if never called)
    UFUNCTION(BlueprintCallable, Category = "Menu")
    void SetInstructionPages(const TArray<FVRInstructionPage>& InPages);

    // The built-in manual text
    static TArray<FVRInstructionPage> MakeDefaultPages();

    UPROPERTY(BlueprintAssignable, Category = "Menu")
    FOnMenuButtonClicked OnStartClicked;

    UPROPERTY(BlueprintAssignable, Category = "Menu")
    FOnMenuButtonClicked OnResumeClicked;

    UPROPERTY(BlueprintAssignable, Category = "Menu")
    FOnMenuButtonClicked OnRestartClicked;

protected:
    virtual TSharedRef<SWidget> RebuildWidget() override;

private:
    UFUNCTION() void HandleStart();
    UFUNCTION() void HandleResume();
    UFUNCTION() void HandleRestart();
    UFUNCTION() void HandleInstructions();
    UFUNCTION() void HandleBack();
    UFUNCTION() void HandlePrev();
    UFUNCTION() void HandleNext();

    void ApplyMode();
    void RefreshPage();

    EVRMenuMode Mode = EVRMenuMode::Start;
    EVRMenuMode PreviousMode = EVRMenuMode::Start;
    int32 CurrentPage = 0;

    UPROPERTY() TArray<FVRInstructionPage> Pages;

    UPROPERTY() class UVerticalBox* MenuBox = nullptr;
    UPROPERTY() class UVerticalBox* InstructionBox = nullptr;

    UPROPERTY() class UTextBlock* TitleText = nullptr;
    UPROPERTY() class UTextBlock* SubText = nullptr;
    UPROPERTY() class UButton* StartButton = nullptr;
    UPROPERTY() class UButton* InstructionsButton = nullptr;
    UPROPERTY() class UButton* ResumeButton = nullptr;
    UPROPERTY() class UButton* RestartButton = nullptr;

    UPROPERTY() class UTextBlock* PageTitleText = nullptr;
    UPROPERTY() class UTextBlock* PageBodyText = nullptr;
    UPROPERTY() class UTextBlock* PageNumberText = nullptr;
    UPROPERTY() class UButton* PrevButton = nullptr;
    UPROPERTY() class UButton* NextButton = nullptr;
    UPROPERTY() class UButton* BackButton = nullptr;
};