// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreInclude.h"
#include "Animation/AnimNotifies/AnimNotifyState.h"
#include "AnimNotifyStateEx.generated.h"

UENUM(BlueprintType)
enum class EAnimNotifyStateEventType : uint8
{
	Begin,
	Tick,
	End,
};

USTRUCT(BlueprintType)
struct FAnimNotifyStateData
{
	GENERATED_USTRUCT_BODY()

	const UAnimNotifyStateEx* AnimNotifyState;
	EAnimNotifyStateEventType Type;

	USkeletalMeshComponent* MeshComp;
	UAnimSequenceBase* Animation;
	const FAnimNotifyEventReference* EventReference;
	float TotalDuration = 0; // Valid in Begin
	float FrameDeltaTime = 0; // Valid in Tick
};

USTRUCT(BlueprintType)
struct FAnimNotifyStateInfo
{
	GENERATED_USTRUCT_BODY()

	bool isValid;
	bool isInMontage;
	FAnimNotifyEvent* animNotifyEvent;
	float duration;
	float start;
	float end;
	float currentTime; // Valid during montage playing(invalid value = -1)
	float currentRatio; // Valid during montage playing(invalid value = -1)
};

DECLARE_DYNAMIC_DELEGATE_OneParam(FAnimNotifyStateCallback, const FAnimNotifyStateData&, data);

/**
 Callback Function Name: UClassName -> ClassName_NotifyState
 Callback Function Params: const FAnimNotifyStateData&
 */
UCLASS(Abstract)
class THIRDPERSON_API UAnimNotifyStateEx : public UAnimNotifyState
{
	GENERATED_BODY()

public:
	UAnimNotifyStateEx();
	
	FString GetCallbackFunctionName() const { return GetNotifyName() + "_NofityState"; }
	FAnimNotifyStateInfo GetAnimNotifyStateInfo(const FAnimNotifyStateData& data) const;

protected:
	bool CanNotifyInEditorTool(USkeletalMeshComponent* MeshComp) const;
	virtual void NotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float TotalDuration, const FAnimNotifyEventReference& EventReference) final override;
	virtual void NotifyTick(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float FrameDeltaTime, const FAnimNotifyEventReference& EventReference) final override;
	virtual void NotifyEnd(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference) final override;
	virtual void OnNotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float TotalDuration, const FAnimNotifyEventReference& EventReference);
	virtual void OnNotifyTick(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float FrameDeltaTime, const FAnimNotifyEventReference& EventReference);
	virtual void OnNotifyEnd(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference);

	void CallAnimNotifyStateCallback(
		UObject* Object,
		EAnimNotifyStateEventType Type,
		USkeletalMeshComponent* MeshComp,
		UAnimSequenceBase* Animation,
		const FAnimNotifyEventReference& EventReference,
		float TotalDuration = 0, // Valid in Begin
		float FrameDeltaTime = 0 // Valid in Tick
	) const;
};
