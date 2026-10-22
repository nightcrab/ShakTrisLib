#pragma once

#include <array>
#include <bit>
#include <bitset>
#include <cstdint>
#include <ranges>
#include <set>
#include <unordered_set>
#include <vector>
#include <bitset>
#include <bit>

#include <cstring>
#include <cassert>

#include "Board.hpp"
#include "Piece.hpp"
#include "RotationSystems.hpp"
#include "ShaktrisConstants.hpp"
#include "Utility.hpp"
#include <immintrin.h>

#include <cstring>
#include <cassert>

// precomputed piece movements

/// <summary>
/// this function returns the size of the zero_g_starts
/// </summary>
/// <returns>size_t for the zero_g_starts of any given piece</returns>
consteval static size_t zero_g_starts_size(const PieceType p) {
    Piece initial_piece = Piece(p);

    auto collides = [](const Piece& piece) {

        for (auto& mino : piece.minos) {
            int x_pos = mino.x + piece.position.x;
            if (x_pos < 0 || x_pos >= Board::width)
                return true;

            int y_pos = mino.y + piece.position.y;
            if (y_pos < 0 || y_pos >= Board::height)
                return true;
        }

        return false;
        };

    auto shift = [&](Piece& piece, int dir) {
        piece.position.x += dir;

        if (collides(piece))
            piece.position.x -= dir;
        else
            piece.spin = spinType::null;
        };

    std::vector<Piece> open_nodes;
    open_nodes.reserve(150);
    std::vector<Piece> next_nodes;
    next_nodes.reserve(150);
    std::vector<bool> visited = std::vector<bool>(6444);

    std::vector<Piece> valid_moves;
    valid_moves.reserve(150);

    // rotations
    for (int r = 0; r < (int)RotationDirections_N; r++) {
        open_nodes.emplace_back(initial_piece);
        if (initial_piece.type != PieceType::O)
            initial_piece.rotate(TurnDirection::Right);

        // lateral movement
        while (open_nodes.size() > 0) {
            // expand edges
            for (const auto& piece : open_nodes) {
                size_t h = piece.compact_hash();
                if (visited[h])
                    continue;
                // mark node as visited
                visited[h] = true;

                valid_moves.push_back(piece);

                Piece new_piece = piece;
                shift(new_piece, -1);
                next_nodes.emplace_back(new_piece);

                new_piece = piece;
                shift(new_piece, 1);
                next_nodes.emplace_back(new_piece);
            }
            open_nodes = next_nodes;
            next_nodes.clear();
        }
    }

    return valid_moves.size();
};



// returns the zero g starts for the given piece
template<PieceType p>
consteval static auto zero_g_starts() {

    constexpr std::array<size_t, static_cast<size_t>(PieceType::PieceTypes_N)> zero_g_start_size = {
        zero_g_starts_size(PieceType::S),
        zero_g_starts_size(PieceType::Z),
        zero_g_starts_size(PieceType::J),
        zero_g_starts_size(PieceType::L),
        zero_g_starts_size(PieceType::T),
        zero_g_starts_size(PieceType::O),
        zero_g_starts_size(PieceType::I),
    };

    Piece initial_piece = Piece(p);

    auto collides = [](const Piece& piece) {

        for (auto& mino : piece.minos) {
            int x_pos = mino.x + piece.position.x;
            if (x_pos < 0 || x_pos >= Board::width)
                return true;

            int y_pos = mino.y + piece.position.y;
            if (y_pos < 0 || y_pos >= Board::height)
                return true;
        }

        return false;
        };

    auto shift = [&](Piece& piece, int dir) {
        piece.position.x += dir;

        if (collides(piece))
            piece.position.x -= dir;
        else
            piece.spin = spinType::null;
        };

    std::vector<Piece> open_nodes;
    open_nodes.reserve(150);
    std::vector<Piece> next_nodes;
    next_nodes.reserve(150);
    std::vector<bool> visited = std::vector<bool>(6444);

    std::vector<Piece> valid_moves;
    valid_moves.reserve(150);

    // rotations
    for (int r = 0; r < 4; r++) {
        open_nodes.emplace_back(initial_piece);
        if (initial_piece.type != PieceType::O)
            initial_piece.rotate(TurnDirection::Right);

        // lateral movement
        while (open_nodes.size() > 0) {
            // expand edges
            for (const auto& piece : open_nodes) {
                size_t h = piece.compact_hash();
                if (visited[h])
                    continue;
                // mark node as visited
                visited[h] = true;

                valid_moves.push_back(piece);

                Piece new_piece = piece;
                shift(new_piece, -1);
                next_nodes.emplace_back(new_piece);

                new_piece = piece;
                shift(new_piece, 1);
                next_nodes.emplace_back(new_piece);
            }
            open_nodes = next_nodes;
            next_nodes.clear();
        }
    }

    // shoutouts to friedkeenan
    // github link: https://github.com/friedkeenan/
    auto zero_g_starts = [&]<std::size_t... Indices>(std::index_sequence<Indices...>) {
        return std::array{
            valid_moves[Indices]...
        };
    }(std::make_index_sequence<zero_g_start_size[(size_t)p]>{});

    return zero_g_starts;
}

constexpr auto zero_g_starts_S = zero_g_starts<PieceType::S>();
constexpr auto zero_g_starts_Z = zero_g_starts<PieceType::Z>();
constexpr auto zero_g_starts_J = zero_g_starts<PieceType::J>();
constexpr auto zero_g_starts_L = zero_g_starts<PieceType::L>();
constexpr auto zero_g_starts_T = zero_g_starts<PieceType::T>();
constexpr auto zero_g_starts_O = zero_g_starts<PieceType::O>();
constexpr auto zero_g_starts_I = zero_g_starts<PieceType::I>();

namespace Shaktris {
    namespace MoveGen {
        namespace Traditional {
            // Movegen for a convex board with free movement at the top. (decided by board height of 16 or lower)
            inline std::vector<Piece> sky_piece_movegen(const Board& board, PieceType piece_type) {

                std::vector<Piece> valid_moves;

                switch (piece_type) {
                case PieceType::S:
                    valid_moves.assign(zero_g_starts_S.begin(), zero_g_starts_S.end());
                    break;
                case PieceType::Z:
                    valid_moves.assign(zero_g_starts_Z.begin(), zero_g_starts_Z.end());
                    break;
                case PieceType::J:
                    valid_moves.assign(zero_g_starts_J.begin(), zero_g_starts_J.end());
                    break;
                case PieceType::L:
                    valid_moves.assign(zero_g_starts_L.begin(), zero_g_starts_L.end());
                    break;
                case PieceType::T:
                    valid_moves.assign(zero_g_starts_T.begin(), zero_g_starts_T.end());
                    break;
                case PieceType::O:
                    valid_moves.assign(zero_g_starts_O.begin(), zero_g_starts_O.end());
                    break;
                case PieceType::I:
                    valid_moves.assign(zero_g_starts_I.begin(), zero_g_starts_I.end());
                    break;
                case PieceType::Empty:
                    break;
                case PieceType::PieceTypes_N:
                    break;
                    // default:
                        //break;
                }

                for (Piece& piece : valid_moves) {
                    Shaktris::Utility::sonic_drop(board, piece);
                }

                return valid_moves;
            }


            // movegen for if the board is convex and the height is 17 or higher
            inline std::vector<Piece> convex_movegen(const Board& board, PieceType piece_type) {
                Piece initial_piece = Piece(piece_type);

                std::vector<Piece> open_nodes;
                open_nodes.reserve(150);
                std::vector<Piece> next_nodes;
                next_nodes.reserve(150);
                std::bitset<6444> visited;

                std::vector<Piece> valid_moves;
                valid_moves.reserve(150);

                // rotations
                for (int r = 0; r < 4; r++) {
                    open_nodes.emplace_back(initial_piece);

                    initial_piece.rotate(TurnDirection::Right);

                    // lateral movement
                    while (open_nodes.size() > 0) {
                        // expand edges
                        for (auto& piece : open_nodes) {
                            size_t h = piece.compact_hash();
                            if (visited[h])
                                continue;
                            // mark node as visited
                            visited[h] = true;

                            valid_moves.push_back(piece);

                            Piece new_piece = piece;
                            Shaktris::Utility::shift(board, new_piece, -1);
                            next_nodes.emplace_back(new_piece);

                            new_piece = piece;
                            Shaktris::Utility::shift(board, new_piece, 1);
                            next_nodes.emplace_back(new_piece);
                        }
                        open_nodes = next_nodes;
                        next_nodes.clear();
                    }
                    if (initial_piece.type == PieceType::O && r == 0)
                        break;
                    if ((
                        (initial_piece.type == PieceType::Z) ||
                        (initial_piece.type == PieceType::S) ||
                        (initial_piece.type == PieceType::O)
                        ) && r == 0)
                        break;
                }

                for (Piece& piece : valid_moves) {
                    Shaktris::Utility::sonic_drop(board, piece);
                }

                return valid_moves;
            }


