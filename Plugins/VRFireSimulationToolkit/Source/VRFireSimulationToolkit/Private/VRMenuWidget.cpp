#include "VRMenuWidget.h"
#include "Blueprint/WidgetTree.h"
#include "Components/Border.h"
#include "Components/BorderSlot.h"
#include "Components/Button.h"
#include "Components/ButtonSlot.h"
#include "Components/TextBlock.h"
#include "Components/VerticalBox.h"
#include "Components/VerticalBoxSlot.h"
#include "Components/HorizontalBox.h"
#include "Components/HorizontalBoxSlot.h"
#include "Components/SizeBox.h"
#include "Components/SizeBoxSlot.h"
#include "Components/Overlay.h"
#include "Components/OverlaySlot.h"
#include "Components/Spacer.h"
#include "Components/ScrollBox.h"
#include "Brushes/SlateColorBrush.h"
#include "Brushes/SlateRoundedBoxBrush.h"
#include "Styling/SlateTypes.h"

// ════════════════════════════════════════════════════════════════════
//  COLOUR PALETTE
//  Cohesive palette defined once here for easy tuning.
// ════════════════════════════════════════════════════════════════════

namespace VRMenuColors
{
    // --- Panel backgrounds ---
    static const FLinearColor PanelOuter         (0.020f, 0.028f, 0.065f, 0.97f);   // deep navy
    static const FLinearColor PanelInner         (0.045f, 0.060f, 0.115f, 1.0f);    // slightly lighter card bg
    static const FLinearColor CardBody           (0.070f, 0.095f, 0.165f, 1.0f);    // instruction body card
    static const FLinearColor KeyCapBg           (0.120f, 0.160f, 0.260f, 1.0f);    // key-cap chip bg

    // --- Accent ---
    static const FLinearColor AccentOrange       (1.0f,   0.55f,  0.10f,  1.0f);    // primary accent
    static const FLinearColor AccentAmber        (1.0f,   0.75f,  0.22f,  1.0f);    // warm gold
    static const FLinearColor AccentGlow         (1.0f,   0.50f,  0.08f,  0.12f);   // soft title glow

    // --- Divider ---
    static const FLinearColor DividerOrange      (1.0f,   0.55f,  0.10f,  0.50f);

    // --- Text ---
    static const FLinearColor TextWhite          (0.95f,  0.95f,  0.95f,  1.0f);
    static const FLinearColor TextLightGray      (0.78f,  0.80f,  0.84f,  1.0f);
    static const FLinearColor TextMuted          (0.48f,  0.50f,  0.56f,  1.0f);

    // --- Dot indicator ---
    static const FLinearColor DotActive          (1.0f,   0.55f,  0.10f,  1.0f);
    static const FLinearColor DotInactive        (0.25f,  0.28f,  0.35f,  1.0f);

    // --- Button: Primary (START, RESUME) ---
    static const FLinearColor BtnPrimary         (0.12f,  0.56f,  0.22f,  1.0f);
    static const FLinearColor BtnPrimaryHover    (0.18f,  0.68f,  0.30f,  1.0f);
    static const FLinearColor BtnPrimaryPressed  (0.08f,  0.40f,  0.15f,  1.0f);

    // --- Button: Secondary (INSTRUCTIONS) ---
    static const FLinearColor BtnSecondary       (0.14f,  0.32f,  0.58f,  1.0f);
    static const FLinearColor BtnSecondaryHover  (0.20f,  0.42f,  0.70f,  1.0f);
    static const FLinearColor BtnSecondaryPressed(0.09f,  0.22f,  0.42f,  1.0f);

    // --- Button: Destructive (RESTART, BACK) ---
    static const FLinearColor BtnDanger          (0.68f,  0.17f,  0.12f,  1.0f);
    static const FLinearColor BtnDangerHover     (0.82f,  0.25f,  0.18f,  1.0f);
    static const FLinearColor BtnDangerPressed   (0.48f,  0.12f,  0.08f,  1.0f);

    // --- Button: Nav (PREV, NEXT) ---
    static const FLinearColor BtnNav             (0.18f,  0.20f,  0.27f,  1.0f);
    static const FLinearColor BtnNavHover        (0.28f,  0.32f,  0.40f,  1.0f);
    static const FLinearColor BtnNavPressed      (0.12f,  0.14f,  0.19f,  1.0f);

    // --- Badge ---
    static const FLinearColor BadgeBg            (1.0f,   0.55f,  0.10f,  0.18f);
}

