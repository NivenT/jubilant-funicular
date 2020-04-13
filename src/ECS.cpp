#include "nta/ECS.h"

namespace nta {
	/*
	void Component::send(const Message& message) {
		m_system->broadcast(message, m_id);
	}
	utils::Option<Message> Component::request(const Message& request) {
		return m_system->shout(request, m_id);
	}
	*/
	ECS::ECS(const ComponentRegistry& registry) : m_registry(registry) {
		for (auto it = m_registry.cbegin(); it != m_registry.cend(); ++it) {
			it->second.create_list(m_components);
		}
	}
	Entity ECS::gen_entity() {
		Entity ret = m_entity_gen();
		/*
		for (auto it = m_registry.cbegin(); it != m_registry.cend(); ++it) {
			it->second.resize_list(m_components, m_entity_gen.get_last_id().to_inner()+1);
		}
		*/
		return ret;
	}
	void ECS::gen_entities(std::size_t num, Entity* ids) {
		for (std::size_t i = 0; i < num; i++) ids[i] = gen_entity();
	}
	bool ECS::delete_entity(Entity id) {
		if (!does_entity_exist(id)) return false;
		m_entity_gen.free(id);
		for (auto it = m_registry.cbegin(); it != m_registry.cend(); ++it) {
			it->second.delete_entity(m_components, m_cmpn_gen, id);
		}
		return true;
	}
	bool ECS::delete_component(ComponentID cmpn) {
		if (m_cmpn_gen.is_free(cmpn)) return false;

		m_cmpn_gen.free(cmpn);

		auto maybe_info = m_component_info[cmpn];
		if (maybe_info.is_none()) {
			NTA_ASSERT(false, "This should never happen");
		}
		ComponentInfo info = maybe_info.unwrap();
		m_component_info.remove(cmpn);

		return m_registry[info.type].map_or<bool>([&](ComponentRegistry::Record rec) {
			return rec.delete_component(m_components, cmpn, info.owner);
		}, false);
	}
	bool ECS::does_entity_exist(Entity entity) const {
		return m_entity_gen.is_in_use(entity);
	}
	utils::Option<Entity> ECS::get_owner(ComponentID cmpn) const {
		return m_component_info[cmpn].map<Entity>([](ComponentInfo& info) { return info.owner; });
	}
	utils::Option<Component&> ECS::get_component(ComponentID cmpn) const {
		auto maybe_info = m_component_info[cmpn];
		if (maybe_info.is_none()) return utils::make_none<Component&>();
		auto info = maybe_info.unwrap();
		return m_registry[info.type].map_or<utils::Option<Component&>>([&](ComponentRegistry::Record rec) {
			return rec.get_component(m_components, info.owner);
		}, utils::make_none<Component&>());
	}
	/*
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
	*/
	void ECS::clear() {
		for (auto& record : m_registry) {
			record.second.clear(m_components);
		}
		m_component_info.clear();
		m_entity_gen.clear();
		m_cmpn_gen.clear();
	}
}