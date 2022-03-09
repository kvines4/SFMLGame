#include "Scene.h"
#include "GameEngine.h"

Scene::Scene()
{

}

Scene::Scene(GameEngine* gameEngine)
	: m_game(gameEngine)
{

}

void Scene::setPaused(bool paused)
{
	m_paused = paused;
}

std::map<int, std::string>& Scene::getActionMap()
{
	return m_actionMap;
}

size_t Scene::width() const
{
	return m_game->window().getSize().x;
}

size_t Scene::height() const
{
	return m_game->window().getSize().y;
}

void Scene::simulate(int i)
{
	update();
	sRender();
	m_game->window().display();
}

void Scene::doAction(Action action)
{
	sDoAction(action);
}

void Scene::registerAction(sf::Keyboard::Key key, const std::string& action)
{
	m_actionMap[key] = action;
}