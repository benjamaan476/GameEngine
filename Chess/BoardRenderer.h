#pragma once
#include <iostream>

#include "Board.h"

class AsciiBoardRenderer : public BoardRenderer
{
public:
	void draw(const Board& board) const override
	{
		std::cout << "Z" << std::endl;
	}
};

class FENRenderer : public BoardRenderer
{
public:
	void draw(const Board& board) const override
	{
		std::cout << "FEN" << std::endl;
	}
};