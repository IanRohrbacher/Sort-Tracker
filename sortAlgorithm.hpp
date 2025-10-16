#ifndef SORTALGORITHM_HPP
#define SORTALGORITHM_HPP

#include <algorithm>
#include <random>

// some method that modifys a deque of ints
void sortTick(std::deque<int>& list) {
    // bogo sort :)
    std::random_device rd;
    std::mt19937 rng(rd());
    std::shuffle(std::begin(list), std::end(list), rng);
}

#endif