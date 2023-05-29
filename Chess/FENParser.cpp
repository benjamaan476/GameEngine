#include "FENParser.h"

Board FENParser::parse(std::string_view fenString)
{
	assert(!fenString.empty());

	auto [nextIndex, boardField] = next_field(fenString, _fieldDelimiter);
	auto boardBuilder = parse_board(boardField);

	auto [nextPlayerIndex, nextPlayer] = next_field(fenString, _fieldDelimiter, nextIndex);
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

Board FENParser::parse_starting_position()
{
	FENParser parser{};
	return parser.parse(_startingPosition);
}

BoardBuilder FENParser::parse_board(std::string_view boardField)
{
	BoardBuilder builder{};
	auto rank{ 0 };
	size_t newIndex{ 1 };
	std::string_view rankString;
	size_t index{ 0 };

	for (; newIndex != std::string::npos;)
	{
		std::tie(newIndex, rankString) = next_field(boardField, _rankDelimiter, index);
		builder.new_rank();

		for (const auto& c : rankString)
		{
			if (std::isdigit(c))
			{
				auto blank = c - '0';
				builder.set_blank(rank, blank);
			}
			else
			{
				builder.set_piece(rank, c);
			}
		}
		index += newIndex;
		++rank;
	}
	return builder;
}

void FENParser::parse_player(BoardBuilder& board, std::string_view playerString)
{
	assert(playerString.length() == 1);

	board.set_next_player(playerString);
}

void FENParser::parse_castling(BoardBuilder& builder, std::string_view castlingString)
{
	ENGINE_ASSERT(!castlingString.empty(), "Invalid castling string")
	builder.set_castling(castlingString);
}

void FENParser::parse_enpassant(BoardBuilder& builder, std::string_view enpassantString)
{
	if (enpassantString != "-")
	{
		builder.set_enpassant_square(enpassantString);
	}
}

void FENParser::parse_halfmove(BoardBuilder& builder, std::string_view halfmoveString)
{
	assert(!halfmoveString.empty());
	builder.set_halfmove(halfmoveString);
}

void FENParser::parse_fullmove(BoardBuilder& builder, std::string_view fullmoveString)
{
	assert(!fullmoveString.empty());
	builder.set_fullmove(fullmoveString);
}

auto FENParser::next_field(std::string_view fields, char delimiter, size_t index) -> std::pair<size_t, std::string_view>
{
	auto nextField = fields.substr(index);
	auto nextIndex = nextField.find_first_of(delimiter);
	if (nextIndex == std::string::npos)
	{
		return { -1, nextField };
	}
	auto field = nextField.substr(0, nextIndex);
	return { nextIndex + 1, field };
};