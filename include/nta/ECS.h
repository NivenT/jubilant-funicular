#ifndef NTA_ECS_H_INCLUDED
#define NTA_ECS_H_INCLUDED

#include <unordered_set>
#include <unordered_map>
#include <vector>

#include "nta/Event.h"
#include "nta/Option.h"
#include "nta/IDFactory.h"
#include "nta/TypeMap.h"
#include "nta/SlotMap.h"

namespace nta {
    class ECS;

    // Use signed integer type 
    using Entity = utils::SlotMapKey<>;
    using ComponentID = utils::SlotMapKey<>;
    template<typename T>
    using ComponentList = utils::SlotMap<T>;

    /// Base class for components
    class Component {
    protected:
        /// Create using ECS::add_component
        Component() : m_ecs(nullptr) {}
        /// The System this Component belongs to
        ECS* m_ecs;
        /// Unique identifier
        ComponentID m_id;
    public:
        virtual ~Component() {}
        /*
        /// Receives a message from another component
        virtual void receive(const Message& message) {};
        /// (Possibly) responds to a request from another component
        virtual utils::Option<Message> respond(const Message& request) { return utils::Option<Message>::none(); };
        /// Sends message to all components associated to the same Entity
        virtual void send(const Message& message);
        /// Sends a request to all sibling components, returning the first
        /// received response.
        virtual utils::Option<Message> request(const Message& request);
        */
        /// Returns this Component's id
        const ComponentID get_id() const { return m_id; }

        friend ECS;
    };
    /// A store of every Component type in use by an ECS
    class ComponentRegistry {
    public:
        /// \todo Reduce the number of functions this needs to store
        struct Record {
        private:
            Record() {}
        public:
            template<typename T>
            static const Record get() {
                Record ret;
                
                ret.create_list = [](utils::TypeMap& map) {
                    map.insert<ComponentList<T>>(ComponentList<T>());
                };
                ret.delete_entity = [](const utils::TypeMap& map, utils::IDFactory<ComponentID>& cmpn_gen, Entity e) {
                    if (!map.contains<ComponentList<T>>()) return;
                    auto& list = map.find<ComponentList<T>>();
                    list[e].map([&](T& cmpn) {
                        cmpn_gen.free(cmpn.get_id());
                    });
                    list.remove(e);
                };
                ret.delete_component = [](const utils::TypeMap& map, ComponentID c, Entity e) -> bool {
                    if (!map.contains<ComponentList<T>>()) return false;
                    auto& list = map.find<ComponentList<T>>();
                    list.remove(e);
                    return true;
                };
                ret.get_component = [](const utils::TypeMap& map, Entity e) -> utils::Option<Component&> {
                    if (!map.contains<ComponentList<T>>()) return utils::Option<Component&>::none();
                    auto& list = map.find<ComponentList<T>>();
                    return list[e];
                };
                ret.clear = [](const utils::TypeMap& map) {
                    if (!map.contains<ComponentList<T>>()) return false;
                    map.find<ComponentList<T>>().clear();
                };
                /*
                ret.broadcast = [](const utils::TypeMap& map, Entity e, const Message& msg) {
                    if (!map.contains<ComponentList<T>>()) return;
                    auto& list = map.find<ComponentList<T>>();
                    if (e >= list.size()) return;
                    auto node = list[(std::size_t)e];
                    while (node) {
                        node->data.receive(msg);
                        node = node->next;
                    }
                };
                ret.shout = [](const utils::TypeMap& map, Entity e, const Message& req) {
                    if (!map.contains<ComponentList<T>>()) return utils::Option<Message>::none();
                    auto& list = map.find<ComponentList<T>>();
                    if (e >= list.size()) return utils::Option<Message>::none();
                    auto node = list[(std::size_t)e];
                    while (node) {
                        auto resp = node->data.respond(req);
                        if (resp) return resp;
                        node = node->next;
                    }
                    return utils::Option<Message>::none();
                };
                */
                return ret;
            }