// ════════════════════════════════════════════════════════════════════
//  SIZING CONSTANTS
// ════════════════════════════════════════════════════════════════════

namespace VRMenuSizes
{
    static constexpr float PanelCornerRadius      = 20.f;
    static constexpr float PanelPadding           = 36.f;

    static constexpr float CardCornerRadius       = 14.f;
    static constexpr float CardPadding            = 28.f;

    static constexpr float ButtonCornerRadius     = 12.f;
    static constexpr float ButtonWidth            = 640.f;
    static constexpr float ButtonHeight           = 78.f;
    static constexpr float NavButtonHeight        = 64.f;

    static constexpr float DividerHeight          = 3.f;
    static constexpr float DividerWidthFraction   = 0.55f;  // ~55% of panel width

    static constexpr float DotSize                = 10.f;
    static constexpr float DotActiveSizeExtra     = 2.f;
    static constexpr float DotSpacing             = 6.f;
    static constexpr float DotCornerRadius        = 5.f;

    static constexpr float BadgeSize              = 38.f;
    static constexpr float BadgeCornerRadius      = 19.f;   // circle

    // Body card on instruction page
    static constexpr float BodyCardWidth          = 910.f;
    static constexpr float BodyCardHeight         = 345.f;

    // Key-cap chips on CONTROLS page
    static constexpr float KeyCapRadius           = 6.f;
    static constexpr int32 KeyCapFontSize         = 18;
    static constexpr int32 ControlDescFontSize     = 20;

    // Font sizes
    static constexpr int32 TitleFontSize          = 44;
    static constexpr int32 SubtitleFontSize       = 24;
    static constexpr int32 ButtonFontSize         = 30;
    static constexpr int32 PageTitleFontSize      = 36;
    static constexpr int32 BodyFontSize           = 22;
    static constexpr int32 PageNumFontSize        = 17;
    static constexpr int32 NavButtonFontSize      = 26;
    static constexpr int32 BadgeFontSize          = 18;
}

// ════════════════════════════════════════════════════════════════════
//  DEFAULT MANUAL TEXT
// ════════════════════════════════════════════════════════════════════

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

// ════════════════════════════════════════════════════════════════════
//  BUTTON STYLING
// ════════════════════════════════════════════════════════════════════

void UVRMenuWidget::StyleButton(UButton* Btn, const FLinearColor& NormalCol, const FLinearColor& HoveredCol, const FLinearColor& PressedCol, float Radius)
{
    if (!Btn) return;

    FButtonStyle Style = Btn->GetStyle();

    // Use FSlateRoundedBoxBrush for each state — gives rounded corners natively.
    Style.SetNormal(FSlateRoundedBoxBrush(NormalCol, Radius));
    Style.SetHovered(FSlateRoundedBoxBrush(HoveredCol, Radius));
    Style.SetPressed(FSlateRoundedBoxBrush(PressedCol, Radius));

    // Remove default press-offset so button content doesn't jump
    Style.NormalPadding  = FMargin(0.f);
    Style.PressedPadding = FMargin(0.f);

    Btn->SetStyle(Style);
}

// ════════════════════════════════════════════════════════════════════
//  PAGE DOTS
// ════════════════════════════════════════════════════════════════════

void UVRMenuWidget::BuildPageDots(UHorizontalBox* Container)
{
    using namespace VRMenuColors;
    using namespace VRMenuSizes;

    if (!Container || !WidgetTree) return;

    PageDots.Empty();

    const int32 NumDots = FMath::Max(Pages.Num(), 1);
    for (int32 i = 0; i < NumDots; ++i)
    {
        UBorder* Dot = WidgetTree->ConstructWidget<UBorder>(UBorder::StaticClass(),
            FName(*FString::Printf(TEXT("Dot_%d"), i)));
        Dot->SetBrush(FSlateRoundedBoxBrush(DotInactive, DotCornerRadius));

        USizeBox* DotBox = WidgetTree->ConstructWidget<USizeBox>(USizeBox::StaticClass(),
            FName(*FString::Printf(TEXT("DotBox_%d"), i)));
        DotBox->SetWidthOverride(DotSize);
        DotBox->SetHeightOverride(DotSize);
        DotBox->SetContent(Dot);

        if (UHorizontalBoxSlot* S = Container->AddChildToHorizontalBox(DotBox))
        {
            S->SetPadding(FMargin(DotSpacing * 0.5f, 0.f));
            S->SetHorizontalAlignment(HAlign_Center);
            S->SetVerticalAlignment(VAlign_Center);
        }

        PageDots.Add(Dot);
    }
}

