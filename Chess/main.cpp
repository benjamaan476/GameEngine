#include "../GameEngine/Log/Log.h"
#include "../GameEngine/EngineCore.h"

import Board;
import BoardRenderer;
import Bitboard;
import MovementSet;
import MagicNumberGenerator;
import FENParser;

int main()
{
	Log::Init();
	LOG_INFO("Hello");

	constinit static const int width = 8;
	constinit static const int height = 8;

	const int2 size{ width, height };
	auto renderer = std::make_unique<AsciiBoardRenderer>();

	Board board2(size, std::move(renderer));
	board2.draw();
	auto white = board2.getWhitePieces();

	auto customSet = MovementSet::generateMovementSet({ width, height }, PieceType::Queen, Colour::Black);
	auto rook = customSet.getMovement()[29];
	rook.draw();
	//for (auto i = 0; i < 100; i++)
	//{
	//	auto occupancy = MovementSet::setOccupancies({ width, height }, i, rook);
	//	occupancy.draw();
	//}
	//MagicNumberGenerator mng{};

	//auto b = mng.generateRandomBitboard(width, height);
	//b.draw();

	FENParser parse{};

	parse.parse_starting_position();
	return 0;
}