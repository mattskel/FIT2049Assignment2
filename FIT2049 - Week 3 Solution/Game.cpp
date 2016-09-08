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

	m_collisionManager = new CollisionManager(&m_karts, &m_itemBoxes);

	Kart* kart = new Kart(Mesh::GetMesh("Kart"), 
		m_texturedShader, 
		Texture::GetTexture("RedKart"), 
		Vector3(0, 0, 0), 
		m_input);
	m_gameObjects.push_back(kart);
	m_karts.push_back(kart);

	// Create item boxes
	for (int i = 0; i < 3; i++) {
		ItemBox* itemBox = new ItemBox(Mesh::GetMesh("Box"),
			m_texturedShader,
			Texture::GetTexture("Box"),
			Vector3(i * 30, 0, 0));
		m_gameObjects.push_back(itemBox);
		m_itemBoxes.push_back(itemBox);
	}

	// Create the enemies
	for (int i = 0; i < 3; i++) {
		EnemyKart* enemy = new EnemyKart(Mesh::GetMesh("Kart"),
			m_texturedShader,
			Texture::GetTexture("RedKart"),
			Vector3(i*50, 0, i*50 + 100));
		m_gameObjects.push_back(enemy);
		m_karts.push_back(enemy);
	}
	
	m_gameObjects.push_back(new StaticObject(Mesh::GetMesh("Ground"), m_texturedShader, Texture::GetTexture("Grass"), Vector3(0,0,0)));

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

	if (!Texture::LoadFromFile(L"Assets/Textures/item_box.png", "Box", m_renderer))
		return false;

	if (!Texture::LoadFromFile(L"Assets/Textures/grass.jpg", "Grass", m_renderer))
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
		m_gameObjects[i]->Update(timestep);
	}

	// Check for pause
	if (m_input->GetKeyDown('P')) {
		m_stateMachine->ChangeState(GameStates::PAUSE_STATE);
	}

	m_collisionManager->CheckCollisions();

	m_currentCam->Update(timestep);
}

void Game::Gameplay_OnRender()
{
	// Draw our gameobjects
	for (unsigned int i = 0; i < m_gameObjects.size(); i++)
	{
		m_gameObjects[i]->Render(m_renderer, m_currentCam);
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

void Game::DrawGameUI()
{
	// Sprites don't use a shader 
	m_renderer->SetCurrentShader(NULL);

	CommonStates states(m_renderer->GetDevice());
	m_spriteBatch->Begin(SpriteSortMode_Deferred, states.NonPremultiplied());

	m_arialFont12->DrawString(m_spriteBatch, L"P to toggle pause",
		Vector2(450, 700), Color(0.5f, 0.5f, 0.5f), 0, Vector2(0, 0));

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