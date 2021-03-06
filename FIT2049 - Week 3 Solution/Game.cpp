/*	FIT2049 - Example Code
*	Game.cpp
*	Created by Elliott Wilson & Mike Yeates - 2016 - Monash University
*/

#include "Game.h"
#include "DirectXTK/CommonStates.h"
#include "TexturedShader.h"
#include "StaticObject.h"
#include "SurveillanceCamera.h"
#include "ThirdPersonCamera.h"

#include "MathsHelper.h"

#include <iostream>

Game::Game()
{
	m_renderer = NULL;
	m_currentCam = NULL;
	m_input = NULL;
	m_unlitShader = NULL;
	m_texturedShader = NULL;
	m_spriteBatch = NULL;
	m_arialFont12 = NULL;
	m_arialFont18 = NULL;

	// Menu buttons
	m_startButton = NULL;
	m_quitButton = NULL;
}

Game::~Game() {}

bool Game::Initialise(Direct3D* renderer, InputController* input)
{
	m_gameOver = false;

	m_renderer = renderer;	
	m_input = input;

	if (!InitShaders())
		return false;

	if (!LoadTextures())
		return false;

	if (!LoadMeshes())
		return false;

	LoadFonts();
	InitUI();
	InitStates();

	// TODO
	// Need to give the collisions manager the moving items as well
	m_collisionManager = new CollisionManager(&m_karts, 
											&m_itemBoxes, 
											&m_walls, 
											&m_shells,
											&m_otherItems);

	Kart* kart = new Kart(Mesh::GetMesh("Kart"), 
		m_texturedShader, 
		Texture::GetTexture("RedKart"), 
		Vector3(0, 0, 0), 
		m_input);
	m_playerKart = kart;
	kart->SetItemPointers(&m_itemTextures,
						&m_itemMeshes,
						m_texturedShader);

	// Give the kart pointers to all the lists it will need
	kart->SetObjects(&m_gameObjects, &m_karts, &m_shells, &m_otherItems);
	kart->SetItemBoxes(&m_itemBoxes);
	kart->SetBalloonPointers("Balloon", "Balloon");
	kart->InitBalloons();
	kart->SetGameObjectIndex(m_gameObjects.size());
	m_gameObjects.push_back(kart);
	m_karts.push_back(kart);

	// Create item boxes
	for (int i = 0; i < 4; i++) {
		ItemBox* itemBox = new ItemBox(Mesh::GetMesh("Box"),
			m_texturedShader,
			Texture::GetTexture("Box"),
			Vector3(MathsHelper::RandomRange(-290, 290), 0, MathsHelper::RandomRange(-290, 290)));
		m_gameObjects.push_back(itemBox);
		m_itemBoxes.push_back(itemBox);
	}

	// Create the enemies
	for (int i = 0; i < 4; i++) {
		int flag = (i == 0) ? 1 : 0;
		int textureIndex = i;
		EnemyKart* enemy = new EnemyKart(Mesh::GetMesh("Kart"),
			m_texturedShader,
			Texture::GetTexture(m_kartTextures[textureIndex]),
			Vector3(i*50, 0, i*50 + 100),
			flag);
		if (flag == 1) {
			enemy->SetPlayerKart(m_playerKart);
		}
		// Give the kart pointers to all the lists it will need
		enemy->SetItemPointers(&m_itemTextures, &m_itemMeshes, m_texturedShader);
		enemy->SetObjects(&m_gameObjects, &m_karts, &m_shells, &m_otherItems);
		enemy->SetItemBoxes(&m_itemBoxes);
		enemy->SetBalloonPointers("Balloon", "Balloon");
		enemy->InitBalloons();
		enemy->SetGameObjectIndex(m_gameObjects.size());
		m_gameObjects.push_back(enemy);
		m_karts.push_back(enemy);
	}

	int playerItemIndex = m_playerKart->GetItemValue();
	m_currentItem = m_currentItemArray[playerItemIndex];
	m_currentItemSprite = Texture::GetTexture(m_currentItem);
	
	m_gameObjects.push_back(new StaticObject(Mesh::GetMesh("Ground"), m_texturedShader, Texture::GetTexture("Grass"), Vector3(0,0,0)));
	m_gameObjects.push_back(new StaticObject(Mesh::GetMesh("RumbleStrip"), m_texturedShader, Texture::GetTexture("RumbleStrip"), Vector3(0, 0, 0)));
	m_gameObjects.push_back(new StaticObject(Mesh::GetMesh("Wall"), m_texturedShader, Texture::GetTexture("Wall"), Vector3(0, 0, 0)));

	// Create the four walls for collisions

	Wall* northWall = new Wall();
	northWall->SetBounds(CBoundingBox(Vector3(-300,0,300),Vector3(300,10,320)));
	northWall->SetFace(Vector3(0, 0, -1));
	m_walls.push_back(northWall);

	Wall* southWall = new Wall();
	southWall->SetBounds(CBoundingBox(Vector3(-300, 0, -320), Vector3(300, 10, -300)));
	southWall->SetFace(Vector3(0, 0, 1));
	m_walls.push_back(southWall);

	Wall* eastWall = new Wall();
	eastWall->SetBounds(CBoundingBox(Vector3(-320, 0, -300), Vector3(-300, 10, 300)));
	eastWall->SetFace(Vector3(1, 0, 0));
	m_walls.push_back(eastWall);

	Wall* westWall = new Wall();
	westWall->SetBounds(CBoundingBox(Vector3(300, 0, -300), Vector3(320, 10, 300)));
	westWall->SetFace(Vector3(-1, 0, 0));
	m_walls.push_back(westWall);

	//m_currentCam = new Camera();
	//m_currentCam = new SurveillanceCamera(kart,0.5f);
	m_currentCam = new ThirdPersonCamera(kart, Vector3(0, 7, -20));

	//m_stateMachine->ChangeState(GameStates::GAMEPLAY_STATE);

	return true;
}

