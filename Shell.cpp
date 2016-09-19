#include "Shell.h"

Shell::Shell(Mesh* mesh,
	Shader* shader,
	Texture* texture,
	Vector3 position,
	Vector3 direction) :
	PhysicsObject(mesh, shader, texture, position + 20 * direction) {

	m_moveSpeed = 7.0f; // Alter these to change the speed
	m_turnSpeed = 4.0f;
	m_frictionAmount = 3.0f;
	m_boundingBox = CBoundingBox(GetPosition() + m_mesh->GetMin(),
		GetPosition() + m_mesh->GetMax());
	m_direction = direction;
}

Vector3 Shell::GetLocalForward() {

	Vector3 worldForward = Vector3(0, 0, 1);
	Matrix heading = Matrix::CreateRotationY(m_rotY);
	Vector3 localForward = Vector3::TransformNormal(worldForward, heading);
	return localForward;
}

void Shell::OnObjectCollisionEnter() {}
void Shell::OnKartCollisionEnter() { m_status = 0; }
void Shell::OnWallCollisionEnter(Wall* other) {}