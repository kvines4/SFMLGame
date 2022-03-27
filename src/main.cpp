/* This project will be a basic implementation of an Game Engine using an ECS system and RAII
 *
 * Author: Kristian Vines
 * Contact: kvines4@outlook.com
 */

#include "GameEngine.h"
#include "Profiler.h"

int main()
{
	PROFILE_FUNCTION();
	GameEngine g("assets.txt");
	g.run();
}