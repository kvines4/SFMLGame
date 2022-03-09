#include "Scene_Play.h"
#include "Common.h"
#include "Physics.h"
#include "Assets.h"
#include "GameEngine.h"
#include "Components.h"
#include "Action.h"

Scene_Play::Scene_Play(GameEngine* gameEngine, const std::string& levelPath)
	: Scene(gameEngine)
	, m_levelPath(levelPath)
{
	init(m_levelPath);
}

void Scene_Play::init(const std::string& levelPath)
{
	registerAction(sf::Keyboard::P,		 "PAUSE");
	registerAction(sf::Keyboard::Escape, "QUIT");
	registerAction(sf::Keyboard::T,		 "TOGGLE_TEXTURE");			// toggle drawing (T)extures
	registerAction(sf::Keyboard::C,		 "TOGGLE_COLLISION");		// toggle drawing (C)ollision Boxes
	registerAction(sf::Keyboard::G,		 "TOGGLE_GRID");			// toggle drawing (G)rid

	registerAction(sf::Keyboard::D,		 "RIGHT");
	registerAction(sf::Keyboard::A,		 "LEFT");
	registerAction(sf::Keyboard::W,		 "JUMP");

	registerAction(sf::Keyboard::Space,  "SHOOT");

	m_gridText.setCharacterSize(12);
	m_gridText.setFont(m_game->assets().getFont("Arial"));

	loadLevel(levelPath);
}

Vec2 Scene_Play::gridToMidPixel(float gridX, float gridY, std::shared_ptr<Entity> entity)
{
	auto& animSize = entity->getComponent<CAnimation>().animation.getSize();

	return Vec2((gridX * m_gridSize.x) + (animSize.x / 2),
			    (12    * m_gridSize.y) - (gridY * m_gridSize.y) - (animSize.y / 2));
}

// Note: components are now returned as references rather than pointers
//		 if you do not specify a reference variable type, it will COPY the component
void Scene_Play::loadLevel(const std::string& filename)
{
	// reset the entity manager every time we load a level
	m_entityManager = EntityManager();

	std::ifstream file(filename);
	std::string str;
	while (file.good())
	{
		file >> str;

		if (str == "Tile")
		{
			std::string n;
			float gx, gy;
			file >> n >> gx >> gy;
			auto tile = m_entityManager.addEntity("tile");
			// IMPORTANT: always add the CAnimation component first so that gridToMidPixel can compute correctly
			tile->addComponent<CAnimation>(m_game->assets().getAnimation(n), true);
			tile->addComponent<CTransform>(gridToMidPixel(gx, gy, tile));
			tile->addComponent<CBoundingBox>(tile->getComponent<CAnimation>().animation.getSize());
		}
		else if (str == "Dec")
		{
			std::string n;
			float gx, gy;
			file >> n >> gx >> gy;
			auto dec = m_entityManager.addEntity("dec");
			// IMPORTANT: always add the CAnimation component first so that gridToMidPixel can compute correctly
			dec->addComponent<CAnimation>(m_game->assets().getAnimation(n), true);
			dec->addComponent<CTransform>(gridToMidPixel(gx, gy, dec));
		}
		else if (str == "Player")
		{
			float gx, gy, cw, ch, sx, sy, sm, g;
			std::string b;
			file >> gx >> gy >> cw >> ch >> sx >> sy >> sm >> g >> b;

			m_playerConfig.X		= gx;
			m_playerConfig.Y		= gy;
			m_playerConfig.CX		= cw;
			m_playerConfig.CY		= ch;
			m_playerConfig.SPEED	= sx;
			m_playerConfig.JUMP		= sy;
			m_playerConfig.MAXSPEED = sm;
			m_playerConfig.GRAVITY	= g;
			m_playerConfig.WEAPON	= b;

			spawnPlayer();
		}
		else
		{
			std::cerr << "Unknown Entity Type: " << str << " " << filename << std::endl;
		}
	}
}

void Scene_Play::spawnPlayer()
{
	m_player = m_entityManager.addEntity("player");
	m_player->addComponent<CAnimation>(m_game->assets().getAnimation("Air"), true);
	m_player->addComponent<CTransform>(Vec2(224, 352));
	m_player->addComponent<CBoundingBox>(Vec2(48, 48));
	m_player->addComponent<CGravity>(m_playerConfig.GRAVITY);
}

