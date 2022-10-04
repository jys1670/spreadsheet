#include "cell.h"

#include <cassert>
#include <string>
#include <utility>

Impl::Value EmptyImpl::GetValue() const
{
    return std::string{};
}

std::string EmptyImpl::GetText() const
{
    return {};
}

TextImpl::TextImpl(std::string text) : text_(std::move(text))
{
}

Impl::Value TextImpl::GetValue() const
{
    if (!text_.empty() && text_.front() == '\'')
    {
        return text_.substr(1);
    }
    return text_;
}

std::string TextImpl::GetText() const
{
    return text_;
}

FormulaImpl::FormulaImpl(std::string text) : formula_(ParseFormula(std::move(text)))
{
}

Impl::Value FormulaImpl::GetValue() const
{
    auto value = formula_->Evaluate();
    if (std::holds_alternative<double>(value))
    {
        return Value(std::get<double>(value));
    }

    return Value(std::get<FormulaError>(value));
}

std::string FormulaImpl::GetText() const
{
    return "=" + formula_->GetExpression();
}

void Cell::Set(std::string text)
{

    if (text.empty())
    {
        impl_ = std::make_unique<EmptyImpl>();
    }
    else if (text.front() != FORMULA_SIGN || text.size() == 1)
    { // '=' is not formula
        impl_ = std::make_unique<TextImpl>(std::move(text));
    }
    else
    {
        auto tmp = std::make_unique<FormulaImpl>(text.substr(1));
        impl_ = std::move(tmp);
    }
}

void Cell::Clear()
{
    impl_ = std::make_unique<EmptyImpl>();
}

Cell::Value Cell::GetValue() const
{
    return impl_->GetValue();
}

std::string Cell::GetText() const
{
    return impl_->GetText();
}

Cell &Cell::SetPosition(Position pos)
{
    pos_ = pos;
    return *this;
}

Cell &Cell::SetSheet(SheetInterface *sheet)
{
    sheet_ = sheet;
    return *this;
}