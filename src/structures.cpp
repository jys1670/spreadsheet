#include "common.h"
#include <algorithm>
#include <cctype>
#include <sstream>
#include <tuple>


const int LETTERS = 26; //* Total amount of symbols in our numeral system
const int MAX_POSITION_LENGTH = 17;
const int MAX_POS_LETTER_COUNT = 3;

const Position Position::NONE = {-1, -1};

bool Position::operator==(const Position rhs) const {
  return row == rhs.row && col == rhs.col;
}

bool Position::operator<(const Position rhs) const {
  return std::tie(row, col) < std::tie(rhs.row, rhs.col);
}

bool Position::IsValid() const {
  return row >= 0 && col >= 0 && row < MAX_ROWS && col < MAX_COLS;
}

std::string Position::ToString() const {
  if (!IsValid())
    return {};
  std::string cell;
  for (int count{col}; count >= 0; count = (count / LETTERS) - 1) {
    cell += 'A' + (count % LETTERS);
  }
  std::reverse(cell.begin(), cell.end());
  cell += std::to_string(row + 1);
  return cell;
}

Position Position::FromString(std::string_view str) {
  if (str.size() > MAX_POSITION_LENGTH)
    return Position::NONE;
  auto it = std::find_if(str.begin(), str.end(),
                         [](char ch) { return !(ch >= 'A' && ch <= 'Z'); });
  size_t split_pos = std::distance(str.begin(), it);
  auto letters = str.substr(0, split_pos),
       digits = str.substr(split_pos, str.size());

  if (letters.empty() || digits.empty() ||
      letters.size() > MAX_POS_LETTER_COUNT) {
    return Position::NONE;
  }

  for (char ch : digits) {
    if (!std::isdigit(ch))
      return Position::NONE;
  }

  int row = std::stoi(std::string{digits});
  int col = 0;
  for (char ch : letters) {
    col *= LETTERS;
    col += ch + 1 - 'A';
  }

  return {row - 1, col - 1};
}