            // movegen for a non convex board with free movement at the top
            inline std::vector<Piece> movegen(const rotation_function rot_func, const Board& board, PieceType piece_type) {
                std::vector<Piece> valid_moves;

                bool convex = board.surface_convex();
                bool low = board.is_low();

                if (convex && low) {
                    valid_moves = sky_piece_movegen(board, piece_type);
                    return valid_moves;
                }
                else if (convex) {
                    valid_moves = convex_movegen(board, piece_type);
                    return valid_moves;
                }

                Piece initial_piece = Piece(piece_type);

                std::vector<Piece> open_nodes;
                open_nodes.reserve(150);

                if (low) {
                    open_nodes = sky_piece_movegen(board, piece_type);
                }
                else {
                    // root node
                    open_nodes.emplace_back(initial_piece);
                }

                std::vector<Piece> next_nodes;
                next_nodes.reserve(150);
                std::bitset<6444> visited;
                valid_moves.reserve(100);

                while (open_nodes.size() > 0) {
                    // expand edges
                    for (auto& piece : open_nodes) {
                        size_t h = piece.compact_hash();
                        if (visited[h])
                            continue;
                        // mark node as visited
                        visited[h] = true;

                        // try all movements
                        Piece new_piece = piece;
                        if (initial_piece.type != PieceType::O) {
                            rot_func(board, new_piece, TurnDirection::Left);
                            next_nodes.emplace_back(new_piece);

                            new_piece = piece;
                            rot_func(board, new_piece, TurnDirection::Right);
                            next_nodes.emplace_back(new_piece);
                        }

                        new_piece = piece;
                        Shaktris::Utility::shift(board, new_piece, -1);
                        next_nodes.emplace_back(new_piece);

                        new_piece = piece;
                        Shaktris::Utility::shift(board, new_piece, 1);
                        next_nodes.emplace_back(new_piece);

                        new_piece = piece;
                        Shaktris::Utility::sonic_drop(board, new_piece);
                        next_nodes.emplace_back(new_piece);

                        // check if the piece is grounded and therefore valid

                        piece.position.y--;

                        if (Shaktris::Utility::collides(board, piece)) {
                            piece.position.y++;
                            valid_moves.emplace_back(piece);
                        }
                    }
                    open_nodes = next_nodes;
                    next_nodes.clear();
                }

                return valid_moves;
            }

        }; // namespace Traditional

        namespace Smeared {

            struct SmearedBoard {
                std::array<Board, 4> boards; // 0 => north, etc

                inline bool convex(bool surface) const {
                    bool ret = false;

                    if (surface) {
                        for (const auto& board : boards) {
                            ret |= !board.surface_convex();
                        }
                    }
                    else {
                        for (const auto& board : boards) {
                            ret |= !board.true_convex();
                        }
                    }

                    return !ret;
                }
                inline bool operator==(const SmearedBoard& other) const {
                    return boards == other.boards; // std::array's == compares all elements
                }

                // shift both left and right
                inline SmearedBoard shift() const {
                    SmearedBoard ret = *this;
                    // shift each board to the direction
                    for (auto& board : ret.boards) {

                        board.board[1] |= board.board[0];
                        board.board[board.board.size() - 2] |= board.board[board.board.size() - 1];

                        for (size_t i = 1; i < board.board.size() - 1; ++i)
                        {
                            board.board[i - 1] |= board.board[i];
                            board.board[i + 1] |= board.board[i];
                        }
                    }

                    return ret;
                }

                inline bool empty() const {
                    bool has_mino = false;
                    for (const auto& board : boards) {
                        for (const auto& col : board.board) {
                            has_mino |= col != 0;
                        }
                    }
                    return !has_mino;
                }

                // the this is the board
                inline void non_collides(SmearedBoard& pieces)const {
                    //    A & ~B
                    // where A is the piece and B is this

                    for (size_t b_index = 0; b_index < boards.size(); ++b_index) {
                        const auto& board = boards[b_index];
                        const auto& piece = pieces.boards[b_index];
                        for (size_t x = 0; x < Board::width; x++) {
                            pieces.boards[b_index].board[x] = piece.board[x] & ~board.board[x];
                        }
                    }
                }

                inline void collides(SmearedBoard& pieces)const {

                    for (size_t b_index = 0; b_index < boards.size(); ++b_index) {
                        const auto& board = boards[b_index];
                        const auto& piece = pieces.boards[b_index];
                        for (size_t x = 0; x < Board::width; x++) {
                            pieces.boards[b_index].board[x] = piece.board[x] & board.board[x];
                        }
                    }
                }

                // the this is the board
                inline SmearedBoard rotate_no_srs(const SmearedBoard& pieces, PieceType type) const {

                    SmearedBoard ret{};

                    const auto* offsets = &piece_offsets_JLSTZ;
                    const auto* prev_offsets = &piece_offsets_JLSTZ;

                    if (type == PieceType::I) {
                        offsets = &piece_offsets_I;
                        prev_offsets = &piece_offsets_I;
                    }
                    else if (type == PieceType::O) {
                        return ret;
                        offsets = &piece_offsets_O;
                        prev_offsets = &piece_offsets_O;
                    }

                    SmearedBoard left_rotating_set = pieces;
                    SmearedBoard right_rotating_set = pieces;

                    left_rotating_set.rotate_left();
                    right_rotating_set.rotate_right();

                    ret |= left_rotating_set;
                    ret |= right_rotating_set;

                    return ret;
                }

                inline SmearedBoard rotate_srs(const SmearedBoard& pieces, PieceType type) const {
                    SmearedBoard ret;

                    const auto* offsets = &piece_offsets_JLSTZ;
                    const auto* prev_offsets = &piece_offsets_JLSTZ;

                    if (type == PieceType::I) {
                        offsets = &piece_offsets_I;
                        prev_offsets = &piece_offsets_I;
                    }
                    else if (type == PieceType::O) {
                        offsets = &piece_offsets_O;
                        prev_offsets = &piece_offsets_O;
                    }

                    SmearedBoard left_rotating_set = pieces;
                    SmearedBoard right_rotating_set = pieces;

                    // for every piece offset
                    for (size_t srs_i = 0; srs_i < srs_kicks; srs_i++) {

                        std::array<Coord, 4> rot_offsets;
                        rot_offsets[0].x = (*prev_offsets)[3][srs_i].x - (*offsets)[0][srs_i].x;
                        rot_offsets[0].y = (*prev_offsets)[3][srs_i].y - (*offsets)[0][srs_i].y;

                        rot_offsets[1].x = (*prev_offsets)[0][srs_i].x - (*offsets)[1][srs_i].x;
                        rot_offsets[1].y = (*prev_offsets)[0][srs_i].y - (*offsets)[1][srs_i].y;

                        rot_offsets[2].x = (*prev_offsets)[1][srs_i].x - (*offsets)[2][srs_i].x;
                        rot_offsets[2].y = (*prev_offsets)[1][srs_i].y - (*offsets)[2][srs_i].y;

                        rot_offsets[3].x = (*prev_offsets)[2][srs_i].x - (*offsets)[3][srs_i].x;
                        rot_offsets[3].y = (*prev_offsets)[2][srs_i].y - (*offsets)[3][srs_i].y;

                        if (!right_rotating_set.empty()) {
                            SmearedBoard tmp_set = right_rotating_set;
                            tmp_set.rotate_right();
                            tmp_set.offset(rot_offsets);

                            // tmp after this line is which pieces did not collide after rotation
                            this->non_collides(tmp_set);

                            ret |= tmp_set;

                            // gotta rotate back to the original position
                            for (auto& offsets : rot_offsets) {
                                offsets.x *= -1;
                                offsets.y *= -1;
                            }

                            // tmp after this line is which pieces did not collide in the board space
                            tmp_set.offset(rot_offsets);
                            tmp_set.rotate_left();

                            // the ones that are still here are the ones we keep for the next iteration
                            right_rotating_set ^= tmp_set;
                        }

                        // use the rot_offsets for the other direction now
                        rot_offsets[0].x = (*prev_offsets)[1][srs_i].x - (*offsets)[0][srs_i].x;
                        rot_offsets[0].y = (*prev_offsets)[1][srs_i].y - (*offsets)[0][srs_i].y;

                        rot_offsets[1].x = (*prev_offsets)[2][srs_i].x - (*offsets)[1][srs_i].x;
                        rot_offsets[1].y = (*prev_offsets)[2][srs_i].y - (*offsets)[1][srs_i].y;

                        rot_offsets[2].x = (*prev_offsets)[3][srs_i].x - (*offsets)[2][srs_i].x;
                        rot_offsets[2].y = (*prev_offsets)[3][srs_i].y - (*offsets)[2][srs_i].y;

                        rot_offsets[3].x = (*prev_offsets)[0][srs_i].x - (*offsets)[3][srs_i].x;
                        rot_offsets[3].y = (*prev_offsets)[0][srs_i].y - (*offsets)[3][srs_i].y;

                        if (!left_rotating_set.empty()) {
                            SmearedBoard tmp_set = left_rotating_set;
                            tmp_set.rotate_left();
                            tmp_set.offset(rot_offsets);

                            // tmp after this line is which pieces did not collide after rotation
                            this->non_collides(tmp_set);

                            ret |= tmp_set;
                            // figure out which pieces do not collide by bringing them back to the original position and check if they are in the rotating pieces

                            for (auto& offsets : rot_offsets) {
                                offsets.x *= -1;
                                offsets.y *= -1;
                            }

                            // tmp after this line is which pieces did not collide in the board space
                            tmp_set.offset(rot_offsets);
                            tmp_set.rotate_right();

                            // the ones that are still here are the ones we keep for the next iteration
                            left_rotating_set ^= tmp_set;
                        }
                    }

                    // add the remaining pieces that did not rotate :(
                    ret |= left_rotating_set;
                    ret |= right_rotating_set;
                    return ret;
                }