void UVRMenuWidget::UpdatePageDots()
{
    using namespace VRMenuColors;
    using namespace VRMenuSizes;

    for (int32 i = 0; i < PageDots.Num(); ++i)
    {
        if (!PageDots[i]) continue;

        const bool bActive = (i == CurrentPage);
        const FLinearColor Color = bActive ? DotActive : DotInactive;
        PageDots[i]->SetBrush(FSlateRoundedBoxBrush(Color, DotCornerRadius));

        if (USizeBox* Box = Cast<USizeBox>(PageDots[i]->GetParent()))
        {
            const float Size = bActive ? (DotSize + DotActiveSizeExtra) : DotSize;
            Box->SetWidthOverride(Size);
            Box->SetHeightOverride(Size);
        }
    }
}

// ════════════════════════════════════════════════════════════════════
//  CONTROLS KEY-CAP ROWS
// ════════════════════════════════════════════════════════════════════

void UVRMenuWidget::BuildControlsRows(UVerticalBox* Container)
{
    using namespace VRMenuColors;
    using namespace VRMenuSizes;

    if (!Container || !WidgetTree) return;

    ControlKeyTexts.Empty();
    ControlDescTexts.Empty();
    ControlRowBoxes.Empty();

    constexpr int32 MaxControlRows = 8;
    for (int32 i = 0; i < MaxControlRows; ++i)
    {
        UHorizontalBox* Row = WidgetTree->ConstructWidget<UHorizontalBox>(UHorizontalBox::StaticClass(),
            FName(*FString::Printf(TEXT("CtrlRow_%d"), i)));

        // Key-cap chip border
        UBorder* Chip = WidgetTree->ConstructWidget<UBorder>(UBorder::StaticClass(),
            FName(*FString::Printf(TEXT("CtrlChip_%d"), i)));
        Chip->SetBrush(FSlateRoundedBoxBrush(KeyCapBg, KeyCapRadius));
        Chip->SetPadding(FMargin(12.f, 4.f));

        UTextBlock* KeyText = WidgetTree->ConstructWidget<UTextBlock>(UTextBlock::StaticClass(),
            FName(*FString::Printf(TEXT("CtrlKey_%d"), i)));
        FSlateFontInfo KeyFont = KeyText->GetFont();
        KeyFont.Size = KeyCapFontSize;
        KeyFont.TypefaceFontName = FName(TEXT("Bold"));
        KeyText->SetFont(KeyFont);
        KeyText->SetColorAndOpacity(FSlateColor(AccentAmber));
        KeyText->SetJustification(ETextJustify::Center);
        KeyText->SetAutoWrapText(false);
        Chip->SetContent(KeyText);

        if (UBorderSlot* ChipSlot = Cast<UBorderSlot>(Chip->GetContentSlot()))
        {
            ChipSlot->SetHorizontalAlignment(HAlign_Center);
            ChipSlot->SetVerticalAlignment(VAlign_Center);
        }

        if (UHorizontalBoxSlot* ChipHSlot = Row->AddChildToHorizontalBox(Chip))
        {
            ChipHSlot->SetVerticalAlignment(VAlign_Center);
            ChipHSlot->SetPadding(FMargin(0.f, 0.f, 16.f, 0.f));
        }

        // Action description text
        UTextBlock* DescText = WidgetTree->ConstructWidget<UTextBlock>(UTextBlock::StaticClass(),
            FName(*FString::Printf(TEXT("CtrlDesc_%d"), i)));
        FSlateFontInfo DescFont = DescText->GetFont();
        DescFont.Size = ControlDescFontSize;
        DescText->SetFont(DescFont);
        DescText->SetColorAndOpacity(FSlateColor(TextWhite));
        DescText->SetJustification(ETextJustify::Left);
        DescText->SetAutoWrapText(false);

        if (UHorizontalBoxSlot* DescHSlot = Row->AddChildToHorizontalBox(DescText))
        {
            DescHSlot->SetVerticalAlignment(VAlign_Center);
            DescHSlot->SetSize(FSlateChildSize(ESlateSizeRule::Fill));
        }

        if (UVerticalBoxSlot* RowVSlot = Container->AddChildToVerticalBox(Row))
        {
            RowVSlot->SetPadding(FMargin(0.f, 3.f));
            RowVSlot->SetHorizontalAlignment(HAlign_Fill);
        }

        ControlKeyTexts.Add(KeyText);
        ControlDescTexts.Add(DescText);
        ControlRowBoxes.Add(Row);
    }
}

