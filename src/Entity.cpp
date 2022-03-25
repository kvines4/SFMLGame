#include "Entity.h"
#include "EntityMemoryPool.h"

Entity::Entity(const size_t id)
	: m_id(id)
{

}

bool Entity::isActive() const
{
	return EntityMemoryPool::Instance().isActive(m_id);
}

const std::string& Entity::tag() const
{
	return EntityMemoryPool::Instance().getTag(m_id);
}

size_t Entity::id() const
{
	return m_id;
}

void Entity::destroy()
{
	EntityMemoryPool::Instance().destroyEntity(m_id);
}