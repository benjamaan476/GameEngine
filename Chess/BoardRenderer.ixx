module;

#include <iostream>

export module BoardRenderer;
import Board;

export class AsciiBoardRenderer : public BoardRenderer
{
public:
	void draw(const Board& board) const override
	{
		std::cout << "Z" << std::endl;
	}
};

export class FENRenderer : public BoardRenderer
{
public:
	void draw(const Board& board) const override
	{
		std::cout << "FEN" << std::endl;
	}
};