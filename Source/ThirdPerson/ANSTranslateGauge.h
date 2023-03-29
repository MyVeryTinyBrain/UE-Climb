// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AnimNotifyStateEx.h"
#include "ANSTranslateGauge.generated.h"

UENUM(BlueprintType)
enum class ETranslateGaugeType : uint8
{
	Linear,
	Parabola,
};

USTRUCT(BlueprintType)
struct FTranslateGaugeData
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int Slot = 0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FVector Offset = FVector::ZeroVector;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool ToggleFlyDuringTranslate = true;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, DisplayName = "ParabolaOffset(ParabolaOnly)", meta = (EditCondition = "Type == ETranslateGaugeType::Parabola"))
	float ParabolaOffset = 100.0f;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	ETranslateGaugeType Type = ETranslateGaugeType::Linear;
};

/**
 * 
 */
UCLASS()
class THIRDPERSON_API UANSTranslateGauge : public UAnimNotifyStateEx
{
	GENERATED_BODY()
	
	UPROPERTY(EditAnywhere, meta = (AllowPrivateAccess = true))
	FTranslateGaugeData data;

public:
	const FTranslateGaugeData& GetData() const { return data; }
};
