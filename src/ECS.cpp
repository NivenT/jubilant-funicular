#include <algorithm>

#include "nta/ECS.h"

namespace nta {
	void Component::send(const Message& message) {
		m_system->broadcast(message, m_id);
	}

	Entity ECS::gen_entity() {
		Entity ret = m_entity_gen();
		m_entity_set.insert(ret);
		return ret;
	}
	void ECS::gen_entities(std::size_t num, Entity* ids) {
		for (std::size_t i = 0; i < num; i++) ids[i] = gen_entity();
	}
	bool ECS::delete_entity(Entity id) {
		if (m_entity_set.find(id) == m_entity_set.end()) return false;
		m_entity_set.erase(id);
		m_entity_gen.free(id);
		while (!m_components_map[id].empty()) {
			auto list = (std::vector<Component*>*)m_components_map[id].begin()->second;
			while (!list->empty()) delete_component(list->front()->get_id());
			m_components_map[id].erase(m_components_map[id].begin()->first);
		}
		return true;
	}
	ComponentID ECS::delete_component(ComponentID cmpn) {
		if (m_component_set.find(cmpn) == m_component_set.end()) return NTA_INVALID_ID;
		auto entity = m_entity_map[cmpn];
		m_entity_map.erase(cmpn);

		for (auto& info : m_list_map[cmpn]) {
			std::vector<Component*>* list = (std::vector<Component*>*)m_components_map[entity].find(info);
			assert(list != nullptr);
			auto end = std::remove(list->begin(), list->end(), m_component_set[cmpn]);
			list->resize(end - list->begin());

			list = (std::vector<Component*>*)m_component_lists.find(info);
			end = std::remove(list->begin(), list->end(), m_component_set[cmpn]);
			list->resize(end - list->begin());
		}
		m_list_map.erase(cmpn);
		delete m_component_set[cmpn];
		m_component_set.erase(cmpn);
		m_cmpn_gen.free(cmpn);
		return cmpn;
	}
	bool ECS::does_entity_exist(Entity entity) const {
		return m_entity_set.find(entity) != m_entity_set.end();
	}
	Entity ECS::get_owner(ComponentID cmpn) const {
		if (m_component_set.find(cmpn) == m_component_set.end()) return NTA_INVALID_ID;
		return m_entity_map.find(cmpn)->second;
	}
	utils::Option<ComponentLists&> ECS::get_components(Entity entity) {
		if (m_entity_set.find(entity) == m_entity_set.end()) return utils::Option<ComponentLists&>::none();
		return utils::Option<ComponentLists&>::new_some(m_components_map[entity]);
	}
	Component* ECS::get_component(ComponentID id) const {
		auto it = m_component_set.find(id);
		return it == m_component_set.end() ? nullptr : it->second;
	}
	void ECS::broadcast(const Message& message, ComponentID cmpn) {
		if (m_component_set.find(cmpn) == m_component_set.end()) return;
		broadcast(message, m_entity_map[cmpn]);
	}
	void ECS::broadcast(const Message& message, Entity entity) {
		std::unordered_set<Component*> seen;
		auto& lists = m_components_map[entity];
		for (auto& pair: lists) {
			auto& list = *(std::vector<Component*>*)pair.second;
			for (Component* cmpn : list) {
				if (seen.find(cmpn) == seen.end()) {
					cmpn->receive(message);
					seen.insert(cmpn);
				}
			}
		}
	}
	void ECS::clear() {
		while (!m_entity_set.empty()) {
			delete_entity(*m_entity_set.begin());
		}
		m_entity_gen.clear();
	}
}