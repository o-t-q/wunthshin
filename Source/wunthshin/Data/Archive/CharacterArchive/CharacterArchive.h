#pragma once
#include "wunthshin/Data/Archive/WSArchive.h"

class ICommonPawn;

struct FCharacterArchive : public FWSArchive
{
	explicit FCharacterArchive(FArchive& InnerAr)
		: FWSArchive(InnerAr) {}

	FName GetAssetName() const { return AssetName; }
	void SerializeCharacter(ICommonPawn* InCommonPawn);
	FCharacterArchive& operator<<(ICommonPawn* InCommonPawn);

	virtual FArchive& operator<<(FName& Value) override
	{
		return FWSArchive::operator<<(Value);
	}

	virtual FArchive& operator<<(FText& Value) override
	{
		return FWSArchive::operator<<(Value);
	}

	virtual FArchive& operator<<(UObject*& Value) override
	{
		return FWSArchive::operator<<(Value);
	}

	virtual FArchive& operator<<(FField*& Value) override
	{
		return FWSArchive::operator<<(Value);
	}

	virtual FArchive& operator<<(FLazyObjectPtr& Value) override
	{
		return FWSArchive::operator<<(Value);
	}

	virtual FArchive& operator<<(FObjectPtr& Value) override
	{
		return FWSArchive::operator<<(Value);
	}

	virtual FArchive& operator<<(FSoftObjectPtr& Value) override
	{
		return FWSArchive::operator<<(Value);
	}

	virtual FArchive& operator<<(FSoftObjectPath& Value) override
	{
		return FWSArchive::operator<<(Value);
	}

	virtual FArchive& operator<<(FWeakObjectPtr& Value) override
	{
		return FWSArchive::operator<<(Value);
	}

private:
	FName AssetName;
};
