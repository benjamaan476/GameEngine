#pragma once

#include "../GameEngine/EngineCore.h"
#include "Pieces/Piece.h"
#include <iostream>

class Board;
class BoardRenderer
{
public:
	virtual void draw(const Board& board) = 0;
}; 

class AsciiBoardRenderer : public BoardRenderer
{
public:
	void draw(const Board& board) override;
};

class Board
{
public:
	Board() = default;
	explicit Board(uint2 size) noexcept : size{ size }
	{
		board.resize(size.x * size.y);

		renderer = std::unique_ptr<BoardRenderer>(new AsciiBoardRenderer());
	}

	uint2 getSize() const noexcept { return size; }
	std::optional<Piece> getPiece(uint2 location) const noexcept
	{
		if (location.x > size.x || location.y > size.y)
		{
			LOG_WARN("Attempted to index out of board size");
			return {};
		}
		return board[location.y * size.x + location.x];
	}

	void setPiece(Piece piece, uint2 location) noexcept
	{
		if (location.x > size.x || location.y > size.y)
		{
			LOG_WARN("Attempted to index out of board size");
			return;
		}
		//Put capture event here?
		// if(board[location.x][location.y].has_value())
		//{
		//	Event.Capture(capturedPiece);
		//}

		board[location.y * size.x + location.x] = piece;
	}

	std::vector<std::optional<Piece>> getBoard() const noexcept { return board; }

	void draw()
	{
		renderer->draw(*this);
	}
private:

	uint2 size{ 8, 8 };

	std::vector<std::optional<Piece>> board{};

	std::unique_ptr<BoardRenderer> renderer;
};

