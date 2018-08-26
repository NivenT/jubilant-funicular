#ifndef NTA_ENTITY_H_INCLUDED
#define NTA_ENTITY_H_INCLUDED

#include "nta/MyEngine.h"

namespace nta {
	class ECS;
	typedef uint64_t EntityID;
	/// A generic game entity (basically a wrapper around a *unique* id)
	class Entity {
	private:
		Entity(EntityID id) : m_id(id) {}

		/// The unique id identifying this Entity
		EntityID m_id;
	public:
		/// Returns the id of this Entity
		EntityID get_id() const { return m_id; }
		bool operator==(const Entity rhs) const { return m_id == rhs.m_id; }

		friend ECS;
	};
}

namespace std {
	template<>
	struct hash<nta::Entity> {
		std::size_t operator()(const nta::Entity& e) const {
			return hash<nta::EntityID>()(e.get_id());
		}
	};
}

#endif // NTA_ENTITY_H_INCLUDED