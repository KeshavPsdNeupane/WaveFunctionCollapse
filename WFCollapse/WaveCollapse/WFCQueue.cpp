#include "WFCQueue.h"

bool WFCPriorityQueue::EntropyComparator::operator()(const GridCell* a, const GridCell* b) const {
    if (a->GetEntropy() != b->GetEntropy()) {
        return a->GetEntropy() > b->GetEntropy();  // Lower entropy should come first
    }
    return(a->GetGridLocation().y > b->GetGridLocation().y) &&
        (a->GetGridLocation().x > b->GetGridLocation().x);
}

WFCPriorityQueue::WFCPriorityQueue(sf::Vector2i gridSize)
    : gridCount(gridSize), mtRand(Utility::SEED) {
    InitializeGrid();
    CreateVisuals();
}

void WFCPriorityQueue::InitializeGrid() {
    grid.resize(gridCount.x, std::vector<GridCell>(gridCount.y));
    for (int x = 0; x < gridCount.x; ++x) {
        for (int y = 0; y < gridCount.y; ++y) {
            grid[x][y] = GridCell(sf::Vector2i(x, y));
        }
    }
}

void WFCPriorityQueue::CreateVisuals() {
    sf::Vector2f cellSize(
        Utility::WINDOW_SIZE.x / static_cast<float>(gridCount.x),
        Utility::WINDOW_SIZE.y / static_cast<float>(gridCount.y)
    );
    gridRect.resize(gridCount.x, std::vector<sf::RectangleShape>(gridCount.y));

    for (int x = 0; x < gridCount.x; ++x) {
        for (int y = 0; y < gridCount.y; ++y) {
            sf::RectangleShape rect;
            rect.setSize(cellSize);
            rect.setPosition(x * cellSize.x, y * cellSize.y);
            rect.setFillColor(sf::Color::White);

            if (Utility::IS_PADDING) {
                rect.setOutlineThickness(1);
                rect.setOutlineColor(sf::Color::Black);
            }

            gridRect[x][y] = rect;
        }
    }
}


void WFCPriorityQueue::Init() {
    int totalCells = gridCount.x * gridCount.y;
    std::uniform_int_distribution<int> dist(0, totalCells - 1);
    int startingIndex = dist(mtRand);

    int startX = startingIndex % gridCount.x;
    int startY = startingIndex / gridCount.x;
    startY = std::min(startY, gridCount.y - 1);

    waveQueue.push(&grid[startX][startY]);
}

void WFCPriorityQueue::WaveOperation() {
    if (!waveQueue.empty()) {
        GridCell* currentCell = waveQueue.top();
        waveQueue.pop();
        if (!currentCell->IsGraphCollapsed()) {
            CollapseCell(*currentCell);
            Propagate(*currentCell);
        }
    }
}

void WFCPriorityQueue::CollapseCell(GridCell& cell) {
    auto& possibilities = cell.GetPossibleTileForThisCell();
    if (possibilities.empty()) return;

    unsigned index = GetWeightedRandomIndex(cell);
    TileType selected = possibilities[index];
    cell.CollapseGraph(selected);
    UpdateCellColor(cell);
}

unsigned WFCPriorityQueue::GetWeightedRandomIndex(GridCell& cell) {
    auto& weights = cell.GetWeight();
    unsigned total = 0;
    for (auto w : weights) total += w;

    std::uniform_int_distribution<unsigned> dist(0, total - 1);
    unsigned random = dist(mtRand);

    unsigned cumulative = 0;
    for (unsigned i = 0; i < weights.size(); ++i) {
        cumulative += weights[i];
        if (random < cumulative) return i;
    }
    return 0;
}

void WFCPriorityQueue::Propagate(GridCell& cell) {
    const sf::Vector2i pos = cell.GetGridLocation();
    const TileType type = cell.GetType();

    const std::vector<sf::Vector2i> neighbors = {
        {pos.x, pos.y - 1}, 
        {pos.x + 1, pos.y},  
        {pos.x, pos.y + 1},  
        {pos.x - 1, pos.y} 
    };

    for (auto& neighborPos : neighbors) {
        if (IsValidPosition(neighborPos)) {
            GridCell& neighbor = grid[neighborPos.x][neighborPos.y];
            if (!neighbor.IsGraphCollapsed()) {
                neighbor.PropagateGraphCollapse(type);
                waveQueue.push(&neighbor);
            }
        }
    }
}

bool WFCPriorityQueue::IsValidPosition(const sf::Vector2i& pos) const {
    return pos.x >= 0 && pos.x < gridCount.x &&
        pos.y >= 0 && pos.y < gridCount.y;
}

void WFCPriorityQueue::UpdateCellColor(GridCell& cell) {
    const sf::Vector2i pos = cell.GetGridLocation();
    sf::RectangleShape& rect = gridRect[pos.x][pos.y];
    switch (cell.GetType()) {
    case TileType::Empty:    rect.setFillColor(sf::Color::White); break;
    case TileType::Plain:    rect.setFillColor(sf::Color::Green); break;
    case TileType::Sand:     rect.setFillColor(sf::Color::Yellow); break;
    case TileType::Water:    rect.setFillColor(sf::Color::Blue); break;
    case TileType::Forest:   rect.setFillColor(sf::Color(0, 100, 0)); break;
    case TileType::Mountain: rect.setFillColor(sf::Color(85, 85, 85)); break;
    default: rect.setFillColor(sf::Color::White); break;
    }
}

// Draw the grid to the window
void WFCPriorityQueue::Draw(sf::RenderWindow& window) {
    for (auto& column : gridRect) {
        for (auto& rect : column) {
            window.draw(rect);
        }
    }
}