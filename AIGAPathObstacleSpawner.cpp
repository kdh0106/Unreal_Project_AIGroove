// Fill out your copyright notice in the Description page of Project Settings.


#include "AIGAPathObstacleSpawner.h"
#include "AIGGameInstance.h"
#include "Engine/World.h"
#include "Kismet/GameplayStatics.h"



// Sets default values
AAIGAPathObstacleSpawner::AAIGAPathObstacleSpawner()
{
    // Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
    PrimaryActorTick.bCanEverTick = true;
}

// Called when the game starts or when spawned
void AAIGAPathObstacleSpawner::BeginPlay()
{
    Super::BeginPlay();
}

// Called every frame
void AAIGAPathObstacleSpawner::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);

    CurrentTime += DeltaTime;

    if (GEngine)
    {
        GEngine->AddOnScreenDebugMessage(1, 0.0f, FColor::Green, FString::Printf(TEXT("Game Time: %.2f"), CurrentTime));
    }

    if (bStopObstacleSpawningAfterLoading)
    {
        return;
    }

    for (FAIGFPathObstacleData& obstacleData : ObstacleList)
    {
        if (!obstacleData.bSpawned && CurrentTime >= obstacleData.SpawnTime)
        {
            FActorSpawnParameters SpawnParams;
            AActor* SpawnObstacle = GetWorld()->SpawnActor<AActor>(
                ObstacleClass,
                obstacleData.Position,
                FRotator::ZeroRotator,
                SpawnParams
            );

            if (SpawnObstacle)
            {
                SpawnObstacle->SetActorScale3D(obstacleData.Scale);
                obstacleData.bSpawned = true;

                if (ObstacleMaterials.Num() > 0)
                {
                    int32 RandomIndex = FMath::RandRange(0, ObstacleMaterials.Num() - 1);
                    UMaterialInterface* RandomMaterial = ObstacleMaterials[RandomIndex];

                    UStaticMeshComponent* MeshComp = SpawnObstacle->FindComponentByClass<UStaticMeshComponent>();
                    if (MeshComp && RandomMaterial)
                    {
                        MeshComp->SetMaterial(0, RandomMaterial);
                    }
                }
            }
        }
    }

}

