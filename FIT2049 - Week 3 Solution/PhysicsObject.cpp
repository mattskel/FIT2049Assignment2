#include "PhysicsObject.h"

PhysicsObject::PhysicsObject(Mesh* mesh,
	Shader* shader,
	Texture* texture,
	Vector3 position) :
	GameObject(mesh,
	shader,
	texture,
	position)
{
	m_velocity = Vector3::Zero;
	m_acceleration = Vector3::Zero;
}

void PhysicsObject::ApplyForce(Vector3 force) {
	m_acceleration += force;
}

void PhysicsObject::ApplyFriction(float strength) {
	ApplyForce(-m_velocity *strength);
}

void PhysicsObject::Update(float timestep) {
	ApplyFriction(m_frictionAmount * timestep);

	m_velocity += m_acceleration;
	m_position += m_velocity;

	m_acceleration = Vector3::Zero;
}
