#ifndef COLLISION_MANAGER_H
#define COLLISION_MANAGER_H

#include <vector>
#include "Kart.h"
#include "ItemBox.h"
#include "Collisions.h"
#include "Wall.h"
#include "MovingItemObject.h"
#include "Shell.h"

#define MAX_ALLOWED_COLLISIONS 2048

//class MovingItemObject;

class CollisionManager
{
private:
	std::vector<Kart*>* m_karts;
	std::vector<ItemBox*>* m_itemBoxes;
	std::vector<Wall*>* m_walls;
	std::vector<MovingItemObject*>* m_movingItemObjects;
	std::vector<Shell*>* m_shells;
	std::vector<GameObject*>* m_otherItems;
	// You could also pass in an array of items

	GameObject* m_currentCollisions[MAX_ALLOWED_COLLISIONS];

	// We need to know what objects were colliding last frame so we can determine if a collision has just begun or ended
	GameObject* m_previousCollisions[MAX_ALLOWED_COLLISIONS];

	int m_nextCurrentCollisionSlot;

	// Check if we already know about two objects colliding
	bool ArrayContainCollision(GameObject* arrayToSearch[], GameObject* first, GameObject* second);

	// Register that a collision has occurred
	void AddCollision(GameObject* first, GameObject* second);

	// Collision check helpers
	// You might write one of these for each type of collision you want to check
	// Potential to add more here
	void KartToItemBox();

	void KartToKart();

	// Check collisions with the wall
	void KartToWall();

	void KartToItem();

	void KartToShell();

	void ShellToWall();

	void KartToOtherIterm();

public:
	/*
	CollisionManager(std::vector<Kart*>* karts, 
		std::vector<ItemBox*>* itemBoxes, 
		std::vector<Wall*>* walls,
		std::vector<MovingItemObject*>* movingItemObjects);
	*/
	CollisionManager(std::vector<Kart*>* karts,
		std::vector<ItemBox*>* itemBoxes,
		std::vector<Wall*>* walls,
		std::vector<Shell*>* shells,
		std::vector<GameObject*>* otherItems);

	void CheckCollisions();

};

#endif