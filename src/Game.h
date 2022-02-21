#pragma once

#include "Common.h"
#include "Entity.h"
#include "EntityManager.h"

struct PlayerConfig { int SR, CR, FR, FG, FB, OR, OG, OB, OT, V; float S; }; 
struct EnemyConfig  { int SR, CR, OR, OG, OB, OT, VMIN, VMAX, L, SI; float SMIN, SMAX; }; 
struct BulletConfig { int SR, CR, FR, FG, FB, OR, OG, OB, OT, V, L; float S; };

class Game
{
	sf::RenderWindow	m_window;			// the window we will draw to
	float				m_fps;
	EntityManager		m_entities;			// vector of entities to maintain
	sf::Font			m_font;				// the font we will use to draw
	sf::Text			m_text;				// the score text to be drawn to the screen 
	PlayerConfig		m_playerConfig;
	EnemyConfig			m_enemyConfig;
	BulletConfig		m_bulletConfig;
	int					m_score = 0;
	int					m_currentFrame = 0;
	int					m_lastEnemySpawnTime = 0;
	bool				m_paused = false;	// whether we update game logic
	bool				m_running = true;	// whether the game is running
	bool				m_debug = false;

	std::shared_ptr<Entity> m_player;

	void init(const std::string& config);	// initialise the GameState with a config file path
	void togglePause();						// pause the game
	void toggleDebug();
	float toRadians(int a);

	void sMovement();
	void sUserInput();
	void sLifespan();
	void sRender();
	void sEnemySpawner();
	void sCollision();

	void spawnPlayer();
	void spawnEnemy();
	void spawnSmallEnemies(std::shared_ptr<Entity> entity);
	void spawnBullet(std::shared_ptr<Entity> entity, const Vec2& mousePos);

public:

	Game(const std::string& config);	// constructor, takes in game config

	void run();
};