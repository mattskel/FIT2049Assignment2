#ifndef KART_H
#define KART_H

#include "PhysicsObject.h"
#include "InputController.h"


class Kart : public PhysicsObject
{
private:
	InputController* m_input;

	float m_moveSpeed;
	float m_turnSpeed;

	CBoundingBox m_boundingBox;

public:
	Kart(Mesh* mesh,
		Shader* shader,
		Texture* texture,
		Vector3 position,
		InputController* input);

	void Update(float timestep);

	CBoundingBox GetBounds() { return m_boundingBox; }

	void OnKartCollisionEnter(Kart* other);
	void OnKartCollisionStay(Kart* other);
	void OnKartCollisionExit(Kart* other);
	
};

#endif