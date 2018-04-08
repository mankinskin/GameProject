#include "storage.h"
#include <unordered_map>

std::unordered_map<unsigned int, unsigned int> targetBindingCounts;
int gl::SHADER_STORAGE_BUFFER_OFFSET_ALIGNMENT = 0;
int gl::UNIFORM_BUFFER_OFFSET_ALIGNMENT = 0;
int gl::MAX_UNIFORM_BUFFER_BINDINGS = 0;
int gl::MIN_MAP_BUFFER_ALIGNMENT = 0;

gl::Storage gl::createStorage( std::string pName, unsigned int pCapacity, unsigned int pFlags, void* pData )
{
	unsigned int id = 0;
	glCreateBuffers( 1, &id );
	glNamedBufferStorage( id, pCapacity, pData, pFlags );
	return Storage( pName, id, pCapacity, pFlags );
}

unsigned int getNewTargetBinding( const unsigned int pTarget )
{
	auto target_it = targetBindingCounts.find( pTarget );
	if ( target_it == targetBindingCounts.end() ) {
        targetBindingCounts.insert( { pTarget, 1 } );
        return 0;
	}
	return target_it->second++;
}

void gl::setStorageTarget( gl::Storage& pStorage, const unsigned int pTarget )
{
	pStorage.target = pTarget;
	pStorage.binding = getNewTargetBinding( pStorage.target );
	glBindBufferBase( pStorage.target, pStorage.binding, pStorage.ID );	
}
