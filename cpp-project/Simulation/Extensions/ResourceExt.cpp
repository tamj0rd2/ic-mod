/////////////////////////////////////////////////////////////////////
// File    : HealthExtension.cpp
// Desc    :
// Created : Tuesday, February 13, 2001
// Author  :
//
// (c) 2001 Relic Entertainment Inc.
//

#include "pch.h"
#include "ResourceExt.h"

#include "../RDNTuning.h"
#include "../GameEventDefs.h"

#include "../Controllers/ModController.h"

#include "../ExtInfo/ResourceExtInfo.h"

#include <SimEngine/Entity.h>
#include <SimEngine/EntityAnimator.h>
#include <SimEngine/Pathfinding/ImpassMap.h>

#include <Util/Iff.h>
#include <Util/Biff.H>

/////////////////////////////////////////////////////////////////////
// Desc.     :
// Result    :
// Param.    :
// Author    :
//
ResourceExt::ResourceExt(const ResourceExtInfo *pResourceExtInfo)
		: m_numResources(0),
			m_maxGatherersOnSite(0)
{
	dbAssert(pResourceExtInfo);

	dbAssert(pResourceExtInfo->resourceMax > 0);
	m_numResources = pResourceExtInfo->resourceMax;
	m_maxResources = pResourceExtInfo->resourceMax;

	m_maxGatherersOnSite = pResourceExtInfo->maxGatherers;
}

void ResourceExt::InitLooks()
{
	OnResourceProgress(m_numResources / m_maxResources);
}

bool ResourceExt::IsDepleted() const
{
	return m_numResources <= 0.0f;
}

float ResourceExt::DecResources(float amount)
{
	// validate parm
	dbAssert(amount >= 0);

	// If i have none, you get none bwahahaha
	if (m_numResources == 0)
	{
		return 0;
	}
	//
	if (amount > m_numResources)
		amount = m_numResources;

	//
	m_numResources -= amount;

	//
	const float progress = m_numResources / m_maxResources;

	OnResourceProgress(progress);

	// check for zero
	if (m_numResources == 0)
	{
		// generate event
		GameEventSys::Instance()->PublishEvent(GameEvent_ResourceGatherDepleted(GetSelf()->GetEntity(), m_gatherers));

		// delegate
		OnZeroResources();
	}

	return amount;
}

float ResourceExt::GetResources() const
{
	return m_numResources;
}

void ResourceExt::OnResourceProgress(float progress)
{
	GetSelf()->GetEntity()->GetAnimator()->SetMotionVariable("Resource", progress);
}

/////////////////////////////////////////////////////////////////////
// Desc.     :
// Result    :
// Param.    :
// Author    :
//
void ResourceExt::SaveExt(BiFF &biff) const
{
	IFF &iff = *biff.GetIFF();

	biff.StartChunk(Type_NormalVers, 'ERCE', "Extension: Resource", 1);

	IFFWrite(iff, m_numResources);
	IFFWrite(iff, m_maxResources);

	biff.StopChunk();
}

/////////////////////////////////////////////////////////////////////
// Desc.     :
// Result    :
// Param.    :
// Author    :
//
void ResourceExt::LoadExt(IFF &iff)
{
	iff.AddParseHandler(HandleERCE, Type_NormalVers, 'ERCE', this, NULL);
}

/////////////////////////////////////////////////////////////////////
// Desc.     :
// Result    :
// Param.    :
// Author    :
//
unsigned long ResourceExt::HandleERCE(IFF &iff, ChunkNode *, void *pContext1, void *)
{
	ResourceExt *pResourceExt = static_cast<ResourceExt *>(pContext1);
	dbAssert(pResourceExt);

	IFFRead(iff, pResourceExt->m_numResources);
	IFFRead(iff, pResourceExt->m_maxResources);

	const float progress = pResourceExt->m_numResources / pResourceExt->m_maxResources;

	pResourceExt->OnResourceProgress(progress);

	return 0;
}

