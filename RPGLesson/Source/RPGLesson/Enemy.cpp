// Fill out your copyright notice in the Description page of Project Settings.


#include "Enemy.h"
#include "Components/SphereComponent.h"
#include "AIController.h"
#include "EnemyAIController.h"
#include "RPGLessonCharacter.h"
#include "Kismet/KismetSystemLibrary.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Engine/EngineTypes.h"



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

	// INITIALIZING ENUMS 
	EnemyMovementStatus = EEnemyMovementStatus::EMS_Walking;
	
	

    // INITIALIZING BOOLS 
	bCharacterSpottedByEnemy = false; // Is an enemy "see" and ready to attack our character (changes to "true" if we reach the necessary agro distance)
	bCharInsideAgroSphere = false; // Do we collide with the Enemy Agro sphere? Nope, at least at the beginning
	bIsAttackingTarget = false; // the Enemy is not attacking us 
	bCharInsideCombatSphere = false; // Same to the AgroSphere
	bCanMoveToTarget = false; // The Enemy has to wait some time between Detected and MoveTo states.

	// INITIALIZING STATS

	// Depends on current mode 
	MaxHealth = 1000.f;
	CurrentHealth = 1000.f;
	Damage = 50.f;

	AcceptableDistance = 250.f;
	CurrentLocation = GetActorLocation();
}

// Called when the game starts or when spawned
void AEnemy::BeginPlay()
{
	Super::BeginPlay();
	
	// EAIController = Cast<AEnemyAIController>(GetController());;

	// Initializing overlap spheres
	AgroSphere -> OnComponentBeginOverlap.AddDynamic(this,&AEnemy::AgroSphereOnOverlapBegin);
	CombatSphere -> OnComponentBeginOverlap.AddDynamic(this,&AEnemy::CombatSphereOnOverlapBegin);

	AgroSphere->OnComponentEndOverlap.AddDynamic(this,&AEnemy::AgroSphereOnOverlapEnd);
	CombatSphere->OnComponentEndOverlap.AddDynamic(this,&AEnemy::CombatSphereOnOverlapEnd);
}

/* Spheres Overlapping /StopOverlapping*/
/************************************************************************************************************/
void AEnemy::AgroSphereOnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
    UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if(OtherActor)
	{
		CharacterToMove = Cast<ARPGLessonCharacter>(OtherActor);
		if(CharacterToMove)
		{
			bCharInsideAgroSphere = true;
		}
	}
}

void AEnemy::AgroSphereOnOverlapEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
    UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	if(OtherActor)
	{
		CharacterToMove = Cast<ARPGLessonCharacter>(OtherActor);
		if(CharacterToMove)
		{
			bCharInsideAgroSphere = false;
		}
	}
}

void AEnemy::CombatSphereOnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
    UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if(OtherActor)
	{
		CharacterToMove = Cast<ARPGLessonCharacter>(OtherActor);
		if(CharacterToMove)
		{
			bCharInsideCombatSphere = true;
		}
	}
}

void AEnemy::CombatSphereOnOverlapEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
    UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	if(OtherActor)
	{
		CharacterToMove = Cast<ARPGLessonCharacter>(OtherActor);
		if(CharacterToMove)
		{
			bCharInsideCombatSphere = false;
		}
	}
}

