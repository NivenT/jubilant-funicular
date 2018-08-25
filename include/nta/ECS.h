#ifndef NTA_ECS_H_INCLUDED
#define NTA_ECS_H_INCLUDED

#include "nta/Message.h"

namespace nta {
	typedef uint64_t EntityID;
	/// A generic game entity (basically a wrapper around a *unique* id)
	class Entity {
	private:
		/// The unique id identifying this Entity
		EntityID m_id;
	public:
		Entity(EntityID id) : m_id(id) {}
		bool operator==(const Entity rhs) const { return m_id == rhs.m_id; }
	};
}

#endif // NTA_ECS_H_INCLUDED