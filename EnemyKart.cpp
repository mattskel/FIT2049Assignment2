#include "EnemyKart.h"
#include "MathsHelper.h"

EnemyKart::EnemyKart(Mesh* mesh,
	Shader* shader,
	Texture* texture,
	Vector3 position) :
	Kart(mesh,shader,texture,position,NULL) {

	
	m_moveSpeed = 2.0f; // Alter these to change the speed
	m_turnSpeed = 4.0f;
	m_frictionAmount = 4.0f;

	m_boundingBox = CBoundingBox(GetPosition() + m_mesh->GetMin(),
		GetPosition() + m_mesh->GetMax());
	

	m_targetPosition = GetRandomPosition();
}

void EnemyKart::Update(float timestep) {

	//System.out.println("HERE");

	if (Vector3::DistanceSquared(GetPosition(), m_targetPosition) <= 50.0f) {
		m_targetPosition = GetRandomPosition();
	}
	else {

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
		
		// Get the dot product
		// This might not be necessary
		/*
		float dotProduct = normalTargetVector.x * localForward.x +
							normalTargetVector.y * localForward.y +
							normalTargetVector.z * localForward.z;
		*/

		// Use the cross product to determine what direction to turn
		float crossProduct = localForward.x * normalTargetVector.z - localForward.z*normalTargetVector.x;

		// Use the sign of the cross product to turn left or right
		if (crossProduct > 0.05f) {
			m_rotY -= m_turnSpeed * timestep;
		}
		else if (crossProduct < -0.05f) {
			m_rotY += m_turnSpeed * timestep;
		}

		// Apply the force in the forward direction
		ApplyForce(localForward * m_moveSpeed * timestep);
	}

	// Move collider
	m_boundingBox.SetMin(GetPosition() + m_mesh->GetMin());
	m_boundingBox.SetMax(GetPosition() + m_mesh->GetMax());

	PhysicsObject::Update(timestep);
}

Vector3 EnemyKart::GetRandomPosition() {

	return Vector3(MathsHelper::RandomRange(-WORLD_WIDTH, WORLD_WIDTH),0,
		MathsHelper::RandomRange(-WORLD_DEPTH, WORLD_DEPTH));
}