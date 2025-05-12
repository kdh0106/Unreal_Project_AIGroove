#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "Http.h"
#include "FNoticeData.h"
#include "AIGNoticeAPIService.generated.h"

UCLASS(Blueprintable)
class AIGROOVE_API UAIGNoticeAPIService : public UObject
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, Category = "HTTP|Notice")
	void SendGetNoticeRequest(class UUserWidget* TargetWidget);  // ���� ���� ����
private:
	void OnNoticeResponseReceived(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful);

	UPROPERTY()
	TWeakObjectPtr<UUserWidget> StoredWidget;  // ���� ����
};
