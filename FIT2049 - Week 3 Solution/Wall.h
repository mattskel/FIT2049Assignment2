#ifndef WALL_H
#define WALL_H

#include "StaticObject.h"

class Wall : public GameObject {

private:
	CBoundingBox m_boundingBox;
	Vector3 m_localFace;

public:

	Wall();
	void Update(float timestep);
	void SetBounds(CBoundingBox);
	void SetFace(Vector3 localFace);
	CBoundingBox GetBounds() { return m_boundingBox; }
	Vector3 GetLocalFace() { return m_localFace; }

};

#endif