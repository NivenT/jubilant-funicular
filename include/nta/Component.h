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
		ECS& m_system;
	protected:
		/// Sends message to recipients (good descriptions are good)
		virtual void send(const Message& message, ComponentListID recipients);
	public:
		Component(ECS& system, ComponentListID type) : m_system(system), type(type) {}
		virtual ~Component() {}
		/// Allows for communication between components
		virtual void receive(const Message& message) = 0;

		/// A bitmask corresponding to the Lists (in m_container) that this Component belongs to
		const ComponentListID type;
	};
}

#endif // NTA_COMPONENT_H_INCLUDED