#ifndef NTA_COMPONENT_H_INCLUDED
#define NTA_COMPONENT_H_INCLUDED

#include "nta/Message.h"

namespace nta {
	/// Base class for components
	class Component {
	private:
		/// The System this Component belongs to
		ECS& m_container;
	public:
		Component(ECS& container) : m_container(container) {}
		virtual ~Component() {}
		/// Allows for communication between components
		virtual void receive(const Message& message) = 0;
	};
}

#endif // NTA_COMPONENT_H_INCLUDED