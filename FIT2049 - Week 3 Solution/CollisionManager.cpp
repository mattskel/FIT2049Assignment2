#include "CollisionManager.h"

#include <iostream>

CollisionManager::CollisionManager(std::vector<Kart*>* karts, std::vector<ItemBox*>* itemBoxes, std::vector<Wall*>* walls,
	std::vector<MovingItemObject*>* movingItemObjects)
{
	m_karts = karts;
	m_itemBoxes = itemBoxes;
	m_walls = walls;
	m_movingItemObjects = movingItemObjects;

	// Clear our arrays to 0 (NULL)
	memset(m_currentCollisions, 0, sizeof(m_currentCollisions));
	memset(m_previousCollisions, 0, sizeof(m_previousCollisions));

	m_nextCurrentCollisionSlot = 0;
}

void CollisionManager::CheckCollisions()
{
	// Check kart to item collisions
	KartToItemBox();

	// Check kart to kart collisions
	KartToKart();

	// Check kart to wall collisions
	KartToWall();

	// Check kart to item
	KartToItem();

	// Move all current collisions into previous
	memcpy(m_previousCollisions, m_currentCollisions, sizeof(m_currentCollisions));

	// Clear out current collisions
	memset(m_currentCollisions, 0, sizeof(m_currentCollisions));
	
	// Now current collisions is empty, we'll start adding from the start again
	m_nextCurrentCollisionSlot = 0;

}

bool CollisionManager::ArrayContainCollision(GameObject* arrayToSearch[], GameObject* first, GameObject* second)
{
	// See if these two GameObjects appear one after the other in specified collisions array
	// Stop one before length so we don't overrun as we'll be checking two elements per iteration
	for (int i = 0; i < MAX_ALLOWED_COLLISIONS - 1; i += 2)
	{
		if ((arrayToSearch[i] == first && arrayToSearch[i + 1] == second) ||
			arrayToSearch[i] == second && arrayToSearch[i + 1] == first)
		{
			// Found them!
			return true;
		}
	}

	// These objects were not colliding last frame
	return false;
}

void CollisionManager::AddCollision(GameObject* first, GameObject* second)
{	
	// Add the two colliding objects to the current collisions array
	// We keep track of the next free slot so no searching is required
	m_currentCollisions[m_nextCurrentCollisionSlot] = first;
	m_currentCollisions[m_nextCurrentCollisionSlot + 1] = second;
	
	m_nextCurrentCollisionSlot += 2;
}

void CollisionManager::KartToItemBox()
{
	
	for (unsigned int i = 0; i < m_karts->size(); i++) {
		for (unsigned int j = 0; j < m_itemBoxes->size(); j++) {

			Kart* kart = (*m_karts)[i];
			ItemBox* itemBox = (*m_itemBoxes)[j];

			CBoundingBox kartBounds = kart->GetBounds();
			CBoundingBox itemBoxBounds = itemBox->GetBounds();

			bool isColliding = CheckCollision(kartBounds, itemBoxBounds);
			bool wasColliding = ArrayContainCollision(m_previousCollisions, kart, itemBox);

			
			if (isColliding) {
				// Register the collision
				AddCollision(kart, itemBox);
				if (wasColliding) {
					// We were colliding
					kart->OnItemCollisionStay(itemBox);
					itemBox->OnKartCollisionStay(kart);
				}
				else {
					// Colliding this frame but not last frame
					kart->OnItemCollisionEnter(itemBox);
					itemBox->OnKartCollisionEnter(kart);
				}
			}
			else {
				if (wasColliding) {
					// We aren't colliding this frame but were last
					kart->OnItemCollisionExit(itemBox);
					itemBox->OnKartCollisionExit(kart);
				}
			}
			
		}
	}

}

