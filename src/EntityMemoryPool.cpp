#include "EntityMemoryPool.h"

#include "Entity.h"

EntityMemoryPool::EntityMemoryPool(size_t maxEntities)
{
	m_numEntities = 0;
	m_active = std::vector<bool>(maxEntities, false);
	m_tags = std::vector<Tag>(maxEntities);
	m_pool = {
			std::vector<CTransform>	 (maxEntities),
			std::vector<CLifespan>	 (maxEntities),
			std::vector<CInput>		 (maxEntities),
			std::vector<CBoundingBox>(maxEntities),
			std::vector<CAnimation>	 (maxEntities),
			std::vector<CGravity>	 (maxEntities),
			std::vector<CState>		 (maxEntities),
			std::vector<CDraggable>	 (maxEntities)
	};
}

const Tag EntityMemoryPool::getTag(size_t entityID) const
{
	return m_tags[entityID];
}

const bool EntityMemoryPool::isActive(size_t entityID) const
{
	return m_active[entityID];
}

size_t EntityMemoryPool::getNextEntityIndex()
{
	// get first index in the active vector where the active flag is false
	auto iterator = std::find_if(
		m_active.begin(),
		m_active.end(),
		[](bool e) { return !e; });

	return iterator != m_active.end() 
		? iterator - m_active.begin() 
		: -1;
}

Entity EntityMemoryPool::addEntity(const Tag tag)
{
	size_t index = getNextEntityIndex();

	m_tags[index] = tag;
	m_active[index] = true;

	// set components to default
	std::get<std::vector<CTransform>>  (m_pool)[index]	= CTransform();
	std::get<std::vector<CLifespan>>   (m_pool)[index]	= CLifespan();
	std::get<std::vector<CInput>>	   (m_pool)[index]	= CInput();
	std::get<std::vector<CBoundingBox>>(m_pool)[index]	= CBoundingBox();
	std::get<std::vector<CAnimation>>  (m_pool)[index]	= CAnimation();
	std::get<std::vector<CGravity>>	   (m_pool)[index]	= CGravity();
	std::get<std::vector<CState>>	   (m_pool)[index]	= CState();
	std::get<std::vector<CDraggable>>  (m_pool)[index]	= CDraggable();

	m_numEntities++;
	return Entity(index);
}

void EntityMemoryPool::destroyEntity(size_t entityID)
{
	m_numEntities--;
	m_active[entityID] = false;
}