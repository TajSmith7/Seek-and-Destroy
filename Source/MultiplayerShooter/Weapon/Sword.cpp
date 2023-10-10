// Fill out your copyright notice in the Description page of Project Settings.


#include "Sword.h"
#include "Components/SkeletalMeshComponent.h"
#include "Components/SphereComponent.h"
#include "Components/WidgetComponent.h"

ASword::ASword()
{
	GetWeaponMesh()->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);

}

void ASword::OnEquipped()
{
	ShowPickupWidget(false);
	GetAreaSphere()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	GetWeaponMesh()->SetSimulatePhysics(false);
	GetWeaponMesh()->SetEnableGravity(false);
	GetWeaponMesh()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	EnableCustomDepth(false);
}

void ASword::OnDropped()
{
	if (HasAuthority())
	{
		GetAreaSphere()->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	}
	GetWeaponMesh()->SetSimulatePhysics(true);
	GetWeaponMesh()->SetEnableGravity(true);
	GetWeaponMesh()->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	GetWeaponMesh()->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Block);
	GetWeaponMesh()->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Ignore);
	GetWeaponMesh()->SetCollisionResponseToChannel(ECollisionChannel::ECC_Camera, ECollisionResponse::ECR_Ignore);

	GetWeaponMesh()->SetCustomDepthStencilValue(CUSTOM_DEPTH_PURPLE);
	GetWeaponMesh()->MarkRenderStateDirty();
	EnableCustomDepth(true);
}