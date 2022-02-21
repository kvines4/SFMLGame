#include "Game.h"

Game::Game(const std::string& config)
{
	init(config);
}

void Game::init(const std::string& config)
{
	// initialise random number generator
	srand(time(0));

	std::ifstream fin(config);
	if (!fin.is_open())
	{
		std::cerr << "Unable to open config!" << std::endl;
		exit(-1);
	}

	// window properties
	int W = 1280;
	int H = 720;
	int FL = 60;
	int FS = 0;

	while (!fin.eof())
	{
		std::string option;
		fin >> option;
		if (option == "Window")
		{
			fin >> W >> H >> FL >> FS;
		}
		else if (option == "Font")
		{
			std::string fontPath;
			int fontSize, r, g, b;

			fin >> fontPath >> fontSize >> r >> g >> b;
			sf::Color fontColor(r, g, b);

			// attempt to load the font from a file
			if (!m_font.loadFromFile(fontPath))
			{
				std::cerr << "Could not load font!" << std::endl;
				exit(-1);
			}

			m_text = sf::Text("Score: 0", m_font, fontSize);
			m_text.setColor(fontColor);
			m_text.setPosition(10, 10);
		}
		else if (option == "Player")
		{
			fin >> m_playerConfig.SR	// Shape Radius
				>> m_playerConfig.CR	// Collision Radius
				>> m_playerConfig.S		// Speed
				>> m_playerConfig.FR	// Fill Color R
				>> m_playerConfig.FG	// Fill Color G
				>> m_playerConfig.FB	// Fill Color B
				>> m_playerConfig.OR	// Outline Color R
				>> m_playerConfig.OG	// Outline Color G
				>> m_playerConfig.OB	// Outline Color B
				>> m_playerConfig.OT	// Outline Thickness
				>> m_playerConfig.V;	// Shape Vertices
		}
		else if (option == "Enemy")
		{
			fin >> m_enemyConfig.SR		// Shape Radius
				>> m_enemyConfig.CR		// Collision Radius
				>> m_enemyConfig.SMIN	// Min Speed
				>> m_enemyConfig.SMAX	// Max Speed
				>> m_enemyConfig.OR		// Outline Color R 
				>> m_enemyConfig.OG		// Outline Color G
				>> m_enemyConfig.OB		// Outline Color B
				>> m_enemyConfig.OT		// Outline Thickness
				>> m_enemyConfig.VMIN	// Min Vertices
				>> m_enemyConfig.VMAX	// Max Vertices
				>> m_enemyConfig.L		// Small Lifespan
				>> m_enemyConfig.SI;	// Spawn Interval
		}
		else if (option == "Bullet")
		{
			fin >> m_bulletConfig.SR	// Shape Radius
				>> m_bulletConfig.CR	// Collision Radius
				>> m_bulletConfig.S		// Speed
				>> m_bulletConfig.FR	// Fill Color R
				>> m_bulletConfig.FG	// Fill Color G
				>> m_bulletConfig.FB	// Fill Color B
				>> m_bulletConfig.OR	// Outline Color R
				>> m_bulletConfig.OG	// Outline Color G
				>> m_bulletConfig.OB	// Outline Color B
				>> m_bulletConfig.OT	// Outline Thickness
				>> m_bulletConfig.V		// Shape Vertices
				>> m_bulletConfig.L;	// Lifespan
		}
	}

	// set up default window parameters
	m_window.create(sf::VideoMode(W, H), "Geometry Wars Clone");
	m_window.setFramerateLimit(FL);

	spawnPlayer();
}

