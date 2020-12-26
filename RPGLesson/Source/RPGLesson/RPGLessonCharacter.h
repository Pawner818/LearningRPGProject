// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "GameFramework/GameMode.h"
#include "Sound/SoundCue.h"
#include "UObject/ObjectMacros.h"

#include "RPGLessonCharacter.generated.h"

/***********************************************************************************
*
* ENUMS
* 
**********************************************************************************/
UENUM(BlueprintType)
enum class EMovementStatus : uint8
{

	EMS_Idle UMETA(DisplayName = "Idle"),
	EMS_Running UMETA(DisplayName = "Running"),
	EMS_Walking UMETA(DisplayName = "Walking"),
	EMS_Sprinting UMETA(DisplayName = "Sprinting"),
	EMS_Crouching UMETA (DisplayName = "Crouching"),
	EMS_Dead UMETA (DisplayName = "Dead"),
	
	EMS_MAX UMETA(DisplayName = "DefaultMAX")
};

// Enum of the CurrentStamina Status. Used in Animation_BP of the Character.
UENUM(BlueprintType)
enum class EStaminaStatus : uint8
{
	ESS_Normal UMETA(DisplayName = "Normal"),
	ESS_BelowMinimum UMETA(DisplayName = "BelowMinimum"),
	ESS_Recovering UMETA(DisplayName = "Recovering"),

	ESS_MAX UMETA(DisplayName = "DefaultMAX")
};


UCLASS(config=Game)
class ARPGLessonCharacter : public ACharacter
{
	GENERATED_BODY()

public:

	ARPGLessonCharacter();

	// debugging array of the pick up location of coins 
	TArray<FVector>PickupLocation;

	//press T to see the previous location of the picked up coins  
	UFUNCTION(BlueprintCallable)
    void ShowPickupLocation();
	
	/***********************************************************************************
	 *
	 * CurrentStamina
	 * 
	 **********************************************************************************/
	
	UPROPERTY(VisibleAnywhere,BlueprintReadWrite,Category="Enums")
	EStaminaStatus StaminaStatus;
	
	void SetStaminaStatus (EStaminaStatus Status);

	UFUNCTION(BlueprintCallable,Category="Movement | CurrentStamina")
	void StaminaStatusUpdating(float DeltaValue); 
	
	UPROPERTY(EditAnywhere,BlueprintReadWrite, Category="Movement | CurrentStamina")
	float StaminaDrainRate;

	UPROPERTY(EditAnywhere,BlueprintReadWrite, Category="Movement | CurrentStamina")
	bool  bCouldWeDrainStamina;
	
	UPROPERTY(EditAnywhere,BlueprintReadWrite, Category="Movement | CurrentStamina")
	float StaminaExhausted;

	/*************************************************************************
	 *
	 * Movement 
	 *
	 **************************************************************************/
	
	// Describes what status the Character is in 
	UPROPERTY(VisibleAnywhere,BlueprintReadWrite,Category="Enums")
    EMovementStatus MovementStatus;
	
	// Set movement status and running speed.
	UFUNCTION(BlueprintCallable,Category="Enums")
	void SetMovementStatus(EMovementStatus Status);

    UFUNCTION(BlueprintCallable,Category="Movement")
	void MovementStatusUpdating(float DeltaValue);

	UPROPERTY(EditAnywhere,BlueprintReadWrite,Category = "Movement | Booleans")
	bool bCharacterMoving;

	// 150.f
	UPROPERTY(EditAnywhere,BlueprintReadOnly,Category="Movement")
    float CrouchingSpeed;
	// 300.f
	UPROPERTY(EditAnywhere,BlueprintReadOnly,Category="Movement")
    float WalkingSpeed;
	// 600.f
	UPROPERTY(EditAnywhere,BlueprintReadOnly,Category="Movement")
	float RunningSpeed;
	// 1200.f
	UPROPERTY(EditAnywhere,BlueprintReadOnly,Category="Movement")
	float SprintingSpeed;