                inline void rotate_right() {
                    if (boards.empty()) return; // Handle empty array
                    Board tmp = boards[boards.size() - 1];

                    // Shift elements right by 1 using std::memmove
                    std::memmove(&boards[1], &boards[0], (boards.size() - 1) * sizeof(Board));

                    boards[0] = tmp;
                }

                inline void rotate_left() {
                    if (boards.empty()) return; // Handle empty array
                    Board tmp = boards[0];

                    // Shift elements left by 1 using std::memmove
                    std::memmove(&boards[0], &boards[1], (boards.size() - 1) * sizeof(Board));

                    boards[boards.size() - 1] = tmp;
                }

                inline void offset(const std::array<Coord, 4>& offsets) {

                    for (size_t rot = 0; rot < 4; ++rot) {
                        this->boards[rot].offset(offsets[rot]);
                    }
                }

                // this is board
                inline SmearedBoard smear_drop(const SmearedBoard& pieces) const {
                    // pseudo code
                    // piece |= (piece >> 1) & ~column

                    SmearedBoard ret{};

                    for (size_t b_index = 0; b_index < boards.size(); ++b_index) {
                        auto& board = this->boards[b_index];
                        auto& piece = pieces.boards[b_index];

                        for (size_t x = 0; x < Board::width; x++) {
                            auto piece_col = piece.board[x];
                            for (int n = 0; n < 1; n++) {
                                piece_col |= (piece_col >> 1) & ~board.board[x];
                            }
                            ret.boards[b_index].board[x] = piece_col;
                        }
                    }

                    return ret;
                }

                // this is board
                inline SmearedBoard grounded(const SmearedBoard& pieces) const {
                    // the way to check if a piece is grounded is to shift it down once
                    // and then check if it collides with the board
                    // if it does then its grounded
                    // but also all pieces that are on & 1 are grounded too

                    SmearedBoard ret;

                    for (size_t b_index = 0; b_index < boards.size(); ++b_index) {
                        auto& board = this->boards[b_index];
                        const auto& piece = pieces.boards[b_index];

                        for (size_t x = 0; x < Board::width; x++) {
                            auto piece_col = piece.board[x];
                            auto board_col = board.board[x];

                            piece_col >>= 1;
                            piece_col &= board_col;
                            piece_col <<= 1;
                            piece_col |= piece.board[x] & 1;

                            ret.boards[b_index].board[x] = piece_col;
                        }
                    }
                    return ret;
                }

                inline void operator|=(const SmearedBoard& other) {
                    for (size_t b_index = 0; b_index < boards.size(); ++b_index) {
                        auto& board = boards[b_index];
                        const auto& other_board = other.boards[b_index];
                        for (size_t x = 0; x < Board::width; x++) {
                            board.board[x] |= other_board.board[x];
                        }
                    }
                }

                inline void operator&=(const SmearedBoard& other) {
                    for (size_t b_index = 0; b_index < boards.size(); ++b_index) {
                        auto& board = boards[b_index];
                        const auto& other_board = other.boards[b_index];
                        for (size_t x = 0; x < Board::width; x++) {
                            board.board[x] &= other_board.board[x];
                        }
                    }
                }

                inline void operator^=(const SmearedBoard& other) {
                    for (size_t b_index = 0; b_index < boards.size(); ++b_index) {
                        auto& board = boards[b_index];
                        const auto& other_board = other.boards[b_index];
                        for (size_t x = 0; x < Board::width; x++) {
                            board.board[x] ^= other_board.board[x];
                        }
                    }
                }

                inline SmearedBoard operator^(const SmearedBoard& other) const {
                    SmearedBoard ret;

                    for (size_t b_index = 0; b_index < boards.size(); ++b_index) {
                        auto& board = boards[b_index];
                        const auto& other_board = other.boards[b_index];
                        for (size_t x = 0; x < Board::width; x++) {
                            ret.boards[b_index].board[x] = board.board[x] ^ other_board.board[x];
                        }
                    }

                    return ret;
                }
            };

            struct SmearedPiece {
                Coord position;
                u8 rot;
            };

            inline void deduplicate(SmearedBoard& dedup, PieceType type) {
                if (type == PieceType::Z || type == PieceType::S || type == PieceType::I) {
                    dedup.boards[2].zero();
                    dedup.boards[3].zero();
                }
            }

            inline std::vector<Piece> moves_to_vec(const SmearedBoard& moves, PieceType type) {
                std::vector<Piece> ret;
                ret.reserve(150);
                for (size_t b_index = 0; b_index < moves.boards.size(); ++b_index) {
                    const auto& board = moves.boards[b_index];
                    for (size_t x = 0; x < Board::width; x++) {
                        auto col = moves.boards[b_index].board[x];
                        while (auto height = (sizeof(column_t) * CHAR_BIT) - std::countl_zero(col)) {
                            ret.emplace_back(type, (RotationDirection)b_index, Coord(x, height - 1));

                            col &= ~(1 << (height - 1)); // clear the bit
                        }
                    }
                }

                return ret;
            }

            inline std::vector<SmearedPiece> smeared_moves_to_vec(const SmearedBoard& moves, PieceType type) {
                std::vector<SmearedPiece> ret;
                ret.reserve(150);
                for (size_t b_index = 0; b_index < moves.boards.size(); ++b_index) {
                    const auto& board = moves.boards[b_index];
                    for (size_t x = 0; x < Board::width; x++) {
                        auto col = moves.boards[b_index].board[x];
                        while (auto height = (sizeof(column_t) * CHAR_BIT) - std::countl_zero(col)) {
                            ret.push_back(SmearedPiece{ Coord(x, height - 1), (u8)b_index });

                            col &= ~(1 << (height - 1)); // clear the bit
                        }
                    }
                }

                return ret;
            }

            inline SmearedBoard smear(const Board& board, PieceType type) {
                SmearedBoard ret{};
                std::array<column_t, 14> thick_board;
                // initialize the first and last two with ~0
                thick_board[0] = ~0;
                thick_board[1] = ~0;
                thick_board[12] = ~0;
                thick_board[13] = ~0;

                // initialize the middle with the board
                for (size_t i = 0; i < Board::width; i++) {
                    thick_board[i + 2] = board.board[i];
                }

                for (size_t rot = 0; rot < 4; rot++) {
                    for (const Coord& mino : rot_piece_def[static_cast<size_t>(type)][rot]) {
                        for (size_t x = 0; x < Board::width; x++) {
                            column_t c = thick_board[2 + x + mino.x];
                            bool cond = (mino.y < 0);
                            c = ((!cond) * (c >> mino.y)) | (cond * ~(~c << -mino.y));

                            ret.boards[rot].board[x] |= c;
                        }
                    }
                }

                return ret;
            }

            // movegen for only one rotation of the convex movegen
            inline Board partial_convex_movegen(const Board& board, const PieceType type) {
                Board ret{};
                for (size_t x = 0; x < Board::width; x++) {
                    // if column is nearly full skip
                    // (this is because the faster way of smearing does not set top bits because of the shift after the not)
                    // two because the I piece sticks out that long
                    bool cond = (board.board[x] >= std::numeric_limits<column_t>::max() >> 2);

                    auto height = (sizeof(column_t) * CHAR_BIT) - std::countl_zero(board.board[x]);
                    ret.board[x] = (!cond) * (1 << height);  // set the column to the height
                }

                return ret;
            }

