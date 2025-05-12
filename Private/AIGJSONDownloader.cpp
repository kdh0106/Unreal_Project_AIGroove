// Fill out your copyright notice in the Description page of Project Settings.

#include "AIGJSONDownloader.h"
#include "HttpModule.h"
#include "Interfaces/IHttpResponse.h"
#include "Http.h"
#include "Misc/FileHelper.h"
#include "Misc/Paths.h"
#include "HAL/PlatformFilemanager.h"

void UAIGJSONDownloader::DownloadJSONToTimingDataFolder(const FString& URL, const FString& FileName)
{
    UE_LOG(LogTemp, Log, TEXT("Downloading JSON from URL: %s"), *URL);

    TSharedRef<IHttpRequest, ESPMode::ThreadSafe> Request = FHttpModule::Get().CreateRequest();
    Request->SetURL(URL);
    Request->SetVerb("GET");
    Request->SetHeader("Content-Type", "application/json");

    Request->OnProcessRequestComplete().BindLambda([FileName](FHttpRequestPtr RequestPtr, FHttpResponsePtr Response, bool bWasSuccessful)
        {
            if (bWasSuccessful && Response.IsValid())
            {
                FString SaveDir = FPaths::ProjectPersistentDownloadDir() / TEXT("AIGAssets/TimingData");
                IFileManager::Get().MakeDirectory(*SaveDir, true);

                FString SavePath = SaveDir / FileName;

                if (FFileHelper::SaveStringToFile(Response->GetContentAsString(), *SavePath))
                {
                    UE_LOG(LogTemp, Log, TEXT("JSON Save Complete: %s"), *SavePath);
                }
                else
                {
                    UE_LOG(LogTemp, Error, TEXT("JSON Save Failed: %s"), *SavePath);
                }
            }
            else
            {
                UE_LOG(LogTemp, Error, TEXT("JSON Download Failed"));
            }
        });

    Request->ProcessRequest();

}
