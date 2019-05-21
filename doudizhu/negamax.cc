//
// Created by deng on 18-11-28.
//
#include "negamax.h"

namespace doudizhu_endgame {

int8_t Negamax::negamax_dev(const CardSet &lord, const CardSet &farmer, const Pattern &last_move, int8_t turn)
{
    nodes_searched_ += 1;
    if (lord.empty() || farmer.empty()) {
        return -1;
    }

    uint32_t key = transposition_table_.gen_key(lord, farmer, last_move.hand, turn);
    int8_t search = transposition_table_.get(key);
    if (search != 0) {
        hash_hit_ += 1;
        return search;
    }

    int8_t score = -1;
    if (turn == FARMER_PLAY) {//farmer
        std::vector<Pattern> selections;
        DouDiZhuHand::next_hand(farmer, last_move, selections);
        for (Pattern &move : selections) {
            CardSet after_play;
            DouDiZhuHand::play(farmer, move.hand, after_play);
            int8_t val = -negamax_dev(lord, after_play, move, LORD_PLAY);
            if (val > 0) {
                score = val;
                break;
            }
        }

    } else {//lord
        std::vector<Pattern> selections;
        DouDiZhuHand::next_hand(lord, last_move, selections);
        for (Pattern &move : selections) {
            CardSet after_play;
            DouDiZhuHand::play(lord, move.hand, after_play);
            int8_t val = -negamax_dev(after_play, farmer, move, FARMER_PLAY);
            if (val > 0) {
                score = val;
                best_move = move;
                break;
            }
        }
    }

    transposition_table_.add(key, score);
    return score;
}

bool Negamax::search(const CardSet &lord, const CardSet &farmer)
{
    Pattern start = {-1, Pass, CardSet()};
    int8_t res = negamax_dev(lord, farmer, start, LORD_PLAY);
    return res == 1;
}

bool Negamax::search(const CardSet &lord, const CardSet &farmer, const Pattern &last)
{
    int8_t res = negamax_dev(lord, farmer, last, LORD_PLAY);
    return res == 1;
}

size_t Negamax::nodes_searched()
{
    return nodes_searched_;
}

double Negamax::hash_hit_rate()
{
    return ((double) hash_hit_ / nodes_searched_) * 100;
}

void Negamax::reset_counter()
{
    nodes_searched_ = 0;
    hash_hit_ = 0;
    best_move = Pattern();
}

void Negamax::reset_transposition_table()
{
    transposition_table_.reset();
}

TranspositionTable::TranspositionTable()
{
    table_ = new HashItem[TRANSPOSITION_TABLE_SIZE];
}

TranspositionTable::~TranspositionTable()
{
    delete[] table_;
}

void TranspositionTable::reset()
{
    memset(table_, 0, sizeof(struct HashItem) * TRANSPOSITION_TABLE_SIZE);
}

void TranspositionTable::add(uint32_t key, int8_t score)
{
    size_t index = key & TRANSPOSITION_TABLE_SIZE_MASK;
    table_[index].key = key;
    table_[index].score = score;
}

int8_t TranspositionTable::get(uint32_t key)
{
    size_t index = key & TRANSPOSITION_TABLE_SIZE_MASK;
    if (table_[index].key == key) {
        return table_[index].score;

    } else {
        return 0;
    }
}

uint32_t TranspositionTable::gen_key(const CardSet &lord, const CardSet &farmer, const CardSet &last_move, int8_t turn)
{
    uint64_t key = 0;

    key ^= farmer.to_ullong() + 0x9e3779b9 + (key << 6) + (key >> 2);
    key ^= lord.to_ullong() + 0x9e3779b9 + (key << 6) + (key >> 2);
    key ^= last_move.to_ullong() + 0x9e3779b9 + (key << 6) + (key >> 2);
    key ^= turn + 0x9e3779b9 + (key << 6) + (key >> 2);

    //64 bit to 32 bit hash
    key = (~key) + (key << 18);
    key = key ^ (key >> 31);
    key = (key + (key << 2)) + (key << 4);
    key = key ^ (key >> 11);
    key = key + (key << 6);
    key = key ^ (key >> 22);

    return (int) key;
}
}//namespace doudizhu_endgame
