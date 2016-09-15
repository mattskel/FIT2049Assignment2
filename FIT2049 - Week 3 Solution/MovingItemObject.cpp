#include "MovingItemObject.h"

MovingItemObject::MovingItemObject(Mesh* mesh,
	Shader* shader,
	Texture* texture,
	Vector3 position,
	Vector3 direction) :
	PhysicsObject(mesh, shader, texture, position) {

	m_moveSpeed = 5.0f; // Alter these to change the speed
	m_frictionAmount = 4.0f;

	m_boundingBox = CBoundingBox(GetPosition() + m_mesh->GetMin(),
		GetPosition() + m_mesh->GetMax());

	currentDirection = direction;
}

void MovingItemObject::Update(float timestep) {
	ApplyForce(currentDirection * m_moveSpeed * timestep);
}