#include "SpinningObject.h"

SpinningObject::SpinningObject() : GameObject()
{
	m_spinningSpeed = 0.0f;
}

SpinningObject::SpinningObject(Mesh* mesh, Shader* shader, float spinSpeed) : 
	GameObject(mesh, shader)
{
	m_spinningSpeed = spinSpeed;
}

SpinningObject::SpinningObject(Mesh* mesh, Shader* shader, Vector3 position, float spinSpeed) : 
	GameObject(mesh, shader, position)
{
	m_spinningSpeed = spinSpeed;
}

SpinningObject::SpinningObject(Mesh* mesh, Shader* shader, Texture* texture, float spinSpeed) :
	GameObject(mesh, shader, texture)
{
	m_spinningSpeed = spinSpeed;
}

SpinningObject::SpinningObject(Mesh* mesh, Shader* shader, Texture* texture, Vector3 position, float spinSpeed) :
	GameObject(mesh, shader, texture, position)
{
	m_spinningSpeed = spinSpeed;
}

SpinningObject::~SpinningObject() { }

void SpinningObject::Update(float timestep)
{
	//m_rotY += m_spinningSpeed * timestep;
}