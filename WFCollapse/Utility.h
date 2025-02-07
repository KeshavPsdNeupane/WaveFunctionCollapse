#pragma once
#include<SFML/Graphics.hpp>
namespace Utility {
	 const sf::Vector2i WINDOW_SIZE = sf::Vector2i(800, 600);

	 const int size = 200;

	 const sf::Vector2i GRID_COUNT = sf::Vector2i(size, size);
	 const sf::Vector2i GRID_SIZE = sf::Vector2i(WINDOW_SIZE.x / GRID_COUNT.x, WINDOW_SIZE.y / GRID_COUNT.y);
	 //IF TRUE, THE GRID WILL HAVE WHITE OUTLINE
	 const bool IS_PADDING = false;
	 
	 //THIS SEED DETERMINE THE SHAPE OF MAP
	 const bool DO_AT_ONCE = true;	//IF TRUE, THE WAVE COLLAPSE WILL BE DONE AT ONCE

	 const unsigned int SEED = 2000;
	 const bool USE_WEIGHT = true;
	 const bool USE_STACK = false;
	 const bool CAN_SORT = true;
	 const float INFLUENCE = 0.05f;
}