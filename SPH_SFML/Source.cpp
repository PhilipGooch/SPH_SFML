#pragma once
#include "Application.h"

using namespace std;
using namespace sf;

int main()
{
	RenderWindow window(VideoMode(W, H), "SPH");

	Application application(&window);

	application.run();

	return 0;
}