void Game::InitStates()
{
	m_stateMachine = new StateMachine<GameStates, Game>(this, GameStates::NO_STATE);
	
	m_stateMachine->RegisterState(GameStates::GAMEPLAY_STATE, 
		&Game::Gameplay_OnEnter,
		&Game::Gameplay_OnUpdate, 
		&Game::Gameplay_OnRender, 
		&Game::Gameplay_OnExit);

	// Menu
	m_stateMachine->RegisterState(GameStates::MENU_STATE,
		&Game::Menu_OnEnter,
		&Game::Menu_OnUpdate,
		&Game::Menu_OnRender,
		&Game::Menu_OnExit);

	// Pause
	m_stateMachine->RegisterState(GameStates::PAUSE_STATE,
		&Game::Pause_OnEnter,
		&Game::Pause_OnUpdate,
		&Game::Pause_OnRender,
		&Game::Pause_OnExit);

	// GameOver
	m_stateMachine->RegisterState(GameStates::GAMEOVER_STATE,
		&Game::GameOver_OnEnter,
		&Game::GameOver_OnUpdate,
		&Game::GameOver_OnRender,
		&Game::GameOver_OnExit);

	// Initialise m_stateMachine to MENUSTATE
	m_stateMachine->ChangeState(GameStates::MENU_STATE);
}

bool Game::InitShaders()
{
	m_unlitShader = new Shader();
	if (!m_unlitShader->Initialise(m_renderer->GetDevice(), L"Assets/Shaders/VertexShader.vs", L"Assets/Shaders/VertexColourPixelShader.ps"))
	{
		return false;
	}

	m_texturedShader = new TexturedShader();
	if (!m_texturedShader->Initialise(m_renderer->GetDevice(), L"Assets/Shaders/VertexShader.vs", L"Assets/Shaders/TexturedPixelShader.ps"))
	{
		return false;
	}

	return true;
}

