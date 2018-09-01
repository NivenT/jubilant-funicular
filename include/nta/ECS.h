#ifndef NTA_ECS_H_INCLUDED
#define NTA_ECS_H_INCLUDED

#include <unordered_set>
#include <unordered_map>

#include "nta/Entity.h"
#include "nta/Component.h"
#include "nta/IDFactory.h"
#include "nta/LinkedNode.h"

#define NTA_ECS_NUM_COMPONENT_LISTS (sizeof(ComponentListID)*8)

namespace nta {
    typedef utils::LinkedNode<Component> ComponentNode;
    typedef std::unordered_set<Entity> EntitySet;
    typedef std::unordered_set<Component*> ComponentSet;
    typedef std::unordered_map<EntityID, ComponentNode*> EntityComponentMap;
    typedef std::unordered_map<Component*, EntityID> ComponentEntityMap;
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
        /// Removes all entites and components from this system
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
        /// Returns false on failure
        bool add_component(Component* cmpn, EntityID entity);
        /// Attempts to delete the given Component
        ///
        /// Returns true on success
        bool delete_component(Component* cmpn);
        /// Deletes all components of the given types from the given entity
        void delete_components(EntityID entity, ComponentListID lists);

        /// Returns true if the given Entity has a Component belonging to
        /// the given list
        bool has_component(EntityID entity, ComponentListID list) const;
        /// Returns true if the given Entity exists
        bool does_entity_exist(EntityID entity) const;

        /// Returns the Entity associated to this Component
        EntityID get_owner(Component* cmpn) const;
        /// Returns the associated list of components
        ///
        /// id should have only 1 nonzero bit
        ComponentNode* get_component_list(ComponentListID id) const;
        /// Returns the list of components associated to the given Entity
        ComponentNode* get_components(EntityID entity) const;
        /// Returns the list of components with the same owner as this one
        ComponentNode* get_siblings(Component* cmpn) const { return get_components(get_owner(cmpn)); }
        /// Returns the (first) Component of the given type associated the the given Entity
        Component* get_component(EntityID entity, ComponentListID list) const;

        /// Forwards message to all Components associated to the same Entity as cmpn
        void broadcast(const Message& message, Component* cmpn);
        /// Forwards message to all Components belonging to entity
        void broadcast(const Message& message, EntityID entity);
    };
}

#endif // NTA_ECS_H_INCLUDED