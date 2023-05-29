#include "BoardBuilder.h"

BoardBuilder::BoardBuilder()
{
	register_default_pieces();
}

void BoardBuilder::new_rank() noexcept
{
	_board.emplace_back();
}

void BoardBuilder::set_blank(int rank, int blank)
{
	assert(_board.size() > rank);
	auto& r = _board.at(rank);

	//for (auto i{ 0 }; i < blank; ++i)
	{
		r.insert(r.end(), blank, '0');
	}

}

void BoardBuilder::set_piece(int rank, char piece)
{
	assert(_board.size() > rank);
	auto& r = _board.at(rank);

	r.emplace_back(piece);
}

void BoardBuilder::set_next_player(std::string_view player)
{
	if (player.compare("w") == 0)
	{
		_nextPlayer = Colour::White;
	}
	else if (player.compare("b") == 0)
	{
		_nextPlayer = Colour::Black;
	}
	else
	{
		assert(false);
	}
}

void BoardBuilder::set_castling(std::string_view /*castling*/)
{
}

void BoardBuilder::set_enpassant_square(std::string_view square)
{
	assert(square.length() == 2);
	auto file = std::tolower(square[0]) - 'a';
	auto rank = square[1] - '0';
	_enpassant = { rank, file };
}

void BoardBuilder::set_halfmove(std::string_view halfmove)
{
	_halfmove = std::stoi(halfmove.data());
}

void BoardBuilder::set_fullmove(std::string_view fullmove)
{
	_fullmove = std::stoi(fullmove.data());
}

Board BoardBuilder::build() const noexcept
{
	ENGINE_ASSERT(!_board.empty(), "Board is empty");
	const auto numRanks = _board.size();
	const auto numFiles = _board.at(0).size();

	for (const auto& r : _board)
	{
		ENGINE_ASSERT(r.size() == numFiles, "Board does not have same sized ranks")
	}
	
	Board board({numFiles, numRanks}, nullptr );

	for (auto r{ 0 }; r < numRanks; ++r)
	{
		const auto& rank = _board.at(r);
		for (auto f{ 0 }; f < numFiles; ++f)
		{
			const auto& df = rank.at(f);
			if (df != '0')
			{
				const auto& piece = _pieceMap.at(df);

				auto& bitboard = board._pieces.at(piece.colour).at(piece.type);
				bitboard.set_square(f, r);
			}
		}
		
	}

	return board;
}
void BoardBuilder::register_default_pieces()
{
	_pieceMap.emplace('p', Piece{ PieceType::Pawn, Colour::Black });
	_pieceMap.emplace('r', Piece{ PieceType::Rook, Colour::Black });
	_pieceMap.emplace('n', Piece{ PieceType::Knight, Colour::Black });
	_pieceMap.emplace('b', Piece{ PieceType::Bishop, Colour::Black });
	_pieceMap.emplace('k', Piece{ PieceType::King, Colour::Black });
	_pieceMap.emplace('q', Piece{ PieceType::Queen, Colour::Black });

	_pieceMap.emplace('P', Piece{ PieceType::Pawn, Colour::White });
	_pieceMap.emplace('R', Piece{ PieceType::Rook, Colour::White });
	_pieceMap.emplace('N', Piece{ PieceType::Knight, Colour::White });
	_pieceMap.emplace('B', Piece{ PieceType::Bishop, Colour::White });
	_pieceMap.emplace('K', Piece{ PieceType::King, Colour::White });
	_pieceMap.emplace('Q', Piece{ PieceType::Queen, Colour::White });
}
