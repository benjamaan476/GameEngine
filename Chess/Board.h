#pragma once

#include "../GameEngine/EngineCore.h"

#include <iostream>

class BoardRenderer;
class AsciiRenderer;

class Board
{
public:
	Board() = default;
	explicit Board(uint2 size) noexcept : size{ size }
	{
		board.resize(size.x, std::vector<std::optional<Piece>>(size.y));

		//renderer = std::make_unique<BoardRenderer>(new AsciiRenderer());
	}

	uint2 getSize() const noexcept { return size; }
	std::optional<Piece> getPiece(uint2 location) const noexcept
	{
		if (location.x > board.size() || location.y > board[0].size())
		{
			LOG_WARN("Attempted to index out of board size");
			return {};
		}
		return board[location.x][location.y];
	}

	void setPiece(Piece piece, uint2 location) noexcept
	{
		if (location.x > board.size() || location.y > board[0].size())
		{
			LOG_WARN("Attempted to index out of board size");
			return;
		}
		//Put capture event here?
		// if(board[location.x][location.y].has_value())
		//{
		//	Event.Capture(capturedPiece);
		//}

		board[location.x][location.y] = piece;
	}

	std::vector<std::vector<std::optional<Piece>>> getBoard() const noexcept { return board; }
private:

	uint2 size{ 8, 8 };

	std::vector<std::vector<std::optional<Piece>>> board{};

	//std::unique_ptr<BoardRenderer> renderer;
};

class BoardRenderer
{
	virtual void draw() = 0;
};

class AsciiBoardRenderer : public BoardRenderer
{
	Board board;
	void draw() override
	{
		std::cout << "\n";
		for (const auto& row : board.getBoard())
		{
			std::ostringstream ss;
			for (const auto& piece : row)
			{
				ss << "|";
				if (piece.has_value())
				{
					ss << "*";
				}
				else
				{
					ss << " ";
				}
			}

			std::cout << ss.str() << "|\n";
		}
	}
};