#include "Assertion.h"
#include "RenderManager2D.h"

#include "Board.h"

Board::Board(const Vec2f& center, float cellSize, uint32_t row, uint32_t col)
	: center_(center)
	, cellSize_(cellSize)
	, row_(row)
	, col_(col)
	, inlines_((row_ - 2)* (col_ - 2) * 2)
	, cells_(row_* col_)
	, removeColumn_(col_)
{
	size_.x = static_cast<float>(row) * cellSize_;
	size_.y = static_cast<float>(col) * cellSize_;

	outlineColor_ = Vec4f(1.0f, 1.0f, 1.0f, 1.0f);
	inlineColor_ = Vec4f(0.3f, 0.3f, 0.3f, 0.3f);
	
	CleanupInlines(inlines_);
	CleanupCells(cells_);
	startPos_ = CalculateCellPos(4, 0);

	maxRemoveStepTime_ = 1.0f;
	maxFillStepTime_ = 1.0f;

	bIsInitialized_ = true;
}

Board::~Board()
{
	if (bIsInitialized_)
	{
		Release();
	}
}

void Board::Tick(float deltaSeconds)
{
	switch (status_)
	{
	case Status::WAIT:
	{

	}
	break;

	case Status::REMOVE:
	{
		removeStepTime_ -= deltaSeconds;
		for (uint32_t col = 0; col < col_; ++col)
		{
			if (removeColumn_[col])
			{
				for (uint32_t row = 0; row < row_; ++row)
				{
					uint32_t index = row + col * row_;
					Vec4f color = cells_[index].first.GetColor();
					color.w = removeStepTime_ / maxRemoveStepTime_;
					cells_[index].first.SetColor(color);
				}
			}
		}

		if (removeStepTime_ <= 0.0f)
		{
			for (uint32_t col = 0; col < col_; ++col)
			{
				if (removeColumn_[col])
				{
					for (uint32_t row = 0; row < row_; ++row)
					{
						uint32_t index = row + col * row_;
						Vec2f center = CalculateCellPos(row, col);
						cells_[index] = { Block(Rect2D(center, cellSize_), Vec4f(0.0f, 0.0f, 0.0f, 0.0f)), false };
					}
				}
			}
			
			status_ = Status::CONFIRM;
		}
	}
	break;

	case Status::CONFIRM:
	{

	}
	break;

	case Status::FILL:
	{

	}
	break;
	}
}

void Board::Render()
{
	RenderManager2D& renderMgr = RenderManager2D::Get();

	renderMgr.DrawRectWireframe(center_, size_.x, size_.y, outlineColor_, 0.0f);

	for (uint32_t index = 0; index < inlines_.size(); index += 2)
	{
		renderMgr.DrawLine(inlines_[index + 0], inlines_[index + 1], inlineColor_);
	}

	for (const auto& cell : cells_)
	{
		if (cell.second)
		{
			const Block& block = cell.first;
			const Rect2D& bound = block.GetBound();
			renderMgr.DrawRoundRect(bound.center, bound.size.x, bound.size.y, 10.0f, block.GetColor(), 0.0f);
		}
	}
}

void Board::Release()
{
	CHECK(bIsInitialized_);

	bIsInitialized_ = false;
}

bool Board::IsBlocksInside(const Block* blocks, uint32_t count)
{
	for (uint32_t index = 0; index < count; ++index)
	{
		bool bIsBlockInside = false;
		const Rect2D& bound = blocks[index].GetBound();

		for (const auto& cell : cells_)
		{
			const Rect2D cellBound = cell.first.GetBound();

			if (cellBound.Intersect(&bound))
			{
				bIsBlockInside = true;
				break;
			}
		}

		if (!bIsBlockInside)
		{
			return false;
		}
	}
	
	return true;
}

