#ifndef RED_SHELL_H
#define RED_SHELL_H

#include "Shell.h"

#include <vector>

//class Shell;

class RedShell : public Shell {
private:
	std::vector<Kart*>* m_karts;	// Pointer to list of active Kart pointers
	Kart* m_targetKart;
	Vector3 m_targetPosition;
	//Vector3 m_localForward;
public: 
	RedShell(Mesh* mesh,
	Shader* shader,
	Texture* texture,
	Vector3 position,
	Vector3 direction,
	float rotY,
	std::vector<Kart*>* karts);

	void Update(float timestep);

	Vector3 GetLocalForward();
};

#endif