// Fill out your copyright notice in the Description page of Project Settings.


#include "Enemy.h"
#include "Components/SphereComponent.h"
#include "AIController.h"
#include "RPGLessonCharacter.h"
#include "Kismet/KismetSystemLibrary.h"
#include "NavigationData.h"



// Sets default values
AEnemy::AEnemy()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	AgroSphere = CreateDefaultSubobject<USphereComponent>(TEXT("AgroSphere"));
	AgroSphere ->SetupAttachment(GetRootComponent());
	AgroSphere->InitSphereRadius(600.f);

	CombatSphere = CreateDefaultSubobject<USphereComponent>(TEXT("CombatSphere"));
	CombatSphere ->SetupAttachment(GetRootComponent());
	CombatSphere->InitSphereRadius(80.f);

	bCharacterInTarget = false; // Is an enemy "see" and ready to attack our character (changes to "true" if we reach the necessary agro distance) 
	
}

void AEnemy::SetEnemyMovementStatus(EEnemyMovementStatus Status)
{
	Status = EnemyMovementStatus;
	switch (EnemyMovementStatus)
	{
		case EEnemyMovementStatus::EMS_Idle:
		break;

		case EEnemyMovementStatus::EMS_DetectCharacter:
		break;
		
	    case EEnemyMovementStatus::EMS_MoveToTarget:
        break;
		
		case EEnemyMovementStatus::EMS_Attacking:
		break;
		
		default:
		break;
	}
}

// Called when the game starts or when spawned
void AEnemy::BeginPlay()
{
	Super::BeginPlay();

	AIController = Cast<AAIController>(GetController());

	AgroSphere -> OnComponentBeginOverlap.AddDynamic(this,&AEnemy::AgroSphereOnOverlapBegin);
	CombatSphere -> OnComponentBeginOverlap.AddDynamic(this,&AEnemy::CombatSphereOnOverlapBegin);

	AgroSphere->OnComponentEndOverlap.AddDynamic(this,&AEnemy::AgroSphereOnOverlapEnd);
	CombatSphere->OnComponentEndOverlap.AddDynamic(this,&AEnemy::CombatSphereOnOverlapEnd);
}

// Called every frame
void AEnemy::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

// Called to bind functionality to input
void AEnemy::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
}

void AEnemy::AgroSphereOnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if(OtherActor)
	{
		ARPGLessonCharacter*Character=Cast<ARPGLessonCharacter>(OtherActor);
		if(Character)
		{
			SetEnemyMovementStatus(EEnemyMovementStatus::EMS_DetectCharacter);
			MoveToTarget(Character);
		}
	}
}

void AEnemy::AgroSphereOnOverlapEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
}

void AEnemy::CombatSphereOnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if(OtherActor)
	{
		ARPGLessonCharacter*Character = Cast<ARPGLessonCharacter>(OtherActor);
		if(Character)
		{
			SetEnemyMovementStatus(EEnemyMovementStatus::EMS_Attacking);
		}
		
	}
}

void AEnemy::CombatSphereOnOverlapEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
}

void AEnemy::MoveToTarget(ARPGLessonCharacter* Character)
{
	SetEnemyMovementStatus(EEnemyMovementStatus::EMS_MoveToTarget);
	
	if(AIController)
	{
		/* Creating a FAIMoveRequest/FNavPathSharePtr to feed them to MoveTo() function */
		FAIMoveRequest MoveRequest;
		MoveRequest.SetGoalActor(Character);
		MoveRequest.SetAcceptanceRadius(10.0f);

		FNavPathSharedPtr NavigationPath;

		AIController->MoveTo(MoveRequest, &NavigationPath);

		// // Draw debug sphere which shows the shortest path from Enemy to the main Character
		//
		// /*auto PathPoints = NavigationPath ->GetPathPoints();
		// for (auto Point : PathPoints)
		// {
		// FVector Location = Point.Location;
		// 	UKismetSystemLibrary::DrawDebugSphere(this,Location,25.f,
		//  		8,FLinearColor::Blue,10.f,1.5f);
		// };*/
	}
}

// Toggles to control the Enemies reaction to the Character (depends on how far we are from the enemy)
void AEnemy::DetectCharacter()
{
	bCharacterInTarget = true;
	SetEnemyMovementStatus(EEnemyMovementStatus::EMS_DetectCharacter);
}

void AEnemy::LostCharacter()
{
	bCharacterInTarget = false;
	SetEnemyMovementStatus(EEnemyMovementStatus::EMS_Idle);
}

