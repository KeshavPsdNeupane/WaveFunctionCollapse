#include "WFCStack.h"
#include <iostream>
#include <algorithm>
#include <random>
#include"../Utility.h"
WFCStack::WFCStack(sf::Vector2i gridSize)
	: gridCount(gridSize), startingPoint(0) {
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
			rect.setFillColor(sf::Color::White);
			rect.setSize(size);
			if (Utility::IS_PADDING) {
				rect.setOutlineThickness(1);
				rect.setOutlineColor(sf::Color::Black);
			}
			this->gridRect[x][y] = rect;

		}
	}
}

void WFCStack::Init() {
	startingPoint = rand() % (gridCount.x * gridCount.y / 4);
	sf::Vector2i tempStart(0, (int)(gridCount.y * .2f));

	int startX = tempStart.x + startingPoint % gridCount.x;
	int startY = tempStart.y + startingPoint / gridCount.x;

	if (startX >= gridCount.x) startX = gridCount.x - 1;
	if (startY >= gridCount.y) startY = gridCount.y - 1;
	std::cout << "start = " << startX << " " << startY << "\n";
	this->waveOperation.push(&grid[startX][startY]);
}



void WFCStack::WaveOperation() {
	if (gridCount.x == 0 || gridCount.y == 0) return;
	if (!waveOperation.empty()) {
		SortWaveOperation();
		GridCell* currentCell = waveOperation.top();
		waveOperation.pop();
		if (!currentCell->IsGraphCollapsed()) {
			CollapseTheCurrentGrid(*currentCell);
			PropagateTheWave(*currentCell);
		}
	}
}


void WFCStack::CollapseTheCurrentGrid(GridCell& cell) {
	auto& possibleTiles = cell.GetPossibleTileForThisCell();
	if (possibleTiles.empty()) return;
	TileType chosenType;
	if (possibleTiles.size() == 1) {
		chosenType = possibleTiles[0];
	}
	else {
		int randomIndex = rand() % possibleTiles.size();
		chosenType = possibleTiles[randomIndex];
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

void WFCStack::SetColor(GridCell& cell) {
	sf::Vector2i location = cell.GetGridLocation();
	TileType colorid = cell.GetType();
	sf::RectangleShape& rect = this->gridRect[location.x][location.y];

	switch (colorid) {
	case TileType::Empty:
		rect.setFillColor(sf::Color::White);
		break;
	case TileType::Plain:
		rect.setFillColor(sf::Color::Green);
		break;
	case TileType::Sand:
		rect.setFillColor(sf::Color::Yellow);
		break;
	case TileType::Water:
		rect.setFillColor(sf::Color::Blue);
		break;
	case TileType::Forest:
		rect.setFillColor(sf::Color(0, 100, 0));
		break;
	default:
		rect.setFillColor(sf::Color::White);
		break;
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
		});

	for (auto it = tempVector.rbegin(); it != tempVector.rend(); ++it) {
		waveOperation.push(*it);
	}
}
















void WFCStack::ShowOnConsole() {
	if (!waveOperation.empty()) {
		for (int y = 0; y < gridCount.y; ++y) {
			for (int x = 0; x < gridCount.x; ++x) {
				std::cout << tileSet.GetTypeName(grid[x][y].GetType()) << " ";
			}
			std::cout << "\n";
		}
		std::cout << "\n\n\n";
	}
}

void WFCStack::Draw(sf::RenderWindow& window) {
	for (int x = 0; x < gridCount.x; ++x) {
		for (int y = 0; y < gridCount.y; ++y) {
			window.draw(this->gridRect[x][y]);
		}
	}
}

