/* This project will be a basic implementation of an Game Engine using an ECS system and RAII
 * 
 * Author: Kristian Vines
 * Contact: kvines4@outlook.com
 */

#include <SFML/Graphics.hpp>
#include <iostream>
#include <fstream>
#include <strstream>
#include <vector>

class Entity
{
public:
	std::shared_ptr<sf::Vector2f> speed;
	std::shared_ptr<sf::Shape>    shape;
	std::shared_ptr<sf::Text>     name;

	Entity(std::shared_ptr<sf::Text>        n,
		   std::shared_ptr<sf::CircleShape> c,
		   std::shared_ptr<sf::Vector2f>    s)
		: speed(s)
		, shape(c)
		, name (n)
	{
	}

	Entity(std::shared_ptr<sf::Text>           n,
		   std::shared_ptr<sf::RectangleShape> c,
		   std::shared_ptr<sf::Vector2f>       s)
		: speed(s)
		, shape(c)
		, name (n)
	{
	}
};

///// globals /////

const std::string configFilePath = "config.txt";

// screen dimensions
int wWidth  = 640;
int wHeight = 480;

// program font
sf::Font  wFont;
sf::Color wFontColor;
int       wFontSize;

// shapes container
std::vector<Entity> shapes;

bool loadConfig(const std::string & sFilename)
{
	std::ifstream fin(sFilename);
	if (!fin.is_open())
		return false;

	while (!fin.eof())
	{
		// temp variables
		std::string option;
		std::string n;
		int r, g, b;
		int x, y, w, h, rad;
		float sx, sy;

		fin >> option;

		if (option == "Window")
		{
			fin >> wWidth >> wHeight;
		}
		else if (option == "Font")
		{
			std::string wFontFile;

			fin >> wFontFile >> wFontSize >> r >> g >> b;
			wFontColor = sf::Color(r, g, b);

			// attempt to load the font from a file
			if (!wFont.loadFromFile(wFontFile))
			{
				// if we can't load the font, print an error to the error console and and exit
				std::cerr << "Could not load font!" << std::endl;
				exit(-1);
			}
		}
		else if (option == "Rectangle")
		{
			fin >> n >> x >> y >> sx >> sy >> r >> g >> b >> w >> h;			// temp params
			auto na = std::make_shared<sf::Text>(n, wFont, wFontSize);			// name text
			auto sp = std::make_shared<sf::Vector2f>(sx, sy);					// speed
			auto rs = std::make_shared<sf::RectangleShape>(sf::Vector2f(w, h));	// rectangle
			rs->setFillColor(sf::Color(r, g, b));
			rs->setPosition(x, y);
			Entity shape(na, rs, sp);
			shapes.push_back(shape);
		}
		else if (option == "Circle")
		{
			fin >> n >> x >> y >> sx >> sy >> r >> g >> b >> rad;				// temp params
			auto na = std::make_shared<sf::Text>(n, wFont, wFontSize);			// name text
			auto sp = std::make_shared<sf::Vector2f>(sx, sy);					// speed
			auto cs = std::make_shared<sf::CircleShape>(rad);					// circle
			cs->setFillColor(sf::Color(r, g, b));
			cs->setPosition(x, y);
			Entity shape(na, cs, sp);
			shapes.push_back(shape);
		}
	}
}

int main(int argv, char* argc[])
{
	loadConfig(configFilePath);

	// top-left of the window is (0,0) and bottom right is (w,h) 
	sf::RenderWindow window(sf::VideoMode(wWidth, wHeight), "SFML works!");

	// main loop - continues for each frame while window is open
	while (window.isOpen())
	{
		// event handling
		sf::Event event;
		while (window.pollEvent(event))
		{
			// this event triggers when the window is closed
			if (event.type == sf::Event::Closed)
			{
				window.close();
			}

			// this event is triggered when a key is pressed
			if (event.type == sf::Event::KeyPressed)
			{
				// print the key that was pressed to the console
				std::cout << "Key pressed with code = " << event.key.code << std::endl;
			}
		}

		window.clear();
		for (Entity e : shapes) 
		{
			sf::Vector2f cPos = e.shape->getPosition();
			sf::FloatRect c   = e.shape->getLocalBounds();

			// collision check
			if (cPos.x < 0 || cPos.x + c.width > wWidth)
				e.speed->x *= -1;
			if (cPos.y < 0 || cPos.y + c.height > wHeight)
				e.speed->y *= -1;

			cPos += *e.speed;
			sf::FloatRect t = e.name->getLocalBounds();
			sf::Vector2f tPos(cPos.x + (c.width  / 2) - (t.width  / 2),
				              cPos.y + (c.height / 2) - (t.height / 2));

			e.shape->setPosition(cPos);
			e.name->setPosition(tPos);

			window.draw(*e.shape);
			window.draw(*e.name);
		}
		window.display();
	}

	return 0;
}