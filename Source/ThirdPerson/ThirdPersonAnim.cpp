// Fill out your copyright notice in the Description page of Project Settings.


#include "ThirdPersonAnim.h"

void UThirdPersonAnim::NativeUpdateAnimation(float DeltaSeconds)
{
	Super::NativeUpdateAnimation(DeltaSeconds);
}

void UThirdPersonAnim::SetMoveDirection(FVector2D direction)
{
	moveDirection = direction.GetSafeNormal();

	if (moveDirection.SquaredLength() > 0.01f)
	{
		isMove = true;
		
		static const FVector2D forward = FVector2D(0, 1);
		float z = FVector2D::CrossProduct(forward, moveDirection);

		float angle = FMath::RadiansToDegrees((acosf(FVector2D::DotProduct(forward, moveDirection))));
		float normalizedAngle = angle / 180.0f;

		// To Right			: z < 0
		// Same Direction	: z = 0
		// To Left			: z > 0
		float cross = 1.0f;
		if (z < 0)
			cross = -1;
		else if (z > 0)
			cross = +1;

		// Keep direction
		normalizedDirection = normalizedAngle * cross;
	}
	else
	{
		isMove = false;
	}
}

void UThirdPersonAnim::SetNormalizedSpeed(float NormalizedSpeed)
{
	this->normalizedSpeed = NormalizedSpeed;
}

void UThirdPersonAnim::SetCharacterState(ECharacterState state)
{
	characterState = state;
}
