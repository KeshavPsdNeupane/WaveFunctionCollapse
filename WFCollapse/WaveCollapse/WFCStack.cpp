#include "WFCStack.h"
#include <iostream>
#include <algorithm>
#include <random>
#include"../Utility.h"
WFCStack::WFCStack(sf::Vector2i gridSize)
	: gridCount(gridSize),mtRand(Utility::SEED) {
	srand((unsigned int)(time(0)));

	grid.resize(gridCount.x, std::vector<GridCell>(gridCount.y));
	gridRect.resize(gridCount.x, std::vector<sf::RectangleShape>(gridCount.y));

	sf::Vector2f size((float)(Utility::WINDOW_SIZE.x / gridSize.x),
		(float)(Utility::WINDOW_SIZE.y / gridSize.y));

	for (int x = 0; x < gridCount.x; ++x) {
		for (int y = 0; y < gridCount.y; ++y) {
			grid[x][y] = GridCell(sf::Vector2i(x, y));
			sf::RectangleShape rect;
			rect.setPosition(sf::Vector2f(size.x * x, size.y * y));
			rect.setSize(size);
			rect.setFillColor(sf::Color::Black);
			if (Utility::IS_PADDING) {
				rect.setOutlineThickness(1);
				rect.setOutlineColor(sf::Color::White);
			}
			this->gridRect[x][y] = rect;

		}
	}
}

void WFCStack::Init() {
	int totalCells = gridCount.x * gridCount.y;
	int startingPoint = std::uniform_int_distribution<int>(0, totalCells - 1)(mtRand);
	int startX = startingPoint % gridCount.x;
	int startY = startingPoint / gridCount.x;
	startX = std::min(startX, gridCount.x - 1);
	startY = std::min(startY, gridCount.y - 1);
	this->waveOperation.push(&grid[startX][startY]);
}


void WFCStack::WaveOperation() {
	if (gridCount.x == 0 || gridCount.y == 0) return;

	if (Utility::DO_AT_ONCE) {
		while (!waveOperation.empty()) {
			GridCell* currentCell = waveOperation.top();
			waveOperation.pop();
			if (!currentCell->IsGraphCollapsed()) {
				CollapseTheCurrentGrid(*currentCell);
				PropagateTheWave(*currentCell);
			}
		}
		return;

	}
	else {
		if (!waveOperation.empty()) {
			if (Utility::CAN_SORT) {
				SortWaveOperation();
			}
			GridCell* currentCell = waveOperation.top();
			waveOperation.pop();
			if (!currentCell->IsGraphCollapsed()) {
				CollapseTheCurrentGrid(*currentCell);
				PropagateTheWave(*currentCell);
			}
		}
	}
}

void WFCStack::CollapseTheCurrentGrid(GridCell& cell){
	auto& possibleTiles = cell.GetPossibleTileForThisCell();
	if (possibleTiles.empty()) return;
	TileType chosenType;
	if (possibleTiles.size() == 1) {
		chosenType = possibleTiles[0];
	}
	else {
		chosenType = possibleTiles[GetRandomIndexBasedOnWeight(cell)];
	}
	cell.CollapseGraph(chosenType);
	SetColor(cell);
}



void WFCStack::PropagateTheWave(GridCell& cell) {
	sf::Vector2i location = cell.GetGridLocation();
	TileType collapsedType = cell.GetType();
	std::vector<sf::Vector2i> neighbors = {
	{ location.x + 1, location.y },  // Right
	{ location.x, location.y - 1 },  // Up
	{ location.x - 1, location.y }, // Left
	{ location.x, location.y + 1 } // Down (Bottom)
	};
	for (sf::Vector2i& neighborPos : neighbors) {
		if (neighborPos.x >= 0 && neighborPos.x < gridCount.x &&
			neighborPos.y >= 0 && neighborPos.y < gridCount.y) {
			GridCell& neighborCell = grid[neighborPos.x][neighborPos.y];
			if (!neighborCell.IsGraphCollapsed()) {
				neighborCell.PropagateGraphCollapse(collapsedType);
				waveOperation.push(&neighborCell);
			}
		}
	}
}

unsigned int WFCStack::GetRandomIndexBasedOnWeight(GridCell& cell){
	auto& weight = cell.GetWeight();
	unsigned int totalWeight = 0;
	for (unsigned int w : weight) {
		totalWeight += w;
	}
	unsigned int randomWeight = std::uniform_int_distribution<int>(0, (int)(totalWeight - 1))(mtRand);
	unsigned int sum = 0;
	for (unsigned int i = 0; i < weight.size(); ++i) {
		sum += weight[i];
		if (randomWeight < sum) {
			return i;
		}
	}
	return 0;
}

void WFCStack::SetColor(GridCell& cell) {
	 const sf::Vector2i pos = cell.GetGridLocation();
    sf::RectangleShape& rect = gridRect[pos.x][pos.y];
    switch (cell.GetType()) {
	case TileType::Empty:    rect.setFillColor(sf::Color::White); break;
    case TileType::Plain:    rect.setFillColor(sf::Color::Green); break;
    case TileType::Sand:     rect.setFillColor(sf::Color::Yellow); break;
    case TileType::Water:    rect.setFillColor(sf::Color::Blue); break;
    case TileType::Forest:   rect.setFillColor(sf::Color(0, 100, 0)); break;
    case TileType::Mountain: rect.setFillColor(sf::Color(85, 85, 85)); break;
    default:       rect.setFillColor(sf::Color::White); break;
    }
}

void WFCStack::SortWaveOperation() {
	std::vector<GridCell*> tempVector;
	while (!waveOperation.empty()) {
		tempVector.push_back(waveOperation.top());
		waveOperation.pop();
	}

	// Sort based on entropy in ascending order, and by grid position (x and y) if entropies are equal
	std::stable_sort(tempVector.begin(), tempVector.end(),
		[](const GridCell* a, const GridCell* b) {
			if (a->GetEntropy() == b->GetEntropy()) {
				return (a->GetGridLocation().y > b->GetGridLocation().y) &&
					(a->GetGridLocation().x < b->GetGridLocation().x);
			}
			return a->GetEntropy() < b->GetEntropy();
		});;
	for (auto it = tempVector.rbegin(); it != tempVector.rend(); ++it) {
		waveOperation.push(*it);
	}
}


void WFCStack::Draw(sf::RenderWindow& window) {
	for (int x = 0; x < gridCount.x; ++x) {
		for (int y = 0; y < gridCount.y; ++y) {
			window.draw(this->gridRect[x][y]);
		}
	}
}