void AAIGAPathObstacleSpawner::CalculatePlayerPath(float fSongDuration)
{
    PlayerPathList.Empty();

    if (!bPathInitialized)
    {
        UE_LOG(LogTemp, Warning, TEXT("Path not initialized yet."));
        return;
    }

    FVector CurrentPosition = FixedStartPosition;
    float BaseYaw = 0.0f;

    bool bGoingRight = true;
    float CurrentYaw = bGoingRight ? BaseYaw + 45.0f : BaseYaw - 45.0f;

    const float Speed = 800.0f;
    const float TimeStep = 0.01f;
    const float DistanceStep = 50.0f;  // 일정 간격마다 포인트 저장
    float AccumulatedDistance = 0.0f;
    float TotalTime = 0.0f;

    int32 NextTurnIndex = 0;

    FVector LastStoredPosition = CurrentPosition;

    while (TotalTime <= fSongDuration)
    {
        if (NextTurnIndex < PathTurnTimes.Num() && TotalTime >= PathTurnTimes[NextTurnIndex])
        {
            bGoingRight = !bGoingRight;
            CurrentYaw = bGoingRight ? BaseYaw + 45.0f : BaseYaw - 45.0f;
            NextTurnIndex++;
        }

        FRotator MoveRotator(0.0f, CurrentYaw, 0.0f);
        FVector MoveDirection = MoveRotator.Vector();
        FVector DeltaMove = MoveDirection * Speed * TimeStep;

        CurrentPosition += DeltaMove;
        AccumulatedDistance += DeltaMove.Size();

        if (AccumulatedDistance >= DistanceStep)
        {
            PlayerPathList.Add(CurrentPosition);
            AccumulatedDistance = 0.0f;
            LastStoredPosition = CurrentPosition;
        }

        TotalTime += TimeStep;
    }

    // 마지막 포인트도 추가
    PlayerPathList.Add(CurrentPosition);


    //PlayerPathList.Empty();

    //if (!bPathInitialized)
    //{
    //    UE_LOG(LogTemp, Warning, TEXT("Path not initialized yet."));
    //    return;
    //}

    //FVector CurrentPosition = FixedStartPosition;
    //float BaseYaw = 0.0f;

    //// 회전 관련
    //bool bGoingRight = true;
    //float CurrentYaw = bGoingRight ? BaseYaw + 45.0f : BaseYaw - 45.0f;

    //// 시뮬레이션 변수
    //const float Speed = 800.0f; // 직선방향으로 가는 속도는 대각선의 속도 * 2분의 루트2 즉 0.7071 곱하면 됨.
    //const float DeltaTime = 0.1f;
    //float TotalSimulateTime = fSongDuration;

    //int32 NextTurnIndex = 0;
    //float SimulatedTime = 0.0f;

    //while (SimulatedTime <= TotalSimulateTime)
    //{
    //    if (NextTurnIndex < PathTurnTimes.Num() && SimulatedTime >= PathTurnTimes[NextTurnIndex])
    //    {
    //        bGoingRight = !bGoingRight;
    //        CurrentYaw = bGoingRight ? BaseYaw + 45.0f : BaseYaw - 45.0f;
    //        NextTurnIndex++;
    //    }

    //    FRotator MoveRotator(0.0f, CurrentYaw, 0.0f);
    //    FVector MoveDirection = MoveRotator.Vector();

    //    CurrentPosition += MoveDirection * Speed * DeltaTime;
    //    PlayerPathList.Add(CurrentPosition);

    //    SimulatedTime += DeltaTime;
    //}
}

void AAIGAPathObstacleSpawner::GenerateObstaclesFromPath(float fSongDuration)
{
    const float WorldXMin = 0.0f;
    float WorldXMax = fSongDuration * 565.68f;
    const float WorldYMin = 3700.0f;
    const float WorldYMax = 7700.0f;
    const float GenerateInterval = 0.003f; // 장애물 생성 주기
    const float RespawnSafeZoneInterval = 0.05f;

    // SafeZone 박스의 Half Extents (2m 박스 = 200cm니까, 반은 100cm)
    FVector SafeZoneHalfSize(270.0f, 270.0f, 270.0f);

    ObstacleList.Empty();

    for (float Time = 0.0f; Time <= fSongDuration; Time += GenerateInterval)
    {
        FVector RandomPosition;
        float fZRandomSize{};
        float fXYRandomSize{};
        bool bValidPosition = false;

        for (int32 Attempt = 0; Attempt < 5; Attempt++)
        {
            float RandX = FMath::FRandRange(WorldXMin, WorldXMax);
            float RandY = FMath::FRandRange(WorldYMin, WorldYMax);
            float Z = 0.0f;

            RandomPosition = FVector(RandX, RandY, Z);
            fZRandomSize = FMath::FRandRange(3.0f, 20.0f);
            fXYRandomSize = FMath::FRandRange(2.0f, 4.5f);

            bool bInsideAnySafeZone = false;
            for (int32 i = 0; i < PlayerPathList.Num(); ++i)
            {
                const FVector& PlayerPos = PlayerPathList[i];
                float PathTime = i * GenerateInterval;
                float RespawnPathTime = i * RespawnSafeZoneInterval;

                FVector DynamicSafeZoneHalfSize = (RespawnPathTime <= 0.4f)
                    ? FVector(500.0f, 500.0f, 1000.0f)
                    : SafeZoneHalfSize;

                FBox SafeZoneBox(PlayerPos - DynamicSafeZoneHalfSize, PlayerPos + DynamicSafeZoneHalfSize);

                if (SafeZoneBox.IsInside(RandomPosition))
                {
                    bInsideAnySafeZone = true;
                    break;
                }
            }

            if (!bInsideAnySafeZone)
            {
                bValidPosition = true;
                break;
            }
        }

        if (bValidPosition)
        {
            FAIGFPathObstacleData NewObstacle;
            NewObstacle.SpawnTime = Time;
            NewObstacle.Position = RandomPosition;
            NewObstacle.Scale = FVector(fXYRandomSize, fXYRandomSize, fZRandomSize); // 장애물 크기
            NewObstacle.bSpawned = false;

            ObstacleList.Add(NewObstacle);
        }
    }

    UE_LOG(LogTemp, Log, TEXT("Generated %d obstacles with FBox SafeZone protection"), ObstacleList.Num());
}

