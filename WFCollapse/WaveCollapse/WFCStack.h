#pragma once
#include "WaveRes.h"
#include <vector>
#include <stack>
#include<SFML/Graphics.hpp>
#include<random>

class WFCStack {
private:
    sf::Vector2i gridCount;
    std::vector<std::vector<GridCell>> grid;
    std::stack<GridCell*> waveOperation;
    std::vector<std::vector<sf::RectangleShape>> gridRect;

	// for deterministic random number generation with seed
	std::mt19937 mtRand;
public:
    WFCStack(sf::Vector2i gridCount = sf::Vector2i(10, 10));
public:
    void Init();
    void WaveOperation();
    void Draw(sf::RenderWindow& window);

private:
    void CollapseTheCurrentGrid(GridCell& cell);
    void PropagateTheWave(GridCell& cell);
	unsigned int GetRandomIndexBasedOnWeight(GridCell& cell);
    void SetColor(GridCell& cell);
 
    void SortWaveOperation();
};
