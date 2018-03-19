#include "vao.h"
#include "gldebug.h"
#include <algorithm>
#include "debug.h"
#include <gtc/type_ptr.hpp>
#include <glew.h>

std::vector<vao::Storage> vao::allStorages = std::vector<vao::Storage>();
std::vector<vao::Stream> vao::allStreams;

int vao::UNIFORM_BUFFER_OFFSET_ALIGNMENT = 0;
int vao::SHADER_STORAGE_BUFFER_OFFSET_ALIGNMENT = 0;
int vao::MAX_UNIFORM_BUFFER_BINDINGS = 0;
int vao::MIN_MAP_BUFFER_ALIGNMENT = 0;
int vao::MAP_PERSISTENT_FLAGS = GL_MAP_PERSISTENT_BIT | GL_MAP_COHERENT_BIT;
std::unordered_map<unsigned int, unsigned int> vao::bufferTargetBinds;


unsigned int vao::createStorage( std::string Name )
{
	Storage storage( Name );
	unsigned int ind = allStorages.size();
	glCreateBuffers( 1, &storage.ID );
	allStorages.push_back( storage );
	return ind;
}
unsigned int vao::createStorage( std::string Name, unsigned int pCapacity, const void* pData, unsigned int pFlags )
{
	unsigned int ind = createStorage( Name );
	initStorageData( ind, pCapacity, pData, pFlags );
	return ind;
}
void vao::initStorageData( unsigned int pStorage, unsigned int pCapacity, const void* pData, unsigned int pFlags )
{
	Storage& stor = allStorages[pStorage];
    printf( "Allocating %d bytes for Storage %s...\n", pCapacity, stor.name.c_str() );
	glNamedBufferStorage( stor.ID, pCapacity, pData, pFlags );
	stor.capacity = pCapacity;
	stor.bufferFlags = pFlags;
}

unsigned int vao::createStream( unsigned int pStorageIndex, unsigned int pMapFlags )
{
	Stream stream;
	unsigned int ind = allStreams.size();
	Storage& storage = allStorages[pStorageIndex];
	storage.streamIndex = ind;
	if ( storage.target == GL_UNIFORM_BUFFER ) {
		stream.alignment = UNIFORM_BUFFER_OFFSET_ALIGNMENT;
	}
	else {
		stream.alignment = storage.stride;
	}

	stream.mapFlags = pMapFlags | storage.bufferFlags | MAP_PERSISTENT_FLAGS;
	stream.mappedPtr = mapStorage( pStorageIndex, pMapFlags );
	allStreams.push_back( stream );
	return ind;
}

void* vao::mapStorage( unsigned int pStorageIndex, unsigned int pFlags )
{
	return mapStorage( allStorages[pStorageIndex], pFlags );
}

void* vao::mapStorage( Storage& pStorage, unsigned int pFlags )
{ 
    printf( "Mapping Storage %s...\n", pStorage.name.c_str() );
	void* p = glMapNamedBufferRange( pStorage.ID, 0, pStorage.capacity, pFlags );
	if ( !p ) {
		debug::pushError( "Failed to map Storage " + pStorage.name + " !\n" );
	}
	return p;
}


void vao::uploadStorage( unsigned int pStorageIndex, unsigned int pUploadSize, void* pData )
{
	uploadStorage( allStorages[pStorageIndex], pUploadSize, pData );
}

void vao::uploadStorage( Storage& pStorage, unsigned int pUploadSize, void* pData )
{
	Stream& stream = allStreams[pStorage.streamIndex];
	//stream.updateOffset = stream.updateOffset + stream.lastUpdateSize;
	//stream.updateOffset += ( stream.alignment - stream.updateOffset%stream.alignment ) % stream.alignment;
	//unsigned int updateSize = std::max( pUploadSize, stream.alignment );

	//stream.updateOffset = stream.updateOffset * ( 1 - ( stream.updateOffset + updateSize > pStorage.capacity ) );


	//stream.lastUpdateSize = updateSize;
	std::memcpy( ( char* )stream.mappedPtr + stream.updateOffset, pData, pUploadSize );

	//if ( pStorage.target == GL_UNIFORM_BUFFER ) {
	//	glBindBufferRange( GL_UNIFORM_BUFFER, pStorage.binding, pStorage.ID, stream.updateOffset, updateSize );
	//}
	//else if ( pStorage.target == GL_ARRAY_BUFFER ) {
	//	glVertexArrayVertexBuffer( pStorage.vaoID, pStorage.binding, pStorage.ID, stream.updateOffset, pStorage.stride );
	//}
}

