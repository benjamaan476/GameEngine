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

	auto customSet = MovementSet<width, height>::generateMovementSet(PieceType::Rook, Colour::White);
	customSet.getMovement()[61].draw();

	MagicNumberGenerator mng{};

	auto b = mng.generateRandomBitboard(width, height);
	b.draw();
	return 0;
}