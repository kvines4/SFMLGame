#pragma once

#include "Common.h"
#include "Entity.h"
#include "EntityMemoryPool.h"

typedef std::vector<Entity>					EntityVec;
typedef std::map<std::string, EntityVec>	EntityMap;

class EntityManager
{
	EntityVec	m_entities;
	EntityVec	m_entitiesToAdd;
	EntityMap	m_entityMap;
	size_t		m_totalEntities = 0;

	void removeDeadEntities(EntityVec& vec);

public:
	EntityManager();

	void update();

	Entity addEntity(const std::string& tag);

	const EntityVec& getEntities();
	const EntityVec& getEntities(const std::string& tag);
	const size_t getTotal() const;
};