            // Movegen for a convex board with free movement at the top. (decided by board height of 16 or lower)
            inline SmearedBoard convex_movegen(const Board& board, const PieceType type) {
                SmearedBoard ret;
                const SmearedBoard smeared_board = smear(board, type);
                for (size_t b_index = 0; b_index < smeared_board.boards.size(); ++b_index) {
                    const auto& s_board = smeared_board.boards[b_index];
                    ret.boards[b_index] = partial_convex_movegen(s_board, type);

                    if (type == PieceType::O && b_index == 0) {
                        break;
                    }
                    if ((
                        type == PieceType::I ||
                        type == PieceType::Z ||
                        type == PieceType::S
                        ) && b_index == 1) {
                        break;
                    }
                }

                return ret;
            }

            inline std::vector<Piece> nosrs_movegen(const Board& board, PieceType type) {
                // movegen without srs

                if (board.surface_convex()) {
                    return moves_to_vec(convex_movegen(board, type), type);
                }

                const SmearedBoard s_board = smear(board, type);

                SmearedBoard flood_old{};
                SmearedBoard flood_new = convex_movegen(board, type);

                bool convex = true;

                for (auto& board : s_board.boards) {
                    if (type == PieceType::O) {
                        if (!board.surface_convex()) {
                            convex = false;
                        }
                    } else if (!board.true_convex()) {
                        convex = false;
                    }
                }

                if (convex) {
                    return moves_to_vec(flood_new, type);
                }

                while (flood_new != flood_old) {
                    flood_old = flood_new;


                    // left & right
                    flood_new |= flood_new.shift();

                    // rotate
                    flood_new |= s_board.rotate_no_srs(flood_new, type);

                    // down 
                    flood_new |= s_board.smear_drop(flood_new);

                    // cull
                    s_board.non_collides(flood_new);
                }

                deduplicate(flood_new, type);

                // version of grounded() that doesn't require collision checking
                for (auto& board : flood_new.boards) {
                    for (size_t x = 0; x < Board::width; x++) {
                        // grounded pieces are last bits or bits followed by 0
                        board.board[x] = (board.board[x] & ~(board.board[x] << 1)) | (board.board[x] & 1);
                    }
                }
                return moves_to_vec(flood_new, type);
            }

            inline std::vector<Piece> movegen(const Board& board, PieceType type) {
                if (board.surface_convex()) {
                    return moves_to_vec(convex_movegen(board, type), type);
                }
                const SmearedBoard smeared_board = smear(board, type);
                SmearedBoard visited{};
                SmearedBoard open_nodes{};
                SmearedBoard next_nodes{};

                open_nodes.boards[0].board[4/* x */] = 1 << 19/* y */; // set north piece rotation to the piece position

                for (int i = 0; i < 5; ++i) { // valid shifts put onto the next nodes
                    open_nodes = open_nodes.shift();
                    smeared_board.non_collides(open_nodes);
                }
                while (!open_nodes.empty()) {

                    { // valid shifts put onto the next nodes
                        SmearedBoard shifted_pieces = open_nodes.shift();
                        smeared_board.non_collides(shifted_pieces);
                        visited.non_collides(shifted_pieces);
                        visited |= shifted_pieces;
                        next_nodes |= shifted_pieces;
                    }

                    { // valid rotations put onto the next nodes
                        SmearedBoard tmp_pieces = smeared_board.rotate_srs(open_nodes, type);
                        smeared_board.non_collides(tmp_pieces);
                        visited.non_collides(tmp_pieces);
                        visited |= tmp_pieces;
                        next_nodes |= tmp_pieces;
                    }

                    { // already taken care of by smear drop
                        SmearedBoard smeared_pieces = smeared_board.smear_drop(open_nodes);
                        // smeared_board.non_collides(smeared_pieces);
                        visited.non_collides(smeared_pieces);
                        visited |= smeared_pieces;
                        next_nodes |= smeared_pieces;
                    }

                    std::swap(open_nodes, next_nodes);
                    next_nodes = {};
                }

                return moves_to_vec(smeared_board.grounded(visited), type);
            }

            template <TurnDirection dir>
            inline void srs(const SmearedBoard& s_board, SmearedPiece& p, PieceType type) {
                const auto* offsets = &piece_offsets_JLSTZ;
                const auto* prev_offsets = &piece_offsets_JLSTZ;

                if (type == PieceType::I) {
                    offsets = &piece_offsets_I;
                    prev_offsets = &piece_offsets_I;
                }
                else if (type == PieceType::O) {
                    offsets = &piece_offsets_O;
                    prev_offsets = &piece_offsets_O;
                }

                u8 new_rot{};
                if constexpr (dir == TurnDirection::Right) {
                    new_rot = (p.rot + 1) % 4;
                } else {
                    new_rot = (p.rot + 3) % 4;
                }

                std::array<bool, srs_kicks> worked{};
                for (size_t i = 0; i < srs_kicks; ++i) {
                    const Coord offset = Coord(
                        (i8)(*prev_offsets)[p.rot][i].x - (i8)(*offsets)[static_cast<size_t>(new_rot)][i].x,
                        (i8)(*prev_offsets)[p.rot][i].y - (i8)(*offsets)[static_cast<size_t>(new_rot)][i].y);

                    SmearedPiece tmp = p;
                    tmp.position.x += offset.x;
                    tmp.position.y += offset.y;
                    tmp.rot = new_rot;

                    if (tmp.position.x >= 0 && tmp.position.x < Board::width && tmp.position.y >= 0) {
                        auto& col = s_board.boards[static_cast<size_t>(new_rot)].board[static_cast<size_t>(tmp.position.x)];
                        if (!(col & (column_t(1) << tmp.position.y))) {
							worked[i] = true;
                        }
                    }
                }
                SmearedPiece ret = p;

				// return the piece corresponding with the first successful srs
                for (int i = srs_kicks - 1; i >= 0; --i) {
                    if (worked[i]) {
                        const Coord offset = Coord(
                            (i8)(*prev_offsets)[p.rot][i].x - (i8)(*offsets)[static_cast<size_t>(new_rot)][i].x,
                            (i8)(*prev_offsets)[p.rot][i].y - (i8)(*offsets)[static_cast<size_t>(new_rot)][i].y);

                        ret = p;
                        ret.position.x += offset.x;
                        ret.position.y += offset.y;
                        ret.rot = new_rot;
                    }
                }
                p = ret;
            }

