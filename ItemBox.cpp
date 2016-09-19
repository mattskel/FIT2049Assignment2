#include "ItemBox.h"
#include "Kart.h"
//#include "SpinningObject.h"

ItemBox::ItemBox(Mesh* mesh,
	Shader* shader,
	Texture* texture,
	Vector3 position) :
	SpinningObject(mesh,
	shader,
	texture,
	position,
	4.0f) {
	m_boundingBox = CBoundingBox(GetPosition() + m_mesh->GetMin(),
		GetPosition() + m_mesh->GetMax());
	itemValue = std::rand() % 3;
}

/*void ItemBox::Update(float timestep) {
	// TODO move bounds

}*/

void ItemBox::OnKartCollisionEnter(Kart* other) {
	OutputDebugString("OnKartCollisionEnter\n");
	other->SetItemValue(itemValue);
	itemValue = std::rand() % 3;
}
void ItemBox::OnKartCollisionStay(Kart* other) {
	//OutputDebugString("Collision Stay\n");
}
void ItemBox::OnKartCollisionExit(Kart* other) {
	//OutputDebugString("Collision Exit\n");
}
