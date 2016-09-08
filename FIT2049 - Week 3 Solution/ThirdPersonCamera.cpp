#include "ThirdPersonCamera.h"

ThirdPersonCamera::ThirdPersonCamera(GameObject* target, Vector3 offset) {
	m_objectToFollow = target;
	m_offset = offset;
}

void ThirdPersonCamera::Update(float timestep) {

	if (m_objectToFollow != NULL) {
		SetLookAt(m_objectToFollow->GetPosition());

		float heading = m_objectToFollow->GetYRotation();

		Matrix rotation = Matrix::CreateRotationY(heading);

		Vector3 rotatedOffset = Vector3::TransformNormal(
			m_offset,
			rotation);

		SetPosition(m_objectToFollow->GetPosition() + rotatedOffset);
	}
	Camera::Update(timestep);
}