#ifndef PHYSICS_OBJECT_H
#define PHYSICS_OBJECT_H

#include "GameObject.h"

class PhysicsObject : public GameObject
{
	
protected:
	Vector3 m_velocity;
	Vector3 m_acceleration;

	float m_frictionAmount;

	void ApplyForce(Vector3 force);
	void ApplyFriction(float strength);
	
public:
	PhysicsObject(Mesh* mesh,
		Shader* shader,
		Texture* texture,
		Vector3 position);

	virtual void Update(float timestep);

	Vector3 GetVelocity() { return m_velocity; }
	Vector3 GetAcceleration() { return m_acceleration; }
	
};

#endif