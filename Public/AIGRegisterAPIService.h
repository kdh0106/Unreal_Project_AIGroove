// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "Http.h"
#include "AIGRegisterAPIService.generated.h"

/**
 * 
 */
UCLASS(Blueprintable)
class AIGROOVE_API UAIGRegisterAPIService : public UObject
{
	GENERATED_BODY()
	
public:
	UFUNCTION(BlueprintCallable, Category = "HTTP|Register")
	void SendRegisterRequest(const FString& Username, const FString& Password, const FString& Nickname, const FString& Email);

private:
	void OnRegisterResponseReceived(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful);
};
