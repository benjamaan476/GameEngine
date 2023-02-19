#pragma once

#include "../../GameEngine/EngineCore.h"

#include "../ChessCore.h"
#include "Move.h"

class Piece
{
public:

	Piece() = default;
	constexpr Piece(PieceType pieceType, Colour pieceColour) noexcept : colour{ pieceColour }, type{ pieceType }
	{
		moves = MoveSet::createMoveSet(pieceType);
	}
	virtual ~Piece() {};

	uint2 getPosition() const noexcept { return position; }
	Colour getColour() const noexcept { return colour; }
	PieceType getType() const noexcept { return type; }

	friend constexpr bool operator== (const Piece& lhs, const Piece& rhs) noexcept;
	friend constexpr bool operator!= (const Piece& lhs, const Piece& rhs) noexcept;
private:

	uint2 position{};
	Colour colour{};
	PieceType type{};
	MoveSet moves;
};

bool constexpr operator== (const Piece& lhs, const Piece& rhs) noexcept
{
	return lhs.position == rhs.position && lhs.colour == rhs.colour && lhs.type == rhs.type;
}

bool constexpr operator!= (const Piece& lhs, const Piece& rhs) noexcept
{
	return !(rhs == lhs);
}