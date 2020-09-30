// Fill out your copyright notice in the Description page of Project Settings.


#include "Enemy.h"
#include "Components/SphereComponent.h"
#include "AIController.h"

#include "RPGLessonCharacter.h"
#include "Kismet/KismetSystemLibrary.h"
#include "NavigationData.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "TimerManager.h"
#include "Actions/PawnAction_Move.h"
#include "Animation/AnimTypes.h"
#include "Engine/EngineTypes.h"
#include "Field/FieldSystemNodes.h"


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
	NormalWalkingSpeed = 400.f;
	BattleWalkingSpeed = 500.f;

	CurrentLocation = GetActorLocation();
}

// Called when the game starts or when spawned
void AEnemy::BeginPlay()
{
	Super::BeginPlay();

	/*EAIController = Cast<AEnemyAIController>(GetController());*/
	AIController = Cast<AAIController>(GetController());

	// Initializing overlap spheres
	AgroSphere -> OnComponentBeginOverlap.AddDynamic(this,&AEnemy::AgroSphereOnOverlapBegin);
	CombatSphere -> OnComponentBeginOverlap.AddDynamic(this,&AEnemy::CombatSphereOnOverlapBegin);

	AgroSphere->OnComponentEndOverlap.AddDynamic(this,&AEnemy::AgroSphereOnOverlapEnd);
	CombatSphere->OnComponentEndOverlap.AddDynamic(this,&AEnemy::CombatSphereOnOverlapEnd);
}

// Updating enemy states, called in Tick function. 
void AEnemy::EnemyStatusUpdating(float DeltaValue)
{
	DeltaValue = GetWorld()->GetDeltaSeconds();

	switch (EnemyMovementStatus)
	{
		/* EMS_IDLE *///***********************************************************************************/
		case EEnemyMovementStatus::EMS_Idle:
		// UE_LOG(LogTemp, Warning, TEXT("we are inside THE IDLE "));

		if(bCharacterSpottedByEnemy && bCharInsideAgroSphere && !bCharInsideCombatSphere)
		{
			UE_LOG(LogTemp, Warning, TEXT("we are inside THE IDLE IF "));
			SetEnemyMovementStatus(EEnemyMovementStatus::EMS_CharacterDetected);
			
			// Timer which allows to play an animation before the Enemy starts moving to the Character
			GetWorld()->GetTimerManager().SetTimer(DelayTimerBetweenStates,this, &AEnemy::ResetTimer,2.90f,false);
		}

		// The Character isn't inside the agro/combat radius. 
		else if (!bCharInsideAgroSphere && !bCharInsideCombatSphere )
		{
			
			TargetLost();
			bCanMoveToTarget = false;
			
			SetEnemyMovementStatus(EEnemyMovementStatus::EMS_Walking);
			UE_LOG(LogTemp, Warning, TEXT("we are inside THE IDLE ELSE IF"));	
		}
		
		break;

		
		/* EMS_Walking *//***********************************************************************************/
        case EEnemyMovementStatus::EMS_Walking:

        //TODO: move to random location using navdata
        DirectionToMove = CurrentLocation.GetClampedToMaxSize2D(25.f);
		
		AIController->MoveToLocation(DirectionToMove,250.f,true,
            true,false,true);
        
      
        
		if(bCharInsideAgroSphere)
			SetEnemyMovementStatus(EEnemyMovementStatus::EMS_Idle);
		break;
		
		

		/* EMS_CharacterDetected *//***********************************************************************************/
		case EEnemyMovementStatus::EMS_CharacterDetected:
		UE_LOG(LogTemp, Warning, TEXT("we are inside THE CHARACTERDETECTED "));
		
		if(bCharInsideAgroSphere && !bCharInsideCombatSphere && bCanMoveToTarget)
		{
			UE_LOG(LogTemp, Warning, TEXT("we are inside THE CHARACTERDETECTED IF"));
			
				SetEnemyMovementStatus(EEnemyMovementStatus::EMS_MoveToTarget);
		}
		else
		{
			if(!bCharInsideAgroSphere)
			{
				UE_LOG(LogTemp, Warning, TEXT("we are inside THE CHARACTERDETECTED ELSE IF"));
				SetEnemyMovementStatus(EEnemyMovementStatus::EMS_Idle);
			}

			else if(bCharInsideCombatSphere)
			{
				UE_LOG(LogTemp, Warning, TEXT("we are inside THE CHARACTERDETECTED ELSE ELSE IF"));
				SetEnemyMovementStatus(EEnemyMovementStatus::EMS_Attacking);
			}
		}
		break;

		/* EMS_MoveToTarget *//***********************************************************************************/
		case EEnemyMovementStatus::EMS_MoveToTarget:
		UE_LOG(LogTemp, Warning, TEXT("we are inside THE MOVETOTARGET "));
		StopAttackingTarget(); // bIsAttackingTarget = false;
		bCanMoveToTarget = true;

		// Checking if we inside the Combat / Agro sphere. If so, the status will change to the necessary one. 
		if(bCharInsideCombatSphere)
		{
			UE_LOG(LogTemp, Warning, TEXT("we are inside THE MOVETOTARGET IF"));
			SetEnemyMovementStatus(EEnemyMovementStatus::EMS_Attacking);
		}
		else if (bCharInsideAgroSphere && !bCharInsideCombatSphere)
		{
			UE_LOG(LogTemp, Warning, TEXT("we are inside THE MOVETOTARGET ELSE IF"));
			MoveToTarget(CharacterToMove);
			if(!bCharInsideAgroSphere && !bCharInsideCombatSphere)
			{
				TargetLost();
				UE_LOG(LogTemp, Warning, TEXT("we are inside THE MOVETOTARGET ELSE IF IF"));
				SetEnemyMovementStatus(EEnemyMovementStatus::EMS_Walking);
			}
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("we are inside THE MOVETOTARGET ELSE"));
			SetEnemyMovementStatus(EEnemyMovementStatus::EMS_Walking);
		}
		break;

		/* EMS_Attacking *//***********************************************************************************/
		case EEnemyMovementStatus::EMS_Attacking:
		UE_LOG(LogTemp, Warning, TEXT("we are inside THE ATTACKING "));
		
		if (bCharInsideCombatSphere)
		{
			UE_LOG(LogTemp, Warning, TEXT("we are inside THE ATTACKING IF"));
			AttackingTarget(); //bIsAttacking = true;
		}
		else if (!bCharInsideCombatSphere && bCharInsideAgroSphere)
		{
			UE_LOG(LogTemp, Warning, TEXT("we are inside THE ATTACKING ELSE IF"));
			SetEnemyMovementStatus(EEnemyMovementStatus::EMS_MoveToTarget);
		}
		else if (!bCharInsideCombatSphere && !bCharInsideAgroSphere)
		{
			UE_LOG(LogTemp, Warning, TEXT("we are inside THE ATTACKING ELSE IF (LAST ONE)"));
			SetEnemyMovementStatus(EEnemyMovementStatus::EMS_Idle);
		}
		break;

		default:
		break;
	}
}

