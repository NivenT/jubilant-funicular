#include "nta/ECS.h"

namespace nta {
	void Component::send(const Message& message, ComponentListID recipients) {
		m_container.broadcast(message, recipients);
	}

	ECS::ECS() {
		memset(m_component_lists, 0, sizeof(m_component_lists));
	}
	ECS::~ECS() {
		/// \todo Implement
	}
	EntityID ECS::gen_entity() {
		EntityID ret = m_entity_gen();
		m_entity_set.insert(Entity(ret));
		return ret;
	}
	void ECS::gen_entities(std::size_t num, EntityID* ids) {
		for (std::size_t i = 0; i < num; i++) ids[i] = gen_entity();
	}
	bool ECS::delete_entity(EntityID id) {
		if (m_entity_set.find(id) == m_entity_set.end()) return false;
		m_entity_set.erase(id);
		m_entity_gen.free(id);
		/// \todo remove components owned by entity
		return true;
	}
	ComponentNode* ECS::get_component_list(ComponentListID id) {
		// Helpful error messages are helpful
		assert(__builtin_popcount(id) != 1 && "id should be a power of 2");
		int log = 0;
		while (id) {
			log++;
			id >>= 1;
		}
		return m_component_lists[log];
	}
	void ECS::broadcast(const Message& message, ComponentListID recipients) {
	}
}