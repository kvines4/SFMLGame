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
	PROFILE_FUNCTION();

	{
		PROFILE_SCOPE("Register Actions");

		registerAction(sf::Keyboard::W,		 "JUMP");
		registerAction(sf::Keyboard::A,		 "LEFT");
		registerAction(sf::Keyboard::D,		 "RIGHT");
		registerAction(sf::Keyboard::P,		 "PAUSE");
		registerAction(sf::Keyboard::Space,  "SHOOT");
		registerAction(sf::Keyboard::Escape, "QUIT");
		registerAction(sf::Keyboard::T,		 "TOGGLE_TEXTURE");			// toggle drawing (T)extures
		registerAction(sf::Keyboard::C,		 "TOGGLE_COLLISION");		// toggle drawing (C)ollision Boxes
		registerAction(sf::Keyboard::G,		 "TOGGLE_GRID");			// toggle drawing (G)rid
	}

	m_mouseShape.setRadius(8);
	m_mouseShape.setOrigin(8,8);
	m_mouseShape.setPointCount(32);
	m_mouseShape.setFillColor(sf::Color(255,0,0,196));

	m_gridText.setCharacterSize(12);
	m_gridText.setFont(m_game->assets().getFont("Arial"));

	loadLevel(levelPath);
}

Vec2 Scene_Play::gridToMidPixel(float gridX, float gridY, Entity entity)
{
	const Vec2& animSize = entity.getComponent<CAnimation>().animation.getSize();

	return Vec2((gridX * m_gridSize.x) + (animSize.x / 2),
		        height() - (gridY * m_gridSize.y) - (animSize.y / 2));
}

void Scene_Play::loadLevel(const std::string& filename)
{
	PROFILE_FUNCTION();

	// reset the entity manager every time we load a level
	m_entityManager = EntityManager();

	std::ifstream file(filename);
	std::string str;

	float x, y;
	while (file.good())
	{
		file >> str;

		if (str == "Tile")
		{
			file >> str >> x >> y;
			Entity tile = m_entityManager.addEntity(Tag::tile);
			tile.addComponent<CAnimation>(m_game->assets().getAnimation(str), true);
			tile.addComponent<CTransform>(gridToMidPixel(x, y, tile));
			tile.addComponent<CBoundingBox>(tile.getComponent<CAnimation>().animation.getSize());
			tile.addComponent<CDraggable>();
		}
		else if (str == "Dec")
		{
			file >> str >> x >> y;
			Entity dec = m_entityManager.addEntity(Tag::decoration);
			dec.addComponent<CAnimation>(m_game->assets().getAnimation(str), true);
			dec.addComponent<CTransform>(gridToMidPixel(x, y, dec));
			dec.addComponent<CDraggable>();
		}
		else if (str == "Player")
		{
			file >> m_playerConfig.X >> m_playerConfig.Y >> m_playerConfig.CX >> m_playerConfig.CY;
			file >> m_playerConfig.SPEED >> m_playerConfig.JUMP >> m_playerConfig.MAXSPEED >> m_playerConfig.GRAVITY >> m_playerConfig.WEAPON;
			spawnPlayer();
		}
		else
		{
			std::cerr << "Unknown Entity Type: " << str << " " << filename << std::endl;
		}
	}

	m_entityManager.update();
}

void Scene_Play::spawnPlayer()
{
	PROFILE_FUNCTION();

	for (Entity entity : m_entityManager.getEntities(Tag::player)) { entity.destroy(); }

	Entity player = m_entityManager.addEntity(Tag::player);
	player.addComponent<CAnimation>(m_game->assets().getAnimation("Air"), true);
	player.addComponent<CTransform>(gridToMidPixel(m_playerConfig.X, m_playerConfig.Y, player));
	player.addComponent<CInput>();
	player.addComponent<CBoundingBox>(Vec2(48, 48));
	player.addComponent<CGravity>(m_playerConfig.GRAVITY);
	player.addComponent<CState>("air");
	player.addComponent<CDraggable>();
}

void Scene_Play::hitBlock(Entity entity)
{
	auto& tTransform = entity.getComponent<CTransform>();
	auto& tAnimation = entity.getComponent<CAnimation>();

	if (tAnimation.animation.getName() == "Brick")
	{
		entity.addComponent<CAnimation>(m_game->assets().getAnimation("Explosion"), false);
		entity.removeComponent<CBoundingBox>();
	}
	else if (tAnimation.animation.getName() == "Question")
	{
		tAnimation.animation = m_game->assets().getAnimation("Question2");

		Entity dec = m_entityManager.addEntity(Tag::decoration);
		dec.addComponent<CAnimation>(m_game->assets().getAnimation("Coin"), false);
		dec.addComponent<CTransform>(Vec2(tTransform.pos.x, tTransform.pos.y - m_gridSize.y));
	}
}