// // Updating enemy states, called in Tick function. 
// void AEnemy::EnemyStatusUpdating(float DeltaValue)
// {
// 	switch (EnemyMovementStatus)
// 	{
// 		/* EMS_IDLE *///***********************************************************************************/
// 		case EEnemyMovementStatus::EMS_Idle:
//
// 			if(EAIController) EAIController->StopMovement();
// 		
//
// 		if(bCharacterSpottedByEnemy && bCharInsideAgroSphere && !bCharInsideCombatSphere)
// 		{
// 			SetEnemyMovementStatus(EEnemyMovementStatus::EMS_CharacterDetected);
// 		
// 			// Timer which allows to play an animation before the Enemy starts moving to the Character
// 			GetWorld()->GetTimerManager().SetTimer(DelayTimerBetweenStates,this, &AEnemy::ResetTimer,2.90f,false);
// 		}
//
// 		// The Character isn't inside the agro/combat radius. 
// 		else if (!bCharInsideAgroSphere && !bCharInsideCombatSphere )
// 		{
// 			bCanMoveToTarget = false;
// 			SetEnemyMovementStatus(EEnemyMovementStatus::EMS_Walking);	
// 		}
// 		
// 		break;
//
// 		/* EMS_Walking *//***********************************************************************************/
//         case EEnemyMovementStatus::EMS_Walking:
// 		
//         //TODO: move to random location using navdata
//
// 		/*DirectionToMove = CurrentLocation.GetSafeNormal() ;
//   
// 		// GoalLocationToMove = DirectionToMove*AcceptableDistance;
//   
// 		// EAIController->MoveToLocation(DirectionToMove,250.f); */
//  
// 		if(bCharInsideAgroSphere)
// 			
// 			SetEnemyMovementStatus(EEnemyMovementStatus::EMS_Idle);
// 		
//
// 		else
// 			
// 			SetEnemyMovementStatus(EEnemyMovementStatus::EMS_Walking);
// 		
// 		break;
//
// 		/* EMS_CharacterDetected *//***********************************************************************************/
// 		case EEnemyMovementStatus::EMS_CharacterDetected:
//
// 		
// 		if(bCharInsideAgroSphere && !bCharInsideCombatSphere && bCanMoveToTarget)
// 			
// 				SetEnemyMovementStatus(EEnemyMovementStatus::EMS_MoveToTarget);
// 		
// 		
// 		else
// 		{
// 			if(!bCharInsideAgroSphere)
// 				SetEnemyMovementStatus(EEnemyMovementStatus::EMS_Idle);
// 			
// 			else if(bCharInsideCombatSphere)
// 				SetEnemyMovementStatus(EEnemyMovementStatus::EMS_Attacking);
// 		}
// 		
// 		break;
//
// 		/* EMS_MoveToTarget *//***********************************************************************************/
// 		case EEnemyMovementStatus::EMS_MoveToTarget:
//
// 		StopAttackingTarget(); // bIsAttackingTarget = false;
// 		bCanMoveToTarget = true;
//
// 		// Checking if we inside the Combat / Agro sphere. If so, the status will change to the necessary one. 
// 		if(bCharInsideCombatSphere)
// 		{
// 			SetEnemyMovementStatus(EEnemyMovementStatus::EMS_Attacking);
// 		}
// 		else if (bCharInsideAgroSphere && !bCharInsideCombatSphere)
// 		{
// 				MoveToTarget(CharacterToMove);
// 		}
// 		else if(!bCharInsideAgroSphere && !bCharInsideCombatSphere)
// 		{
// 			SetEnemyMovementStatus(EEnemyMovementStatus::EMS_Idle);
// 		}
// 		else
// 		{
// 			SetEnemyMovementStatus(EEnemyMovementStatus::EMS_Idle);
// 		}
// 		break;
//
// 		/* EMS_Attacking *//***********************************************************************************/
// 		case EEnemyMovementStatus::EMS_Attacking:
// 		
// 		if (bCharInsideCombatSphere)
// 		{
// 			AttackingTarget(); //bIsAttacking = true;
// 		}
// 		else if (!bCharInsideCombatSphere && bCharInsideAgroSphere)
// 		{
// 			SetEnemyMovementStatus(EEnemyMovementStatus::EMS_MoveToTarget);
// 		}
// 		else if (!bCharInsideCombatSphere && !bCharInsideAgroSphere)
// 		{
// 			SetEnemyMovementStatus(EEnemyMovementStatus::EMS_Idle);
// 		}
// 		
// 		break;
//
// 		default:
// 		break;
// 	}
// }
//
// Called every frame
void AEnemy::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	
	// EnemyStatusUpdating(DeltaTime);
}

// Called to bind functionality to input
void AEnemy::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
}

// void AEnemy::MoveToTarget(ARPGLessonCharacter* Character)
// {
// 	SetEnemyMovementStatus(EEnemyMovementStatus::EMS_MoveToTarget);
// 		if(EAIController)
// 		{
// 			if(Character)
// 			{
// 				EAIController->MoveToTheMainCharacter(Character);
// 			}
// 			
// 			/* Draw debug sphere which shows the shortest path from Enemy to the main Character */
// 		  
// 			/*auto PathPoints = NavigationPath ->GetPathPoints();
// 			for (auto Point : PathPoints)
// 			{
// 			FVector Location = Point.Location;
// 				UKismetSystemLibrary::DrawDebugSphere(this,Location,25.f,
// 			 		8,FLinearColor::Blue,2.5f,1.5f);
// 			};*/
// 		}
// }
//
//



// Toggles to control the Enemies reaction to the Character (depends on how far we are from the enemy)
void AEnemy::TargetDetected()
{
	bCharacterSpottedByEnemy = true;
	
}

void AEnemy::TargetLost()
{
	bCharacterSpottedByEnemy = false;
	
}

void AEnemy::AttackingTarget()
{
	bIsAttackingTarget = true;
	AttackTheCharacter();
}

void AEnemy::StopAttackingTarget()
{
	bIsAttackingTarget = false;
}

void AEnemy::AttackTheCharacter()
{
	bIsAttackingTarget = true;
	UAnimInstance*AnimInstance=GetMesh()->GetAnimInstance();
	if(AnimInstance && EnemyCombatMontage)
	{

		int32 MontageSection = FMath::RandRange(0,3);
		switch (MontageSection)
		{
		case 0:
			AnimInstance->Montage_Play(EnemyCombatMontage, 1.0f);
			AnimInstance->Montage_JumpToSection(FName("Attack_1"), EnemyCombatMontage);
			break;
				
		case 1:
			AnimInstance->Montage_Play(EnemyCombatMontage, 1.0f);
			AnimInstance->Montage_JumpToSection(FName("Attack_2"), EnemyCombatMontage);
			break;
				
		case 2:
			AnimInstance->Montage_Play(EnemyCombatMontage, 1.0f);
			AnimInstance->Montage_JumpToSection(FName("Attack_3"), EnemyCombatMontage);
			break;
				
		case 3:
			AnimInstance->Montage_Play(EnemyCombatMontage, 1.0f);
			AnimInstance->Montage_JumpToSection(FName("Attack_4"), EnemyCombatMontage);
			break;
				
		default:
			break;
		}
			
	}
	
}

void AEnemy::AttackEnd()
{

}
