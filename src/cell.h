#pragma once

#include "common.h"
#include "formula.h"
#include <optional>

class Impl
{
  public:
    virtual ~Impl() = default;

    using Value = std::variant<std::string, double, FormulaError>;

    virtual Value GetValue() const = 0;

    virtual std::string GetText() const = 0;

    virtual std::vector<Position> GetReferencedCells() const = 0;

    virtual void PurgeCache() = 0;
};

class EmptyImpl : public Impl
{
  public:
    Value GetValue() const override;

    std::string GetText() const override;

    std::vector<Position> GetReferencedCells() const override;

    void PurgeCache() override;
};

class TextImpl : public Impl
{
  public:
    explicit TextImpl(std::string text);

    Value GetValue() const override;

    std::string GetText() const override;

    std::vector<Position> GetReferencedCells() const override;

    void PurgeCache() override;

  private:
    std::string text_;
};

class FormulaImpl : public Impl
{
  public:
    FormulaImpl(std::string text, Position pos, SheetInterface *sheet);

    Value GetValue() const override;

    std::string GetText() const override;

    std::vector<Position> GetReferencedCells() const override;

    void PurgeCache() override;

  private:
    Position pos_;
    std::unique_ptr<FormulaInterface> formula_;
    SheetInterface *sheet_;
    mutable std::optional<Value> cache_{};
};

class Graph
{
    using CellsStorage = std::unordered_set<Position, Position::Hasher>;
    using LinkedCellsStorage = std::unordered_map<Position, CellsStorage, Position::Hasher>;
    using VertexTagger = std::unordered_map<Position, int, Position::Hasher>;

  public:
    explicit Graph(SheetInterface &sheet);

    bool UpdateCell(Position pos, const std::vector<Position> &new_referenced_cells);

  private:
    SheetInterface &sheet_;
    LinkedCellsStorage referenced_cells_;
    LinkedCellsStorage dependants_;

    bool HasCircularDependency(Position pos) const;

    void PurgeCache(Position pos);

    void CircularDepsDFS(Position pos, VertexTagger &tags, bool &is_cyclic) const;

    void PurgeCacheDFS(Position pos, VertexTagger &visited);
};

class Cell : public CellInterface
{
  public:
    Cell() = default;

    ~Cell() override = default;

    Cell &SetPosition(Position pos);

    Cell &SetSheet(SheetInterface *sheet);

    Cell &SetGraph(Graph *graph);

    void Set(std::string text) override; // cyclic graph check here

    Value GetValue() const override;

    void Clear();

    std::string GetText() const override;

    std::vector<Position> GetReferencedCells() const override;

    void PurgeCache();

  private:
    Position pos_{Position::NONE};
    SheetInterface *sheet_{nullptr};
    Graph *graph_{nullptr};
    std::unique_ptr<Impl> impl_{std::make_unique<EmptyImpl>()};
};
