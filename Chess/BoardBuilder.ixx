module;

#include "../GameEngine/EngineCore.h"
#include "ChessCore.h"

#include <vector>

export module BoardBuilder;

import Board;

export class BoardBuilder
{
public:

	void new_rank() { _board.emplace_back(); }

	void set_piece(const int2& position, char piece)
	{
		assert(_board.size() > position.x);
		auto& rank = _board.at(position.x);

		if(position.y >= rank.size())
		{
			rank.resize(position.y + 1);
		}
		rank[position.y] = piece;
	}

	void set_next_player(Colour player)
	{
		_nextPlayer = player;
	}

	Board build() { return Board{ {8, 8}, nullptr }; };

private:
	std::vector<std::vector<int>> _board;
	Colour _nextPlayer;
};