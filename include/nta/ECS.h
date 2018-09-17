#ifndef NTA_ECS_H_INCLUDED
#define NTA_ECS_H_INCLUDED

#include <unordered_set>
#include <unordered_map>
#include <vector>

#include "nta/Entity.h"
#include "nta/Component.h"
#include "nta/IDFactory.h"
#include "nta/TypeMap.h"
#include "nta/Option.h"

namespace nta {
    /// Will only hold types for the form std::vector<T*>
    /// where T inherits Component
    typedef utils::TypeMap ComponentLists;
    typedef std::unordered_set<Entity> EntitySet;
    typedef std::unordered_set<Component*> ComponentSet;
    typedef std::unordered_map<Entity, ComponentLists> EntityComponentMap;
    typedef std::unordered_map<Component*, Entity> ComponentEntityMap;
    typedef std::unordered_map<Component*, std::vector<utils::TypeInfo>> ComponentListMap;
    /// Container class representing a complete Entity-Component System
    ///
    /// Manages creation of Entities and grouping of Components
    ///
    /// \todo Make this class's implementation less complicated
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
        /// Map associating each component to the lists (roughly, types) it belongs to
        ComponentListMap m_list_map;
        /// Components are organized into several lists for convenience
        ///
        /// e.g. all GraphicsComponents may be in one list that's looped over
        /// during rendering. 
        ComponentLists m_component_lists;
        /// Responsible for creating unique IDs for the Entities
        utils::IDFactory<Entity> m_entity_gen;
    public:
        ECS() {}
        ~ECS() { clear(); }

        /// Generates a new Entity, returning its ID
        Entity gen_entity();
        /// Generates several entities, storing their IDs in ids
        void gen_entities(std::size_t num, Entity* ids);
        /// Attempts to delete Entity with given id.
        ///
        /// Returns true on success
        bool delete_entity(Entity id);

        /// Adds the given Component to the given Entity.
        ///
        /// T should be a concrete type, not a pointer type
        ///
        /// Returns false on failure
        template<typename T>
        bool add_component(T cmpn, Entity entity);
        /// Attempts to delete the given Component
        ///
        /// Returns true on success
        bool delete_component(Component* cmpn);
        /// Deletes all componenets of the given type from the given entity
        ///
        /// if entity == NTA_INVALID_ID, then deletes all components of that type
        /// from all entities
        template<typename T>
        void delete_components(Entity entity);

        /// Returns true if the given Entity has a Component of type T
        template<typename T>
        bool has_component(Entity entity) const;
        /// Returns true if the given Entity exists
        bool does_entity_exist(Entity entity) const;

        /// Returns the Entity associated to this Component
        Entity get_owner(Component* cmpn) const;
        /// Returns a list of all components of the given type
        template<typename T>
        std::vector<T*>& get_component_list();
        /// Returna a list of all component of a given type belonging to entity
        template<typename T>
        utils::Option<std::vector<T*>&> get_component_list(Entity entity);
        /// Returns the lists of components associated to the given Entity
        ///
        /// Crashes if entity does not exist
        utils::Option<ComponentLists&> get_components(Entity entity);
        /// Returns the lists of components with the same owner as this one
        utils::Option<ComponentLists&> get_siblings(Component* cmpn) { return get_components(get_owner(cmpn)); }
        /// Returns the (first) Component of the given type associated to the given Entity
        template<typename T>
        utils::Option<T&> get_component(Entity entity) const;
        /// Returns the (first) Component of the given type with the same owner as cmpn
        template<typename T>
        utils::Option<T&> get_sibling(Component* cmpn) const { return get_component<T>(get_owner(cmpn)); }

        /// Forwards message to all Components associated to the same Entity as cmpn
        void broadcast(const Message& message, Component* cmpn);
        /// Forwards message to all Components belonging to entity
        void broadcast(const Message& message, Entity entity);
        /// Forwards message to all Components of the given type
        template<typename T>
        void broadcast(const Message& message);

        /// Removes all entites and components from this system
        void clear();
    };
    template<typename T>
    bool ECS::add_component(T cmpn, Entity entity) {
        if (m_entity_set.find(entity) == m_entity_set.end()) return false;
        cmpn.m_system = this;

        std::vector<T*>& list = m_component_lists.get<std::vector<T*>>();
        list.push_back(new T(cmpn));
        m_components_map[entity].get<std::vector<T*>>().push_back(list.back());
        m_entity_map[list.back()] = entity;
        m_component_set.insert(list.back());
        m_list_map[list.back()].push_back(utils::TypeInfo::get<std::vector<T*>>());
        return true;
    }
    template<typename T>
    void ECS::delete_components(Entity entity) {
        if (!does_entity_exist(entity)) return;
        auto& list = m_components_map[entity].get<std::vector<T*>>();
        while (!list.empty()) {
            delete_component(list.front());
        }
    }
    template<typename T>
    bool ECS::has_component(Entity entity) const {
        if (!does_entity_exist(entity)) return false;
        if (!m_components_map.find(entity)->second.contains<std::vector<T*>>()) return false;
        return !m_components_map.find(entity)->second.find<std::vector<T*>>().empty();
    }
    template<typename T>
    std::vector<T*>& ECS::get_component_list() {
        return m_component_lists.get<std::vector<T*>>();
    }
    template<typename T>
    utils::Option<std::vector<T*>&> ECS::get_component_list(Entity entity) {
        if (!does_entity_exist(entity)) return utils::Option<std::vector<T*>&>::none();
        return utils::Option<std::vector<T*>&>::new_some(m_components_map[entity].get<std::vector<T*>>());
    }
    template<typename T>
    utils::Option<T&> ECS::get_component(Entity entity) const {
        if (!has_component<T>(entity)) return utils::Option<T&>::none();
        return utils::Option<T&>::new_some(*m_components_map.find(entity)->second.find<std::vector<T*>>().front());
    }
    template<typename T>
    void ECS::broadcast(const Message& message) {
        for (auto& cmpn : m_component_lists.get<std::vector<T*>>()) {
            cmpn->receive(message);
        }
    }
}

#endif // NTA_ECS_H_INCLUDED