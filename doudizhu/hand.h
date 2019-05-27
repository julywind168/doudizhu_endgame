//
// Created by deng on 19-5-17.
//

#ifndef DOUDIZHU_ENDGAME_HAND_H
#define DOUDIZHU_ENDGAME_HAND_H

#include "cardset.h"

namespace doudizhu_endgame {

enum Type {
    Pass,               //pass
    Single,             //3,4,5...
    Pair,               //33 44 55..
    Triple,             //333
    Bomb,               //3333
    Triple_single,      //3334
    Triple_pair,        //33345
    Bomb_single,        //333345
    Bomb_pair,          //33334455
    Straight_single,    //34567
    Straight_pair,      //334455
    Plane,              //333444
    Plane_single,       //33344456
    Plane_pair,         //3334445566
    Rocket              //black joker red joker
};

struct Pattern {
    int8_t power{};
    Type type{};
    CardSet hand{};

    Pattern(int8_t power, Type type, CardSet hand) : power(power), type(type), hand(hand)
    {}

    Pattern() = default;

    ~Pattern() = default;
};

class DouDiZhuHand {
public:

    DouDiZhuHand() = default;

    ~DouDiZhuHand() = default;

    static void play(const CardSet &hand, const CardSet &toplay, CardSet &res);

    static void next_hand(const CardSet &hand, const Pattern &last, std::vector<Pattern> &next_moves);

private:

    static void combination(CardSet hand, CardSet used, size_t comb_len, int8_t size,
                            std::vector<std::vector<int8_t>> &combinations);

    static void create_straight(const CardSet &hand, int8_t min_len, int8_t size,
                                std::vector<std::vector<int8_t >> &straights);

    static void get_rocket(const CardSet &hand, std::vector<Pattern> &next_moves);

    static void get_pass(const CardSet &hand, const Pattern &last, std::vector<Pattern> &next_moves);

    static void get_single(const CardSet &hand, const Pattern &last, std::vector<Pattern> &next_moves);

    static void get_pair(const CardSet &hand, const Pattern &last, std::vector<Pattern> &next_moves);

    static void get_triple(const CardSet &hand, const Pattern &last, std::vector<Pattern> &next_moves);

    static void get_triple_single(const CardSet &hand, const Pattern &last, std::vector<Pattern> &next_moves);

    static void get_triple_pair(const CardSet &hand, const Pattern &last, std::vector<Pattern> &next_moves);

    static void get_bomb(const CardSet &hand, const Pattern &last, std::vector<Pattern> &next_moves);

    static void get_bomb_single(const CardSet &hand, const Pattern &last, std::vector<Pattern> &next_moves);

    static void get_bomb_pair(const CardSet &hand, const Pattern &last, std::vector<Pattern> &next_moves);

    static void get_straight_single(const CardSet &hand, const Pattern &last, std::vector<Pattern> &next_moves);

    static void get_straight_pair(const CardSet &hand, const Pattern &last, std::vector<Pattern> &next_moves);

    static void get_plane(const CardSet &hand, const Pattern &last, std::vector<Pattern> &next_moves);

    static void get_plane_single(const CardSet &hand, const Pattern &last, std::vector<Pattern> &next_moves);

    static void get_plane_pair(const CardSet &hand, const Pattern &last, std::vector<Pattern> &next_moves);
};

}   //namespace doudizhu_endgame

#endif //DOUDIZHU_ENDGAME_HAND_H
