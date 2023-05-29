#include <gtest/gtest.h>
#include "../Chess/ChessCore.h"

//TEST(Pieces, ctor)
//{
//	const auto piece = Piece(PieceType::Bishop, Colour::White);
//
//	EXPECT_NE(piece, Piece{});
//}
//
//TEST(Pieces, getType)
//{
//	const auto piece = Piece(PieceType::King, Colour::White);
//
//	const auto type = piece.type;
//
//	EXPECT_EQ(type, PieceType::King);
//}
//
//TEST(Pieces, getColour)
//{
//	const auto col = Colour::White;
//	const auto piece = Piece(PieceType::King, col);
//
//	const auto colour = piece.colour;
//
//	EXPECT_EQ(colour, col);
//}