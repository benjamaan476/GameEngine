#include "../GameEngine/Log/Log.h"

#include "Pieces/Piece.h"
#include "Board.h"
#include "Bitboard.h"
int main()
{
	Log::Init();
	LOG_INFO("Hello");

	auto bitboard4 = Bitboard<8, 8>();

	bitboard4.fillFile(5);
	bitboard4.fillFile(2);
	bitboard4.fillRank(2);
	bitboard4.fillRank(5);
	bitboard4.draw();
	return 0;
}