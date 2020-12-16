// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Item.h"
#include "Weapon.generated.h"

UENUM(BlueprintType)
enum class EWeaponStates :uint8
{
	EWS_Pickup		UMETA (DisplayName = "Pickup"),
    EWS_Equipped	UMETA (DisplayName = "Equipped"),

    EWS_MAX			UMETA (DisplayName = "DefaultMAX")
};


UCLASS()
class RPGLESSON_API AWeapon : public AItem
{
	GENERATED_BODY()

public:

	AWeapon();
	
	UPROPERTY(VisibleAnywhere,BlueprintReadWrite,Category="Item")
	EWeaponStates WeaponState;

	FORCEINLINE void SetWeaponState(EWeaponStates State) {WeaponState = State;}
	FORCEINLINE EWeaponStates GetWeaponState() const {return WeaponState;}

	void EquipWeapon(class ARPGLessonCharacter*Character);
	
	UPROPERTY(VisibleAnywhere,BlueprintReadOnly,Category = "Particles")
	class UParticleSystem*ParticleSystem;

	UPROPERTY(EditAnywhere,BlueprintReadOnly,Category = "Item | Particles")
	bool bWeaponParticle;

	UPROPERTY(EditAnywhere,BlueprintReadWrite,Category="Item | Sound")
	class USoundCue*OnEquppedSound;
	
	UPROPERTY(EditAnywhere,BlueprintReadWrite,Category="Item | Sound")
    USoundCue*SwingSound;

	UPROPERTY(VisibleAnywhere,BlueprintReadWrite,Category="SekeletalMesh")
	class USkeletalMeshComponent*SkeletalMeshComponent;

	UPROPERTY(VisibleAnywhere,BlueprintReadOnly,Category="Combat")
	class UBoxComponent*DamageBoxComponent;

	UPROPERTY(VisibleAnywhere,BlueprintReadOnly,Category = "Particles")
	class UParticleSystemComponent*ParticleSystemComponent;

	protected:
	
	virtual void BeginPlay() override;

	virtual void Tick(float DeltaSeconds) override;

	UFUNCTION(BlueprintCallable)
	void ActivateCollision();

	UFUNCTION(BlueprintCallable)
	void DeactivateCollision();

	virtual void OnOverlapBegin (UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult & SweepResult) override;

	virtual void OnOverlapEnd (UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex) override;

	UFUNCTION()
    void OnCombatBegin (UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult & SweepResult);

	UFUNCTION()
    void OnCombatEnd (UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);
	
};
