//
// Created by deng on 18-11-28.
//

#ifndef DOUDIZHU_ENDGAME_CARDSET_H
#define DOUDIZHU_ENDGAME_CARDSET_H

#include <vector>
#include <bitset>
#include "utils.h"

namespace doudizhu_endgame {

HAS_MEMBER(_Find_first);
HAS_MEMBER(_Find_next);

#define BITSET_SIZE 64

class CardSet {
public:
    CardSet() = default;

    ~CardSet() = default;

    bool operator==(const CardSet &other)
    {
        return this->card_mask_ == other.card_mask_;
    }

    std::bitset<BITSET_SIZE> raw() const
    {
        return card_mask_;
    }

    size_t size() const
    {
        return card_mask_.count();
    }

    bool empty() const
    {
        return card_mask_.none();
    }

    void clear()
    {
        card_mask_.reset();
    }

    bool test(size_t pos) const
    {
        return card_mask_.test(pos);
    }

    uint64_t to_ullong() const
    {
        return card_mask_.to_ullong();
    }

    size_t find_first() const
    {
        if (has_member__Find_first<std::bitset<64>>::value) {
            return card_mask_._Find_first();

        } else {
            return my_find_first(card_mask_.to_ullong(), BITSET_SIZE);
        }
    }

    size_t find_next(size_t prev) const
    {
        if (has_member__Find_next<std::bitset<64>, size_t >::value) {
            return card_mask_._Find_next(prev);

        } else {
            return my_find_next(prev, card_mask_.to_ullong(), BITSET_SIZE);
        }
    }

    bool is_single(int8_t card) const
    {
        return card_mask_.test((size_t) card << 2);
    }

    bool is_pair(int8_t card) const
    {
        return card_mask_.test((size_t) (card << 2) + 1);
    }

    bool is_trio(int8_t card) const
    {
        return card_mask_.test((size_t) (card << 2) + 2);
    }

    bool is_bomb(int8_t card) const
    {
        return card_mask_.test((size_t) (card << 2) + 3);
    }

    void set(size_t pos)
    {
        card_mask_.set(pos);
    }

    void set(size_t pos, bool val)
    {
        card_mask_.set(pos, val);
    }

    void remove_taken(CardSet &taken)
    {
        taken.card_mask_.flip();
        card_mask_ &= taken.card_mask_;
    }

    void set_single(int8_t card)
    {
        card_mask_.set((size_t) (card << 2));
    }

    void set_pair(int8_t card)
    {
        card <<= 2;
        card_mask_.set((size_t) card);
        card_mask_.set((size_t) (card + 1));
    }

    void set_rocket();

    void set_single(int8_t card, bool val);

    void set_pair(int8_t card, bool val);

    void set_trio(int8_t card);

    void set_trio_single(int8_t trio, int8_t single);

    void set_trio_pair(int8_t trio, int8_t pair);

    void set_bomb(int8_t bomb);

    void set_straight_s(int8_t start, int8_t end);

    void set_straight_p(int8_t start, int8_t end);

    void set_plane(int8_t start, int8_t end);

    void set_plane_single(int8_t start, int8_t end, std::vector<int8_t> &comb);

    void set_plane_pair(int8_t start, int8_t end, std::vector<int8_t> &comb);

    void remove(const CardSet &hand);

    bool has_count(int8_t card, int8_t count) const;

    std::string str();

    std::string bitset_str();

    bool from_string(std::string string);

    void from_c_string(char *string);

private:
    std::bitset<BITSET_SIZE> card_mask_{};
};
}   //namespace doudizhu_endgame
#endif //DOUDIZHU_ENDGAME_CARDSET_H
