#include "cell.h"

#include <cassert>
#include <string>
#include <utility>

Graph::Graph(SheetInterface &sheet) : sheet_(sheet)
{
}

bool Graph::UpdateCell(Position pos, const std::vector<Position> &new_referenced_cells)
{

    CellsStorage old_referenced_cells = referenced_cells_[pos];
    referenced_cells_[pos] = CellsStorage{new_referenced_cells.begin(), new_referenced_cells.end()};

    if (HasCircularDependency(pos))
    {
        referenced_cells_[pos] = old_referenced_cells;
        return false;
    }

    for (const auto &cell : old_referenced_cells)
        dependants_[cell].erase(pos);
    for (const auto &cell : new_referenced_cells)
        dependants_[cell].insert(pos);

    PurgeCache(pos);
    return true;
}

bool Graph::HasCircularDependency(Position pos) const
{
    bool is_cyclic_graph{false};
    VertexTagger tags{};
    CircularDepsDFS(pos, tags, is_cyclic_graph);
    return is_cyclic_graph;
}

void Graph::CircularDepsDFS(Position pos, VertexTagger &tags, bool &is_cyclic) const
{
    enum
    {
        VISITED,
        FINISHED
    };
    if (!referenced_cells_.count(pos))
    { // no referenced cells, this cell is finished
        tags[pos] = FINISHED;
        return;
    }
    tags[pos] = VISITED;
    for (const auto &cell : referenced_cells_.at(pos))
    {
        if (tags.count(cell))
        { // if cell tag exists
            // if cell has VISITED status -> cycle found
            if (tags[cell] == VISITED)
                is_cyclic = true;
        }
        else
        {
            // cell tag does not exist, visit cell
            CircularDepsDFS(cell, tags, is_cyclic);
        }
        if (is_cyclic)
            return;
    }
    tags[pos] = FINISHED;
}

void Graph::PurgeCache(Position pos)
{
    VertexTagger visited;
    PurgeCacheDFS(pos, visited);
}

void Graph::PurgeCacheDFS(Position pos, VertexTagger &visited)
{
    visited[pos];
    if (!dependants_.count(pos))
        return; // means nothing depends on this pos
    for (const auto &cell : dependants_.at(pos))
    {
        if (!visited.count(cell))
        {
            static_cast<Cell *>(sheet_.GetCell(cell))->PurgeCache();
            PurgeCacheDFS(cell, visited);
        }
    }
}

Impl::Value EmptyImpl::GetValue() const
{
    return std::string{};
}

std::string EmptyImpl::GetText() const
{
    return {};
}

std::vector<Position> EmptyImpl::GetReferencedCells() const
{
    return {};
}

void EmptyImpl::PurgeCache()
{
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

std::vector<Position> TextImpl::GetReferencedCells() const
{
    return {};
}

void TextImpl::PurgeCache()
{
}

FormulaImpl::FormulaImpl(std::string text, Position pos, SheetInterface *sheet)
    : pos_(pos), formula_(ParseFormula(std::move(text))), sheet_(sheet)
{
    assert(sheet);
}

Impl::Value FormulaImpl::GetValue() const
{
    if (cache_)
        return *cache_;
    auto value = formula_->Evaluate(*sheet_);
    if (std::holds_alternative<double>(value))
    {
        cache_ = std::optional(Value(std::get<double>(value)));
    }
    else
    {
        cache_ = std::optional(Value(std::get<FormulaError>(value)));
    }
    return *cache_;
}

std::string FormulaImpl::GetText() const
{
    return "=" + formula_->GetExpression();
}

std::vector<Position> FormulaImpl::GetReferencedCells() const
{
    return formula_->GetReferencedCells();
}

void FormulaImpl::PurgeCache()
{
    cache_.reset();
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
        auto tmp = std::make_unique<FormulaImpl>(text.substr(1), pos_, sheet_);
        if (!graph_->UpdateCell(pos_, tmp->GetReferencedCells()))
        {
            throw CircularDependencyException("Circular dependency detected");
        }
        impl_ = std::move(tmp);
    }
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

Cell &Cell::SetGraph(Graph *graph)
{
    graph_ = graph;
    return *this;
}

void Cell::PurgeCache()
{
    impl_->PurgeCache();
}

void Cell::Clear()
{
    graph_->UpdateCell(pos_, {});
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

std::vector<Position> Cell::GetReferencedCells() const
{
    return impl_->GetReferencedCells();
}