// Called every frame
void AEnemy::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	
	EnemyStatusUpdating(DeltaTime);
}


// Called to bind functionality to input
void AEnemy::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
}





                              /* Spheres */
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
			TargetDetected(); // bCharacterSpottedByEnemy = true;
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
			TargetLost(); // bCharacterSpottedByEnemy = false;
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
			// UE_LOG(LogTemp, Warning, TEXT("we are inside COMBATSPHERE OVERLAP BEGIN"));
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
			// UE_LOG(LogTemp, Warning, TEXT("we are inside COMBATSPHERE OVERLAP END"));
		}
	}
}

void AEnemy::GetRandomPoint(FVector& Direction) const
{
	
	Direction += CurrentLocation;
}


void AEnemy::MoveToTarget(ARPGLessonCharacter* Character)
{
	SetEnemyMovementStatus(EEnemyMovementStatus::EMS_MoveToTarget);
	if(AIController)
	{
		/* Creating a FAIMoveRequest/FNavPathSharePtr to feed them to MoveTo() function */
		FAIMoveRequest MoveRequest;
		MoveRequest.SetGoalActor(Character);
		MoveRequest.SetAcceptanceRadius(15.0f);

		FNavPathSharedPtr NavigationPath;

		AIController->MoveTo(MoveRequest,&NavigationPath);

		
		

		// Draw debug sphere which shows the shortest path from Enemy to the main Character
		
		// auto PathPoints = NavigationPath ->GetPathPoints();
		// for (auto Point : PathPoints)
		// {
		// FVector Location = Point.Location;
		// 	UKismetSystemLibrary::DrawDebugSphere(this,Location,25.f,
		//  		8,FLinearColor::Blue,2.5f,1.5f);
		// };
	}
}

void AEnemy::ResetTimer()
{
	GetWorld()->GetTimerManager().ClearTimer(DelayTimerBetweenStates);
	bCanMoveToTarget = true;
	GEngine->AddOnScreenDebugMessage(-1,5.f,FColor::Red,"Timer Was reset"); // debug 
}

// Toggles to control the Enemies reaction to the Character (depends on how far we are from the enemy)
void AEnemy::TargetDetected()
{
	bCharacterSpottedByEnemy = true;
	GetCharacterMovement()->MaxWalkSpeed = BattleWalkingSpeed;
}

void AEnemy::TargetLost()
{
	bCharacterSpottedByEnemy = false;
	GetCharacterMovement()->MaxWalkSpeed = NormalWalkingSpeed;
}

void AEnemy::AttackingTarget()
{
	bIsAttackingTarget = true;
}

void AEnemy::StopAttackingTarget()
{
	bIsAttackingTarget = false;
}

