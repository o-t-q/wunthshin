// Fill out your copyright notice in the Description page of Project Settings.


#include "Component/C_WSShield.h"

DEFINE_LOG_CATEGORY(LogShieldComponent);

// Sets default values for this component's properties
UC_WSShield::UC_WSShield()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = false;
	bWantsInitializeComponent = true;

	// ...
	// todo: 쉴드 매시 바꾸기
	if (static ConstructorHelpers::FObjectFinder<UStaticMesh> SM_Shield(TEXT("/Script/Engine.StaticMesh'/Game/ThirdPerson/Blueprints/Meshes/SM_Shield.SM_Shield'"));
		SM_Shield.Succeeded())
	{
		UStaticMeshComponent::SetStaticMesh(SM_Shield.Object);
	}

	// 쉴드를 처음에는 숨기고, 충돌을 무시함
	SetHiddenInGame(true);
	UPrimitiveComponent::SetCollisionEnabled(ECollisionEnabled::NoCollision);

	UActorComponent::SetAutoActivate(false);	
}

void UC_WSShield::SetActive(bool bNewActive, bool bReset)
{
	if ( GetNetMode() == NM_Client )
	{
		return;
	}

	Super::SetActive(bNewActive, bReset);

	if (bNewActive)
	{
		// 쉴드를 활성화하고 시간만큼 쉴드를 활성화 한 후 비활성화함
		SetHiddenInGame( false );
		UE_LOG(LogShieldComponent, Log, TEXT("Shield activated!"));
		FTimerDelegate OnShieldFinished;
		OnShieldFinished.BindUObject(this, &UC_WSShield::SetActive, false, false);
		
		GetWorld()->GetTimerManager().SetTimer
		(
			ShieldTimerHandle,
			OnShieldFinished,
			ShieldTimer,
			false
		);
	}
	else
	{
		// 시야에서 숨기고 컴포넌트를 비활성화함
		UE_LOG(LogShieldComponent, Log, TEXT("Shield deactivated!"));
		SetHiddenInGame( true );
		if (ShieldTimerHandle.IsValid())
		{
			GetWorld()->GetTimerManager().ClearTimer(ShieldTimerHandle);
		}
	}
}

void UC_WSShield::InitializeComponent()
{
	// note: InitializeComponent는 ExecuteConstruction(OnConstruction) 이후에 호출됨
	Super::InitializeComponent();

	if (const AActor* Pawn = GetOwner())
	{
		// todo: 애니메이션에 맞춰 쉴드도 움직이기
		// 첫번째 매시 컴포넌트가 외향 컴포넌트라고 가정
		const FBoxSphereBounds ActorBound = Pawn->GetComponentByClass<UMeshComponent>()->GetLocalBounds();
		const FBoxSphereBounds MeshBound = GetStaticMesh()->GetBounds();
		float Ratio = ActorBound.SphereRadius / MeshBound.SphereRadius;

		SetRelativeScale3D({Ratio, Ratio, Ratio});
	}
}


// Called when the game starts
void UC_WSShield::BeginPlay()
{
	Super::BeginPlay();

	// ..
}
