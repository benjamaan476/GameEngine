#include "../GameEngine/Log/Log.h"

#include "Pieces/Piece.h"
#include "Board.h"

int main()
{
	Log::Init();
	LOG_INFO("Hello");
	int i = 0;
	
	auto piece = Piece(PieceType::Bishop, Colour::White);

	auto type = piece.getType();

	Board board;

	return 0;
}