/* This project will be a basic implementation of an Game Engine using an ECS system and RAII
 *
 * Author: Kristian Vines
 * Contact: kvines4@outlook.com
 */

#include <SFML/Graphics.hpp>

#include "GameEngine.h"

int main()
{
	GameEngine g("assets.txt");
	g.run();
}