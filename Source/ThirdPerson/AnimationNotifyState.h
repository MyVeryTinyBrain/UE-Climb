// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AnimNotifyStateEx.h"
#include "AnimationNotifyState.generated.h"

/**
 * 
 */
UCLASS()
class THIRDPERSON_API UAnimationNotifyState : public UAnimNotifyStateEx
{
	GENERATED_BODY()
	
	UPROPERTY(EditAnywhere, meta = (AllowPrivateAccess = true))
	FString notifyName = TEXT("AnimationNotifyState");

public:
	FString GetNotifyName_Implementation() const override { return notifyName; }
};
