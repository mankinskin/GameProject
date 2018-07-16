#include "storage.h"
#include <unordered_map>
#include "gl.h"

std::unordered_map<unsigned int, unsigned int> targetBindingCounts;
int gl::SHADER_STORAGE_BUFFER_OFFSET_ALIGNMENT = 0;
int gl::UNIFORM_BUFFER_OFFSET_ALIGNMENT = 0;
int gl::MAX_UNIFORM_BUFFER_BINDINGS = 0;
int gl::MIN_MAP_BUFFER_ALIGNMENT = 0;

unsigned int gl::getNewTargetBinding(const unsigned int pTarget)
{
    auto target_it = targetBindingCounts.find(pTarget);
    if (target_it == targetBindingCounts.end()) {
        targetBindingCounts.insert({ pTarget, 1 });
        return 0;
    }
    if (pTarget == GL_UNIFORM_BUFFER) { 
        if (target_it->second == MAX_UNIFORM_BUFFER_BINDINGS) {
            puts("!!! Exceeding MAX_UNIFORM_BUFFER_BINDINGS!");
        }
    }
    return target_it->second++;
}