	UPROPERTY(VisibleAnywhere,BlueprintReadWrite, Category="Movement | Booleans")
	bool bIsAltPressed;

	UPROPERTY(VisibleAnywhere,BlueprintReadWrite, Category="Movement | Booleans")
	bool bIsCtrlPressed;
	
	UPROPERTY(VisibleAnywhere,BlueprintReadWrite, Category="Movement | Booleans")
	bool bShiftKeyDown;

	
	/*************************************************************************
	*
	* Input
	*
	**************************************************************************/

	// Keyboard buttons
    void AltUp();
	void AltDown();

	void CtrlUp();
	void CtrlDown();

	void ShiftKeyDown();
	void ShiftKeyUp();

	
	void EKeyUp();
	void EKeyDown();
	bool bEKeyPressed;

	UFUNCTION(BlueprintCallable)
	void SpaceKeyDown();
	UFUNCTION(BlueprintCallable)
	void SpaceKeyUp();
	
	// Mouse buttons 
	void LMBPressed();
	void LMBReleased();
	void RMBPressed();
	void RMBReleased();

	UPROPERTY(VisibleAnywhere,BlueprintReadWrite, Category="Input | Booleans")
    bool bIsLMBPressed;

	UPROPERTY(VisibleAnywhere,BlueprintReadWrite, Category="Input | Booleans")
    bool bIsRMBPressed;
    
	UPROPERTY(EditDefaultsOnly,BlueprintReadOnly,Category="Items")
	class AWeapon*EquippedWeapon;

	UPROPERTY(VisibleAnywhere,BlueprintReadOnly,Category="Items")
	class AItem*ActiveOverlappItem;

	UFUNCTION(BlueprintCallable)
	void PlaySwingSound(USoundCue*SwingSoundCue);

	void SetEquippedWeapon(AWeapon*WeaponToSet);
	FORCEINLINE AWeapon* GetEquippedWeapon(){return EquippedWeapon;}
	FORCEINLINE void SetActiveOvelappingItem(AItem*ActiveItem){ActiveOverlappItem=ActiveItem;}

	/** Base turn rate, in deg/sec. Other scaling may affect final turn rate. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category=Camera)
	float BaseTurnRate;

	/** Base look up/down rate, in deg/sec. Other scaling may affect final rate. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category=Camera)
	float BaseLookUpRate;

	/************************************************************************************
	 *
	 * Camera
	 *
	 ************************************************************************************/
	
	/** Camera boom positioning the camera behind the character */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
    class USpringArmComponent* SpringArm;

	/** Follow camera */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
    class UCameraComponent* FollowCamera;

	/** Returns SpringArm subobject **/
	FORCEINLINE class USpringArmComponent* GetCameraBoom() const { return SpringArm; }
	/** Returns FollowCamera subobject **/
	FORCEINLINE class UCameraComponent* GetFollowCamera() const { return FollowCamera; }

protected:

	/** Resets HMD orientation in VR. */
	void OnResetVR();

	/** Called for forwards/backward input */
	void MoveForward(float Value);

	/** Called for side to side input */
	void MoveRight(float Value);

	/** 
	 * Called via input to turn at a given rate. 
	 * @param Rate	This is a normalized rate, i.e. 1.0 means 100% of desired turn rate
	 */
	void TurnAtRate(float Rate);

	/**
	 * Called via input to turn look up/down at a given rate. 
	 * @param Rate	This is a normalized rate, i.e. 1.0 means 100% of desired turn rate
	 */
	void LookUpAtRate(float Rate);

	/** Handler for when a touch input begins. */
	void TouchStarted(ETouchIndex::Type FingerIndex, FVector Location);

	/** Handler for when a touch input stops. */
	void TouchStopped(ETouchIndex::Type FingerIndex, FVector Location);

	// APawn interface
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	// End of APawn interface

	
public:
	
	UPROPERTY(EditDefaultsOnly,BlueprintReadOnly,Category="Player Stats")
    float MaxHealth;
	
