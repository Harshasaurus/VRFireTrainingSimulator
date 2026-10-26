#include "VRMenuWidget.h"
#include "Blueprint/WidgetTree.h"
#include "Components/Border.h"
#include "Components/Button.h"
#include "Components/TextBlock.h"
#include "Components/VerticalBox.h"
#include "Components/VerticalBoxSlot.h"
#include "Components/HorizontalBox.h"
#include "Components/HorizontalBoxSlot.h"
#include "Components/SizeBox.h"
#include "Components/SizeBoxSlot.h"
#include "Components/BorderSlot.h"
#include "Brushes/SlateColorBrush.h"
#include "Brushes/SlateRoundedBoxBrush.h"

// ----------------------------------------------------------------
// Default manual text (edit here, or override via the menu manager's InstructionPages)
// ----------------------------------------------------------------

TArray<FVRInstructionPage> UVRMenuWidget::MakeDefaultPages()
{
    auto Page = [](const TCHAR* Title, const TCHAR* Body)
        {
            FVRInstructionPage P;
            P.Title = FText::FromString(Title);
            P.Body = FText::FromString(Body);
            return P;
        };

    TArray<FVRInstructionPage> Out;

    Out.Add(Page(TEXT("WELCOME"),
        TEXT("Welcome to the Fire Safety Training Simulator.\n\n")
        TEXT("You are inside a building when a fire breaks out. Workers are still inside and depend on you to act.\n\n")
        TEXT("This guide shows you what to do, step by step. Use NEXT to continue, or BACK to return to the menu.")));

    Out.Add(Page(TEXT("YOUR MISSION"),
        TEXT("1.  Raise the alarm so everyone can evacuate.\n")
        TEXT("2.  Take the fire extinguisher.\n")
        TEXT("3.  Put out every fire before it spreads.\n\n")
        TEXT("Your score depends on how fast and how accurately you act.")));

    Out.Add(Page(TEXT("STEP 1: RAISE THE ALARM"),
        TEXT("A fire starts a few seconds after you press START.\n\n")
        TEXT("Walk up to the alarm buzzer on the wall and press B.\n")
        TEXT("The alarm sounds and the workers begin to evacuate.\n\n")
        TEXT("Do this first. Every second counts.")));

    Out.Add(Page(TEXT("STEP 2: GRAB THE EXTINGUISHER"),
        TEXT("Find the red fire extinguisher in the room.\n\n")
        TEXT("Reach out and grab it (U).\n")
        TEXT("Hold it upright, with the hose pointing away from you.\n\n")
        TEXT("Keep hold of it while the fire is burning.")));

    Out.Add(Page(TEXT("STEP 3: PULL THE PIN"),
        TEXT("The safety pin stops the extinguisher from firing by accident.\n\n")
        TEXT("Press P to pull the pin.\n\n")
        TEXT("The extinguisher cannot spray until the pin has been pulled.")));

    Out.Add(Page(TEXT("STEP 4: AIM AND SPRAY"),
        TEXT("Point the nozzle at the BASE of the fire, not the flames.\n\n")
        TEXT("Tilt it down about 20 to 45 degrees toward the base.\n")
        TEXT("Stay within about 2 metres of the fire.\n")
        TEXT("Squeeze the right trigger and hold it to spray.")));

    Out.Add(Page(TEXT("READING YOUR AIM"),
        TEXT("A readout above the extinguisher shows your angle and tells you how to correct it:\n\n")
        TEXT("  Turn LEFT / Turn RIGHT  -  turn toward the fire\n")
        TEXT("  Aim DOWN / Aim UP  -  change the tilt of the nozzle\n")
        TEXT("  ON TARGET  -  your aim is correct")));

    Out.Add(Page(TEXT("SPRAY COLOUR"),
        TEXT("The colour of the spray shows how well you are aiming:\n\n")
        TEXT("  WHITE  -  correct aim, full power\n")
        TEXT("  YELLOW  -  partly off, weaker effect\n")
        TEXT("  RED  -  wrong angle, almost no effect\n\n")
        TEXT("Adjust your aim until the spray turns white.")));

    Out.Add(Page(TEXT("SWEEP THE FIRE"),
        TEXT("Sweep the spray slowly from side to side across the base of the fire.\n\n")
        TEXT("Keep spraying until the flames are completely out.\n")
        TEXT("Fires can spread if you wait, so look out for new ones.\n\n")
        TEXT("The simulation ends when every fire is out.")));

    Out.Add(Page(TEXT("CONTROLS"),
        TEXT("B  -  press the alarm buzzer\n")
        TEXT("U  -  grab the extinguisher\n")
        TEXT("P  -  pull the safety pin\n")
        TEXT("Right trigger  -  spray\n")
        TEXT("N or left menu button  -  pause menu\n")
        TEXT("Controller laser + trigger  -  click menu buttons")));

    Out.Add(Page(TEXT("SCORING"),
        TEXT("Your score is out of 100.\n\n")
        TEXT("  Alert speed  -  30%\n")
        TEXT("  Extinguish speed  -  50%\n")
        TEXT("  Aim accuracy  -  20%\n\n")
        TEXT("Press the buzzer quickly, put the fire out fast, and keep the spray white.")));

    Out.Add(Page(TEXT("TIPS"),
        TEXT("Stay calm. A steady aim works best.\n\n")
        TEXT("  -  Do not spray from too far away.\n")
        TEXT("  -  Do not aim at the top of the flames.\n")
        TEXT("  -  Always pull the pin before you spray.\n\n")
        TEXT("When you are ready, press BACK and then START.")));

    return Out;
}