            std::function<void(utils::TypeMap&)> create_list;
            std::function<void(const utils::TypeMap&, utils::IDFactory<ComponentID>&, Entity)> delete_entity;
            std::function<bool(const utils::TypeMap&, ComponentID, Entity)> delete_component;
            std::function<utils::Option<Component&>(const utils::TypeMap&, Entity)> get_component;
            std::function<void(const utils::TypeMap&)> clear;
            /*
            std::function<void(const utils::TypeMap&, Entity, const Message&)> broadcast;
            std::function<utils::Option<Message>(const utils::TypeMap&, Entity, const Message&)> shout;
            */
        };

        using iterator = std::unordered_map<std::size_t, Record>::iterator;
        using const_iterator = std::unordered_map<std::size_t, Record>::const_iterator;
    private:
        std::unordered_map<std::size_t, Record> m_records;
    public:
        ComponentRegistry() {}
        ~ComponentRegistry() {}

        template<typename T>
        void register_component() { m_records.insert(std::make_pair(typeid(T).hash_code(), Record::get<T>())); }
        template<typename T>
        utils::Option<Record> get_record() const { return operator[](typeid(T).hash_code()); }
        utils::Option<Record> operator[](std::size_t hash) const {
            auto rec = m_records.find(hash);
            return rec == m_records.end() ? utils::Option<Record>::none() : utils::Option<Record>::some(rec->second);
        };

        iterator begin() { return m_records.begin(); }
        const_iterator begin() const { return m_records.cbegin(); }
        const_iterator cbegin() const { return m_records.cbegin(); }
        iterator end() { return m_records.end(); }
        const_iterator end() const { return m_records.cend(); }
        const_iterator cend() const { return m_records.cend(); }
    };
    /// Container class representing a complete Entity-Component System
    ///
    /// Manages creation of Entities and grouping of Components.
    ///
    /// Assumes each entity can only have one Component of a given type.
    ///
    /// \todo Make this class's implementation less complicated
    class ECS {
    private:
        /// Info directly attached to a ComponentID
        struct ComponentInfo {
            std::size_t type;
            Entity owner;
        };

        /// All the components in this system
        ///
        /// Only stores types of the form ComponentList<T>
        /// where T inherits Component. Each vector has size
        /// m_entity_gen.get_last_id().
        utils::TypeMap m_components;
        /*
        /// A map from ComponentID to type of Component with that ID
        ///
        /// Since ComponentIDs are unique, I could get rid of this and just
        /// always loop over all records instead.
        utils::SlotMap<std::size_t> m_component_types;
        /// A map from ComponentID to the Entity owning that component
        utils::SlotMap<Entity> m_component_owners;
        */
        /// A map from ComponentID to relevant information about that Component
        ///
        /// Since ComponentIDs are unique, I could get rid of this and just
        /// always loop over all records instead.
        utils::SlotMap<ComponentInfo> m_component_info;
        /// Responsible for creating unique IDs for the Entities
        utils::IDFactory<Entity> m_entity_gen;
        /// Responsible for creating unique IDs for the Components
        utils::IDFactory<ComponentID> m_cmpn_gen;
        /// The various Component types in use
        ///
        /// This registry should not change after it's associated to an ECS
        const ComponentRegistry& m_registry;
    public:
        ECS(const ComponentRegistry& registry);
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
        std::size_t num_entities() const { return m_entity_gen.get_count(); }

        /// Adds the given Component to the given Entity.
        ///
        /// You pass in the entity and any arguments needed to construct a new object of type T
        ///
        /// T should be a concrete type, not a pointer type
        ///
        /// Returns the id of the created Component (or None on failure).
        template<typename T, typename... Args>
        utils::Option<ComponentID> add_component(Entity entity, Args&&... args);
        /// Attempts to delete the given Component
        ///
        /// Returns false on failure
        bool delete_component(ComponentID cmpn);

        /// Returns true if the given Entity has a Component of type T
        template<typename T>
        bool has_component(Entity entity) const;
        /// Returns true if the given Entity exists
        bool does_entity_exist(Entity entity) const;

