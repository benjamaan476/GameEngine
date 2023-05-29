#include "MovementSet.h"

constexpr MovementSet::MovementSet(uint2 size, MoveType movementType) noexcept
	: _size{ size }, _movementType{ movementType }
{
	_movement.resize(size.x * size.y);
}

MovementSet MovementSet::generate_movement_set(uint2 size, PieceType type, Colour colour)
{
	std::vector<int2> moveDescription;

	switch (type)
	{
	case PieceType::Pawn:
	{
		if (colour == Colour::White)
		{
			moveDescription = whitePawnMoveDescription;
		}
		else if (colour == Colour::Black)
		{
			moveDescription = blackPawnMoveDescription;
		}
		return generate_movement_set(size, MoveType::Leap, moveDescription);
	}
	case PieceType::Knight:
		return generate_movement_set(size, MoveType::Leap, knightMoveDescription);
	case PieceType::King:
		return generate_movement_set(size, MoveType::Leap, kingMoveDescription);
	case PieceType::Bishop:
		return generate_movement_set(size, MoveType::Slide, bishopMoveDescription);
	case PieceType::Rook:
		return generate_movement_set(size, MoveType::Slide, rookMoveDescription);
	case PieceType::Queen:
		return generate_movement_set(size, MoveType::Slide, queenMoveDescription);
	case PieceType::None:
		ENGINE_ASSERT(false, "Invalid piece given");
	default:
		return MovementSet(size, MoveType::None);
	}
}

MovementSet MovementSet::generate_movement_set(uint2 size, MoveType moveType, const std::vector<int2>& moveDescription)
{
	MovementSet set{ size, moveType };
	if (moveType == MoveType::Leap)
	{
		for (auto i = 0u; i < size.x * size.y; i++)
		{
			Bitboard bitboard{ size };
			for (const auto& move : moveDescription)
			{
				bitboard.set_square(i + move.y * size.x + move.x);
			}

			if (i % size.x == 0 || i % size.x == 1)
			{
				Bitboard h{ size };
				h.fill_file(size.x - 1);
				h.fill_file(size.x - 2);
				bitboard &= ~h;
			}
			else if (i % size.x == size.x - 1 || i % size.x == size.x - 2)
			{
				Bitboard a{ size };
				a.fill_file(0);
				a.fill_file(1);
				bitboard &= ~a;
			}
			set.set_square(i, bitboard);
		}
	}
	else if (moveType == MoveType::Slide)
	{
		calculate_sliding_attack(size, set, moveDescription);
	}
	return set;
}

void MovementSet::calculate_sliding_attack(uint2 size, MovementSet& set, const std::vector<int2>& moveDescription)
{
	for (auto i = 0u; i < size.x * size.y; i++)
	{
		const auto file = i % size.x;
		const auto rank = i / size.y;

		auto onEdgeRank = (rank == 0 || rank == size.y - 1);
		auto onEdgeFile = (file == 0 || file == size.x - 1);

		Bitboard bitboard{ size };
		for (const auto& move : moveDescription)
		{
			if (onEdgeRank && move.y == 0)
			{
				for (auto f = file + move.x; f > 0 && f < size.x - 1; f += move.x)
				{
					bitboard.set_square(rank * size.x + f);
				}
			}
			else if (onEdgeFile && move.x == 0)
			{
				for (auto r = rank + move.y; r > 0 && r < size.y - 1; r += move.y)
				{
					bitboard.set_square(r * size.x + file);
				}
			}
			else
			{

				//for(int2 fr = {file, rank} + move; fr)
				for (auto r = rank + move.y, f = file + move.x; (r > 0 && r < size.y - 1) && (f > 0 && f < size.x - 1); r += move.y, f += move.x)
				{
					bitboard.set_square(r * size.x + f);
				}
			}
			set.set_square(i, bitboard);
		}
	}

}

Bitboard MovementSet::set_occupancies(uint2 size, int index, Bitboard attackBoard)
{
	Bitboard board{ size };
	auto bitCount = attackBoard.bit_count();

	for (auto count = 0; count < bitCount; count++)
	{
		//TODO attackBoard &= ~attackBoard.ls1b
		auto square = attackBoard.least_significant_first_bit_index();
		attackBoard.unset_square(square);

		if (index & (1ull << count))
		{
			board.set_square(square);
		}
	}

	return board;
}

void MovementSet::set_square(uint32_t index, const Bitboard& board) noexcept
{
	ENGINE_ASSERT(board.get_size() == _size, "Incorrect bit board size");
	ENGINE_ASSERT(index < _movement.size(), "Incorrect index");

	_movement[index] = board;
}
