// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Item.h"
#include "Weapon.generated.h"


UCLASS()
class RPGLESSON_API AWeapon : public AItem
{
	GENERATED_BODY()

public:

	AWeapon();

	UPROPERTY(VisibleAnywhere,BlueprintReadWrite,Category="SekeletalMesh")
	class USkeletalMeshComponent*SkeletalMeshComponent;

	UPROPERTY(VisibleAnywhere,BlueprintReadOnly,Category="Combat")
	class UBoxComponent*DamageBoxComponent;

	UPROPERTY(VisibleAnywhere,BlueprintReadOnly,Category = "Particles")
	class UParticleSystemComponent*ParticleSystemComponent;

	UPROPERTY(EditAnywhere,BlueprintReadWrite,Category="Combat")
	float Damage;

	protected:
	
	virtual void BeginPlay() override;

	virtual void Tick(float DeltaSeconds) override;

	virtual void OnOverlapBegin (UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult & SweepResult) override;

	virtual void OnOverlapEnd (UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex) override;

	UFUNCTION()
    void OnCombatBegin (UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult & SweepResult);

	UFUNCTION()
    void OnCombatEnd (UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);
	
};
