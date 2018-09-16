#ifndef NTA_ENTITY_H_INCLUDED
#define NTA_ENTITY_H_INCLUDED

#include "nta/Wrapper.h"

namespace nta {
	class ECS;
	NTA_CREATE_WRAPPER(Entity, uint64_t)
}

namespace std {
	NTA_HASH_WRAPPER(nta::Entity)
}

#endif // NTA_ENTITY_H_INCLUDED