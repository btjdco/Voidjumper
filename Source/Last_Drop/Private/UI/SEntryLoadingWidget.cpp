// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/SEntryLoadingWidget.h"
#include "Widgets/Layout/SBox.h"

void SEntryLoadingWidget::Construct(const FArguments& InArgs)
{
	SBaseLoadingWidget::Construct(
		SBaseLoadingWidget::FArguments()
		.LogoPath(TEXT("/Game/Project/Rendering/Hud/Texture/T_LastDropLogo.T_LastDropLogo"))
		.LogoSize(FVector2D(300.f, 300.f))
		.FadeIn(FVector2D(0.f, 1.00f))
		.FadeOut(FVector2D(0.f, 1.00f))
		.ShowIndicator(true)
		.IndicatorPadding(FMargin(0, 0, 28, 22))
		.BackgroundColor(FLinearColor(0.00f, 0.00f, 0.00f, 1.0f)) // 예: 아주 어두운 남색
	);
}

TSharedRef<SWidget> SEntryLoadingWidget::BuildCenterContent()
{
	return SNew(SBox)
		.WidthOverride(300.f)
		.HeightOverride(300.f)
		[
			BuildLogoWidget()
		];
}