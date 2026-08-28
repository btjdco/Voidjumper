// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/LoadingSubsystem.h"
#include "UI/FLoadingScreenService.h"

#include "Engine/Engine.h"
#include "Engine/World.h"
#include "Kismet/GameplayStatics.h"

#include "GameFramework/PlayerController.h"
#include "GameFramework/GameModeBase.h"

#include "UObject/UObjectGlobals.h"

#include "UObject/UObjectGlobals.h"
#include "Engine/World.h"

void ULoadingSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);

	// 맵 로드 전/후 훅
	PreLoadHandle = FCoreUObjectDelegates::PreLoadMap.AddUObject(this, &ULoadingSubsystem::HandlePreLoadMap);
	PostLoadHandle = FCoreUObjectDelegates::PostLoadMapWithWorld.AddUObject(this, &ULoadingSubsystem::HandlePostLoadMap);
}

void ULoadingSubsystem::Deinitialize()
{
	if (PreLoadHandle.IsValid())
	{
		FCoreUObjectDelegates::PreLoadMap.Remove(PreLoadHandle);
		PreLoadHandle.Reset();
	}
	if (PostLoadHandle.IsValid())
	{
		FCoreUObjectDelegates::PostLoadMapWithWorld.Remove(PostLoadHandle);
		PostLoadHandle.Reset();
	}

	Super::Deinitialize();
}

UWorld* ULoadingSubsystem::GetWorldSafe() const
{
	UGameInstance* GI = GetGameInstance();
	return GI ? GI->GetWorld() : nullptr;
}

APlayerController* ULoadingSubsystem::GetFirstLocalPC(UWorld* World) const
{
	if (!World) return nullptr;
	return World->GetFirstPlayerController();
}

FString ULoadingSubsystem::BuildMapURL(const FString& MapName, const FString& ExtraOptions, bool bListen)
{
	// MapName: "MainMenu" 또는 "/Game/Maps/MainMenu"
	FString URL = MapName;

	// ?listen 추가
	if (bListen)
	{
		// 이미 옵션이 있으면 &listen 또는 ?listen? 헷갈릴 수 있으니 안전하게 처리
		if (URL.Contains(TEXT("?")))
			URL += TEXT("&listen");
		else
			URL += TEXT("?listen");
	}

	// ExtraOptions 합치기: "?A=B&C=D" 또는 "A=B" 등 들어올 수 있어서 정리
	if (!ExtraOptions.IsEmpty())
	{
		FString Opt = ExtraOptions;

		// 선행 '?' 제거 (우리가 알아서 붙일 것)
		if (Opt.StartsWith(TEXT("?")))
			Opt.RightChopInline(1);

		if (URL.Contains(TEXT("?")))
			URL += TEXT("&") + Opt;
		else
			URL += TEXT("?") + Opt;
	}

	return URL;
}

bool ULoadingSubsystem::TravelWithLoading(
	const FString& Target,
	ETravelMode TravelMode,
	ELoadingScreenType LoadingType,
	const FString& ExtraOptions
)
{
	UWorld* World = GetWorldSafe();
	if (!World) return false;

	// 로딩 타입만 예약
	PendingType = LoadingType;

	if (TravelMode == ETravelMode::Auto)
	{
		const bool bLooksLikeAddress =
			Target.Contains(TEXT(":")) ||
			Target.StartsWith(TEXT("127.")) ||
			Target.StartsWith(TEXT("localhost"));

		TravelMode = bLooksLikeAddress
			? ETravelMode::ClientToServer
			: ETravelMode::Standalone;
	}

	switch (TravelMode)
	{
	case ETravelMode::Standalone:
	{
		UGameplayStatics::OpenLevel(
			World,
			FName(*Target),
			true,
			ExtraOptions
		);
		return true;
	}

	case ETravelMode::ListenHost:
		return TravelAsListenHostWithLoading(Target, LoadingType, ExtraOptions);

	case ETravelMode::ClientToServer:
		return ClientTravelToServerWithLoading(Target, LoadingType);

	default:
		return false;
	}
}

