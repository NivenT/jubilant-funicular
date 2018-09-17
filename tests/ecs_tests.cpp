#include <iostream>

#include <nta/ECS.h>

using namespace std;
using namespace nta;

class HealthComponent : public Component {
private:
	int m_health;
public:
	HealthComponent() : m_health(100) {}
	int get_health() const { return m_health; }
	void receive(const Message& message) {
		int damage = static_cast<int>(reinterpret_cast<std::uintptr_t>(message.data));
		m_health -= damage;
	}
};

class DamageComponent : public Component {
public:
	DamageComponent() {}
	void receive(const Message& _) {}
};

class FakeComponent : public Component {
};

int main(int argc, char* argv[]) {
    cout<<"Running ECS tests..."<<endl;
    
    ECS system;
    Entity healthy = system.gen_entity();
    Entity sick = system.gen_entity();
    Entity empty = system.gen_entity();
    Entity noone = empty + 10;

    assert(system.add_component<HealthComponent>(HealthComponent(), sick));
    assert(system.add_component<DamageComponent>(DamageComponent(), sick));
    assert(system.add_component<HealthComponent>(HealthComponent(), healthy));
    
    for (int sick_health = 100; sick_health; sick_health -= 5) {
        vector<HealthComponent*> healths = system.get_component_list<HealthComponent>();
        assert(healths[0]->get_health() == sick_health);
        assert(healths[1]->get_health() == 100);
        assert(healths.size() == 2);

        vector<DamageComponent*> dams = system.get_component_list<DamageComponent>();
        for (auto& dam : dams) {
            dam->send(Message(0, (void*)5));
        }
        assert(dams.size() == 1);
    }
    
    assert(!system.has_component<DamageComponent>(healthy));
    assert(system.has_component<HealthComponent>(sick));
    assert(!system.has_component<FakeComponent>(healthy));
    assert(system.get_components(empty).empty());
    assert(system.get_components(sick).size() == 2);
    
    assert(system.get_owner(system.get_component_list<DamageComponent>().front()) == sick);
    HealthComponent temp;
    assert(system.get_owner(&temp) == NTA_INVALID_ID);
    assert(system.get_siblings(system.get_component_list<DamageComponent>().front()).size() == 2);

    assert(system.get_component<HealthComponent>(sick).get_health() == 0);
    assert(system.get_component<HealthComponent>(healthy).get_health() == 100);
    assert(system.get_component_list<DamageComponent>().front() == &system.get_component<DamageComponent>(sick));
    
    assert(system.delete_component(system.get_component_list<DamageComponent>().front()));
    assert(!system.has_component<DamageComponent>(sick));
    
    for (int i = 0; i < 4; i++) {
        assert(system.add_component<DamageComponent>(DamageComponent(), sick));
    }
    assert(system.delete_component(&system.get_component<DamageComponent>(sick)));
    assert(system.has_component<DamageComponent>(sick));
    assert(system.get_component_list<DamageComponent>(sick).size() == 3);
    system.delete_components<DamageComponent>(sick);
    assert(!system.has_component<DamageComponent>(sick));
    
    for (const HealthComponent* health : system.get_component_list<HealthComponent>(sick)) {
        assert(health->get_health() == 0);
    }
    
    assert(system.get_owner(nullptr) == NTA_INVALID_ID);
    assert(!system.delete_component(nullptr));
    assert(!system.add_component<HealthComponent>(HealthComponent(), noone));
    assert(system.get_component_list<FakeComponent>().empty());
    
    for (auto null : system.get_component_list<FakeComponent>()) {
        assert(false);
    }
    
    assert(system.does_entity_exist(sick));
    assert(system.does_entity_exist(healthy));
    assert(system.does_entity_exist(empty));
    assert(system.delete_entity(healthy));
    assert(!system.does_entity_exist(healthy));
    assert(!system.does_entity_exist(NTA_INVALID_ID));
    assert(!system.does_entity_exist(noone));
    
    cout<<"Tests passed"<<endl;
    return 0;
}

