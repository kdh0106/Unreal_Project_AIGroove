// Fill out your copyright notice in the Description page of Project Settings.


#include "AIGPawn.h"
#include "GameFramework/FloatingPawnMovement.h"
#include "Components/InputComponent.h"

// Sets default values
AAIGPawn::AAIGPawn()
{
	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	BoxCollision = CreateDefaultSubobject<UBoxComponent>(TEXT("BoxCollision"));
	Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
	RootComponent = BoxCollision;
	Mesh->SetupAttachment(RootComponent);
	Mesh->SetWorldScale3D(FVector(1.0f, 1.0f, 1.0f));

	BoxCollision->SetBoxExtent(FVector(50.0f, 50.0f, 50.0f));
	BoxCollision->SetCollisionProfileName(TEXT("AIGPawn"));
	BoxCollision->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	BoxCollision->OnComponentBeginOverlap.AddDynamic(this, &AAIGPawn::OnOverlapBegin);

	static ConstructorHelpers::FObjectFinder<UStaticMesh>MeshAsset(TEXT("/Engine/BasicShapes/Cube"));
	if (MeshAsset.Succeeded())
	{
		Mesh->SetStaticMesh(MeshAsset.Object);
	}

	SpringArm = CreateDefaultSubobject<USpringArmComponent>(TEXT("SPRINGARM"));
	SpringArm->SetupAttachment(RootComponent);
	SpringArm->TargetArmLength = 4000.0f;
	SpringArm->SetRelativeRotation(FRotator(-55.0f, 0.0f, 0.0f)); // 위에서 약간 기울여 보기 => -55.0f, 0.0f, 0.0f
	SpringArm->bEnableCameraLag = true; // 부드러운 움직임 적용
	SpringArm->CameraLagSpeed = 5.0f;
	SpringArm->bDoCollisionTest = false;

	Camera = CreateDefaultSubobject<UCameraComponent>(TEXT("CAMERA"));
	Camera->SetupAttachment(SpringArm);
	Camera->FieldOfView = 65.0f;

	// Pawn이 회전해도 SpringArm의 방향이 유지되도록 설정
	SpringArm->bUsePawnControlRotation = false;
	SpringArm->bInheritYaw = false;
	SpringArm->bInheritPitch = false;
	SpringArm->bInheritRoll = false;

	TargetRotation = FRotator(0.0f, 45.0f, 0.0f);

	static ConstructorHelpers::FClassFinder<AActor>CubeBPClass(TEXT("/Game/HandMade/Blueprint/BP_AIGTrailCube.BP_AIGTrailCube_C"));
	if (CubeBPClass.Class != nullptr)
	{
		CubeBlueprint = CubeBPClass.Class;
	}

}

// Called when the game starts or when spawned
void AAIGPawn::BeginPlay()
{
	Super::BeginPlay();

	SetActorRotation(TargetRotation);

	LastSpawnLocation = GetActorLocation();
}

// Called every frame
void AAIGPawn::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	FVector ForwardDirection = GetActorForwardVector();
	FVector NewLocation = GetActorLocation() + (ForwardDirection * MoveSpeed * DeltaTime);
	SetActorLocation(NewLocation);

	FActorSpawnParameters SpawnParams;
	AActor* SpawnedCube = GetWorld()->SpawnActor<AActor>(CubeBlueprint, GetActorLocation(), GetActorRotation(), SpawnParams);

	if (SpawnedCube)
	{
		SpawnedCube->SetLifeSpan(5.0f);
	}

}

void AAIGPawn::Turn()
{
	RotationDirection *= -1;

	FRotator NewRotation = GetActorRotation();
	NewRotation.Yaw += 90.0f * RotationDirection;

	SetActorRotation(NewRotation);

	//CreateTrailCube();

	ABLOG(Warning, TEXT("Turn!!"));
}

void AAIGPawn::OnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (OtherActor && OtherActor != this)
	{
		ABLOG(Warning, TEXT("Overlap! This Comp: %s, OtherActor: %s, OtherComp: %s"),
			*OverlappedComponent->GetName(),
			*OtherActor->GetName(),
			*OtherComp->GetName());

		if (OtherActor->ActorHasTag(TEXT("PopupTrigger")))
		{
			ABLOG(Warning, TEXT("Overlap with the fuckin trigger"));
		}
		else
		{
			ABLOG(Warning, TEXT("overlap with the ssibal others"));
			MoveSpeed = 0.0f;

			UAIGGameInstance* GI = Cast<UAIGGameInstance>(UGameplayStatics::GetGameInstance(GetWorld()));
			if (GI)
			{
				GI->OnPlayerDeath();
			}
		}
	}
}
// Called to bind functionality to input
void AAIGPawn::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	PlayerInputComponent->BindAction("Turn", IE_Pressed, this, &AAIGPawn::Turn);
}
