#pragma once

#include "AIGroove.h"
#include "UObject/NoExportTypes.h"
#include "Http.h"
#include "AIGLogoutAPIService.generated.h"

UCLASS(Blueprintable)
class AIGROOVE_API UAIGLogoutAPIService : public UObject
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, Category = "HTTP|Logout")
	void SendLogoutRequest();

private:
	void OnLogoutResponseReceived(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful);
};
