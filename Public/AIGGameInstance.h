// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "AIGroove.h"
#include "HAL/PlatformFilemanager.h"
#include "Misc/Paths.h"
#include "Misc/FileHelper.h"
#include "Engine/GameInstance.h"
#include "AIGStageData.h"
#include "AIGGameInstance.generated.h"

/**
 *
 */
UCLASS()
class AIGROOVE_API UAIGGameInstance : public UGameInstance
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable)
	void OnPlayerDeath();

	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable)
	void ShowMainMenu();

	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable)
	void ShowLoginScreen();

	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable)
	void ShowLoginErrorPopup();

	UPROPERTY(BlueprintReadOnly)
	FString AuthToken;

	UFUNCTION(BlueprintCallable)
	void SetAuthToken(const FString& InToken);

	UFUNCTION(BlueprintPure)
	FString GetAuthToken() const;

	UPROPERTY(BlueprintReadOnly)
	FString UserName;

	UFUNCTION(BlueprintCallable)
	void SetUserName(const FString& InUserName);

	UFUNCTION(BlueprintPure)
	FString GetUserName() const;

	UPROPERTY(BlueprintReadWrite, Category = "StageData")
	FStageData SelectedStageData;


	//오디오 관련---
	UPROPERTY()
	UAudioComponent* StageMusicComponent;

	UFUNCTION(BlueprintCallable)
	void PlayStageMusic(USoundBase* Music);

	UFUNCTION(BlueprintCallable)
	void StopStageMusic();
	//----

	//안드로이드에서 폴더 생성하는 함수
	UFUNCTION(BlueprintCallable)
	void EnsureAIGFoldersExist();

	UFUNCTION(BlueprintCallable)
	bool CopySelectedWavToAudioFolder(const FString& SourcePath, const FString& SongTitle);

	//StageData Attempts 수정 관련
	UPROPERTY(BlueprintReadWrite)
	TArray<FStageData> StageDataArray;

	UFUNCTION(BlueprintCallable, Category = "Stage Data")
	void SetStageAttemptsAtIndex(int32 Index, int32 NewAttempts);
};
