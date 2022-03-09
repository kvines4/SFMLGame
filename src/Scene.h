#pragma once

#include "Common.h"
#include "Action.h"
#include "EntityManager.h"

#include <memory>

class GameEngine;

typedef std::map<int, std::string> ActionMap;

class Scene
{

protected:

    GameEngine*     m_game;
    EntityManager   m_entityManager;
    ActionMap       m_actionMap;
    bool            m_paused = false;
    bool            m_hasEnded = false;
    size_t          m_currentFrame = 0;

    virtual void onEnd() = 0;
    void setPaused(bool paused);

public:

    Scene();
    Scene(GameEngine* gameEngine);

    virtual void update() = 0;
    virtual void sDoAction(Action action) = 0;
    virtual void sRender() = 0;

    void simulate(int i);
    void doAction(Action action);
    void registerAction(sf::Keyboard::Key key, const std::string& action);

    size_t width() const;
    size_t height() const;

    ActionMap& getActionMap();
};