void UVRMenuWidget::UpdateControlsPage(const FText& BodyText)
{
    TArray<FString> Lines;
    BodyText.ToString().ParseIntoArrayLines(Lines);

    for (int32 i = 0; i < ControlRowBoxes.Num(); ++i)
    {
        if (!ControlRowBoxes[i]) continue;

        if (i < Lines.Num())
        {
            const FString& Line = Lines[i];
            FString KeyPart, DescPart;

            if (Line.Split(TEXT("  -  "), &KeyPart, &DescPart) || Line.Split(TEXT(" - "), &KeyPart, &DescPart))
            {
                if (ControlKeyTexts.IsValidIndex(i) && ControlKeyTexts[i])
                    ControlKeyTexts[i]->SetText(FText::FromString(KeyPart.TrimStartAndEnd()));

                if (ControlDescTexts.IsValidIndex(i) && ControlDescTexts[i])
                    ControlDescTexts[i]->SetText(FText::FromString(DescPart.TrimStartAndEnd()));
            }
            else
            {
                if (ControlKeyTexts.IsValidIndex(i) && ControlKeyTexts[i])
                    ControlKeyTexts[i]->SetText(FText::GetEmpty());

                if (ControlDescTexts.IsValidIndex(i) && ControlDescTexts[i])
                    ControlDescTexts[i]->SetText(FText::FromString(Line.TrimStartAndEnd()));
            }

            ControlRowBoxes[i]->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
        }
        else
        {
            ControlRowBoxes[i]->SetVisibility(ESlateVisibility::Collapsed);
        }
    }
}

// ════════════════════════════════════════════════════════════════════
//  BUILD
// ════════════════════════════════════════════════════════════════════

