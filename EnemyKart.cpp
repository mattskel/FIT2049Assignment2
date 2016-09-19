#include "EnemyKart.h"
#include "MathsHelper.h"

#include <iostream>

EnemyKart::EnemyKart(Mesh* mesh,
	Shader* shader,
	Texture* texture,
	Vector3 position) :
	Kart(mesh,shader,texture,position,NULL) {

	
	m_moveSpeed = 3.0f; // Alter these to change the speed
	m_turnSpeed = 4.0f;
	m_frictionAmount = 4.0f;

	m_boundingBox = CBoundingBox(GetPosition() + m_mesh->GetMin(),
		GetPosition() + m_mesh->GetMax());
	
	m_targetIsItemBox = false;
	m_targetPosition = GetRandomPosition();
	m_previousItemBox = NULL;
}

void EnemyKart::Update(float timestep) {

	// Check for an item
	// Release if it exists
	if (m_itemValue >= 0) {
		ItemReleased();
	}

	if (Vector3::DistanceSquared(GetPosition(), m_targetPosition) <= 2.0f) {
		m_targetPosition = GetRandomPosition();
		m_targetIsItemBox = false;
	}
	else {

		// First check if there are any item boxes near by
		// If there are then we will reset m_targetPosition
		// Only check if we are not already looking for an ItemBox
		float maxDist = 50.0f;	// We are looking for an item box less than 50.f away
		float minDist = 20.0f;
		if (!m_targetIsItemBox) {
			for (ItemBox* itemBox : *m_itemBoxes) {
				// Check we are not returning to Item Box just visited
				if (itemBox != m_previousItemBox) {
					Vector3 itemBoxPosition = itemBox->GetPosition();
					Vector3 itemBoxVector = itemBoxPosition - GetPosition();

					//Get the vector length
					float vectorLength = itemBoxVector.Length();
					if (vectorLength < maxDist && vectorLength > minDist) {
						maxDist = vectorLength;
						m_targetPosition = itemBoxPosition;
						m_previousItemBox = itemBox;
					}
				}
			}
		}
		if (maxDist < 50.0f) {
			m_targetIsItemBox = true;
		}

		Vector3 worldForward = Vector3(0, 0, 1);
		Matrix heading = Matrix::CreateRotationY(m_rotY);
		Vector3 localForward = Vector3::TransformNormal(worldForward, heading);

		// A vector from current position to target position
		Vector3 targetVector = m_targetPosition - GetPosition();
		// Normalise the vector
		// Get the vector length
		float vectorLength = targetVector.Length();
		Vector3 normalTargetVector = Vector3(targetVector.x / vectorLength, 
												targetVector.y / vectorLength, 
												targetVector.z / vectorLength);

		// Use the cross product to determine what direction to turn
		float crossProduct = localForward.x * normalTargetVector.z - localForward.z*normalTargetVector.x;

		// Use the sign of the cross product to turn left or right
		if (crossProduct > 0.01f) {
			m_rotY -= m_turnSpeed * timestep;
		}
		else if (crossProduct < -0.01f) {
			m_rotY += m_turnSpeed * timestep;
		}

		// Apply the force in the forward direction
		ApplyForce(localForward * m_moveSpeed * timestep);
	}

	// Move collider
	m_boundingBox.SetMin(GetPosition() + m_mesh->GetMin());
	m_boundingBox.SetMax(GetPosition() + m_mesh->GetMax());

	//std::cout << m_boundingBox.GetMax().x << " " << m_boundingBox.GetMax().y << " " << m_boundingBox.GetMax().z << std::endl;

	PhysicsObject::Update(timestep);
}

Vector3 EnemyKart::GetRandomPosition() {

	return Vector3(MathsHelper::RandomRange(-WORLD_WIDTH, WORLD_WIDTH),0,
		MathsHelper::RandomRange(-WORLD_DEPTH, WORLD_DEPTH));
}