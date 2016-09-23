#include "ItemBox.h"
#include "Kart.h"
//#include "SpinningObject.h"

#include <iostream>

ItemBox::ItemBox(Mesh* mesh,
	Shader* shader,
	Texture* texture,
	Vector3 position) :
	SpinningObject(mesh,
	shader,
	texture,
	position,
	8.0f) {
	m_boundingBox = CBoundingBox(GetPosition() + m_mesh->GetMin(),
		GetPosition() + m_mesh->GetMax());
	recentCollision = false;
	itemValue = std::rand() % 4;
	m_timeStart = 0;
	m_badBox = false;
}

void ItemBox::Update(float timestep) {
	// TODO move bounds
	if (!m_badBox) {

		if (time(NULL) - m_timeStart < 3) {
			SetUniformScale(m_tmpUniformScale);
			//std::cout << time(NULL) - m_timeStart << std::endl;
		}
		
		else if (m_tmpUniformScale < 1.0) {
			
			//OutputDebugString("HERE");
			m_rotY += m_spinningSpeed * timestep;
			m_tmpUniformScale += 0.01;
			SetUniformScale(m_tmpUniformScale);
		}
	}
}

void ItemBox::OnKartCollisionEnter(Kart* other) {
	if (!m_badBox) {
		if (time(NULL) - m_timeStart > 5) {
			OutputDebugString("OnKartCollisionEnter\n");
			other->SetItemValue(itemValue);
			m_tmpUniformScale = 0.0;
			itemValue = std::rand() % 4;
			m_timeStart = time(NULL);
		}
	}
	else {

		m_status = 0;
	}
	
}
void ItemBox::OnKartCollisionStay(Kart* other) {
	//OutputDebugString("Collision Stay\n");
}
void ItemBox::OnKartCollisionExit(Kart* other) {
	//OutputDebugString("Collision Exit\n");
}
