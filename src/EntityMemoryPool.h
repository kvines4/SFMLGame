#pragma once

#include "Common.h"
#include "Components.h"

class Entity;

typedef std::tuple<
	std::vector<CTransform>,
	std::vector<CLifespan>,
	std::vector<CInput>,
	std::vector<CBoundingBox>,
	std::vector<CAnimation>,
	std::vector<CGravity>,
	std::vector<CState>,
	std::vector<CDraggable>
> EntityComponentVectorTuple;

const long long MAX_ENTITIES = 100000;

class EntityMemoryPool
{
	long long					m_numEntities;
	EntityComponentVectorTuple	m_pool;
	std::vector<std::string>	m_tags;
	std::vector<bool>			m_active;
	EntityMemoryPool(size_t maxEntities);

	// TODO: keep a reference to the last accessed index

public:
	static EntityMemoryPool& Instance()
	{
		static EntityMemoryPool pool(MAX_ENTITIES);
		return pool;
	}

	const std::string& getTag(size_t entityID) const;

	const bool isActive(size_t entityID) const;

	size_t getNextEntityIndex();

	Entity addEntity(const std::string& tag);

	void destroyEntity(size_t entityID);

	template <typename T>
	bool hasComponent(size_t entityID)
	{
		return std::get<std::vector<T>>(m_pool)[entityID].has;
	}

	template <typename T, typename... TArgs>
	T& addComponent(size_t entityID, TArgs&&... mArgs)
	{
		auto& component = std::get<std::vector<T>>(m_pool)[entityID];
		component = T(std::forward<TArgs>(mArgs)...);
		component.has = true;
		return component;
	}

	template <typename T>
	T& removeComponent(size_t entityID)
	{
		auto& component = std::get<std::vector<T>>(m_pool)[entityID];
		component.has = false;
		return component;
	}

	template <typename T>
	T& getComponent(size_t entityID)
	{
		return std::get<std::vector<T>>(m_pool)[entityID];
	}
};