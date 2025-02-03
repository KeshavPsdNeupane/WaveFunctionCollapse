#pragma once
#include "WaveRes.h"
#include <queue>
#include <vector>
#include <SFML/Graphics.hpp>
#include <random>

class WFCPriorityQueue {
private:
    sf::Vector2i gridCount;
    TileSet tileSet;
    std::vector<std::vector<GridCell>> grid;
    std::vector<std::vector<sf::RectangleShape>> gridRect;
    std::mt19937 mtRand;

    // Comparator for priority queue (lower entropy first)
    struct EntropyComparator {
        bool operator()(const GridCell* a, const GridCell* b) const;
    };

    std::priority_queue<GridCell*,
        std::vector<GridCell*>,
        EntropyComparator> waveQueue;

private:
    void InitializeGrid();
    void CreateVisuals();
    void CollapseCell(GridCell& cell);
    unsigned GetWeightedRandomIndex(GridCell& cell);
    void Propagate(GridCell& cell);
    bool IsValidPosition(const sf::Vector2i& pos) const;
    void UpdateCellColor(GridCell& cell);

public:
    WFCPriorityQueue(sf::Vector2i gridSize = Utility::GRID_COUNT);
    void Init();
    void WaveOperation();
    void Draw(sf::RenderWindow& window);
};