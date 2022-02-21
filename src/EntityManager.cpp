#include "EntityManager.h"

EntityManager::EntityManager()
{
}

void EntityManager::update()
{
	// iterate through all entities waiting to be added
	for (auto e : m_entitiesToAdd)
	{
		m_entities.push_back(e);
		m_entityMap[e->tag()].push_back(e);
	}
	m_entitiesToAdd.clear();

	// remove dead entities from the vector of all entities 
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
			[](std::shared_ptr<Entity>& e) { return !e->isActive(); }),
		vec.end());
}

std::shared_ptr<Entity> EntityManager::addEntity(const std::string& tag)
{
	auto entity = std::shared_ptr<Entity>(new Entity(m_totalEntities++, tag));
	m_entitiesToAdd.push_back(entity);
	return entity;
}

const EntityVec& EntityManager::getEntities()
{
	return m_entities;
}

const EntityVec& EntityManager::getEntities(const std::string& tag)
{
	return m_entityMap[tag];
}

const size_t EntityManager::getTotal() const
{
	return m_totalEntities;
}
