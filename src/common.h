/**
 * @file common.h
 * @brief widely used auxiliary structures
 */
#pragma once

#include <memory>
#include <sstream>
#include <stdexcept>
#include <string>
#include <string_view>
#include <variant>

/**
 * @brief cell position, index starts from zero
 *
 */
struct Position
{
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

    struct Hasher
    {
        size_t operator()(const Position &pos) const
        {
            return pos.row + 37 * pos.col;
        }
    };
};

struct Size
{
    int rows = 0;
    int cols = 0;

    bool operator==(Size rhs) const;
};

// Описывает ошибки, которые могут возникнуть при вычислении формулы.
class FormulaError : public std::runtime_error
{
  public:
    using std::runtime_error::runtime_error;
};

std::ostream &operator<<(std::ostream &output, FormulaError fe);

// Исключение, выбрасываемое при попытке задать синтаксически некорректную
// формулу
class FormulaException : public std::runtime_error
{
  public:
    using std::runtime_error::runtime_error;
};

// Исключение, выбрасываемое при попытке передать в метод некорректную позицию
class InvalidPositionException : public std::out_of_range
{
  public:
    using std::out_of_range::out_of_range;
};

// Исключение, выбрасываемое, если вставка строк/столбцов в таблицу приведёт к
// ячейке с позицией больше максимально допустимой
class TableTooBigException : public std::runtime_error
{
  public:
    using std::runtime_error::runtime_error;
};

inline constexpr char FORMULA_SIGN = '=';
inline constexpr char ESCAPE_SIGN = '\'';

class CellInterface
{
  public:
    // Либо текст ячейки, либо значение формулы, либо сообщение об ошибке из
    // формулы
    using Value = std::variant<std::string, double, FormulaError>;

    virtual ~CellInterface() = default;

    // Задаёт содержимое ячейки. Если текст начинается со знака "=", то он
    // интерпретируется как формула. Уточнения по записи формулы:
    // * Если текст содержит только символ "=" и больше ничего, то он не считается
    // формулой
    // * Если текст начинается с символа "'" (апостроф), то при выводе значения
    // ячейки методом GetValue() он опускается. Можно использовать, если нужно
    // начать текст со знака "=", но чтобы он не интерпретировался как формула.
    virtual void Set(std::string text) = 0;

    // Возвращает видимое значение ячейки.
    // В случае текстовой ячейки это её текст (без экранирующих символов). В
    // случае формулы - числовое значение формулы или сообщение об ошибке.
    virtual Value GetValue() const = 0;
    // Возвращает внутренний текст ячейки, как если бы мы начали её
    // редактирование. В случае текстовой ячейки это её текст (возможно,
    // содержащий экранирующие символы). В случае формулы - её выражение.
    virtual std::string GetText() const = 0;
};

// Интерфейс таблицы
class SheetInterface
{
  public:
    virtual ~SheetInterface() = default;

    // Задаёт содержимое ячейки.
    // * Если текст начинается с символа "'" (апостроф), то при выводе значения
    // ячейки методом GetValue() он опускается. Можно использовать, если нужно
    // начать текст со знака "=", но чтобы он не интерпретировался как формула.
    virtual void SetCell(Position pos, std::string text) = 0;

    // Возвращает значение ячейки.
    // Если ячейка пуста, может вернуть nullptr.
    virtual const CellInterface *GetCell(Position pos) const = 0;
    virtual CellInterface *GetCell(Position pos) = 0;

    // Очищает ячейку.
    // Последующий вызов GetCell() для этой ячейки вернёт либо nullptr, либо
    // объект с пустым текстом.
    virtual void ClearCell(Position pos) = 0;

    // Вычисляет размер области, которая участвует в печати.
    // Определяется как ограничивающий прямоугольник всех ячеек с непустым
    // текстом.
    virtual Size GetPrintableSize() const = 0;

    // Выводит всю таблицу в переданный поток. Столбцы разделяются знаком
    // табуляции. После каждой строки выводится символ перевода строки. Для
    // преобразования ячеек в строку используются методы GetValue() или GetText()
    // соответственно. Пустая ячейка представляется пустой строкой в любом случае.
    virtual void PrintValues(std::ostream &output) const = 0;
    virtual void PrintTexts(std::ostream &output) const = 0;
};

// Создаёт готовую к работе пустую таблицу.
std::unique_ptr<SheetInterface> CreateSheet();