#pragma once

struct FEventTicket
{
public:
	virtual ~FEventTicket() = default;
	virtual void Execute(UWorld* InWorld) = 0;
	bool IsValid() const { return !bDisposed; }

	FTimerHandle& GetTimerHandle() { return TimerHandle; }
	float GetRate() const { return Rate; }
	void SetRate(const float InRate) { Rate = InRate; }
	
protected:
	void SetDisposed() { bDisposed = true; }
	
private:
	FTimerHandle TimerHandle;
	
	float Rate = -1.f;
	bool bDisposed = false;
};
