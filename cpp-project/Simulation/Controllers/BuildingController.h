/////////////////////////////////////////////////////////////////////
// File    : BuildingController.h
// Desc    :
// Created :
// Author  :
//
// (c) 2003 Relic Entertainment Inc.
//

#pragma once

#include "ModController.h"

#include "../Extensions/BuildingExt.h"
#include "../Extensions/HealthExt.h"
#include "../Extensions/SightExt.h"
#include "../Extensions/ModifierExt.h"

#include "../ExtInfo/ResourceExtInfo.h"
#include "../ExtInfo/HealthExtInfo.h"
#include "../ExtInfo/SightExtInfo.h"
#include "../ExtInfo/SiteExtInfo.h"
#include "../ExtInfo/CostExtInfo.h"
#include "../ExtInfo/UIExtInfo.h"

#include "../States/StateIdle.h"
#include "../States/StateDead.h"

#include "../CommandProcessor.h"

// forward declarations
class Entity;
class EntityGroup;

class Vec3f;

///////////////////////////////////////////////////////////////////////////////
// BuildingController

class BuildingController : private HealthExt,
													 private SightExt,
													 private ModifierExt,
													 public ModController,
													 private BuildingExt
{
	// types
public:
	class StaticInfo : private HealthExtInfo,
										 private SightExtInfo,
										 private CostExtInfo,
										 private SiteExtInfo,
										 private UIExtInfo,
										 public ModStaticInfo
	{
	public:
		StaticInfo(const ControllerBlueprint *);

	public:
		virtual const ModStaticInfo::ExtInfo *QInfo(unsigned char id) const;
	};

	// construction
public:
	BuildingController(Entity *pEntity, const ECStaticInfo *);
	virtual ~BuildingController();

	// inherited -- ModController
public:
	// commands issued to the entity controllers.
	virtual bool CommandDoProcessNow(const EntityCommand *);
	virtual void OnSpawnEntity();
	virtual void OnDeSpawnEntity();

	virtual void Execute();
	virtual bool Update(const EntityCommand *);

	virtual Extension *QI(unsigned char InterfaceID);

	virtual State *QIActiveState(unsigned char);

	// inherited -- ModController
private:
	virtual Extension *QIAll(unsigned char);

	virtual State *QIStateAll(unsigned char);

	virtual void SetActiveState(unsigned char);

	// Save and Load functions.  You MUST call the base class version of this function first
public:
	virtual void Save(BiFF &biff) const;
	virtual void Load(IFF &iff);

	// Chunk Handlers
private:
	static unsigned long HandleLECD(IFF &, ChunkNode *, void *, void *);

	// inherited -- Extension
private:
	virtual ModController *GetSelf();

	// inherited - HealthExt
private:
	virtual void NotifyHealthGone();

	// dead state
private:
	//
	StateIdle m_stateidle;
	StateDead m_statedead;

	State *m_pCurrentState;

	CommandProcessor m_commandproc;
};
