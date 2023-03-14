module;

#include "../GameEngine/EngineCore.h"
#include "ChessCore.h"

#include <array>
#include <vector>

export module MovementSet;

import Bitboard;

export enum class MoveType
{
	None,
	Leap,
	Slide,
	Custom
};

export class MovementSet
{
public:
	constexpr static MovementSet generateMovementSet(uint2 size, PieceType type, Colour colour)
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
			return generateMovementSet(size, MoveType::Leap, moveDescription);
		}
		case PieceType::Knight:
			return generateMovementSet(size, MoveType::Leap, knightMoveDescription);
		case PieceType::King:
			return generateMovementSet(size, MoveType::Leap, kingMoveDescription);
		case PieceType::Bishop:
			return generateMovementSet(size, MoveType::Slide, bishopMoveDescription);
		case PieceType::Rook:
			return generateMovementSet(size, MoveType::Slide, rookMoveDescription);
		case PieceType::Queen:
			return generateMovementSet(size, MoveType::Slide, queenMoveDescription);
		case PieceType::None:
			ENGINE_ASSERT(false, "Invalid piece given");
		default:
			return MovementSet(size, MoveType::None);
		}
	}
		static MovementSet generateMovementSet(uint2 size, MoveType moveType, const std::vector<int2>&moveDescription)
		{
			MovementSet set{ size, moveType };
			if (moveType == MoveType::Leap)
			{
				for (auto i = 0u; i < size.x * size.y; i++)
				{
					Bitboard bitboard{ size };
					for (const auto& move : moveDescription)
					{
						bitboard.setSquare(i + move.y * size.x + move.x);
					}

					if (i % size.x == 0 || i % size.x == 1)
					{
						Bitboard h{ size };
						h.fillFile(size.x - 1);
						h.fillFile(size.x - 2);
						bitboard &= ~h;
					}
					else if (i % size.x == size.x - 1 || i % size.x == size.x - 2)
					{
						Bitboard a{ size };
						a.fillFile(0);
						a.fillFile(1);
						bitboard &= ~a;
					}
					set.setSquare(i, bitboard);
				}
			}
			else if (moveType == MoveType::Slide)
			{
				calculateSlidingAttack(size, set, moveDescription);
			}
			return set;
		}

		static void calculateSlidingAttack(uint2 size, MovementSet& set, const std::vector<int2>& moveDescription)
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
							bitboard.setSquare(rank * size.x + f);
						}
					}
					else if (onEdgeFile && move.x == 0)
					{
						for (auto r = rank + move.y; r > 0 && r < size.y - 1; r += move.y)
						{
							bitboard.setSquare(r * size.x + file);
						}
					}
					else
					{

						//for(int2 fr = {file, rank} + move; fr)
						for (auto r = rank + move.y, f = file + move.x; (r > 0 && r < size.y - 1) && (f > 0 && f < size.x - 1); r += move.y, f += move.x)
						{
							bitboard.setSquare(r * size.x + f);
						}
					}
					set.setSquare(i, bitboard);
				}
			}

		}

		static Bitboard setOccupancies(uint2 size, int index, Bitboard attackBoard)
		{
			Bitboard board{ size };
			auto bitCount = attackBoard.bitCount();

			for (auto count = 0; count < bitCount; count++)
			{
				//TODO attackBoard &= ~attackBoard.ls1b
				auto square = attackBoard.ls1bIndex();
				attackBoard.unsetSquare(square);

				if (index & (1ull << count))
				{
					board.setSquare(square);
				}
			}

			return board;
		}

		constexpr MovementSet(uint2 size, MoveType movementType) noexcept : size{ size }, movementType{ movementType }
		{
			Bitboard blank{size};
			movement.resize(size.x * size.y, blank);
		}

		uint2 getSize() const noexcept { return size; }
		MoveType getMoveType() const noexcept { return movementType; }
		auto getMovement() const noexcept { return movement; }

		void setSquare(uint32_t index, const Bitboard& board) noexcept
		{
			ENGINE_ASSERT(board.getSize() == size, "Incorrect bit board size");
			ENGINE_ASSERT(index < movement.size(), "Incorrect index");

			movement[index] = board;
		}

private:
	uint2 size{};
	MoveType movementType{};
	std::vector<Bitboard> movement{};

	static inline std::vector<int2> knightMoveDescription = { {-1, -2}, {1, -2}, {-2, -1}, {2, -1}, {-2, 1}, {2, 1}, {-1, 2}, {1, 2} };
	static inline std::vector<int2> whitePawnMoveDescription = { {-1, -1}, {1, -1} };
	static inline std::vector<int2> blackPawnMoveDescription = { {-1, 1}, {1, 1} };
	static inline std::vector<int2> kingMoveDescription = { {-1, -1}, {0, -1}, {1, -1}, {-1, 0}, {1, 0}, {-1, 1}, {0, 1}, {1, 1} };
	static inline std::vector<int2> bishopMoveDescription = { {-1, -1}, {1, -1}, {-1, 1}, {1, 1} };
	static inline std::vector<int2> rookMoveDescription = { {-1, 0}, {1, 0}, {0, 1}, {0, -1} };
	static inline std::vector<int2> queenMoveDescription = { {-1, 0}, {-1, -1}, {1, -1}, {1, 1}, {-1, 1}, {1, 0}, {0, 1}, {0, -1} };

	//static inline std::vector<uint32_t> ahBlankingFile = { 0, size.x - 1 };
	//static inline std::vector<uint32_t> abghBlankingFile = { 0, 1, width - 2, width - 1 };

};