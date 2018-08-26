#include <iostream>

#include <nta/ECS.h>

using namespace std;
using namespace nta;

class HealthComponent : public Component {
private:
	int m_health;
public:
	HealthComponent() : m_health(100), Component(1 << 0) {}
	int get_health() const { return m_health; }
	void receive(const Message& message) {
		int damage = static_cast<int>(reinterpret_cast<std::uintptr_t>(message.data));
		m_health -= damage;
	}
};

class DamageComponent : public Component {
public:
	DamageComponent() : Component(1 << 1) {}
	void receive(const Message& _) {}
};

int main(int argc, char* argv[]) {
    nta::init(); // This is useless here but it can't hurt
    cout<<"Running ECS tests..."<<endl;

    ECS system;
    EntityID healthy = system.gen_entity();
    EntityID sick = system.gen_entity();

    assert(system.add_component(new HealthComponent, sick));
    assert(system.add_component(new DamageComponent, sick));
    assert(system.add_component(new HealthComponent, healthy));

    int sick_health = 100;
    while (sick_health) {
    	ComponentNode* health_node = system.get_component_list(1);
    	assert(((HealthComponent*)health_node->comp)->get_health() == 100);
    	health_node = health_node->next;
    	assert(((HealthComponent*)health_node->comp)->get_health() == sick_health);
    	assert(health_node->next == nullptr);
    	
    	ComponentNode* damage_node = system.get_component_list(2);
    	while (damage_node) {
    		damage_node->comp->send(Message(0, (void*)5));
    		damage_node = damage_node->next;
    	}

    	sick_health -= 5;
    }

    cout<<"Tests passed"<<endl;
    nta::cleanup();
    return 0;
}