// ----------------------------------------------------------------
// Build
// ----------------------------------------------------------------

TSharedRef<SWidget> UVRMenuWidget::RebuildWidget()
{
    if (!WidgetTree)
        WidgetTree = NewObject<UWidgetTree>(this, TEXT("WidgetTree"), RF_Transient);

    if (Pages.Num() == 0)
        Pages = MakeDefaultPages();

    if (!WidgetTree->RootWidget)
    {
        UBorder* Bg = WidgetTree->ConstructWidget<UBorder>(UBorder::StaticClass(), TEXT("Bg"));
        Bg->SetBrush(FSlateRoundedBoxBrush(FLinearColor(0.03f, 0.05f, 0.10f, 0.96f), 24.f));
        Bg->SetPadding(FMargin(40.f));
        WidgetTree->RootWidget = Bg;

        UVerticalBox* Root = WidgetTree->ConstructWidget<UVerticalBox>(UVerticalBox::StaticClass(), TEXT("Root"));
        Bg->SetContent(Root);

        auto MakeText = [&](const FString& Name, int32 Size, const FLinearColor& Color, ETextJustify::Type Just) -> UTextBlock*
            {
                UTextBlock* T = WidgetTree->ConstructWidget<UTextBlock>(UTextBlock::StaticClass(), FName(*Name));
                FSlateFontInfo Font = T->GetFont();
                Font.Size = Size;
                T->SetFont(Font);
                T->SetColorAndOpacity(FSlateColor(Color));
                T->SetJustification(Just);
                T->SetAutoWrapText(true);
                return T;
            };

        auto MakeButtonBox = [&](const FString& Name, UButton* Button, float Width, float Height) -> USizeBox*
            {
                USizeBox* Box = WidgetTree->ConstructWidget<USizeBox>(USizeBox::StaticClass(), FName(*Name));
                Box->SetWidthOverride(Width);
                Box->SetHeightOverride(Height);
                Box->SetContent(Button);
                return Box;
            };

        auto MakeButton = [&](const FString& Name, const TCHAR* Label, const FLinearColor& Color, int32 Size) -> UButton*
            {
                UButton* B = WidgetTree->ConstructWidget<UButton>(UButton::StaticClass(), FName(*Name));
                B->SetBackgroundColor(Color);
                UTextBlock* L = MakeText(Name + TEXT("_Label"), Size, FLinearColor::White, ETextJustify::Center);
                L->SetText(FText::FromString(Label));
                B->AddChild(L);
                return B;
            };

        auto AddToBox = [&](UVerticalBox* Box, UWidget* W, const FMargin& Pad, bool bFill = false)
            {
                if (UVerticalBoxSlot* S = Box->AddChildToVerticalBox(W))
                {
                    S->SetPadding(Pad);
                    S->SetHorizontalAlignment(HAlign_Fill);
                    if (bFill) S->SetSize(FSlateChildSize(ESlateSizeRule::Fill));
                }
            };

        const FLinearColor Green(0.1f, 0.55f, 0.2f);
        const FLinearColor Red(0.7f, 0.2f, 0.15f);
        const FLinearColor Blue(0.15f, 0.35f, 0.65f);
        const FLinearColor Grey(0.3f, 0.32f, 0.38f);
        const FLinearColor Accent(1.f, 0.75f, 0.2f);

        auto AddCentered = [&](UVerticalBox* Box, UWidget* W, const FMargin& Pad)
            {
                if (UVerticalBoxSlot* S = Box->AddChildToVerticalBox(W))
                {
                    S->SetPadding(Pad);
                    S->SetHorizontalAlignment(HAlign_Center);
                }
            };

        auto MakeDivider = [&](const FString& Name) -> USizeBox*
            {
                USizeBox* DividerBox = WidgetTree->ConstructWidget<USizeBox>(USizeBox::StaticClass(), FName(*Name));
                DividerBox->SetHeightOverride(3.f);
                UBorder* Divider = WidgetTree->ConstructWidget<UBorder>(UBorder::StaticClass(), FName(*(Name + TEXT("_Border"))));
                Divider->SetBrush(FSlateColorBrush(FLinearColor(1.f, 0.75f, 0.2f, 0.6f)));
                DividerBox->SetContent(Divider);
                return DividerBox;
            };

        // ---------------- Start / Pause screen ----------------
        MenuBox = WidgetTree->ConstructWidget<UVerticalBox>(UVerticalBox::StaticClass(), TEXT("MenuBox"));
        AddToBox(Root, MenuBox, FMargin(0.f));

        TitleText = MakeText(TEXT("Title"), 42, Accent, ETextJustify::Center);
        {
            FSlateFontInfo Font = TitleText->GetFont();
            Font.TypefaceFontName = FName(TEXT("Bold"));
            TitleText->SetFont(Font);
        }
        AddToBox(MenuBox, TitleText, FMargin(0.f));

        AddToBox(MenuBox, MakeDivider(TEXT("StartDivider")), FMargin(0.f, 12.f, 0.f, 18.f));

        SubText = MakeText(TEXT("Sub"), 24, FLinearColor(0.8f, 0.8f, 0.8f), ETextJustify::Center);
        AddToBox(MenuBox, SubText, FMargin(0.f, 0.f, 0.f, 24.f));

        StartButton = MakeButton(TEXT("StartBtn"), TEXT("START"), Green, 34);
        InstructionsButton = MakeButton(TEXT("InstructionsBtn"), TEXT("INSTRUCTIONS"), Blue, 34);
        ResumeButton = MakeButton(TEXT("ResumeBtn"), TEXT("RESUME"), Green, 34);
        RestartButton = MakeButton(TEXT("RestartBtn"), TEXT("RESTART"), Red, 34);

        AddCentered(MenuBox, MakeButtonBox(TEXT("StartBtnBox"), StartButton, 620.f, 78.f), FMargin(0.f, 7.f));
        AddCentered(MenuBox, MakeButtonBox(TEXT("InstructionsBtnBox"), InstructionsButton, 620.f, 78.f), FMargin(0.f, 7.f));
        AddCentered(MenuBox, MakeButtonBox(TEXT("ResumeBtnBox"), ResumeButton, 620.f, 78.f), FMargin(0.f, 7.f));
        AddCentered(MenuBox, MakeButtonBox(TEXT("RestartBtnBox"), RestartButton, 620.f, 78.f), FMargin(0.f, 7.f));

        StartButton->OnClicked.AddDynamic(this, &UVRMenuWidget::HandleStart);
        InstructionsButton->OnClicked.AddDynamic(this, &UVRMenuWidget::HandleInstructions);
        ResumeButton->OnClicked.AddDynamic(this, &UVRMenuWidget::HandleResume);
        RestartButton->OnClicked.AddDynamic(this, &UVRMenuWidget::HandleRestart);

        // ---------------- Instructions screen ----------------
        InstructionBox = WidgetTree->ConstructWidget<UVerticalBox>(UVerticalBox::StaticClass(), TEXT("InstructionBox"));
        AddToBox(Root, InstructionBox, FMargin(0.f), true);

        PageTitleText = MakeText(TEXT("PageTitle"), 38, Accent, ETextJustify::Center);
        {
            FSlateFontInfo Font = PageTitleText->GetFont();
            Font.TypefaceFontName = FName(TEXT("Bold"));
            PageTitleText->SetFont(Font);
        }
        AddToBox(InstructionBox, PageTitleText, FMargin(0.f));

        PageNumberText = MakeText(TEXT("PageNumber"), 18, FLinearColor(0.6f, 0.6f, 0.65f), ETextJustify::Center);
        AddToBox(InstructionBox, PageNumberText, FMargin(0.f, 6.f, 0.f, 14.f));

        PageBodyText = MakeText(TEXT("PageBody"), 22, FLinearColor(0.92f, 0.92f, 0.92f), ETextJustify::Left);
        PageBodyText->SetWrapTextAt(860.f);
        PageBodyText->SetLineHeightPercentage(1.2f);

        UBorder* BodyCard = WidgetTree->ConstructWidget<UBorder>(UBorder::StaticClass(), TEXT("BodyCard"));
        BodyCard->SetBrush(FSlateRoundedBoxBrush(FLinearColor(0.08f, 0.11f, 0.18f, 1.0f), 16.f));
        BodyCard->SetPadding(FMargin(24.f));
        BodyCard->SetClipping(EWidgetClipping::ClipToBounds);
        BodyCard->SetContent(PageBodyText);
        USizeBox* BodyCardBox = WidgetTree->ConstructWidget<USizeBox>(USizeBox::StaticClass(), TEXT("BodyCardBox"));
        BodyCardBox->SetWidthOverride(920.f);
        BodyCardBox->SetHeightOverride(400.f);
        BodyCardBox->SetContent(BodyCard);
        AddToBox(InstructionBox, BodyCardBox, FMargin(0.f));

        UHorizontalBox* Nav = WidgetTree->ConstructWidget<UHorizontalBox>(UHorizontalBox::StaticClass(), TEXT("Nav"));
        AddToBox(InstructionBox, Nav, FMargin(0.f, 14.f, 0.f, 0.f));

        PrevButton = MakeButton(TEXT("PrevBtn"), TEXT("< PREV"), Grey, 26);
        BackButton = MakeButton(TEXT("BackBtn"), TEXT("BACK"), Red, 26);
        NextButton = MakeButton(TEXT("NextBtn"), TEXT("NEXT >"), Grey, 26);

        for (UButton* B : { PrevButton, BackButton, NextButton })
        {
            USizeBox* ButtonBox = WidgetTree->ConstructWidget<USizeBox>(USizeBox::StaticClass(), FName(*(B->GetName() + TEXT("Box"))));
            ButtonBox->SetHeightOverride(60.f);
            ButtonBox->SetContent(B);
            if (UHorizontalBoxSlot* S = Nav->AddChildToHorizontalBox(ButtonBox))
            {
                S->SetSize(FSlateChildSize(ESlateSizeRule::Fill));
                S->SetPadding(FMargin(6.f, 0.f));
            }
        }

        PrevButton->OnClicked.AddDynamic(this, &UVRMenuWidget::HandlePrev);
        BackButton->OnClicked.AddDynamic(this, &UVRMenuWidget::HandleBack);
        NextButton->OnClicked.AddDynamic(this, &UVRMenuWidget::HandleNext);
    }

    TSharedRef<SWidget> Result = Super::RebuildWidget();
    ApplyMode();
    return Result;
}

