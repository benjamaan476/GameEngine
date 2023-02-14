#include "../GameEngine/Log/Log.h"

#include "Pieces/Piece.h"
#include "Board.h"
#include "Bitboard.h"
int main()
{
	Log::Init();
	LOG_INFO("Hello");
	int i = 0;
	
	auto piece = Piece(PieceType::Bishop, Colour::White);

	auto type = piece.getType();

	Board board{ {8, 8} };

	board.setPiece(piece, { 0, 0 });
	board.draw();

	auto bitboard4 = Bitboard<9,8>::fileMask(3);
	bitboard4.draw();
	return 0;
}