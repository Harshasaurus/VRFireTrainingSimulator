#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "VRAimWidget.generated.h"

class UTextBlock;

UCLASS()
class VRFIRESIMULATIONTOOLKIT_API UVRAimWidget : public UUserWidget
{
    GENERATED_BODY()

public:
    void SetAim(int32 AngleDeg, const FLinearColor& Color, const FString& Hint);

protected:
    virtual TSharedRef<SWidget> RebuildWidget() override;

private:
    UPROPERTY()
    UTextBlock* AngleText = nullptr;

    UPROPERTY()
    UTextBlock* HintText = nullptr;
};
