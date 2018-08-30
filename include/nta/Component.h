#ifndef NTA_COMPONENT_H_INCLUDED
#define NTA_COMPONENT_H_INCLUDED

#include "nta/Message.h"

namespace nta {
	class ECS;
	typedef uint64_t ComponentListID;
	/// Base class for components
	class Component {
	private:
		/// The System this Component belongs to
		ECS* m_system;
	public:
		Component(ComponentListID type) : m_system(nullptr), type(type) {}
		virtual ~Component() {}
		/// Allows for communication between components
		virtual void receive(const Message& message) = 0;
		/// Sends message to all components associated to the same Entity
		virtual void send(const Message& message);

		/// A bitmask corresponding to the Lists (in m_container) that this Component belongs to
		const ComponentListID type;
		/// \todo Replace with SetSystemKey
		friend ECS;
	};
}

#endif // NTA_COMPONENT_H_INCLUDED