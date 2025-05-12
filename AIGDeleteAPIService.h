#pragma once

#include "AIGroove.h"
#include "UObject/NoExportTypes.h"
#include "Http.h"
#include "AIGDeleteAPIService.generated.h"

UCLASS(Blueprintable)
class AIGROOVE_API UAIGDeleteAPIService : public UObject
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, Category = "HTTP|Withdraw")
	void SendDeleteRequest();

private:
	void OnDeleteResponseReceived(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful);
};