bool Board::CanBlocksDeploy(const Block* blocks, uint32_t count)
{
	for (uint32_t index = 0; index < count; ++index)
	{
		bool bCanBlocksDeploy = true;
		const Rect2D& bound = blocks[index].GetBound();

		for (const auto& cell : cells_)
		{
			const Rect2D cellBound = cell.first.GetBound();

			if (cellBound.Intersect(&bound) && cell.second)
			{
				bCanBlocksDeploy = false;
				break;
			}
		}

		if (!bCanBlocksDeploy)
		{
			return false;
		}
	}

	return true;
}

void Board::DeployBlocks(const Block* blocks, uint32_t count)
{
	for (uint32_t index = 0; index < count; ++index)
	{
		const Block& block = blocks[index];
		const Rect2D& bound = block.GetBound();

		for (auto& cell : cells_)
		{
			const Rect2D cellBound = cell.first.GetBound();

			if (cellBound.Intersect(&bound))
			{
				cell = { block, true };
				break;
			}
		}
	}

	if (!UpdateRemoveColumn())
	{
		status_ = Status::WAIT;
	}
	else
	{
		removeStepTime_ = maxRemoveStepTime_;
		status_ = Status::REMOVE;
	}
}

Vec2f Board::CalculateCellPos(uint32_t row, uint32_t col)
{
	Vec2f cellPos = center_;
	cellPos += Vec2f(-size_.x * 0.5f + (static_cast<float>(row) + 0.5f) * cellSize_, +size_.y * 0.5f - (static_cast<float>(col) + 0.5f) * cellSize_);
	return cellPos;
}

bool Board::UpdateRemoveColumn()
{
	bool bNeedUpdateRemoveColumn = false;

	for (uint32_t col = 0; col < col_; ++col)
	{
		int32_t count = 0;

		for (uint32_t row = 0; row < row_; ++row)
		{
			uint32_t index = row + col * row_;
			if (cells_[index].second)
			{
				count++;
			}
		}

		if (count == row_)
		{
			removeColumn_[col] = true;
			bNeedUpdateRemoveColumn = true;
		}
		else
		{
			removeColumn_[col] = false;
		}
	}

	return bNeedUpdateRemoveColumn;
}

bool Board::IsEmptyColumn(uint32_t col)
{
	for (uint32_t row = 0; row < row_; ++row)
	{
		uint32_t index = row + col * row_;
		if (cells_[index].second)
		{
			return false;
		}
	}

	return true;
}

void Board::CleanupInlines(std::vector<Vec2f>& inlines)
{
	int32_t index = 0;
	for (uint32_t row = 1; row < row_; ++row)
	{
		inlines[index++] = center_ + Vec2f(-size_.x * 0.5f + static_cast<float>(row) * cellSize_, size_.y * 0.5f -    static_cast<float>(0) * cellSize_);
		inlines[index++] = center_ + Vec2f(-size_.x * 0.5f + static_cast<float>(row) * cellSize_, size_.y * 0.5f - static_cast<float>(col_) * cellSize_);
	}

	for (uint32_t col = 1; col < col_; ++col)
	{
		inlines[index++] = center_ + Vec2f(-size_.x * 0.5f +    static_cast<float>(0) * cellSize_, size_.y * 0.5f - static_cast<float>(col) * cellSize_);
		inlines[index++] = center_ + Vec2f(-size_.x * 0.5f + static_cast<float>(row_) * cellSize_, size_.y * 0.5f - static_cast<float>(col) * cellSize_);
	}
}

void Board::CleanupCells(std::vector<std::pair<Block, bool>>& cells)
{
	for (uint32_t col = 0; col < col_; ++col)
	{
		for (uint32_t row = 0; row < row_; ++row)
		{
			uint32_t index = row + col * row_;
			Vec2f center = CalculateCellPos(row, col);

			cells[index] = { Block(Rect2D(center, cellSize_), Vec4f(0.0f, 0.0f, 0.0f, 0.0f)), false };
		}
	}
}