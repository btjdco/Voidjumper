// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Navigation/NavLinkProxy.h"
#include "AI_SmartLinkProxy.generated.h"

/**
 * 
 */
UCLASS()
class LAST_DROP_API AAI_SmartLinkProxy : public ANavLinkProxy
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintPure, Category = "Smart Link")
	void GetSmartLinkPoints(FVector& Start, FVector& End) const;

	UFUNCTION(BlueprintPure, Category = "Smart Link")
	float GetSmartLinkHeight() const;

	UFUNCTION(BlueprintCallable, Category = "Smart Link")
	void SetSmartLinkPoints(FVector Start, FVector End);
};
