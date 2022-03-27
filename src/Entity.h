#pragma once

#include "EntityMemoryPool.h"

class Entity
{
	friend class EntityMemoryPool;
	
	size_t m_id = 0;

	// constructor is private so we can never create
	// entities outside the EntityMemoryPool which has friend access
	Entity(const size_t id);

public:
	void destroy();
	size_t id()	const;
	bool isActive()	const;
	const Tag tag() const;

	template <typename T>
	bool hasComponent() const
	{
		return EntityMemoryPool::Instance().hasComponent<T>(m_id);
	}

	template <typename T, typename... TArgs>
	T& addComponent(TArgs&&... mArgs)
	{
		return EntityMemoryPool::Instance().addComponent<T>(m_id, mArgs...);
	}

	template <typename T>
	T& removeComponent()
	{
		return EntityMemoryPool::Instance().removeComponent<T>(m_id);
	}

	template <typename T>
	T& getComponent()
	{
		return EntityMemoryPool::Instance().getComponent<T>(m_id);
	}

	template <typename T>
	const T& getComponent() const
	{
		return EntityMemoryPool::Instance().getComponent<T>(m_id);
	}
};