// ----------------------------------------------------------------
// State
// ----------------------------------------------------------------

void UVRMenuWidget::SetMode(EVRMenuMode NewMode)
{
    Mode = NewMode;
    if (Mode == EVRMenuMode::Instructions)
        CurrentPage = 0;
    ApplyMode();
}

void UVRMenuWidget::SetInstructionPages(const TArray<FVRInstructionPage>& InPages)
{
    Pages = InPages;
    CurrentPage = 0;
    if (Mode == EVRMenuMode::Instructions)
        RefreshPage();
}

void UVRMenuWidget::ApplyMode()
{
    const bool bInstr = (Mode == EVRMenuMode::Instructions);
    const bool bStart = (Mode == EVRMenuMode::Start);

    if (MenuBox)        MenuBox->SetVisibility(bInstr ? ESlateVisibility::Collapsed : ESlateVisibility::SelfHitTestInvisible);
    if (InstructionBox) InstructionBox->SetVisibility(bInstr ? ESlateVisibility::SelfHitTestInvisible : ESlateVisibility::Collapsed);

    if (bInstr)
    {
        RefreshPage();
        return;
    }

    if (TitleText)
        TitleText->SetText(FText::FromString(bStart ? TEXT("FIRE SAFETY TRAINING") : TEXT("PAUSED")));
    if (SubText)
        SubText->SetText(FText::FromString(TEXT("Point at an option and pull the trigger")));

    const ESlateVisibility StartVis = bStart ? ESlateVisibility::Visible : ESlateVisibility::Collapsed;
    const ESlateVisibility PauseVis = bStart ? ESlateVisibility::Collapsed : ESlateVisibility::Visible;

    if (StartButton)        StartButton->SetVisibility(StartVis);
    if (InstructionsButton) InstructionsButton->SetVisibility(StartVis);
    if (ResumeButton)       ResumeButton->SetVisibility(PauseVis);
    if (RestartButton)      RestartButton->SetVisibility(PauseVis);
}

