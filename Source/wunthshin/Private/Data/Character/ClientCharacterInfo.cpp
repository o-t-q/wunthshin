// Fill out your copyright notice in the Description page of Project Settings.


#include "Data/Character/ClientCharacterInfo.h"
#include "Actor/Pawn/AA_WSCharacter.h"
#include "Component/StatsComponent.h"
#include "Data/Character/SG_WSCharacterSnapshot.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/GameplayStatics.h"

#include "Net/UnrealNetwork.h"

void AClientCharacterInfo::GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(AClientCharacterInfo, CurrentSpawnedIndex);
	DOREPLIFETIME(AClientCharacterInfo, OwnedCharacters);
}

void AClientCharacterInfo::ResetPlayer()
{
	if ( HasAuthority() )
	{
		for (const AA_WSCharacter* Character : OwnedCharacters)
		{
			Character->GetStatsComponent()->SetHP(100);
		}
	}
}

void AClientCharacterInfo::TakeCharacterLevelSnapshot()
{
	if ( HasAuthority() )
	{
		CharacterSnapshots.SetNumZeroed(OwnedCharacters.Num());
	
		for (auto It = OwnedCharacters.CreateIterator(); It; ++It)
		{
			USG_WSCharacterSnapshot* Snapshot = Cast<USG_WSCharacterSnapshot>(UGameplayStatics::CreateSaveGameObject(USG_WSCharacterSnapshot::StaticClass()));
			Snapshot->SetCharacter(FCharacterContext(*It));
			UGameplayStatics::SaveGameToMemory(Snapshot,CharacterSnapshots[It.GetIndex()]); 
		}
	}
}

void AClientCharacterInfo::LoadCharacterLevelSnapshot()
{
	if ( HasAuthority() )
	{
		for (auto It = CharacterSnapshots.CreateIterator(); It; ++It)
		{
			const int32 Index = It.GetIndex();
			
			if (const USG_WSCharacterSnapshot* Snapshot = Cast<USG_WSCharacterSnapshot>(UGameplayStatics::LoadGameFromMemory(CharacterSnapshots[Index])))
			{
				if ( OwnedCharacters[Index] == nullptr )
				{
					OwnedCharacters[Index] = Snapshot->SpawnCharacter(GetWorld(), FTransform::Identity, nullptr);
					OwnedCharacters[Index]->SetActorEnableCollision(false);
					OwnedCharacters[Index]->SetActorHiddenInGame(true);
				}
			}
		}
	}
}

void AClientCharacterInfo::Client_OnRep_OnCharacterAdded_Implementation()
{
	OnCharacterAdded.Broadcast();
}

int32 AClientCharacterInfo::GetAvailableCharacter() const
{
	for (auto It = OwnedCharacters.CreateConstIterator(); It; ++It)
	{
		if ((*It)->GetStatsComponent()->GetHP() > 0)
		{
			return It.GetIndex();
		}
	}

	return INDEX_NONE;
}

int32 AClientCharacterInfo::GetIndexOfCharacter( const AA_WSCharacter* InCharacter ) const
{
	return OwnedCharacters.IndexOfByPredicate([&InCharacter](const AA_WSCharacter* InItem)
	{
		return InItem == InCharacter;
	});
}

void AClientCharacterInfo::AddCharacter( AA_WSCharacter* InCharacter )
{
	if ( HasAuthority() )
	{
		uint32 Index = 0;

		// 최대 5개에
		while (Index != 5)
		{
			// 같은 캐릭터가 없고
			if (OwnedCharacters.IsValidIndex(Index) &&
				OwnedCharacters[Index] == InCharacter)
			{
				break;
			}

			// 이미 점유된 공간이 아니면
			if (OwnedCharacters.IsValidIndex(Index) &&
				OwnedCharacters[Index] != nullptr)
			{
				++Index;
				continue;
			}

			// 새로운 공간을 할당 후 추가
			AddCharacter(InCharacter, Index);
		}
	}
}

void AClientCharacterInfo::AddCharacter( AA_WSCharacter* Character, const int32 InIndex )
{
	if ( HasAuthority() )
	{
		if (OwnedCharacters.IsValidIndex(InIndex))
		{
			OwnedCharacters[InIndex] = Character;
			OnCharacterAdded.Broadcast();
			Client_OnRep_OnCharacterAdded();
		}
		else
		{
			OwnedCharacters.SetNum(InIndex + 1, EAllowShrinking::No);
			CharacterSnapshots.SetNum(InIndex + 1, EAllowShrinking::No);
			OwnedCharacters[InIndex] = Character;

			OnCharacterAdded.Broadcast();
			Client_OnRep_OnCharacterAdded();
		}
	}
}

const TArray<AA_WSCharacter*>& AClientCharacterInfo::GetOwnedCharacters() const
{
	return OwnedCharacters;
}

AA_WSCharacter* AClientCharacterInfo::GetCharacter( const int32 InIndex ) const
{
	if (OwnedCharacters.IsValidIndex(InIndex))
	{
		return OwnedCharacters[InIndex];
	}

	return nullptr;
}

bool AClientCharacterInfo::IsOwnedCharacter( const AA_WSCharacter* InCharacter ) const
{
	if (InCharacter == nullptr)
	{
		return false;
	}
		
	return OwnedCharacters.Contains(InCharacter);
}

AA_WSCharacter* AClientCharacterInfo::GetCurrentCharacter() const
{
	if (OwnedCharacters.IsValidIndex(CurrentSpawnedIndex))
	{
		return OwnedCharacters[CurrentSpawnedIndex];
	}

	return nullptr;
}

void AClientCharacterInfo::SpawnAsCharacter( const int32 InIndex )
{
	if (HasAuthority())
	{
		// 이미 스폰한 캐릭터가 아니면서
		if (CurrentSpawnedIndex == InIndex)
		{
			return;
		}

		// 배열 범위 내에 있어야 하고
		if (!OwnedCharacters.IsValidIndex(InIndex))
		{
			return;
		}

		// 체력이 남아있다면 전환
		if (OwnedCharacters[InIndex]->GetStatsComponent()->GetHP() <= 0)
		{
			return;
		}
	
		if (APlayerController* PlayerController = GetWorld()->GetFirstPlayerController())
		{
			AA_WSCharacter*  CurrentCharacter  = Cast<AA_WSCharacter>(PlayerController->GetPawn());
			const FTransform PreviousTransform = CurrentCharacter->GetActorTransform();
			
			if (OwnedCharacters[CurrentSpawnedIndex] == nullptr)
			{
				OwnedCharacters[CurrentSpawnedIndex] = CurrentCharacter;
			}

			CurrentCharacter->GetCharacterMovement()->StopMovementImmediately();
			CurrentCharacter->SetActorEnableCollision(false);
			CurrentCharacter->SetActorHiddenInGame(true);
			CurrentCharacter->SetActorTransform(FTransform::Identity);
		
			CurrentSpawnedIndex = InIndex;
			PlayerController->Possess(OwnedCharacters[InIndex]);
			OwnedCharacters[InIndex]->SetActorEnableCollision(true);
			OwnedCharacters[InIndex]->SetActorHiddenInGame(false);
			OwnedCharacters[InIndex]->SetActorTransform(PreviousTransform);
		}
	}
}