void Game::run()
{
	sf::Clock clock;
	float lastTime = 0;
	float framesSinceFPSUpdate = 0;

	while (m_running)
	{
		m_entities.update();

		if (!m_paused)
		{
			sUserInput();
			sLifespan();
			sEnemySpawner();
			sMovement();
			sCollision();

			m_currentFrame++;
		}

		sRender();

		// Check FPS
		framesSinceFPSUpdate++;
		float currentTime = clock.restart().asSeconds();

		if (framesSinceFPSUpdate >= 30)
		{
			m_fps = 1.f / currentTime;
			lastTime = currentTime;
			framesSinceFPSUpdate = 0;
		}
	}
}

void Game::togglePause()
{
	m_paused = !m_paused;
}

void Game::toggleDebug()
{
	m_debug = !m_debug;
}

float Game::toRadians(int a)
{
	return a * 3.141f / 180;
}

// respawn the player in the middle of the screen
void Game::spawnPlayer()
{
	auto wDimensions = m_window.getSize();
	auto entity = m_entities.addEntity("player");

	// give this entity a Transform so it spawns in the centre
	entity->cTransform = std::make_shared<CTransform>(
		Vec2(wDimensions.x / 2, wDimensions.y / 2),
		Vec2(0, 0), 
		0.0f);

	// give this entity a shape based on playerConfig
	entity->cShape = std::make_shared<CShape>(
		m_playerConfig.SR,
		m_playerConfig.V,
		sf::Color(m_playerConfig.FR, m_playerConfig.FG, m_playerConfig.FB),
		sf::Color(m_playerConfig.OR, m_playerConfig.OG, m_playerConfig.OB),
		m_playerConfig.OT);

	// give this entity a collision radius based on playerConfig
	entity->cCollision = std::make_shared<CCollision>(m_playerConfig.CR);

	// add an input component to the player so that we can use inputs
	entity->cInput = std::make_shared<CInput>();

	// since we want this Entity to be our player, set our Game's player variable to be this Entity
	// this goes slightly agains the EntityManager paradigm, but we use the player so much its worth it
	m_player = entity;
}

// spawns an enemy at a random position
void Game::spawnEnemy()
{
	auto wDimensions = m_window.getSize();

	int x = rand() % (wDimensions.x - m_enemyConfig.SR * 2) + m_enemyConfig.SR;
	int y = rand() % (wDimensions.y - m_enemyConfig.SR * 2) + m_enemyConfig.SR;
	int a = rand() % 361;
	int s = rand() % (int)(1 + m_enemyConfig.SMAX - m_enemyConfig.SMIN) + m_enemyConfig.SMIN;
	int v = rand() %      (1 + m_enemyConfig.VMAX - m_enemyConfig.VMIN) + m_enemyConfig.VMIN;
	int r = rand() % 256;
	int g = rand() % 256;
	int b = rand() % 256;

	auto entity = m_entities.addEntity("enemy");

	// give this entity a Transform so it spawns somewhere within the screen
	entity->cTransform = std::make_shared<CTransform>(
		Vec2(x, y),
		Vec2(cos(toRadians(a)), sin(toRadians(a))),
		a);

	// give this entity a shape based on enemyConfig
	entity->cShape = std::make_shared<CShape>(
		m_enemyConfig.SR,
		v,
		sf::Color(r, g, b),
		sf::Color(m_enemyConfig.OR, m_enemyConfig.OG, m_enemyConfig.OB),
		m_enemyConfig.OT);

	// give this entity a collision radius based on enemyConfig
	entity->cCollision = std::make_shared<CCollision>(m_enemyConfig.CR);

	// give this entity a score based on enemyConfig
	entity->cScore = std::make_shared<CScore>(100);

	// record when the most recent enemy was spawned
	m_lastEnemySpawnTime = m_currentFrame;
}

