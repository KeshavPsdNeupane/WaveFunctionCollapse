#pragma once
#include <vector>
#include <string>
#include <SFML/System/Vector2.hpp>

// TileType Enum
enum TileType {
    Empty,
    Plain,
    Sand,
    Water,
    Forest,
    MaxSize = Forest,
};

// Tile Structure
struct Tile {
    TileType type;
    std::string graph;
    Tile() : type(TileType::Empty), graph("Empty") {}
    Tile(TileType type, const std::string& graph) : type(type), graph(graph) {}
};

// TileSet Class
struct TileSet {
    std::vector<Tile> tiles;
    std::vector<std::vector<TileType>> possibleAdjacentTile;
    TileSet() {
        tiles.resize(TileType::MaxSize + 1);
		possibleAdjacentTile.resize(TileType::MaxSize + 1);

        tiles[Empty] = Tile(Empty, GetTypeName(Empty));
        tiles[Plain] = Tile(Plain, GetTypeName(Plain));
        tiles[Sand] = Tile(Sand, GetTypeName(Sand));
        tiles[Water] = Tile(Water, GetTypeName(Water));
        tiles[Forest] = Tile(Forest, GetTypeName(Forest));

        possibleAdjacentTile[Empty] = { Plain, Sand, Water, Forest };
        possibleAdjacentTile[Plain] = { Plain, Sand, Forest };
        possibleAdjacentTile[Sand] = { Sand, Plain, Water };
        possibleAdjacentTile[Water] = { Water, Sand };
        possibleAdjacentTile[Forest] = { Forest, Plain };
    }

    std::string GetTypeName(TileType type) const {
        switch (type) {
        case Empty: return "Empty";
        case Plain: return "Plain";
        case Sand: return "Sand";
        case Water: return "Water";
        case Forest: return "Forest";
        default: return "Unknown";
        }
    }
};

// GridCell Class
class GridCell {
private:
    sf::Vector2i gridPosition;
    TileSet tileSet;
    TileType type;
    std::string graph;
    std::vector<TileType> possibleTileForThisCell;
    bool isGraphCollapsed;

public:
    GridCell() : tileSet(), type(Empty), isGraphCollapsed(false) {}

    GridCell(sf::Vector2i gridPos)
        : gridPosition(gridPos), tileSet(), isGraphCollapsed(false) {
        type = Empty;
        graph = tileSet.tiles[type].graph;
        possibleTileForThisCell = tileSet.possibleAdjacentTile[type];
    }

    bool IsGraphCollapsed() const { return isGraphCollapsed; }
    sf::Vector2i GetGridLocation() const { return gridPosition; }
    TileType GetType() const { return type; }
    std::vector<TileType>& GetPossibleTileForThisCell() { return possibleTileForThisCell; }
    unsigned int GetEntropy() const { return possibleTileForThisCell.size(); }


    void CollapseGraph(TileType type) {
        if (type >= Empty && type <= MaxSize) {
            this->type = type;
            this->graph = tileSet.tiles[this->type].graph;
            this->isGraphCollapsed = true;
        }
    }

    void PropagateGraphCollapse(TileType type) {
        if (type >= Empty && type <= MaxSize) {
            this->possibleTileForThisCell = PropagateTheGraphCollapse(type);
        }
    }

private:
    std::vector<TileType> PropagateTheGraphCollapse(TileType type) {
        std::vector<TileType> incomingTileVector = tileSet.possibleAdjacentTile[type];
        std::vector<TileType> newPossibleTiles;

        for (TileType homeTile : possibleTileForThisCell) {
            for (TileType incomingTile : incomingTileVector) {
                if (homeTile == incomingTile) {
                    newPossibleTiles.push_back(homeTile);
                }
            }
        }

        return newPossibleTiles.empty() ? possibleTileForThisCell : newPossibleTiles;
    }
};
