#pragma once

#include "CoreInclude.h"

const float Deg2Rad = (UE_PI / 180.f);
const float Rad2Deg = (180.f / UE_PI);

struct FMathEx
{
	template <typename T>
	static T Min(const T& a, const T& b);
	template <typename T>
	static T Max(const T& a, const T& b);

	static float BetweenAngle(const FVector& a, const FVector& b);
	static float BetweenAngle(const FVector2D& a, const FVector2D& b);
	static double BetweenRadian(const FQuat& a, const FQuat& b);
	static float BetweenAngle(const FQuat& a, const FQuat& b);

	static FVector BezierCurve(const FVector& start, const FVector& control, const FVector& end, float t);
	static FVector2D BezierCurve(const FVector2D& start, const FVector2D& control, const FVector2D& end, float t);
	static FVector BezierCurve(const FVector& start, const FVector& controlA, const FVector& controlB, const FVector& end, float t);
	static FVector2D BezierCurve(const FVector2D& start, const FVector2D& controlA, const FVector2D& controlB, const FVector2D& end, float t);

	static FQuat RotateTowards(const FQuat& from, const FQuat& to, float maxDeltaAngle);
};
