#ifndef NTA_ECS_H_INCLUDED
#define NTA_ECS_H_INCLUDED

#include <unordered_set>
#include <unordered_map>
#include <vector>

#include "nta/Option.h"
#include "nta/IDFactory.h"
#include "nta/TypeMap.h"
#include "nta/SlotMap.h"
#include "nta/Event.h"

namespace nta {
    class ECS;

    using Entity = utils::SlotMapKey<>;
    using ComponentID = utils::SlotMapKey<>;
    template<typename T>
    using ComponentList = utils::SlotMap<T>;

    /// Base class for components
    class Component {
    protected:
        /// Create using ECS::add_component
        Component() {}
        /// Unique identifier
        ComponentID m_id;
    public:
        virtual ~Component() {}
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
                    list.deactivate(e);
                    return true;
                };
                ret.get_component = [](const utils::TypeMap& map, Entity e) -> utils::Option<Component&> {
                    if (!map.contains<ComponentList<T>>()) return utils::Option<Component&>::none();
                    auto& list = map.find<ComponentList<T>>();
                    return list[e];
                };
                ret.clear = [](const utils::TypeMap& map) {
                    if (!map.contains<ComponentList<T>>()) return;
                    map.find<ComponentList<T>>().clear();
                };
                return ret;
            }

            std::function<void(utils::TypeMap&)> create_list;
            std::function<void(const utils::TypeMap&, utils::IDFactory<ComponentID>&, Entity)> delete_entity;
            std::function<bool(const utils::TypeMap&, ComponentID, Entity)> delete_component;
            std::function<utils::Option<Component&>(const utils::TypeMap&, Entity)> get_component;
            std::function<void(const utils::TypeMap&)> clear;
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

        std::size_t size() const { return m_records.size(); }

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
    /// The Event template parameter used in some functions should usually be 
    /// an instantiation of EventTemplate. It is used for passing 
    /// around events/signals/commands, i.e. it should be invokable on components
    /// ```
    /// Event f;
    /// Component c;
    /// ...
    /// f(c);
    /// ```
    ///
    /// If you don't need events, leave this parameter unspecified.
    // I really should have just called this thing EntityManager
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
        bool delete_owner(ComponentID cmpn);
        /// Returns the number of entities in the system
        std::size_t num_entities() const { return m_entity_gen.get_count(); }
        /// Returns the number of components of the given type
        template<typename T>
        std::size_t num_components() const { return get_component_list<T>().size(); }

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
        template<typename T>
        bool has_sibling(ComponentID cmpn) const;
        /// Returns true if the given Entity exists
        bool does_entity_exist(Entity entity) const;

        /// Returns the Entity associated to this Component
        utils::Option<Entity> get_owner(ComponentID cmpn) const;
        /// Returns a list of all components of the given type
        template<typename T>
        ComponentList<T>& get_component_list() const;
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

        /// Enacts the event on the Component of the given type owned by the given Entity
        template<typename T, typename Event>
        void enact_on(const Event& event, Entity entity) const;
        template<typename T, typename Event, typename Event::enum_type e>
        void enact_on(const Event& event, Entity entity) const;
        template<typename T, typename Event>
        void enact_on(const Event& event, typename Event::enum_type e, Entity entity) const;
        /// Enacts the event on the Component of the given type owned by the same entity
        template<typename T, typename Event>
        void enact_on_sibling(const Event& event, ComponentID cmpn) const;
        template<typename T, typename Event, typename Event::enum_type e>
        void enact_on_sibling(const Event& event, ComponentID cmpn) const;
        template<typename T, typename Event>
        void enact_on_sibling(const Event& event, typename Event::enum_type e, ComponentID cmpn) const;
        /// Enacts the event on all the Components of the given type
        template<typename T, typename Event>
        void enact_on_all(const Event& event, typename Event::enum_type e) const;
        template<typename T, typename Event, typename Event::enum_type e>
        void enact_on_all(const Event& event) const;

        /// Removes all entities and components from this system
        void clear();
    };
    template<typename T, typename... Args>
    utils::Option<ComponentID> ECS::add_component(Entity entity, Args&&... args) {
        if (!does_entity_exist(entity)) return utils::make_none<ComponentID>();
        ComponentList<T>& list = m_components.find<ComponentList<T>>();
        list.reserve(entity.idx+1);

        if (!list.insert_emplace(entity, std::forward<Args>(args)...)) {
            return utils::make_none<ComponentID>();
        }
        T& cmpn = list[entity].unwrap();
        //cmpn.m_ecs = this;
        cmpn.m_id = m_cmpn_gen();

        m_component_info.reserve(cmpn.m_id.idx+1);
        ComponentInfo info = { .type = typeid(T).hash_code(), .owner = entity };
        if (!m_component_info.insert(cmpn.m_id, info)) {
            assert(false && "This should never happen");
        }
        return utils::make_some(cmpn.m_id);
    }
    template<typename T>
    bool ECS::has_component(Entity entity) const {
        if (!does_entity_exist(entity)) return false;
        if (!m_components.contains<ComponentList<T>>()) return false;
        return m_components.find<ComponentList<T>>()[entity].is_some();
    }
    template<typename T>
    bool ECS::has_sibling(ComponentID cmpn) const {
        auto maybe_owner = get_owner(cmpn);
        return maybe_owner ? has_component<T>(maybe_owner.unwrap()) : false;
    }
    template<typename T>
    ComponentList<T>& ECS::get_component_list() const {
        if (!m_components.contains<ComponentList<T>>()) {
            assert(false && "Attempted to get a ComponentList for a non-registered type");
        }
        return m_components.find<ComponentList<T>>();
    }
    template<typename T>
    utils::Option<T&> ECS::get_component(Entity entity) const {
        if (!has_component<T>(entity)) return utils::Option<T&>::none();
        return m_components.find<ComponentList<T>>()[entity];
    }
    template<typename T>
    utils::Option<T&> ECS::get_sibling(ComponentID cmpn) const {
        return get_owner(cmpn).map<T&>([&](Entity e) { return get_component<T>(e); });
    }
    template<typename T>
    void ECS::for_each(std::function<void(T&)> func) const {
        auto& list = get_component_list<T>();
        for (auto& cmpn : list) {
            func(cmpn);
        }
    }
    template<typename T, typename Event>
    void ECS::enact_on(const Event& event, Entity entity) const {
        get_component<T>(entity).map([&](T& cmpn) { event(cmpn); });
    }
    template<typename T, typename Event, typename Event::enum_type e>
    void ECS::enact_on(const Event& event, Entity entity) const {
        get_component<T>(entity).map([&](T& cmpn) { event.enact<e>(cmpn); });
    }
    template<typename T, typename Event>
    void ECS::enact_on(const Event& event, typename Event::enum_type e, Entity entity) const {
        get_component<T>(entity).map([&](T& cmpn) { event(e, cmpn); });
    }
    template<typename T, typename Event>
    void ECS::enact_on_sibling(const Event& event, ComponentID cmpn) const {
        get_sibling<T>(cmpn).map([&](T& sib) { event(sib); });
    }
    template<typename T, typename Event, typename Event::enum_type e>
    void ECS::enact_on_sibling(const Event& event, ComponentID cmpn) const {
        get_sibling<T>(cmpn).map([&](T& sib) { event.enact<e>(sib); });
    }
    template<typename T, typename Event>
    void ECS::enact_on_sibling(const Event& event, typename Event::enum_type e, ComponentID cmpn) const {
        get_sibling<T>(cmpn).map([&](T& sib) { event(e, sib); });
    }
    template<typename T, typename Event>
    void ECS::enact_on_all(const Event& event, typename Event::enum_type e) const {
        for (auto& cmpn : get_component_list<T>()) {
            event(e, cmpn);
        }
    }
    template<typename T, typename Event, typename Event::enum_type e>
    void ECS::enact_on_all(const Event& event) const {
        for (auto& cmpn : get_component_list<T>()) {
            event.enact<e>(cmpn);
        }
    }
}

#endif // NTA_ECS_H_INCLUDED
