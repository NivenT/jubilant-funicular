#ifndef NTA_ECS_H_INCLUDED
#define NTA_ECS_H_INCLUDED

#include <unordered_set>
#include <unordered_map>
#include <vector>

#include "nta/LinkedNode.h"
#include "nta/Component.h"
#include "nta/IDFactory.h"
#include "nta/Wrapper.h"
#include "nta/TypeMap.h"

namespace nta {
    typedef utils::Wrapper<std::size_t, struct EntityTag> Entity;
    template<typename T>
    using ComponentList = std::vector<utils::LinkedNode<T>*>;
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
                    if (e >= list.size()) return;
                    auto node = list[(std::size_t)e];
                    while (node) {
                        cmpn_gen.free(node->data.get_id());
                        node = node->next;
                    }
                    if (list[(std::size_t)e]) delete list[(std::size_t)e];
                    list[(std::size_t)e] = nullptr;
                };
                ret.delete_component = [](const utils::TypeMap& map, ComponentID c) -> ComponentID {
                    if (!map.contains<ComponentList<T>>()) return NTA_INVALID_ID;
                    auto& list = map.find<ComponentList<T>>();
                    for (std::size_t i = 0; i < list.size(); i++) {
                        bool succ = utils::LinkedNode<T>::remove(list[i], [c](const T& rhs) {
                            return rhs.get_id() == c;
                        });
                        if (succ) return c;
                    }
                    return NTA_INVALID_ID;
                };
                ret.resize_list = [](const utils::TypeMap& map, std::size_t size) {
                    if (!map.contains<ComponentList<T>>()) return;
                    map.find<ComponentList<T>>().resize(size);
                };
                ret.get_owner = [](const utils::TypeMap& map, ComponentID c) -> Entity {
                    if (!map.contains<ComponentList<T>>()) return NTA_INVALID_ID;
                    auto& list = map.find<ComponentList<T>>();
                    for (Entity e = 0; e < list.size(); e++) {
                        auto node = list[(std::size_t)e];
                        while (node) {
                            if (node->data.get_id() == c) return e;
                            node = node->next;
                        }
                    }
                    return NTA_INVALID_ID;
                };
                ret.get_component = [](const utils::TypeMap& map, ComponentID c) -> Component* {
                    if (!map.contains<ComponentList<T>>()) return nullptr;
                    auto& list = map.find<ComponentList<T>>();
                    for (auto node : list) {
                        while (node) {
                            if (node->data.get_id() == c) return &node->data;
                            node = node->next;
                        }
                    }
                    return nullptr;
                };
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
                return ret;
            }

            std::function<void(utils::TypeMap&)> create_list;
            std::function<void(const utils::TypeMap&, utils::IDFactory<ComponentID>&, Entity)> delete_entity;
            std::function<ComponentID(const utils::TypeMap&, ComponentID)> delete_component;
            std::function<void(const utils::TypeMap&, std::size_t)> resize_list;
            std::function<Entity(const utils::TypeMap&, ComponentID)> get_owner;
            std::function<Component*(const utils::TypeMap&, ComponentID)> get_component;
            std::function<void(const utils::TypeMap&, Entity, const Message&)> broadcast;
            std::function<utils::Option<Message>(const utils::TypeMap&, Entity, const Message&)> shout;
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
        const_iterator cbegin() const { return m_records.cbegin(); }
        iterator end() { return m_records.end(); }
        const_iterator cend() const { return m_records.cend(); }
    };
    /// Container class representing a complete Entity-Component System
    ///
    /// Manages creation of Entities and grouping of Components
    ///
    /// \todo Make this class's implementation less complicated
    class ECS {
    private:
        /// All the components in this system
        ///
        /// Only stores types of the form ComponentList<T>
        /// where T inherits Component. Each vector has size
        /// m_entity_gen.get_last_id().
        utils::TypeMap m_components;
        /// A map from ComponentID to type of Component with that ID
        ///
        /// Since ComponentIDs are unique, I could get rid of this and just
        /// always loop over all records instead.
        std::vector<std::size_t> m_component_types;
        /// Responsible for creating unique IDs for the Entities
        utils::IDFactory<Entity> m_entity_gen;
        /// Responsible for creating unique IDs for the Components
        utils::IDFactory<ComponentID> m_cmpn_gen;
        /// The various Component types in use
        ///
        /// This registry should not change after its associated to an ECS
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
        /// Returns the id of the created Component (NTA_INVALID_ID on failure).
        template<typename T, typename... Args>
        ComponentID add_component(Entity entity, Args&&... args);
        /// Attempts to delete the given Component
        ///
        /// Returns the ID of the deleted Component
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
        ///
        /// This is a vector of linked lists where the ith entry of the vector
        /// is the list of components associated to entity i
        template<typename T>
        ComponentList<T> get_component_list() const;
        /// Returns a vector of all components of the given type without
        /// information on which component belongs to which entity
        template<typename T>
        std::vector<T*> get_flat_component_list() const;
        /// Returns a list of all components of the given type associated to this entity
        template<typename T>
        utils::LinkedNode<T>* get_component_list(Entity entity) const;
        // This feels very stupid
        template<typename T>
        std::vector<T*> get_flat_component_list(Entity entity) const;
        /// Returns the (first) Component of the given type associated to the given Entity
        template<typename T>
        utils::Option<T&> get_component(Entity entity) const;
        /// Returns the Component with the given id
        Component* get_component(ComponentID id) const;
        /// Returns the (first) Component of the given type with the same owner as cmpn
        template<typename T>
        utils::Option<T&> get_sibling(ComponentID cmpn) const { return get_component<T>(get_owner(cmpn)); }

        /// Runs a function of each Component of a given type
        template<typename T>
        void for_each(std::function<void(T&)> func) const;
        /// Returns a list of all compoonents of a given type satisfying some predicate
        template<typename T>
        std::vector<T*> filter(std::function<bool(T&)> pred) const;

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

        /// Removes all entites and components from this system
        void clear();
    };
    template<typename T, typename... Args>
    ComponentID ECS::add_component(Entity entity, Args&&... args) {
        if (m_entity_gen.is_free(entity)) return NTA_INVALID_ID;
        ComponentList<T>& list = m_components.get<ComponentList<T>>();
        list[(std::size_t)entity] = new utils::LinkedNode<T>(T(std::forward<Args>(args)...),
                                                list[(std::size_t)entity]);
        list[(std::size_t)entity]->data.m_system = this;
        list[(std::size_t)entity]->data.m_id = m_cmpn_gen();

        m_component_types.resize(m_cmpn_gen.get_last_id().to_inner()+1);
        m_component_types[(std::size_t)list[(std::size_t)entity]->data.m_id] = typeid(T).hash_code();
        return list[(std::size_t)entity]->data.m_id;
    }
    template<typename T>
    void ECS::delete_components(Entity entity) {
        if (!does_entity_exist(entity)) return;
        m_registry.get_record<T>().map([&](ComponentRegistry::Record rec) {
            rec.delete_entity(m_components, m_cmpn_gen, entity);
        });
    }
    template<typename T>
    bool ECS::has_component(Entity entity) const {
        if (!does_entity_exist(entity)) return false;
        if (!m_components.contains<ComponentList<T>>()) return false;
        return m_components.find<ComponentList<T>>()[(std::size_t)entity] != nullptr;
    }
    template<typename T>
    ComponentList<T> ECS::get_component_list() const {
        if (!m_components.contains<ComponentList<T>>()) return ComponentList<T>();
        return m_components.find<ComponentList<T>>();
    }
    template<typename T>
    std::vector<T*> ECS::get_flat_component_list() const {
        auto list = get_component_list<T>();
        std::vector<T*> flat_list;
        for (auto node : list) {
            while (node) {
                flat_list.push_back(&node->data);
                node = node->next;
            }
        }
        return flat_list;
    }
    template<typename T>
    utils::LinkedNode<T>* ECS::get_component_list(Entity entity) const {
        if (!does_entity_exist(entity)) return nullptr;
        else if (!m_components.contains<ComponentList<T>>()) return nullptr;
        return m_components.find<ComponentList<T>>()[(std::size_t)entity];
    }
    template<typename T>
    std::vector<T*> ECS::get_flat_component_list(Entity entity) const {
        auto list = get_component_list<T>(entity);
        std::vector<T*> flat_list;
        for (auto& cmpn : *list) {
            flat_list.push_back(&cmpn);
        }
        return flat_list;
    }
    template<typename T>
    utils::Option<T&> ECS::get_component(Entity entity) const {
        if (!has_component<T>(entity)) return utils::Option<T&>::none();
        return utils::Option<T&>::some(m_components.find<ComponentList<T>>()[(std::size_t)entity]->data);
    }
    template<typename T>
    void ECS::for_each(std::function<void(T&)> func) const {
        auto list = get_component_list<T>();
        for (auto node : list) {
            while (node) {
                func(node->data);
                node = node->next;
            }
        }
    }
    template<typename T>
    std::vector<T*> ECS::filter(std::function<bool(T&)> pred) const {
        auto list = get_component_list<T>();
        std::vector<T*> flat_list;
        for (auto node : list) {
            while (node) {
                if (pred(node->data)) flat_list.push_back(&node->data);
                node = node->next;
            }
        }
        return flat_list;
    }
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
}

#endif // NTA_ECS_H_INCLUDED
