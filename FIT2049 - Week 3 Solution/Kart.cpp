#include "Kart.h"
//#include "ItemBox.h"
#include "RedShell.h"
#include "GreenShell.h"
#include "Banana.h"
#include "ItemBox.h"

#include <iostream>

Kart::Kart(Mesh* mesh,
	Shader* shader,
	Texture* texture,
	Vector3 position,
	InputController* input) :
	PhysicsObject(mesh, shader, texture, position) {

	m_input = input;
	m_moveSpeed = 4.0f; // Alter these to change the speed
	m_turnSpeed = 2.0f;
	m_frictionAmount = 4.0f;

	m_boundingBox = CBoundingBox(GetPosition() + m_mesh->GetMin(),
		GetPosition() + m_mesh->GetMax());

	m_itemValue = std::rand() % 5;
	//m_itemReleased = -1;
	m_livesRemaining = 3;

	m_invincible = false;
	m_invincibleStart = 0;
}

// Initialise the balloon list
void Kart::InitBalloons() {
	float displacementX = -2.0;
	float displacementY = 4.0;
	float displacementZ = -3.0;
	for (int i = 0; i < 3; i++) {
		Balloon* balloon = new Balloon(Mesh::GetMesh(m_balloonMesh),
									m_texturedShader,
									Texture::GetTexture(m_balloonTexture),
									Vector3(displacementX, displacementY, displacementZ),
									this);
		displacementX += 2.0;
		m_balloons.push_back(balloon);
		m_gameObjects->push_back(balloon);
	}
}

