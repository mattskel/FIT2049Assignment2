#ifndef KART_H
#define KART_H

#include "PhysicsObject.h"
#include "InputController.h"
#include "ItemBox.h"
#include "Wall.h"
#include "MovingItemObject.h"
#include "Shell.h"
#include "Balloon.h"
//#include "RedShell.h"

#include <vector>

// Forward declare ItemBox
class ItemBox;
class MovingItemObject;
class Shell;

class Kart : public PhysicsObject
{
private:
	InputController* m_input;
	

protected:
	float m_moveSpeed;
	float m_turnSpeed;

	CBoundingBox m_boundingBox;

	int m_itemValue;
	int m_itemReleased;

	// Kart will have pointers to everything required to build its own Items
	std::vector<const char*>* m_itemTextures;
	std::vector<const char*>* m_itemMeshes;
	const char* m_balloonTexture;
	const char* m_balloonMesh;
	Shader* m_texturedShader;


	// Kart needs to store the items it creates somewhere
	std::vector<GameObject*>* m_gameObjects;
	//std::vector<MovingItemObject*>* m_movingItemObjects;
	std::vector<Kart*>* m_karts;	// Need a pointer to other karts for RedShell
	std::vector<Shell*>* m_shells;
	std::vector<GameObject*>* m_otherItems;
	std::vector<Balloon*> m_balloons;

	// Need to know where to put on item after it's been shot
	//std::vector<MovingItemObject*>* m_movingItems;

public:
	Kart(Mesh* mesh,
		Shader* shader,
		Texture* texture,
		Vector3 position,
		InputController* input);

	void InitBalloons();

	void Update(float timestep);

	CBoundingBox GetBounds() { return m_boundingBox; }

	Vector3 GetLocalForward();

	void SetItemValue(int item) { m_itemValue = item; }
	int GetItemValue() { return m_itemValue; }

	// Gives Kart all the relevant pointers for construction an item
	void GetItemPointers(std::vector<const char*>* itemTextures,
							std::vector<const char*>* itemMeshes,
							Shader* texturedShader);

	void GetBalloonPointers(const char* balloonTexture, const char* balloonMesh);

	// Gives Kart pointers for storing items it creates
	/*void GetItemList(std::vector<GameObject*>* gameObjects,
						std::vector<MovingItemObject*>* movingItemObjects);*/

	void GetObjects(std::vector<GameObject*>* gameObjects,
					std::vector<Kart*>* karts,
					std::vector<Shell*>* shells,
					std::vector<GameObject*>* otherItems);

	// Kart executes this when an object is fired
	// Constructs the item object and then adds it to the item lists
	void ItemReleased();

	void OnKartCollisionEnter(Kart* other);
	void OnKartCollisionStay(Kart* other);
	void OnKartCollisionExit(Kart* other);
	
	void OnItemCollisionEnter(ItemBox* other);
	void OnItemCollisionStay(ItemBox* other);
	void OnItemCollisionExit(ItemBox* other);

	void OnWallCollisionEnter(Wall* other);
	void OnWallCollisionStay(Wall* other);
	void OnWallCollisionExit(Wall* other);

	//void OnItemObjectCollisionEnter(MovingItemObject* other);

	void OnShellCollisionEnter(Shell* other);
	void OnOtherItemCollisionEnter();
	
};

#endif