bool Game::LoadTextures()
{
	if (!Texture::LoadFromFile(L"Assets/Textures/Button.png", "ButtonWhite", m_renderer))
		return false;
	
	if (!Texture::LoadFromFile(L"Assets/Textures/kart_red.png", "RedKart", m_renderer))
		return false;

	if (!Texture::LoadFromFile(L"Assets/Textures/kart_blue.png", "BlueKart", m_renderer))
		return false;

	m_kartTextures.push_back("BlueKart");

	if (!Texture::LoadFromFile(L"Assets/Textures/kart_green.png", "GreenKart", m_renderer))
		return false;

	m_kartTextures.push_back("GreenKart");

	if (!Texture::LoadFromFile(L"Assets/Textures/kart_orange.png", "OrangeKart", m_renderer))
		return false;

	m_kartTextures.push_back("OrangeKart");

	if (!Texture::LoadFromFile(L"Assets/Textures/kart_purple.png", "PurpleKart", m_renderer))
		return false;

	m_kartTextures.push_back("PurpleKart");

	if (!Texture::LoadFromFile(L"Assets/Textures/item_box.png", "Box", m_renderer))
		return false;

	// Load the green shell
	if (!Texture::LoadFromFile(L"Assets/Textures/shell_green.png", "GreenShell", m_renderer))
		return false;

	m_itemTextures.push_back("GreenShell");

	if (!Texture::LoadFromFile(L"Assets/Textures/sprite_green_shell.png", "GreenShellIcon", m_renderer)) {
		return false;
	}
	m_currentItemArray.push_back("GreenShellIcon");

	// Load the red shell
	if (!Texture::LoadFromFile(L"Assets/Textures/shell_red.png", "RedShell", m_renderer))
		return false;
	m_itemTextures.push_back("RedShell");
	if (!Texture::LoadFromFile(L"Assets/Textures/sprite_red_shell.png", "RedShellIcon", m_renderer)) {
		return false;
	}
	m_currentItemArray.push_back("RedShellIcon");

	// Load Banana
	if(!Texture::LoadFromFile(L"Assets/Textures/banana.png", "Banana", m_renderer))
		return false;
	m_itemTextures.push_back("Banana");
	if (!Texture::LoadFromFile(L"Assets/Textures/sprite_banana.png", "BananaIcon", m_renderer)) {
		return false;
	}
	m_currentItemArray.push_back("BananaIcon");

	// Load BadBox
	if (!Texture::LoadFromFile(L"Assets/Textures/item_box_bad.png", "BadBox", m_renderer))
		return false;
	m_itemTextures.push_back("BadBox");
	if (!Texture::LoadFromFile(L"Assets/Textures/sprite_bad_box.png", "BadBoxIcon", m_renderer)) {
		return false;
	}
	m_currentItemArray.push_back("BadBoxIcon");

	// Load Star
	if (!Texture::LoadFromFile(L"Assets/Textures/sprite_star.png", "StarIcon", m_renderer)) {
		return false;
	}
	m_currentItemArray.push_back("StarIcon");


	// Load Balloon
	if (!Texture::LoadFromFile(L"Assets/Textures/balloon.png", "Balloon", m_renderer))
		return false;
	

	if (!Texture::LoadFromFile(L"Assets/Textures/grass.jpg", "Grass", m_renderer))
		return false;

	if (!Texture::LoadFromFile(L"Assets/Textures/wall.png", "Wall", m_renderer))
		return false;

	if (!Texture::LoadFromFile(L"Assets/Textures/rumble_strip.jpg", "RumbleStrip", m_renderer))
		return false;
	
	return true;
}

bool Game::LoadMeshes()
{
	if (!Mesh::LoadFromFile(L"Assets/Meshes/kart.obj", "Kart", m_renderer))
		return false;

	if (!Mesh::LoadFromFile(L"Assets/Meshes/item_box.obj", "Box", m_renderer))
		return false;

	if (!Mesh::LoadFromFile(L"Assets/Meshes/ground.obj", "Ground", m_renderer))
		return false;

	if (!Mesh::LoadFromFile(L"Assets/Meshes/wall.obj", "Wall", m_renderer))
		return false;

	if (!Mesh::LoadFromFile(L"Assets/Meshes/rumble_strip.obj", "RumbleStrip", m_renderer))
		return false;

	if (!Mesh::LoadFromFile(L"Assets/Meshes/shell.obj", "Shell", m_renderer))
		return false;

	if (!Mesh::LoadFromFile(L"Assets/Meshes/banana.obj", "Banana", m_renderer))
		return false;

	if (!Mesh::LoadFromFile(L"Assets/Meshes/balloon.obj", "Balloon", m_renderer))
		return false;

	// push the shell on twice, once for the red, once for the green
	m_itemMeshes.push_back("Shell");
	m_itemMeshes.push_back("Shell");
	m_itemMeshes.push_back("Banana");
	m_itemMeshes.push_back("Box");


	return true;
}

void Game::LoadFonts()
{
	// There's a few different size fonts in there, you know
	m_arialFont12 = new SpriteFont(m_renderer->GetDevice(), L"Assets/Fonts/Arial-12pt.spritefont");
	m_arialFont18 = new SpriteFont(m_renderer->GetDevice(), L"Assets/Fonts/Arial-18pt.spritefont");
}

