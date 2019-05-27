//
// Created by deng on 18-11-28.
//

#ifndef DOUDIZHU_ENDGAME_SOLUTION_H
#define DOUDIZHU_ENDGAME_SOLUTION_H

#include "negamax.h"

namespace doudizhu_endgame {

class Solution {
public:
    Solution() = default;

    ~Solution() = default;

    void start();

private:
    Negamax engine_;

    void search_remaining_move(const CardSet &lord, const CardSet &farmer);

    std::string input_stdin(const char *prompt);

    Pattern get_enemy_current_hand(CardSet &current, const Pattern &last);
};
}   //namespace doudizhu_endgame
#endif //DOUDIZHU_ENDGAME_SOLUTION_H
