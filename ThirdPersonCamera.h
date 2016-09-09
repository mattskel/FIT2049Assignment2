#ifndef THIRD_PERSON_CAMERA_H
#define THIRD_PERSON_CAMERA_H

#include "Camera.h"
#include "GameObject.h"

class ThirdPersonCamera : public Camera {
private:
	GameObject* m_objectToFollow;
	Vector3 m_offset;
public:
	ThirdPersonCamera(GameObject* target, Vector3 offset);
	void Update(float timestep);
};



#endif