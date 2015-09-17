// Copyright 1998-2015 Epic Games, Inc. All Rights Reserved.

#include "Character.h"
#include "CharacterGameMode.h"
#include "CharacterCharacter.h"

ACharacterGameMode::ACharacterGameMode()
{
	// set default pawn class to our Blueprinted character
   static ConstructorHelpers::FClassFinder<APawn> PlayerPawnBPClass(TEXT("/Game/MyStuff/Blueprints/MyCR_MainCharacter")); ///Game/ThirdPersonCPP/Blueprints/ThirdPersonCharacter
	if (PlayerPawnBPClass.Class != NULL)
	{
		DefaultPawnClass = PlayerPawnBPClass.Class;
	}
}