void Scene_Play::spawnBullet(Entity entity)
{
	auto& transform = entity.getComponent<CTransform>();
	Entity bullet	= m_entityManager.addEntity(Tag::bullet);
	bullet.addComponent<CTransform>(transform.pos, Vec2(12 * transform.scale.x, 0), transform.scale, 0.0f);
	bullet.addComponent<CAnimation>(m_game->assets().getAnimation(m_playerConfig.WEAPON), true);
	bullet.addComponent<CBoundingBox>(bullet.getComponent<CAnimation>().animation.getSize());
	bullet.addComponent<CLifespan>(60);
}

void Scene_Play::update()
{
	PROFILE_FUNCTION();

	m_entityManager.update();

	if (!m_paused)
	{
		sLifespan();
		sMovement();
		sDraggable();
		sCollision();
		sAnimation();
	}
}

void Scene_Play::sMovement()
{
	PROFILE_FUNCTION();

	Entity player = m_entityManager.getEntities(Tag::player)[0];

	auto& pTransform = player.getComponent<CTransform>();
	auto& pInput	 = player.getComponent<CInput>();
	auto& pState	 = player.getComponent<CState>();

	// player input
	Vec2 previousPlayerSpeed = pTransform.velocity;
	Vec2 playerInputSpeed(0.0f, pTransform.velocity.y);
	if (pInput.left)
	{
		playerInputSpeed.x -= m_playerConfig.SPEED;
		pTransform.scale.x = -1.0f;
	}
	if (pInput.right)
	{
		playerInputSpeed.x += m_playerConfig.SPEED;
		pTransform.scale.x = 1.0f;
	}
	if (pInput.up && pState.state != "air" && pInput.canJump)
	{
		playerInputSpeed.y = m_playerConfig.JUMP;
		pInput.canJump = false;
	}
	pTransform.velocity = playerInputSpeed;

	// shoot
	if (pInput.shoot && pInput.canShoot)
	{
		spawnBullet(player);
		pInput.canShoot = false;
	}

	// move entities
	for (Entity entity : m_entityManager.getEntities())
	{
		auto& transform = entity.getComponent<CTransform>();

		if (entity.hasComponent<CGravity>())
		{
			transform.velocity.y += entity.getComponent<CGravity>().gravity;
		}

		transform.prevPos = transform.pos;
		transform.pos += transform.velocity;
	}
}

void Scene_Play::sDraggable()
{
	PROFILE_FUNCTION();

	for (Entity draggable : m_entityManager.getEntities())
	{
		// update dragging entity
		if (draggable.hasComponent<CDraggable>() && draggable.getComponent<CDraggable>().dragging)
		{
			auto mousePosition = m_mouseShape.getPosition();
			auto& eTransform   = draggable.getComponent<CTransform>();
			auto& animSize     = draggable.getComponent<CAnimation>().animation.getSize();

			Vec2 p = Vec2(mousePosition.x + (animSize.x / 2) - (m_gridSize.x / 2),
				          mousePosition.y - (animSize.y / 2) + (m_gridSize.y / 2));

			eTransform.prevPos = eTransform.pos;
			eTransform.pos = p;

			return; // assume there is only 1
		}
	}
}

void Scene_Play::sLifespan()
{
	PROFILE_FUNCTION();

	for (Entity entity : m_entityManager.getEntities())
	{
		if (!entity.hasComponent<CLifespan>()) { continue; }

		auto& lifespan = entity.getComponent<CLifespan>();
		if (lifespan.remaining > 0)
		{
			lifespan.remaining -= 1;
		}
		else
		{
			entity.destroy();
		}
	}
}