            // the original piece-by-piece movegen. god_movegen (Flood) returns the same moves much faster,
            // this one is kept as the reference it's checked against in bench.cpp
            inline std::vector<Piece> god_movegen_bfs(const Board& board, const PieceType type) {
                std::vector<Piece> ret;
                ret.reserve(150);
                if (board.surface_convex() && board.is_low()) {
                    ret = moves_to_vec(convex_movegen(board, type), type);
                    return ret;
                }
                const SmearedBoard s_board = smear(board, type);
                std::vector<SmearedPiece> open_nodes;
                open_nodes.reserve(150);
                std::vector<SmearedPiece> next_nodes;
                next_nodes.reserve(150);
                std::bitset<32 * 10 * 4> visited;
                std::bitset<32 * 10 * 4> returned;
                auto is_immobile = [](const SmearedBoard& board, const SmearedPiece& piece) {
                    bool left = false;
                    bool right = false;
                    bool down = false;
                    bool up = false;
                    if (piece.position.x == 0)
                        left = true;
                    else {
                        const auto col = board.boards[static_cast<size_t>(piece.rot)].board[static_cast<size_t>(piece.position.x - 1)];
                        if (col & (1 << piece.position.y))
                            left = true;
                    }

                    if (piece.position.x == Board::width - 1)
                        right = true;
                    else {
                        const auto col = board.boards[static_cast<size_t>(piece.rot)].board[static_cast<size_t>(piece.position.x + 1)];
                        if (col & (1 << piece.position.y))
                            right = true;
                    }

                    if (piece.position.y == 0)
                        down = true;
                    else {
                        const auto col = board.boards[static_cast<size_t>(piece.rot)].board[static_cast<size_t>(piece.position.x)];
                        if (col & (1 << (piece.position.y - 1)))
                            down = true;
                    }

                    if (piece.position.y == Board::height - 1)
                        up = true;
                    else {
                        const auto col = board.boards[static_cast<size_t>(piece.rot)].board[static_cast<size_t>(piece.position.x)];
                        if (col & (1 << (piece.position.y + 1)))
                            up = true;
                    }

                    return left & right & down & up;
                };
                // thanks Citrus for this piece of code!
                // https://github.com/citrus610/tetris-movegen/blob/bd6ff34145c8898a6365bdc603706e7f318430e5/src/piece.cpp#L25
                auto cannonicalize = [](const SmearedPiece& piece, PieceType type) {
					SmearedPiece ret = piece;
                    switch (type) {
                    case PieceType::I:
                        switch (piece.rot) {
                        case 0:
                            break;
                        case 1:
                            break;
                        case 2:
                            ret.rot = 0;
                            ret.position.x--;
                            break;
                        case 3:
                            ret.rot = 1;
                            ret.position.y++;
                            break;
                        }
                        break;
					case PieceType::S:
						switch (piece.rot) {
						case 0:
							break;
						case 1:
							break;
						case 2:
                            ret.rot = 0;
                            ret.position.y--;
							break;
						case 3:
                            ret.position.x--;
                            ret.rot = 1;
							break;
						}
						break;
					case PieceType::Z:
						switch (piece.rot) {
						case 0:
							break;
						case 1:
							break;
						case 2:
							ret.rot = 0;
							ret.position.y--;
							break;
						case 3:
							ret.position.x--;
							ret.rot = 1;
							break;
						}
						break;
					default:
						break;
                    }

					return ret;
			    };

                if (s_board.convex(PieceType::O == type)) {
                    SmearedBoard moves{};
                    for (size_t i = 0; i < 4; ++i) {
                        moves.boards[i] = partial_convex_movegen(s_board.boards[i], type);
                        if (type == PieceType::O && i == 0) {
                            break;
                        }
                        if ((
                                type == PieceType::I ||
                                type == PieceType::Z ||
                                type == PieceType::S) &&
                            i == 1) {
                            break;
                        }
                    }

                    ret = moves_to_vec(moves, type);
                    return ret;
                } else if (board.is_low()) {
                    SmearedBoard moves{};
                    for (int rot = 0; rot < 4; ++rot) {

                        moves.boards[rot] = partial_convex_movegen(s_board.boards[rot], type);

                        if (type == PieceType::O && rot == 0) {
                            break;
                        }
                        if ((
                                type == PieceType::I ||
                                type == PieceType::Z ||
                                type == PieceType::S) &&
                            rot == 1) {
                            break;
                        }
                    }
                    auto full_pieces = moves_to_vec(moves, type);

                    for(auto& piece : full_pieces) {
                        open_nodes.push_back(SmearedPiece{piece.position, (u8)piece.rotation});
                    }
                } else {
                    open_nodes.push_back({Coord((i8)4, (i8)19), 0});
                }

                while (true) {
                    for (const auto &piece : open_nodes) {
                        auto to_iter = [](auto x, auto y, auto r) {
                            return y + x * 32 + r * 32 * 10;
                        };
                        size_t iter = to_iter(piece.position.x, piece.position.y, piece.rot);

                        if (visited[iter])
                            continue;
                        visited[iter] = true;

                        // shift left
                        if (piece.position.x > 0) {
                            const auto col = s_board.boards[static_cast<size_t>(piece.rot)].board[static_cast<size_t>(piece.position.x - 1)];
                            SmearedPiece new_piece = { Coord(piece.position.x - 1, piece.position.y), piece.rot };
                            if (!(col & (1 << piece.position.y)) &&
                                !visited[to_iter(new_piece.position.x, new_piece.position.y, piece.rot)])
                                    next_nodes.push_back(new_piece);
                        }

                        // shift right
                        if (piece.position.x < Board::width - 1) {
                            const auto col = s_board.boards[static_cast<size_t>(piece.rot)].board[static_cast<size_t>(piece.position.x + 1)];
                            SmearedPiece new_piece = { Coord(piece.position.x + 1, piece.position.y), piece.rot };
							// if its not colliding with the board
							// if its not in the visited set push it to the next nodes
                            if (!(col & (1 << piece.position.y)) && 
                                !visited[to_iter(new_piece.position.x, new_piece.position.y, piece.rot)])
                                    next_nodes.push_back(new_piece);
                        }

                        // sonic drop
                        {
                            auto col = s_board.boards[static_cast<size_t>(piece.rot)].board[static_cast<size_t>(piece.position.x)];
                            // mask out every bit that is above the current y position
                            col &= (1 << piece.position.y) - 1;

                            const auto height = ((int)sizeof(column_t) * 8) - std::countl_zero(col);
                            // const auto height = std::clamp(piece.position.y - 1,0,32);
                            SmearedPiece new_piece = { Coord(piece.position.x, height), piece.rot };
							if (!visited[to_iter(new_piece.position.x, new_piece.position.y, piece.rot)])
								next_nodes.push_back(new_piece);
                        }

                        // rotate srs
                        if (type != PieceType::O) {
                            SmearedPiece next_piece = piece;

                            srs<TurnDirection::Right>(s_board, next_piece, type);

                            next_nodes.push_back({next_piece});

                            next_piece = piece;

                            srs<TurnDirection::Left>(s_board, next_piece, type);

                            next_nodes.push_back({next_piece});
                        }

                        // if is grounded push to ret
                        {
                            auto& col = s_board.boards[static_cast<size_t>(piece.rot)].board[static_cast<size_t>(piece.position.x)];

                            if ((piece.position.y == 0) || (col & (1 << (piece.position.y - 1)))) {
								SmearedPiece new_piece = cannonicalize(piece, type);
                                Piece p = Piece(type, (RotationDirection)new_piece.rot, new_piece.position, spinType::null);

								auto iter = to_iter(new_piece.position.x, new_piece.position.y, new_piece.rot);
                                // does not contain
                                if (!returned[iter]) {
									returned[iter] = true;
                                    bool is_immobile_piece =  is_immobile(s_board, piece);
                                    if (is_immobile_piece)
                                        p.spin = spinType::normal;
                                    ret.push_back(p);
                                }
                            }
                        }
                    }
                    std::swap(open_nodes, next_nodes);
                    next_nodes.clear();

					if (open_nodes.empty())
						break;
                }

                // go through all pieces and check if they are grounded

                // check which pieces are immobile all spin


                return ret;
            }
        }; // namespace Smeared

        // god_movegen (flood fill ver.)
        // same moves as god_movegen_bfs (shift, sonic drop, srs with the first kick that fits, same seeds, canonical
        // forms and spin flags), but flood filled over whole column bitboards instead of walking one piece at a time
        namespace Flood {
            using Smeared::SmearedBoard;

            // board types: all hold one column_t per x for x < Board::width and keep every other lane zero.
            // simd boards get the slide and precomputed kick paths, which only pay off with real vector registers

            // column-major bitboard as plain loops, for compilers without vector extensions (msvc)
            struct ColBoard {
                static constexpr bool simd = false;

                std::array<column_t, Board::width> c{};

                static inline ColBoard load(const Board& b) { return { b.board }; }

                static inline ColBoard lanes() {
                    ColBoard r;
                    r.c.fill(~column_t(0));
                    return r;
                }

                inline ColBoard operator&(const ColBoard& o) const { ColBoard r; for (size_t x = 0; x < Board::width; ++x) r.c[x] = c[x] & o.c[x]; return r; }
                inline ColBoard operator|(const ColBoard& o) const { ColBoard r; for (size_t x = 0; x < Board::width; ++x) r.c[x] = c[x] | o.c[x]; return r; }
                inline ColBoard& operator|=(const ColBoard& o) { for (size_t x = 0; x < Board::width; ++x) c[x] |= o.c[x]; return *this; }
                // this & ~o
                inline ColBoard andnot(const ColBoard& o) const { ColBoard r; for (size_t x = 0; x < Board::width; ++x) r.c[x] = c[x] & ~o.c[x]; return r; }

                template <int dy>
                inline ColBoard shift_y() const {
                    ColBoard r;
                    for (size_t x = 0; x < Board::width; ++x) {
                        if constexpr (dy >= 0) r.c[x] = c[x] << dy;
                        else r.c[x] = c[x] >> -dy;
                    }
                    return r;
                }

                inline ColBoard or_bits(column_t bits) const { ColBoard r; for (size_t x = 0; x < Board::width; ++x) r.c[x] = c[x] | bits; return r; }

                // moves every piece position by (dx, dy), dropping what leaves the board
                template <int dx, int dy>
                inline ColBoard shift() const {
                    ColBoard r;
                    for (int x = 0; x < (int)Board::width; ++x) {
                        const int s = x - dx;
                        if (s < 0 || s >= (int)Board::width)
                            continue;
                        if constexpr (dy >= 0) r.c[x] = c[s] << dy;
                        else r.c[x] = c[s] >> -dy;
                    }
                    return r;
                }

                inline bool any() const {
                    column_t acc = 0;
                    for (size_t x = 0; x < Board::width; ++x)
                        acc |= c[x];
                    return acc != 0;
                }

                inline int popcount() const {
                    int n = 0;
                    for (size_t x = 0; x < Board::width; ++x)
                        n += std::popcount(c[x]);
                    return n;
                }

