#include "ItemBox.h"

ItemBox::ItemBox(Mesh* mesh,
	Shader* shader,
	Texture* texture,
	Vector3 position) :
	GameObject(mesh,
	shader,
	texture,
	position) {
	m_boundingBox = CBoundingBox(GetPosition() + m_mesh->GetMin(),
		GetPosition() + m_mesh->GetMax());
}

void ItemBox::Update(float timestep) {
	// TODO move bounds
}

void ItemBox::OnKartCollisionEnter(Kart* other) {
	OutputDebugString("Collision Enter\n");
}
void ItemBox::OnKartCollisionStay(Kart* other) {
	OutputDebugString("Collision Stay\n");
}
void ItemBox::OnKartCollisionExit(Kart* other) {
	OutputDebugString("Collision Exit\n");
}