module;

#include "../GameEngine/EngineCore.h"
#include "ChessCore.h"

#include <string_view>

export module FENParser;

import Board;
import BoardBuilder;

export class FENParser
{
public:
	constexpr FENParser() = default;

	Board parse(std::string_view fenString)
	{
		assert(!fenString.empty());

		auto [ nextIndex, boardField ] = next_field(fenString, _fieldDelimiter);
		auto boardBuilder = parse_board(boardField);

		auto [ nextPlayerIndex, nextPlayer] = next_field(fenString, _fieldDelimiter, nextIndex);
		parse_player(boardBuilder, nextPlayer);

		nextIndex += nextPlayerIndex;
		auto [castlingIndex, castling] = next_field(fenString, _fieldDelimiter, nextIndex);
		parse_castling(boardBuilder, castling);

		nextIndex += castlingIndex;
		auto [enPassantTargetIndex, enPassantTarget] = next_field(fenString, _fieldDelimiter, nextIndex);
		parse_enpassant(boardBuilder, enPassantTarget);

		nextIndex += enPassantTargetIndex;
		auto [halfmoveClockIndex, halfmoveClock] = next_field(fenString, _fieldDelimiter, nextIndex);
		parse_halfmove(boardBuilder, halfmoveClock);

		nextIndex += halfmoveClockIndex;
		auto [fullmoveClockIndex, fullmoveClock] = next_field(fenString, _fieldDelimiter, nextIndex);
		parse_fullmove(boardBuilder, fullmoveClock);

		return boardBuilder.build();
	}

	Board parse_starting_position()
	{
		return parse(_startingPosition);
	}

private:
	BoardBuilder parse_board(std::string_view boardField)
	{
		BoardBuilder builder{};
		auto rank{ 0 };
		auto file{ 0 };
		size_t newIndex{1};
		auto [index, rankString] = next_field(boardField, _rankDelimiter);

		for (; newIndex != 0;)
		{
			builder.new_rank();

			for (const auto& c : rankString)
			{
				if (std::isdigit(c))
				{
					file += c - '0';
				}
				else
				{
					builder.set_piece({ rank, file }, c);
					++file;
				}
			}
			std::tie(newIndex, rankString) = next_field(boardField, _rankDelimiter, index);
			index += newIndex;
			++rank;
			file = 0;
		}
		return builder;
	}

	void parse_player(BoardBuilder& board, std::string_view playerString)
	{
		assert(playerString.length() == 1);
		
		if (playerString.compare("w") == 0)
		{
			board.set_next_player(Colour::White);
		}
		else if (playerString.compare("b") == 0)
		{
			board.set_next_player(Colour::Black);
		}
		else
		{
			assert(false);
		}
	}

	void parse_castling(BoardBuilder& builder, std::string_view castlingString)
	{
		
	}

	void parse_enpassant(BoardBuilder& builder, std::string_view enpassantString)
	{

	}

	void parse_halfmove(BoardBuilder& builder, std::string_view halfmoveString)
	{

	}

	void parse_fullmove(BoardBuilder& builder, std::string_view fullmoveString)
	{

	}

private:
	auto next_field(std::string_view fields, char delimiter, size_t index = 0) -> std::pair<size_t, std::string_view>
	{
		auto nextField = fields.substr(index);
		auto nextIndex = nextField.find_first_of(delimiter);
		auto field = nextField.substr(0, nextIndex);
		return { nextIndex + 1, field };
	};


private:

	static constexpr char _fieldDelimiter = ' ';
	static constexpr char _rankDelimiter = '/';

	std::string _startingPosition = "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1";
};