        /// Returns the Entity associated to this Component
        utils::Option<Entity> get_owner(ComponentID cmpn) const;
        /// Returns a list of all components of the given type
        template<typename T>
        ComponentList<T> get_component_list() const;
        /// Returns the Component of the given type associated to the given Entity
        template<typename T>
        utils::Option<T&> get_component(Entity entity) const;
        /// Returns the Component with the given id
        utils::Option<Component&> get_component(ComponentID id) const;
        /// Returns the Component of the given type with the same owner as cmpn
        template<typename T>
        utils::Option<T&> get_sibling(ComponentID cmpn) const;

        /// Runs a function of each Component of a given type
        template<typename T>
        void for_each(std::function<void(T&)> func) const;
        /*
        /// Forwards message to all Components associated to the same Entity as cmpn
        void broadcast(const Message& message, ComponentID cmpn);
        /// Forwards message to all Components belonging to entity
        void broadcast(const Message& message, Entity entity);
        /// Forwards message to all Components of the given type
        template<typename T>
        void broadcast(const Message& message);

        /// Like broadcast but returns the first response received
        ///
        /// \todo Return utils::LinkedNode<Message> of all received responses
        utils::Option<Message> shout(const Message& request, ComponentID cmpn);
        utils::Option<Message> shout(const Message& request, Entity entity);
        template<typename T>
        utils::Option<Message> shout(const Message& request);
        */
        /// Removes all entites and components from this system
        void clear();
    };
    template<typename T, typename... Args>
    utils::Option<ComponentID> ECS::add_component(Entity entity, Args&&... args) {
        if (!does_entity_exist(entity)) return utils::make_none<ComponentID>();
        ComponentList<T>& list = m_components.get<ComponentList<T>>();
        if (list.size() <= entity.idx) list.reserve(entity.idx+1);

        if (!list.insert_emplace(entity, std::forward<Args>(args)...)) {
            NTA_ASSERT(false, "This should never happen 1");
        }
        T& cmpn = list[entity].unwrap();
        cmpn.m_ecs = this;
        cmpn.m_id = m_cmpn_gen();

        m_component_info.reserve(cmpn.m_id.idx+1);
        ComponentInfo info = { .type = typeid(T).hash_code(), .owner = entity };
        if (!m_component_info.insert(cmpn, info)) {
            NTA_ASSERT(false, "This should never happen");
        }
        return list[entity].m_id;
    }
    template<typename T>
    bool ECS::has_component(Entity entity) const {
        if (!does_entity_exist(entity)) return false;
        if (!m_components.contains<ComponentList<T>>()) return false;
        return m_components.find<ComponentList<T>>()[entity].is_some();
    }
    template<typename T>
    ComponentList<T> ECS::get_component_list() const {
        if (!m_components.contains<ComponentList<T>>()) return ComponentList<T>();
        return m_components.find<ComponentList<T>>();
    }
    template<typename T>
    utils::Option<T&> ECS::get_component(Entity entity) const {
        if (!has_component<T>(entity)) return utils::Option<T&>::none();
        return m_components.find<ComponentList<T>>()[entity];
    }
    template<typename T>
    void ECS::for_each(std::function<void(T&)> func) const {
        auto list = get_component_list<T>();
        for (auto& cmpn : list) {
            func(cmpn);
        }
    }
    /*
    /// \todo Make these more efficient
    template<typename T>
    void ECS::broadcast(const Message& message) {
        m_registry.get_record<T>().map([&](ComponentRegistry::Record rec) {
            for (Entity e = 0; e < m_entity_gen.get_last_id(); e++) {
                rec.broadcast(m_components, e, message);
            }
        });
    }
    template<typename T>
    utils::Option<Message> ECS::shout(const Message& request) {
        return m_registry.get_record<T>().map_or([&](ComponentRegistry::Record rec) {
            for (Entity e = 0; e < m_entity_gen.get_last_id(); e++) {
                auto resp = rec.shout(m_components, e, request);
                if (resp) return resp;
            }
        }, utils::Option<Message>::none());
    }
    */
}

#endif // NTA_ECS_H_INCLUDED
