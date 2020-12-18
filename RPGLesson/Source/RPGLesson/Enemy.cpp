// Fill out your copyright notice in the Description page of Project Settings.


#include "Enemy.h"
#include "Components/SphereComponent.h"
#include "EnemyAIController.h"
#include "RPGLessonCharacter.h"
#include "Kismet/KismetSystemLibrary.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Components/BoxComponent.h"
#include "Components/SceneComponent.h"
#include "Particles/ParticleSystemComponent.h"
#include "Engine/EngineTypes.h"
#include "Engine/StaticMeshSocket.h"
#include "Kismet/GameplayStatics.h"
#include "Sound/SoundCue.h"
#include "Animation/AnimInstance.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Components/CapsuleComponent.h"

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

    	/* I did the Attachment setup inside the Construction Script (I mean I had to, because for some reasons it couldn't attach to the proper socket in a right way) */
	LeftWeapon = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("LeftWeapon"));
    RightWeapon = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("RightWeapon"));

    	/* Left Damage Box (attached to the LeftWeapon */
    CombatCollisionL = CreateDefaultSubobject<UBoxComponent>(TEXT("CombatCollisionL"));
    CombatCollisionL->InitBoxExtent(FVector(100.f,100.f,100.f));
    	
    	/* Right Damage Box (attached to the RightWeapon */
    CombatCollisionR = CreateDefaultSubobject<UBoxComponent>(TEXT("CombatCollisionR"));
    CombatCollisionR->InitBoxExtent(FVector(100.f,100.f,100.f));

    LeftWeaponParticle = CreateDefaultSubobject<UParticleSystemComponent>(TEXT("LeftWeaponParticle"));
    LeftWeaponParticle ->AttachToComponent(LeftWeapon,FAttachmentTransformRules::SnapToTargetIncludingScale,"ParticleSocket") ;

    RightWeaponParticle = CreateDefaultSubobject<UParticleSystemComponent>(TEXT("RightWeaponParticle"));
    RightWeaponParticle ->AttachToComponent(RightWeapon,FAttachmentTransformRules::SnapToTargetIncludingScale,"ParticleSocket") ;

	// INITIALIZING ENUMS 
	EnemyMovementStatus = EEnemyMovementStatus::EMS_Idle;
	
    // INITIALIZING BOOLS 
	bCharacterSpottedByEnemy = false; // Is an enemy "see" and ready to attack our character (changes to "true" if we reach the necessary agro distance)
	bCharInsideAgroSphere = false; // Do we collide with the Enemy Agro sphere? Nope, at least at the beginning
	bIsAttackingTarget = false; // the Enemy is not attacking us 
	bCharInsideCombatSphere = false; // Same to the AgroSphere
	bCanMoveToTarget = false; // The Enemy has to wait some time between Detected and MoveTo states.
    	bEnemyAlive = true;

	// INITIALIZING STATS

	// Depends on current mode 
	MaxHealth = 1000.f;
	CurrentHealth = 1000.f;
	Damage = 50.f;

    	AttackMinTime = 0.5f;
    	AttackMaxTime = 2.f;

	AcceptableDistance = 250.f;
	CurrentLocation = GetActorLocation();

    	DeathDelay = 3.f; // seconds before the Enemy body will be destroyed 
}

