// Fill out your copyright notice in the Description page of Project Settings.


#include "AIGObstacleLevelLoader.h"
#include "Misc/FileHelper.h"
#include "Misc/Paths.h"
#include "Serialization/JsonReader.h"
#include "Serialization/JsonSerializer.h"

bool FObstacleLevelLoader::LoadLevelDataFromJson(const FString& FilePath, TArray<FObstacleData>& OutObstacles)
{
    FString FullPath = FPaths::ProjectContentDir() + FilePath;
    FString JsonString;

    if (!FFileHelper::LoadFileToString(JsonString, *FullPath))
    {
        UE_LOG(LogTemp, Error, TEXT("Failed to load JSON file at path: %s"), *FullPath);
        GEngine->AddOnScreenDebugMessage(-1, 10.f, FColor::Cyan, TEXT("Failed to load JSON file at path"));
        return false;
    }

    TSharedRef<TJsonReader<>> Reader = TJsonReaderFactory<>::Create(JsonString);
    TArray<TSharedPtr<FJsonValue>> JsonArray;

    if (!FJsonSerializer::Deserialize(Reader, JsonArray))
    {
        //UE_LOG(LogTemp, Error, TEXT("Failed to parse JSON."));
        GEngine->AddOnScreenDebugMessage(-1, 10.f, FColor::Cyan, TEXT("Failed to parse JSON"));

        return false;
    }

    for (const TSharedPtr<FJsonValue>& Value : JsonArray)
    {
        TSharedPtr<FJsonObject> Obj = Value->AsObject();
        if (!Obj.IsValid()) continue;

        FObstacleData Obstacle;

        Obstacle.SpawnTime = Obj->GetNumberField("time");

        // Position
        TSharedPtr<FJsonObject> Pos = Obj->GetObjectField("position");
        Obstacle.Position.X = Pos->GetNumberField("x");
        Obstacle.Position.Y = Pos->GetNumberField("y");
        Obstacle.Position.Z = Pos->GetNumberField("z");

        // Size/Scale
        TSharedPtr<FJsonObject> Size = Obj->GetObjectField("size");
        Obstacle.Scale.X = Size->GetNumberField("x");
        Obstacle.Scale.Y = Size->GetNumberField("y");
        Obstacle.Scale.Z = Size->GetNumberField("z");

        OutObstacles.Add(Obstacle);
    }

    UE_LOG(LogTemp, Log, TEXT("Loaded %d obstacles from JSON"), OutObstacles.Num());
    GEngine->AddOnScreenDebugMessage(-1, 10.f, FColor::Cyan, TEXT("Loaded obstacles from JSON"));

    return true;
}
