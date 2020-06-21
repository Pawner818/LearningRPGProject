// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "GameFramework/GameMode.h"
#include "UObject/ObjectMacros.h"

#include "RPGLessonCharacter.generated.h"

UENUM(BlueprintType)
enum class EMovementStatus : uint8
{
	EMS_Normal UMETA(DisplayName = "Normal"),
	EMS_Sprinting UMETA(DisplayName = "Sprinting"),
	EMS_MAX UMETA(DisplayName = "DefaultMAX")
};

UENUM(BlueprintType)
enum class EStaminaStatus:uint8
{
	ESS_Normal UMETA(DisplayName = "Normal"),
	ESS_BelowMinimum UMETA(DisplayName = "BelowMinimum"),
	ESS_Exhausted UMETA(DisplayName = "Exhausted"),
	ESS_ExhaustedRecovering UMETA(DisplayName = "ExhaustedRecovering"),

	ESS_MAX UMETA(DisplayName = "DefaultMAX")
};


UCLASS(config=Game)
class ARPGLessonCharacter : public ACharacter
{
	GENERATED_BODY()

public:

	ARPGLessonCharacter();

	TArray<FVector>PickupLocation;
	
	UFUNCTION(BlueprintCallable)
    void ShowPickupLocation();
	
	/***********
	 *
	 * Stamina
	 * 
	 **********/
	
	// Different status for the stamina bar. there are 4 status: the first one is active by default and it's called
	// "Normal". When we using sprint we use stamina as well. when we reach "BelowMinimum" status, the StaminaBar changes
	// it's colour from green to red - it shows that we're starting to be exhausted. "Exhausted" status will show the
	// lowest possible value of stamina and stop sprinting. Right after that automatically begins the last status - ExhaustedRecovering,
	// which refills the StaminaBar, and return our colour to green.
	UPROPERTY(VisibleAnywhere,BlueprintReadWrite,Category="ENUMS")
	EStaminaStatus StaminaStatus;

    
	FORCEINLINE void SetStaminaStatus (EStaminaStatus Status){Status = StaminaStatus;};

	UPROPERTY(EditAnywhere,BlueprintReadWrite, Category="Movement")
	float StaminaDrainRate;
	
	UPROPERTY(EditAnywhere,BlueprintReadWrite, Category="Movement")
	float MinSprintStamina;

	/************
	 *
	 * Movement 
	 *
	 ************/
	
	// Describes what status the Character is in 
	UPROPERTY(VisibleAnywhere,BlueprintReadWrite,Category="ENUMS")
    EMovementStatus MovementStatus;
	
	// Set movement status and running speed.
	UFUNCTION(BlueprintCallable,Category="Enums")
	void SetMovementStatus(EMovementStatus Status);

	UPROPERTY(EditAnywhere,BlueprintReadOnly,Category="Running")
	float RunningSpeed;
	
	UPROPERTY(EditAnywhere,BlueprintReadOnly,Category="Running")
	float SprintingSpeed;

	// Simple togle to know the state of this button
	bool bShiftKeyDown;

	// Press down to enable sprinting
	void ShiftKeyDown();

	// Realeased to stop sprinting 
	void ShiftKeyUp();
	
	/** Base turn rate, in deg/sec. Other scaling may affect final turn rate. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category=Camera)
	float BaseTurnRate;

	/** Base look up/down rate, in deg/sec. Other scaling may affect final rate. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category=Camera)
	float BaseLookUpRate;

	/************
	 *
	 * Camera
	 *
	 ************/
	
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
	
	
	////////
	///
	/// Interacting with the World, using debug line as a indicator when we hit something, overlapping delegate functions 
	///
	///////

	// Sensible value, which shows how far we can interact with smth. By default it's 100.f;
	UPROPERTY(EditAnywhere,BlueprintReadOnly,Category="Interaction")
    float TraceDistance;

	//Storing result from the LineTrace function. 
	bool bHit;

	void OnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult & SweepResult);
	
	void OnOverlapEnd (UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);



	UPROPERTY(EditDefaultsOnly,BlueprintReadOnly,Category="Player Stats")
    float MaxHealth;
	
	UPROPERTY(EditAnywhere,BlueprintReadWrite, Category="Player Stats")
	float Health;
	
	UPROPERTY(EditDefaultsOnly,BlueprintReadOnly,Category="Player Stats")
	float MaxStamina;
	
	UPROPERTY(EditAnywhere,BlueprintReadWrite,Category="Player Stats")
	float Stamina;
	
	UPROPERTY(EditDefaultsOnly,BlueprintReadWrite,Category="Player Stats")
	int32 Coins;

	/**********
	///
	/// Stats changing functions 
	///
	**********/
	
	void DecrementHealth(float HealthAmount);

	void IncrementHealth(float HealthAmount);

	void IncrementCoins (int32 CoinsAmount);

	void Death();
	
private:

	// Updating every frame 
	virtual void Tick(float DeltaSeconds) override;

	virtual void BeginPlay() override;
};

