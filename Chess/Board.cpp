#include "Board.h"

#include <ranges>
void AsciiBoardRenderer::draw(const Board& board)
{
	std::cout << "\n";
	auto count = 0u;

	std::ostringstream ss;
	for (const auto& piece : board.getBoard())
	{
		if (count == 8)
		{
			ss << "|\n";
			count = 0;
		}
		
		ss << "|";
		if (piece.has_value())
		{
			ss << "*";
		}
		else
		{
			ss << " ";
		}

		count++;
	}
	ss << "|";
	std::cout << ss.str() << std::endl;
}