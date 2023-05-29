#pragma once

#include "../GameEngine/EngineCore.h"
#include "ChessCore.h"

#include "Board.h"
#include "BoardBuilder.h"

#include <string_view>

const static std::string _startingPosition = "rnbqkbnr/pppppp1p/6p1/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1";

class FENParser
{
public:
	constexpr FENParser() = default;

	Board parse(std::string_view fenString);

	static Board parse_starting_position();

private:
	BoardBuilder parse_board(std::string_view boardField);

	void parse_player(BoardBuilder& board, std::string_view playerString);
	void parse_castling(BoardBuilder& builder, std::string_view castlingString);
	void parse_enpassant(BoardBuilder& builder, std::string_view enpassantString);
	void parse_halfmove(BoardBuilder& builder, std::string_view halfmoveString);
	void parse_fullmove(BoardBuilder& builder, std::string_view fullmoveString);

private:
	auto next_field(std::string_view fields, char delimiter, size_t index = 0) -> std::pair<size_t, std::string_view>;

private:

	static constexpr char _fieldDelimiter = ' ';
	static constexpr char _rankDelimiter = '/';

};