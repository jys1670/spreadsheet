#pragma once

#include "cell.h"
#include "common.h"

#include <functional>

class Sheet : public SheetInterface
{
    using Table = std::unordered_map<Position, Cell, Position::Hasher>;

  public:
    Sheet();
    ~Sheet();

    void SetCell(Position pos, std::string text) override;

    const CellInterface *GetCell(Position pos) const override;
    CellInterface *GetCell(Position pos) override;

    void ClearCell(Position pos) override;

    Size GetPrintableSize() const override;

    void PrintValues(std::ostream &output) const override;
    void PrintTexts(std::ostream &output) const override;

    // Можете дополнить ваш класс нужными полями и методами

  private:
    Table table_;
    Size size_;

    static void CheckCorrectness(const Position &pos);
};

std::ostream &operator<<(std::ostream &output, const CellInterface::Value &value);