#pragma once
#include <string>
#include <vector>
#include <unordered_map>

namespace vao {
	extern int MIN_MAP_BUFFER_ALIGNMENT;
	extern int MAP_PERSISTENT_FLAGS;
	extern int UNIFORM_BUFFER_OFFSET_ALIGNMENT;
	extern int SHADER_STORAGE_BUFFER_OFFSET_ALIGNMENT;
	extern int MAX_UNIFORM_BUFFER_BINDINGS;

	struct Storage {
		Storage( std::string Name )
            :name( Name ) {}
        std::string name;
		unsigned int ID = 0;
		unsigned int capacity = 0;
		unsigned int bufferFlags = 0;
		unsigned int streamIndex = 0;
		unsigned int target = 0;
		unsigned int binding = 0;

		unsigned int vaoID = 0;
		unsigned int stride = UNIFORM_BUFFER_OFFSET_ALIGNMENT;
	};

	unsigned int createStorage(std::string Name );
	void initStorageData( unsigned int pStorage, unsigned int pCapacity, const void * pData, unsigned int pFlags );
	unsigned int createStorage( std::string Name, unsigned int pCapacity, const void* pData, unsigned int pFlags );

	struct Stream {
		Stream() : mapFlags( 0 ), mappedPtr( nullptr ), alignment( UNIFORM_BUFFER_OFFSET_ALIGNMENT ), updateOffset( 0 ), lastUpdateSize( 0 ) {}
		unsigned int mapFlags;
		void* mappedPtr;
		unsigned int alignment;
		unsigned int updateOffset;
		unsigned int lastUpdateSize;
	};

	unsigned int createStream( unsigned int pStorageIndex, unsigned int pMapFlags );


	const Storage& getStorage( unsigned int pStorageIndex );
	unsigned int getStorageID( unsigned int pStorageIndex );
	void* getMappedPtr( Storage& pStorage );
	void* getMappedPtr( unsigned int pStorageIndex );
	void uploadStorage( unsigned int pStorageIndex, unsigned int pUploadSize, void * pData );
	void uploadStorage( Storage& pStorage, unsigned int pUploadSize, void * pData );
	void * mapStorage( unsigned int pStorageIndex, unsigned int pFlags );
	void * mapStorage( Storage& pStorage, unsigned int pFlags );
	void bindStorage( unsigned int pTarget, unsigned int pStorageIndex );
	void bindStorage( unsigned int pTarget, Storage& pStorage );
	void bindStorageRange( Storage& pStorage, unsigned int pOffset, unsigned int pSize );
	void bindStorageRange( unsigned int pStorageIndex, unsigned int pOffset, unsigned int pSize );
	void setVertexArrayVertexStorage( unsigned int pVAO, unsigned int pBinding, unsigned int pStorageIndex, unsigned int pStride );
	void setVertexArrayVertexStorage( unsigned int pVAO, unsigned int pBinding, Storage& pStorage, unsigned int pStride );
	void setVertexAttrib( unsigned int pVAO, unsigned int pBindingIndex, unsigned int pAttributeIndex, unsigned int pCount, unsigned int pType, unsigned int pOffset, unsigned int pNormalize = false );


	extern std::unordered_map<unsigned int, unsigned int> bufferTargetBinds;
	extern std::vector<Storage> allStorages;
	extern std::vector<Stream> allStreams;


}