void Scene_Play::spawnBullet(std::shared_ptr<Entity> Entity)
{
	auto bullet = m_entityManager.addEntity("bullet");
	bullet->addComponent<CAnimation>(m_game->assets().getAnimation(m_playerConfig.WEAPON), true);
	auto & t = bullet->addComponent<CTransform>(Entity->getComponent<CTransform>().pos);
	bullet->addComponent<CBoundingBox>(bullet->getComponent<CAnimation>().animation.getSize());
	bullet->addComponent<CLifespan>(120);

	if (Entity->getComponent<CTransform>().scale.x > 0) { t.velocity = Vec2( 15, 0); }
	else												{ t.velocity = Vec2(-15, 0); }
}

void Scene_Play::update()
{
	m_entityManager.update();

	if (!m_paused)
	{
		sMovement();
		sLifespan();
		sCollision();
		sAnimation();
	}
}

void Scene_Play::sMovement()
{
	auto& pTransform = m_player->getComponent<CTransform>();
	auto& pInput	 = m_player->getComponent<CInput>();

	pTransform.prevPos = pTransform.pos;

	// player horizontal velocity
	if (pInput.right)
	{
		pTransform.scale.x = 1.0f;
		pTransform.velocity.x += 2.0f;
		if (pTransform.velocity.x > m_playerConfig.SPEED)
		{
			pTransform.velocity.x = m_playerConfig.SPEED;
		}

	}
	if (pInput.left)
	{
		pTransform.scale.x = -1.0f;
		pTransform.velocity.x -= 2.0f;
		if (pTransform.velocity.x < -m_playerConfig.SPEED)
		{
			pTransform.velocity.x = -m_playerConfig.SPEED;
		}
	}
	if ( pInput.left &&  pInput.right ||
		!pInput.left && !pInput.right)
	{
		pTransform.velocity.x = 0;
	}

	// player vertical velocity
	if (pInput.up && pInput.canJump)
	{
		pInput.canJump = false;
		m_player->getComponent<CState>().state = "jumping";
		pTransform.velocity.y = m_playerConfig.JUMP;
	}
	else
	{
		if (pTransform.velocity.y < 0 && !pInput.up)
		{
			pTransform.velocity.y = 0;
		}
		pTransform.velocity.y += m_player->getComponent<CGravity>().gravity;
		if (pTransform.velocity.y > m_playerConfig.MAXSPEED)
		{
			pTransform.velocity.y = m_playerConfig.MAXSPEED;
		}
	}

	pTransform.pos += pTransform.velocity;

	// loop through bullets explicitly as they are the 
	// only moving entities besides m_player in this implementation
	for (auto e : m_entityManager.getEntities("bullet"))
	{
		auto& transform = e->getComponent<CTransform>();
		transform.pos += transform.velocity;
	}
}

void Scene_Play::sLifespan()
{
	for (auto e : m_entityManager.getEntities())
	{
		if (e->hasComponent<CLifespan>())
		{
			auto& lifespan = e->getComponent<CLifespan>();
			if (lifespan.remaining > 0)
			{
				lifespan.remaining -= 1;
			}
			else
			{
				e->destroy();
			}
		}
	}
}

