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
    std::cout << "------------------------------------------------" << "\n";
    std::cout << "Usage: enter without spaces, not case sensitive." << "\n";
    std::cout << "e.g. ['little joker' 'big joker' 2 A K Q J 10 9]" << "\n";
    std::cout << "enter: zy2akqj09" << "\n";
    std::cout << "------------------------------------------------" << "\n";

    CardSet lord, farmer;
    while (!lord.from_string(input_stdin("enter lord hand (your hand):"))) {}
    while (!farmer.from_string(input_stdin("enter farmer hand:"))) {}
    Pattern start_pattern = {-1, Pass, CardSet()};

    std::cout << "------------------------------------------------" << "\n";
    std::cout << "lord:   [" << lord.str() << "]\n";
    std::cout << "farmer: [" << farmer.str() << "]\n";
    std::cout << "------------------------------------------------" << "\n";

    using namespace std::chrono;
    steady_clock::time_point start = steady_clock::now();
    bool win = engine_.search_multithreading(lord, farmer, start_pattern);
    steady_clock::time_point end = steady_clock::now();

    duration<double> time_span = duration_cast<duration<double>>(end - start);
    std::cout << "search_multithreading time: " << time_span.count() << " seconds.\n";
    std::cout << "------------------------------------------------" << "\n";

    if (win) {
        std::cout << "play: [" << engine_.best_move.hand.str() << "]\n";
        search_remaining_move(lord, farmer);

    } else {
        std::cout << "can not win\n";
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

void Solution::search_remaining_move(const CardSet &lord, const CardSet &farmer)
{
    CardSet lord_current = lord;
    CardSet farmer_current = farmer;
    lord_current.remove(engine_.best_move.hand);
    while (!lord_current.empty()) {
        Pattern farmer_move = get_enemy_current_hand(farmer_current, engine_.best_move);
        engine_.reset_result();
        engine_.search_multithreading(lord_current, farmer_current, farmer_move);
        lord_current.remove(engine_.best_move.hand);

        std::cout << "------------------------------------------------" << "\n";
        if (engine_.best_move.hand.empty()) {
            std::cout << "PASS\n";

        } else {
            std::cout << "play: [" << engine_.best_move.hand.str() << "]\n";
        }
        std::cout << "currt loard hand: [" << lord_current.str() << "]\n";
        std::cout << "currt farmer hand:[" << farmer_current.str() << "]\n";
    }

}

Pattern Solution::get_enemy_current_hand(CardSet &hand, const Pattern &last)
{
    while (true) {
        CardSet move;
        while (!move.from_string(input_stdin("enter farmer play:"))) {}
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
