// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "Engine/World.h"
#include "Creature.generated.h"

UCLASS()
class RPGLESSON_API ACreature : public APawn
{
	GENERATED_BODY()

public:
	// Sets default values for this pawn's properties
	ACreature();
	
	UPROPERTY(EditAnywhere,Category="Camera")
	class UCameraComponent*Camera;
	
	UPROPERTY(EditAnywhere,Category="Camera")
	class USpringArmComponent*SpringArm;
	
	UPROPERTY(EditInstanceOnly, BlueprintReadOnly, Category="Speed")
	float MaxSpeed;


protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	UPROPERTY(EditAnywhere, Category="Mesh")
    class UStaticMeshComponent* MeshComponent;

	FORCEINLINE UStaticMeshComponent*GetMeshComponent() const {return MeshComponent;}
	FORCEINLINE void SetMeshComponent(UStaticMeshComponent*Mesh){MeshComponent=Mesh;};

	private:

	// Some movement functionality 
	void MoveForward(float Value);
	void MoveRight(float Value);
	FVector CurrentVelocity;
};
