#pragma once
#include <vector>
#include <string>
#include <SFML/System/Vector2.hpp>
#include<utility>
#include<iostream>
#include"../Utility.h"
#include<random>
#include<unordered_map>
// TileType Enum
enum TileType {
    Empty,
    Plain,
    Sand,
    Water,
    Forest,
    Mountain,
    MaxSize,
};
typedef std::pair<std::vector<TileType>, std::vector<unsigned int>> TileTypeAndWeight;



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
    std::vector<std::vector<unsigned int>> weight;
    TileSet() {
        tiles.resize(TileType::MaxSize);
		possibleAdjacentTile.resize(TileType::MaxSize);
		weight.resize(TileType::MaxSize);


        tiles[Empty] = Tile(Empty, GetTypeName(Empty));
        tiles[Plain] = Tile(Plain, GetTypeName(Plain));
        tiles[Sand] = Tile(Sand, GetTypeName(Sand));
        tiles[Water] = Tile(Water, GetTypeName(Water));
        tiles[Forest] = Tile(Forest, GetTypeName(Forest));
		tiles[Mountain] = Tile(Mountain, GetTypeName(Mountain));

        if (Utility::USE_WEIGHT) {
            possibleAdjacentTile[Empty] = { Plain, Sand, Water, Forest, Mountain };
            weight[Empty] = { 1, 1, 1, 1, 1 };  
            possibleAdjacentTile[Plain] = { Plain, Sand, Forest };
            weight[Plain] = { 110, 96, 94 };  
            possibleAdjacentTile[Sand] = { Sand, Plain, Water };
            weight[Sand] = { 95, 101, 97 }; 
            possibleAdjacentTile[Water] = { Water, Sand };
            weight[Water] = { 98, 95 }; 
            possibleAdjacentTile[Forest] = { Forest, Plain, Mountain };
            weight[Forest] = { 97, 100, 94 }; 
            possibleAdjacentTile[Mountain] = { Mountain, Forest };
            weight[Mountain] = { 98, 96 };
        }
        else {
            possibleAdjacentTile[Empty] = { Plain, Sand, Water, Forest, Mountain };
            weight[Empty] = { 1, 1,1,1, 1 };
            possibleAdjacentTile[Plain] = { Plain, Sand, Forest };
            weight[Plain] = { 1, 1, 1 };
            possibleAdjacentTile[Sand] = { Sand, Plain, Water };
            weight[Sand] = { 1, 1, 1 };
            possibleAdjacentTile[Water] = { Water, Sand };
            weight[Water] = { 1, 1};
            possibleAdjacentTile[Forest] = { Forest, Plain, Mountain };
            weight[Forest] = { 1, 1, 1 };
            possibleAdjacentTile[Mountain] = { Mountain, Forest };
            weight[Mountain] = { 1, 1};

        }

    }

    std::string GetTypeName(TileType type) const {
        switch (type) {
        case Empty: return "Empty";
        case Plain: return "Plain";
        case Sand: return "Sand";
        case Water: return "Water";
        case Forest: return "Forest";
		case Mountain: return "Mountain";
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
    std::vector<unsigned int> weight;
    bool isGraphCollapsed;

public:
    GridCell() : tileSet(), type(Empty), isGraphCollapsed(false) {}

    GridCell(sf::Vector2i gridPos)
        : gridPosition(gridPos), tileSet(), 
        isGraphCollapsed(false){
        type = Empty;
        graph = tileSet.tiles[type].graph;
        possibleTileForThisCell = tileSet.possibleAdjacentTile[type];
        weight = tileSet.weight[type];
    }
    bool IsGraphCollapsed() const { return isGraphCollapsed; }
    sf::Vector2i GetGridLocation() const { return gridPosition; }
    TileType GetType() const { return type; }
    std::vector<TileType>& GetPossibleTileForThisCell() { return possibleTileForThisCell; }
    unsigned int GetEntropy() const { return possibleTileForThisCell.size(); }
	std::vector<unsigned int>& GetWeight() { return weight; }

    void CollapseGraph(TileType type) {
        if (type >= Empty && type <= MaxSize) {
            this->type = type;
            this->graph = tileSet.tiles[this->type].graph;
            this->isGraphCollapsed = true;
        }
    }

    void PropagateGraphCollapse(TileType type) {
        if (type >= Empty && type <= MaxSize) {
            TileTypeAndWeight temp = PropagateBasedOnWeight(type);
			this->possibleTileForThisCell = temp.first;
			this->weight = temp.second;
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

    TileTypeAndWeight PropagateBasedOnWeight(TileType incomingType) {
        const auto& incomingTiles = tileSet.possibleAdjacentTile[incomingType];
        const auto& incomingWeights = tileSet.weight[incomingType];
        std::vector<TileType> newPossibleTiles;
        std::vector<unsigned int> newWeights;

        std::unordered_map<TileType, unsigned int> incomingMap;
        for (size_t i = 0; i < incomingTiles.size(); ++i) {
            incomingMap[incomingTiles[i]] = incomingWeights[i];
        }
        for (size_t i = 0; i < possibleTileForThisCell.size(); ++i) {
            TileType tile = possibleTileForThisCell[i];
            if (incomingMap.find(tile) != incomingMap.end()) {
                newPossibleTiles.push_back(tile);
                newWeights.push_back(weight[i] + (int)(Utility::INFLUENCE * incomingMap[tile]));
            }
        }
        if (newPossibleTiles.empty()) {
            return TileTypeAndWeight(possibleTileForThisCell, weight);
        }
        return TileTypeAndWeight(newPossibleTiles, newWeights);
    }

};
