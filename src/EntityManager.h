#pragma once

#include "Entity.h"
#include "EntityMemoryPool.h"

typedef std::vector<Entity>			EntityVec;
typedef std::map<Tag, EntityVec>	EntityMap;

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

	Entity addEntity(const Tag tag);

	const EntityVec& getEntities();
	const EntityVec& getEntities(const Tag tag);
	const size_t getTotal() const;
};