#include "AIGLogoutAPIService.h"
#include "HttpModule.h"
#include "Interfaces/IHttpResponse.h"
#include "Kismet/GameplayStatics.h"
#include "AIGGameInstance.h"

void UAIGLogoutAPIService::SendLogoutRequest()
{
    UE_LOG(LogTemp, Warning, TEXT("Sending logout request..."));

    // Get token from GameInstance
    UWorld* World = GEngine->GetWorldFromContextObjectChecked(this);
    UAIGGameInstance* GI = Cast<UAIGGameInstance>(UGameplayStatics::GetGameInstance(World));
    if (!GI)
    {
        UE_LOG(LogTemp, Error, TEXT("GameInstance not found!"));
        return;
    }

    FString Token = GI->GetAuthToken();
    if (Token.IsEmpty())
    {
        UE_LOG(LogTemp, Error, TEXT("Auth token is missing!"));
        return;
    }

    // Setup HTTP Request
    TSharedRef<IHttpRequest, ESPMode::ThreadSafe> Request = FHttpModule::Get().CreateRequest();
    Request->SetURL(TEXT("http://ceprj.gachon.ac.kr:60002/game/user/logout")); // 예시 URL
    Request->SetVerb(TEXT("POST"));
    Request->SetHeader(TEXT("Content-Type"), TEXT("application/json"));
    Request->SetHeader(TEXT("Authorization"), FString::Printf(TEXT("Bearer %s"), *Token));

    Request->OnProcessRequestComplete().BindUObject(this, &UAIGLogoutAPIService::OnLogoutResponseReceived);
    Request->ProcessRequest();
}

void UAIGLogoutAPIService::OnLogoutResponseReceived(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful)
{
    if (!bWasSuccessful || !Response.IsValid())
    {
        UE_LOG(LogTemp, Error, TEXT("Logout request failed (Network error)"));
        return;
    }

    int32 StatusCode = Response->GetResponseCode();
    FString ResponseBody = Response->GetContentAsString();

    if (StatusCode == 200)
    {
        UE_LOG(LogTemp, Warning, TEXT("Logout successful!"));

        // 선택적으로 GameInstance의 토큰을 제거할 수 있음
        if (UWorld* World = GEngine->GetWorldFromContextObjectChecked(this))
        {
            UAIGGameInstance* GI = Cast<UAIGGameInstance>(UGameplayStatics::GetGameInstance(World));
            if (GI)
            {
                GI->SetAuthToken(TEXT(""));
                GI->SetUserName(TEXT(""));
                GI->ShowLoginScreen();

                FString CurrentToken = GI->GetAuthToken();
                FString CurrentUserName = GI->GetUserName();

                ABLOG(Warning, TEXT("Current Token : %s"), *CurrentToken);
                ABLOG(Warning, TEXT("Current UserName : %s"), *CurrentUserName);
            }
        }
    }
    else
    {
        UE_LOG(LogTemp, Error, TEXT("Logout failed [%d] - %s"), StatusCode, *ResponseBody);
    }
}
