#pragma once 
#include <vector> 
#include <glm.hpp>
#include "storage.h"

//Lights are a set of data used as a light source in a layered shading process
//each light is either of type
//  - point light/omnidirectional light
//  - spot light/cone light ( not yet implemented )
//
//Using the data rendered into the Geometry Framebuffer, each light can calculate its influence 
//on every pixel on the screen

namespace lights
{
	struct LightIndexRange 
    {
		LightIndexRange( unsigned int pOffset, unsigned int pCount )
			:offset( pOffset ), count( pCount ) 
        {}
		unsigned int offset;
		unsigned int count;
	};

	void initLights();

	extern std::vector<glm::vec4> allLightData;
	extern std::vector<LightIndexRange> allLightIndexRanges;
	extern unsigned int lightVAO;
	extern gl::StreamStorage lightIndexVBO;
	extern gl::StreamStorage lightDataUBO;
	extern unsigned int lightShaderProgram;
	const unsigned int MAX_LIGHT_COUNT = 100;

	void createLightVAO();
    void createLightVBO();

	void createLightDataBuffer();
	void updateLightDataBuffer();
	void updateLightIndexRangeBuffer();

	void initLightShader();

	void renderLights();
	void setupLightShader();
	unsigned int createLight( glm::vec4 pPos, glm::vec4 pColor );
	unsigned int createLight( glm::vec4 pPos, glm::vec4 pColor, glm::vec4 pFrustum );
	void setLightPos( unsigned int pLightIndex, glm::vec3& pPos );
	void setLightPos( unsigned int pLightIndex, glm::vec4& pPos );
	void setLightColor( unsigned int pLightIndex, glm::vec3& pColor );
	void setLightColor( unsigned int pLightIndex, glm::vec4& pColor );
	
	glm::vec4 & getLightColor( unsigned int pLightIndex );

	void reserveLightSpace( unsigned int pCount );//reserves 3 vec4s and one index range for count
	void reservePointLightSpace( unsigned int pCount );//reserves only 2 vec4 ( and one index range ) per light
}

