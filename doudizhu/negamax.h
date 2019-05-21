//
// Created by deng on 18-11-28.
//

#ifndef DOUDIZHU_ENDGAME_NEGAMAX_H
#define DOUDIZHU_ENDGAME_NEGAMAX_H

#include <vector>
#include <cstdint>
#include <cstring>

#include "cardset.h"
#include "hand.h"

namespace doudizhu_endgame {

#define TRANSPOSITION_TABLE_SIZE (1024*1024*2)
#define TRANSPOSITION_TABLE_SIZE_MASK ((TRANSPOSITION_TABLE_SIZE) - 1)
#define FARMER_PLAY 1
#define LORD_PLAY   0

class TranspositionTable {
public:

    TranspositionTable();

    ~TranspositionTable();

    struct HashItem {
        uint32_t key{};
        int8_t score{};
    };

    void reset();

    void add(uint32_t key, int8_t score);

    int8_t get(uint32_t key);

    uint32_t gen_key(const CardSet &lord, const CardSet &farmer, const CardSet &last_move, int8_t turn);

private:

    HashItem *table_;
};

class Negamax {
public:

    Negamax() = default;

    ~Negamax() = default;

    Pattern best_move{};

    bool search(const CardSet &lord, const CardSet &farmer);

    bool search(const CardSet &lord, const CardSet &farmer, const Pattern &last);

    size_t nodes_searched();

    double hash_hit_rate();

    void reset_counter();

    void reset_transposition_table();

private:
    TranspositionTable transposition_table_;
    size_t nodes_searched_{};
    size_t hash_hit_{};

    int8_t negamax_dev(const CardSet &loard, const CardSet &farmer, const Pattern &last_move, int8_t turn);
};
} //namespace doudizhu_endgame

#endif //DOUDIZHU_ENDGAME_NEGAMAX_H
