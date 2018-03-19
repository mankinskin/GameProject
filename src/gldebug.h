#pragma once
#include "gl.h"
#include <vector>
#include <unordered_map>
#include <string>

namespace glDebug {

	extern std::unordered_map<std::string, unsigned int> debugMeshLookup;
	unsigned int generateDebugGrid( std::string pName, float pTileSize, unsigned int pTileCount, float pColorR, float pColorG, float pColorB, float pAlpha );
	unsigned int generateDebugGrid( std::string pName, float pTileSizeX, float pTileSizeY, unsigned int pTileCountX, unsigned int pTileCountY, float pColorR, float pColorG, float pColorB, float pAlpha );
	void init();
    void createDebugGeometry();
	void toggleGrid();
	void toggleCoord();
	unsigned int initCoordinateSystem( std::string pName );
}
