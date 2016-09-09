#include "CollisionManager.h"

CollisionManager::CollisionManager(std::vector<Kart*>* karts, std::vector<ItemBox*>* itemBoxes)
{
	m_karts = karts;
	m_itemBoxes = itemBoxes;

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
				//OutputDebugString("COLLIDE\n");
				// Register the collision
				AddCollision(kart1, kart2);
				if (wasColliding) {
					//OutputDebugString("wasColliding\n");
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
