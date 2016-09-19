#ifndef ENEMY_KART_H
#define ENEMY_KART_H

#include "PhysicsObject.h"
#include "Kart.h"
#include "InputController.h"


class EnemyKart : public Kart
{
private:

	const int WORLD_WIDTH = 299;
	const int WORLD_DEPTH = 299;

	Vector3 m_targetPosition;
	Vector3 GetRandomPosition();

	std::vector<ItemBox*>* m_itemBoxes;
	boolean m_targetIsItemBox;
	ItemBox* m_previousItemBox;		// To make sure we don't return to the previous ItemBox

public:
	EnemyKart(Mesh* mesh,
		Shader* shader,
		Texture* texture,
		Vector3 position);

	void GetItemBoxes(std::vector<ItemBox*>* itemBoxes) { m_itemBoxes = itemBoxes; }

	void Update(float timestep);

	//CBoundingBox GetBounds() { return m_boundingBox; }

};

#endif