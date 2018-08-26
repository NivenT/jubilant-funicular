#ifndef NTA_ECS_H_INCLUDED
#define NTA_ECS_H_INCLUDED

#include <unordered_set>
#include <unordered_map>

#include "nta/Entity.h"
#include "nta/Component.h"
#include "nta/IDFactory.h"

#define NTA_ECS_NUM_COMPONENT_LISTS (sizeof(ComponentListID)*8)

namespace nta {
	struct ComponentNode;
	typedef std::unordered_set<Entity> EntitySet;
	typedef std::unordered_set<Component*> ComponentSet;
	typedef std::unordered_map<EntityID, ComponentNode*> EntityComponentMap;
	typedef std::unordered_map<Component*, EntityID> ComponentEntityMap;
	/// Linked list of Components
	struct ComponentNode {
		ComponentNode(Component* cmp) : comp(cmp) {}
        ComponentNode(Component* cmp, ComponentNode* nxt) : next(nxt), comp(cmp) {}
        ~ComponentNode() { if (next) delete next; }

        static void remove(ComponentNode** node, Component* cmpn) {
        	ComponentNode** curr = node;
			while ((*curr) && (*curr)->comp != cmpn) {
				curr = &(*curr)->next;
			}
			if (*curr) *curr = (*curr)->next;
        }

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
		EntitySet m_entity_set;
		/// All components in the system
		ComponentSet m_component_set;
		/// Map associating each Entity to its (linked) list of components
		EntityComponentMap m_components_map;
		/// Map associating each component to its entity
		ComponentEntityMap m_entity_map;
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
		void clear();

		/// Generates a new Entity, returning its ID
		EntityID gen_entity();
		/// Generates several entities, storing their IDs in ids
		void gen_entities(std::size_t num, EntityID* ids);
		/// Attempts to delete Entity with given id.
		///
		/// Returns true on success
		bool delete_entity(EntityID id);

		/// Adds the given Component to the given Entity
		///
		/// Returns false if entity does not exist
		bool add_component(Component* cmpn, EntityID entity);
		/// Attempts to deletet the given Component
		///
		/// Returns true on success
		bool delete_component(Component* cmpn);

		/// Returns the associated list of components
		///
		/// id should have only 1 nonzero bit
		ComponentNode* get_component_list(ComponentListID id);

		/// Forwards message to all Components associated to the same Entity as cmpn
		void broadcast(const Message& message, Component* cmpn);
	};
}

#endif // NTA_ECS_H_INCLUDED