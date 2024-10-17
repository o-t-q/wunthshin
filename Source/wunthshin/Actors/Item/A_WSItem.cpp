// Fill out your copyright notice in the Description page of Project Settings.


#include "A_WSItem.h"

#include "Components/ShapeComponent.h"
#include "wunthshin/Components/PickUp/C_WSPickUp.h"

const FName AA_WSItem::CollisionComponentName = TEXT("Collision");

void AA_WSItem::InitializeCollisionLazy() const
{
	if (CollisionComponent)
	{
		// 충돌체가 루트에 부착되지 않았다면,
		if (CollisionComponent->GetAttachParent() != RootSceneComponent)
		{
			// SetupAttachment는 생성자에서 호출가능
			if (const FUObjectThreadContext& Context = FUObjectThreadContext::Get();
				Context.IsInConstructor)
			{
				CollisionComponent->SetupAttachment(RootSceneComponent);
			}
			else
			{
				CollisionComponent->AttachToComponent
				(
					RootSceneComponent,
					FAttachmentTransformRules::SnapToTargetNotIncludingScale
				);
			}
		}

		// 충돌 설정
		CollisionComponent->SetCollisionProfileName("ItemProfile");
		CollisionComponent->SetGenerateOverlapEvents(true);

		// 충돌체의 중심점은 원점, 바닥과 일치하도록 올리기
		CollisionComponent->SetRelativeLocation({0.f, 0.f, MeshComponent->Bounds.BoxExtent.Z});
	}
}

// Sets default values
AA_WSItem::AA_WSItem()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;
		
	// 추상 클래스인 ShapeComponent, CollisionComponent는 nullptr로 초기화됨
	// 다른 충돌체 모양을 호환하기 위해 유지, 상속하는 객체 또는 사용하는 객체에서 설정해야함
	CollisionComponent = CreateDefaultSubobject<UShapeComponent>(CollisionComponentName);
	
	RootSceneComponent = CreateDefaultSubobject<USceneComponent>(TEXT("RootScene"));
	MeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("StaticMesh"));
	PickUpComponent = CreateDefaultSubobject<UC_WSPickUp>(TEXT("PickUp"));

	SetRootComponent(RootSceneComponent);
	MeshComponent->SetupAttachment(RootSceneComponent);

	// Mesh 충돌 대신 CollisionComponent를 사용
	MeshComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	MeshComponent->SetSimulatePhysics(false);
	MeshComponent->SetGenerateOverlapEvents(false);
}

void AA_WSItem::OnConstruction(const FTransform& Transform)
{
	Super::OnConstruction(Transform);

	SetData(DataTableRowHandle);
}

void AA_WSItem::InitializeCollisionComponent(TSubclassOf<UShapeComponent> InClass)
{
	// 새로운 충돌체로의 교환
	if (CollisionComponent)
	{
		CollisionComponent->DestroyComponent();
	}
	
	CollisionComponent = NewObject<UShapeComponent>(this, InClass, CollisionComponentName);
	if (CollisionComponent)
	{
		CollisionComponent->RegisterComponent();
		InitializeCollisionLazy();
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("Unknown error! Collision component is not initialized properly!"));
	}
}

void AA_WSItem::SetData(const FDataTableRowHandle& InRowHandle)
{
	DataTableRowHandle = InRowHandle;
	if (DataTableRowHandle.IsNull()) return;

	FItemTableRow* Data = DataTableRowHandle.GetRow<FItemTableRow>(TEXT("Item"));
	if (!Data) { ensure(false); return; }

	if (Data->StaticMesh) MeshComponent->SetStaticMesh(Data->StaticMesh);
	
	// todo: Icon, ItemName 등 정보 추가
}

// Called when the game starts or when spawned
void AA_WSItem::BeginPlay()
{
	Super::BeginPlay();

	// note: 동적으로 설정한 충돌체의 초기화를 해야함 (블루프린트 또는 상속 클래스에서)
}
