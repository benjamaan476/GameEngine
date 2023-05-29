#pragma once

#include "../GameEngine/EngineCore.h"
#include "ChessCore.h"

#include "Bitboard.h"

enum class MoveType
{
	None,
	Leap,
	Slide,
	Custom
};

class MovementSet
{
public:
	constexpr MovementSet(uint2 size, MoveType movementType) noexcept;
	
	static MovementSet generate_movement_set(uint2 size, PieceType type, Colour colour);
	static MovementSet generate_movement_set(uint2 size, MoveType moveType, const std::vector<int2>& moveDescription);

	static void calculate_sliding_attack(uint2 size, MovementSet& set, const std::vector<int2>& moveDescription);

	static Bitboard set_occupancies(uint2 size, int index, Bitboard attackBoard);
	void set_square(uint32_t index, const Bitboard& board) noexcept;

	uint2 get_size() const noexcept { return _size; }
	MoveType get_move_type() const noexcept { return _movementType; }
	auto get_movement() const noexcept { return _movement; }

private:
	uint2 _size{};
	MoveType _movementType{};
	std::vector<Bitboard> _movement{};

	static inline std::vector<int2> knightMoveDescription = { {-1, -2}, {1, -2}, {-2, -1}, {2, -1}, {-2, 1}, {2, 1}, {-1, 2}, {1, 2} };
	static inline std::vector<int2> whitePawnMoveDescription = { {-1, -1}, {1, -1} };
	static inline std::vector<int2> blackPawnMoveDescription = { {-1, 1}, {1, 1} };
	static inline std::vector<int2> kingMoveDescription = { {-1, -1}, {0, -1}, {1, -1}, {-1, 0}, {1, 0}, {-1, 1}, {0, 1}, {1, 1} };
	static inline std::vector<int2> bishopMoveDescription = { {-1, -1}, {1, -1}, {-1, 1}, {1, 1} };
	static inline std::vector<int2> rookMoveDescription = { {-1, 0}, {1, 0}, {0, 1}, {0, -1} };
	static inline std::vector<int2> queenMoveDescription = { {-1, 0}, {-1, -1}, {1, -1}, {1, 1}, {-1, 1}, {1, 0}, {0, 1}, {0, -1} };

	//static inline std::vector<uint32_t> ahBlankingFile = { 0, size.x - 1 };
	//static inline std::vector<uint32_t> abghBlankingFile = { 0, 1, width - 2, width - 1 };

};
