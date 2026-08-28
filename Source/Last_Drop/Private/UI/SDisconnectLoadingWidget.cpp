// Fill out your copyright notice in the Description page of Project Settings.

#include "UI/SDisconnectLoadingWidget.h"
#include "Internationalization/Text.h"

void SDisconnectLoadingWidget::Construct(const FArguments& InArgs)
{

	SBaseLoadingWidget::Construct(
		SBaseLoadingWidget::FArguments()
		.LogoPath(TEXT("/Game/Project/Rendering/Hud/Texture/T_LastDropLogo.T_LastDropLogo"))
		.LogoSize(FVector2D(300.f, 300.f))
		.FadeIn(FVector2D(0.f, 1.00f))
		.FadeOut(FVector2D(0.f, 1.00f))
		.ShowIndicator(true)
		.IndicatorPadding(FMargin(0, 0, 28, 22))
		.BackgroundColor(FLinearColor(0.6f, 0.6f, 0.6f, 1.0f)) // 예: 아주 어두운 남색
	);
}

void SDisconnectLoadingWidget::SetStringTable()
{
	/*if (TipKey.IsEmpty())
	{
		CurrentTip = FText::FromString(TEXT("Error : Disconneted..."));
		return;
	}
	*/
	CurrentTip = FText::FromString(TEXT("Error : Disconneted..."));
	//CurrentTip = FText::FromStringTable(TipStringTableId, TipKey);
}

TSharedRef<SWidget> SDisconnectLoadingWidget::BuildCenterContent()
{
	// 로고 아래 ~ 화면 하단 사이 느낌: VerticalBox + padding
	return SNew(SVerticalBox)

		// 로고
		+ SVerticalBox::Slot()
		.AutoHeight()
		.HAlign(HAlign_Center)
		[
			SNew(SBox)
				.WidthOverride(300.f)
				.HeightOverride(300.f)
				[
					BuildLogoWidget()
				]
		]

	// 간격
	+ SVerticalBox::Slot()
		.AutoHeight()
		.Padding(FMargin(0, 16, 0, 0))
		.HAlign(HAlign_Center)
		[
			SNew(SBorder)
				.Padding(FMargin(18, 12))
				.BorderImage(FCoreStyle::Get().GetBrush("WhiteBrush"))
				.BorderBackgroundColor(FLinearColor(0.f, 0.f, 0.f, 0.35f))
				[
					SNew(STextBlock)
						.Text(CurrentTip)
						.Font(FCoreStyle::GetDefaultFontStyle("Regular", 14))
						.Justification(ETextJustify::Center)
						.WrapTextAt(900.f)
				]
		];
}