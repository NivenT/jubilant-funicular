#ifndef NTA_COMPONENT_H_INCLUDED
#define NTA_COMPONENT_H_INCLUDED

#include "nta/Message.h"
#include "nta/Wrapper.h"
#include "nta/Option.h"

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
		/// Receives a message from another component
		virtual void receive(const Message& message) {};
		/// (Possibly) responds to a request from another component
		virtual utils::Option<Message> respond(const Message& request) { return utils::Option<Message>::none(); };
		/// Sends message to all components associated to the same Entity
		virtual void send(const Message& message);
		/// Sends a request to all sibling components, returning the first
		/// received response.
		virtual utils::Option<Message> request(const Message& request);
		/// Returns this Component's id
		const ComponentID get_id() const { return m_id; }

		friend ECS;
	};
}

namespace std {
	NTA_HASH_WRAPPER(nta::ComponentID)
}

#endif // NTA_COMPONENT_H_INCLUDED