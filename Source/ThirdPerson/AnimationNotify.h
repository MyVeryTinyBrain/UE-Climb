// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AnimNotifyEx.h"
#include "AnimationNotify.generated.h"

/**
 * 
 */
UCLASS()
class THIRDPERSON_API UAnimationNotify : public UAnimNotifyEx
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, meta = (AllowPrivateAccess = true))
	FString notifyName = TEXT("AnimationNotify");

public:
	FString GetNotifyName_Implementation() const override { return notifyName; }
};
