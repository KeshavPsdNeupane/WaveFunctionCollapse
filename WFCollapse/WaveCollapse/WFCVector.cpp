#include "WFCVector.h"
#include <iostream>
#include <algorithm>
#include <random>
#include"../Utility.h"

WFCVector::WFCVector(sf::Vector2i gridSize)
	: gridCount(gridSize), waveOperation(),startingPoint(0) {

	srand((unsigned int)(time(0)));
	grid.resize(gridCount.x, std::vector<GridCell>(gridCount.y));
	waveOperation.resize(gridCount.x * gridCount.y);
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

			waveOperation[x + y * gridCount.x] = &grid[x][y];
			this->gridRect[x][y] = rect;
		}
	}
}



void WFCVector::Init() {
	startingPoint = rand() % (gridCount.x * gridCount.y);
	std::cout << "init 2 = " << this->startingPoint << "\n";
	std::swap(this->waveOperation[startingPoint], this->waveOperation[0]);
}



void WFCVector::WaveOperation() {
	if (gridCount.x == 0 || gridCount.y == 0) return;
	if (!waveOperation.empty()) {
		SortWaveOperation();
		GridCell* currentCell = waveOperation[0];
		if (!currentCell->IsGraphCollapsed()) {
			CollapseTheCurrentGrid(*currentCell);
			PropagateTheWave(*currentCell);
		}
	}
}


void WFCVector::CollapseTheCurrentGrid(GridCell& cell) {
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
	DeleteUnwantedGraphs(cell);
}



void WFCVector::PropagateTheWave(GridCell& cell) {
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
			}
		}
	}
}


void WFCVector::SetColor(GridCell& cell) {
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

void WFCVector::DeleteUnwantedGraphs(GridCell& cell) {
	auto location = cell.GetGridLocation();
	this->waveOperation.erase(std::find_if(waveOperation.begin(), waveOperation.end(), [&](GridCell* cell) {
		return cell->GetGridLocation() == location;
		}));
}

void WFCVector::SortWaveOperation() {
	const auto compareEntropy = [](const GridCell* a, const GridCell* b) {
		return a->GetEntropy() < b->GetEntropy();
		};
	std::stable_sort(this->waveOperation.begin(), this->waveOperation.end(), compareEntropy);
}





void WFCVector::ShowOnConsole() {
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

void WFCVector::Draw(sf::RenderWindow& window) {
	for (int x = 0; x < gridCount.x; ++x) {
		for (int y = 0; y < gridCount.y; ++y) {
			window.draw(this->gridRect[x][y]);
		}
	}
}
