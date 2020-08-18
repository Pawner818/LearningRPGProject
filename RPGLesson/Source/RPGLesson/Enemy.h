// Fill out your copyright notice in the Description page of Project Settings.

#pragma once


#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Enemy.generated.h"



UENUM(BlueprintType)
enum class EEnemyMovementStatus : uint8
{
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
	AEnemy();

	UPROPERTY(VisibleAnywhere,BlueprintReadWrite,Category="Movement")
	EEnemyMovementStatus EnemyMovementStatus;

	// void SetEnemyMovementStatus(EEnemyMovementStatus Status);

	FORCEINLINE void SetEnemyMovementStatus(EEnemyMovementStatus Status){EnemyMovementStatus = Status;}

	UPROPERTY(VisibleAnywhere,BlueprintReadOnly,Category="AI")
	class USphereComponent*AgroSphere;

	UPROPERTY(VisibleAnywhere,BlueprintReadOnly,Category="AI")
    USphereComponent*CombatSphere;

	UPROPERTY(VisibleAnywhere,BlueprintReadOnly,Category="AI")
	class AAIController*AIController;


protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	UFUNCTION()
     virtual void AgroSphereOnOverlapBegin (UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult & SweepResult);

	UFUNCTION()
     virtual void AgroSphereOnOverlapEnd (UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

	UFUNCTION()
     virtual void CombatSphereOnOverlapBegin (UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult & SweepResult);

	UFUNCTION()
    virtual void CombatSphereOnOverlapEnd (UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);



	/* Movement and interaction with the main Character */ 


	// Main updating function, which is updating every frame 
	void EnemyStatusUpdating(float DeltaValue);

	// Allows the Enemy move to his target (in our case, to the main Character)
	UFUNCTION(BLueprintCallable)
    void MoveToTarget(class ARPGLessonCharacter* Character);

	ARPGLessonCharacter*CharacterToMove;

	struct FTimerHandle DelayTimerBetweenStates;

	UPROPERTY(VisibleAnywhere,BlueprintReadOnly,Category="AI")
	bool bCanMoveToTarget;
	
	UFUNCTION()
	void ResetTimer();

	// if the Character is inside the AgroSphere, the Enemy starts reacting, Status changes to CharacterDetected

	UPROPERTY(VisibleAnywhere,BlueprintReadWrite,Category = "AI")
    bool bCharacterSpottedByEnemy;

	bool bCharInsideAgroSphere;
	
	void  TargetDetected();

	void  TargetLost();

	// Enemy near the Character, inside Combat Sphere, ready to attack us, Status changes to Attacking

	UPROPERTY(VisibleAnywhere,BlueprintReadWrite,Category = "AI")
    bool bIsAttackingTarget;

	bool bCharInsideCombatSphere;

	void AttackingTarget();

	void StopAttackingTarget();


	/*
	 *
	 * ENEMY STATS 
	 * 
	 */

	UPROPERTY(EditAnywhere,BlueprintReadWrite,Category="AI | Stats ")
	float NormalWalkingSpeed;

	UPROPERTY(EditAnywhere,BlueprintReadWrite,Category="AI | Stats ")
	float BattleWalkingSpeed;
	
};