// spawns the small enemies when a big one (input entity e) explodes
void Game::spawnSmallEnemies(std::shared_ptr<Entity> e)
{
	int vertices = e->cShape->circle.getPointCount();

	for (int i = 0; i < vertices; i++)
	{
		int a = 360 / vertices * i;
		auto entity = m_entities.addEntity("small");

		// give this entity a Transform so it spawns shooting out from its parent
		entity->cTransform = std::make_shared<CTransform>(
			e->cTransform->pos,
			Vec2(cos(toRadians(a)), sin(toRadians(a))),
			0);

		// give this entity a shape based on its parent
		entity->cShape = std::make_shared<CShape>(
			m_enemyConfig.SR/2,
			e->cShape->circle.getPointCount(),
			e->cShape->circle.getFillColor(),
			e->cShape->circle.getOutlineColor(),
			m_enemyConfig.OT);

		entity->cLifespan = std::make_shared<CLifespan>(m_enemyConfig.L);

		// give this entity a collision radius based on enemyConfig
		entity->cCollision = std::make_shared<CCollision>(e->cCollision->radius / 2);

		// give this entity a score twice that of its parent
		entity->cScore = std::make_shared<CScore>(e->cScore->score * 2);
	}
}

// spawns a bullet from a given entity to a target location
void Game::spawnBullet(std::shared_ptr<Entity> p, const Vec2& mousePos)
{
	auto entity = m_entities.addEntity("bullet");

	// first get the direction
	Vec2 direction = mousePos - p->cTransform->pos;
	// then normalise
	float length = p->cTransform->pos.dist(mousePos);
	Vec2 normalizedDir = direction / length;

	// give this entity a Transform so it spawns somewhere within the screen
	entity->cTransform = std::make_shared<CTransform>(
		p->cTransform->pos,
		normalizedDir * m_bulletConfig.S,
		0.0f);

	// give this entity a collision radius based on m_bulletConfig
	entity->cCollision = std::make_shared<CCollision>(m_bulletConfig.CR);

	// give this entity a lifespan based on m_bulletConfig
	entity->cLifespan = std::make_shared<CLifespan>(m_bulletConfig.L);

	// give this entity a shape based on enemyConfig
	entity->cShape = std::make_shared<CShape>(
		m_bulletConfig.SR,
		m_bulletConfig.V,
		sf::Color(m_bulletConfig.FR, m_bulletConfig.FG, m_bulletConfig.FB),
		sf::Color(m_bulletConfig.OR, m_bulletConfig.OG, m_bulletConfig.OB),
		m_bulletConfig.OT);

}

void Game::sMovement()
{
	// calculate player velocity
	Vec2 playerVelocity;
	if (m_player->cInput->left)	 { playerVelocity.x -= m_playerConfig.S; }
	if (m_player->cInput->right) { playerVelocity.x += m_playerConfig.S; }
	if (m_player->cInput->up)    { playerVelocity.y -= m_playerConfig.S; }
	if (m_player->cInput->down)  { playerVelocity.y += m_playerConfig.S; }
	m_player->cTransform->velocity = playerVelocity;

	// add entity velocities to position
	for (auto e : m_entities.getEntities()) 
	{
		e->cTransform->pos += e->cTransform->velocity;
	}
}

void Game::sLifespan()
{
	for (auto e : m_entities.getEntities())
	{
		// check if still alive and update alpha value accordingly
		if (e->cLifespan)
		{
			if (e->cLifespan->remaining > 0)
			{
				e->cLifespan->remaining -= 1;
				int newAlpha = 255 * e->cLifespan->remaining / e->cLifespan->total;

				auto fill = e->cShape->circle.getFillColor();
				auto outline = e->cShape->circle.getOutlineColor();

				sf::Color newfill(fill.r, fill.g, fill.b, newAlpha);
				sf::Color newoutline(outline.r, outline.g, outline.b, newAlpha);

				e->cShape->circle.setFillColor(newfill);
				e->cShape->circle.setOutlineColor(newoutline);
			}
			else
			{
				e->destroy();
			}
		}
	}
}