void UVRMenuWidget::RefreshPage()
{
    if (Pages.Num() == 0)
        Pages = MakeDefaultPages();

    CurrentPage = FMath::Clamp(CurrentPage, 0, Pages.Num() - 1);
    const FVRInstructionPage& P = Pages[CurrentPage];

    if (PageTitleText)  PageTitleText->SetText(P.Title);
    if (PageBodyText)   PageBodyText->SetText(P.Body);
    if (PageNumberText) PageNumberText->SetText(FText::FromString(FString::Printf(TEXT("Page %d of %d"), CurrentPage + 1, Pages.Num())));

    if (PageBodyText)
    {
        FSlateFontInfo Font = PageBodyText->GetFont();
        Font.Size = 22;
        PageBodyText->SetFont(Font);
    }

    if (PrevButton) PrevButton->SetVisibility(CurrentPage > 0 ? ESlateVisibility::Visible : ESlateVisibility::Hidden);
    if (NextButton) NextButton->SetVisibility(CurrentPage < Pages.Num() - 1 ? ESlateVisibility::Visible : ESlateVisibility::Hidden);
}

// ----------------------------------------------------------------
// Buttons
// ----------------------------------------------------------------

void UVRMenuWidget::HandleStart() { OnStartClicked.Broadcast(); }
void UVRMenuWidget::HandleResume() { OnResumeClicked.Broadcast(); }
void UVRMenuWidget::HandleRestart() { OnRestartClicked.Broadcast(); }

void UVRMenuWidget::HandleInstructions()
{
    PreviousMode = Mode;
    SetMode(EVRMenuMode::Instructions);
}

void UVRMenuWidget::HandleBack()
{
    SetMode(PreviousMode);
}

void UVRMenuWidget::HandlePrev()
{
    --CurrentPage;
    RefreshPage();
}

void UVRMenuWidget::HandleNext()
{
    ++CurrentPage;
    RefreshPage();
}
