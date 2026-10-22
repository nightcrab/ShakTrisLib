// perft bench for the movegen: checks every board type against god_movegen_bfs, then times perft
// usage: bench [depth = 6] [reps = 3] [bfs = 0]
#include <algorithm>
#include <chrono>
#include <cstdio>
#include <string>
#include <tuple>
#include <vector>

#include "engine/Board.hpp"
#include "engine/MoveGen.hpp"

using namespace Shaktris::MoveGen;

static constexpr PieceType queue[] = { PieceType::I, PieceType::O, PieceType::T, PieceType::L, PieceType::J, PieceType::S, PieceType::Z };

struct Variant {
    std::string name;
    std::vector<Piece> (*gen)(const Board&, PieceType);
    size_t (*count)(const Board&, PieceType); // nullptr => gen().size()
};

static uint64_t perft(const Board& b, const PieceType* q, int depth, const Variant& v) {
    if (depth == 1)
        return v.count ? v.count(b, *q) : v.gen(b, *q).size();
    uint64_t n = 0;
    for (const Piece& m : v.gen(b, *q)) {
        Board c = b;
        c.set(m);
        c.clearLines();
        n += perft(c, q + 1, depth - 1, v);
    }
    return n;
}

using Key = std::tuple<int, int, int, int>; // rot, x, y, spin

static std::vector<Key> keys(const std::vector<Piece>& moves) {
    std::vector<Key> k;
    for (const Piece& p : moves)
        k.emplace_back((int)p.rotation, p.position.x, p.position.y, (int)p.spin);
    std::sort(k.begin(), k.end());
    return k;
}

// walks the god_movegen_bfs tree and counts the boards where the variant returns a different move set
static uint64_t check(const Board& b, const PieceType* q, int depth, const Variant& v, uint64_t& boards) {
    const auto ref = Smeared::god_movegen_bfs(b, *q);
    boards++;
    uint64_t bad = keys(ref) != keys(v.gen(b, *q)) || (v.count && v.count(b, *q) != ref.size());
    if (depth == 1)
        return bad;
    for (const Piece& m : ref) {
        Board c = b;
        c.set(m);
        c.clearLines();
        bad += check(c, q + 1, depth - 1, v, boards);
    }
    return bad;
}

template <class BB>
static void add(std::vector<Variant>& out, const char* name) {
    out.push_back({ std::string(name) + " vec", &Flood::movegen<BB>, nullptr });
    out.push_back({ std::string(name) + " count", &Flood::movegen<BB>, &Flood::count<BB> });
}

int main(int argc, char** argv) {
    const int depth = argc > 1 ? std::stoi(argv[1]) : 6;
    const int reps = argc > 2 ? std::stoi(argv[2]) : 3;
    const bool run_bfs = argc > 3 && std::stoi(argv[3]) != 0;

    std::vector<Variant> variants;
    variants.push_back({ "god_movegen", &Smeared::god_movegen, nullptr });
    variants.push_back({ "god_movegen_count", &Smeared::god_movegen, &Smeared::god_movegen_count });
    add<Flood::ColBoard>(variants, "ColBoard");
#if defined(__GNUC__) || defined(__clang__)
    add<Flood::ColBoardVec>(variants, "ColBoardVec");
    add<Flood::ColBoardVec2>(variants, "ColBoardVec2");
#endif
#if defined(__AVX512F__)
    add<Flood::ColBoard512>(variants, "ColBoard512");
#endif

    for (const Variant& v : variants) {
        uint64_t boards = 0;
        const uint64_t bad = check(Board{}, queue, 4, v, boards);
        std::printf("check %-20s %llu / %llu boards differ from god_movegen_bfs\n", v.name.c_str(), (unsigned long long)bad, (unsigned long long)boards);
    }

    if (run_bfs)
        variants.push_back({ "god_movegen_bfs", &Smeared::god_movegen_bfs, nullptr });

    for (const Variant& v : variants) {
        uint64_t nodes = 0;
        double best = 1e30;
        for (int i = 0; i < reps; ++i) {
            const auto start = std::chrono::steady_clock::now();
            nodes = perft(Board{}, queue, depth, v);
            best = std::min(best, std::chrono::duration<double, std::milli>(std::chrono::steady_clock::now() - start).count());
        }
        std::printf("perft d%d %-20s nodes %llu best %.1f ms\n", depth, v.name.c_str(), (unsigned long long)nodes, best);
    }
    return 0;
}