void Scene_Play::sCollision()
{
	// SFML's (0,0) position is on the TOP-LEFT corner
	// This means jumping will have a negative y-component
	// and also gravity will have a positive y-component
	// Also, something BELOW something else will have a y value GREATER than it
	// Also, something ABOVE something else will have a y value LESS than it

	for (auto b : m_entityManager.getEntities("bullet"))
	{
		for (auto t : m_entityManager.getEntities("tile"))
		{
			Vec2 overlap = Physics::GetOverlap(b, t);
			if (overlap.x > 0 && overlap.y > 0)
			{
				b->destroy();
				if (t->getComponent<CAnimation>().animation.getName() == "Brick")
				{
					auto& transformT = t->getComponent<CTransform>();

					t->destroy();
					//spawn explosion
					auto dec = m_entityManager.addEntity("dec");
					dec->addComponent<CAnimation>(m_game->assets().getAnimation("Explosion"), false);
					dec->addComponent<CTransform>(Vec2(transformT.pos.x, transformT.pos.y));
				}
			}
		}
	}

	for (auto p : m_entityManager.getEntities("player"))
	{
		for (auto t : m_entityManager.getEntities("tile"))
		{
			Vec2 overlap	 = Physics::GetOverlap(p, t);
			Vec2 prevOverlap = Physics::GetPreviousOverlap(p, t);

			if (overlap.x > 0 && overlap.y > 0)
			{
				auto& transformP = p->getComponent<CTransform>();
				auto& transformT = t->getComponent<CTransform>();

				if (t->getComponent<CAnimation>().animation.getName() == "Pole" ||
					t->getComponent<CAnimation>().animation.getName() == "PoleTop")
				{
					// you win. restart level.
					m_game->changeScene("PLAY", std::make_shared<Scene_Play>(m_game, "level1.txt"));
				}

				if (prevOverlap.y > 0 && transformP.pos.x > transformT.pos.x)
				{
					// Right
					transformP.pos.x += overlap.x;
				}
				else if (prevOverlap.y > 0 && transformP.pos.x < transformT.pos.x)
				{
					// left
					transformP.pos.x -= overlap.x;
				}
				else if (prevOverlap.x > 0 && transformP.pos.y > transformT.pos.y)
				{
					// Bottom
					transformP.pos.y += overlap.y;

					p->getComponent<CInput>().canJump = false;
					if (t->getComponent<CAnimation>().animation.getName() == "Brick")
					{
						t->destroy();
						//spawn explosion
						auto dec = m_entityManager.addEntity("dec");
						dec->addComponent<CAnimation>(m_game->assets().getAnimation("Explosion"), false);
						dec->addComponent<CTransform>(Vec2(transformT.pos.x, transformT.pos.y));
					}
					else if (t->getComponent<CAnimation>().animation.getName() == "Question")
					{
						t->getComponent<CAnimation>().animation = m_game->assets().getAnimation("Question2");
						
						//spawn coin
						auto dec = m_entityManager.addEntity("dec");
						dec->addComponent<CAnimation>(m_game->assets().getAnimation("Coin"), false);
						dec->addComponent<CTransform>(Vec2(transformT.pos.x, transformT.pos.y - m_gridSize.y));
					}
					if (transformP.velocity.y < 0)
					{
						transformP.velocity.y = 0;
					}
				}
				// on the rare edge case where the previous frame had 
				// no overlap, just assume we landed on top 
				else //if (prevOverlap.x > 0 && transformP.pos.y < transformT.pos.y)
				{
					// Top
					transformP.pos.y -= overlap.y;

					transformP.velocity.x == 0
						? p->getComponent<CState>().state = "stand"
						: p->getComponent<CState>().state = "run";

					if (!p->getComponent<CInput>().up)
					{
						p->getComponent<CInput>().canJump = true;
					}
					if (transformP.velocity.y > 0)
					{
						transformP.velocity.y = 0;
					}
				}
				
			}
		}
	}

	auto& transformP   = m_player->getComponent<CTransform>();
	auto& boundingBoxP = m_player->getComponent<CBoundingBox>();

	if (transformP.pos.y > height())
	{
		m_player->destroy();
		spawnPlayer();
	}

	if (transformP.pos.x < boundingBoxP.halfSize.x)
	{
		transformP.pos.x = boundingBoxP.halfSize.x;
	}
}

void Scene_Play::sDoAction(Action action)
{
	if (action.type() == "START")
	{
			 if (action.name() == "TOGGLE_TEXTURE")		{ m_drawTextures   = !m_drawTextures;	}
		else if (action.name() == "TOGGLE_COLLISION")	{ m_drawCollisions = !m_drawCollisions;	}
		else if (action.name() == "TOGGLE_GRID")		{ m_drawGrid	   = !m_drawGrid;		}

		else if (action.name() == "PAUSE")				{ setPaused(!m_paused);	}
		else if (action.name() == "QUIT")				{ onEnd();				}

		else if (action.name() == "RIGHT")				{ m_player->getComponent<CInput>().right = true; }
		else if (action.name() == "LEFT")				{ m_player->getComponent<CInput>().left  = true; }
		else if (action.name() == "JUMP")				{ m_player->getComponent<CInput>().up	 = true; }

		else if (action.name() == "SHOOT")
		{
			m_player->getComponent<CInput>().shoot = true;
			if (m_player->getComponent<CInput>().canShoot == true && !m_paused)
			{
				spawnBullet(m_player);
				m_player->getComponent<CInput>().canShoot = false;
			}
		}
	}
	else if (action.type() == "END") 
	{
			 if (action.name() == "RIGHT")	{ m_player->getComponent<CInput>().right    = false; }
		else if (action.name() == "LEFT")	{ m_player->getComponent<CInput>().left     = false; }
		else if (action.name() == "JUMP")	{ m_player->getComponent<CInput>().up		= false;
											  m_player->getComponent<CInput>().canJump	= false; }

		else if (action.name() == "SHOOT")	{ m_player->getComponent<CInput>().shoot	= false;
											  m_player->getComponent<CInput>().canShoot = true;  }
	}
}

