// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "AIGroove.h"
#include "UObject/NoExportTypes.h"
#include "Http.h"
#include "AIGLoginAPIService.generated.h"

/**
 *
 */
UCLASS(Blueprintable)
class AIGROOVE_API UAIGLoginAPIService : public UObject
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, Category = "HTTP|Login")
	void SendLoginRequest(const FString& Username, const FString& Password);

private:
	void OnLoginResponseReceived(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful);

protected:
	virtual void OnLoginSuccess(const FString& Token, const FString& UserName);
	virtual void OnLoginFail(const FString& ErrorMessage);

};
