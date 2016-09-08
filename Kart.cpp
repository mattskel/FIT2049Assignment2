#include "Kart.h"

Kart::Kart(Mesh* mesh,
	Shader* shader,
	Texture* texture,
	Vector3 position,
	InputController* input) :
	PhysicsObject(mesh, shader, texture, position) {

	m_input = input;
	m_moveSpeed = 4.0f; // Alter these to change the speed
	m_turnSpeed = 4.0f;
	m_frictionAmount = 4.0f;

	m_boundingBox = CBoundingBox(GetPosition() + m_mesh->GetMin(),
		GetPosition() + m_mesh->GetMax());
}

void Kart::Update(float timestep) {
	Vector3 worldForward = Vector3(0, 0, 1);

	Matrix heading = Matrix::CreateRotationY(m_rotY);
	Vector3 localForward = Vector3::TransformNormal(worldForward, heading);

	if (m_input->GetKeyHold('A')) {
		m_rotY -= m_turnSpeed * timestep;
		// Need to see whay local forward is doing
		//float myZFloat = localForward.z;
		//float myXFloat = localForward.x;
		//printf("%f, %f\n", myXFloat, myZFloat);
	}

	if (m_input->GetKeyHold('D'))
		m_rotY += m_turnSpeed * timestep;

	if (m_input->GetKeyHold('W'))
		ApplyForce(localForward * m_moveSpeed * timestep);

	// Move collider
	m_boundingBox.SetMin(GetPosition() + m_mesh->GetMin());
	m_boundingBox.SetMax(GetPosition() + m_mesh->GetMax());

	PhysicsObject::Update(timestep);
}

void Kart::OnKartCollisionEnter(Kart* other) {
	OutputDebugString("Kart Collision Enter\n");
}
void Kart::OnKartCollisionStay(Kart* other) {
	OutputDebugString("Kart Collision Stay\n");
}
void Kart::OnKartCollisionExit(Kart* other) {
	OutputDebugString("Kart Collision Exit\n");
}