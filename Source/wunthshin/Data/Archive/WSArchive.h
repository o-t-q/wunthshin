#pragma once

struct FWSSerializeStruct;

struct FWSArchive : public FArchiveProxy
{
	explicit FWSArchive(FArchive& InnerAr) : FArchiveProxy(InnerAr)
	{
		ArIsSaveGame = true;
	};

	void SerializeObject(FWSSerializeStruct& InObject);
	FWSArchive& operator<<(FWSSerializeStruct& InObject);

	virtual FArchive& operator<<(FName& Value) override
	{
		return FArchive::operator<<(Value);
	}

	virtual FArchive& operator<<(FText& Value) override
	{
		return FArchive::operator<<(Value);
	}

	virtual FArchive& operator<<(UObject*& Value) override
	{
		return FArchive::operator<<(Value);
	}

	virtual FArchive& operator<<(FField*& Value) override
	{
		return FArchive::operator<<(Value);
	}

	virtual FArchive& operator<<(FLazyObjectPtr& Value) override
	{
		return FArchive::operator<<(Value);
	}

	virtual FArchive& operator<<(FObjectPtr& Value) override
	{
		return FArchive::operator<<(Value);
	}

	virtual FArchive& operator<<(FSoftObjectPtr& Value) override
	{
		return FArchive::operator<<(Value);
	}

	virtual FArchive& operator<<(FSoftObjectPath& Value) override
	{
		return FArchive::operator<<(Value);
	}

	virtual FArchive& operator<<(FWeakObjectPtr& Value) override
	{
		return FArchive::operator<<(Value);
	}
};
