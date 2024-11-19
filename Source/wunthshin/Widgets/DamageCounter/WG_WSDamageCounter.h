// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "WG_WSDamageCounter.generated.h"

class UWidgetComponent;

class UTextBlock;
class UCanvasPanel;
/**
 * 
 */
UCLASS()
class WUNTHSHIN_API UWG_WSDamageCounter : public UUserWidget
{
	GENERATED_BODY()
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true", BindWidget))
	UCanvasPanel* Canvas;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true", BindWidget))
	UTextBlock* DamageText;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true", BindWidgetAnim), Transient)
	UWidgetAnimation* FlyUpAnimation;

public:
	UTextBlock* GetDamageText() const { return DamageText; }
	UWidgetAnimation* GetFlyUpAnimation() const { return FlyUpAnimation; }
};

USTRUCT(BlueprintType)
struct FDamageCounterContext
{
	GENERATED_BODY()

	FDamageCounterContext() = default;
	explicit FDamageCounterContext(const short InIdentifier, UWidgetComponent* InComponent) : Identifier(InIdentifier), Widget(InComponent) {}

	short GetIdentifier() const { return Identifier; }
	UWidgetComponent* GetWidget() const { return Widget; }
	FTimerHandle& GetTimerHandle() { return TimerHandle; }
	
private:
	short Identifier;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	UWidgetComponent* Widget = nullptr;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	FTimerHandle TimerHandle;
	
};

UCLASS(BlueprintType)
class UDamageCounterPool : public UObject
{
	GENERATED_BODY()

	UDamageCounterPool();

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	TSubclassOf<UUserWidget> DefaultWidgetClass;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	TArray<FDamageCounterContext> Widgets;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	int32 ConsecutiveOverflow = 0;
	
	int32 AllocationMask = 0;

public:
	static constexpr uint32 MaxAllocation = (std::numeric_limits<int32>::digits + 1);

	void Initialize(AActor* InAttachTo, const TFunction<void(UWidgetComponent*)>& InFnPostInitialization = {}, UMeshComponent* InMeshComponent =
						nullptr);

	void Allocate(const float Damage);
	void Deallocate(const short InIndex);
	
};