                inline void store(column_t* out) const { std::memcpy(out, c.data(), sizeof(column_t) * Board::width); }

                // the bit right above each column's top, same as partial_convex_movegen
                inline ColBoard above_top() const {
                    ColBoard r;
                    for (size_t x = 0; x < Board::width; ++x)
                        r.c[x] = c[x] >= (~column_t(0) >> 2) ? 0 : column_t(1) << (32 - std::countl_zero(c[x]));
                    return r;
                }
            };

#if defined(__GNUC__) || defined(__clang__)
            // 16 lanes through the compiler's vector extensions, lanes 0..9 are columns.
            // lowers to one zmm, two ymm or four wasm v128 registers depending on the target
            struct ColBoardVec {
                static constexpr bool simd = true;
                typedef column_t vec __attribute__((vector_size(64)));
                typedef u64 vec64 __attribute__((vector_size(64)));
                static constexpr int W = Board::width;

                vec v;

                // source lane for lane x after moving every column by dx, 16 = zero
                static constexpr int lane_src(int x, int dx) {
                    const int s = x - dx;
                    return (x < W && s >= 0 && s < W) ? s : 16;
                }

                template <int dx>
                static inline vec shift_lanes(vec a) {
                    return [&]<int... X>(std::integer_sequence<int, X...>) {
                        return __builtin_shufflevector(a, vec{}, lane_src(X, dx)...);
                    }(std::make_integer_sequence<int, 16>{});
                }

                static inline ColBoardVec load(const Board& b) {
                    ColBoardVec r{};
                    std::memcpy(&r.v, b.board.data(), sizeof(column_t) * W);
                    return r;
                }

                static inline ColBoardVec lanes() {
                    return { vec{ ~0u, ~0u, ~0u, ~0u, ~0u, ~0u, ~0u, ~0u, ~0u, ~0u, 0, 0, 0, 0, 0, 0 } };
                }

                inline ColBoardVec operator&(const ColBoardVec& o) const { return { v & o.v }; }
                inline ColBoardVec operator|(const ColBoardVec& o) const { return { v | o.v }; }
                inline ColBoardVec& operator|=(const ColBoardVec& o) { v |= o.v; return *this; }
                inline ColBoardVec andnot(const ColBoardVec& o) const { return { v & ~o.v }; }

                template <int dy>
                inline ColBoardVec shift_y() const {
                    if constexpr (dy > 0) return { v << dy };
                    else if constexpr (dy < 0) return { v >> -dy };
                    else return *this;
                }

                inline ColBoardVec or_bits(column_t bits) const { return { v | (lanes().v & bits) }; }

                template <int dx, int dy>
                inline ColBoardVec shift() const {
                    if constexpr (dx == 0) return shift_y<dy>();
                    else return { shift_lanes<dx>(shift_y<dy>().v) };
                }

                inline bool any() const {
                    const vec64 q = (vec64)v;
                    return ((q[0] | q[1]) | (q[2] | q[3]) | q[4]) != 0; // lanes 10..15 are zero
                }

                inline int popcount() const {
                    const vec64 q = (vec64)v;
                    return std::popcount(q[0]) + std::popcount(q[1]) + std::popcount(q[2]) + std::popcount(q[3]) + std::popcount(q[4]);
                }

                inline void store(column_t* out) const { std::memcpy(out, &v, sizeof(column_t) * W); }

                inline ColBoardVec above_top() const {
                    ColBoardVec r{};
                    for (int x = 0; x < W; ++x)
                        r.v[x] = v[x] >= (~column_t(0) >> 2) ? 0 : column_t(1) << (32 - std::countl_zero(v[x]));
                    return r;
                }
            };

            // same as ColBoardVec but as two 8 lane halves: columns 0..7 in lo, 8..9 in hi[0..1].
            // gcc can't shuffle a 64 byte vector without avx512 and falls back to scalar, but it can shuffle these
            struct ColBoardVec2 {
                static constexpr bool simd = true;
                typedef column_t vec __attribute__((vector_size(32)));
                typedef u64 vec64 __attribute__((vector_size(32)));
                static constexpr int W = Board::width;

                vec lo, hi;

                // shuffle index for lane x after moving every column by dx. lo is 0..7, hi is 8..15,
                // and hi[7] (15) is always zero so it doubles as the zero source
                static constexpr int lane_src(int x, int dx) {
                    const int s = x - dx;
                    return (x < W && s >= 0 && s < W) ? s : 15;
                }

                template <int dx>
                inline ColBoardVec2 shift_lanes() const {
                    return [&]<int... X>(std::integer_sequence<int, X...>) {
                        return ColBoardVec2{ __builtin_shufflevector(lo, hi, lane_src(X, dx)...),
                                             __builtin_shufflevector(lo, hi, lane_src(X + 8, dx)...) };
                    }(std::make_integer_sequence<int, 8>{});
                }

                static inline ColBoardVec2 load(const Board& b) {
                    ColBoardVec2 r{};
                    std::memcpy(&r.lo, b.board.data(), sizeof(column_t) * 8);
                    std::memcpy(&r.hi, b.board.data() + 8, sizeof(column_t) * (W - 8));
                    return r;
                }

                static inline ColBoardVec2 lanes() {
                    return { vec{ ~0u, ~0u, ~0u, ~0u, ~0u, ~0u, ~0u, ~0u }, vec{ ~0u, ~0u, 0, 0, 0, 0, 0, 0 } };
                }

                inline ColBoardVec2 operator&(const ColBoardVec2& o) const { return { lo & o.lo, hi & o.hi }; }
                inline ColBoardVec2 operator|(const ColBoardVec2& o) const { return { lo | o.lo, hi | o.hi }; }
                inline ColBoardVec2& operator|=(const ColBoardVec2& o) { lo |= o.lo; hi |= o.hi; return *this; }
                inline ColBoardVec2 andnot(const ColBoardVec2& o) const { return { lo & ~o.lo, hi & ~o.hi }; }

                template <int dy>
                inline ColBoardVec2 shift_y() const {
                    if constexpr (dy > 0) return { lo << dy, hi << dy };
                    else if constexpr (dy < 0) return { lo >> -dy, hi >> -dy };
                    else return *this;
                }

                inline ColBoardVec2 or_bits(column_t bits) const { const ColBoardVec2 m = lanes(); return { lo | (m.lo & bits), hi | (m.hi & bits) }; }

                template <int dx, int dy>
                inline ColBoardVec2 shift() const {
                    if constexpr (dx == 0) return shift_y<dy>();
                    else return shift_y<dy>().template shift_lanes<dx>();
                }

                inline bool any() const {
                    const vec64 q = (vec64)(lo | hi);
                    return ((q[0] | q[1]) | (q[2] | q[3])) != 0;
                }

                inline int popcount() const {
                    const vec64 a = (vec64)lo;
                    const vec64 b = (vec64)hi;
                    return std::popcount(a[0]) + std::popcount(a[1]) + std::popcount(a[2]) + std::popcount(a[3]) + std::popcount(b[0]);
                }

                inline void store(column_t* out) const {
                    std::memcpy(out, &lo, sizeof(column_t) * 8);
                    std::memcpy(out + 8, &hi, sizeof(column_t) * (W - 8));
                }

                inline ColBoardVec2 above_top() const {
                    column_t c[16]{};
                    store(c);
                    for (int x = 0; x < W; ++x)
                        c[x] = c[x] >= (~column_t(0) >> 2) ? 0 : column_t(1) << (32 - std::countl_zero(c[x]));
                    ColBoardVec2 r{};
                    std::memcpy(&r.lo, c, sizeof(column_t) * 8);
                    std::memcpy(&r.hi, c + 8, sizeof(column_t) * (W - 8));
                    return r;
                }
            };
#endif

#if defined(__AVX512F__)
            // the whole board in one zmm register, lanes 0..9 are columns
            struct ColBoard512 {
                static constexpr bool simd = true;

                __m512i v;

                static constexpr __mmask16 lane_mask = (1u << Board::width) - 1;

                static inline ColBoard512 load(const Board& b) { return { _mm512_maskz_loadu_epi32(lane_mask, b.board.data()) }; }
                static inline ColBoard512 lanes() { return { _mm512_maskz_set1_epi32(lane_mask, -1) }; }

                inline ColBoard512 operator&(const ColBoard512& o) const { return { _mm512_and_si512(v, o.v) }; }
                inline ColBoard512 operator|(const ColBoard512& o) const { return { _mm512_or_si512(v, o.v) }; }
                inline ColBoard512& operator|=(const ColBoard512& o) { v = _mm512_or_si512(v, o.v); return *this; }
                inline ColBoard512 andnot(const ColBoard512& o) const { return { _mm512_andnot_si512(o.v, v) }; }

