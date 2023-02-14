#include <gtest/gtest.h>
#include "../Chess/Pieces/Piece.h"

TEST(Pieces, ctor)
{
	const auto piece = Piece(PieceType::Bishop, Colour::White);

	EXPECT_NE(piece, Piece{});
}

TEST(Pieces, getType)
{
	const auto piece = Piece(PieceType::King, Colour::White);

	const auto type = piece.getType();

	EXPECT_EQ(type, PieceType::King);
}

TEST(Pieces, getColour)
{
	const auto col = Colour::White;
	const auto piece = Piece(PieceType::King, col);

	const auto colour = piece.getColour();

	EXPECT_EQ(colour, col);
}

TEST(MoveSet, ctor)
{
	const auto moveSet = MoveSet::createMoveSet(PieceType::Pawn);

	EXPECT_NE(moveSet, MoveSet{});
	EXPECT_FALSE(moveSet.getMoves().empty());
}

TEST(MoveSet, expandMovesZeroDistance)
{
	std::vector<Move> moves = { Move({0,1}) };

	const auto& expandedMoves = moves[0].expandMove(0);

	EXPECT_TRUE(expandedMoves.empty());
}

//fuzz this?
TEST(MoveSet, expandMovesNonZeroDistance)
{
	std::vector<Move> moves = { Move({0,1}, 5) };
	const auto distance = 6u;

	const auto& expandedMoves = moves[0].expandMove(distance);

	EXPECT_TRUE(expandedMoves.size() <= distance);
}