void Kart::Update(float timestep) {

	Vector3 worldForward = Vector3(0, 0, 1);

	Matrix heading = Matrix::CreateRotationY(m_rotY);
	Vector3 localForward = Vector3::TransformNormal(worldForward, heading);

	if (m_input->GetKeyHold('A')) {
		m_rotY -= m_turnSpeed * timestep;
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

	// Draw Balloons
	for (Balloon* balloon : m_balloons) {
		balloon->Update(timestep);
	}

	int balloonIndex = 0;
	while (balloonIndex < m_balloons.size()) {
		Balloon* balloon = m_balloons[balloonIndex];
		balloon->Update(timestep);
		if (balloon->GetStatus() == 0) {
			//delete m_balloons[balloonIndex];
			m_balloons.erase(m_balloons.begin() + balloonIndex);
		}
		balloonIndex += 1;
	}

	// If the kart is currently invincible check how long it has been invincible for
	// If it is longer than some predetermined time stop invincibility mode
	if (m_invincible) {
		if (time(NULL) - m_invincibleStart > 5) {
			m_moveSpeed = 4.0f;
			m_invincible = false;
		}
	}

	// Move collider
	m_boundingBox.SetMin(GetPosition() + m_mesh->GetMin());
	m_boundingBox.SetMax(GetPosition() + m_mesh->GetMax());

	PhysicsObject::Update(timestep);

	if (m_livesRemaining == 0) {
		m_status = 0;
	}
}

// Returns the local forard of the kart
// Use this for collisions
Vector3 Kart::GetLocalForward() {
	Vector3 worldForward = Vector3(0, 0, 1);
	Matrix heading = Matrix::CreateRotationY(m_rotY);
	Vector3 localForward = Vector3::TransformNormal(worldForward, heading);

	return localForward;
}

void Kart::SetItemPointers(std::vector<const char*>* itemTextures,
	std::vector<const char*>* itemMeshes,
	Shader* texturedShader) {
	m_itemTextures = itemTextures;
	m_itemMeshes = itemMeshes;
	m_texturedShader = texturedShader;
}

void Kart::SetBalloonPointers(const char* balloonTexture, const char* balloonMesh) {
	m_balloonTexture = balloonTexture;
	m_balloonMesh = balloonMesh;
}

void Kart::SetObjects(std::vector<GameObject*>* gameObjects,
	std::vector<Kart*>* karts,
	std::vector<Shell*>* shells,
	std::vector<GameObject*>* otherItems) {
	m_gameObjects = gameObjects;
	m_karts = karts;
	m_shells = shells;
	m_otherItems = otherItems;
}

// Handles the release of items by a kart
void Kart::ItemReleased() {
	// First check that Kart has an item
	if (m_itemValue >= 0) {
		// Check what type of item was released
		switch (m_itemValue) {
		case 0:
		{
			GreenShell* greenShell = new GreenShell(Mesh::GetMesh((*m_itemMeshes)[0]),
				m_texturedShader,
				Texture::GetTexture((*m_itemTextures)[m_itemValue]),
				GetPosition(),
				GetLocalForward());
			m_shells->push_back(greenShell);
			break;
		}
		case 1:
		{
			RedShell* redShell = new RedShell(Mesh::GetMesh((*m_itemMeshes)[1]),
				m_texturedShader,
				Texture::GetTexture((*m_itemTextures)[m_itemValue]),
				GetPosition(),
				GetLocalForward(),
				m_rotY,
				m_karts);
			m_shells->push_back(redShell);
			break;
		}
		case 2:
		{
			Banana* banana = new Banana(Mesh::GetMesh((*m_itemMeshes)[2]),
										m_texturedShader,
										Texture::GetTexture((*m_itemTextures)[m_itemValue]),
										GetPosition() - 10.0 * GetLocalForward());
			m_otherItems->push_back(banana);
			break;
		}
		case 3:
		{
			ItemBox* badBox = new ItemBox(Mesh::GetMesh((*m_itemMeshes)[3]),
											m_texturedShader,
											Texture::GetTexture((*m_itemTextures)[m_itemValue]),
											GetPosition() - 10.0 * GetLocalForward());
			badBox->SetBadBox();
			m_otherItems->push_back(badBox);
			m_itemBoxes->push_back(badBox);	// By putting it on the item box list, enemies will go for it
			break;
		}
		case 4:
		{
			m_moveSpeed = 6.0f;
			m_invincible = true;
			m_invincibleStart = time(NULL);
		}
		}
		m_itemValue = -1;
	}
}

void Kart::LifeLost() {
	if (m_livesRemaining > 0) {
		Balloon* balloon = m_balloons[m_livesRemaining - 1];
		balloon->SetLifeLost();
		m_livesRemaining -= 1;
	}
}

// Handles the collisions with other karts
// A force is applied to kart indicative of the angle of collision
void Kart::OnKartCollisionEnter(Kart* other) {

	if (!m_invincible) {
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

		ApplyForce(4.0 * unitNormal);

		if (other->GetInvincibility()) {
			LifeLost();
		}
	}
}
void Kart::OnKartCollisionStay(Kart* other) {
	//OutputDebugString("Kart Collision Stay\n");
	//m_frictionAmount = 4.0f;
}
void Kart::OnKartCollisionExit(Kart* other) {
	//OutputDebugString("Kart Collision Exit\n");
}


void Kart::OnItemCollisionEnter(ItemBox* other) {
	//OutputDebugString("OnItemCollisionEnter\n");
}
void Kart::OnItemCollisionStay(ItemBox* other) {
	//OutputDebugString("OnItemCollisionStay\n");
}
void Kart::OnItemCollisionExit(ItemBox* other) {
	//OutputDebugString("OnItemCollisionExit\n");
}

// Handles collisions with the walls
// When colliding with a wall the kart rebounds at the same angle
void Kart::OnWallCollisionEnter(Wall* other) {
	OutputDebugString("OnWallCollisionEnter\n");

	Vector3 localForward = GetLocalForward();
	Vector3 acceleration = GetAcceleration();
	// Only using velocity at the moment
	Vector3 velocity = GetVelocity();

	Vector3 wallFace = other->GetLocalFace();
	float dotProd = velocity.x * wallFace.x +
					velocity.z * wallFace.z;

	ApplyForce(4.0 * (velocity - wallFace * 2 * dotProd));

}
void Kart::OnWallCollisionStay(Wall* other) {
	//OutputDebugString("OnWallCollisionStay\n");
}
void Kart::OnWallCollisionExit(Wall* other) {
	//OutputDebugString("OnWallCollisionExit\n");
}

// Handles collisions with shells
// The kart rebounds based on the direction of the shell
void Kart::OnShellCollisionEnter(Shell* other) {
	if (!m_invincible) {
		Vector3 localNormal = GetPosition() - other->GetPosition();
		Matrix heading = Matrix::CreateRotationY(m_rotY);

		float normalLength = localNormal.Length();

		// localNormal.normalize()

		Vector3 unitNormal = Vector3(localNormal.x / normalLength,
			localNormal.y / normalLength,
			localNormal.z / normalLength);

		Vector3 otherVelocity = other->GetVelocity();

		float collisionForce = (otherVelocity.x * unitNormal.x + otherVelocity.z * unitNormal.z);
		Vector3 forceVector = Vector3(otherVelocity.x * unitNormal.x, 0, otherVelocity.z * unitNormal.z);

		ApplyForce(2.0 * otherVelocity);

		LifeLost();
	}
}

// Collisions with items other than shells
// Rebound off the item in the reverse direction
void Kart::OnOtherItemCollisionEnter() {
	if (!m_invincible) {
		Vector3 velocity = GetVelocity();
		ApplyForce(-2.0 * velocity);

		LifeLost();
	}
}