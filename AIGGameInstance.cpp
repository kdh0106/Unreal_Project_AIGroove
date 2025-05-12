// Fill out your copyright notice in the Description page of Project Settings.


#include "AIGGameInstance.h"

void UAIGGameInstance::SetAuthToken(const FString& InToken)
{
	AuthToken = InToken;
}

FString UAIGGameInstance::GetAuthToken() const
{
	return AuthToken;
}

void UAIGGameInstance::SetUserName(const FString& InUserName)
{
	UserName = InUserName;
}

FString UAIGGameInstance::GetUserName() const
{
	return UserName;
}

void UAIGGameInstance::PlayStageMusic(USoundBase* Music)
{
    if (!StageMusicComponent)
    {
        StageMusicComponent = NewObject<UAudioComponent>(this);
        StageMusicComponent->RegisterComponentWithWorld(GetWorld());
    }

    StageMusicComponent->SetSound(Music);
    StageMusicComponent->Play();
}

void UAIGGameInstance::StopStageMusic()
{
    if (StageMusicComponent && StageMusicComponent->IsPlaying())
    {
        StageMusicComponent->Stop();
    }
}

void UAIGGameInstance::EnsureAIGFoldersExist()
{
    FString BaseDir = FPaths::ProjectPersistentDownloadDir();  // /Android/data/.../files/

    FString ThumbnailDir = BaseDir / TEXT("AIGAssets/Thumbnails/");
    FString TimingDataDir = BaseDir / TEXT("AIGAssets/TimingData/");
    FString AudioDir = BaseDir / TEXT("AIGAssets/Audio/");

    IPlatformFile& PlatformFile = FPlatformFileManager::Get().GetPlatformFile();

    if (!PlatformFile.DirectoryExists(*ThumbnailDir))
        PlatformFile.CreateDirectoryTree(*ThumbnailDir);

    if (!PlatformFile.DirectoryExists(*TimingDataDir))
        PlatformFile.CreateDirectoryTree(*TimingDataDir);

    if (!PlatformFile.DirectoryExists(*AudioDir))
        PlatformFile.CreateDirectoryTree(*AudioDir);

    FString DebugJSONPath = TimingDataDir / TEXT("hello_TimingData.json");

    if (!PlatformFile.FileExists(*DebugJSONPath))
    {
        FString JsonContent = R"({
  "TurnTimes": [
    1.75, 3.02, 4.65, 6.33, 7.49, 9.18, 11.01, 12.62, 14.31, 16.12,
    17.77, 19.55, 21.24, 23.01, 24.76, 26.44, 28.37, 30.12, 31.93, 33.55,
    35.41, 37.13, 39.01, 40.77, 42.39, 44.26, 46.17, 47.88, 49.76, 51.48,
    53.25, 55.03, 56.88, 58.47, 60.26, 62.11, 63.89, 65.63, 67.41, 69.16,
    70.94, 72.72, 74.56, 76.42, 78.13, 79.81, 81.69, 83.44, 85.17, 86.89,
    88.66, 90.34, 92.12, 93.81, 95.69, 97.48, 99.16, 100.84, 102.69, 104.53,
    106.22, 107.98, 109.67, 111.49, 113.23, 115.04, 116.76, 118.41, 120.23, 121.94,
    123.66, 125.42, 127.13, 128.84, 130.59, 132.27, 134.01, 135.82, 137.56, 139.27,
    141.06, 142.78, 144.49, 146.23, 148.01, 149.67, 151.44, 153.27, 155.01, 156.72,
    158.47, 160.28, 162.01, 163.76, 165.55, 167.23, 168.99, 170.82, 172.63, 174.42,
    176.23
  ]
})";

        if (FFileHelper::SaveStringToFile(JsonContent, *DebugJSONPath))
        {
            UE_LOG(LogTemp, Log, TEXT("hello_TimingData.json created at: %s"), *DebugJSONPath);
            GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Blue, "DEBUG JSON SAVE COMPLETE");
        }
        else
        {
            UE_LOG(LogTemp, Error, TEXT("Failed to create hello_TimingData.json"));
            GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Red, "DEBUG JSON SAVE FAILED");
        }
    }

    UE_LOG(LogTemp, Log, TEXT("AIG Runtime Folder Create Complete"));
}

bool UAIGGameInstance::CopySelectedWavToAudioFolder(const FString& SourcePath, const FString& SongTitle)
{
    FString TargetDir = FPaths::ProjectPersistentDownloadDir() / TEXT("AIGAssets/Audio");

    //확장자 추출 (ex: .wav)
    FString Extension = FPaths::GetExtension(SourcePath, true);  // true → . 포함
    FString TargetPath = TargetDir / (SongTitle + Extension);

    IPlatformFile& PlatformFile = FPlatformFileManager::Get().GetPlatformFile();

    if (!PlatformFile.CopyFile(*TargetPath, *SourcePath))
    {
        UE_LOG(LogTemp, Error, TEXT("WAV Copy Failed!\nFrom: %s\nTo  : %s"), *SourcePath, *TargetPath);
        return false;
    }

    UE_LOG(LogTemp, Log, TEXT("WAV Copy Success!\nFrom: %s\nTo  : %s"), *SourcePath, *TargetPath);
    return true;
}

void UAIGGameInstance::SetStageAttemptsAtIndex(int32 Index, int32 NewAttempts)
{
    if (!StageDataArray.IsValidIndex(Index))
    {
        UE_LOG(LogTemp, Warning, TEXT("Invalid index %d for StageDataArray"), Index);
        return;
    }

    StageDataArray[Index].Attempts = NewAttempts;

    UE_LOG(LogTemp, Log, TEXT("Updated StageDataArray[%d].Attempts to %d"), Index, NewAttempts);
    GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Blue, "Attempts Update O");
}

//다른 c++ 클래스에서 Token 받아올 때
//UAIGGameInstance* GI = Cast<UAIGGameInstance>(UGameplayStatics::GetGameInstance(GetWorld()));
//if (GI)
//{
//	FString Token = GI->GetAuthToken();
//}

//블루프린트에서 Token 받아올 떄
//Get Game Instance → Cast to BP_UIManager (or AIGGameInstance) → Get AuthToken