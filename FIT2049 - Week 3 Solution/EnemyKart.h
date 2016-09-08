#ifndef ENEMY_KART_H
#define ENEMY_KART_H

#include "PhysicsObject.h"
#include "Kart.h"
#include "InputController.h"


class EnemyKart : public Kart
{
private:

	const int WORLD_WIDTH = 200;
	const int WORLD_DEPTH = 200;

	Vector3 m_targetPosition;

	
	float m_moveSpeed;
	float m_turnSpeed;

	CBoundingBox m_boundingBox;
	

	Vector3 GetRandomPosition();

public:
	EnemyKart(Mesh* mesh,
		Shader* shader,
		Texture* texture,
		Vector3 position);

	void Update(float timestep);

	//CBoundingBox GetBounds() { return m_boundingBox; }

};

#endif