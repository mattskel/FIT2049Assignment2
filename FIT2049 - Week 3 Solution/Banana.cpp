#include "Banana.h"

Banana::Banana(Mesh* mesh,
	Shader* shader,
	Texture* texture,
	Vector3 position) :
	StaticObject(mesh, shader, texture, position) {

	m_boundingBox = CBoundingBox(GetPosition() + m_mesh->GetMin(),
		GetPosition() + m_mesh->GetMax());
}

void Banana::OnKartCollisionEnter() { m_status = 0; }