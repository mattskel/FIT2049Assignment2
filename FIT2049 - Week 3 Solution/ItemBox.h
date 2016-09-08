#ifndef ITEM_BOX_H
#define ITEM_BOX_H

#include "GameObject.h"
#include "Kart.h"

class ItemBox : public GameObject
{
private:
	CBoundingBox m_boundingBox;
	
public:
	ItemBox(Mesh* mesh,
		Shader* shader,
		Texture* texture,
		Vector3 position);

	void Update(float timestep);

	CBoundingBox GetBounds() { return m_boundingBox; }

	void OnKartCollisionEnter(Kart* other);
	void OnKartCollisionStay(Kart* other);
	void OnKartCollisionExit(Kart* other);

};

#endif