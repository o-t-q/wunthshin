// Fill out your copyright notice in the Description page of Project Settings.


#include "A_WSItem.h"

#include "Components/BoxComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/ShapeComponent.h"
#include "Components/SphereComponent.h"
#include "GameFramework/ProjectileMovementComponent.h"

#include "wunthshin/Components/PickUp/C_WSPickUp.h"
#include "wunthshin/Data/Items/ItemTableRow/ItemTableRow.h"
#include "wunthshin/Data/Items/ItemMetadata/SG_WSItemMetadata.h"
#include "wunthshin/Subsystem/Utility.h"
#include "wunthshin/Widgets/ItemNotify/WG_WSItemNotify.h"
#include "Components/WidgetComponent.h"

class USphereComponent;
const FName AA_WSItem::CollisionComponentName = TEXT("Collision");

void AA_WSItem::InitializeCollisionLazy() const
{
	if (CollisionComponent)
	{
		// 투사체 컴포넌트에 의해 물리처리
		ItemPhysics->SetUpdatedComponent(CollisionComponent);
		// 충돌 설정
		CollisionComponent->SetCollisionProfileName("ItemProfile");
		CollisionComponent->SetGenerateOverlapEvents(true);
		// 부모 컴포넌트가 있는 상태로 simulate하면 바로 부모로부터 detach됨
		CollisionComponent->SetSimulatePhysics(!CollisionComponent->GetAttachParent());
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
	CollisionComponent = CreateOptionalDefaultSubobject<UShapeComponent>(CollisionComponentName);
	
	MeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("StaticMesh"));
	PickUpComponent = CreateDefaultSubobject<UC_WSPickUp>(TEXT("PickUp"));
	ItemPhysics = CreateOptionalDefaultSubobject<UProjectileMovementComponent>(TEXT("ItemPhysics"));

	if (ItemPhysics)
	{
		ItemPhysics->bInterpMovement = true;
		ItemPhysics->bInterpRotation = true;
		ItemPhysics->bSimulationEnabled = false;
	}
	
	// Mesh 충돌 대신 CollisionComponent를 사용
	MeshComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	MeshComponent->SetSimulatePhysics(false);
	MeshComponent->SetGenerateOverlapEvents(false);

	// Item notify widget
	ItemNotifyWidget = CreateDefaultSubobject<UWidgetComponent>(TEXT("ItemNotifyWidget"));
	ItemNotifyWidget->SetWidgetSpace(EWidgetSpace::World);
	ItemNotifyWidget->SetComponentTickEnabled(true);
	ItemNotifyWidget->SetDrawSize(FVector2D{ 500.f, 45.f });
	ItemNotifyWidget->SetAbsolute(false, true, true);
	ItemNotifyWidget->SetCastShadow(false);

	if (static ConstructorHelpers::FClassFinder<UUserWidget> DefaultItemNotifyWidgetClass(TEXT("/Script/UMGEditor.WidgetBlueprint'/Game/ThirdPerson/Blueprints/Widgets/BP_WG_WSItemNotify.BP_WG_WSItemNotify_C'"));
		DefaultItemNotifyWidgetClass.Succeeded())
	{
		ItemNotifyWidget->SetWidgetClass(DefaultItemNotifyWidgetClass.Class);
	}
}

void AA_WSItem::OnConstruction(const FTransform& Transform)
{
	Super::OnConstruction(Transform);
	FetchAsset(this, AssetName);
}

void AA_WSItem::InitializeCollisionComponent(TSubclassOf<UShapeComponent> InClass)
{
	USceneComponent* PreviousAttachParent = GetRootComponent()->GetAttachParent();
	const FTransform PreviousTransform    = GetRootComponent()->GetComponentTransform();
	
	// 새로운 충돌체로의 교환
	if (!CollisionComponent || InClass != CollisionComponent->GetClass())
	{
		if (CollisionComponent)
		{
			CollisionComponent->DestroyComponent();
		}

		CollisionComponent = NewObject<UShapeComponent>(this, InClass, CollisionComponentName, RF_Public);
	}

	// 기존에 충돌체가 있었거나, 새로 생성된 충돌체의 구조 재설정
	if (CollisionComponent)
	{
		SetRootComponent(CollisionComponent);
		if (PreviousAttachParent)
		{
			CollisionComponent->SetupAttachment(PreviousAttachParent);
		}
		CollisionComponent->RegisterComponent();

		MeshComponent->DetachFromComponent(FDetachmentTransformRules::KeepRelativeTransform);
		MeshComponent->AttachToComponent
		(
			CollisionComponent, 
			FAttachmentTransformRules::KeepRelativeTransform
		);
		ItemNotifyWidget->AttachToComponent
		(
			CollisionComponent,
			FAttachmentTransformRules::KeepRelativeTransform
		);

		CollisionComponent->SetWorldTransform(PreviousTransform);
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

		FTransform Offset = Data->MeshOffset;
		// 충돌체의 원점을 물체의 중간으로 옮김
		Offset.SetTranslation({0.f, 0.f, -MeshComponent->Bounds.BoxExtent.Z});
		MeshComponent->SetRelativeTransform(Offset);
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

	ItemMetadata = FItemSubsystemUtility::GetMetadata<UItemSubsystem, UItemEditorSubsystem, USG_WSItemMetadata>(GetWorld(), Data->ItemName);
	
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

	ItemNotifyWidget->InitWidget();

	if (UWG_WSItemNotify* ItemWidget = Cast<UWG_WSItemNotify>(ItemNotifyWidget->GetWidget()))
	{
		ItemWidget->SetParentItem(this);
	}

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

