#include "Bitboard.h"
#include "../GameEngine/EngineCore.h"
#include "ChessCore.h"

#include <array>
#include <vector>

enum class MoveType
{
	None,
	Leap,
	Slide,
	Custom
};

template<int32_t width, int32_t height>
class MovementSet
{
public:
	constexpr static MovementSet generateMovementSet(PieceType type, Colour colour)
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
			else
			{
				moveDescription = blackPawnMoveDescription;
			}
			return generateMovementSet(MoveType::Leap, moveDescription, ahBlankingFile);
		}
		case PieceType::Knight:
			return generateMovementSet(MoveType::Leap, knightMoveDescription, abghBlankingFile);
		case PieceType::King:
			return generateMovementSet(MoveType::Leap, kingMoveDescription, ahBlankingFile);
		case PieceType::Bishop:
			return generateMovementSet(MoveType::Slide, bishopMoveDescription, ahBlankingFile);
		case PieceType::Rook:
			return generateMovementSet(MoveType::Slide, rookMoveDescription, ahBlankingFile);
		case PieceType::Queen:
			return generateMovementSet(MoveType::Slide, queenMoveDescription, ahBlankingFile);
		case PieceType::None:
			ENGINE_ASSERT(false, "Invalid piece given");
		default:
			return MovementSet(MoveType::None);
		}
	}

	static MovementSet generateMovementSet(MoveType moveType, const std::vector<int2>& moveDescritpion, const std::vector<uint32_t>& blankingFile)
	{
		MovementSet set{ moveType };
		if (moveType == MoveType::Leap)
		{
			for (auto i = 0; i < width * height; i++)
			{
				Bitboard bitboard{ (uint32_t)width, (uint32_t)height };
				for (const auto& move : moveDescritpion)
				{
					bitboard.setSquare(i + move.y * width + move.x);
				}

				if (i % width == 0 || i % width == 1)
				{
					Bitboard h{ (uint32_t)width, (uint32_t)height };
					h.fillFile(width - 1);
					h.fillFile(width - 2);
					bitboard &= ~h;
				}
				else if (i % width == width - 1 || i % width == width - 2)
				{
					Bitboard a{ (uint32_t)width, (uint32_t)height };
					a.fillFile(0);
					a.fillFile(1);
					bitboard &= ~a;
				}
				set.setSquare(i, bitboard);
			}
		}
		else if (moveType == MoveType::Slide)
		{
			for (int i = 0; i < width * height; i++)
			{
				auto file = i % width;
				auto rank = i / height;
		
				Bitboard bitboard{ (uint32_t)width, (uint32_t)height };
				for (const auto& move : moveDescritpion)
				{
					for (auto r = rank + move.y, f = file + move.x; (r > 0 && r < height - 1) && (f > 0 && f < width - 1); r += move.y, f += move.x)
					{
						bitboard.setSquare(r * width + f);
					}

					set.setSquare(i, bitboard);
				}
			}
		}
		return set;
	}

	MovementSet(MoveType movementType) noexcept : size{ width, height }, movementType{ movementType }
	{
		Bitboard blank{ width, height };
		movement.resize(width * height, blank);
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

	static inline std::vector<uint32_t> ahBlankingFile = { 0, width - 1 };
	static inline std::vector<uint32_t> abghBlankingFile = { 0, 1, width - 2, width - 1 };

};