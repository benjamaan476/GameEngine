#pragma once
#include "../../GameEngine/EngineCore.h"


#include "ChessCore.h"
enum class MovementDirection
{
	None,
	Cardinal,
	Diagonal,
	Custom,
	All
};

struct Move
{
	constexpr explicit Move(uint2 dir) noexcept : direction{ dir } {}
	constexpr Move(uint2 dir, uint32_t dist) noexcept : direction{ dir }, distance{ dist } {}

	std::vector<uint2> expandMove(uint32_t maxDistance) const noexcept
	{
		std::vector<uint2> expandedMoves{};

		for (auto i = 0u; i < distance && maxDistance; i++)
		{
			expandedMoves.emplace_back(direction * i);
		}

		return expandedMoves;
	}

	friend constexpr bool operator== (const Move& lhs, const Move& rhs)
	{
		return lhs.direction == rhs.direction && lhs.distance == rhs.distance;
	}
	friend constexpr bool operator!= (const Move& lhs, const Move& rhs)
	{
		return !(lhs == rhs);
	}

	uint2 direction{};
	uint32_t distance{ 1 };
};

class MoveSet
{
public:
	constexpr static MoveSet createMoveSet(PieceType type)
	{
		MoveSet moveSet{};
		auto& moves = moveSet.moves;

		switch (type)
		{
		case PieceType::Pawn:
			moveSet.movement = MovementDirection::Cardinal;
			moves.emplace_back(Move({ 0, 1 }));
			break;
		case PieceType::Rook:
			moveSet.movement = MovementDirection::Cardinal;
			moves.emplace_back(Move({ 0, 1 }, std::numeric_limits<uint32_t>::max()));
			moves.emplace_back(Move({ 0, -1 }, std::numeric_limits<uint32_t>::max()));
			moves.emplace_back(Move({ 1, 0 }, std::numeric_limits<uint32_t>::max()));
			moves.emplace_back(Move({ -1, 0 }, std::numeric_limits<uint32_t>::max()));
			break;
		case PieceType::Knight:
			moveSet.movement = MovementDirection::Custom;
			moves.emplace_back(Move({ 1, 2 }));
			moves.emplace_back(Move({ 1, -2 }));
			moves.emplace_back(Move({ -1, 2 }));
			moves.emplace_back(Move({ -1, -2 }));
			moves.emplace_back(Move({ 2, 1 }));
			moves.emplace_back(Move({ 2, -1 }));
			moves.emplace_back(Move({ -2, 1 }));
			moves.emplace_back(Move({ -2, -1 }));
			break;
		case PieceType::Bishop:
			moveSet.movement = MovementDirection::Diagonal;
			moves.emplace_back(Move({ 1, 1 }, std::numeric_limits<uint32_t>::max()));
			moves.emplace_back(Move({ 1, -1 }, std::numeric_limits<uint32_t>::max()));
			moves.emplace_back(Move({ -1, 1 }, std::numeric_limits<uint32_t>::max()));
			moves.emplace_back(Move({ -1, -1 }, std::numeric_limits<uint32_t>::max()));
			break;
		case PieceType::Queen:
			moveSet.movement = MovementDirection::All;
			moves.emplace_back(Move({ 0, 1 }, std::numeric_limits<uint32_t>::max()));
			moves.emplace_back(Move({ 0, -1 }, std::numeric_limits<uint32_t>::max()));
			moves.emplace_back(Move({ 1, 0 }, std::numeric_limits<uint32_t>::max()));
			moves.emplace_back(Move({ -1, 0 }, std::numeric_limits<uint32_t>::max()));
			moves.emplace_back(Move({ 1, 1 }, std::numeric_limits<uint32_t>::max()));
			moves.emplace_back(Move({ 1, -1 }, std::numeric_limits<uint32_t>::max()));
			moves.emplace_back(Move({ -1, 1 }, std::numeric_limits<uint32_t>::max()));
			moves.emplace_back(Move({ -1, -1 }, std::numeric_limits<uint32_t>::max()));
			break;
		case PieceType::King:
			moveSet.movement = MovementDirection::All;
			moves.emplace_back(Move({ 0, 1 }));
			moves.emplace_back(Move({ 0, -1 }));
			moves.emplace_back(Move({ 1, 0 }));
			moves.emplace_back(Move({ -1, 0 }));
			moves.emplace_back(Move({ 1, 1 }));
			moves.emplace_back(Move({ 1, -1 }));
			moves.emplace_back(Move({ -1, 1 }));
			moves.emplace_back(Move({ -1, -1 }));
			break;
		case PieceType::None:
		default:
			break;
		}

		return moveSet;
	}

	friend constexpr bool operator== (const MoveSet& lhs, const MoveSet& rhs)
	{
		return lhs.movement == rhs.movement && lhs.moves == rhs.moves;
	}
	friend constexpr bool operator!= (const MoveSet& lhs, const MoveSet& rhs)
	{
		return !(lhs == rhs);
	}

	MovementDirection getMovement() const noexcept { return movement; }
	std::vector<Move> getMoves() const noexcept { return moves; }
private:
	MovementDirection movement{};
	std::vector<Move> moves{};
};