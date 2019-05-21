//
// Created by deng on 18-11-28.
//

#include <iostream>
#include <chrono>
#include <cstring>

#include "solution.h"

namespace doudizhu_endgame {

void Solution::start()
{
    std::cout << "----------------------------------------" << "\n";
    std::cout << "输入不带空格, 10(0零)大王(z)小王(y)代替" << "\n";
    std::cout << "如：[大王 小王 2 A 10 9 9 9 4]" << "\n";
    std::cout << "输入：zy2a09994" << "\n";
    std::cout << "----------------------------------------" << "\n";

    CardSet lord, farmer;
    while (!lord.from_string(input_stdin("输入我方手牌:"))) {}
    while (!farmer.from_string(input_stdin("输入对手手牌:"))) {}

    std::cout << "----------------------------------------" << "\n";
    std::cout << "我方手牌: [" << lord.str() << "]\n";
    std::cout << "对手手牌: [" << farmer.str() << "]\n";
    std::cout << "----------------------------------------" << "\n";

    using namespace std::chrono;
    steady_clock::time_point start = steady_clock::now();
    bool win = engine_.search(lord, farmer);
    steady_clock::time_point end = steady_clock::now();

    duration<double> time_span = duration_cast<duration<double>>(end - start);
    std::cout << "nodes calculated: " << engine_.nodes_searched() << "\n";
    std::cout << "search time: " << time_span.count() << " seconds.\n";
    std::cout << "transposition table hit rate: " << engine_.hash_hit_rate() << "%\n";
    std::cout << "----------------------------------------" << "\n";

    if (win) {
        std::cout << "出:[" << engine_.best_move.hand.str() << "]\n";
        search_remaining_move(lord, farmer, engine_.best_move);

    } else {
        std::cout << "无法取胜\n";
    }
}

std::string Solution::input_stdin(const char *prompt)
{
    std::cout << prompt << "\n";
    char buff[50];
    std::cin.getline(buff, 50);
    std::string string(buff);
    if (string.length() == 0) {
        return std::string("\n");
    }
    return string;
}

void Solution::search_remaining_move(const CardSet &lord, const CardSet &farmer, const Pattern &move)
{
    CardSet lord_current = lord;
    CardSet farmer_current = farmer;
    lord_current.remove(move.hand);
    while (!lord_current.empty()) {
        Pattern farmer_move = get_enemy_current_hand(farmer_current, move);
        engine_.reset_counter();
        engine_.reset_transposition_table();
        engine_.search(lord_current, farmer_current, farmer_move);
        lord_current.remove(engine_.best_move.hand);

        std::cout << "----------------------------------------" << "\n";
        if (engine_.best_move.hand.empty()) {
            std::cout << "出: Pass\n";

        } else {
            std::cout << "出: [" << engine_.best_move.hand.str() << "]\n";
        }
        std::cout << "currt loard hand: [" << lord_current.str() << "]\n";
        std::cout << "currt farmer hand:[" << farmer_current.str() << "]\n";
    }

}

Pattern Solution::get_enemy_current_hand(CardSet &hand, const Pattern &last)
{
    while (true) {
        CardSet move;
        while (!move.from_string(input_stdin("输入对方出牌:"))) {}
        std::vector<Pattern> selections;
        DouDiZhuHand::next_hand(hand, last, selections);
        for (Pattern &i : selections) {
            if (i.hand == move) {
                hand.remove(move);
                return i;
            }
        }
        std::cout << "invalid hand: [" << move.str() << "]\n";
    }
}
} //namespace doudizhu_endgame