const vao::Storage& vao::getStorage( unsigned int pStorageIndex )
{
	return allStorages[pStorageIndex];
}

unsigned int vao::getStorageID( unsigned int storageIndex ) 
{
    return getStorage( storageIndex ).ID;
}

void * vao::getMappedPtr( Storage & pStorage )
{
	return allStreams[pStorage.streamIndex].mappedPtr;
}

void * vao::getMappedPtr( unsigned int pStorageIndex )
{
	return getMappedPtr( allStorages[pStorageIndex] );
}

void vao::setVertexAttrib( unsigned int pVAO, unsigned int pBindingIndex, unsigned int pAttributeIndex, unsigned int pCount, unsigned int pType, unsigned int pOffset, unsigned int pNormalize ) {
	if ( pType == GL_FLOAT ) {
		glVertexArrayAttribFormat( pVAO, pAttributeIndex, pCount, pType, pNormalize, pOffset );
	}
	else if ( pType == GL_INT || pType == GL_UNSIGNED_INT ) {
		glVertexArrayAttribIFormat( pVAO, pAttributeIndex, pCount, pType, pOffset );
	}
	else if ( pType == GL_DOUBLE ) {
		glVertexArrayAttribLFormat( pVAO, pAttributeIndex, pCount, pType, pOffset );
	}
	glVertexArrayAttribBinding( pVAO, pAttributeIndex, pBindingIndex );
	glEnableVertexArrayAttrib( pVAO, pAttributeIndex );
}

void vao::bindStorage( unsigned int pTarget, unsigned int pStorageIndex )
{
	bindStorage( pTarget, allStorages[pStorageIndex] );
}
void vao::bindStorage( unsigned int pTarget, Storage& pStorage )
{
	pStorage.target = pTarget;
	
	if ( pTarget == GL_UNIFORM_BUFFER || pTarget == GL_SHADER_STORAGE_BUFFER ) {//binds a storage to an index of a target ( like GL_UNIFORM_BUFFER or GL_SHADER_STORAGE_BUFFER )
		auto bufferTargetBinding = bufferTargetBinds.find( pTarget );
		if ( bufferTargetBinding == bufferTargetBinds.end() ) {
			bufferTargetBinding = bufferTargetBinds.insert( std::make_pair( pTarget, 0 ) ).first;
		}

		pStorage.binding = bufferTargetBinding->second++;

		glBindBufferBase( pTarget, pStorage.binding, pStorage.ID );
	}else{
		pStorage.binding = 0;
	}
	debug::printErrors();
}

void vao::bindStorageRange( Storage& pStorage, unsigned int pOffset, unsigned int pSize )
{
	glBindBufferRange( pStorage.target, pStorage.binding, pStorage.ID, pOffset, pSize );
}
void vao::bindStorageRange( unsigned int pStorageIndex, unsigned int pOffset, unsigned int pSize )
{
	Storage& stor = allStorages[pStorageIndex];
	glBindBufferRange( stor.target, stor.binding, stor.ID, pOffset, pSize );
}

void vao::setVertexArrayVertexStorage( unsigned int pVAO, unsigned int pBinding, unsigned int pStorageIndex, unsigned int pStride )
{
	setVertexArrayVertexStorage( pVAO, pBinding, allStorages[pStorageIndex], pStride );
}

void vao::setVertexArrayVertexStorage( unsigned int pVAO, unsigned int pBinding, Storage & pStorage, unsigned int pStride )
{
	pStorage.vaoID = pVAO;
	pStorage.target = GL_ARRAY_BUFFER;
	pStorage.binding = pBinding;
	pStorage.stride = pStride;
}
