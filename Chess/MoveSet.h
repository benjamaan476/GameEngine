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
		std::vector<int32_t> moveDescription;
		switch (type)
		{
		case PieceType::Pawn:
		{
			if (colour == Colour::White)
			{
				moveDescription = whitePawnMoveDesription;
			}
			else
			{
				moveDescription = blackPawnMoveDesription;
			}
			return generateMovementSet(MoveType::Leap, moveDescription, ahBlankingFile);
		}
		case PieceType::Knight:
		{
			return generateMovementSet(MoveType::Leap, knightMoveDescription, abghBlankingFile);
		}
		case PieceType::King:
			return generateMovementSet(MoveType::Leap, kingMoveDesription, ahBlankingFile);
		case PieceType::None:
			ENGINE_ASSERT(false, "Invalid piece given");
		default:
			return MovementSet(MoveType::None);
		}
	}

	static MovementSet generateMovementSet(MoveType moveType, std::vector<int32_t> moveDescritpion, std::vector<int32_t> blankingFile)
	{
		MovementSet set{ moveType };
		for (auto i = 0; i < width * height; i++)
		{
			Bitboard bitboard{ (uint32_t)width, (uint32_t)height };
			for (const auto& move : moveDescritpion)
			{
				bitboard.setSquare(i + move);
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

	static inline std::vector<int32_t> knightMoveDescription = { -2 * width - 1, -2 * width + 1, -width - 2, -width + 2, width - 2, width + 2, 2 * width - 1, 2 * width + 1 };
	static inline std::vector<int32_t> whitePawnMoveDesription = { -width - 1, -width + 1 };
	static inline std::vector<int32_t> blackPawnMoveDesription = { width - 1, width + 1 };
	static inline std::vector<int32_t> kingMoveDesription = { -width -1, -width, -width + 1, -1, 1, width - 1, width, width + 1 };

	static inline std::vector<int32_t> ahBlankingFile = { 0, width - 1 };
	static inline std::vector<int32_t> abghBlankingFile = { 0, 1, width - 2, width - 1 };

};