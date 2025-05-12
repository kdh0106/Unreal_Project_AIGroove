#include "AIGDeleteAPIService.h"
#include "HttpModule.h"
#include "Interfaces/IHttpResponse.h"
#include "Dom/JsonObject.h"
#include "Serialization/JsonSerializer.h"
#include "AIGGameInstance.h"
#include "Kismet/GameplayStatics.h"
#include "Engine/Engine.h"

void UAIGDeleteAPIService::SendDeleteRequest()
{
	UWorld* World = GEngine->GetWorldFromContextObjectChecked(this);
	if (!World) return;

	UAIGGameInstance* GameInstance = Cast<UAIGGameInstance>(UGameplayStatics::GetGameInstance(World));
	if (!GameInstance)
	{
		UE_LOG(LogTemp, Error, TEXT("GameInstance casting failed"));
		return;
	}

	ABLOG(Warning, TEXT("Before Delete, session check"));
	ABLOG(Warning, TEXT("UserName : %s"), *GameInstance->GetUserName());
	ABLOG(Warning, TEXT("Token : %s"), *GameInstance->GetAuthToken());

	const FString Token = GameInstance->GetAuthToken();
	const FString DeleteURL = FString::Printf(TEXT("http://ceprj.gachon.ac.kr:60002/game/user"));

	if (Token.IsEmpty())
	{
		UE_LOG(LogTemp, Error, TEXT("No Auth Token. Delete request unable"));
		return;
	}

	// HTTP 요청 구성
	TSharedRef<IHttpRequest, ESPMode::ThreadSafe> Request = FHttpModule::Get().CreateRequest();
	Request->SetURL(DeleteURL);
	Request->SetVerb(TEXT("DELETE"));
	Request->SetHeader(TEXT("Content-Type"), TEXT("application/json"));
	Request->SetHeader(TEXT("Authorization"), FString::Printf(TEXT("Bearer %s"), *Token));
	Request->SetContentAsString(TEXT("{}")); 

	Request->OnProcessRequestComplete().BindUObject(this, &UAIGDeleteAPIService::OnDeleteResponseReceived);
	Request->ProcessRequest();
}

void UAIGDeleteAPIService::OnDeleteResponseReceived(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful)
{
	if (!bWasSuccessful || !Response.IsValid())
	{
		UE_LOG(LogTemp, Error, TEXT("Network error"));
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("Delete failed - network error"));
		return;
	}

	int32 StatusCode = Response->GetResponseCode();

	if (StatusCode == 200 || StatusCode == 204)
	{
		UE_LOG(LogTemp, Warning, TEXT("Delete Success!!"));
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Green, TEXT("Delete Success!!"));

		if (UWorld* World = GEngine->GetWorldFromContextObjectChecked(this))
		{
			UAIGGameInstance* GI = Cast<UAIGGameInstance>(UGameplayStatics::GetGameInstance(World));
			if (GI)
			{
				GI->SetAuthToken(TEXT(""));
				GI->SetUserName(TEXT(""));
				GI->ShowLoginScreen();
			}
		}

		// 추가 처리: UI 리셋 또는 로그인 화면 이동 등
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("Delete failed: %d - %s"), StatusCode, *Response->GetContentAsString());
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("Delete failed!"));
	}
}