void Scene_Play::sAnimation()
{
	auto& pState	 = m_player->getComponent<CState>();
	auto& pAnimation = m_player->getComponent<CAnimation>();

	if (pState.state != pState.previousState)
	{
		if (pState.state == "run")	   { pState.previousState = pState.state; 
										 pAnimation.animation = m_game->assets().getAnimation("Run");   }
		if (pState.state == "stand")   { pState.previousState = pState.state; 
										 pAnimation.animation = m_game->assets().getAnimation("Stand"); }
		if (pState.state == "jumping") { pState.previousState = pState.state; 
										 pAnimation.animation = m_game->assets().getAnimation("Air");   }
	}

	for (auto e : m_entityManager.getEntities())
	{
		if (e->hasComponent<CAnimation>()) 
		{
			auto& anim = e->getComponent<CAnimation>();
			if (anim.repeat || !anim.animation.hasEnded())
			{
				anim.animation.update();
			}
			else if (anim.animation.hasEnded())
			{
				e->destroy();
			}
		}
	}
}

void Scene_Play::onEnd()
{
	m_game->changeScene("MENU", nullptr, true);
}

void Scene_Play::drawLine(const Vec2& p1, const Vec2& p2)
{
	sf::Vertex line[] = { sf::Vector2f(p1.x, p1.y), sf::Vector2f(p2.x, p2.y) };
	m_game->window().draw(line, 2, sf::Lines);
}

void Scene_Play::sRender()
{
	// set the viewport of the window to be centered on the player if it's far enough right
	auto& pPos = m_player->getComponent<CTransform>().pos;
	float windowCenterX = std::max(m_game->window().getSize().x / 2.0f, pPos.x);
	sf::View view = m_game->window().getView();
	view.setCenter(windowCenterX, m_game->window().getSize().y - view.getCenter().y);
	m_game->window().setView(view);

	// color the background darker so you know that the game is paused
	if (!m_paused) { m_game->window().clear(sf::Color(100, 100, 255)); }
	else { m_game->window().clear(sf::Color(50, 50, 150)); }

	// draw all Entity textures / animations
	if (m_drawTextures)
	{
		for (auto e : m_entityManager.getEntities())
		{
			if (e->hasComponent<CAnimation>()) {
				auto& transform = e->getComponent<CTransform>();
				auto& animation = e->getComponent<CAnimation>().animation;

				animation.getSprite().setRotation(transform.angle);
				animation.getSprite().setPosition(transform.pos.x, transform.pos.y);
				animation.getSprite().setScale(transform.scale.x, transform.scale.y);

				m_game->window().draw(animation.getSprite());
			}
		}
	}

	// draw the grid so that we can easily debug
	if (m_drawGrid)
	{
		float leftX = m_game->window().getView().getCenter().x - width() / 2;
		float rightX = leftX + width() + m_gridSize.x;
		float nextGridX = leftX - ((int)leftX % (int)m_gridSize.x);

		for (float x = nextGridX; x < rightX; x += m_gridSize.x)
		{
			drawLine(Vec2(x, 0), Vec2(x, height()));
		}

		for (float y = 0; y < height(); y += m_gridSize.y)
		{
			drawLine(Vec2(leftX, height() - y), Vec2(rightX, height() - y));

			for (float x = nextGridX; x < rightX; x += m_gridSize.x)
			{
				std::string xCell = std::to_string((int)x / (int)m_gridSize.x);
				std::string yCell = std::to_string((int)y / (int)m_gridSize.y);
				m_gridText.setString("(" + xCell + "," + yCell + ")");
				m_gridText.setPosition(x + 3, height() - y - m_gridSize.y + 2);
				m_game->window().draw(m_gridText);
			}
		}
	}

	// draw all Entity collision bounding boxes so that we can easily debug
	if (m_drawCollisions)
	{
		for (auto e : m_entityManager.getEntities())
		{
			if (e->hasComponent<CBoundingBox>())
			{
				auto& box = e->getComponent<CBoundingBox>();
				auto& transform = e->getComponent<CTransform>();

				sf::RectangleShape rect;
				rect.setSize(sf::Vector2f(box.size.x - 1, box.size.y - 1));
				rect.setOrigin(sf::Vector2f(box.halfSize.x, box.halfSize.y));
				rect.setPosition(transform.pos.x, transform.pos.y + 1);
				rect.setFillColor(sf::Color(0, 0, 0, 0));
				rect.setOutlineColor(sf::Color::Red);
				rect.setOutlineThickness(1);

				m_game->window().draw(rect);
			}
		}
	}
}