#ifndef NTA_ECS_H_INCLUDED
#define NTA_ECS_H_INCLUDED

#include <unordered_set>

#include "nta/Entity.h"
#include "nta/Component.h"
#include "nta/IDFactory.h"

#define NTA_ECS_NUM_COMPONENT_LISTS (sizeof(ComponentListID)*8)

namespace nta {
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
	/// Container class representing a complete Entity-Component System
	///
	/// Manages creation of Entities and grouping of Components
	class ECS {
	private:
		/// All Entities in the system
		std::unordered_set<Entity> m_entity_set;
		/// All components in the system
		std::unordered_set<Component*> m_component_set;
		/// Components are organized into several lists for convenience
		///
		/// e.g. all GraphicsComponents may be in one list that's looped over
		/// during rendering. 
		ComponentNode* m_component_lists[NTA_ECS_NUM_COMPONENT_LISTS];
		/// Responsible for creating unique IDs for the Entities
		utils::IDFactory<EntityID> m_entity_gen;
	public:
		ECS();
		~ECS();

		/// Generates a new Entity, returning its ID
		EntityID gen_entity();
		/// Generates several entities, storing their IDs in ids
		void gen_entities(std::size_t num, EntityID* ids);

		/// Attempts to delete Entity with given id.
		///
		/// Returns true on success
		bool delete_entity(EntityID id);

		/// Returns the associated list of components
		///
		/// id should have only 1 nonzero bit
		ComponentNode* get_component_list(ComponentListID id);

		/// Forwards message to lists specified by recipients
		///
		/// Every 1 bit in recipient corresponds to a different list
		void broadcast(const Message& message, ComponentListID recipients);
	};
}

#endif // NTA_ECS_H_INCLUDED