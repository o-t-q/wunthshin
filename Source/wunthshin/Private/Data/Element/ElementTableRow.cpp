#include "Data/Element/ElementTableRow.h"
#include "Data/Element/O_WSElementReactor.h"

#ifdef WITH_EDITOR
void FElementTableRow::OnDataTableChanged(const UDataTable* InDataTable, const FName InRowName)
{
	FTableRowBase::OnDataTableChanged(InDataTable, InRowName);

	// 프로젝트 로드 도중에 호출되는 것을 방지
	if (GIsRunning)
	{
		TArray<FDataTableRowHandle> OtherElementHandles;
		TArray<FElementReaction> RemoveCandidates;

		// 중복제거
		{
			for (FElementReaction& Reaction : Reactions)
			{
				if (OtherElementHandles.Contains(Reaction.OtherElement))
				{
					RemoveCandidates.Add(Reaction);
				}
				else
				{
					OtherElementHandles.Add(Reaction.OtherElement);
				}
			}

			for (const FElementReaction& Removal : RemoveCandidates)
			{
				Reactions.Remove(Removal);
			}
		}
	
		// bi-directional 추가 (반대측의 원소에도 현재 원소의 이름으로 추가)
		for (const FElementReaction& Reaction : Reactions) 
		{
			// 반대 측의 원소에 접근해서
			FElementTableRow* OtherPair = Reaction.OtherElement.GetRow<FElementTableRow>(TEXT(""));

			if (!OtherPair)
			{
				continue;
			}
			
			// 반대 측 반응에 지금 원소가 있는지 확인하고
			FElementReaction* OtherPairRegisteredReaction = OtherPair->Reactions.FindByPredicate([this](const FElementReaction& OtherReaction)
				{
					return OtherReaction.OtherElement.GetRow<FElementTableRow>(TEXT("")) == this;

				});

			FDataTableRowHandle ThisRowHandle;
			ThisRowHandle.DataTable = InDataTable;
			ThisRowHandle.RowName = InRowName;

			if (OtherPairRegisteredReaction == nullptr)
			{
				// 없다면 새로 추가
				FElementReaction OtherPerspectiveReaction
				{
					.OtherElement = ThisRowHandle,
					.Reactor = Reaction.Reactor
				};

				OtherPair->Reactions.Add(OtherPerspectiveReaction);
			}
			else 
			{
				// 있다면 같은지 확인
				if (OtherPairRegisteredReaction->OtherElement != ThisRowHandle) 
				{
					OtherPairRegisteredReaction->OtherElement = ThisRowHandle;
				}
				if (OtherPairRegisteredReaction->Reactor != Reaction.Reactor) 
				{
					OtherPairRegisteredReaction->Reactor = Reaction.Reactor;
				}
			}
		}
	}
}
#endif

bool FElementReaction::operator==(const FElementReaction& Other) const
{
	return OtherElement == Other.OtherElement && Reactor == Other.Reactor;
}
