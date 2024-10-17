#include "Assertion.h"
#include "ButtonUI.h"
#include "EntityManager.h"
#include "GameUtils.h"
#include "PanelUI.h"
#include "ResourceManager.h"
#include "RenderManager2D.h"
#include "RenderStateManager.h"
#include "TextUI.h"
#include "TTFont.h"
#include "UIManager.h"

#include "GameApp.h"
#include "GamePauseScene2D.h"
#include "GamePlayScene2D.h"
#include "GameTitleScene2D.h"
#include "MainCamera2D.h"

GameApp::GameApp() : IApp("Tetris2D", 200, 200, 600, 800, false, false)
{}

GameApp::~GameApp()
{}

void GameApp::Startup()
{
	RenderStateManager::GetRef().SetAlphaBlendMode(true);

	LoadResource();

	MainCamera2D* mainCamera = EntityManager::GetRef().Create<MainCamera2D>();
	EntityManager::GetRef().Register("MainCamera", mainCamera);

	gameTitleScene_ = std::make_unique<GameTitleScene2D>();
	AddSceneByName("GameTitleScene", gameTitleScene_.get());

	gamePlayScene_ = std::make_unique<GamePlayScene2D>();
	AddSceneByName("GamePlayScene", gamePlayScene_.get());

	gamePauseScene_ = std::make_unique<class GamePauseScene2D>();
	AddSceneByName("GamePauseScene", gamePauseScene_.get());

	SetCurrentScene(gameTitleScene_.get());
}

void GameApp::Shutdown()
{
	gamePauseScene_.reset();
	gamePlayScene_.reset();
	gameTitleScene_.reset();
}

void GameApp::LoadResource()
{
	std::string resourcePath = "Tetris2D\\Res\\";
	std::string fontPath = resourcePath + "Font\\SeoulNamsanEB.ttf";
	std::array<int32_t, 2> fontSizes = { 32, 128, };
	for (const auto& fontSize : fontSizes)
	{
		TTFont* font = ResourceManager::GetRef().Create<TTFont>(fontPath, 0x00, 0x128, static_cast<float>(fontSize));
		ResourceManager::GetRef().Register(GameUtils::PrintF("Font%d", fontSize), font);
	}
}