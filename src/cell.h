
#pragma once

#include "common.h"
#include "formula.h"

#include <functional>
#include <unordered_set>

class Impl
{
  public:
    using Value = std::variant<std::string, double, FormulaError>;

    virtual ~Impl() = default;

    virtual Value GetValue() const = 0;

    virtual std::string GetText() const = 0;
};

class EmptyImpl : public Impl
{
  public:
    Value GetValue() const override;

    std::string GetText() const override;
};

class TextImpl : public Impl
{
  public:
    explicit TextImpl(std::string text);

    Value GetValue() const override;

    std::string GetText() const override;

  private:
    std::string text_;
};

class FormulaImpl : public Impl
{
  public:
    FormulaImpl(std::string text);

    Value GetValue() const override;

    std::string GetText() const override;

  private:
    std::unique_ptr<FormulaInterface> formula_;
};

class Cell : public CellInterface
{
  public:
    Cell() = default;
    ~Cell() override = default;

    void Set(std::string text) override;
    void Clear();

    Cell &SetPosition(Position pos);
    Cell &SetSheet(SheetInterface *sheet);

    Value GetValue() const override;
    std::string GetText() const override;

  private:
    Position pos_{Position::NONE};
    SheetInterface *sheet_{nullptr};
    std::unique_ptr<Impl> impl_{std::make_unique<EmptyImpl>()};
};