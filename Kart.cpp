#include "Kart.h"
//#include "ItemBox.h"
#include "RedShell.h"

#include <iostream>

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

	m_itemValue = std::rand() % 2;
	m_itemReleased = -1;
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

	if (m_input->GetKeyUp(VK_SPACE)) {
		//m_itemReleased = m_itemValue;
		ItemReleased();
		m_itemValue = -1;
	}

	// Move collider
	m_boundingBox.SetMin(GetPosition() + m_mesh->GetMin());
	m_boundingBox.SetMax(GetPosition() + m_mesh->GetMax());

	PhysicsObject::Update(timestep);
}

Vector3 Kart::GetLocalForward() {

	Vector3 worldForward = Vector3(0, 0, 1);
	Matrix heading = Matrix::CreateRotationY(m_rotY);
	Vector3 localForward = Vector3::TransformNormal(worldForward, heading);

	return localForward;
}

void Kart::GetItemPointers(std::vector<const char*>* itemTextures,
	std::vector<const char*>* itemMeshes,
	Shader* texturedShader) {
	m_itemTextures = itemTextures;
	m_itemMeshes = itemMeshes;
	m_texturedShader = texturedShader;
}

/*void Kart::GetItemList(std::vector<GameObject*>* gameObjects,
	std::vector<MovingItemObject*>* movingItemObjects) {
	m_gameObjects = gameObjects;
	m_movingItemObjects = movingItemObjects;
}*/

void Kart::GetObjects(std::vector<GameObject*>* gameObjects,
	std::vector<Kart*>* karts,
	std::vector<Shell*>* shells) {
	m_gameObjects = gameObjects;
	m_karts = karts;
	m_shells = shells;
}


void Kart::ItemReleased() {
	// First check that Kart has an item
	if (m_itemValue >= 0) {

		switch (m_itemValue) {
		case 0:
			break;
		case 1:
			RedShell* redShell = new RedShell(Mesh::GetMesh((*m_itemMeshes)[1]),
				m_texturedShader,
				Texture::GetTexture((*m_itemTextures)[m_itemValue]),
				GetPosition(),
				GetLocalForward(),
				m_rotY,
				m_karts);
			m_shells->push_back(redShell);
			m_gameObjects->push_back(redShell);
			break;
		}

		/*MovingItemObject* newItem = new MovingItemObject(Mesh::GetMesh((*m_itemMeshes)[1]),
			m_texturedShader,
			Texture::GetTexture((*m_itemTextures)[m_itemValue]),
			GetPosition(),
			GetLocalForward());*/
			

		// Need to add the new item to the moving items list
		//m_movingItemObjects->push_back(newItem);
		//m_shells->push_back(newItem);
		//m_gameObjects->push_back(newItem);
		m_itemValue = -1;
	}
}


void Kart::OnKartCollisionEnter(Kart* other) {

	Vector3 v1 = GetLocalForward();
	Vector3 v2 = other->GetLocalForward();

	Vector3 localNormal = GetPosition() - other->GetPosition();
	Matrix heading = Matrix::CreateRotationY(m_rotY);

	float normalLength = localNormal.Length();

	// localNormal.normalize()

	Vector3 unitNormal = Vector3(localNormal.x / normalLength,
		localNormal.y / normalLength,
		localNormal.z / normalLength);

	// Should change this to velocity...
	Vector3 acceleration2 = other->GetAcceleration();

	float collisionForce = (acceleration2.x * unitNormal.x + acceleration2.z * unitNormal.z);
	Vector3 forceVector = Vector3(acceleration2.x * unitNormal.x, 0, acceleration2.z * unitNormal.z);

	//ApplyForce(v1 + unitNormal);
	//ApplyForce(v1 + unitNormal * collisionForce);
	//ApplyForce(4.0 * forceVector);
	// Change the 4.0 multiplier to be indicitive of speeds, eg average of both speeds
	// Reduce the 4.0, but not ghosting
	ApplyForce(4.0 * unitNormal);

}
void Kart::OnKartCollisionStay(Kart* other) {
	//OutputDebugString("Kart Collision Stay\n");
	//m_frictionAmount = 4.0f;
}
void Kart::OnKartCollisionExit(Kart* other) {
	//OutputDebugString("Kart Collision Exit\n");
}


void Kart::OnItemCollisionEnter(ItemBox* other) {
	OutputDebugString("OnItemCollisionEnter\n");
}
void Kart::OnItemCollisionStay(ItemBox* other) {
	//OutputDebugString("OnItemCollisionStay\n");
}
void Kart::OnItemCollisionExit(ItemBox* other) {
	//OutputDebugString("OnItemCollisionExit\n");
}

void Kart::OnWallCollisionEnter(Wall* other) {
	OutputDebugString("OnWallCollisionEnter\n");

	Vector3 localForward = GetLocalForward();
	Vector3 acceleration = GetAcceleration();
	// Only using velocity at the moment
	Vector3 velocity = GetVelocity();

	Vector3 wallFace = other->GetLocalFace();
	//float dotProd = localForward.x * wallFace.x +
		//localForward.z * wallFace.z;
	float dotProd = velocity.x * wallFace.x +
					velocity.z * wallFace.z;

	std::cout << dotProd << std::endl;
	//ApplyForce(4.0 * (localForward - wallFace * 2 * dotProd));
	ApplyForce(4.0 * (velocity - wallFace * 2 * dotProd));

}
void Kart::OnWallCollisionStay(Wall* other) {
	//OutputDebugString("OnWallCollisionStay\n");
}
void Kart::OnWallCollisionExit(Wall* other) {
	//OutputDebugString("OnWallCollisionExit\n");
}

/*void Kart::OnItemObjectCollisionEnter(MovingItemObject* other) {
	Vector3 v1 = GetLocalForward();
	Vector3 v2 = other->GetLocalForward();

	Vector3 localNormal = GetPosition() - other->GetPosition();
	Matrix heading = Matrix::CreateRotationY(m_rotY);

	float normalLength = localNormal.Length();

	// localNormal.normalize()

	Vector3 unitNormal = Vector3(localNormal.x / normalLength,
		localNormal.y / normalLength,
		localNormal.z / normalLength);

	// Should change this to velocity...
	Vector3 velocity2 = other->GetVelocity();

	float collisionForce = (velocity2.x * unitNormal.x + velocity2.z * unitNormal.z);
	Vector3 forceVector = Vector3(velocity2.x * unitNormal.x, 0, velocity2.z * unitNormal.z);

	ApplyForce(4.0 * velocity2);
}*/

void Kart::OnShellCollisionEnter(Shell* other) {
	Vector3 v1 = GetLocalForward();
	Vector3 v2 = other->GetLocalForward();

	Vector3 localNormal = GetPosition() - other->GetPosition();
	Matrix heading = Matrix::CreateRotationY(m_rotY);

	float normalLength = localNormal.Length();

	// localNormal.normalize()

	Vector3 unitNormal = Vector3(localNormal.x / normalLength,
		localNormal.y / normalLength,
		localNormal.z / normalLength);

	Vector3 velocity2 = other->GetVelocity();

	float collisionForce = (velocity2.x * unitNormal.x + velocity2.z * unitNormal.z);
	Vector3 forceVector = Vector3(velocity2.x * unitNormal.x, 0, velocity2.z * unitNormal.z);

	ApplyForce(4.0 * velocity2);
}