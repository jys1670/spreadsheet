#include "sheet.h"

#include "common.h"

#include <functional>
#include <iostream>

using namespace std::literals;

Sheet::Sheet() : table_{}, size_{0, 0}, graph_(*this)
{
}

Sheet::~Sheet() = default;

void Sheet::SetCell(Position pos, std::string text)
{
    CheckCorrectness(pos);
    if (table_.count(pos) && table_.at(pos).GetText() == text)
        return;

    table_[pos].SetPosition(pos).SetSheet(this).SetGraph(&graph_).Set(text);
    for (const auto &cell : table_[pos].GetReferencedCells())
    {
        if (!table_.count(cell))
        {
            table_[cell].Set(std::string{});
            if (cell.row >= size_.rows)
                size_.rows = cell.row + 1;
            if (cell.col >= size_.cols)
                size_.cols = cell.col + 1;
        }
    }

    if (pos.row >= size_.rows)
        size_.rows = pos.row + 1;
    if (pos.col >= size_.cols)
        size_.cols = pos.col + 1;
}

const CellInterface *Sheet::GetCell(Position pos) const
{
    CheckCorrectness(pos);
    if (table_.count(pos))
    {
        return &table_.at(pos);
    }
    return nullptr;
}

CellInterface *Sheet::GetCell(Position pos)
{
    CheckCorrectness(pos);
    if (table_.count(pos))
    {
        return &table_.at(pos);
    }
    return nullptr;
}

void Sheet::ClearCell(Position pos)
{
    CheckCorrectness(pos);
    if (!table_.count(pos))
        return;
    table_[pos].Clear();
    table_.erase(pos);

    int max_col{-1}, max_row{-1};
    for (const auto &[p, _] : table_)
    {
        if (table_.at(p).GetText().empty())
            continue;
        max_row = std::max(p.row, max_row);
        max_col = std::max(p.col, max_col);
    }
    size_ = Size{max_row + 1, max_col + 1};
}

Size Sheet::GetPrintableSize() const
{
    return size_;
}

void Sheet::PrintValues(std::ostream &output) const
{
    for (int i = 0; i < size_.rows; ++i)
    {
        for (int k = 0; k < size_.cols; ++k)
        {
            if (table_.count({i, k}))
            {
                output << table_.at({i, k}).GetValue();
            }
            if (k != size_.cols - 1)
            {
                output << "\t";
            }
        }
        output << "\n";
    }
}

void Sheet::PrintTexts(std::ostream &output) const
{
    for (int i = 0; i < size_.rows; ++i)
    {
        for (int k = 0; k < size_.cols; ++k)
        {
            if (table_.count({i, k}))
            {
                output << table_.at({i, k}).GetText();
            }
            if (k != size_.cols - 1)
            {
                output << "\t";
            }
        }
        output << "\n";
    }
}

void Sheet::CheckCorrectness(const Position &pos)
{
    if (!pos.IsValid())
    {
        throw InvalidPositionException("Invalid position " + pos.ToString());
    }
}

std::unique_ptr<SheetInterface> CreateSheet()
{
    return std::make_unique<Sheet>();
}

std::ostream &operator<<(std::ostream &output, const CellInterface::Value &value)
{
    std::visit([&](const auto &x) { output << x; }, value);
    return output;
}
