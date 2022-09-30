/**
 * @file common.h
 * @brief widely used auxiliary structures
 */
#pragma once

#include <sstream>
#include <string>
#include <string_view>

/**
 * @brief cell position, index starts from zero
 * 
 */
struct Position {
    int row = 0;
    int col = 0;

    bool operator==(Position rhs) const;
    bool operator<(Position rhs) const;

    /**
     * @brief checks if position has valid index
     * 
     */
    bool IsValid() const;

    /**
     * @brief converts Position to string, opposite of FromString(std::string_view str)
     * 
     */
    std::string ToString() const;

    /**
     * @brief converts text into Position
     * 
     * (0,0) -> A1, (0,1) -> B1, (1,1) -> A2, etc
     * index: 0 1 2 3 4 ...
     * nums:  1 2 3 4 5 ...
     * cols:  A B C D E ...
     * Algo example:
     * 800 = 1 * 26^2 + 4 * 26^1 + 20 * 26^0            numerical
     * 800 = (1-1) * 26^2 + (4-1) * 26^1 + 20 * 26^0    index
     * 800 = ADU                                        column
     * 
     * @param str std:string in format like "A1", "ADU123", etc
     * @return Position 
     */
    static Position FromString(std::string_view str);

    static const int MAX_ROWS = 16384;
    static const int MAX_COLS = 16384;
    static const Position NONE;
};