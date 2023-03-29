// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreInclude.h"
#include "Animation/AnimNotifies/AnimNotify.h"
#include "AnimNotifyEx.generated.h"

USTRUCT(BlueprintType)
struct FAnimNotifyData
{
	GENERATED_USTRUCT_BODY()

	const UAnimNotifyEx* AnimNotifyState;

	USkeletalMeshComponent* MeshComp;
	UAnimSequenceBase* Animation;
	const FAnimNotifyEventReference* EventReference;
};

DECLARE_DYNAMIC_DELEGATE_OneParam(FAnimNotifyCallback, const FAnimNotifyData&, data);

/**
 Callback Function Name: UClassName -> ClassName_Notify
 Callback Function Params: const FAnimNotifyData&
 */
UCLASS(Abstract)
class THIRDPERSON_API UAnimNotifyEx : public UAnimNotify
{
	GENERATED_BODY()
	
public:
	UAnimNotifyEx();

	FString GetCallbackFunctionName() const { return GetNotifyName() + "_Nofity"; }

protected:
	bool CanNotifyInEditorTool(USkeletalMeshComponent* MeshComp) const;
	virtual void Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference) final override;
	virtual void OnNotify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference);

	void CallAnimNotifyCallback(
		UObject* Object,
		USkeletalMeshComponent* MeshComp,
		UAnimSequenceBase* Animation,
		const FAnimNotifyEventReference& EventReference
	) const;
};
