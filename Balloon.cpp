#include "Balloon.h"
#include "Kart.h"

Balloon::Balloon(Mesh* mesh,
	Shader* shader,
	Texture* texture,
	Vector3 displacement,
	Kart* parentKart) :
	GameObject(mesh, shader, texture) {
	m_displacement = displacement;
	m_parentKart = parentKart;
}

void Balloon::Update(float timestep) {

	float heading = m_parentKart->GetYRotation();
	Vector3 velocity = dynamic_cast<PhysicsObject*>(m_parentKart)->GetVelocity();

	Matrix rotation = Matrix::CreateRotationY(heading);

	Vector3 rotatedOffset = Vector3::TransformNormal(m_displacement, rotation);
	SetPosition(Vector3::Lerp(GetPosition(),
							m_parentKart->GetPosition() + rotatedOffset,
							timestep * 3.0));

	//SetPosition(m_parentKart->GetPosition() + m_displacement);
}