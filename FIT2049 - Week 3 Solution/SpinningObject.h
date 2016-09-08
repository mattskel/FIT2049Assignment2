#ifndef SPINNINGOBJECT_H
#define SPINNINGOBJECT_H

#include "GameObject.h"

class SpinningObject : public GameObject
{
private:
	float m_spinningSpeed;

public:
	SpinningObject();
	SpinningObject(Mesh* mesh, Shader* shader, float spinSpeed);
	SpinningObject(Mesh* mesh, Shader* shader, Vector3 position, float spinSpeed);
	SpinningObject(Mesh* mesh, Shader* shader, Texture* texture, float spinSpeed);
	SpinningObject(Mesh* mesh, Shader* shader, Texture* texture, Vector3 position, float spinSpeed);
	
	virtual ~SpinningObject();

	float GetSpinningSpeed() { return m_spinningSpeed; }
	void SetSpinningSpeed(float spinningSpeed) { m_spinningSpeed = spinningSpeed; }

	void Update(float timestep);
};

#endif