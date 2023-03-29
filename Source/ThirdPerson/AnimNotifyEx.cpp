// Fill out your copyright notice in the Description page of Project Settings.


#include "AnimNotifyEx.h"

UAnimNotifyEx::UAnimNotifyEx()
{
	bIsNativeBranchingPoint = true;
	bShouldFireInEditor = false;
}

bool UAnimNotifyEx::CanNotifyInEditorTool(USkeletalMeshComponent* MeshComp) const
{
	if (!bShouldFireInEditor)
	{
		if (!MeshComp->GetWorld())
			return true;
		if (MeshComp->GetWorld()->WorldType == EWorldType::EditorPreview)
			return true;
	}
	return false;
}

void UAnimNotifyEx::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference)
{
	Super::Notify(MeshComp, Animation, EventReference);
	
	if (CanNotifyInEditorTool(MeshComp))
		return;

	OnNotify(MeshComp, Animation, EventReference);
}

void UAnimNotifyEx::OnNotify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference)
{
	CallAnimNotifyCallback(MeshComp->GetAnimInstance(), MeshComp, Animation, EventReference);
	CallAnimNotifyCallback(MeshComp->GetOwner(), MeshComp, Animation, EventReference);
}

void UAnimNotifyEx::CallAnimNotifyCallback(UObject* Object, USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference) const
{
	if (!Object)
		return;

	UFunction* function = Object->FindFunction(*GetCallbackFunctionName());
	if (!function)
		return;

	if (function->NumParms != 1)
		return;

	FAnimNotifyData data;
	data.AnimNotifyState = this;

	data.MeshComp = MeshComp;
	data.Animation = Animation;
	data.EventReference = &EventReference;

	FAnimNotifyCallback Delegate;
	Delegate.BindUFunction(Object, *GetCallbackFunctionName());
	Delegate.Execute(data);
}
