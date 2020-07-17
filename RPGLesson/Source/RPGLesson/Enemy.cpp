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

	AgroSphere = CreateDefaultSubobject<USphereComponent>(FName("AgroSphere"));
	AgroSphere ->SetupAttachment(GetRootComponent());
	AgroSphere->InitSphereRadius(600.f);

	CombatSphere = CreateDefaultSubobject<USphereComponent>(FName("CombatSphere"));
	CombatSphere ->SetupAttachment(GetRootComponent());
	CombatSphere->InitSphereRadius(80.f);
}

void AEnemy::SetEnemyMovementStatus(EEnemyMovementStatus Status)
{
	Status = EnemyMovementStatus;
	switch (EnemyMovementStatus)
	{
		case EEnemyMovementStatus::EMS_Idle:
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
		MoveRequest.SetAcceptanceRadius(35.0f);

		FNavPathSharedPtr NavigationPath;

		AIController->MoveTo(MoveRequest, &NavigationPath);

		// // Draw debug sphere which shows the shortest path from Enemy to the main Character
		//
		// auto PathPoints = NavigationPath ->GetPathPoints();
		// for (auto Point : PathPoints)
		// {
		// 	FVector Location = Point.Location;
		// 	UKismetSystemLibrary::DrawDebugSphere(this,Location,25.f,
		// 		8,FLinearColor::Blue,10.f,1.5f);
		// }
	}
}

