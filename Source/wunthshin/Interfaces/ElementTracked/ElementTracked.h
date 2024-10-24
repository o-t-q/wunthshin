// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

#include "ElementTracked.generated.h"

struct FElementRowHandle;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnElementApplied, const FElementRowHandle&, InElementHandle);

// This class does not need to be modified.
UINTERFACE()
class UElementTracked : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class WUNTHSHIN_API IElementTracked
{
	GENERATED_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:

	// 물체에 원소가 적용된 경우
	FOnElementApplied OnElementApplied;
	
	/**
	 * @brief 해당 물체에 원소를 적용함 
	 * @param ThisObject 해당 물체의 포인터
	 * @param InInstigator 적용되는 물체에 원소를 적용시킨 물체
	 * @param InHandle 적용될 원소의 테이블 핸들
	 */
	static void ApplyElement(UObject* ThisObject, AActor* InInstigator, const FElementRowHandle& InHandle);
};
