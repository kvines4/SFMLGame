#include "Entity.h"

Entity::Entity(const size_t i, const std::string& t)
	: m_id(i)
	, m_tag(t)
{

}

bool Entity::isActive()
{
	return m_active;
}

const std::string& Entity::tag() const
{
	return m_tag;
}

const size_t Entity::id() const
{
	return m_id;
}

void Entity::destroy()
{
	m_active = false;
}

////// Examples //////

// Declaring an Entity example
//int main()
//{
//	std::vector<Entity> entities;
//	Vec2 p(100, 200);
//	Vec2 v(10, 10);
//	Entity e;
//	e.cTransform = std::make_shared<CTransform>(p, v);
//	e.cName		 = std::make_shared<CName>     ("Red Box");
//	e.cShape	 = std::make_shared<CShape>    (args);
//	entities.push_back(e);
//	doStuff(entities);
//}

// Using Entities
//void doStuff(std::vector<Entity>& entities)
//{
//	for (Entity& e : entities)
//	{
//		// Move entity based on its current speed
//		e.cTransform->pos += e.cTransform->velocity;
//		e.cShape->shape.setPosition(e.cTransform->pos);
//		window.draw(e.cShape->shape);
//	}
//}

// Checking for component
//int main()
//{
//	Entity e;
//	e.cName = std::make_shared<CName>("Red Box");
//	// component stored as shared_ptr, false if not present
//	if (e.cTransform) {}				// will be false
//	if (e.cName) {}						// will be true
//	if (e.cTransform && e.cShape) {}	// needed to render
//}
