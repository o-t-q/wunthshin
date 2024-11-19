// Fill out your copyright notice in the Description page of Project Settings.


#include "WG_WSDamageCounter.h"

#include "Components/TextBlock.h"
#include "Components/WidgetComponent.h"

UDamageCounterPool::UDamageCounterPool()
{
	if (static ConstructorHelpers::FClassFinder<UUserWidget> DefaultClass(TEXT("/Script/UMGEditor.WidgetBlueprint'/Game/ThirdPerson/Blueprints/Widgets/BP_WG_WSDamageIndicator.BP_WG_WSDamageIndicator_C'"));
		DefaultClass.Succeeded())
	{
		DefaultWidgetClass = DefaultClass.Class;
	}
}

void UDamageCounterPool::Initialize(AActor* InAttachTo, const TFunction<void(UWidgetComponent*)>& InFnPostInitialization, UMeshComponent* InMeshComponent)
{
	for (uint32 i = 0; i < MaxAllocation; ++i)
	{
		UWidgetComponent* WidgetComponent = NewObject<UWidgetComponent>(InAttachTo, UWidgetComponent::StaticClass(), NAME_None, RF_Transient);
		
		if (InMeshComponent)
		{
			WidgetComponent->SetupAttachment(InMeshComponent, "HeadSocket");
		}
		else
		{
			WidgetComponent->SetupAttachment(InAttachTo->GetRootComponent());
		}
		
		WidgetComponent->SetWidgetSpace(EWidgetSpace::Screen);
		WidgetComponent->SetDrawSize({100, 30});
		WidgetComponent->SetWidgetClass(DefaultWidgetClass);
		WidgetComponent->SetVisibility(false);
		WidgetComponent->RegisterComponent();

		if (InFnPostInitialization)
		{
			InFnPostInitialization(WidgetComponent);
		}
		
		Widgets.Push(FDamageCounterContext(i, WidgetComponent));
	}
}

void UDamageCounterPool::Allocate(const float Damage)
{
	// 비트가 플립되지 않은 번호를 찾는다
	int Available = _tzcnt_u32(AllocationMask) - 1;

	// 모든 비트가 사용중일 경우 끝에서 돌면서 다시 재사용
	if (Available == 0)
	{
		Available = FMath::Abs(std::numeric_limits<uint32>::digits - ConsecutiveOverflow) % std::numeric_limits<uint32>::digits + 1;
		Deallocate(Available);
		++ConsecutiveOverflow;
	}
	else
	{
		ConsecutiveOverflow = 0;
	}
	
	AllocationMask |= 1 << Available;
	
	FDamageCounterContext& Context = Widgets[Available];
	UWG_WSDamageCounter* DamageIndicator = Cast<UWG_WSDamageCounter>(Context.GetWidget()->GetWidget());

	// 데미지를 설정하고 Visibility를 바꾼 다음 애니메이션을 재생한다
	DamageIndicator->GetDamageText()->SetText(FText::FromString(FString::FromInt(static_cast<int32>(Damage))));
	Context.GetWidget()->SetVisibility(true);
	DamageIndicator->PlayAnimation(DamageIndicator->GetFlyUpAnimation());

	// 일정 시간이 지나고 나서 해당 위젯을 해제
	FTimerDelegate TimerDelegate;
	TimerDelegate.BindUObject(this, &UDamageCounterPool::Deallocate, Context.GetIdentifier());
	Context.GetWidget()->GetWorld()->GetTimerManager().SetTimer
	(
		Context.GetTimerHandle(),
		TimerDelegate,
		1.f,
		false,
		-1
	);
}

void UDamageCounterPool::Deallocate(const short InIndex)
{
	// 위젯을 숨기고 다시 사용 가능함으로 표기
	Widgets[InIndex].GetWidget()->SetVisibility(false);

	// 시간 초과 이전에 초기화가 된 경우
	if (Widgets[InIndex].GetTimerHandle().IsValid())
	{
		Widgets[InIndex].GetWidget()->GetWorld()->GetTimerManager().ClearTimer(Widgets[InIndex].GetTimerHandle());
	}
	
	AllocationMask &= ~(1 << InIndex);
}
