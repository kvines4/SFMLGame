#pragma once

#include "Scene.h"

class Scene_Menu : public Scene
{

protected:

	std::string					m_title;
	std::vector<std::string>	m_menuStrings;
	std::vector<std::string>	m_levelPaths;
	int							m_selectedMenuIndex = 0;
	sf::Text					m_menuText;

	void init();

public:

	Scene_Menu(GameEngine* gameEngine);

	virtual void update() override;
	virtual void sDoAction(Action action) override;
	virtual void sRender() override;
	virtual void onEnd() override;

};