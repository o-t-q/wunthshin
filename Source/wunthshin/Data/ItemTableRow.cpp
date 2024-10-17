#include "ItemTableRow.h"

#include "Components/BoxComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/SphereComponent.h"

void FItemTableRow::OnDataTableChanged(const UDataTable* InDataTable, const FName InRowName)
{
	FTableRowBase::OnDataTableChanged(InDataTable, InRowName);

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
