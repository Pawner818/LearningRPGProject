// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "EnemyAIController.h"
#include "Field/FieldSystemNodes.h"
#include "GameFramework/Character.h"
#include "Enemy.generated.h"

/* Forward declaration */
class AEnemyAIController;

/* Enums */
UENUM(BlueprintType)
enum class EEnemyMovementStatus : uint8
{
	/* Bunch of States, depends on is an enemy interacting with the main Char at this moment? */
	EMS_Idle UMETA(DisplayName = "Idle"),
	EMS_Walking UMETA(DisplayName = "Walking"),
	EMS_CharacterDetected UMETA(DisplayName = "DetectCharacter"),
    EMS_MoveToTarget UMETA(DisplayName = "MoveToTarget"),
	EMS_Attacking UMETA(DisplayName = "Attacking"),
	EMS_Death UMETA(DisplayName = "Death"),

	EMS_MAX UMETA (DisplayName = "MAX")
};

UCLASS()
class RPGLESSON_API AEnemy : public ACharacter
{
	GENERATED_BODY()

public:
	
	// Sets default values for this character's properties
	AEnemy(const FObjectInitializer& ObjectInitializer);

	/* Sets Enemy Movement status from the Enum list */
	UPROPERTY(VisibleAnywhere,BlueprintReadWrite,Category="Movement")
	EEnemyMovementStatus EnemyMovementStatus;

	/* Setter for the EnemyMovementStatus */
	FORCEINLINE void SetEnemyMovementStatus(EEnemyMovementStatus Status){EnemyMovementStatus = Status;}

	/* AgroSphere is invicible in the game, but when the Character cross the border, our Enemy status switches to DetectCharacter and so on */
	UPROPERTY(VisibleAnywhere,BlueprintReadOnly,Category="AI")
	class USphereComponent*AgroSphere;

	/* Same as the sphere above, but has closest distance to the Enemy, so when we reach it, the Enemy starts attacking us */
	UPROPERTY(VisibleAnywhere,BlueprintReadOnly,Category="AI")
    USphereComponent*CombatSphere;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="AI")
	class AEnemyAIController *EAIController;



	/* Function which allows the Enemy to reach the Character */
	void MoveToTarget(ARPGLessonCharacter* Character);

protected:
	
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;


	/* Overlapping overriden methods. When we overlap or stop overlapping with our spheres, the Enemy does something. */
	UFUNCTION()
     virtual void AgroSphereOnOverlapBegin (UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult & SweepResult);

	UFUNCTION()
     virtual void AgroSphereOnOverlapEnd (UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

	UFUNCTION()
     virtual void CombatSphereOnOverlapBegin (UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult & SweepResult);

	UFUNCTION()
    virtual void CombatSphereOnOverlapEnd (UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);


	/* Movement and interaction with the main Character */

	/* UPD: Some of those parameters don't used anywhere, but in the future I'll fix that */

	UPROPERTY(VisibleAnywhere,BlueprintReadWrite,Category="AI Movement")
	FVector CurrentLocation;
	
	UPROPERTY(VisibleAnywhere,BlueprintReadWrite,Category="AI Movement")
	FVector DirectionToMove;

	UPROPERTY(VisibleAnywhere,BlueprintReadWrite,Category="AI Movement")
	float AcceptableDistance;

	UPROPERTY(VisibleAnywhere,BlueprintReadWrite,Category="AI Movement") 	
	FVector GoalLocationToMove;

	/* Main updating function, which is updating every frame */
	void EnemyStatusUpdating(float DeltaValue);

	/* Reference to the Character */ 
	UPROPERTY()
	class ARPGLessonCharacter*CharacterToMove;

	
	/* Timer for creating a little delay between animations */
	
	/* UPD: That's my way to build animations and dependencies between states, I know that we could use BlackBoards and Trees, but for learning purposes
	 * I guess it's the easiest way to learn how it's actually working inside the C++ class.  
	 */
	struct FTimerHandle DelayTimerBetweenStates;

	/* When the Enemy detect the Character (or lost), we change this boolean to enable/disable moving function, switch the state */
	UPROPERTY(VisibleAnywhere,BlueprintReadOnly,Category="AI")
	bool bCanMoveToTarget;

	/* Works with FTimerHandle struct */
	UFUNCTION()
	void ResetTimer();

	/* If the Character is inside the AgroSphere, the Enemy starts reacting, status changes to CharacterDetected */
	UPROPERTY(VisibleAnywhere,BlueprintReadWrite,Category = "AI")
    bool bCharacterSpottedByEnemy;

	/* Is the main Character inside agrosphere? */
	UPROPERTY(VisibleAnywhere,BlueprintReadWrite,Category = "AI")
	bool bCharInsideAgroSphere;

	/* Well, those 2 functions below just toggle a boolean, nothing more */
	void  TargetDetected();
	void  TargetLost();

	/* Enemy near the Character, inside Combat Sphere, ready to attack us, status changes to Attacking */
	UPROPERTY(VisibleAnywhere,BlueprintReadWrite,Category = "AI")
    bool bIsAttackingTarget;

	/* Two functions to switch our bool IsAttackingTarget */
	void AttackingTarget();
	void StopAttackingTarget();

	/* Is the main Character inside CombatSphere? */ 
	UPROPERTY(VisibleAnywhere,BlueprintReadWrite,Category = "AI")
	bool bCharInsideCombatSphere;

	/*
	 *
	 * ENEMY STATS 
	 * 
	 */

	//TODO:
	// float CurrentHealth;
	//
	// float MaxHealth;
	//
	// float Damage;

	UPROPERTY(EditAnywhere,BlueprintReadWrite,Category="AI | Stats ")
	float NormalWalkingSpeed;

	UPROPERTY(EditAnywhere,BlueprintReadWrite,Category="AI | Stats ")
	float BattleWalkingSpeed;
	
};