void Scene_Play::sCollision()
{
	PROFILE_FUNCTION();
	
	const EntityVec& tiles = m_entityManager.getEntities(Tag::tile);

	{
		PROFILE_SCOPE("Bullet/Tile Collisions");

		auto& bullets = m_entityManager.getEntities(Tag::bullet);
		for (Entity bullet : bullets)
		{
			for (Entity tile : tiles)
			{
				if (!tile.hasComponent<CBoundingBox>()) { continue; }

				// if we aren't overlapping, continue to next tile
				Vec2 overlap = Physics::GetOverlap(bullet, tile);
				if (overlap.x < 0 || overlap.y < 0) { continue; }

				bullet.destroy();
				if (tile.getComponent<CAnimation>().animation.getName() == "Brick")
				{
					tile.addComponent<CAnimation>(m_game->assets().getAnimation("Explosion"), false);
					tile.removeComponent<CBoundingBox>();
				}
			}
		}
	}

	{
		PROFILE_SCOPE("Player/Tile Collisions");

		Entity player = m_entityManager.getEntities(Tag::player)[0];
		auto& pTransform = player.getComponent<CTransform>();
		auto& pState = player.getComponent<CState>();
		auto& pBoundingBox = player.getComponent<CBoundingBox>();
		auto& pInput = player.getComponent<CInput>();

		pState.state = "air";
		for (Entity tile : tiles)
		{
			// if we aren't overlapping, continue to next tile
			Vec2 overlap = Physics::GetOverlap(player, tile);
			if (overlap.x < 0 || overlap.y < 0) { continue; }

			Vec2 prevOverlap = Physics::GetPreviousOverlap(player, tile);
			auto& tTransform = tile.getComponent<CTransform>();
			auto& tAnimation = tile.getComponent<CAnimation>();

			if (tAnimation.animation.getName() == "Pole" ||
				tAnimation.animation.getName() == "PoleTop")
			{
				// you win. restart level.
				m_game->changeScene("PLAY", std::make_shared<Scene_Play>(m_game, "level1.txt"));
				return;
			}

			Vec2 diff = pTransform.pos - tTransform.pos;
			Vec2 shift(0, 0);
			// if there was a non-zero previous x overlap, then the collision came from y
			if (prevOverlap.x > 0)
			{
				shift.y += diff.y > 0 ? overlap.y : -overlap.y;
				pTransform.velocity.y = 0;
				if (diff.y < 0)
				{
					pState.state = "ground";
					pTransform.pos += (tTransform.velocity);
				}
				else
				{
					hitBlock(tile);
				}
			}
			// if there was a non-zero previous y overlap, then the collision came from y
			else if (prevOverlap.y > 0)
			{
				shift.x += diff.x > 0 ? overlap.x : -overlap.x;
				pTransform.velocity.x = 0;
				pTransform.pos += (tTransform.velocity);

			}
			pTransform.pos += shift;
		}

		// respawn if lower than bottom of screen
		if (pTransform.pos.y > height()) { spawnPlayer(); }
		// block left side of screen
		if (pTransform.pos.x < pBoundingBox.halfSize.x) { pTransform.pos.x = pBoundingBox.halfSize.x; }
	}
}

void Scene_Play::sDoAction(Action action)
{
	PROFILE_FUNCTION();

	Entity player	 = m_entityManager.getEntities(Tag::player)[0];
	auto& pInput	 = player.getComponent<CInput>();
	auto& pState	 = player.getComponent<CState>();
	auto& pTransform = player.getComponent<CTransform>();

	if (action.type() == "START")
	{
		if (action.name() == "JUMP")				  { pInput.up    = true;				  }
		else if (action.name() == "SHOOT")			  { pInput.shoot = true;				  }
		else if (action.name() == "LEFT")			  { pInput.left  = true;				  }
		else if (action.name() == "DOWN")			  { pInput.down  = true;				  }
		else if (action.name() == "RIGHT")			  { pInput.right = true;				  }
		else if (action.name() == "TOGGLE_TEXTURE")	  { m_drawTextures   = !m_drawTextures;	  }
		else if (action.name() == "TOGGLE_COLLISION") { m_drawCollisions = !m_drawCollisions; }
		else if (action.name() == "TOGGLE_GRID")	  { m_drawGrid	     = !m_drawGrid;		  }
		else if (action.name() == "PAUSE")			  { setPaused(!m_paused);				  }
		else if (action.name() == "QUIT")			  { onEnd();							  }
		else if (action.name() == "LEFT_CLICK")
		{
			// first try and find a draggable entity
			for (Entity draggable : m_entityManager.getEntities())
			{
				// release tile
				if (draggable.hasComponent<CDraggable>() && draggable.getComponent<CDraggable>().dragging)
				{
					auto mp = m_mouseShape.getPosition();
					auto& eTransform = draggable.getComponent<CTransform>();

					draggable.getComponent<CDraggable>().dragging = false;
					Vec2 p = gridToMidPixel((int)(mp.x / m_gridSize.x), (int)((height() - mp.y) / m_gridSize.y), draggable);  // for grid snapping

					eTransform.pos = p;
					eTransform.prevPos = p;

					draggable.removeComponent<CDraggable>();
					return; // we only want one
				}
			}
			// couldn't find a draggable entity. make one.
			{
				float xdiff = m_game->window().getView().getCenter().x - m_game->window().getSize().x / 2;
				float ydiff = m_game->window().getView().getCenter().y - m_game->window().getSize().y / 2;
				Vec2 worldPos(action.pos().x + xdiff, action.pos().y + ydiff);

				// check to see if any entity was clicked at this position
				for (auto e : m_entityManager.getEntities())
				{
					// if I have clicked inside this entity
					if (e.hasComponent<CDraggable>() && Physics::IsInside(worldPos, e))
					{
						e.getComponent<CDraggable>().dragging = true;
						return;
					}
				}
			}
		}
	}
	else if (action.type() == "END") 
	{
		if (action.name() == "JUMP") 
		{ 
			if (pTransform.velocity.y < 0) { pTransform.velocity.y = 0; }
			pInput.canJump = true;
			pInput.up	   = false;
		}
		else if (action.name() == "LEFT")  { pInput.left  = false;	 }
		else if (action.name() == "DOWN")  { pInput.down  = false;	 }
		else if (action.name() == "RIGHT") { pInput.right = false;   }
		else if (action.name() == "SHOOT") { pInput.shoot = false;
											 pInput.canShoot = true; }
	}

	if (action.name() == "MOUSE_MOVE")
	{
		float xdiff = m_game->window().getView().getCenter().x - m_game->window().getSize().x / 2;
		float ydiff = m_game->window().getView().getCenter().y - m_game->window().getSize().y / 2;
		m_mouseShape.setPosition(action.pos().x + xdiff, action.pos().y + ydiff);
	}
}

