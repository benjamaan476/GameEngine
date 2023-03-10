#include "../GameEngine/Log/Log.h"

#include "Pieces/Piece.h"
#include "Board.h"
#include "Bitboard.h"
#include "MoveSet.h"

#include "MagicNumberGenerator.h"

int main()
{
	Log::Init();
	LOG_INFO("Hello");

	constinit static const int width = 16;
	constinit static const int height = 16;
	//uint2 size{ 8, 8 };
	//auto set = MovementSet<16, 16>::generateMovementSet(PieceType::Knight, Colour::Black);
	//auto moveDescription = { -2 * width, -width, -2, -1, 1, 2, width, 2 * width };

	Bitboard board{ width, height };
	board.fillFile(5);
	board.fillFile(6);
	board.fillRank(0);


	auto customSet = MovementSet<width, height>::generateMovementSet(PieceType::Rook, Colour::White);
	auto rook = customSet.getMovement()[54];
	rook.draw();
	//for (auto i = 0; i < 100; i++)
	//{
	//	auto occupancy = MovementSet<width, height>::setOccupancies(i, rook);
	//	occupancy.draw();
	//}
	MagicNumberGenerator mng{};

	auto b = mng.generateRandomBitboard(width, height);
	//b.draw();
	return 0;
}