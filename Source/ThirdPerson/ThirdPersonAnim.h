// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Ex.h"
#include "ThirdPersonEx.h"
#include "AnimInstanceEx.h"
#include "ThirdPersonAnim.generated.h"

/**
 * 
 */
UCLASS()
class THIRDPERSON_API UThirdPersonAnim : public UAnimInstanceEx
{
	GENERATED_BODY()

protected:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Variable, meta = (AllowPrivateAccess = true))
	bool isMove = false;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Variable, meta = (AllowPrivateAccess = true))
	FVector2D moveDirection = FVector2D::ZeroVector;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Variable, meta = (AllowPrivateAccess = true))
	float normalizedDirection = 0.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Variable, meta = (AllowPrivateAccess = true))
	float normalizedSpeed = 0.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Variable, meta = (AllowPrivateAccess = true))
	ECharacterState characterState = ECharacterState::Default;

protected:
	virtual void NativeUpdateAnimation(float DeltaSeconds) override;

public:
	void SetMoveDirection(FVector2D direction);
	void SetNormalizedSpeed(float NormalizedSpeed);
	void SetCharacterState(ECharacterState state);
};