/////////////////////////////////////////////////////////////////////
// Desc.     :
// Result    :
// Param.    :
// Author    :
void ResourceExt::BurnInCantBuild(TerrainCellMap *pTCMap)
{
	// TODO: implement this
	dbTracef("ResourceExt::BurnInCantBuild not implemented and REALLY needs implementing");
	/***
	// Burn in an extra perimmeter around myself so that no structures can be built close to me
	long cellx, cellz;

	cellx = pTCMap->WorldXToCellX( GetSelf()->GetEntity()->GetPosition().x );
	cellz = pTCMap->WorldZToCellZ( GetSelf()->GetEntity()->GetPosition().z );

	long width, height;
	width = height = pTCMap->WorldToCellDim( RDNTuning::Instance()->GetBuildingInfo().resourceNoBuildSize );

	cellx -= width/2;  cellz -= height/2;

	pTCMap->BlitCells( cellx, cellz, width, height, eCantBuild, TerrainCellMap::OP_OR );
***/
}

/////////////////////////////////////////////////////////////////////
// Desc.     :
// Result    :
// Param.    :
// Author    :
//
void ResourceExt::UnBurnCantBuild(TerrainCellMap *pTCMap)
{
	// TODO: implement this
	dbTracef("ResourceExt::UnBurnCantBuild not implemented and REALLY needs implementing");
	/***
	// Un-Burn in the extra perimmeter around myself so that no structures can be built close to me
	long cellx, cellz;

	cellx = pTCMap->WorldXToCellX( GetSelf()->GetEntity()->GetPosition().x );
	cellz = pTCMap->WorldZToCellZ( GetSelf()->GetEntity()->GetPosition().z );

	long width, height;
	width = height = pTCMap->WorldToCellDim( RDNTuning::Instance()->GetBuildingInfo().resourceNoBuildSize );

	cellx -= width/2;  cellz -= height/2;

	unsigned char mask = (unsigned char)(~eCantBuild);
	pTCMap->BlitCells( cellx, cellz, width, height, mask, TerrainCellMap::OP_AND );
***/
}

const EntityGroup &ResourceExt::Gatherers() const
{
	return m_gatherers;
}

size_t ResourceExt::GetGathererCount() const
{
	return m_gatherers.size();
}

bool ResourceExt::HasNoOtherGatherers(const Entity *pEntity) const
{
	size_t count = GetGathererCount();
	if (m_gatherers.find(pEntity) != m_gatherers.end())
		count = count - 1;

	return count == 0;
}

void ResourceExt::GathererAdd(const Entity *p_Entity)
{
	m_gatherers.push_back(const_cast<Entity *>(p_Entity));
}

void ResourceExt::GathererRmv(const Entity *p_Entity)
{
	m_gatherers.remove(p_Entity);
	m_gatherersOnSite.remove(p_Entity);
}

void ResourceExt::GatherersOnSiteAdd(const Entity *p_Entity)
{
	if (GatherersOnSiteIsAtMax())
		dbFatalf("ResourceExt::GatherersOnSiteAdd Resource is already at max capacity");

	m_gatherersOnSite.push_back(const_cast<Entity *>(p_Entity));
}

void ResourceExt::GatherersOnSiteRmv(const Entity *p_Entity)
{
	m_gatherersOnSite.remove(p_Entity);
}

bool ResourceExt::GatherersOnSiteIsAtMax() const
{
	return m_gatherersOnSite.size() >= m_maxGatherersOnSite;
}

bool ResourceExt::HasSpaceForGathererOnSite(const Entity *pEntity) const
{
	size_t count = m_gatherersOnSite.size();
	if (m_gatherersOnSite.find(pEntity) != m_gatherersOnSite.end())
		count = count - 1;

	return count < m_maxGatherersOnSite;
}

bool ResourceExt::CanGatherResourcesOnSiteNow(const Entity *pEntity) const
{
	return !IsDepleted() && HasSpaceForGathererOnSite(pEntity);
}
