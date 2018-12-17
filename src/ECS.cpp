#include "nta/ECS.h"

namespace nta {
	void Component::send(const Message& message) {
		m_system->broadcast(message, m_id);
	}
	utils::Option<Message> Component::request(const Message& request) {
		return m_system->shout(request, m_id);
	}

	ECS::ECS(const ComponentRegistry& registry) : m_registry(registry) {
		for (auto it = m_registry.cbegin(); it != m_registry.cend(); ++it) {
			it->second.create_list(m_components);
		}
	}
	Entity ECS::gen_entity() {
		Entity ret = m_entity_gen();
		for (auto it = m_registry.cbegin(); it != m_registry.cend(); ++it) {
			it->second.resize_list(m_components, m_entity_gen.get_last_id().to_inner()+1);
		}
		return ret;
	}
	void ECS::gen_entities(std::size_t num, Entity* ids) {
		for (std::size_t i = 0; i < num; i++) ids[i] = gen_entity();
	}
	bool ECS::delete_entity(Entity id) {
		if (m_entity_gen.is_free(id)) return false;
		m_entity_gen.free(id);
		for (auto it = m_registry.cbegin(); it != m_registry.cend(); ++it) {
			it->second.delete_entity(m_components, m_cmpn_gen, id);
		}
		return true;
	}
	ComponentID ECS::delete_component(ComponentID cmpn) {
		if (m_cmpn_gen.is_free(cmpn)) return NTA_INVALID_ID;

		m_cmpn_gen.free(cmpn);
		return m_registry[m_component_types[cmpn.to_inner()]].map_or<ComponentID>([&](ComponentRegistry::Record rec) {
			return rec.delete_component(m_components, cmpn);
		}, NTA_INVALID_ID);
	}
	bool ECS::does_entity_exist(Entity entity) const {
		return m_entity_gen.is_in_use(entity);
	}
	Entity ECS::get_owner(ComponentID cmpn) const {
		if (m_cmpn_gen.is_free(cmpn)) return NTA_INVALID_ID;
		return m_registry[m_component_types[cmpn.to_inner()]].map_or<Entity>([&](ComponentRegistry::Record rec) {
			return rec.get_owner(m_components, cmpn);
		}, NTA_INVALID_ID);
	}
	Component* ECS::get_component(ComponentID cmpn) const {
		if (m_cmpn_gen.is_free(cmpn)) return nullptr;
		return m_registry[m_component_types[cmpn.to_inner()]].map_or<Component*>([&](ComponentRegistry::Record rec) {
			return rec.get_component(m_components, cmpn);
		}, nullptr);
	}
	void ECS::broadcast(const Message& message, ComponentID cmpn) {
		if (m_cmpn_gen.is_free(cmpn)) return;
		broadcast(message, get_owner(cmpn));
	}
	void ECS::broadcast(const Message& message, Entity entity) {
		if (m_entity_gen.is_free(entity)) return;
		for (auto it = m_registry.cbegin(); it != m_registry.cend(); ++it) {
			it->second.broadcast(m_components, entity, message);
		}
	}
	utils::Option<Message> ECS::shout(const Message& request, ComponentID cmpn) {
		if (m_cmpn_gen.is_free(cmpn)) return utils::Option<Message>::none();
		return shout(request, get_owner(cmpn));
	}
	utils::Option<Message> ECS::shout(const Message& request, Entity entity) {
		if (m_entity_gen.is_free(entity)) return utils::Option<Message>::none();
		for (auto it = m_registry.cbegin(); it != m_registry.cend(); ++it) {
			auto resp = it->second.shout(m_components, entity, request);
			if (resp) return resp;
		}
		return utils::Option<Message>::none();
	}
	void ECS::clear() {
		for (std::size_t e = 0; e < m_entity_gen.get_last_id().to_inner(); e++) {
			delete_entity(e);
		}
		m_entity_gen.clear();
	}
}