void AAIGAPathObstacleSpawner::SetInitialTransform(const FVector& InPosition, float InYaw)
{
    FixedStartPosition = InPosition;
    FixedBaseYaw = InYaw;
    bPathInitialized = true;

    if (GEngine)
    {
        GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Cyan,
            FString::Printf(TEXT("Transform: %s | Yaw: %.2f"), *InPosition.ToString(), InYaw));
    }
}

void AAIGAPathObstacleSpawner::SetSelectedStage(const FStageData& InStageData)
{
    SelectedStageData = InStageData;

    if (GEngine)
    {
        FString DebugMessage = FString::Printf(TEXT("SetSelectedStage: %s | JSON: %s"),
            *InStageData.SongTitle, *InStageData.JSONPath);

        GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Yellow, DebugMessage);
    }
}

void AAIGAPathObstacleSpawner::ResetLevel()
{
    CurrentTime = 0.0f;
    PlayerPathList.Empty(); // 기존 스폰리스트 비우기
    ObstacleList.Empty();

    FPathTimingData LoadedData;

    if (LoadPathTimingData(SelectedStageData.JSONPath, LoadedData))
    {
        PathTurnTimes = LoadedData.TurnTimes;
    }

    if (GEngine)
    {
        GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Magenta,
            FString::Printf(TEXT("TurnTimes count: %d"), LoadedData.TurnTimes.Num()));
    }
}

void AAIGAPathObstacleSpawner::DrawSafeZones()
{
    const float DeltaTime = 0.1f; // 경로 생성 간격 (시뮬레이션 기준)

    for (int32 i = 0; i < PlayerPathList.Num(); ++i)
    {
        const FVector& PlayerPos = PlayerPathList[i];
        float PathTime = i * DeltaTime;

        //구간별 SafeZone 크기 및 색상 설정
        FVector ZoneSize = (PathTime <= 0.4f)
            ? FVector(500.0f, 500.0f, 250.0f)
            : FVector(50.0f, 50.0f, 50.0f); // 기존 크기

        FColor ZoneColor = (PathTime <= 0.4f) ? FColor::Red : FColor::Green;

        DrawDebugBox(
            GetWorld(),
            PlayerPos,
            ZoneSize,
            ZoneColor,
            false,
            300.0f,
            0,
            5.0f
        );
    }
}

bool AAIGAPathObstacleSpawner::LoadPathTimingData(const FString& JsonPath, FPathTimingData& OutData)
{
    FString FilePath = JsonPath;
    FString JsonString;
    FString LoadFailDebugMessage = FString::Printf(TEXT("JSON: %s"), *FilePath);

    GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Blue, LoadFailDebugMessage);

    if (!FFileHelper::LoadFileToString(JsonString, *FilePath))
    {
        UE_LOG(LogTemp, Error, TEXT("Failed to load JSON file: %s"), *FilePath);
        GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Red, "JSON LOAD FAIL");
        return false;
    }

    return FJsonObjectConverter::JsonObjectStringToUStruct(JsonString, &OutData, 0, 0);
}


