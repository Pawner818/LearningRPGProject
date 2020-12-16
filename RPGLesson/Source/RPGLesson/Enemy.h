// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Field/FieldSystemNodes.h"
#include "GameFramework/Character.h"
#include "Enemy.generated.h"

/* Forward declaration */
class AEnemyAIController;

/* Enums */
UENUM(BlueprintType)
enum class EEnemyMovementStatus : uint8
{
	/* Bunch of States, depends on is the Enemy interacting with the main Char at this moment? */
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

	/* Sets Enemy Movement status from the Enum list */
	UPROPERTY(VisibleAnywhere,BlueprintReadWrite,Category="Movement")
	EEnemyMovementStatus EnemyMovementStatus;

	/* Setter for the EnemyMovementStatus */
	FORCEINLINE void SetEnemyMovementStatus(EEnemyMovementStatus Status){EnemyMovementStatus = Status;}

	/* AgroSphere is invisible in the game, but when the Character cross the border, our Enemy status switches to DetectCharacter and so on */
	UPROPERTY(VisibleAnywhere,BlueprintReadOnly,Category="AI")
	class USphereComponent*AgroSphere;

	/* Same as the sphere above, but has closest distance to the Enemy, so when we reach it, the Enemy starts attacking us */
	UPROPERTY(VisibleAnywhere,BlueprintReadOnly,Category="AI")
    USphereComponent*CombatSphere;

	UPROPERTY(VisibleAnywhere,BlueprintReadOnly,Category="AI")
	class AEnemyAIController*EnemyAIController;

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

	/* Reference to the Character */ 
	UPROPERTY()
	class ARPGLessonCharacter*CharacterToMove;

	UPROPERTY(VisibleAnywhere,BlueprintReadWrite,Category="AI")
	ARPGLessonCharacter*CharacterIsTarget;

	/* When the Enemy detect the Character (or lost), we change this boolean to enable/disable moving function, switch the state */
	UPROPERTY(VisibleAnywhere,BlueprintReadOnly,Category="AI")
	bool bCanMoveToTarget;

	/* If the Character is inside the AgroSphere, the Enemy starts reacting, status changes to CharacterDetected */
	UPROPERTY(VisibleAnywhere,BlueprintReadWrite,Category = "AI")
    bool bCharacterSpottedByEnemy;

	/* Is the main Character inside agrosphere? */
	UPROPERTY(VisibleAnywhere,BlueprintReadWrite,Category = "AI")
	bool bCharInsideAgroSphere;

	/* Well, those 2 functions below just toggle a boolean, nothing more */
	void  TargetDetected();
	void  TargetLost();

	/* Enemy near the Character, the Player inside Combat Sphere, the Enemy ready to attack us */
	UPROPERTY(VisibleAnywhere,BlueprintReadOnly,Category = "Combat")
    bool bIsAttackingTarget;

	/* Two functions to switch our bool IsAttackingTarget */

	void AttackTheCharacter();

	UFUNCTION(BlueprintCallable)
    void AttackEnd();

	UPROPERTY(EditDefaultsOnly,BlueprintReadOnly,Category="Combat")
	class UAnimMontage*EnemyCombatMontage;

	/* Is the main Character inside CombatSphere? */ 
	UPROPERTY(VisibleAnywhere,BlueprintReadWrite,Category = "AI")
	bool bCharInsideCombatSphere;

	/*
	 *
	 * ENEMY STATS 
	 * 
	 */

	UPROPERTY(EditAnywhere,BlueprintReadOnly,Category="AI | Stats")
	float CurrentHealth;

	UPROPERTY(EditAnywhere,BlueprintReadOnly,Category="AI | Stats")
	float MaxHealth;

	UPROPERTY(EditAnywhere,BlueprintReadOnly,Category="AI | Stats")
	float Damage;

	UPROPERTY(EditAnywhere,BlueprintReadOnly,Category="AI")
	class UParticleSystem*HitParticles;
	
	UPROPERTY(EditAnywhere,BlueprintReadWrite,Category="AI")
    class USoundCue*HitSound;

	UPROPERTY(EditAnywhere,BlueprintReadWrite,Category="AI")
	USoundCue*EnemySwingSound;




	UPROPERTY(VisibleAnywhere,BlueprintReadWrite,Category="Combat")
	class UBoxComponent*CombatCollisionL;

	UPROPERTY(VisibleAnywhere,BlueprintReadWrite,Category="Combat")
	UBoxComponent*CombatCollisionR;

    UPROPERTY(VisibleAnywhere,BlueprintReadWrite,Category="Combat")
    class UStaticMeshComponent*RightWeapon;

    UPROPERTY(VisibleAnywhere,BlueprintReadWrite,Category="Combat")
    UStaticMeshComponent*LeftWeapon;

	UPROPERTY(VisibleAnywhere,BlueprintReadWrite,Category="Combat")
	class UParticleSystemComponent*LeftWeaponParticle;

	UPROPERTY(VisibleAnywhere,BlueprintReadWrite,Category="Combat")
	UParticleSystemComponent*RightWeaponParticle;

	UFUNCTION()
    void OnCombatBegin (UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult & SweepResult);

	UFUNCTION()
    void OnCombatEnd (UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

	UFUNCTION(BlueprintCallable)
    void ActivateCollision();

	UFUNCTION(BlueprintCallable)
    void DeactivateCollision();

};
