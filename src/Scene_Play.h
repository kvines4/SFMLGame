#pragma once

#include "Common.h"
#include "Scene.h"
#include <map>
#include <memory>

#include "EntityManager.h"

class Scene_Play : public Scene
{
    struct PlayerConfig
    {
        float X, Y, CX, CY, SPEED, MAXSPEED, JUMP, GRAVITY;
        std::string WEAPON;
    };

protected:

    std::shared_ptr<Entity> m_player;
    std::string             m_levelPath;
    PlayerConfig            m_playerConfig;
    bool                    m_drawTextures = true;
    bool                    m_drawCollisions = false;
    bool                    m_drawGrid = false;
    const Vec2              m_gridSize = { 64, 64 };
    sf::Text                m_gridText;

    sf::CircleShape         m_mouseShape;
    std::shared_ptr<Entity> m_draggable;

    void init(const std::string& levelPath);

    void loadLevel(const std::string& filename);

public:

    Scene_Play(GameEngine* gameEngine, const std::string& levelPath);

    Vec2 gridToMidPixel(float gridX, float gridY, std::shared_ptr<Entity> entity);

    void spawnPlayer();
    void spawnBullet(std::shared_ptr<Entity> Entity);

    void sMovement();
    void sDraggable();
    void sLifespan();
    void sCollision();
    void sAnimation();

    void hitBlock(std::shared_ptr<Entity> Entity);

    void drawLine(const Vec2& p1, const Vec2& p2);

    virtual void update() override;
    virtual void sDoAction(Action action) override;
    virtual void sRender() override;
    virtual void onEnd() override;
};