void Game::InitUI()
{
	m_spriteBatch = new SpriteBatch(m_renderer->GetDeviceContext());

	// Init Buttons
	m_startButton = new Button(128, 64, Texture::GetTexture("ButtonWhite"), L"Start Game", Vector2(448, 384), m_spriteBatch, m_arialFont12, m_input, [this]
	{
		m_stateMachine->ChangeState(GameStates::GAMEPLAY_STATE);
	});

	m_quitButton = new Button(128, 64, Texture::GetTexture("ButtonWhite"), L"Quit", Vector2(576, 384), m_spriteBatch, m_arialFont12, m_input, [this]
	{
		PostQuitMessage(0);
	});

}

void Game::Update(float timestep)
{
	m_input->BeginUpdate();
	
	// The state machine knows what state we're in, so things are nice and simple out here
	m_stateMachine->Update(timestep);

	m_input->EndUpdate();
}

void Game::Render()
{
	m_renderer->BeginScene(0.2f, 0.2f, 0.2f, 1.0f);

	// Render whatever state we're in
	m_stateMachine->Render();

	m_renderer->EndScene();
}

void Game::Gameplay_OnEnter()
{
	OutputDebugString("GamePlay OnEnter\n");
}

void Game::Gameplay_OnUpdate(float timestep)
{
	// Update all our gameobjects. What they really are doesn't matter
	for (unsigned int i = 0; i < m_gameObjects.size(); i++)
	{
		int status = m_gameObjects[i]->GetStatus();
		if (status != 0) {
			m_gameObjects[i]->Update(timestep);
		}
	}

	for (unsigned int i = 0; i < m_shells.size(); i++) {
		m_shells[i]->Update(timestep);
	}

	for (unsigned int i = 0; i < m_otherItems.size(); i++) {
		m_otherItems[i]->Update(timestep);
	}

	// Check for pause
	if (m_input->GetKeyDown('P')) {
		m_stateMachine->ChangeState(GameStates::PAUSE_STATE);
	}

	m_collisionManager->CheckCollisions();

	// Finds all the items that need to be removed from m_gameObjects
	// remove the dead items so they don't get redrawn
	int index = 0;
	while (index < m_gameObjects.size()) {
		int status = m_gameObjects[index]->GetStatus();
		if (status == 0) {
			m_gameObjects.erase(m_gameObjects.begin() + index);
		}
		index++;
	}

	// checks if any karts need to be removed
	index = 0;
	while (index < m_karts.size()) {
		int status = m_karts[index]->GetStatus();
		if (status == 0) {
			m_karts.erase(m_karts.begin() + index);
		}
		index++;
	}

	// checks if any shells need to be removed
	index = 0;
	while (index < m_shells.size()) {
		int status = m_shells[index]->GetStatus();
		if (status == 0) {
			delete m_shells[index];
			m_shells.erase(m_shells.begin() + index);
		}
		index++;
	}

	// checks if any other items need to be removed from the game
	index = 0;
	while (index < m_otherItems.size()) {
		int status = m_otherItems[index]->GetStatus();
		if (status == 0) {
			delete m_otherItems[index];
			m_otherItems.erase(m_otherItems.begin() + index);
		}
		index++;
	}

	m_currentCam->Update(timestep);

	// checks if the player kart is still alive, or
	// checks if there is more than one player left
	// If not the game is over
	if (m_playerKart->GetStatus() == 0 || m_karts.size() <= 1) {
		if (m_playerKart->GetStatus() == 0) {
			m_playerWin = false;
		}
		else {
			m_playerWin = true;
		}
		m_stateMachine->ChangeState(GameStates::GAMEOVER_STATE);
	}
}

void Game::Gameplay_OnRender()
{
	// Draw our gameobjects
	for (unsigned int i = 0; i < m_gameObjects.size(); i++)
	{
		m_gameObjects[i]->Render(m_renderer, m_currentCam);
	}

	for (unsigned int i = 0; i < m_shells.size(); i++) {
		m_shells[i]->Render(m_renderer, m_currentCam);
	}

	for (unsigned int i = 0; i < m_otherItems.size(); i++) {
		m_otherItems[i]->Render(m_renderer, m_currentCam);
	}
	DrawGameUI();
}

void Game::Gameplay_OnExit()
{
	OutputDebugString("GamePlay OnExit\n");
}

void Game::Menu_OnEnter() {
	OutputDebugString("Menu OnEnter\n");
}

void Game::Menu_OnUpdate(float timestep) {
	m_startButton->Update();
	m_quitButton->Update();
}
void Game::Menu_OnRender() {
	DrawMenuUI();
}
void Game::Menu_OnExit() {
	OutputDebugString("Menu OnExit\n");
}

void Game::Pause_OnEnter() {
	OutputDebugString("Pause OnEnter\n");
}

