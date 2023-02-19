#include "../GameEngine/Log/Log.h"

#include "Pieces/Piece.h"
#include "Board.h"
#include "Bitboard.h"
#include "MoveSet.h"
int main()
{
	Log::Init();
	LOG_INFO("Hello");

	auto  bitboard4 = Bitboard( 8, 8 );

	bitboard4.fillFile(5);
	bitboard4.fillFile(2);
	bitboard4.fillFile(23);
	bitboard4.fillRank(2);
	bitboard4.fillRank(5);
	bitboard4.fillRank(23);
	bitboard4.draw();

	auto bitboard = Bitboard(8, 8);
	bitboard.fillFile(5);
	bitboard.fillFile(6);
	bitboard.draw();

	bitboard &= ~bitboard4;
	bitboard.draw();

	constinit static const int width = 16;
	constinit static const int height = 16;
	//uint2 size{ 8, 8 };
	//auto set = MovementSet<16, 16>::generateMovementSet(PieceType::Knight, Colour::Black);
	auto moveDescription = { -2 * width, -width, -2, -1, 1, 2, width, 2 * width };

	auto customSet = MovementSet<width, height>::generateMovementSet(MoveType::Leap, moveDescription, {});
	customSet.getMovement()[61].draw();
	return 0;
}