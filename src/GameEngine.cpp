#include "GameEngine.h"

#include <iostream>

#include "Profiler.h"
#include "Scene.h"
#include "Scene_Menu.h"
#include "Scene_Play.h"

GameEngine::GameEngine(const std::string& path)
{
	init(path);
}

void GameEngine::init(const std::string& path)
{
	PROFILE_FUNCTION();
	m_assets.loadFromFile(path);

	{
		PROFILE_SCOPE("SFML Create Window");
		m_window.create(sf::VideoMode(1280, 768), "Definitely Not Mario");
		m_window.setFramerateLimit(60);
	}

	changeScene("MENU", std::make_shared<Scene_Menu>(this));
}

std::shared_ptr<Scene> GameEngine::currentScene()
{
	return m_sceneMap[m_currentScene];
}

bool GameEngine::isRunning()
{
	return m_running && m_window.isOpen();
}

sf::RenderWindow& GameEngine::window()
{
	return m_window;
}

void GameEngine::run()
{
	while (isRunning())
	{
		update();
	}
}

void GameEngine::sUserInput()
{
	PROFILE_FUNCTION();
	sf::Event event;
	while (m_window.pollEvent(event))
	{
		PROFILE_SCOPE("Poll Event Loop");

		if (event.type == sf::Event::Closed)
		{
			PROFILE_SCOPE("Close Event");
			quit();
		}

		if (event.type == sf::Event::KeyPressed)
		{
			PROFILE_SCOPE("Screenshot Event");

			if (event.key.code == sf::Keyboard::X)
			{
				sf::Texture texture;
				texture.create(m_window.getSize().x, m_window.getSize().y);
				texture.update(m_window);
				if (texture.copyToImage().saveToFile("test.png"))
				{
					std::cout << "screenshot saved to " << "test.png" << std::endl;
				}
			}
		}

		if (event.type == sf::Event::KeyPressed || event.type == sf::Event::KeyReleased)
		{
			PROFILE_SCOPE("Key Event");

			// if the current scene does not have an action associated with this key, skip the event
			if (currentScene()->getActionMap().find(event.key.code) == currentScene()->getActionMap().end()) { continue; }

			// determine start or end action by whether it was key press or release
			const std::string actionType = (event.type == sf::Event::KeyPressed) ? "START" : "END";

			// look up the action and send the action to the scene
			currentScene()->doAction(Action(currentScene()->getActionMap().at(event.key.code), actionType));
		}

		// mouse actions
		if (event.type == sf::Event::MouseButtonPressed)
		{
			PROFILE_SCOPE("Mouse Pressed Event");

			auto mpos = sf::Mouse::getPosition(m_window);
			Vec2 pos(mpos.x, mpos.y);
			switch (event.mouseButton.button)
			{
				case sf::Mouse::Left:   { currentScene()->doAction(Action("LEFT_CLICK",   "START", pos)); break; }
				case sf::Mouse::Middle: { currentScene()->doAction(Action("MIDDLE_CLICK", "START", pos)); break; }
				case sf::Mouse::Right:  { currentScene()->doAction(Action("RIGHT_CLICK",  "START", pos)); break; }
				default: break;
			}
		}

		if (event.type == sf::Event::MouseButtonReleased)
		{
			PROFILE_SCOPE("Mouse Released Event");

			auto mpos = sf::Mouse::getPosition(m_window);
			Vec2 pos(mpos.x, mpos.y);
			switch (event.mouseButton.button)
			{
				case sf::Mouse::Left:   { currentScene()->doAction(Action("LEFT_CLICK",   "END", pos)); break; }
				case sf::Mouse::Middle: { currentScene()->doAction(Action("MIDDLE_CLICK", "END", pos)); break; }
				case sf::Mouse::Right:  { currentScene()->doAction(Action("RIGHT_CLICK",  "END", pos)); break; }
				default: break;
			}
		}

		if (event.type == sf::Event::MouseMoved)
		{
			PROFILE_SCOPE("Mouse Move Event");

			currentScene()->doAction(Action("MOUSE_MOVE", Vec2(event.mouseMove.x, event.mouseMove.y)));
		}
	}
}

void GameEngine::changeScene(const std::string& sceneName, std::shared_ptr<Scene> scene, bool endCurrentScene)
{
	if (scene)
	{
		m_sceneMap[sceneName] = scene;
	}
	else
	{
		if (m_sceneMap.find(sceneName) == m_sceneMap.end())
		{
			std::cerr << "Warning: Scene does not exist: " << sceneName << std::endl;
			return;
		}
	}
	
	if (endCurrentScene)
	{
		m_sceneMap.erase(m_sceneMap.find(m_currentScene));
	}

	m_currentScene = sceneName;
}

void GameEngine::update()
{
	PROFILE_FUNCTION();
	if (!isRunning())       { return; }
	if (m_sceneMap.empty()) { return; }

	sUserInput();
	currentScene()->simulate(m_simulationSpeed);
	currentScene()->sRender();

	{
		PROFILE_SCOPE("SFML Display");
		m_window.display();
	}
}

void GameEngine::quit()
{
	m_running = false;
}

const Assets& GameEngine::assets() const
{
	return m_assets;
}