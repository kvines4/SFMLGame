#include "EntityManager.h"

#include "Profiler.h"

EntityManager::EntityManager()
{

}

void EntityManager::update()
{
	PROFILE_FUNCTION();
	// add all the entities that are pending
	for (auto e : m_entitiesToAdd)
	{
		// add it to the vector of all entities
		m_entities.push_back(e);

		// add it to the entitiy map in the correct place
		// map[key] will crete an element at 'key' if it does not already exist
		//			therefore we are not in danger of adding to a vector that doesn't exist
		m_entityMap[e.tag()].push_back(e);
	}

	// clear the temporary vector since we have added everything
	m_entitiesToAdd.clear();

	// clean up dead entities in all vectors
	removeDeadEntities(m_entities); 
	m_totalEntities = m_entities.size();
	// remove dead entities from each vector in the entity map 
	for (auto& [tag, entityVec] : m_entityMap)
	{
		removeDeadEntities(entityVec);
	}
}

void EntityManager::removeDeadEntities(EntityVec& vec)
{
	// Iterate from begining to end and remove is active = false;
	vec.erase(
		std::remove_if(
			vec.begin(),
			vec.end(),
			[](Entity e) { return !e.isActive(); }),
		vec.end());
}

Entity EntityManager::addEntity(const Tag tag)
{
	Entity e = EntityMemoryPool::Instance().addEntity(tag);
	m_entitiesToAdd.push_back(e);
	return e;
}

const EntityVec& EntityManager::getEntities()
{
	return m_entities;
}

const EntityVec& EntityManager::getEntities(const Tag tag)
{
	return m_entityMap[tag];
}

const size_t EntityManager::getTotal() const
{
	return m_totalEntities;
}
