#include "FMathEx.h"

template<typename T>
inline T FMathEx::Min(const T& a, const T& b)
{
	return a < b ? a : b;
}

template<typename T>
inline T FMathEx::Max(const T& a, const T& b)
{
	return a > b ? a : b;
}

float FMathEx::BetweenAngle(const FVector& a, const FVector& b)
{
	return acosf(FVector::DotProduct(a.GetSafeNormal(), b.GetSafeNormal())) * Rad2Deg;
}

float FMathEx::BetweenAngle(const FVector2D& a, const FVector2D& b)
{
	return acosf(FVector2D::DotProduct(a.GetSafeNormal(), b.GetSafeNormal())) * Rad2Deg;
}

double FMathEx::BetweenRadian(const FQuat& a, const FQuat& b)
{
	double dotProduct = a.X * b.X + a.Y * b.Y + a.Z * b.Z + a.W * b.W;
	return acos(Min(abs(dotProduct), 1.0)) * 2.0;
}

float FMathEx::BetweenAngle(const FQuat& a, const FQuat& b)
{
	return BetweenRadian(a, b) * Rad2Deg;
}

FVector FMathEx::BezierCurve(const FVector& start, const FVector& control, const FVector& end, float t)
{
	FVector u1 = FMath::Lerp(start, control, t);
	FVector u2 = FMath::Lerp(control, end, t);

	FVector u3 = FMath::Lerp(u1, u2, t);
	return u3;
}

FVector2D FMathEx::BezierCurve(const FVector2D& start, const FVector2D& control, const FVector2D& end, float t)
{
	FVector2D u1 = FMath::Lerp(start, control, t);
	FVector2D u2 = FMath::Lerp(control, end, t);

	FVector2D u3 = FMath::Lerp(u1, u2, t);
	return u3;
}

FVector FMathEx::BezierCurve(const FVector& start, const FVector& controlA, const FVector& controlB, const FVector& end, float t)
{
	FVector u1 = FMath::Lerp(start, controlA, t);
	FVector u2 = FMath::Lerp(controlA, controlB, t);
	FVector u3 = FMath::Lerp(controlB, end, t);

	FVector u4 = FMath::Lerp(u1, u2, t);
	FVector u5 = FMath::Lerp(u2, u3, t);

	FVector u6 = FMath::Lerp(u4, u5, t);
	return u6;
}

FVector2D FMathEx::BezierCurve(const FVector2D& start, const FVector2D& controlA, const FVector2D& controlB, const FVector2D& end, float t)
{
	FVector2D u1 = FMath::Lerp(start, controlA, t);
	FVector2D u2 = FMath::Lerp(controlA, controlB, t);
	FVector2D u3 = FMath::Lerp(controlB, end, t);

	FVector2D u4 = FMath::Lerp(u1, u2, t);
	FVector2D u5 = FMath::Lerp(u2, u3, t);

	FVector2D u6 = FMath::Lerp(u4, u5, t);
	return u6;
}

FQuat FMathEx::RotateTowards(const FQuat& from, const FQuat& to, float maxDeltaAngle)
{
	float angle = BetweenAngle(from, to);
	if (angle == 0) 
		return to;
	float t = Min(1.0f, maxDeltaAngle / angle);
	return FQuat::Slerp(from, to, t);
}