bool ULoadingSubsystem::TravelAsListenHostWithLoading(
	const FString& MapName,
	ELoadingScreenType LoadingType,
	const FString& ExtraOptions
)
{
	UWorld* World = GetWorldSafe();
	if (!World || !World->GetAuthGameMode())
		return false;

	// 예약만
	PendingType = LoadingType;

	const FString URL = BuildMapURL(MapName, ExtraOptions, true);

	UGameplayStatics::OpenLevel(
		World,
		FName(*MapName),
		true,
		URL.Contains(TEXT("?")) ? URL.RightChop(URL.Find(TEXT("?")) + 1) : TEXT("listen")
	);

	return true;
}

bool ULoadingSubsystem::ClientTravelToServerWithLoading(
	const FString& ServerURL,
	ELoadingScreenType LoadingType
)
{
	UWorld* World = GetWorldSafe();
	if (!World) return false;

	APlayerController* PC = GetFirstLocalPC(World);
	if (!PC) return false;

	// 예약만
	PendingType = LoadingType;

	// ServerURL 예:
	// - "127.0.0.1:7777"
	// - "steam.1234567890" (Steam OSS 환경에서)
	// - "/Game/Maps/GameMap?listen" 같은 URL도 가능하긴 함

	PC->ClientTravel(ServerURL, ETravelType::TRAVEL_Absolute);
	return true;
	
}

void ULoadingSubsystem::SetNextLoadingScreen(ELoadingScreenType InType)
{
	PendingType = InType;
}

void ULoadingSubsystem::SuppressNextShowOnce()
{
	bSuppressNextShowOnce = true;
}

void ULoadingSubsystem::SetDisconnectedReturn(bool bInDisconnected)
{
	bDisconnectedReturn = bInDisconnected;
}

bool ULoadingSubsystem::ConsumeSkipEntryLevel()
{
	if (bSkipEntryLoadingOnce)
	{
		bSkipEntryLoadingOnce = false; // ✅ 소비
		return true;
	}
	return false;
}

void ULoadingSubsystem::RequestSkipEntryLevel()
{
	bSkipEntryLoadingOnce = true;
}

bool ULoadingSubsystem::ShouldShowForMap(const FString& MapName) const
{
	// 여기서 “어떤 맵에서는 로딩 안 띄움” 규칙을 박을 수 있음
	// 예: 개발용 맵, 테스트 맵, 또는 특정 UI-only 맵 제외 등

	// 연속 같은 맵이면 스킵하고 싶으면:
	// if (LastShownMap.Equals(MapName, ESearchCase::IgnoreCase)) return false;

	return true;
}
void ULoadingSubsystem::HandlePreLoadMap(const FString& MapName)
{
	// 이번 Travel에서만 1회 스킵
	if (bSuppressNextShowOnce)
	{
		bSuppressNextShowOnce = false;
		return;
	}

	// 중복 방지
	if (bLoadingActive)
	{
		return;
	}

	// 맵 기준 필터
	if (!ShouldShowForMap(MapName))
	{
		return;
	}

	// Show
	FLoadingScreenService::Show(PendingType);

	bLoadingActive = true;
	LastShownMap = MapName;

	// 다음 Travel 기본값 복원
	PendingType = ELoadingScreenType::Main;
}

void ULoadingSubsystem::HandlePostLoadMap(UWorld* LoadedWorld)
{
	// MoviePlayer가 bAutoCompleteWhenLoadingCompletes=true면 자동으로 내려가지만,
	// 수동으로 내리고 싶으면 여기서 Hide 가능
	// FLoadingScreenService::Hide();

	bLoadingActive = false;

	// 디스커넥트 복귀 플래그는 “EntryLevel에서 한 번 소비”하고 끌 수도 있음
	// (여기서 끄면, EntryLevel에서 읽을 타이밍이 없을 수 있으니 보통 EntryLevel에서 끔)
}