void Game::Pause_OnUpdate(float timestep) {
	if (m_input->GetKeyDown('P')) {
		m_stateMachine->ChangeState(GameStates::GAMEPLAY_STATE);
	}
}

void Game::Pause_OnRender() {
	Gameplay_OnRender();
	DrawPauseUI();
}
void Game::Pause_OnExit() {
	OutputDebugString("Pause OnExit\n");
}

void Game::GameOver_OnEnter() {}

void Game::GameOver_OnUpdate(float timestep) {}

void Game::GameOver_OnRender(){
	Gameplay_OnRender();
	DrawGameOverUI();
}
void Game::GameOver_OnExit(){}

void Game::DrawGameUI()
{
	// Sprites don't use a shader 
	m_renderer->SetCurrentShader(NULL);

	CommonStates states(m_renderer->GetDevice());
	m_spriteBatch->Begin(SpriteSortMode_Deferred, states.NonPremultiplied());

	m_arialFont12->DrawString(m_spriteBatch, L"P to toggle pause",
		Vector2(450, 700), Color(0.5f, 0.5f, 0.5f), 0, Vector2(0, 0));

	// IMPORTANT
	// Need to instantiate the new int
	int playerItemIndex = m_playerKart->GetItemValue();
	// Only draw if the player currently has an item
	if (playerItemIndex >= 0) {
		m_currentItem = m_currentItemArray[playerItemIndex];
		m_currentItemSprite = Texture::GetTexture(m_currentItem);
		m_spriteBatch->Draw(m_currentItemSprite->GetShaderResourceView(), Vector2(20, 100), Color(1.0f, 1.0f, 1.0f));
	}

	m_spriteBatch->End();
}

void Game::DrawMenuUI() {

	m_renderer->SetCurrentShader(NULL);

	CommonStates states(m_renderer->GetDevice());
	m_spriteBatch->Begin(SpriteSortMode_Deferred, states.NonPremultiplied());

	m_startButton->Render();
	m_quitButton->Render();
	m_arialFont18->DrawString(m_spriteBatch, L"Battle Kart",
		Vector2(420, 290), Color(1, 1, 1), 0, Vector2(0, 0));

	m_spriteBatch->End();

}

void Game::DrawPauseUI() {

	m_renderer->SetCurrentShader(NULL);

	CommonStates states(m_renderer->GetDevice());
	m_spriteBatch->Begin(SpriteSortMode_Deferred, states.NonPremultiplied());

	m_arialFont18->DrawString(m_spriteBatch, L"Pause",
		Vector2(470, 200), Color(1, 1, 1), 0, Vector2(0, 0));

	m_spriteBatch->End();
}

void Game::DrawGameOverUI() {

	m_renderer->SetCurrentShader(NULL);

	CommonStates states(m_renderer->GetDevice());
	m_spriteBatch->Begin(SpriteSortMode_Deferred, states.NonPremultiplied());

	m_arialFont18->DrawString(m_spriteBatch, L"Game Over",
		Vector2(470, 200), Color(1, 1, 1), 0, Vector2(0, 0));
	if (m_playerWin) {
		m_arialFont18->DrawString(m_spriteBatch, L"You Win!",
			Vector2(470, 300), Color(1, 1, 1), 0, Vector2(0, 0));
	}
	else {
		m_arialFont18->DrawString(m_spriteBatch, L"You Lose!",
			Vector2(470, 300), Color(1, 1, 1), 0, Vector2(0, 0));
	}

	m_spriteBatch->End();
}

void Game::Shutdown()
{
	if (m_currentCam)
	{
		delete m_currentCam;
		m_currentCam = NULL;
	}

	if (m_unlitShader)
	{
		m_unlitShader->Release();
		delete m_unlitShader;
		m_unlitShader = NULL;
	}

	if (m_texturedShader)
	{
		m_texturedShader->Release();
		delete m_texturedShader;
		m_texturedShader = NULL;
	}

	if (m_stateMachine)
	{
		delete m_stateMachine;
		m_stateMachine = NULL;
	}

	if (m_spriteBatch)
	{
		delete m_spriteBatch;
		m_spriteBatch = NULL;
	}
	
	if (m_arialFont12)
	{
		delete m_arialFont12;
		m_arialFont12 = NULL;
	}

	if (m_arialFont18)
	{
		delete m_arialFont18;
		m_arialFont18 = NULL;
	}

	Texture::ReleaseAllTextures();
	Mesh::ReleaseAllMeshes();

}