                template <int dy>
                inline ColBoard512 shift_y() const {
                    if constexpr (dy > 0) return { _mm512_slli_epi32(v, dy) };
                    else if constexpr (dy < 0) return { _mm512_srli_epi32(v, -dy) };
                    else return *this;
                }

                inline ColBoard512 or_bits(column_t bits) const { return { _mm512_mask_or_epi32(v, lane_mask, v, _mm512_set1_epi32((int)bits)) }; }

                template <int dx, int dy>
                inline ColBoard512 shift() const {
                    __m512i r = shift_y<dy>().v;
                    if constexpr (dx > 0)
                        r = _mm512_maskz_alignr_epi32(lane_mask, r, _mm512_setzero_si512(), 16 - dx);
                    else if constexpr (dx < 0)
                        r = _mm512_alignr_epi32(_mm512_setzero_si512(), r, -dx);
                    return { r };
                }

                inline bool any() const { return _mm512_test_epi32_mask(v, v) != 0; }

                inline int popcount() const {
#if defined(__AVX512VPOPCNTDQ__)
                    return _mm512_reduce_add_epi32(_mm512_popcnt_epi32(v));
#else
                    alignas(64) column_t buf[16];
                    _mm512_store_si512(buf, v);
                    int n = 0;
                    for (size_t x = 0; x < Board::width; ++x)
                        n += std::popcount(buf[x]);
                    return n;
#endif
                }

                inline void store(column_t* out) const { _mm512_mask_storeu_epi32(out, lane_mask, v); }

                inline ColBoard512 above_top() const {
#if defined(__AVX512CD__)
                    const __m512i height = _mm512_sub_epi32(_mm512_set1_epi32(32), _mm512_lzcnt_epi32(v));
                    const __mmask16 ok = _mm512_mask_cmplt_epu32_mask(lane_mask, v, _mm512_set1_epi32((int)(~column_t(0) >> 2)));
                    return { _mm512_maskz_sllv_epi32(ok, _mm512_set1_epi32(1), height) };
#else
                    alignas(64) column_t buf[16]{};
                    store(buf);
                    for (size_t x = 0; x < Board::width; ++x)
                        buf[x] = buf[x] >= (~column_t(0) >> 2) ? 0 : column_t(1) << (32 - std::countl_zero(buf[x]));
                    return { _mm512_maskz_loadu_epi32(lane_mask, buf) };
#endif
                }
            };
#endif

            // fastest board for the target, picked from perft benchmarks on zen 5
#if defined(__AVX512F__)
            using DefaultBoard = ColBoard512;
#elif defined(__clang__) && !defined(_WIN32)
            using DefaultBoard = ColBoardVec;
#elif defined(__GNUC__) && !defined(__clang__) && defined(__AVX2__)
            using DefaultBoard = ColBoardVec2;
#else
            using DefaultBoard = ColBoard;
#endif

            // kicks[from][to][i] = offset of the i-th srs test when rotating from -> to
            using KickTable = std::array<std::array<std::array<Coord, srs_kicks>, RotationDirections_N>, RotationDirections_N>;

            consteval KickTable make_kicks(PieceType type) {
                const auto& offsets = type == PieceType::I ? piece_offsets_I : type == PieceType::O ? piece_offsets_O : piece_offsets_JLSTZ;
                KickTable k{};
                for (size_t from = 0; from < RotationDirections_N; ++from)
                    for (size_t to = 0; to < RotationDirections_N; ++to)
                        for (size_t i = 0; i < srs_kicks; ++i)
                            k[from][to][i] = Coord((i8)(offsets[from][i].x - offsets[to][i].x), (i8)(offsets[from][i].y - offsets[to][i].y));
                return k;
            }

            template <PieceType P>
            constexpr KickTable kick_table = make_kicks(P);

            // per rotation masks derived from the smeared collision board
            template <class BB>
            struct FloodBoard {
                std::array<BB, 4> coll;
                std::array<BB, 4> free;
                std::array<BB, 4> ground; // free positions resting on the stack or floor
                // fall[r][k]: free runs of length 2^k going up, used for a log-step drop fill
                std::array<std::array<BB, 5>, 4> fall;
                // left[r][k] / right[r][k]: free runs of length 2^k going right / left, for a log-step slide
                std::array<std::array<BB, 4>, 4> left;
                std::array<std::array<BB, 4>, 4> right;

                explicit FloodBoard(const SmearedBoard& s) {
                    const BB lanes = BB::lanes();
                    for (size_t r = 0; r < 4; ++r) {
                        coll[r] = BB::load(s.boards[r]);
                        free[r] = lanes.andnot(coll[r]);
                        ground[r] = free[r] & coll[r].template shift_y<1>().or_bits(1);
                        fall[r][0] = free[r];
                        fall[r][1] = fall[r][0] & fall[r][0].template shift_y<-1>();
                        fall[r][2] = fall[r][1] & fall[r][1].template shift_y<-2>();
                        fall[r][3] = fall[r][2] & fall[r][2].template shift_y<-4>();
                        fall[r][4] = fall[r][3] & fall[r][3].template shift_y<-8>();
                        if constexpr (BB::simd) {
                            left[r][0] = free[r];
                            left[r][1] = left[r][0] & left[r][0].template shift<-1, 0>();
                            left[r][2] = left[r][1] & left[r][1].template shift<-2, 0>();
                            left[r][3] = left[r][2] & left[r][2].template shift<-4, 0>();
                            right[r][0] = free[r];
                            right[r][1] = right[r][0] & right[r][0].template shift<1, 0>();
                            right[r][2] = right[r][1] & right[r][1].template shift<2, 0>();
                            right[r][3] = right[r][2] & right[r][2].template shift<4, 0>();
                        }
                    }
                }

                // every position reachable by moving left or right, any distance on simd boards, one step otherwise
                inline BB slide(const BB& g, size_t r) const {
                    if constexpr (!BB::simd) {
                        return (g.template shift<-1, 0>() | g.template shift<1, 0>()) & free[r];
                    }
                    else {
                        BB l = g, rt = g;
                        l |= l.template shift<-1, 0>() & left[r][0];
                        rt |= rt.template shift<1, 0>() & right[r][0];
                        l |= l.template shift<-2, 0>() & left[r][1];
                        rt |= rt.template shift<2, 0>() & right[r][1];
                        l |= l.template shift<-4, 0>() & left[r][2];
                        rt |= rt.template shift<4, 0>() & right[r][2];
                        l |= l.template shift<-8, 0>() & left[r][3];
                        rt |= rt.template shift<8, 0>() & right[r][3];
                        return l | rt;
                    }
                }

                // sonic drop every position
                inline BB drop(BB g, size_t r) const {
                    g |= g.template shift_y<-1>() & fall[r][0];
                    g |= g.template shift_y<-2>() & fall[r][1];
                    g |= g.template shift_y<-4>() & fall[r][2];
                    g |= g.template shift_y<-8>() & fall[r][3];
                    g |= g.template shift_y<-16>() & fall[r][4];
                    return g & ground[r];
                }

                // positions that can't move left, right, up or down
                inline BB immobile(size_t r) const {
                    const BB& f = free[r];
                    const BB mobile = f.template shift<1, 0>() | f.template shift<-1, 0>() | f.template shift_y<1>() | f.template shift_y<-1>();
                    return f.andnot(mobile);
                }
            };

            template <class BB>
            struct Gen {
                // precomputing which kick every position takes makes the kicks independent of each other.
                // that's about 1.3x faster with gcc and about 1.5x slower with clang, which then prefers the chained kicks
#if defined(__clang__)
                static constexpr bool pre_kicks = false;
#else
                static constexpr bool pre_kicks = BB::simd;
#endif

                // kick_src[from][dir][i]: positions whose first fitting kick is i, dir 0 = right, 1 = left
                using KickSrc = std::array<std::array<std::array<BB, srs_kicks>, 2>, 4>;

                static constexpr size_t rot_to(size_t from, size_t dir) { return dir == 0 ? (from + 1) % 4 : (from + 3) % 4; }

                // f.template operator()<From, Dir, I>() for every kick
                template <class F>
                static inline void for_each_kick(F&& f) {
                    [&]<size_t... J>(std::index_sequence<J...>) {
                        (f.template operator()<J / (2 * srs_kicks), (J / srs_kicks) % 2, J % srs_kicks>(), ...);
                    }(std::make_index_sequence<4 * 2 * srs_kicks>{});
                }

                template <PieceType P>
                static inline void make_kick_src(const FloodBoard<BB>& fb, KickSrc& src) {
                    std::array<std::array<BB, 2>, 4> taken{};
                    for_each_kick([&]<size_t From, size_t Dir, size_t I>() {
                        constexpr Coord k = kick_table<P>[From][rot_to(From, Dir)][I];
                        const BB fits = fb.free[rot_to(From, Dir)].template shift<-k.x, -k.y>();
                        src[From][Dir][I] = fits.andnot(taken[From][Dir]);
                        taken[From][Dir] |= fits;
                    });
                }

