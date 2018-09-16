#ifndef NTA_COMPONENT_H_INCLUDED
#define NTA_COMPONENT_H_INCLUDED

#include "nta/Message.h"

namespace nta {
	class ECS;
	/// Base class for components
	class Component {
	protected:
		/// The System this Component belongs to
		ECS* m_system;
	public:
		Component() : m_system(nullptr) {}
		virtual ~Component() {}
		/// Allows for communication between components
		virtual void receive(const Message& message) = 0;
		/// Sends message to all components associated to the same Entity
		virtual void send(const Message& message);

		/// \todo Replace with SetSystemKey
		friend ECS;
	};
}

#endif // NTA_COMPONENT_H_INCLUDED