#ifndef SHELL_H
#define SHELL_H

#include "PhysicsObject.h"
#include "Kart.h"
#include "Wall.h"

#include <vector>

class Shell : public PhysicsObject {
protected:
	CBoundingBox m_boundingBox;
	Vector3 m_direction;
	float m_moveSpeed;
	float m_turnSpeed;
public:
	Shell(Mesh* mesh,
		Shader* shader,
		Texture* texture,
		Vector3 position,
		Vector3 direction);

	CBoundingBox GetBounds() { return m_boundingBox; }

	Vector3 GetLocalForward();

	// Collisions
	void OnObjectCollisionEnter();
	void OnKartCollisionEnter();
	void OnWallCollisionEnter(Wall* other);

};

#endif