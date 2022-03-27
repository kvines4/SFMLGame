#pragma once

#include "Scene.h"

class Scene_Play : public Scene
{
    struct PlayerConfig
    {
        float X, Y, CX, CY, SPEED, MAXSPEED, JUMP, GRAVITY;
        std::string WEAPON;
    };

protected:

    bool            m_drawTextures   = true;
    bool            m_drawCollisions = false;
    bool            m_drawGrid       = false;
    const Vec2      m_gridSize       = { 64, 64 };
    std::string     m_levelPath;
    PlayerConfig    m_playerConfig;
    sf::Text        m_gridText;
    sf::CircleShape m_mouseShape;

    void init(const std::string& levelPath);

    void loadLevel(const std::string& filename);

public:

    Scene_Play(GameEngine* gameEngine, const std::string& levelPath);

    Vec2 gridToMidPixel(float gridX, float gridY, Entity entity);

    void spawnPlayer();
    void spawnBullet(Entity Entity);

    void sMovement();
    void sDraggable();
    void sLifespan();
    void sCollision();
    void sAnimation();

    void hitBlock(Entity Entity);

    void drawLine(const Vec2& p1, const Vec2& p2);

    virtual void update() override;
    virtual void sDoAction(Action action) override;
    virtual void sRender() override;
    virtual void onEnd() override;
};