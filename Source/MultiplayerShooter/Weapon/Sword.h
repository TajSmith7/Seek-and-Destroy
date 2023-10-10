// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Weapon.h"
#include "Sword.generated.h"

/**
 * 
 */
UCLASS()
class MULTIPLAYERSHOOTER_API ASword : public AWeapon
{
	GENERATED_BODY()
public:
	ASword();

protected:
	virtual void OnEquipped() override;
	virtual void OnDropped() override;
};
