#ifndef BALLOON_H
#define BALLOON_H

#include "GameObject.h"

class Kart;

class Balloon : public GameObject {
private:
	Vector3 m_displacement;
	Kart* m_parentKart;

public:
	Balloon(Mesh* mesh,
		Shader* shader,
		Texture* texture,
		Vector3 displacement,
		Kart* parentKart);

	void Update(float timestep);
};

#endif