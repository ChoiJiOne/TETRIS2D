#include "Assertion.h"
#include "EntityManager.h"
#include "GameUtils.h"
#include "RenderManager2D.h"
#include "ResourceManager.h"
#include "TTFont.h"

#include "GameHistoryTracker.h"
#include "HistoryViewer.h"

HistoryViewer::HistoryViewer()
{
	font_ = ResourceManager::GetRef().GetByName<TTFont>("Font24");
	gameHistoryTracker_ = EntityManager::GetRef().GetByName<GameHistoryTracker>("GameHistoryTracker");

	startHistoryTextPos_ = Vec2f(-150.0f, 250.0f);
	historyTextColor_ = Vec4f(1.0f, 1.0f, 1.0f, 1.0f);
	historyTextStride_ = 30.0f;

	bIsInitialized_ = true;
}

HistoryViewer::~HistoryViewer()
{
	if (bIsInitialized_)
	{
		Release();
	}
}

void HistoryViewer::Tick(float deltaSeconds)
{
}

void HistoryViewer::Render()
{
	const std::list<GameHistoryTracker::History>& histories = gameHistoryTracker_->GetHistories();

	Vec2f historyTextPos = startHistoryTextPos_;
	for (int32_t index = 0; index < currentHistoryTextSize_; ++index)
	{
		RenderManager2D::GetRef().DrawString(font_, historyTexts_[index], historyTextPos, historyTextColor_);
		historyTextPos.y -= historyTextStride_;
	}
}

void HistoryViewer::Release()
{
	CHECK(bIsInitialized_);

	gameHistoryTracker_ = nullptr;
	font_ = nullptr;

	bIsInitialized_ = false;
}

void HistoryViewer::UpdateRecentHistory()
{
	const std::list<GameHistoryTracker::History>& histories = gameHistoryTracker_->GetHistories();
	if (histories.empty())
	{
		return;
	}

	currentHistoryTextSize_ = 0;
	for (auto& rit = histories.rbegin(); rit != histories.rend(); ++rit)
	{
		const GameHistoryTracker::History& recent = *rit;

		historyTexts_[currentHistoryTextSize_++] = GameUtils::PrintF(L"%s | %4d", recent.time.c_str(), recent.score);
		if (currentHistoryTextSize_ >= MAX_VIEW_HISTORY)
		{
			break;
		}
	}
}