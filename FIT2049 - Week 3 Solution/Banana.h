#ifndef BANANA_H
#define BANANA_H

#include "StaticObject.h"

class Banana : public StaticObject {
private:
	CBoundingBox m_boundingBox;
public:
	Banana(Mesh* mesh,
		Shader* shader,
		Texture* texture,
		Vector3 position);

	CBoundingBox GetBounds() { return m_boundingBox; }

	void OnKartCollisionEnter();
};


#endif