// Called when the game starts or when spawned
void AEnemy::BeginPlay()
{
	Super::BeginPlay();
	
	EnemyAIController = Cast<AEnemyAIController>(GetController());
  
		/* Overlap hit events */
    	
	AgroSphere -> OnComponentBeginOverlap.AddDynamic(this,&AEnemy::AgroSphereOnOverlapBegin);
    AgroSphere->OnComponentEndOverlap.AddDynamic(this,&AEnemy::AgroSphereOnOverlapEnd);
    	
	CombatSphere -> OnComponentBeginOverlap.AddDynamic(this,&AEnemy::CombatSphereOnOverlapBegin);
	CombatSphere->OnComponentEndOverlap.AddDynamic(this,&AEnemy::CombatSphereOnOverlapEnd);

    CombatCollisionL->OnComponentBeginOverlap.AddDynamic(this,&AEnemy::OnCombatBegin);
    CombatCollisionL->OnComponentEndOverlap.AddDynamic(this,&AEnemy::OnCombatEnd);

    CombatCollisionR->OnComponentBeginOverlap.AddDynamic(this,&AEnemy::OnCombatBegin);
    CombatCollisionR->OnComponentEndOverlap.AddDynamic(this,&AEnemy::OnCombatEnd);

   // Collision setup

		 /* Left damage box */
    CombatCollisionL->SetCollisionEnabled(ECollisionEnabled::NoCollision);
    CombatCollisionL->SetCollisionObjectType(ECollisionChannel::ECC_WorldDynamic);
    CombatCollisionL->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
    CombatCollisionL->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn,ECollisionResponse::ECR_Overlap);

    	/* Right damage box */
    CombatCollisionR->SetCollisionEnabled(ECollisionEnabled::NoCollision);
    CombatCollisionR->SetCollisionObjectType(ECollisionChannel::ECC_WorldDynamic);
    CombatCollisionR->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
    CombatCollisionR->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn,ECollisionResponse::ECR_Overlap);
}

	/* Spheres Overlapping /StopOverlapping*/
void AEnemy::AgroSphereOnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
    UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if(OtherActor && Alive())
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
	if(OtherActor && Alive())
	{
		CharacterToMove = Cast<ARPGLessonCharacter>(OtherActor);
		if(CharacterToMove)
		{
			CharacterToMove->SetCombatTarget(this);
			CharacterIsTarget = CharacterToMove;
			bCharInsideCombatSphere = true;	
			AttackTheCharacter();
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
			if(CharacterToMove ->CombatTarget == this)
			{
				CharacterToMove->SetCombatTarget(nullptr);
			}
			
			bCharInsideCombatSphere = false;
			AttackEnd();
			
			if(EnemyMovementStatus!=EEnemyMovementStatus::EMS_Attacking)
			{
				CharacterIsTarget = nullptr;
			}

			GetWorldTimerManager().ClearTimer(AttackTimer);
		}
	}
}


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


// Toggles to control the Enemies reaction to the Character (depends on how far we are from the enemy)
void AEnemy::TargetDetected()
{
	bCharacterSpottedByEnemy = true;
}

void AEnemy::TargetLost()
{
	bCharacterSpottedByEnemy = false;
}

void AEnemy::AttackTheCharacter() 
{
    if(Alive())
    {
    		if(EnemyAIController)
    		{
    			SetEnemyMovementStatus(EEnemyMovementStatus::EMS_Attacking);
    		}
    	
    		if(!bIsAttackingTarget)
    		{
    			bIsAttackingTarget = true;

    			UAnimInstance*AnimInstance=GetMesh()->GetAnimInstance();
    			if(AnimInstance)
    			{
    				const int32 MontageSection = FMath::RandRange(0,2);
    				switch (MontageSection)
    				{
    				case 0:
    					AnimInstance->Montage_Play(EnemyCombatMontage, 1.1f);
    					AnimInstance->Montage_JumpToSection(FName("Attack_1"), EnemyCombatMontage);
    					break;
				
    				case 1:
    					AnimInstance->Montage_Play(EnemyCombatMontage, 1.1f);
    					AnimInstance->Montage_JumpToSection(FName("Attack_2"), EnemyCombatMontage);
    					break;

    				case 2:
    					AnimInstance->Montage_Play(EnemyCombatMontage, 1.1f);
    					AnimInstance->Montage_JumpToSection(FName("Attack_3"), EnemyCombatMontage);
    					break;

    					// case 3:
    					// 	AnimInstance->Montage_Play(EnemyCombatMontage, 1.0f);
    					// 	AnimInstance->Montage_JumpToSection(FName("Attack_Range"), EnemyCombatMontage);
    					// 	break;

    					default:
    						break;
    				}
    			}

    		}	
    }
   
}

