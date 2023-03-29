// Copyright Epic Games, Inc. All Rights Reserved.

#include "ThirdPersonGameMode.h"
#include "ThirdPersonCharacter.h"
#include "UObject/ConstructorHelpers.h"
#include "PlayerCharacter.h"

AThirdPersonGameMode::AThirdPersonGameMode()
{
	//DefaultPawnClass = APlayerCharacter::StaticClass();

	ConstructorHelpers::FClassFinder<ACharacter> defaultPawnClassFinder(TEXT("/Game/Characters/YBot/PlayerCharacter.PlayerCharacter_C"));
	DefaultPawnClass = defaultPawnClassFinder.Class;
}
