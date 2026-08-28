// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "UI/LoadingScreenTypes.h"
#include "LoadingSubsystem.generated.h"
/**
 * 
 */

UCLASS()
class ULoadingSubsystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()

	/**
	 * 로딩 + 레벨 이동 (Auto/Standalone/ListenHost/ClientToServer)
	 *
	 * @param Target        "MapName" 또는 ClientToServer일 때 "127.0.0.1:7777" 같은 Address/URL
	 * @param TravelMode    이동 모드
	 * @param LoadingType   표시할 로딩 위젯 타입
	 * @param ExtraOptions  URL 옵션 추가 (예: "?game=/Game/..", "?Name=Player", "?Splitscreen=0" 등)
	 */

public:
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
	virtual void Deinitialize() override;

	UFUNCTION(BlueprintCallable)
	bool TravelWithLoading(
		const FString& Target,
		ETravelMode TravelMode = ETravelMode::Auto,
		ELoadingScreenType LoadingType = ELoadingScreenType::Main,
		const FString& ExtraOptions = TEXT("")
	);

	/** Listen Server로 열기 전용(명확하게 쓰고 싶으면) */
	UFUNCTION(BlueprintCallable)
	bool TravelAsListenHostWithLoading(
		const FString& MapName,
		ELoadingScreenType LoadingType = ELoadingScreenType::Main,
		const FString& ExtraOptions = TEXT("")
	);

	/** 클라가 서버로 붙기(주소/URL) */
	UFUNCTION(BlueprintCallable)
	bool ClientTravelToServerWithLoading(
		const FString& ServerURL,
		ELoadingScreenType LoadingType = ELoadingScreenType::Main
	);

	// 다음 Travel에 보여줄 로딩 타입 "예약"
	UFUNCTION(BlueprintCallable, Category = "LoadingSubsystem")
	void SetNextLoadingScreen(ELoadingScreenType InType);

	// 다음 Travel에서 Show를 1회 스킵 (중복 방지용)
	UFUNCTION(BlueprintCallable, Category = "LoadingSubsystem")
	void SuppressNextShowOnce();

	// (선택) 디스커넥트 같은 특수 상황 예약
	UFUNCTION(BlueprintCallable, Category = "LoadingSubsystem")
	void SetDisconnectedReturn(bool bInDisconnected);

	UFUNCTION(BlueprintCallable, Category = "LoadingSubsystem")
	bool ConsumeSkipEntryLevel();

	UFUNCTION(BlueprintCallable, Category = "LoadingSubsystem")
	void RequestSkipEntryLevel();
private:
	// 맵 로드 훅 (자동 로딩)
	void HandlePreLoadMap(const FString& MapName);
	void HandlePostLoadMap(UWorld* LoadedWorld);

	// 내부 유틸
	bool ShouldShowForMap(const FString& MapName) const;

	// 내부 헬퍼
	UWorld* GetWorldSafe() const;
	APlayerController* GetFirstLocalPC(UWorld* World) const;

	/** MapName + ExtraOptions를 URL로 합치는 유틸 */
	static FString BuildMapURL(const FString& MapName, const FString& ExtraOptions, bool bListen);

private:
	FDelegateHandle PreLoadHandle;
	FDelegateHandle PostLoadHandle;

	// 예약된 로딩 타입
	ELoadingScreenType PendingType = ELoadingScreenType::Entry;

	// 디스커넥트 복귀인지(EntryLevel 거쳐도 이 값으로 분기 가능)
	bool bDisconnectedReturn = false;

	// 다음 PreLoadMap에서 Show를 한 번 막기(중복 제거)
	bool bSuppressNextShowOnce = false;

	// 같은 Travel에서 중복 Show 막기
	bool bLoadingActive = false;

	//EntryLevelSkip
	bool bSkipEntryLoadingOnce = true;

	// 같은 맵 연속 Show 방지(원하면)
	FString LastShownMap;

};
