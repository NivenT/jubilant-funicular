#include "nta/ECS.h"

namespace nta {
	void Component::send(const Message& message, ComponentListID recipients) {
		m_system.broadcast(message, recipients);
	}

	ECS::ECS() {
		memset(m_component_lists, 0, sizeof(m_component_lists));
	}
	ECS::~ECS() {
		clear();
	}
	EntityID ECS::gen_entity() {
		EntityID ret = m_entity_gen();
		m_entity_set.insert(Entity(ret));
		m_components_map[ret] = nullptr;
		return ret;
	}
	void ECS::gen_entities(std::size_t num, EntityID* ids) {
		for (std::size_t i = 0; i < num; i++) ids[i] = gen_entity();
	}
	bool ECS::delete_entity(EntityID id) {
		if (m_entity_set.find(id) == m_entity_set.end()) return false;
		m_entity_set.erase(id);
		m_entity_gen.free(id);
		while (m_components_map[id]) {
			delete_component(m_components_map[id]->comp);
		}
		return true;
	}
	bool ECS::add_component(Component* cmpn, EntityID entity) {
		if (!cmpn || m_entity_set.find(entity) == m_entity_set.end()) return false;
		for (ComponentListID list = 1; list; list <<= 1) {
			if (cmpn->type & list) {
				int idx = __builtin_ctz(list);
				m_component_lists[idx] = new ComponentNode(cmpn, m_component_lists[idx]);
			}
		}
		m_components_map[entity] = new ComponentNode(cmpn, m_components_map[entity]);
		m_entity_map[cmpn] = entity;
		m_component_set.insert(cmpn);
		return true;
	}
	bool ECS::delete_component(Component* cmpn) {
		if (m_component_set.find(cmpn) == m_component_set.end()) return false;
		auto entity = m_entity_map[cmpn];
		m_entity_map.erase(cmpn);
		ComponentNode::remove(&m_components_map[entity], cmpn);
		for (ComponentListID list = 1; list; list <<= 1) {
			if (cmpn->type & list) {
				int idx = __builtin_ctz(list);
				ComponentNode::remove(&m_component_lists[idx], cmpn);
			}
		}
		m_component_set.erase(cmpn);
		return true;
	}
	ComponentNode* ECS::get_component_list(ComponentListID id) {
		return __builtin_popcount(id) == 1 ? m_component_lists[__builtin_ctz(id)] : nullptr;
	}
	void ECS::broadcast(const Message& message, ComponentListID recipients) {
	}
	void ECS::clear() {
		for (auto& entity : m_entity_set) {
			delete_entity(entity);
		}
		m_entity_gen.clear();
		memset(m_component_lists, 0, sizeof(m_component_lists));
	}
}