TSharedRef<SWidget> UVRMenuWidget::RebuildWidget()
{
    using namespace VRMenuColors;
    using namespace VRMenuSizes;

    if (!WidgetTree)
        WidgetTree = NewObject<UWidgetTree>(this, TEXT("WidgetTree"), RF_Transient);

    if (Pages.Num() == 0)
        Pages = MakeDefaultPages();

    if (!WidgetTree->RootWidget)
    {
        // ──────────────────────────────────────────────
        //  Helper lambdas
        // ──────────────────────────────────────────────
        auto MakeText = [&](const FString& Name, int32 Size, const FLinearColor& Color,
            ETextJustify::Type Just, bool bBold = false) -> UTextBlock*
            {
                UTextBlock* T = WidgetTree->ConstructWidget<UTextBlock>(UTextBlock::StaticClass(), FName(*Name));
                FSlateFontInfo Font = T->GetFont();
                Font.Size = Size;
                if (bBold)
                    Font.TypefaceFontName = FName(TEXT("Bold"));
                T->SetFont(Font);
                T->SetColorAndOpacity(FSlateColor(Color));
                T->SetJustification(Just);
                T->SetAutoWrapText(true);
                return T;
            };

        auto MakeStyledButton = [&](const FString& Name, const TCHAR* Label, int32 FontSize,
            const FLinearColor& NormalCol, const FLinearColor& HoverCol, const FLinearColor& PressCol,
            bool bBold = true) -> UButton*
            {
                UButton* B = WidgetTree->ConstructWidget<UButton>(UButton::StaticClass(), FName(*Name));
                UTextBlock* L = MakeText(Name + TEXT("_Label"), FontSize, TextWhite, ETextJustify::Center, bBold);
                L->SetText(FText::FromString(Label));
                L->SetAutoWrapText(false);
                B->AddChild(L);
                if (UButtonSlot* ButtonSlot = Cast<UButtonSlot>(L->Slot))
                {
                    ButtonSlot->SetHorizontalAlignment(HAlign_Center);
                    ButtonSlot->SetVerticalAlignment(VAlign_Center);
                }
                StyleButton(B, NormalCol, HoverCol, PressCol, ButtonCornerRadius);
                return B;
            };

        auto MakeButtonBox = [&](const FString& Name, UButton* Button, float Width, float Height) -> USizeBox*
            {
                USizeBox* Box = WidgetTree->ConstructWidget<USizeBox>(USizeBox::StaticClass(), FName(*Name));
                Box->SetWidthOverride(Width);
                Box->SetHeightOverride(Height);
                Box->SetContent(Button);
                return Box;
            };

        auto AddToVBox = [&](UVerticalBox* Box, UWidget* W, const FMargin& Pad,
            EHorizontalAlignment HAlign = HAlign_Fill, bool bFill = false)
            {
                if (UVerticalBoxSlot* S = Box->AddChildToVerticalBox(W))
                {
                    S->SetPadding(Pad);
                    S->SetHorizontalAlignment(HAlign);
                    if (bFill) S->SetSize(FSlateChildSize(ESlateSizeRule::Fill));
                }
            };

        auto MakeSpacer = [&](const FString& Name, float Height) -> USpacer*
            {
                USpacer* Sp = WidgetTree->ConstructWidget<USpacer>(USpacer::StaticClass(), FName(*Name));
                Sp->SetSize(FVector2D(0.f, Height));
                return Sp;
            };

        auto MakeDivider = [&](const FString& Name, float Width) -> USizeBox*
            {
                USizeBox* DivBox = WidgetTree->ConstructWidget<USizeBox>(USizeBox::StaticClass(), FName(*(Name + TEXT("_SB"))));
                DivBox->SetHeightOverride(DividerHeight);
                DivBox->SetWidthOverride(Width);
                UBorder* Div = WidgetTree->ConstructWidget<UBorder>(UBorder::StaticClass(), FName(*(Name + TEXT("_Bd"))));
                Div->SetBrush(FSlateRoundedBoxBrush(DividerOrange, 2.f));
                DivBox->SetContent(Div);
                return DivBox;
            };

        // ──────────────────────────────────────────────
        //  OUTER PANEL: rounded dark navy
        // ──────────────────────────────────────────────
        UBorder* OuterBorder = WidgetTree->ConstructWidget<UBorder>(UBorder::StaticClass(), TEXT("OuterBorder"));
        OuterBorder->SetBrush(FSlateRoundedBoxBrush(PanelOuter, PanelCornerRadius));
        OuterBorder->SetPadding(FMargin(PanelPadding));
        WidgetTree->RootWidget = OuterBorder;

        // Inner wrapper: slightly lighter card for layered depth
        UBorder* InnerCard = WidgetTree->ConstructWidget<UBorder>(UBorder::StaticClass(), TEXT("InnerCard"));
        InnerCard->SetBrush(FSlateRoundedBoxBrush(PanelInner, CardCornerRadius));
        InnerCard->SetPadding(FMargin(24.f, 20.f));
        OuterBorder->SetContent(InnerCard);

        UVerticalBox* Root = WidgetTree->ConstructWidget<UVerticalBox>(UVerticalBox::StaticClass(), TEXT("Root"));
        InnerCard->SetContent(Root);

        // ══════════════════════════════════════════════
        //  START / PAUSE SCREEN
        // ══════════════════════════════════════════════
        MenuBox = WidgetTree->ConstructWidget<UVerticalBox>(UVerticalBox::StaticClass(), TEXT("MenuBox"));
        AddToVBox(Root, MenuBox, FMargin(0.f));

        // --- Title with soft amber glow background ---
        UOverlay* TitleOverlay = WidgetTree->ConstructWidget<UOverlay>(UOverlay::StaticClass(), TEXT("TitleOverlay"));
        AddToVBox(MenuBox, TitleOverlay, FMargin(0.f, 0.f, 0.f, 4.f), HAlign_Fill);

        // Glow layer sized appropriately behind title text
        USizeBox* GlowBox = WidgetTree->ConstructWidget<USizeBox>(USizeBox::StaticClass(), TEXT("TitleGlowBox"));
        GlowBox->SetWidthOverride(640.f);
        GlowBox->SetHeightOverride(56.f);

        UBorder* TitleGlow = WidgetTree->ConstructWidget<UBorder>(UBorder::StaticClass(), TEXT("TitleGlow"));
        TitleGlow->SetBrush(FSlateRoundedBoxBrush(AccentGlow, 28.f));
        GlowBox->SetContent(TitleGlow);

        if (UOverlaySlot* GlowSlot = TitleOverlay->AddChildToOverlay(GlowBox))
        {
            GlowSlot->SetHorizontalAlignment(HAlign_Center);
            GlowSlot->SetVerticalAlignment(VAlign_Center);
        }

        // Title text
        TitleText = MakeText(TEXT("Title"), TitleFontSize, AccentOrange, ETextJustify::Center, /*bBold=*/true);
        if (UOverlaySlot* TxtSlot = TitleOverlay->AddChildToOverlay(TitleText))
        {
            TxtSlot->SetHorizontalAlignment(HAlign_Center);
            TxtSlot->SetVerticalAlignment(VAlign_Center);
            TxtSlot->SetPadding(FMargin(0.f, 10.f));
        }

        // --- Accent divider ---
        AddToVBox(MenuBox, MakeDivider(TEXT("StartDiv"), ButtonWidth * DividerWidthFraction),
            FMargin(0.f, 10.f, 0.f, 14.f), HAlign_Center);

        // --- Subtitle ---
        SubText = MakeText(TEXT("Sub"), SubtitleFontSize, TextLightGray, ETextJustify::Center);
        AddToVBox(MenuBox, SubText, FMargin(0.f, 0.f, 0.f, 24.f), HAlign_Center);

        // --- Buttons ---
        StartButton = MakeStyledButton(TEXT("StartBtn"), TEXT("START"), ButtonFontSize,
            BtnPrimary, BtnPrimaryHover, BtnPrimaryPressed);
        InstructionsButton = MakeStyledButton(TEXT("InstrBtn"), TEXT("INSTRUCTIONS"), ButtonFontSize,
            BtnSecondary, BtnSecondaryHover, BtnSecondaryPressed, /*bBold=*/false);
        ResumeButton = MakeStyledButton(TEXT("ResumeBtn"), TEXT("RESUME"), ButtonFontSize,
            BtnPrimary, BtnPrimaryHover, BtnPrimaryPressed);
        RestartButton = MakeStyledButton(TEXT("RestartBtn"), TEXT("RESTART"), ButtonFontSize,
            BtnDanger, BtnDangerHover, BtnDangerPressed);

        AddToVBox(MenuBox, MakeButtonBox(TEXT("StartBB"), StartButton, ButtonWidth, ButtonHeight),
            FMargin(0.f, 6.f), HAlign_Center);
        AddToVBox(MenuBox, MakeButtonBox(TEXT("InstrBB"), InstructionsButton, ButtonWidth, ButtonHeight),
            FMargin(0.f, 6.f), HAlign_Center);
        AddToVBox(MenuBox, MakeButtonBox(TEXT("ResumeBB"), ResumeButton, ButtonWidth, ButtonHeight),
            FMargin(0.f, 6.f), HAlign_Center);
        AddToVBox(MenuBox, MakeButtonBox(TEXT("RestartBB"), RestartButton, ButtonWidth, ButtonHeight),
            FMargin(0.f, 6.f), HAlign_Center);

        StartButton->OnClicked.AddDynamic(this, &UVRMenuWidget::HandleStart);
        InstructionsButton->OnClicked.AddDynamic(this, &UVRMenuWidget::HandleInstructions);
        ResumeButton->OnClicked.AddDynamic(this, &UVRMenuWidget::HandleResume);
        RestartButton->OnClicked.AddDynamic(this, &UVRMenuWidget::HandleRestart);

        // ══════════════════════════════════════════════
        //  INSTRUCTIONS SCREEN
        // ══════════════════════════════════════════════
        InstructionBox = WidgetTree->ConstructWidget<UVerticalBox>(UVerticalBox::StaticClass(), TEXT("InstructionBox"));
        AddToVBox(Root, InstructionBox, FMargin(0.f), HAlign_Fill, /*bFill=*/true);

        // --- Title row: badge + page title ---
        UHorizontalBox* TitleRow = WidgetTree->ConstructWidget<UHorizontalBox>(UHorizontalBox::StaticClass(), TEXT("PageTitleRow"));
        AddToVBox(InstructionBox, TitleRow, FMargin(0.f, 0.f, 0.f, 2.f), HAlign_Center);

        // Page number badge (circular amber badge)
        PageBadgeBorder = WidgetTree->ConstructWidget<UBorder>(UBorder::StaticClass(), TEXT("PageBadge"));
        PageBadgeBorder->SetBrush(FSlateRoundedBoxBrush(BadgeBg, BadgeCornerRadius));
        PageBadgeBorder->SetPadding(FMargin(0.f));

        PageBadgeText = MakeText(TEXT("PageBadgeNum"), BadgeFontSize, AccentOrange, ETextJustify::Center, /*bBold=*/true);
        PageBadgeText->SetAutoWrapText(false);
        PageBadgeBorder->SetContent(PageBadgeText);

        if (UBorderSlot* BadgeSlot = Cast<UBorderSlot>(PageBadgeBorder->GetContentSlot()))
        {
            BadgeSlot->SetHorizontalAlignment(HAlign_Center);
            BadgeSlot->SetVerticalAlignment(VAlign_Center);
        }

        USizeBox* BadgeBox = WidgetTree->ConstructWidget<USizeBox>(USizeBox::StaticClass(), TEXT("BadgeBox"));
        BadgeBox->SetWidthOverride(BadgeSize);
        BadgeBox->SetHeightOverride(BadgeSize);
        BadgeBox->SetContent(PageBadgeBorder);

        if (UHorizontalBoxSlot* BS = TitleRow->AddChildToHorizontalBox(BadgeBox))
        {
            BS->SetPadding(FMargin(0.f, 0.f, 12.f, 0.f));
            BS->SetVerticalAlignment(VAlign_Center);
        }

        // Page title text
        PageTitleText = MakeText(TEXT("PageTitle"), PageTitleFontSize, AccentAmber, ETextJustify::Left, /*bBold=*/true);
        PageTitleText->SetAutoWrapText(false);
        if (UHorizontalBoxSlot* TS = TitleRow->AddChildToHorizontalBox(PageTitleText))
        {
            TS->SetVerticalAlignment(VAlign_Center);
        }

        // --- Dot progress indicator ---
        DotRow = WidgetTree->ConstructWidget<UHorizontalBox>(UHorizontalBox::StaticClass(), TEXT("DotRow"));
        AddToVBox(InstructionBox, DotRow, FMargin(0.f, 8.f, 0.f, 6.f), HAlign_Center);
        BuildPageDots(DotRow);

        // --- Page number text below dots ---
        PageNumberText = MakeText(TEXT("PageNumber"), PageNumFontSize, TextMuted, ETextJustify::Center);
        AddToVBox(InstructionBox, PageNumberText, FMargin(0.f, 0.f, 0.f, 10.f), HAlign_Center);

        // --- Body card ---
        UBorder* BodyCard = WidgetTree->ConstructWidget<UBorder>(UBorder::StaticClass(), TEXT("BodyCard"));
        BodyCard->SetBrush(FSlateRoundedBoxBrush(CardBody, CardCornerRadius));
        BodyCard->SetPadding(FMargin(CardPadding));
        BodyCard->SetClipping(EWidgetClipping::ClipToBounds);

        // Overlay inside card: normal body text OR dedicated controls chip rows
        UOverlay* CardOverlay = WidgetTree->ConstructWidget<UOverlay>(UOverlay::StaticClass(), TEXT("CardOverlay"));
        BodyCard->SetContent(CardOverlay);

        // 1. Normal body text (scrollable fallback)
        PageBodyText = MakeText(TEXT("PageBody"), BodyFontSize, TextWhite, ETextJustify::Left);
        PageBodyText->SetWrapTextAt(BodyCardWidth - (CardPadding * 2.f) - 20.f);
        PageBodyText->SetLineHeightPercentage(1.25f);

        BodyScroll = WidgetTree->ConstructWidget<UScrollBox>(UScrollBox::StaticClass(), TEXT("BodyScroll"));
        BodyScroll->AddChild(PageBodyText);
        BodyScroll->SetScrollBarVisibility(ESlateVisibility::Collapsed);
        if (UOverlaySlot* ScrollSlot = CardOverlay->AddChildToOverlay(BodyScroll))
        {
            ScrollSlot->SetHorizontalAlignment(HAlign_Fill);
            ScrollSlot->SetVerticalAlignment(VAlign_Fill);
        }

        // 2. Controls chip list
        ControlsBox = WidgetTree->ConstructWidget<UVerticalBox>(UVerticalBox::StaticClass(), TEXT("ControlsBox"));
        ControlsBox->SetVisibility(ESlateVisibility::Collapsed);
        BuildControlsRows(ControlsBox);
        if (UOverlaySlot* CtrlSlot = CardOverlay->AddChildToOverlay(ControlsBox))
        {
            CtrlSlot->SetHorizontalAlignment(HAlign_Fill);
            CtrlSlot->SetVerticalAlignment(VAlign_Fill);
        }

        USizeBox* BodyCardBox = WidgetTree->ConstructWidget<USizeBox>(USizeBox::StaticClass(), TEXT("BodyCardBox"));
        BodyCardBox->SetWidthOverride(BodyCardWidth);
        BodyCardBox->SetHeightOverride(BodyCardHeight);
        BodyCardBox->SetContent(BodyCard);
        AddToVBox(InstructionBox, BodyCardBox, FMargin(0.f), HAlign_Center, /*bFill=*/false);

        // --- Navigation buttons ---
        AddToVBox(InstructionBox, MakeSpacer(TEXT("NavSpacer"), 10.f), FMargin(0.f));

        UHorizontalBox* Nav = WidgetTree->ConstructWidget<UHorizontalBox>(UHorizontalBox::StaticClass(), TEXT("Nav"));
        AddToVBox(InstructionBox, Nav, FMargin(0.f, 4.f, 0.f, 0.f), HAlign_Fill);

        PrevButton = MakeStyledButton(TEXT("PrevBtn"), TEXT("<  PREV"), NavButtonFontSize,
            BtnNav, BtnNavHover, BtnNavPressed, /*bBold=*/false);
        BackButton = MakeStyledButton(TEXT("BackBtn"), TEXT("BACK"), NavButtonFontSize,
            BtnDanger, BtnDangerHover, BtnDangerPressed);
        NextButton = MakeStyledButton(TEXT("NextBtn"), TEXT("NEXT  >"), NavButtonFontSize,
            BtnNav, BtnNavHover, BtnNavPressed, /*bBold=*/false);

        for (UButton* B : { PrevButton, BackButton, NextButton })
        {
            USizeBox* NavBtnBox = WidgetTree->ConstructWidget<USizeBox>(USizeBox::StaticClass(),
                FName(*(B->GetName() + TEXT("Box"))));
            NavBtnBox->SetHeightOverride(NavButtonHeight);
            NavBtnBox->SetContent(B);
            if (UHorizontalBoxSlot* S = Nav->AddChildToHorizontalBox(NavBtnBox))
            {
                S->SetSize(FSlateChildSize(ESlateSizeRule::Fill));
                S->SetPadding(FMargin(5.f, 0.f));
                S->SetVerticalAlignment(VAlign_Fill);
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

// ════════════════════════════════════════════════════════════════════
//  STATE
// ════════════════════════════════════════════════════════════════════

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
    if (DotRow)
    {
        DotRow->ClearChildren();
        BuildPageDots(DotRow);
    }
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

    // Helper to collapse both the button and its wrapping SizeBox so hidden buttons leave no empty gap
    auto SetButtonBoxVisibility = [](UButton* Btn, ESlateVisibility Vis)
    {
        if (!Btn) return;
        Btn->SetVisibility(Vis);
        if (UWidget* Parent = Cast<UWidget>(Btn->GetParent()))
        {
            Parent->SetVisibility(Vis);
        }
    };

    SetButtonBoxVisibility(StartButton, StartVis);
    SetButtonBoxVisibility(InstructionsButton, StartVis);
    SetButtonBoxVisibility(ResumeButton, PauseVis);
    SetButtonBoxVisibility(RestartButton, PauseVis);
}

void UVRMenuWidget::RefreshPage()
{
    if (Pages.Num() == 0)
        Pages = MakeDefaultPages();

    CurrentPage = FMath::Clamp(CurrentPage, 0, Pages.Num() - 1);
    const FVRInstructionPage& P = Pages[CurrentPage];

    if (PageTitleText)  PageTitleText->SetText(P.Title);
    if (PageBodyText)   PageBodyText->SetText(P.Body);
    if (PageNumberText) PageNumberText->SetText(FText::FromString(
        FString::Printf(TEXT("Page %d of %d"), CurrentPage + 1, Pages.Num())));

    // Update badge number
    if (PageBadgeText)
        PageBadgeText->SetText(FText::AsNumber(CurrentPage + 1));

    // Keep body font size consistent
    if (PageBodyText)
    {
        FSlateFontInfo Font = PageBodyText->GetFont();
        Font.Size = VRMenuSizes::BodyFontSize;
        PageBodyText->SetFont(Font);
    }

    // Toggle between key-cap chip layout (CONTROLS page) and text card
    const bool bIsControls = P.Title.ToString().Equals(TEXT("CONTROLS"), ESearchCase::IgnoreCase);

    if (ControlsBox)
    {
        ControlsBox->SetVisibility(bIsControls ? ESlateVisibility::SelfHitTestInvisible : ESlateVisibility::Collapsed);
        if (bIsControls)
        {
            UpdateControlsPage(P.Body);
        }
    }

    if (BodyScroll)
    {
        BodyScroll->SetVisibility(bIsControls ? ESlateVisibility::Collapsed : ESlateVisibility::SelfHitTestInvisible);
    }

    // Update dot indicators
    UpdatePageDots();

    if (PrevButton) PrevButton->SetVisibility(CurrentPage > 0 ? ESlateVisibility::Visible : ESlateVisibility::Hidden);
    if (NextButton) NextButton->SetVisibility(CurrentPage < Pages.Num() - 1 ? ESlateVisibility::Visible : ESlateVisibility::Hidden);
}

// ════════════════════════════════════════════════════════════════════
//  BUTTON HANDLERS (unchanged)
// ════════════════════════════════════════════════════════════════════

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
