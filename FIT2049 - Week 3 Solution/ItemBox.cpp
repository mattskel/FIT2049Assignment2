#include "ItemBox.h"
#include "Kart.h"

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
	itemValue = std::rand() % 2;
}

void ItemBox::Update(float timestep) {
	// TODO move bounds
}

void ItemBox::OnKartCollisionEnter(Kart* other) {
	other->SetItemValue(itemValue);
	itemValue = std::rand() % 2;
}
void ItemBox::OnKartCollisionStay(Kart* other) {
	OutputDebugString("Collision Stay\n");
}
void ItemBox::OnKartCollisionExit(Kart* other) {
	OutputDebugString("Collision Exit\n");
}
