# coding=utf-8

import time
from functools import wraps
from itertools import combinations
from enum import Enum

# Transposition Table
from ctypes import Structure, Array, c_int32, c_uint32

# for multiprocessing
from multiprocessing import Process, Queue, cpu_count, freeze_support

card2val = {'3': 0, '4': 1, '5': 2, '6': 3, '7': 4, '8': 5, '9': 6, '0': 7, 'J': 8, 'Q': 9, 'K': 10, 'A': 11, '2': 12,
            'Y': 13, 'Z': 14}
val2card = ['3', '4', '5', '6', '7', '8', '9', '10', 'J', 'Q', 'K', 'A', '2', 'Black Joker', 'Red Joker']


class HashItem(Structure):
    _fields_ = [('key', c_uint32), ('score', c_int32)]


class HashTable(Array):
    _length_ = 1024 * 1024
    _type_ = HashItem


class Pattern(Enum):
    PASS = 0
    SINGLE = 1
    PAIR = 2
    TRIPLE = 3
    BOMB = 4
    TRIPLE_SINGLE = 5
    TRIPLE_PAIR = 6
    BOMB_SINGLE = 7
    BOMB_PAIR = 8
    STRAIGHT = 9
    STRAIGHT_PAIR = 10
    PLANE = 11
    PLANE_SINGLE = 12
    PLANE_PAIR = 13
    ROCKET = 14


FARMER_PLAY = 1
LORD_PLAY = 0
TRIPLE = True
TABLE_SIZE_MASK = 1048575  # 1024 * 1024 -1


def transposition_table_get(table, key):
    index = key & TABLE_SIZE_MASK
    if table[index].key == key:
        return table[index].score
    return 0


def transposition_table_add(table, key, score):
    index = key & TABLE_SIZE_MASK
    table[index].key = key
    table[index].score = score


def func_time(func):
    @wraps(func)
    def func_timer(*args, **kwargs):
        print('{name} start'.format(name=func.__name__))
        start = time.time()
        result = func(*args, **kwargs)
        end = time.time()
        print('search finished, use: {time:.6f}s'.format(time=end - start))
        return result

    return func_timer


def create_straight(card_dict, min_len, size, last_power):
    card_for_straight = [x for x in card_dict if card_dict[x] >= size and x > last_power and x < 12]
    if len(card_for_straight) < min_len:
        return []

    straights = []
    start = 0
    while start < len(card_for_straight):
        end = start
        while end < len(card_for_straight) and card_for_straight[end] - card_for_straight[start] is end - start:
            if end - start >= min_len - 1:
                straights.append([card_for_straight[x] for x in range(start, end + 1)])
            end += 1
        start += 1

    return straights


