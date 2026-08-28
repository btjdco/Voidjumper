#pragma once

#include "CoreMinimal.h"

UENUM(BlueprintType)
enum class ELoadingScreenType : uint8
{
	Entry        UMETA(DisplayName = "Entry"),
	Main		 UMETA(DisplayName = "Main"),
	Disconnect   UMETA(DisplayName = "DisconnectReturn"),
};

UENUM(BlueprintType)
enum class ETravelMode : uint8
{
	Auto           UMETA(DisplayName = "Auto"),
	Standalone     UMETA(DisplayName = "Standalone"),
	ListenHost     UMETA(DisplayName = "ListenHost"),
	ClientToServer UMETA(DisplayName = "ClientToServer"),

};

UENUM(BlueprintType)
enum class ELoadingTriggerReason : uint8
{
	NormalTravel,
	DisconnectedReturn,
};