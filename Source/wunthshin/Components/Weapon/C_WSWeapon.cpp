// Fill out your copyright notice in the Description page of Project Settings.


#include "C_WSWeapon.h"

#include "../../Actors/AA_WSCharacter.h"


// Sets default values for this component's properties
UC_WSWeapon::UC_WSWeapon()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = false;

	// ...
}


// Called when the game starts
void UC_WSWeapon::BeginPlay()
{
	Super::BeginPlay();

	// ...
	
}

void UC_WSWeapon::AttackDefault()
{
	// todo: 몽타주 재생
}

void UC_WSWeapon::AttackSecondary()
{
	// todo: 몽타주 재생
}