	UPROPERTY(EditAnywhere,BlueprintReadWrite, Category="Player Stats")
	float CurrentHealth;
	
	UPROPERTY(EditDefaultsOnly,BlueprintReadOnly,Category="Player Stats")
	float MaxStamina;

	UPROPERTY(EditDefaultsOnly,BlueprintReadOnly,Category="Player Stats")
	float MinStamina;
	
	UPROPERTY(EditAnywhere,BlueprintReadWrite,Category="Player Stats")
	float CurrentStamina;

	UPROPERTY(EditDefaultsOnly,BlueprintReadOnly,Category="Player Stats")
	float CurrentMana;
	
	UPROPERTY(EditAnywhere,BlueprintReadWrite,Category="Player Stats")
	float MaxMana;
	
	UPROPERTY(EditDefaultsOnly,BlueprintReadWrite,Category="Player Stats")
	int32 Coins;
	
	void DecrementHealth(float HealthAmount);

	UFUNCTION(BlueprintCallable)
	void IncrementHealth(float HealthAmount);

	UFUNCTION(BlueprintCallable)
	void IncrementCoins (int32 CoinsAmount);

	void Death();

	virtual float  TakeDamage(
		float DamageAmount,
		struct FDamageEvent const & DamageEvent,
		class AController * EventInstigator,
		AActor * DamageCauser) override;

	UPROPERTY(VisibleAnywhere,BlueprintReadOnly,Category="Combat")
	bool bAttacking;

	UPROPERTY(EditAnywhere,BlueprintReadWrite,Category="Combat")
	bool bFirstTouchToWeapon;

	void Attack();
	
	UFUNCTION(BlueprintCallable)
	void AttackEnd();

	UPROPERTY(EditDefaultsOnly,BlueprintReadOnly,Category="Combat")
	class UAnimMontage*	CombatMontage;

	UPROPERTY(EditAnywhere,BlueprintReadOnly,Category="Combat")
	class UParticleSystem*HitParticles;

	UPROPERTY(EditAnywhere,BlueprintReadWrite,Category="Combat")
	class USoundCue*HitSound;

	/* Interpolating to the Enemy */
	float InterpSpeed;

	bool bInterpToEnemy;

	void SetInterpToEnemy(bool Interp);

	UPROPERTY(VisibleAnywhere,BlueprintReadOnly,Category="Combat")
	class AEnemy*EnemyIsTargetForCharacter;

	FORCEINLINE void SetCombatTarget(AEnemy*Target) const {Target = EnemyIsTargetForCharacter;}

	UPROPERTY(VisibleAnywhere,BlueprintReadOnly,Category="Combat")
	bool bHasCombatTarget;

	FRotator GetLookAtRotationYaw(FVector Target);

	UPROPERTY(VisibleAnywhere,BlueprintReadOnly,Category="Controller")
	class AMainPlayerController*MainPlayerController;

	UPROPERTY(VisibleAnywhere,BlueprintReadWrite,Category="Combat")
	FVector CombatTargetLocation;

	void UpdateCombatTarget();

	/* Used in UpdateCombatTarget() to filter unnecessary classes which we dont expect to collect in Array */
	UPROPERTY(EditAnywhere,BlueprintReadWrite,Category="Combat")
	TSubclassOf<AEnemy>EnemyFilter;

	FORCEINLINE void SetHasCombatTarget(bool HasTarget){bHasCombatTarget = HasTarget;}

	UFUNCTION(BlueprintCallable)
	void DeathEnd();

	virtual void Jump() override;



	/* Switching level by name */
	void SwitchLevel(FName LevelName);

	UFUNCTION(BlueprintCallable)
	void SaveGame();

	UFUNCTION(BlueprintCallable)
	void LoadGame(bool SetPosition);
	
private:

	// Updating every frame 
	virtual void Tick(float DeltaSeconds) override;

	virtual void BeginPlay() override;
};

