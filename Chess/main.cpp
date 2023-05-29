#include "../GameEngine/Log/Log.h"
#include "../GameEngine/EngineCore.h"

#include "Board.h"
#include "BoardRenderer.h"
#include "Bitboard.h"
#include "MovementSet.h"
#include "MagicNumberGenerator.h"
#include "FENParser.h"

int main()
{
	Log::Init();
	LOG_INFO("Hello");

	constinit static const int width = 8;
	constinit static const int height = 8;

	const int2 size{ width, height };
	auto renderer = std::make_unique<AsciiBoardRenderer>();


	auto customSet = MovementSet::generate_movement_set({ width, height }, PieceType::Queen, Colour::Black);
	auto rook = customSet.get_movement()[29];
	rook.draw();
	//for (auto i = 0; i < 100; i++)
	//{
	//	auto occupancy = MovementSet::setOccupancies({ width, height }, i, rook);
	//	occupancy.draw();
	//}
	//MagicNumberGenerator mng{};

	//auto b = mng.generateRandomBitboard(width, height);
	//b.draw();

	auto boar = FENParser::parse_starting_position();
	boar.set_renderer(std::move(renderer));
	boar.draw();

	return 0;
}