def get_next_move(card_dict, last):
    last_type = last['type']
    last_power = last['power']
    moves = []
    append = moves.append
    if 13 in card_dict and 14 in card_dict:
        append({'cards': [13, 14], 'type': Pattern.ROCKET, 'power': 99})

    if TRIPLE and last_type != Pattern.ROCKET:
        for card in card_dict:
            if last_type == Pattern.BOMB and card_dict[card] == 4 and card > last_power:
                append({'cards': [card] * 4, 'type': Pattern.BOMB, 'power': card})
            if last_type != Pattern.BOMB and card_dict[card] == 4:
                append({'cards': [card] * 4, 'type': Pattern.BOMB, 'power': card})

    if last_type is Pattern.PASS or last_type is Pattern.PAIR:
        for card in card_dict:
            if card_dict[card] >= 2 and card > last_power:
                append({'cards': [card] * 2, 'type': Pattern.PAIR, 'power': card})

    if last_type is Pattern.PASS or last_type is Pattern.SINGLE:
        for card in card_dict:
            if card > last_power:
                append({'cards': [card], 'type': Pattern.SINGLE, 'power': card})

    if TRIPLE and last_type is Pattern.PASS or last_type is Pattern.TRIPLE:
        for card in card_dict:
            if card_dict[card] >= 3 and card > last_power:
                append({'cards': [card] * 3, 'type': Pattern.TRIPLE, 'power': card})

    if TRIPLE and last_type is Pattern.PASS or last_type is Pattern.TRIPLE_SINGLE:
        for card in card_dict:
            if card_dict[card] >= 3 and card > last_power:
                for single in card_dict:
                    if card != single:
                        append({'cards': [card] * 3 + [single], 'type': Pattern.TRIPLE_SINGLE, 'power': card})

    if TRIPLE and last_type is Pattern.PASS or last_type is Pattern.TRIPLE_PAIR:
        for card in card_dict:
            if card_dict[card] >= 3 and card > last_power:
                for pair in card_dict:
                    if card_dict[pair] >= 2 and pair != card:
                        append({'cards': [card] * 3 + [pair] * 2, 'type': Pattern.TRIPLE_PAIR, 'power': card})

    if last_type is Pattern.PASS:
        straights = create_straight(card_dict, 5, 1, last_power)
        for straight in straights:
            append({'cards': straight, 'type': Pattern.STRAIGHT, 'power': straight[0]})
    if last_type is Pattern.STRAIGHT:
        straights = create_straight(card_dict, len(last['cards']), 1, last_power)
        for straight in straights:
            append({'cards': straight, 'type': Pattern.STRAIGHT, 'power': straight[0]})

    if last_type is Pattern.PASS:
        straights = create_straight(card_dict, 3, 2, last_power)
        for straight in straights:
            append({'cards': straight * 2, 'type': Pattern.STRAIGHT_PAIR, 'power': straight[0]})
    if last_type is Pattern.STRAIGHT_PAIR:
        straights = create_straight(card_dict, len(last['cards']) // 2, 2, last_power)
        for straight in straights:
            append({'cards': straight * 2, 'type': Pattern.STRAIGHT_PAIR, 'power': straight[0]})

    if TRIPLE and last_type is Pattern.PASS:
        straights = create_straight(card_dict, 2, 3, last_power)
        for straight in straights:
            append({'cards': straight * 3, 'type': Pattern.PLANE, 'power': straight[0]})
    if TRIPLE and last_type is Pattern.PLANE:
        straights = create_straight(card_dict, len(last['cards']) // 3, 3, last_power)
        for straight in straights:
            append({'cards': straight * 3, 'type': Pattern.PLANE, 'power': straight[0]})

    if TRIPLE and last_type is Pattern.PASS:
        straights = create_straight(card_dict, 2, 3, last_power)
        for straight in straights:
            card_for_comb = set(card_dict) - set(straight)
            for comb in list(combinations(card_for_comb, len(straight))):
                append({'cards': straight * 3 + list(comb), 'type': Pattern.PLANE_SINGLE, 'power': straight[0]})
    if TRIPLE and last_type is Pattern.PLANE_SINGLE:
        straights = create_straight(card_dict, len(last['cards']) // 4, 3, last_power)
        for straight in straights:
            card_for_comb = set(card_dict) - set(straight)
            for comb in list(combinations(card_for_comb, len(straight))):
                append({'cards': straight * 3 + list(comb), 'type': Pattern.PLANE_SINGLE, 'power': straight[0]})

    if TRIPLE and last_type is Pattern.PASS:
        straights = create_straight(card_dict, 2, 3, last_power)
        for straight in straights:
            card_for_comb = set(x for x in card_dict if card_dict[x] >= 2) - set(straight)
            for comb in list(combinations(card_for_comb, len(straight))):
                append({'cards': straight * 3 + list(comb) * 2, 'type': Pattern.PLANE_PAIR, 'power': straight[0]})
    if TRIPLE and last_type is Pattern.PLANE_PAIR:
        straights = create_straight(card_dict, len(last['cards']) // 5, 3, last_power)
        for straight in straights:
            card_for_comb = set(x for x in card_dict if card_dict[x] >= 2) - set(straight)
            for comb in list(combinations(card_for_comb, len(straight))):
                append({'cards': straight * 3 + list(comb) * 2, 'type': Pattern.PLANE_PAIR, 'power': straight[0]})

    if TRIPLE and last_type is Pattern.PASS or last_type is Pattern.BOMB_SINGLE:
        for card in card_dict:
            if card_dict[card] >= 4 and card > last_power:
                card_for_comb = set(card_dict) - set([card])
                for comb in list(combinations(card_for_comb, 2)):
                    append({'cards': [card] * 4 + list(comb), 'type': Pattern.BOMB_SINGLE, 'power': card})

                pair = set(x for x in card_dict if card_dict[x] >= 2) - set([card])
                for p in list(pair):
                    append({'cards': [card] * 4 + [p] * 2, 'type': Pattern.BOMB_SINGLE, 'power': card})

    if TRIPLE and last_type is Pattern.PASS or last_type is Pattern.BOMB_PAIR:
        for card in card_dict:
            if card_dict[card] >= 4 and card > last_power:
                card_for_comb = set(x for x in card_dict if card_dict[x] >= 2) - set([card])
                for comb in list(combinations(card_for_comb, 2)):
                    append({'cards': [card] * 4 + list(comb) * 2, 'type': Pattern.BOMB_PAIR, 'power': card})

    if last_type != Pattern.PASS:
        append({'cards': [], 'type': Pattern.PASS, 'power': -1})

    return moves


def play(hand, move):
    re_ = hand.copy()
    for card in move:
        re_[card] -= 1
    del_l = [x for x in re_ if re_[x] == 0]
    for i in del_l:
        del re_[i]
    return re_


def negama(tt, lord, farmer, last, turn):
    if not lord or not farmer:
        return -1

    key = c_uint32(hash(str(lord) + str(farmer) + str(last['cards']) + str(turn))).value
    s = transposition_table_get(tt, key)
    if s is not 0:
        return s

    score = -1
    if turn is FARMER_PLAY:
        next_move = get_next_move(farmer, last)
        for move in next_move:
            after = play(farmer, move['cards'])
            val = -negama(tt, lord, after, move, LORD_PLAY)
            if val > 0:
                score = val
                break
    else:
        next_move = get_next_move(lord, last)
        for move in next_move:
            after = play(lord, move['cards'])
            val = -negama(tt, after, farmer, move, FARMER_PLAY)
            if val > 0:
                score = val
                break
    transposition_table_add(tt, key, score)
    return score


def worker(input, output):
    tt = HashTable()
    for args in iter(input.get, 'STOP'):
        score = -negama(tt, *args)
        output.put((score, args[2]))


# @func_time
def search_multi(lord, farmer, last):
    PROCESSES = cpu_count() // 2
    task_queue = Queue()
    done_queue = Queue()
    lord_move = get_next_move(lord, last)
    after = (play(lord, move['cards']) for move in lord_move)
    task = [(lord_, farmer, move, FARMER_PLAY) for lord_, move in zip(after, lord_move)]
    for i in task:
        task_queue.put(i)
    for i in range(PROCESSES):
        task_queue.put('STOP')

    # start worker processes
    for i in range(PROCESSES):
        Process(target=worker, args=(task_queue, done_queue)).start()

    for i in range(len(task)):
        score, move = done_queue.get()
        if score > 0:
            return True, move

    return False, {}


# @func_time
def search(lord, farmer, last):
    tt = HashTable()
    lord_move = get_next_move(lord, last)
    for move in lord_move:
        after = play(lord, move['cards'])
        val = -negama(tt, after, farmer, move, FARMER_PLAY)
        if val > 0:
            return True, move

    return False, {}


def val_from_cli(prompt=''):
    def check(move):
        card_str = input(prompt + '\n').upper()
        for card in card_str:
            if card in card2val:
                move.append(card2val[card])
            else:
                print('invalid card: {card}'.format(card=card))
                move.clear()
                return False
        return True

    re_list = []
    while not check(re_list):
        pass
    re_list.sort()
    return re_list


def val2srt(hand):
    hand_ = []
    if isinstance(hand, list):
        hand_.extend(hand)
    else:
        for i in hand:
            hand_.extend([i] * hand[i])

    hand_.sort()
    return str([val2card[val] for val in hand_])


def get_farmer_play(farmer_dict, last):
    while 1:
        move = val_from_cli('input farmer play:')
        next_ = get_next_move(farmer_dict, last)
        for i in next_:
            move_ = i['cards'].copy()
            move_.sort()
            if move == move_:
                return i
        print('invalid play: {move}'.format(move=val2srt(move)))


def check_pattern(lord, farmer):
    global TRIPLE
    l_check = [x for x in lord if lord[x] > 2]
    f_check = [x for x in farmer if farmer[x] > 2]
    if not l_check and not f_check:
        TRIPLE = False


def get_card_dict(hand):
    card_dict = {}
    for card in set(hand):
        card_dict[card] = hand.count(card)
    return card_dict


def run():
    print('----------------------------------------')
    lord_ = val_from_cli('lord hand:')
    farmer_ = val_from_cli('farmer hand:')
    lord_dict = get_card_dict(lord_)
    farmer_dict = get_card_dict(farmer_)
    check_pattern(lord_dict, farmer_dict)
    start = {'cards': [], 'type': Pattern.PASS, 'power': -1}
    time_start = time.time()
    win, best_move = search_multi(lord_dict, farmer_dict, start)
    time_end = time.time()
    print('----------------------------------------')
    print('lord:   ' + val2srt(lord_))
    print('farmer: ' + val2srt(farmer_))
    print('search finished, use: {time:.6f}s'.format(time=time_end - time_start))
    print('----------------------------------------')
    if win:
        print('play ' + val2srt(best_move['cards']))
        lord_current = play(lord_dict, best_move['cards'])
        farmer_current = farmer_dict.copy()
        print('current lord:   ' + val2srt(lord_current))
        print('current farmer: ' + val2srt(farmer_current))

        while lord_current:
            farmer_move = get_farmer_play(farmer_current, best_move)
            farmer_current = play(farmer_current, farmer_move['cards'])
            win, best_move = search(lord_current, farmer_current, farmer_move)
            lord_current = play(lord_current, best_move['cards'])

            print('----------------------------------------')
            if best_move['type'] is Pattern.PASS:
                print('PASS')
            else:
                print('play ' + val2srt(best_move['cards']))
            print('current lord:   ' + val2srt(lord_current))
            print('current farmer: ' + val2srt(farmer_current))
    else:
        print('can not win')


if __name__ == '__main__':
    # freeze_support()
    run()
