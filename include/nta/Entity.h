#ifndef NTA_ENTITY_H_INCLUDED
#define NTA_ENTITY_H_INCLUDED

#include "nta/MyEngine.h"
#include "nta/Wrapper.h"

namespace nta {
	class ECS;
	typedef uint64_t EntityID;
	/// A generic game entity (basically a wrapper around a *unique* id)
	class Entity {
	private:
		Entity(EntityID id) : id(id) {}
	public:
		bool operator==(const Entity rhs) const { return id == rhs.id; }

		operator EntityID() const { return id; }

		// The unique id identifying this Entity
		const EntityID id;
		friend ECS;
	};
}

namespace std {
	template<>
	struct hash<nta::Entity> {
		std::size_t operator()(const nta::Entity& e) const {
			return hash<nta::EntityID>()(e.id);
		}
	};
}

#endif // NTA_ENTITY_H_INCLUDED