void Scene_Play::sAnimation()
{
	PROFILE_FUNCTION();

	Entity player = m_entityManager.getEntities(Tag::player)[0];
	auto& pState	 = player.getComponent<CState>();
	auto& pAnimation = player.getComponent<CAnimation>();

	// set player animation based on state and input
	if (pState.state == "air")
	{
		player.addComponent<CAnimation>(m_game->assets().getAnimation("Air"), true);
	}
	else if (pState.state == "ground")
	{
		auto& pInput = player.getComponent<CInput>();
		if ((pInput.left || pInput.right) && !(pInput.left && pInput.right))
		{
			if (pAnimation.animation.getName() != "Run")
			{
				player.addComponent<CAnimation>(m_game->assets().getAnimation("Run"), true);
			}
		}
		else
		{
			if (pAnimation.animation.getName() != "Stand")
			{
				player.addComponent<CAnimation>(m_game->assets().getAnimation("Stand"), true);
			}
		}
	}

	// animate all entities
	for (auto e : m_entityManager.getEntities())
	{
		if (!e.hasComponent<CAnimation>()) { continue; }

		auto& anim = e.getComponent<CAnimation>();

		if (anim.repeat || !anim.animation.hasEnded())
		{
			anim.animation.update();
		}
		else if (anim.animation.hasEnded())
		{
			e.destroy();
		}
	}
}

void Scene_Play::drawLine(const Vec2& p1, const Vec2& p2)
{
	sf::Vertex line[] = { sf::Vector2f(p1.x, p1.y), sf::Vector2f(p2.x, p2.y) };
	m_game->window().draw(line, 2, sf::Lines);
}

void Scene_Play::sRender()
{
	PROFILE_FUNCTION();

	Entity player = m_entityManager.getEntities(Tag::player)[0];

	// color the background darker so you know that the game is paused
	if (!m_paused) { m_game->window().clear(sf::Color(100, 100, 255)); }
	else { m_game->window().clear(sf::Color(50, 50, 150)); }

	{
		PROFILE_SCOPE("Camera View");
		// set the viewport of the window to be centered on the player if it's far enough right
		auto& pPos = player.getComponent<CTransform>().pos;
		float windowCenterX = std::max(m_game->window().getSize().x / 2.0f, pPos.x);
		sf::View view = m_game->window().getView();
		view.setCenter(windowCenterX, m_game->window().getSize().y - view.getCenter().y);
		m_game->window().setView(view);
	}

	// draw all Entity textures / animations
	if (m_drawTextures)
	{
		PROFILE_SCOPE("Draw Textures");

		for (auto e : m_entityManager.getEntities())
		{
			if (e.hasComponent<CAnimation>()) {
				auto& transform = e.getComponent<CTransform>();
				auto& animation = e.getComponent<CAnimation>().animation;

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
		PROFILE_SCOPE("Draw Grid");

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
		PROFILE_SCOPE("Draw Collisions");

		for (auto e : m_entityManager.getEntities())
		{
			if (e.hasComponent<CBoundingBox>())
			{
				auto& box		= e.getComponent<CBoundingBox>();
				auto& transform = e.getComponent<CTransform>();

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
	m_game->window().draw(m_mouseShape);
}

void Scene_Play::onEnd()
{
	m_hasEnded = true;
	m_game->changeScene("MENU", nullptr, true);
}