#ifndef NTA_COMPONENT_H_INCLUDED
#define NTA_COMPONENT_H_INCLUDED

#include "nta/Message.h"
#include "nta/Wrapper.h"

namespace nta {
	class ECS;
	NTA_CREATE_WRAPPER(ComponentID, uint64_t)
	/// Base class for components
	class Component {
	protected:
		/// Create using ECS::add_component
		Component() : m_system(nullptr) {}
		/// The System this Component belongs to
		ECS* m_system;
		/// Unique identifier
		ComponentID m_id;
	public:
		virtual ~Component() {}
		/// Allows for communication between components
		virtual void receive(const Message& message) = 0;
		/// Sends message to all components associated to the same Entity
		virtual void send(const Message& message);
		/// Returns this Component's id
		const ComponentID get_id() const { return m_id; }

		friend ECS;
	};
}

namespace std {
	NTA_HASH_WRAPPER(nta::ComponentID)
}

#endif // NTA_COMPONENT_H_INCLUDED