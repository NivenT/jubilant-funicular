#ifndef NTA_ECS_H_INCLUDED
#define NTA_ECS_H_INCLUDED

#include <unordered_set>

#include "nta/Entity.h"
#include "nta/Component.h"

#define NTA_ECS_NUM_COMPONENT_LISTS (sizeof(ComponentListID)*8)

namespace nta {
	typedef uint64_t ComponentListID;
	class ECS {
	private:
		std::unordered_set<Entity> m_entity_set;
		std::unordered_set<Component*> m_component_set;
	public:
		ECS();
	};
}

#endif // NTA_ECS_H_INCLUDED