void CollisionManager::KartToKart() {

	// We will check each kart against every other kart
	for (unsigned int i = 0; i < m_karts->size() - 1; i++) {
		for (unsigned int j = i + 1; j < m_karts->size(); j++) {

			Kart* kart1 = (*m_karts)[i];
			Kart* kart2 = (*m_karts)[j];

			CBoundingBox kart1Bounds = kart1->GetBounds();
			CBoundingBox kart2Bounds = kart2->GetBounds();

			// Are they colliding this frame?
			bool isColliding = CheckCollision(kart1Bounds, kart2Bounds);

			// Were they colliding last frame?
			bool wasColliding = ArrayContainCollision(m_previousCollisions, kart1, kart2);

			if (isColliding) {
				// Register the collision
				AddCollision(kart1, kart2);
				if (wasColliding) {
					// We were colliding
					kart1->OnKartCollisionStay(kart2);
					kart2->OnKartCollisionStay(kart1);
				}
				else {
					// Colliding this frame but not last frame
					kart1->OnKartCollisionEnter(kart2);
					kart2->OnKartCollisionEnter(kart1);
				}
			}
			else {
				if (wasColliding) {
					// We aren't colliding this frame but were last
					kart1->OnKartCollisionExit(kart2);
					kart2->OnKartCollisionExit(kart1);
				}
			}
		}
	}
}

void CollisionManager::KartToWall() {

	// We will check each kart against every other kart
	for (unsigned int i = 0; i < m_karts->size(); i++) {
		for (unsigned int j = 0; j < m_walls->size(); j++) {

			//int tmpInt = m_walls->size();
			//std::cout << tmpInt << std::endl;

			Kart* kart = (*m_karts)[i];
			Wall* wall = (*m_walls)[j];

			CBoundingBox kartBounds = kart->GetBounds();
			CBoundingBox wallBounds = wall->GetBounds();

			//std::cout << wallBounds.GetMax().x << " " << wallBounds.GetMin().x << std::endl;
			//std::cout << "x: " << kartBounds.GetMax().x << " " << kartBounds.GetMin().x << std::endl;
			//std::cout << "z: " << kartBounds.GetMax().z << " " << kartBounds.GetMin().z << std::endl;

			// Are they colliding this frame?
			bool isColliding = CheckCollision(kartBounds, wallBounds);

			// Were they colliding last frame?
			bool wasColliding = ArrayContainCollision(m_previousCollisions, kart, wall);

			if (isColliding) {
				
				// Register the collision
				AddCollision(kart, wall);
				if (wasColliding) {
					// We were colliding
					kart->OnWallCollisionStay(wall);
					//kart2->OnKartCollisionStay(kart1);
				}
				else {
					OutputDebugString("COLLIDING\n");
					// Colliding this frame but not last frame
					kart->OnWallCollisionEnter(wall);
					//kart2->OnKartCollisionEnter(kart1);
				}
			}
			else {
				if (wasColliding) {
					// We aren't colliding this frame but were last
					kart->OnWallCollisionExit(wall);
					//kart2->OnKartCollisionExit(kart1);
				}
			}
		}
	}

}

void CollisionManager::KartToItem() {
	for (unsigned int i = 0; i < m_karts->size(); i++) {
		for (unsigned int j = 0; j < m_movingItemObjects->size(); j++) {

			Kart* kart = (*m_karts)[i];
			MovingItemObject* movingItemObject = (*m_movingItemObjects)[j];

			CBoundingBox kartBounds = kart->GetBounds();
			CBoundingBox movingItemBounds = movingItemObject->GetBounds();

			bool isColliding = CheckCollision(kartBounds, movingItemBounds);
			bool wasColliding = ArrayContainCollision(m_previousCollisions, kart, movingItemObject);

			if (isColliding) {
				AddCollision(kart, movingItemObject);
				if (wasColliding) {
				}
				else {
					OutputDebugString("COLLIDING\n");
					kart->OnItemObjectCollisionEnter(movingItemObject);
					movingItemObject->OnKartCollisionEnter();
				}
			}
			else {
				if (wasColliding) {
				}
			}
		}
	}
}
