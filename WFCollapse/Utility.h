#pragma once
#include<SFML/Graphics.hpp>
namespace Utility {
	 const bool FIX_FRAME = false;
	 const float MAX_FRAME = 240;
	 const sf::Vector2i WINDOW_SIZE = sf::Vector2i(800, 600);
	 const sf::Vector2i GRID_COUNT = sf::Vector2i(60, 60);
	 const sf::Vector2i GRID_SIZE = sf::Vector2i(WINDOW_SIZE.x / GRID_COUNT.x, WINDOW_SIZE.y / GRID_COUNT.y);
	 const bool IS_PADDING = false;

	 const bool USE_STACK = false;
	 const bool USE_WEIGHT = true;
	 const unsigned int SEED = 2;
	 const float INFLUENCE = 0.05f;
}