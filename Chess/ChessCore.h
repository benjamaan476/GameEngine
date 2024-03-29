#pragma once

enum class Colour
{
	None,
	White,
	Black
};

enum class PieceType
{
	None,
	Pawn,
	Rook,
	Knight,
	Bishop,
	Queen,
	King
};

struct Piece
{
	PieceType type;
	Colour colour;
};