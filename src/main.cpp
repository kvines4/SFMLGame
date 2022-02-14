/* This project will be a basic implementation of an Game Engine using an ECS system and RAII
 * 
 * Author: Kristian Vines
 * Contact: kvines4@outlook.com
 */

#include <SFML/Graphics.hpp>
#include <iostream>
#include <fstream>

int main(int argv, char* argc[])
{
	// create a new window of size 640 by 480 pixels
	// top-left of the window is (0,0) and bottom right is (w,h)
	const int wWidth = 640;
	const int wHeight = 480;

	sf::RenderWindow window(sf::VideoMode(wWidth, wHeight), "SFML works!");

	// lets make a shape that we will draw to the screen
	sf::CircleShape circle(50);				// create a circle shape with radius 50
	circle.setFillColor(sf::Color::Green);	// set the circle color to green
	circle.setPosition(300.0f, 300.0f);		// set the top-left position of the circle
	float circleMoveSpeed = -0.01f;			// we will use this to move the circle later

	// lets load a font so we can display some text
	sf::Font myFont;

	// attempt to load the font from a file
	if (!myFont.loadFromFile("fonts/arial.ttf"))
	{
		// if we can't load the font, print an error to the error console and and exit
		std::cerr << "Could not load font!" << std::endl;
		exit(-1);
	}

	// set up the text object that will be drawn to the screen
	sf::Text text("Sample Text", myFont, 24);

	// position the top-left corner of the text so that the text aligns on the bottom
	// text character size is in pixels, so move the text up from the bottom by its height
	text.setPosition(0, wHeight - (float)text.getCharacterSize());

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

				// example, what happens when x is pressed
				if (event.key.code == sf::Keyboard::X)
				{
					// reverse the direction of the circle on the screen
					circleMoveSpeed *= -1.0f;
				}
			}
		}

		// basic animation - move the circle each frame if it's still in the frame
		circle.setPosition(circle.getPosition() + sf::Vector2f(circleMoveSpeed, circleMoveSpeed));

		// basic rendering function calls
		window.clear();			// clear the window of anything previously drawn
		window.draw(circle);	// draw the circle
		window.draw(text);		// draw the text
		window.display();		// call the window display function
	}

	return 0;
}