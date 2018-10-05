#include <iostream>

#include <nta/ECS.h>

using namespace std;
using namespace nta;
using namespace nta::utils;

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
    Option<Message> respond(const Message& req) {
        return Option<Message>::some(Message(0, (void*)(long)m_health));
    }
};

class DamageComponent : public Component {
private:
    long m_damage;
public:
	DamageComponent(long dam = 0) : m_damage(dam) {}
	void receive(const Message& _) {}
    long get_damage() const { return m_damage; }
};

class FakeComponent : public Component {
};

Option<Message> temp() {
    return utils::Option<Message>::some(Message(0, (void*)95));
}

int main(int argc, char* argv[]) {
    cout<<"Running ECS tests..."<<endl;
    
    ECS system;
    Entity healthy = system.gen_entity();
    Entity sick = system.gen_entity();
    Entity empty = system.gen_entity();
    Entity noone = empty + 10;

    ComponentID sick_h = system.add_component<HealthComponent>(sick);
    ComponentID sick_d = system.add_component<DamageComponent>(sick, 5);
    ComponentID healthy_h = system.add_component<HealthComponent>(healthy);

    assert(sick_h != NTA_INVALID_ID);
    assert(sick_d != NTA_INVALID_ID);
    assert(healthy_h != NTA_INVALID_ID);
    
    for (int sick_health = 100; sick_health; sick_health -= 5) {
        vector<HealthComponent*> healths = system.get_component_list<HealthComponent>();
        assert(healths[0]->get_health() == sick_health);
        assert(healths[1]->get_health() == 100);
        assert(healths.size() == 2);

        vector<DamageComponent*> dams = system.get_component_list<DamageComponent>();
        for (auto& dam : dams) {
            dam->send(Message(0, (void*)dam->get_damage()));
            auto resp = dam->request(Message());
            assert(resp.is_some());
            assert((long)resp.unwrap().data == sick_health - dam->get_damage());
        }
        assert(dams.size() == 1);
    }
    
    assert(!system.has_component<DamageComponent>(healthy));
    assert(system.has_component<HealthComponent>(sick));
    assert(!system.has_component<FakeComponent>(healthy));
    assert(system.get_components(empty).unwrap().empty());
    assert(system.get_components(sick).unwrap().size() == 2);
    
    assert(system.get_owner(sick_d) == sick);
    assert(system.get_owner(NTA_INVALID_ID) == NTA_INVALID_ID);
    assert(system.get_siblings(sick_d).unwrap().size() == 2);

    assert(system.get_component<HealthComponent>(sick).unwrap().get_health() == 0);
    assert(system.get_component<HealthComponent>(healthy).unwrap().get_health() == 100);
    assert(system.get_component_list<DamageComponent>().front() == &system.get_component<DamageComponent>(sick).unwrap());
    
    assert(system.delete_component(sick_d) == sick_d);
    assert(!system.has_component<DamageComponent>(sick));
    
    for (int i = 0; i < 4; i++) {
        assert(system.add_component<DamageComponent>(sick) != NTA_INVALID_ID);
    }
    assert(system.delete_component(system.get_component<DamageComponent>(sick).unwrap().get_id()) != NTA_INVALID_ID);
    assert(system.has_component<DamageComponent>(sick));
    assert(system.get_component_list<DamageComponent>(sick).size() == 3);
    system.delete_components<DamageComponent>(sick);
    assert(!system.has_component<DamageComponent>(sick));
    
    for (const HealthComponent* health : system.get_component_list<HealthComponent>(sick)) {
        assert(health->get_health() == 0);
    }
    
    assert(system.get_owner(1 << 10) == NTA_INVALID_ID);
    assert(system.delete_component(NTA_INVALID_ID) == NTA_INVALID_ID);
    assert(system.add_component<HealthComponent>(noone) == NTA_INVALID_ID);
    assert(system.get_component_list<FakeComponent>().empty());
    
    assert(system.does_entity_exist(sick));
    assert(system.does_entity_exist(healthy));
    assert(system.does_entity_exist(empty));
    assert(system.delete_entity(healthy));
    assert(!system.does_entity_exist(healthy));
    assert(!system.does_entity_exist(NTA_INVALID_ID));
    assert(!system.does_entity_exist(noone));

    assert(system.get_component<DamageComponent>(healthy).is_none());
    assert(system.get_component_list<HealthComponent>(empty).empty());
    assert(system.get_components(noone).is_none());
    
    cout<<"Tests passed"<<endl;
    return 0;
}

