#include "Kart.h"
#include "ItemBox.h"

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

void Kart::OnKartCollisionEnter(Kart* other) {

	Vector3 v1 = GetLocalForward();
	Vector3 v2 = other->GetLocalForward();

	Vector3 localNormal = other->GetPosition() - GetPosition();
	Matrix heading = Matrix::CreateRotationY(m_rotY);

	//Vector3 localNormal = Vector3::TransformNormal(normal, heading);

	float normalLength = localNormal.Length();

	Vector3 unitNormal = Vector3(localNormal.x / normalLength,
		localNormal.y / normalLength,
		localNormal.z / normalLength);

	Vector3 unitTangent = Vector3(-unitNormal.z, 0, unitNormal.x);

	float v1NormalBefore = unitNormal.x * v1.x +
		unitNormal.y * v1.y +
		unitNormal.z * v1.z;

	float v2NormalBefore = unitNormal.x * v2.x +
		unitNormal.y * v2.y +
		unitNormal.z * v2.z;

	float v1TangentBefore = unitTangent.x * v1.x +
		unitTangent.y*v1.y +
		unitTangent.z*v1.z;

	float v2TangentBefore = unitTangent.x * v2.x +
		unitTangent.y*v2.y +
		unitTangent.z*v2.z;

	float v1NormalAfter = v2NormalBefore;

	Vector3 vector1NormalAfter = v2NormalBefore * Vector3(1,0,1);
	Vector3 vector1TangentAfter = v2TangentBefore * Vector3(1,0,1);

	ApplyForce(vector1NormalAfter + vector1TangentAfter);

	/*
	//OutputDebugString("Kart Collision Enter\n");
	//m_frictionAmount = 100.0f;

	// Get the vector between the two colliding objects
	Vector3 collisionVector = GetPosition() - other->GetPosition();
	// Get the vector length
	float vectorLength = collisionVector.Length();
	// Normalise
	Vector3 normalCollisionVector = Vector3(collisionVector.x / vectorLength,
		collisionVector.y / vectorLength,
		collisionVector.z / vectorLength);
	
	// Get local forward of other Kart
	Vector3 otherMomentum = other->GetLocalForward();
	// Dot product for magnitude
	float magnitude = normalCollisionVector.x * otherMomentum.x +
		normalCollisionVector.y * otherMomentum.y +
		normalCollisionVector.z * otherMomentum.z;


	Vector3 worldForward = Vector3(0, 0, -1);

	Matrix heading = Matrix::CreateRotationY(m_rotY);
	Vector3 localForward = Vector3::TransformNormal(worldForward, heading);

	ApplyForce(localForward*2.0f + magnitude*normalCollisionVector);
	*/
}
void Kart::OnKartCollisionStay(Kart* other) {
	OutputDebugString("Kart Collision Stay\n");
	//m_frictionAmount = 4.0f;
}
void Kart::OnKartCollisionExit(Kart* other) {
	OutputDebugString("Kart Collision Exit\n");
}


void Kart::OnItemCollisionEnter(ItemBox* other) {
}
void Kart::OnItemCollisionStay(ItemBox* other) {
	OutputDebugString("HERE");
}
void Kart::OnItemCollisionExit(ItemBox* other) {
	OutputDebugString("HERE");
}