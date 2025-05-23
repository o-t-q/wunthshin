#include "Data/Item/ItemTableRow.h"

#include "Components/BoxComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/SphereComponent.h"

#include "Data/Item/SG_WSItemMetadata.h"

void FItemTableRow::OnDataTableChanged(const UDataTable* InDataTable, const FName InRowName)
{
	FTableRowBase::OnDataTableChanged(InDataTable, InRowName);

#ifdef WITH_EDITOR
	TArray<FItemTableRow*> Rows;
	InDataTable->GetAllRows("", Rows);
	for (int i = 0; i < Rows.Num(); ++i)
	{
		if (Rows[i] == this) 
		{
			ID = i;
			break;
		}
	}

	if (GIsRunning)
	{
		if (bCollision)
		{
			if (CollisionShape == UBoxComponent::StaticClass())
			{
				bBox = true;
				bSphere = false;
				bCapsule = false;
			}
			else if (CollisionShape == USphereComponent::StaticClass())
			{
				bBox = false;
				bSphere = true;
				bCapsule = false;
			}
			else if (CollisionShape == UCapsuleComponent::StaticClass())
			{
				bBox = false;
				bSphere = false;
				bCapsule = true;
			}
			else
			{
				ensureAlwaysMsgf(false, TEXT("Unknown collision shape type"));
			}
		}
	}
#endif
}

void FWeaponTableRow::OnDataTableChanged(const UDataTable* InDataTable, const FName InRowName)
{
	// 무기 타입을 강제
	ItemType = EItemType::Weapon;

	Super::OnDataTableChanged(InDataTable, InRowName);
}
