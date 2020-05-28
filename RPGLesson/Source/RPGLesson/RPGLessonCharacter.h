// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include <PxQueryReport.h>

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "GameFramework/GameMode.h"
#include "RPGLessonCharacter.generated.h"

UCLASS(config=Game)
class ARPGLessonCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	ARPGLessonCharacter();

	/** Base turn rate, in deg/sec. Other scaling may affect final turn rate. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category=Camera)
	float BaseTurnRate;

	/** Base look up/down rate, in deg/sec. Other scaling may affect final rate. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category=Camera)
	float BaseLookUpRate;

	/** Camera boom positioning the camera behind the character */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
    class USpringArmComponent* SpringArm;

	/** Follow camera */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
    class UCameraComponent* FollowCamera;

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
	
	/** Returns SpringArm subobject **/
	FORCEINLINE class USpringArmComponent* GetCameraBoom() const { return SpringArm; }
	/** Returns FollowCamera subobject **/
	FORCEINLINE class UCameraComponent* GetFollowCamera() const { return FollowCamera; }

	////////
	///
	/// Interacting with the World, using debug line as a indicator when we hit something 
	///
	///
	///////

	// Sensible value, which shows how far we can interact with smth. By default it's 100.f;
	UPROPERTY(EditAnywhere,BlueprintReadOnly,Category="Interaction")
    float TraceDistance;

	//Storing result from the LineTrace function. 
	bool bHit;

	// Draw a debug line
	void LineTrace();

	//TODO: get an XYZ information of point , where we hit at and output to the LOG 
	FHitResult ResultOfHit;

private:

	// Updating every frame 
	virtual void Tick(float DeltaSeconds) override;
};

