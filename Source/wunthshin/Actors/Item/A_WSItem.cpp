// Fill out your copyright notice in the Description page of Project Settings.


#include "A_WSItem.h"

#include "Components/BoxComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/ShapeComponent.h"
#include "Components/SphereComponent.h"

#include "wunthshin/Components/PickUp/C_WSPickUp.h"
#include "wunthshin/Data/ItemTableRow.h"
#include "wunthshin/Data/CharacterTableRow.h"
#include "wunthshin/Data/ItemMetadata/SG_WSItemMetadata.h"

class USphereComponent;
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
	}
}

// Sets default values
AA_WSItem::AA_WSItem(const FObjectInitializer& ObjectInitializer) :
	Super(ObjectInitializer)
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
	FetchAsset(this, AssetName);
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

UScriptStruct* AA_WSItem::GetTableType() const
{
	return FItemTableRow::StaticStruct();
}

void AA_WSItem::UpdateCollisionFromDataTable(const FItemTableRow* Data)
{
	if (Data->CollisionShape)
	{
		InitializeCollisionComponent(Data->CollisionShape);

		// 추가적으로 설정된 충돌체 크기가 없다면 자동으로 설정
		if ((Data->bBox && Data->BoxExtents.IsZero()) ||
		    (Data->bSphere && Data->Radius == 0.f) ||
		    (Data->bCapsule && Data->CapsuleRadius == 0.f && Data->CapsuleHeight == 0.f))
		{
			FitCollisionToMesh();
		}

		// 충돌체의 원점을 물체의 중간으로 옮김
		FTransform Offset = Data->CollisionOffset;
		Offset.SetTranslation({0.f, 0.f, MeshComponent->Bounds.BoxExtent.Z});
		CollisionComponent->SetRelativeTransform(Offset);
	}
}

void AA_WSItem::ApplyAsset(const FDataTableRowHandle& InRowHandle)
{
	// todo: 상속 클래스의 타입을 사용하기
	if (InRowHandle.IsNull()) return;

	const FItemTableRow* Data = InRowHandle.GetRow<FItemTableRow>(TEXT(""));

	if (!Data)
	{
		return;
	}

	if (Data->StaticMesh) MeshComponent->SetStaticMesh(Data->StaticMesh);

	UpdateCollisionFromDataTable(Data);

	if (Data->GlobalItemMetadataPointer) 
	{
		ItemMetadata = Data->GlobalItemMetadataPointer;
	}
	
	// todo: Icon, ItemName 등 정보 추가
}

const USG_WSItemMetadata* AA_WSItem::GetItemMetadata() const
{
	return ItemMetadata;
}

// Called when the game starts or when spawned
void AA_WSItem::BeginPlay()
{
	Super::BeginPlay();

	// note: 동적으로 설정한 충돌체의 초기화를 해야함 (블루프린트 또는 상속 클래스에서)
}

void AA_WSItem::FitCollisionToMesh() const
{
	const FBoxSphereBounds& Bound = GetMesh()->Bounds;
	
	if (UBoxComponent* BoxComponent = Cast<UBoxComponent>(CollisionComponent))
	{
		BoxComponent->SetBoxExtent(Bound.BoxExtent);
	}
	else if (USphereComponent* SphereComponent = Cast<USphereComponent>(CollisionComponent))
	{
		SphereComponent->SetSphereRadius(Bound.SphereRadius);
	}
	else if (UCapsuleComponent* CapsuleComponent = Cast<UCapsuleComponent>(CollisionComponent))
	{
		CapsuleComponent->SetCapsuleRadius
		(
			Bound.SphereRadius
		);
		CapsuleComponent->SetCapsuleHalfHeight(Bound.BoxExtent.Z);
	}
	else
	{
		ensureAlwaysMsgf(false, TEXT("Unknown collision type"));
	}
}