                // one srs kick applied to every position still looking for a kick, false once none are left
                template <PieceType P, size_t From, size_t To, size_t I>
                static inline bool kick_chained(BB& rem, const BB& free_to, BB& next_to) {
                    constexpr Coord k = kick_table<P>[From][To][I];
                    const BB t = rem.template shift<k.x, k.y>() & free_to;
                    next_to |= t;
                    rem = rem.andnot(t.template shift<-k.x, -k.y>());
                    return rem.any();
                }

                template <PieceType P, size_t From, size_t To>
                static inline void rotate_chained(BB rem, const BB& free_to, BB& next_to) {
                    if (!rem.any())
                        return;
                    [&]<size_t... I>(std::index_sequence<I...>) {
                        (kick_chained<P, From, To, I>(rem, free_to, next_to) && ...);
                    }(std::make_index_sequence<srs_kicks>{});
                }

                template <PieceType P>
                static inline void rotate(const std::array<BB, 4>& from, const FloodBoard<BB>& fb, const KickSrc& src, std::array<BB, 4>& next) {
                    if constexpr (pre_kicks) {
                        for_each_kick([&]<size_t From, size_t Dir, size_t I>() {
                            constexpr Coord k = kick_table<P>[From][rot_to(From, Dir)][I];
                            next[rot_to(From, Dir)] |= (from[From] & src[From][Dir][I]).template shift<k.x, k.y>();
                        });
                    }
                    else {
                        [&]<size_t... R>(std::index_sequence<R...>) {
                            (rotate_chained<P, R, rot_to(R, 0)>(from[R], fb.free[rot_to(R, 0)], next[rot_to(R, 0)]), ...);
                            (rotate_chained<P, R, rot_to(R, 1)>(from[R], fb.free[rot_to(R, 1)], next[rot_to(R, 1)]), ...);
                        }(std::make_index_sequence<4>{});
                    }
                }

                // moves the duplicate orientation onto its canonical one, see cannonicalize in god_movegen_bfs
                template <size_t From, size_t To, int dx, int dy>
                static inline void fold(std::array<BB, 4>& b) {
                    b[To] |= b[From].template shift<dx, dy>();
                    b[From] = BB{};
                }

                template <PieceType P>
                static inline void canonicalize(std::array<BB, 4>& b) {
                    if constexpr (P == PieceType::I) {
                        fold<2, 0, -1, 0>(b);
                        fold<3, 1, 0, 1>(b);
                    }
                    else if constexpr (P == PieceType::S || P == PieceType::Z) {
                        fold<2, 0, 0, -1>(b);
                        fold<3, 1, -1, 0>(b);
                    }
                }

                // grounded placements reachable from the seeds
                template <PieceType P, bool WantSpin>
                static inline void placements(const SmearedBoard& s, const Board& board, std::array<BB, 4>& moves, std::array<BB, 4>& spin) {
                    constexpr bool rotates = P != PieceType::O;
                    const FloodBoard<BB> fb(s);

                    KickSrc src;
                    if constexpr (rotates && pre_kicks)
                        make_kick_src<P>(fb, src);

                    std::array<BB, 4> reach{};
                    if (board.is_low()) {
                        // same seeds as god_movegen_bfs: every column dropped from the top
                        constexpr size_t seeded = P == PieceType::O ? 1 : (P == PieceType::I || P == PieceType::S || P == PieceType::Z) ? 2 : 4;
                        for (size_t r = 0; r < seeded; ++r)
                            reach[r] = fb.coll[r].above_top();
                    }
                    else {
                        Board spawn{};
                        spawn.set(4, 19);
                        reach[0] = BB::load(spawn);
                    }

                    std::array<BB, 4> frontier = reach;
                    while (true) {
                        std::array<BB, 4> next{};
                        for (size_t r = 0; r < 4; ++r) {
                            const BB& f = frontier[r];
                            if (f.any())
                                next[r] = fb.slide(f, r) | fb.drop(f, r);
                        }

                        if constexpr (rotates)
                            rotate<P>(frontier, fb, src, next);

                        bool any = false;
                        for (size_t r = 0; r < 4; ++r) {
                            next[r] = next[r].andnot(reach[r]);
                            reach[r] |= next[r];
                            any |= next[r].any();
                        }
                        if (!any)
                            break;
                        frontier = next;
                    }

                    for (size_t r = 0; r < 4; ++r) {
                        moves[r] = reach[r] & fb.ground[r];
                        if constexpr (WantSpin)
                            spin[r] = moves[r] & fb.immobile(r);
                    }

                    canonicalize<P>(moves);
                    if constexpr (WantSpin)
                        canonicalize<P>(spin);
                }

                template <PieceType P>
                static inline std::vector<Piece> movegen(const Board& board) {
                    if (board.surface_convex() && board.is_low())
                        return Smeared::moves_to_vec(Smeared::convex_movegen(board, P), P);

                    const SmearedBoard s = Smeared::smear(board, P);
                    if (s.convex(PieceType::O == P))
                        return Smeared::moves_to_vec(convex_moves(s, P), P);

                    std::array<BB, 4> moves, spin;
                    placements<P, true>(s, board, moves, spin);

                    std::vector<Piece> ret;
                    ret.reserve(150);
                    for (size_t r = 0; r < 4; ++r) {
                        column_t m[Board::width], sp[Board::width];
                        moves[r].store(m);
                        spin[r].store(sp);
                        for (size_t x = 0; x < Board::width; ++x) {
                            for (column_t col = m[x]; col; col &= col - 1) {
                                const int y = std::countr_zero(col);
                                const bool is_spin = (sp[x] >> y) & 1;
                                ret.emplace_back(P, (RotationDirection)r, Coord((i8)x, (i8)y), is_spin ? spinType::normal : spinType::null);
                            }
                        }
                    }
                    return ret;
                }

                template <PieceType P>
                static inline size_t count(const Board& board) {
                    if (board.surface_convex() && board.is_low())
                        return popcount(Smeared::convex_movegen(board, P));

                    const SmearedBoard s = Smeared::smear(board, P);
                    if (s.convex(PieceType::O == P))
                        return popcount(convex_moves(s, P));

                    std::array<BB, 4> moves, spin;
                    placements<P, false>(s, board, moves, spin);
                    size_t n = 0;
                    for (const BB& m : moves)
                        n += m.popcount();
                    return n;
                }

                static inline SmearedBoard convex_moves(const SmearedBoard& s, PieceType type) {
                    SmearedBoard moves{};
                    for (size_t r = 0; r < 4; ++r) {
                        moves.boards[r] = Smeared::partial_convex_movegen(s.boards[r], type);
                        if (type == PieceType::O && r == 0)
                            break;
                        if ((type == PieceType::I || type == PieceType::Z || type == PieceType::S) && r == 1)
                            break;
                    }
                    return moves;
                }

                static inline size_t popcount(const SmearedBoard& s) {
                    size_t n = 0;
                    for (const Board& b : s.boards)
                        for (column_t col : b.board)
                            n += std::popcount(col);
                    return n;
                }
            };

            // dispatch a runtime piece type to the per-piece template
            template <class F>
            inline decltype(auto) with_piece(PieceType type, F&& f) {
                switch (type) {
                case PieceType::S: return f.template operator()<PieceType::S>();
                case PieceType::Z: return f.template operator()<PieceType::Z>();
                case PieceType::J: return f.template operator()<PieceType::J>();
                case PieceType::L: return f.template operator()<PieceType::L>();
                case PieceType::T: return f.template operator()<PieceType::T>();
                case PieceType::O: return f.template operator()<PieceType::O>();
                default: return f.template operator()<PieceType::I>();
                }
            }

            template <class BB = DefaultBoard>
            inline std::vector<Piece> movegen(const Board& board, PieceType type) {
                return with_piece(type, [&]<PieceType P>() { return Gen<BB>::template movegen<P>(board); });
            }

            template <class BB = DefaultBoard>
            inline size_t count(const Board& board, PieceType type) {
                return with_piece(type, [&]<PieceType P>() { return Gen<BB>::template count<P>(board); });
            }
        }; // namespace Flood

        namespace Smeared {
            inline std::vector<Piece> god_movegen(const Board& board, const PieceType type) {
                return Flood::movegen(board, type);
            }

            // number of moves god_movegen would return, without building them
            inline size_t god_movegen_count(const Board& board, const PieceType type) {
                return Flood::count(board, type);
            }
        }; // namespace Smeared
    }; // namespace MoveGen
}; // namespace Shaktris

