// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "MainAnimIntance.generated.h"

/**
 * 
 */
UCLASS()
class RPGLESSON_API UMainAnimIntance : public UAnimInstance
{
	GENERATED_BODY()

public:

	virtual void NativeInitializeAnimation() override;

	// Update our animation like a Tick function
	UFUNCTION(BlueprintCallable,Category="Animations")
	void UpdateAnimationProperties();

	// Speed that we will use as a parameter to move
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category="Movement" )
	float MovementSpeed;

    UPROPERTY(BlueprintReadWrite, EditAnywhere, Category="Movement" )
    float Direction;

	// Are we in air?
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category="Movement" )
	bool IsInAir;
	
	// reference to the pawn
	UPROPERTY(EditAnywhere,BlueprintReadOnly, Category="Movement" )
	class APawn * Pawn;

	// refernce to the Character 
    UPROPERTY(EditAnywhere,BlueprintReadWrite, Category="Movement")
	class ARPGLessonCharacter*Character;

};
