// Copyright Epic Games, Inc. All Rights Reserved.

#include "RPGLessonGameMode.h"
#include "RPGLessonCharacter.h"
#include "UObject/ConstructorHelpers.h"

ARPGLessonGameMode::ARPGLessonGameMode()
{
	// set default pawn class to our Blueprinted character
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnBPClass(TEXT("/Game/ThirdPersonCPP/Blueprints/ThirdPersonCharacter"));
	if (PlayerPawnBPClass.Class != NULL)
	{
		DefaultPawnClass = PlayerPawnBPClass.Class;
	}
}
