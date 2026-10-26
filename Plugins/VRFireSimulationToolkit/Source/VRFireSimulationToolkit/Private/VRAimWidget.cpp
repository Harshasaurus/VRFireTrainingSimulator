#include "VRAimWidget.h"

#include "Blueprint/WidgetTree.h"
#include "Components/TextBlock.h"
#include "Components/VerticalBox.h"
#include "Components/VerticalBoxSlot.h"

TSharedRef<SWidget> UVRAimWidget::RebuildWidget()
{
    if (!WidgetTree)
    {
        WidgetTree = NewObject<UWidgetTree>(this, TEXT("WidgetTree"), RF_Transient);
    }

    if (!WidgetTree->RootWidget)
    {
        UVerticalBox* Root = WidgetTree->ConstructWidget<UVerticalBox>(UVerticalBox::StaticClass(), TEXT("AimRoot"));
        WidgetTree->RootWidget = Root;

        AngleText = WidgetTree->ConstructWidget<UTextBlock>(UTextBlock::StaticClass(), TEXT("AngleText"));
        FSlateFontInfo AngleFont = AngleText->GetFont();
        AngleFont.Size = 72;
        AngleText->SetFont(AngleFont);
        AngleText->SetJustification(ETextJustify::Center);
        AngleText->SetShadowOffset(FVector2D(2.f, 2.f));
        if (UVerticalBoxSlot* AngleSlot = Root->AddChildToVerticalBox(AngleText))
        {
            AngleSlot->SetHorizontalAlignment(HAlign_Center);
        }

        HintText = WidgetTree->ConstructWidget<UTextBlock>(UTextBlock::StaticClass(), TEXT("HintText"));
        FSlateFontInfo HintFont = HintText->GetFont();
        HintFont.Size = 36;
        HintText->SetFont(HintFont);
        HintText->SetJustification(ETextJustify::Center);
        HintText->SetShadowOffset(FVector2D(2.f, 2.f));
        if (UVerticalBoxSlot* HintSlot = Root->AddChildToVerticalBox(HintText))
        {
            HintSlot->SetHorizontalAlignment(HAlign_Center);
        }
    }

    return Super::RebuildWidget();
}

void UVRAimWidget::SetAim(int32 AngleDeg, const FLinearColor& Color, const FString& Hint)
{
    if (!AngleText || !HintText)
    {
        return;
    }

    AngleText->SetText(AngleDeg < 0
        ? FText::FromString(TEXT("--"))
        : FText::FromString(FString::Printf(TEXT("%d%c"), AngleDeg, TCHAR(176))));
    AngleText->SetColorAndOpacity(Color);
    HintText->SetColorAndOpacity(Color);
    HintText->SetText(FText::FromString(Hint));
}