void AEnemy::AttackEnd()
{
    bIsAttackingTarget = false;
    if(bCharInsideCombatSphere)
    {
    	float AttackTime = FMath::FRandRange(AttackMinTime,AttackMaxTime);
    	GetWorldTimerManager().SetTimer(AttackTimer,this, &AEnemy::AttackTheCharacter,AttackTime);
    }
}

void AEnemy::OnCombatBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp,
	int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
    	if(OtherActor)
    	{
    		ARPGLessonCharacter*Character = Cast<ARPGLessonCharacter>(OtherActor);
    		if(Character)
    		{
    			/* Create a splash of blood when we attack the Enemy */
    			if(Character->HitParticles)
    			{
    				// getting an access to the sword socket to play a particle from 
    				const UStaticMeshSocket*TipSocket = RightWeapon->GetSocketByName("TipSocket");
                
    				if(TipSocket)
    				{
    					const FVector SocketLocation = TipSocket->RelativeLocation;           
    					UGameplayStatics::SpawnEmitterAtLocation(GetWorld(),Character->HitParticles,SocketLocation,FRotator(0.f),false);
    				}
    			}
	    
    			/* Create a hit sound when we attack the Enemy */
    			if(Character->HitSound)
    			{
    				UGameplayStatics::PlaySound2D(this,Character->HitSound);
    			}

    			/* Taking damage */
    			if(DamageTypeClass)
    			{
    				UGameplayStatics::ApplyDamage(CharacterToMove,Damage,EnemyAIController,this,DamageTypeClass);
    			}
    		}
    	}
}

void AEnemy::OnCombatEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp,
	int32 OtherBodyIndex)
{
}

void AEnemy::ActivateCollision()
{
    	CombatCollisionL->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
    	CombatCollisionR->SetCollisionEnabled(ECollisionEnabled::QueryOnly);

    	if(EnemySwingSound)
    	{
    		UGameplayStatics::PlaySound2D(this,EnemySwingSound);
    	}
}

void AEnemy::DeactivateCollision()
{
    	CombatCollisionL->SetCollisionEnabled(ECollisionEnabled::NoCollision);
    	CombatCollisionR->SetCollisionEnabled(ECollisionEnabled::NoCollision);
}

void AEnemy::ChangeAliveBoolBT()
    {
    	if(EnemyAIController)
    	{
    		EnemyAIController->GetBlackboardComponent()->SetValueAsBool("Alive", false);
    	}
    }

void AEnemy::EnemyDeath()
{
    	
    	UAnimInstance*AnimInstance = GetMesh()->GetAnimInstance();
    	if(AnimInstance)
    	{
    		AnimInstance->Montage_Play(EnemyCombatMontage,0.8);
    		AnimInstance->Montage_JumpToSection(FName("Death"),EnemyCombatMontage);
    	}
    	SetEnemyMovementStatus(EEnemyMovementStatus::EMS_Dead);
    	
    	DeactivateCollision(); // weapon collision deactivating
    	AgroSphere->SetCollisionEnabled(ECollisionEnabled::NoCollision);
    	CombatSphere->SetCollisionEnabled(ECollisionEnabled::NoCollision);
    	
    	GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);

    	
}

float AEnemy::TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
{
    	if(CurrentHealth-DamageAmount<=0.f)
    	{
    		CurrentHealth-=DamageAmount;
    		bEnemyAlive = false;
    		ChangeAliveBoolBT();
            EnemyDeath();
    	}
    	else
    	{
    		MaxHealth-=DamageAmount;
    	}

    	return DamageAmount;
}

void AEnemy::DeathEnd()
{
    
	GetMesh()->bPauseAnims = true;
    GetMesh()->bNoSkeletonUpdate = true;

    	GetWorldTimerManager().SetTimer(DeathTimer,this,&AEnemy::EnemyDisappear,DeathDelay);
}

bool AEnemy::Alive()
{	
    return bEnemyAlive;
}

void AEnemy::EnemyDisappear()
{
    	Destroy();
}




