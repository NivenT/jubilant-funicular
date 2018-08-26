#ifndef NTA_ECS_H_INCLUDED
#define NTA_ECS_H_INCLUDED

#include <unordered_set>

#include "nta/Entity.h"
#include "nta/Component.h"
#include "nta/IDFactory.h"

#define NTA_ECS_NUM_COMPONENT_LISTS (sizeof(ComponentListID)*8)

namespace nta {
	typedef uint64_t ComponentListID;
	/// Container class representing a complete Entity-Component System
	///
	/// Manages creation of Entities and grouping of Components
	class ECS {
	private:
		/// Linked list of Components
		struct ComponentNode {
			ComponentNode(Component* cmp) : comp(cmp) {}
            ComponentNode(Component* cmp, ComponentNode* nxt) : next(nxt), comp(cmp) {}
            ~ComponentNode() { if (next) delete next; }

            /// The next node in the linked list
            ComponentNode* next = nullptr;
            /// The Component associated to this node
            Component* comp = nullptr;
		};

		/// All Entities in the system
		std::unordered_set<Entity> m_entity_set;
		/// All components in the system
		std::unordered_set<Component*> m_component_set;
		/// Components are organized into several lists for convenience
		///
		/// e.g. all GraphicsComponents may be in one list that's looped over
		/// during rendering. 
		ComponentNode m_component_lists[NTA_ECS_NUM_COMPONENT_LISTS];
		/// Responsible for creating unique IDs for the Entities
		// I couldn't decide between Factory and Generator for the type name...
		utils::IDFactory<EntityID> m_id_generator;
	public:
		ECS();
		~ECS();
	};
}

#endif // NTA_ECS_H_INCLUDED