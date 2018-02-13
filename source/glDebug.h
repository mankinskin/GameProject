#pragma once
#include "gl.h"
#include <vector>
#include <unordered_map>
#include <string>
namespace glDebug {


	extern std::unordered_map<std::string, size_t> debugMeshLookup;
	size_t generateDebugGrid(std::string pName, float pTileSize, size_t pTileCount, float pColorR, float pColorG, float pColorB, float pAlpha);
	size_t generateDebugGrid(std::string pName, float pTileSizeX, float pTileSizeY, size_t pTileCountX, size_t pTileCountY, float pColorR, float pColorG, float pColorB, float pAlpha);
	void init();
	void toggleGrid();
	void toggleCoord();
	size_t initCoordinateSystem(std::string pName);
}
