#pragma once
#include "WaveRes.h"
#include <vector>
#include <stack>
#include<SFML/Graphics.hpp>

class WFCVector {
private:
    sf::Vector2i gridCount;
    TileSet tileSet;
    std::vector<std::vector<GridCell>> grid;
    unsigned int startingPoint;
    std::vector<GridCell*> waveOperation;
	std::vector<std::vector<sf::RectangleShape>> gridRect;


public:
    WFCVector(sf::Vector2i gridCount = sf::Vector2i(10, 10));
public:
    void Init();
    void WaveOperation();
    void ShowOnConsole();
	void Draw(sf::RenderWindow& window);
private:
    void CollapseTheCurrentGrid(GridCell& cell);
    void PropagateTheWave(GridCell& cell);
	void SetColor(GridCell& cell);
	void DeleteUnwantedGraphs(GridCell& cell);
	void SortWaveOperation();
};
