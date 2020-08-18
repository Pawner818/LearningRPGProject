// Fill out your copyright notice in the Description page of Project Settings.


#include "Enemy.h"
#include "Components/SphereComponent.h"
#include "AIController.h"
#include "RPGLessonCharacter.h"
#include "Kismet/KismetSystemLibrary.h"
#include "NavigationData.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "TimerManager.h"
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
	EnemyMovementStatus = EEnemyMovementStatus::EMS_Idle;

    // Initializing BOOLS 
	bCharacterSpottedByEnemy = false; // Is an enemy "see" and ready to attack our character (changes to "true" if we reach the necessary agro distance)
	bCharInsideAgroSphere = false; // Do we collide with the Enemy Agro sphere? Nope, at least at the beginning
	bIsAttackingTarget = false; // the Enemy is not attacking us 
	bCharInsideCombatSphere = false; // Same to the AgroSphere
	bCanMoveToTarget = false; // The Enemy has to wait some time between Detected and MoveTo states.

	

	// INITIALIZING STATS

	// Depends on current mode 
	NormalWalkingSpeed = 600.f;
	BattleWalkingSpeed = 700.f;
	
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
void AEnemy::EnemyStatusUpdating(float DeltaValue)
{
	DeltaValue = GetWorld()->GetDeltaSeconds();

	switch (EnemyMovementStatus)
	{
		case EEnemyMovementStatus::EMS_Idle:
		UE_LOG(LogTemp, Warning, TEXT("we are inside THE IDLE "));
		if(bCharacterSpottedByEnemy && bCharInsideAgroSphere && !bCharInsideCombatSphere)
		{
			UE_LOG(LogTemp, Warning, TEXT("we are inside THE IDLE IF "));
			SetEnemyMovementStatus(EEnemyMovementStatus::EMS_CharacterDetected);
			
			// Timer which allows to play an animation before the Enemy starts moving to the Character
			GetWorld()->GetTimerManager().SetTimer(DelayTimerBetweenStates,this, &AEnemy::ResetTimer,2.90f,false);
		}
		else
		{
			// the enemy walking around, normal mode
			UE_LOG(LogTemp, Warning, TEXT("we are inside THE IDLE ELSE "));
		}
		
		break;

        //  case EEnemyMovementStatus::EMS_Walking:
		// if(!bCharacterSpottedByEnemy && !bCharInsideAgroSphere)
		// {
		// 	// TODO: walking around function in the "non-disturbing" mode
		// }
		// else
		// {
		// 	SetEnemyMovementStatus(EEnemyMovementStatus::EMS_Idle);
		// }
		// break;

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



		
		case EEnemyMovementStatus::EMS_MoveToTarget:
		UE_LOG(LogTemp, Warning, TEXT("we are inside THE MOVETOTARGET "));
		StopAttackingTarget(); // bIsAttackingTarget = false;

		// MoveToTarget()
		if(bCharInsideCombatSphere)
		{
			UE_LOG(LogTemp, Warning, TEXT("we are inside THE MOVETOTARGET IF"));
			SetEnemyMovementStatus(EEnemyMovementStatus::EMS_Attacking);
		}
		else if (bCharInsideAgroSphere && !bCharInsideCombatSphere)
		{
			UE_LOG(LogTemp, Warning, TEXT("we are inside THE MOVETOTARGET ELSE IF"));
			MoveToTarget(CharacterToMove);
			if(!bCharInsideAgroSphere)
			{
				SetEnemyMovementStatus(EEnemyMovementStatus::EMS_Idle);
			}
		}
		else
		{
			SetEnemyMovementStatus(EEnemyMovementStatus::EMS_Idle);
		}
		break;

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
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("we are inside THE ATTACKING ELSE"));
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

void AEnemy::AgroSphereOnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if(OtherActor)
	{
		CharacterToMove = Cast<ARPGLessonCharacter>(OtherActor);
		// ARPGLessonCharacter*Character=Cast<ARPGLessonCharacter>(OtherActor);
		if(CharacterToMove)
		{
			bCharInsideAgroSphere = true;
			TargetDetected(); // bCharacterSpottedByEnemy = true;
			
			// UE_LOG(LogTemp, Warning, TEXT("we are inside THE AGROOVERPALBEGIN FUNCTION"));
		}
	}
}

void AEnemy::AgroSphereOnOverlapEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	if(OtherActor)
	{
		CharacterToMove = Cast<ARPGLessonCharacter>(OtherActor);
		// ARPGLessonCharacter*Character=Cast<ARPGLessonCharacter>(OtherActor);
		if(CharacterToMove)
		{
			bCharInsideAgroSphere = false;
			TargetLost(); // bCharacterSpottedByEnemy = false;
			
			// UE_LOG(LogTemp, Warning, TEXT("we are inside THE AGROOVERPALEND FUNCTION"));
			
		}
	}
}

void AEnemy::CombatSphereOnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if(OtherActor)
	{
		CharacterToMove = Cast<ARPGLessonCharacter>(OtherActor);
		// ARPGLessonCharacter*Character=Cast<ARPGLessonCharacter>(OtherActor);
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
		// ARPGLessonCharacter*Character=Cast<ARPGLessonCharacter>(OtherActor);
		if(CharacterToMove)
		{
			bCharInsideCombatSphere = false;
			// UE_LOG(LogTemp, Warning, TEXT("we are inside COMBATSPHERE OVERLAP END"));
		}
	}
}



void AEnemy::MoveToTarget(ARPGLessonCharacter* Character)
{
	// SetEnemyMovementStatus(EEnemyMovementStatus::EMS_MoveToTarget);
	
	if(AIController)
	{
		/* Creating a FAIMoveRequest/FNavPathSharePtr to feed them to MoveTo() function */
		FAIMoveRequest MoveRequest;
		MoveRequest.SetGoalActor(Character);
		MoveRequest.SetAcceptanceRadius(15.0f);

		FNavPathSharedPtr NavigationPath;

		AIController->MoveTo(MoveRequest, &NavigationPath);

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
	
	GEngine->AddOnScreenDebugMessage(-1,5.f,FColor::Red,"Timer Was reset");
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

