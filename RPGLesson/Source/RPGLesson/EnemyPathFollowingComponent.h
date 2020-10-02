// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Navigation/PathFollowingComponent.h"
#include "EnemyPathFollowingComponent.generated.h"

/**
 * 
 */
UCLASS()
class RPGLESSON_API UEnemyPathFollowingComponent : public UPathFollowingComponent
{
	GENERATED_BODY()

	public:


	
	/** follow current path segment */
	virtual void FollowPathSegment(float DeltaTime) override;

	/** sets variables related to current move segment */
	virtual void SetMoveSegment(int32 SegmentStartIndex) override;
	
	/** check state of path following, update move segment if needed */
	virtual void UpdatePathSegment() override;
	
};
