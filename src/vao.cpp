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
std::unordered_map<size_t, size_t> vao::bufferTargetBinds;


size_t vao::createStorage()
{
	Storage storage;
	unsigned int ind = allStorages.size();
	glCreateBuffers(1, &storage.ID);
	allStorages.push_back(storage);
	return ind;
}
size_t vao::createStorage(size_t pCapacity, const void* pData, size_t pFlags)
{
	size_t ind = createStorage();
	initStorageData(ind, pCapacity, pData, pFlags);
	return ind;
}
void vao::initStorageData(size_t pStorage, size_t pCapacity, const void* pData, size_t pFlags)
{
	Storage& stor = allStorages[pStorage];
	glNamedBufferStorage(stor.ID, pCapacity, pData, pFlags);
	stor.capacity = pCapacity;
	stor.bufferFlags = pFlags;
}
size_t vao::createStream(size_t pStorageIndex, size_t pMapFlags)
{
	Stream stream;
	size_t ind = allStreams.size();
	Storage& storage = allStorages[pStorageIndex];
	storage.streamIndex = ind;
	if (storage.target == GL_UNIFORM_BUFFER) {
		stream.alignment = UNIFORM_BUFFER_OFFSET_ALIGNMENT;
	}
	else {
		stream.alignment = storage.stride;
	}

	stream.mapFlags = pMapFlags | storage.bufferFlags | MAP_PERSISTENT_FLAGS;
	stream.mappedPtr = mapStorage(pStorageIndex, pMapFlags);
	allStreams.push_back(stream);
	return ind;
}

void* vao::mapStorage(size_t pStorageIndex, size_t pFlags)
{
    printf("Accessing Storage at index %d\n", pStorageIndex);
	return mapStorage(allStorages[pStorageIndex], pFlags);
}

void* vao::mapStorage(Storage& pStorage, size_t pFlags)
{ 
    printf("Mapping Storage %d...\n", pStorage.ID);
	void* p = glMapNamedBufferRange(pStorage.ID, 0, pStorage.capacity, pFlags);
	if (!p) {
		debug::pushError("Failed to map Storage!\n");
	}
	return p;
}


void vao::uploadStorage(size_t pStorageIndex, size_t pUploadSize, void* pData)
{
	uploadStorage(allStorages[pStorageIndex], pUploadSize, pData);
}

void vao::uploadStorage(Storage& pStorage, size_t pUploadSize, void* pData)
{
	Stream& stream = allStreams[pStorage.streamIndex];
	stream.updateOffset = stream.updateOffset + stream.lastUpdateSize;
	stream.updateOffset += (stream.alignment - stream.updateOffset%stream.alignment) % stream.alignment;
	size_t updateSize = std::max(pUploadSize, stream.alignment);

	stream.updateOffset = stream.updateOffset * (1 - (stream.updateOffset + updateSize > pStorage.capacity));


	stream.lastUpdateSize = updateSize;
	std::memcpy((char*)stream.mappedPtr + stream.updateOffset, pData, pUploadSize);

	if (pStorage.target == GL_UNIFORM_BUFFER) {
		glBindBufferRange(GL_UNIFORM_BUFFER, pStorage.binding, pStorage.ID, stream.updateOffset, updateSize);
	}
	else if (pStorage.target == GL_ARRAY_BUFFER) {
		glVertexArrayVertexBuffer(pStorage.vaoID, pStorage.binding, pStorage.ID, stream.updateOffset, pStorage.stride);
	}
}

size_t vao::getStorageID(size_t pStorageIndex)
{
	return allStorages[pStorageIndex].ID;
}

void * vao::getMappedPtr(Storage & pStorage)
{
	return allStreams[pStorage.streamIndex].mappedPtr;
}

void * vao::getMappedPtr(size_t pStorageIndex)
{
	return getMappedPtr(allStorages[pStorageIndex]);
}

void vao::setVertexAttrib(size_t pVAO, size_t pBindingIndex, size_t pAttributeIndex, size_t pCount, size_t pType, size_t pOffset, size_t pNormalize) {
	if (pType == GL_FLOAT) {
		glVertexArrayAttribFormat(pVAO, pAttributeIndex, pCount, pType, pNormalize, pOffset);
	}
	else if (pType == GL_INT || pType == GL_UNSIGNED_INT) {
		glVertexArrayAttribIFormat(pVAO, pAttributeIndex, pCount, pType, pOffset);
	}
	else if (pType == GL_DOUBLE) {
		glVertexArrayAttribLFormat(pVAO, pAttributeIndex, pCount, pType, pOffset);
	}
	glVertexArrayAttribBinding(pVAO, pAttributeIndex, pBindingIndex);
	glEnableVertexArrayAttrib(pVAO, pAttributeIndex);
}

void vao::bindStorage(size_t pTarget, size_t pStorageIndex)
{
	bindStorage(pTarget, allStorages[pStorageIndex]);
}
void vao::bindStorage(size_t pTarget, Storage& pStorage)
{
	pStorage.target = pTarget;
	
	if (pTarget == GL_UNIFORM_BUFFER || pTarget == GL_SHADER_STORAGE_BUFFER) {//binds a storage to an index of a target (like GL_UNIFORM_BUFFER or GL_SHADER_STORAGE_BUFFER)
		auto bufferTargetBinding = bufferTargetBinds.find(pTarget);
		if (bufferTargetBinding == bufferTargetBinds.end()) {
			bufferTargetBinding = bufferTargetBinds.insert(std::make_pair(pTarget, 0)).first;
		}

		pStorage.binding = bufferTargetBinding->second++;

		glBindBufferBase(pTarget, pStorage.binding, pStorage.ID);
	}else{
		pStorage.binding = 0;
	}
	debug::printErrors();
}

void vao::bindStorageRange(Storage& pStorage, size_t pOffset, size_t pSize)
{
	glBindBufferRange(pStorage.target, pStorage.binding, pStorage.ID, pOffset, pSize);
}
void vao::bindStorageRange(size_t pStorageIndex, size_t pOffset, size_t pSize)
{
	Storage& stor = allStorages[pStorageIndex];
	glBindBufferRange(stor.target, stor.binding, stor.ID, pOffset, pSize);
}

void vao::setVertexArrayVertexStorage(size_t pVAO, size_t pBinding, size_t pStorageIndex, size_t pStride)
{
	setVertexArrayVertexStorage(pVAO, pBinding, allStorages[pStorageIndex], pStride);
}

void vao::setVertexArrayVertexStorage(size_t pVAO, size_t pBinding, Storage & pStorage, size_t pStride)
{
	pStorage.vaoID = pVAO;
	pStorage.target = GL_ARRAY_BUFFER;
	pStorage.binding = pBinding;
	pStorage.stride = pStride;
}