void Game::sCollision()
{
	auto wDimensions = m_window.getSize();

	// Check player is within bounds
	Vec2 pPos = m_player->cTransform->pos;
	int pRad  = m_player->cCollision->radius;
	if		(pPos.x - pRad < 0)				{ m_player->cTransform->pos.x = 0			  + m_player->cCollision->radius; }
	else if (pPos.x + pRad > wDimensions.x) { m_player->cTransform->pos.x = wDimensions.x - m_player->cCollision->radius; }
	if		(pPos.y - pRad < 0)				{ m_player->cTransform->pos.y = 0			  + m_player->cCollision->radius; }
	else if (pPos.y + pRad > wDimensions.y) { m_player->cTransform->pos.y = wDimensions.y - m_player->cCollision->radius; }

	// Check enemies are withing bounds
	for (auto e : m_entities.getEntities("enemy"))
	{
		Vec2 pos = e->cTransform->pos;
		float r  = e->cCollision->radius;
		// collision check
		if (pos.x - r < 0 || pos.x + r > wDimensions.x) { e->cTransform->velocity.x *= -1; }
		if (pos.y - r < 0 || pos.y + r > wDimensions.y) { e->cTransform->velocity.y *= -1; }
	}

	// check player collision with enemies
	for (auto p : m_entities.getEntities("player"))
	{
		for (auto e : m_entities.getEntities("enemy"))
		{
			if (p->isActive() && e->isActive())
			{
				Vec2 d  = e->cTransform->pos - p->cTransform->pos;			// distance vector
				float r = e->cCollision->radius + p->cCollision->radius;	// total collision radius
				float distSq = d.x * d.x + d.y * d.y;						// distance squared 
				float radSq  = r * r;										// radius squared
				if (distSq < radSq)
				{
					e->destroy();
					p->destroy();
					spawnPlayer();
				}
			}
		}
		for (auto e : m_entities.getEntities("small"))
		{
			if (p->isActive() && e->isActive())
			{
				Vec2 d  = e->cTransform->pos - p->cTransform->pos;			// distance vector
				float r = e->cCollision->radius + p->cCollision->radius;	// total collision radius
				float distSq = d.x * d.x + d.y * d.y;						// distance squared 
				float radSq  = r * r;										// radius squared
				if (distSq < radSq)
				{
					e->destroy();
					p->destroy();
					spawnPlayer();
				}
			}
		}
	}

	// check player collision with bullets
	for (auto b : m_entities.getEntities("bullet"))
	{
		for (auto e : m_entities.getEntities("enemy"))
		{
			if (b->isActive() && e->isActive())
			{
				Vec2 d  = e->cTransform->pos - b->cTransform->pos;			// distance vector
				float r = e->cCollision->radius + b->cCollision->radius;	// total collision radius
				float distSq = d.x * d.x + d.y * d.y;						// distance squared 
				float radSq  = r * r;										// radius squared
				if (distSq < radSq)
				{
					m_score += e->cScore->score;
					e->destroy();
					b->destroy();
					spawnSmallEnemies(e);
				}
			}
		}
		for (auto e : m_entities.getEntities("small"))
		{
			if (b->isActive() && e->isActive())
			{
				Vec2 d  = e->cTransform->pos - b->cTransform->pos;			// distance vector
				float r = e->cCollision->radius + b->cCollision->radius;	// total collision radius
				float distSq = d.x * d.x + d.y * d.y;						// distance squared 
				float radSq  = r * r;										// radius squared
				if (distSq < radSq)
				{
					m_score += e->cScore->score;
					e->destroy();
					b->destroy();
				}
			}
		}
	}
}

void Game::sEnemySpawner()
{
	if (m_currentFrame - m_lastEnemySpawnTime > m_enemyConfig.SI)
	{
		m_lastEnemySpawnTime = m_currentFrame;
		spawnEnemy();
	}
}

