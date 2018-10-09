#ifndef NTA_ECS_H_INCLUDED
#define NTA_ECS_H_INCLUDED

#include <unordered_set>
#include <unordered_map>
#include <vector>

#include "nta/Component.h"
#include "nta/IDFactory.h"
#include "nta/Wrapper.h"
#include "nta/TypeMap.h"

namespace nta {
    typedef utils::Wrapper<uint64_t, struct EntityTag> Entity;
    /// Will only hold types for the form std::vector<T*>
    /// where T inherits Component
    typedef utils::TypeMap ComponentLists;
    typedef std::unordered_set<Entity> EntitySet;
    typedef std::unordered_map<ComponentID, Component*> ComponentSet;
    typedef std::unordered_map<Entity, ComponentLists> EntityComponentMap;
    typedef std::unordered_map<ComponentID, Entity> ComponentEntityMap;
    typedef std::unordered_map<ComponentID, std::vector<utils::TypeInfo>> ComponentListMap;
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
        /// Responsible for creating unique IDs for the Components
        utils::IDFactory<ComponentID> m_cmpn_gen;
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
        /// Returns the number of entities in the system
        int num_entities() const { return m_entity_set.size(); }

        /// Adds the given Component to the given Entity.
        ///
        /// You pass in the entity and any arguments needed to construct a new object of type T
        ///
        /// T should be a concrete type, not a pointer type
        ///
        /// Returns the id of the created Component (NTA_INVALID_ID on failure).
        template<typename T, typename... Args>
        ComponentID add_component(Entity entity, Args&&... args);
        /// Adds the given component to the vector of components of type T
        ///
        /// cmpn should be a pointer to a Component that inherits from T
        ///
        /// returns false on failure
        template<typename T>
        bool add_component_to_list(ComponentID cmpn);
        /// Attempts to delete the given Component
        ///
        /// Returns the id of the deleted Component
        ComponentID delete_component(ComponentID cmpn);
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
        Entity get_owner(ComponentID cmpn) const;
        /// Returns a list of all components of the given type
        template<typename T>
        std::vector<T*> get_component_list() const;
        /// Same as get_component_list but returns a constant reference
        template<typename T>
        std::vector<T*> get_component_list(Entity entity) const;
        /// Returns the lists of components associated to the given Entity
        ///
        /// Crashes if entity does not exist
        utils::Option<ComponentLists&> get_components(Entity entity);
        /// Returns the lists of components with the same owner as this one
        utils::Option<ComponentLists&> get_siblings(ComponentID cmpn) { return get_components(get_owner(cmpn)); }
        /// Returns the (first) Component of the given type associated to the given Entity
        template<typename T>
        utils::Option<T&> get_component(Entity entity) const;
        /// Returns the Component with the given id
        Component* get_component(ComponentID id) const;
        /// Returns the (first) Component of the given type with the same owner as cmpn
        template<typename T>
        utils::Option<T&> get_sibling(ComponentID cmpn) const { return get_component<T>(get_owner(cmpn)); }

        /// Forwards message to all Components associated to the same Entity as cmpn
        void broadcast(const Message& message, ComponentID cmpn);
        /// Forwards message to all Components belonging to entity
        void broadcast(const Message& message, Entity entity);
        /// Forwards message to all Components of the given type
        template<typename T>
        void broadcast(const Message& message);

        /// Like broadcast but returns the first response received
        utils::Option<Message> shout(const Message& request, ComponentID cmpn);
        utils::Option<Message> shout(const Message& request, Entity entity);
        template<typename T>
        utils::Option<Message> shout(const Message& request);

        /// Removes all entites and components from this system
        void clear();
    };
    template<typename T, typename... Args>
    ComponentID ECS::add_component(Entity entity, Args&&... args) {
        if (m_entity_set.find(entity) == m_entity_set.end()) return NTA_INVALID_ID;
        std::vector<T*>& list = m_component_lists.get<std::vector<T*>>();
        list.push_back(new T(std::forward<Args>(args)...));
        list.back()->m_system = this;
        list.back()->m_id = m_cmpn_gen();

        m_components_map[entity].get<std::vector<T*>>().push_back(list.back());
        m_entity_map[list.back()->m_id] = entity;
        m_component_set[list.back()->m_id] = list.back();
        m_list_map[list.back()->m_id].push_back(utils::TypeInfo::get<std::vector<T*>>());
        return list.back()->m_id;
    }
    template<typename T>
    bool ECS::add_component_to_list(ComponentID cmpn) {
        auto it = m_component_set.find(cmpn);
        if (it == m_component_set.end()) return false;
        m_component_lists.get<std::vector<T*>>().push_back((T*)it->second);
        m_components_map[m_entity_map[cmpn]].get<std::vector<T*>>().push_back((T*)it->second);
        m_list_map[cmpn].push_back(utils::TypeInfo::get<std::vector<T*>>());
        return true;
    }
    template<typename T>
    void ECS::delete_components(Entity entity) {
        if (!does_entity_exist(entity)) return;
        auto& list = m_components_map[entity].get<std::vector<T*>>();
        while (!list.empty()) {
            delete_component(list.front()->get_id());
        }
    }
    template<typename T>
    bool ECS::has_component(Entity entity) const {
        if (!does_entity_exist(entity)) return false;
        if (!m_components_map.find(entity)->second.contains<std::vector<T*>>()) return false;
        return !m_components_map.find(entity)->second.find<std::vector<T*>>().empty();
    }
    template<typename T>
    std::vector<T*> ECS::get_component_list() const {
        if (!m_component_lists.contains<std::vector<T*>>()) return std::vector<T*>();
        return m_component_lists.find<std::vector<T*>>();
    }
    template<typename T>
    std::vector<T*> ECS::get_component_list(Entity entity) const {
        if (!does_entity_exist(entity)) return std::vector<T*>();
        else if (!m_components_map.find(entity)->second.contains<std::vector<T*>>()) return std::vector<T*>();
        return m_components_map.find(entity)->second.find<std::vector<T*>>();
    }
    template<typename T>
    utils::Option<T&> ECS::get_component(Entity entity) const {
        if (!has_component<T>(entity)) return utils::Option<T&>::none();
        return utils::Option<T&>::some(*m_components_map.find(entity)->second.find<std::vector<T*>>().front());
    }
    template<typename T>
    void ECS::broadcast(const Message& message) {
        for (auto& cmpn : m_component_lists.get<std::vector<T*>>()) {
            cmpn->receive(message);
        }
    }
    template<typename T>
    utils::Option<Message> ECS::shout(const Message& request) {
        for (auto& cmpn : m_component_lists.get<std::vector<T*>>()) {
            auto response = cmpn->respond(request);
            if (response.is_some()) return response;
        }
        return utils::Option<Message>::none();
    }
}

#endif // NTA_ECS_H_INCLUDED