// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/AudioComponent.h"
#include "Sound/SoundWave.h"
#include "Sound/SoundWaveProcedural.h"
#include "Sound/SoundClass.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "Kismet/GameplayStatics.h"
#include "Engine/World.h"
#include "AIGUSoundLoader.generated.h"

/**
 * 
 */
UCLASS()
class AIGROOVE_API UAIGUSoundLoader : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()
	
public:

	UFUNCTION(BlueprintCallable, Category = "Sound")
	static USoundWave* LoadWavFromDisk(const FString& FilePath);

	UFUNCTION(BlueprintCallable, Category = "Sound")
	static USoundWave* PrepareSoundWaveWithClass(USoundWave* SoundWave, USoundClass* SoundClass);

	UFUNCTION(BlueprintCallable, Category = "Sound")
	static void UploadMusicToServer(const FString& FilePath, const FString& ServerURL);

private:

	// 로드된 데이터를 기반으로 SoundWave를 생성
	static USoundWave* CreateSoundWaveFromWavData(const TArray<uint8>& WavData);
};
