#pragma once
#include <iostream>
#include <vector>

#include "Board.h"

class AsciiBoardRenderer : public BoardRenderer
{
public:
	void draw(const Board& board) const override
	{
		auto size = board.get_size();
		std::vector<std::vector<char>> pieces{ size.y, std::vector<char>(size.x) };
		
		const auto& whitePawns = board.get_piece({PieceType::Pawn, Colour::White });
		const auto& whiteRooks = board.get_piece({PieceType::Rook, Colour::White });
		const auto& whiteKnights = board.get_piece({PieceType::Knight, Colour::White });
		const auto& whiteBishops = board.get_piece({PieceType::Bishop, Colour::White });
		const auto& whiteKing = board.get_piece({PieceType::King, Colour::White });
		const auto& whiteQueens = board.get_piece({PieceType::Queen, Colour::White });
		
		const auto& blackPawns = board.get_piece({PieceType::Pawn, Colour::Black });
		const auto& blackRooks = board.get_piece({PieceType::Rook, Colour::Black });
		const auto& blackKnights = board.get_piece({PieceType::Knight, Colour::Black });
		const auto& blackBishops = board.get_piece({PieceType::Bishop, Colour::Black });
		const auto& blackKing = board.get_piece({PieceType::King, Colour::Black });
		const auto& blackQueens = board.get_piece({PieceType::Queen, Colour::Black });
		
		for (auto rank{ 0u }; rank < size.y; ++rank)
		{
			for (auto file{ 0u }; file < size.x; ++file)
			{
				if (whitePawns.is_set(file, rank))
				{
					pieces[rank][file] = 'P';
				}
				if (whiteRooks.is_set(file, rank))
				{
					pieces[rank][file] = 'R';
				}
				if (whiteKnights.is_set(file, rank))
				{
					pieces[rank][file] = 'N';
				}
				if (whiteBishops.is_set(file, rank))
				{
					pieces[rank][file] = 'B';
				}
				if (whiteKing.is_set(file, rank))
				{
					pieces[rank][file] = 'K';
				}
				if (whiteQueens.is_set(file, rank))
				{
					pieces[rank][file] = 'Q';
				}
				if (blackPawns.is_set(file, rank))
				{
					pieces[rank][file] = 'p';
				}
				if (blackRooks.is_set(file, rank))
				{
					pieces[rank][file] = 'r';
				}
				if (blackKnights.is_set(file, rank))
				{
					pieces[rank][file] = 'n';
				}
				if (blackBishops.is_set(file, rank))
				{
					pieces[rank][file] = 'b';
				}
				if (blackKing.is_set(file, rank))
				{
					pieces[rank][file] = 'k';
				}
				if (blackQueens.is_set(file, rank))
				{
					pieces[rank][file] = 'q';
				}
	
			}
		}

		std::ostringstream ss;
		for (const auto& rank : pieces)
		{
			for (const auto& file : rank)
			{
				ss << (file == 0 ? '.' : file);
			}
			ss << '\n';
		}

		std::cout << ss.str() << std::endl;
	}
};

class FENRenderer : public BoardRenderer
{
public:
	void draw(const Board& /*board*/) const override
	{
		std::cout << "FEN" << std::endl;
	}
};