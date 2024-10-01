#include "Assertion.h"
#include "Camera2D.h"
#include "EntityManager.h"
#include "ResourceManager.h"
#include "RenderManager2D.h"

#include "Board.h"
#include "GameApp.h"

GameApp::GameApp() : IApp("Tetris2D", 200, 200, 600, 800, false, false)
{
}

GameApp::~GameApp()
{
}

void GameApp::Startup()
{
	EntityManager& entityMgr = EntityManager::Get();

	mainCamera_ = Camera2D::CreateScreenCamera();
	entityMgr.Register("MainCamera", mainCamera_);

	Board* board = entityMgr.Create<Board>(Vec2f(-50.0f, 0.0f), 30.0f, 10, 20);
	entityMgr.Register("Board", board);

	updateEntities_ =
	{
		mainCamera_,
		board,
	};

	renderEntities_ =
	{
		board,
	};
}

void GameApp::Shutdown()
{
	EntityManager::Get().Destroy(mainCamera_);
	mainCamera_ = nullptr;
}

void GameApp::Run()
{
	RunLoop(
		[&](float deltaSeconds)
		{
			for (auto& entity : updateEntities_)
			{
				entity->Tick(deltaSeconds);
			}


			BeginFrame(0.0f, 0.0f, 0.0f, 1.0f);

			RenderManager2D::Get().BatchRenderEntities(mainCamera_, renderEntities_.data(), renderEntities_.size());

			EndFrame();
		}
	);
}