void Game::sRender()
{
	m_window.clear();
	auto wDimensions = m_window.getSize();
	wDimensions.x /= 20;
	wDimensions.y /= 20;

	// draw background shapes
	sf::RectangleShape rectBG(sf::Vector2f(0, 0));
	for (int i = 0; i < wDimensions.x; i++)
	{
		for (int j = 0; j < wDimensions.y; j++)
		{
			rectBG.setPosition(sf::Vector2f(i * 20, j * 20));
			rectBG.setSize(sf::Vector2f(20, 20));
			if ((i + j) % 2 == 0)
				rectBG.setFillColor(sf::Color(20, 20, 20));
			else
				rectBG.setFillColor(sf::Color(0, 0, 0));
			m_window.draw(rectBG);
		}
	}

	for (auto e : m_entities.getEntities())
	{
		// set the position of the shape based on the entity's transform->pos
		e->cShape->circle.setPosition(e->cTransform->pos.x, e->cTransform->pos.y);

		// set the rotation of the shape based on the entity's transform->angle
		e->cTransform->angle += 1.0f;
		e->cShape->circle.setRotation(e->cTransform->angle);

		// draw the entity's sf::CircleShape
		m_window.draw(e->cShape->circle);
	}

	// print score
	std::stringstream score;
	score << "Score: " << m_score << std::endl;
	m_text.setString(score.str());
	m_window.draw(m_text);

	// print some optional game stats
	if (m_debug)
	{
		sf::RectangleShape bg(sf::Vector2f(150, 150));
		bg.setPosition(sf::Vector2f(0, m_window.getSize().y - 130));
		bg.setFillColor(sf::Color::Black);

		std::stringstream ss;

		ss << "Up: " << m_player->cInput->up << std::endl;
		ss << "Down: " << m_player->cInput->down << std::endl;
		ss << "Left: " << m_player->cInput->left << std::endl;
		ss << "Right: " << m_player->cInput->right << std::endl;
		ss << std::endl;
		ss << "FPS: " << m_fps << std::endl;
		ss << "Entites: " << m_entities.getTotal() << std::endl;
		ss << "Player Pos: " << m_player->cTransform->pos.x << "," << m_player->cTransform->pos.y << std::endl;


		sf::Text debug(ss.str(), m_font, 12);
		debug.setColor(sf::Color::White);
		debug.setPosition(10, m_window.getSize().y - 120);
		ss.str(std::string());

		m_window.draw(bg);
		m_window.draw(debug);
	}
	m_window.display();
}

void Game::sUserInput()
{
	sf::Event event;
	while (m_window.pollEvent(event))
	{
		// this event triggers when the window is closed
		if (event.type == sf::Event::Closed)
		{
			m_running = false;
		}

		// this event is triggered when a key is pressed
		if (event.type == sf::Event::KeyPressed)
		{
			switch (event.key.code)
			{
				case sf::Keyboard::W:
					m_player->cInput->up = true;
					break;
				case sf::Keyboard::A:
					m_player->cInput->left = true;
					break;
				case sf::Keyboard::S:
					m_player->cInput->down = true;
					break;
				case sf::Keyboard::D:
					m_player->cInput->right = true;
					break;
				case sf::Keyboard::P:
					togglePause();
					break;
				case sf::Keyboard::F3:
					toggleDebug();
					break;
			}
		}

		// this event is triggered when a key is released
		if (event.type == sf::Event::KeyReleased)
		{
			switch (event.key.code)
			{
				case sf::Keyboard::W:
					m_player->cInput->up = false;
					break;
				case sf::Keyboard::A:
					m_player->cInput->left = false;
					break;
				case sf::Keyboard::S:
					m_player->cInput->down = false;
					break;
				case sf::Keyboard::D:
					m_player->cInput->right = false;
					break;
			}
		}

		if (event.type == sf::Event::MouseButtonPressed)
		{
			if (event.mouseButton.button == sf::Mouse::Left)
			{
				spawnBullet(m_player, Vec2(event.mouseButton.x, event.mouseButton.y));
			}
		}
	}
}