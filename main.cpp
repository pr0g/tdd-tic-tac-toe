#include "gtest/gtest.h"
#include "gmock/gmock.h"

#include <memory>
#include <algorithm>

using namespace testing;

int main(int argc, char **argv)
{
    InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}

class TicTacToe
{
public:
    enum class Cell
    {
        Empty,
        Nought,
        Cross,
    };

    enum class Symbol
    {
        Cross,
        Nought
    };

    enum class State
    {
        Playing,
        NoughtWins,
        CrossWins,
        Draw
    };

    TicTacToe() = default;
    TicTacToe(const TicTacToe&) = default;
    TicTacToe& operator=(const TicTacToe&) = default;
    TicTacToe(TicTacToe&&) = default;
    TicTacToe& operator=(TicTacToe&&) = default;
    ~TicTacToe() = default;

    static int64_t Dimension() { return 3; }
    static int64_t CellCount() { return Dimension() * Dimension(); }

    static const char* CellGlyph(Cell cell)
    {
        switch(cell)
        {
        case Cell::Cross:
            return "[x]";
        case Cell::Nought:
            return "[o]";
        case Cell::Empty:
            return "[-]";
        default:
            assert(false);
            return "";
        }
    }

    static Cell FromSymbol(Symbol symbol)
    {
        return symbol == Symbol::Cross
            ? Cell::Cross
            : Cell::Nought;
    }

    Cell GetCell(int64_t r, int64_t c) const
    {
        return m_cells[rc(r, c)];
    }

    int64_t MovesRemaining() const
    {
        return std::count_if(
            std::begin(m_cells), std::end(m_cells), [](const Cell cell){
                return cell == Cell::Empty;
            });
    }

    bool SetCell(int64_t r, int64_t c, Symbol symbol)
    {
        if (r < 0 || r > 2 || c < 0 || c > 2)
        {
            return false;
        }

        bool wasSet = false;
        if (GetCell(r, c) == Cell::Empty)
        {
            m_cells[rc(r, c)] = FromSymbol(symbol);
            wasSet = true;
        }

        if (wasSet)
        {
            m_state = CheckBoard();

            if (!GameOver())
            {
                m_currentSymbol = NextSymbol();
            }
        }

        return wasSet;
    }

    bool CompleteLine(
        Symbol symbol, int64_t colStride, int64_t rowStride) const
    {
        const Cell cell = FromSymbol(symbol);
        for (int64_t r = 0; r < Dimension(); ++r)
        {
            int64_t count = 0;
            for (int64_t c = 0; c < Dimension(); ++c)
            {
                count = GetCell(c * colStride + r * rowStride) == cell
                    ? count + 1
                    : count;
            }

            if (count == Dimension())
            {
                return true;
            }
        }

        return false;
    }

    bool CompleteRow(Symbol symbol) const
    {
        return CompleteLine(symbol, 1, Dimension());
    }

    bool CompleteColumn(Symbol symbol) const
    {
        return CompleteLine(symbol, Dimension(), 1);
    }

    bool CompleteDiagonal(Symbol symbol) const
    {
        const Cell cell = FromSymbol(symbol);

        if (    CompleteDiagonalInternal(symbol, 0, 1)
            ||  CompleteDiagonalInternal(symbol, 2, -1))
        {
            return true;
        }

        return false;
    }

    bool HasWon(Symbol symbol) const
    {
        if (    CompleteRow(symbol)
            ||  CompleteColumn(symbol)
            ||  CompleteDiagonal(symbol))
        {
            return true;
        }

        return false;
    }

    State CheckBoard() const
    {
        if (HasWon(Symbol::Cross))
        {
            return State::CrossWins;
        }

        if (HasWon(Symbol::Nought))
        {
            return State::NoughtWins;
        }

        if (MovesRemaining() == 0)
        {
            return State::Draw;
        }

        return State::Playing;
    }

    bool GameOver() const
    {
        return m_state != State::Playing;
    }

    State CurrentState() const
    {
        return m_state;
    }

    void Reset()
    {
        memset(m_cells, 0, sizeof(Cell) * CellCount());
        m_state = State::Playing;
        m_currentSymbol = Symbol::Cross;
    }

    void DrawBoard()
    {
        for (int64_t r = 0; r < Dimension(); ++r)
        {
            for (int64_t c = 0; c < Dimension(); ++c)
            {
                printf("%s", CellGlyph(GetCell(r, c)));
            }

            printf("%s", "\n");
        }
    }

    Symbol CurrentSymbol() const
    {
        return m_currentSymbol;
    }

private:
    Symbol NextSymbol() const
    {
        return m_currentSymbol == Symbol::Cross
            ? Symbol::Nought
            : Symbol::Cross;
    }

    static int64_t rc(int64_t r, int64_t c)
    {
        return r * Dimension() + c;
    }

    Cell GetCell(int64_t index) const
    {
        return m_cells[index];
    }

    bool CompleteDiagonalInternal(
        Symbol symbol, int64_t colStart, int64_t direction) const
    {
        const Cell cell = FromSymbol(symbol);

        int64_t count = 0;
        for (int64_t c = colStart, r = 0; r < Dimension(); c += direction, ++r)
        {
            count = GetCell(r, c) == cell
                ? count + 1
                : count;

            if (count == Dimension())
            {
                return true;
            }
        }

        return false;
    }

    Cell m_cells[9] = {};
    State m_state = State::Playing;
    Symbol m_currentSymbol = Symbol::Cross;
};

class TicTacToeFixture
    : public ::testing::Test
{
public:
    void SetUp() override
    {
        m_ticTacToe = std::make_unique<TicTacToe>();
    }

    void TearDown() override
    {
        m_ticTacToe.reset();
    }

    std::unique_ptr<TicTacToe> m_ticTacToe;
};

TEST(TicTacToeTest, NumberOfCellsIsAvailableInConstContext)
{
    std::unique_ptr<const TicTacToe> ticTacToe = std::make_unique<TicTacToe>();
    EXPECT_THAT(ticTacToe->CellCount(), Eq(9));
}

TEST_F(TicTacToeFixture, GameCreated)
{
    EXPECT_THAT(m_ticTacToe.get(), Ne(nullptr));
}

TEST_F(TicTacToeFixture, NumberOfCellsIs9)
{
    EXPECT_THAT(m_ticTacToe->CellCount(), Eq(9));
}

TEST_F(TicTacToeFixture, CrossBeginsGame)
{
    EXPECT_THAT(m_ticTacToe->CurrentSymbol(), Eq(TicTacToe::Symbol::Cross));
}

TEST_F(TicTacToeFixture, ZeroCellIsEmpty)
{
    EXPECT_THAT(m_ticTacToe->GetCell(0, 0), Eq(TicTacToe::Cell::Empty));
}

TEST_F(TicTacToeFixture, AllCellsAreEmptyAtStart)
{
    for (int64_t r = 0; r < TicTacToe::Dimension(); ++r)
    {
        for (int64_t c = 0; c < TicTacToe::Dimension(); ++c)
        {
            EXPECT_THAT(m_ticTacToe->GetCell(r, c), Eq(TicTacToe::Cell::Empty));
        }
    }
}

TEST_F(TicTacToeFixture, Row1Col1IsSetToNought)
{
    m_ticTacToe->SetCell(1, 1, TicTacToe::Symbol::Nought);
    EXPECT_THAT(m_ticTacToe->GetCell(1, 1), Eq(TicTacToe::Cell::Nought));
}

TEST_F(TicTacToeFixture, Row1Col2IsSetToCross)
{
    m_ticTacToe->SetCell(1, 2, TicTacToe::Symbol::Cross);
    EXPECT_THAT(m_ticTacToe->GetCell(1, 2), Eq(TicTacToe::Cell::Cross));
}

TEST_F(TicTacToeFixture, TopRowOfCrossesWinsGame)
{
    m_ticTacToe->SetCell(0, 0, TicTacToe::Symbol::Cross);
    m_ticTacToe->SetCell(0, 1, TicTacToe::Symbol::Cross);
    m_ticTacToe->SetCell(0, 2, TicTacToe::Symbol::Cross);

    EXPECT_THAT(m_ticTacToe->GameOver(), Eq(true));
}

TEST_F(TicTacToeFixture, LeftColOfNoughtsWinsGame)
{
    m_ticTacToe->SetCell(0, 0, TicTacToe::Symbol::Nought);
    m_ticTacToe->SetCell(1, 0, TicTacToe::Symbol::Nought);
    m_ticTacToe->SetCell(2, 0, TicTacToe::Symbol::Nought);

    EXPECT_THAT(m_ticTacToe->GameOver(), Eq(true));
}

TEST_F(TicTacToeFixture, FullRowCrossWinGame)
{
    for (int64_t r = 0; r < TicTacToe::Dimension(); ++r)
    {
        for (int64_t c = 0; c < TicTacToe::Dimension(); ++c)
        {
            m_ticTacToe->SetCell(r, c, TicTacToe::Symbol::Cross);
        }

        EXPECT_THAT(m_ticTacToe->GameOver(), Eq(true));
        m_ticTacToe->Reset();
    }
}

TEST_F(TicTacToeFixture, FullRowNoughtWinGame)
{
    for (int64_t r = 0; r < TicTacToe::Dimension(); ++r)
    {
        for (int64_t c = 0; c < TicTacToe::Dimension(); ++c)
        {
            m_ticTacToe->SetCell(r, c, TicTacToe::Symbol::Nought);
        }

        EXPECT_THAT(m_ticTacToe->GameOver(), Eq(true));
        m_ticTacToe->Reset();
    }
}

TEST_F(TicTacToeFixture, FullColCrossWinGame)
{
    for (int64_t c = 0; c < TicTacToe::Dimension(); ++c)
    {
        for (int64_t r = 0; r < TicTacToe::Dimension(); ++r)
        {
            m_ticTacToe->SetCell(r, c, TicTacToe::Symbol::Cross);
        }

        EXPECT_THAT(m_ticTacToe->GameOver(), Eq(true));
        m_ticTacToe->Reset();
    }
}

TEST_F(TicTacToeFixture, DiagonalTLBRCrossWinGame)
{
    for (int64_t c = 0, r = 0; r < TicTacToe::Dimension() && c < TicTacToe::Dimension(); ++c, ++r)
    {
        m_ticTacToe->SetCell(r, c, TicTacToe::Symbol::Cross);
    }

    EXPECT_THAT(m_ticTacToe->GameOver(), Eq(true));
}

TEST_F(TicTacToeFixture, DiagonalBLTRCrossWinGame)
{
    for (int64_t c = 0, r = 2; r >= 0 && c < TicTacToe::Dimension(); ++c, --r)
    {
        m_ticTacToe->SetCell(r, c, TicTacToe::Symbol::Cross);
    }

    EXPECT_THAT(m_ticTacToe->GameOver(), Eq(true));
}

TEST_F(TicTacToeFixture, DiagonalTLBRNoughtWinGame)
{
    for (int64_t c = 0, r = 0; r < TicTacToe::Dimension() && c < TicTacToe::Dimension(); ++c, ++r)
    {
        m_ticTacToe->SetCell(r, c, TicTacToe::Symbol::Nought);
    }

    EXPECT_THAT(m_ticTacToe->GameOver(), Eq(true));
}

TEST_F(TicTacToeFixture, DiagonalBLTRNoughtWinGame)
{
    for (int64_t c = 0, r = 2; r >= 0 && c < TicTacToe::Dimension(); ++c, --r)
    {
        m_ticTacToe->SetCell(r, c, TicTacToe::Symbol::Nought);
    }

    EXPECT_THAT(m_ticTacToe->GameOver(), Eq(true));
}

TEST_F(TicTacToeFixture, GameInitiallyNotOver)
{
    EXPECT_THAT(m_ticTacToe->GameOver(), Eq(false));
}

TEST_F(TicTacToeFixture, PlayAlternatesAfterMove)
{
    m_ticTacToe->SetCell(0, 0, m_ticTacToe->CurrentSymbol());
    EXPECT_THAT(m_ticTacToe->CurrentSymbol(), Eq(TicTacToe::Symbol::Nought));
}

TEST_F(TicTacToeFixture, StartingSymbolIsCorrect)
{
    EXPECT_THAT(m_ticTacToe->CurrentSymbol(), Eq(TicTacToe::Symbol::Cross));
    m_ticTacToe->SetCell(0, 0, m_ticTacToe->CurrentSymbol());
    EXPECT_THAT(m_ticTacToe->GetCell(0, 0), Eq(TicTacToe::Cell::Cross));
}

TEST_F(TicTacToeFixture, CannotUseSameSquare)
{
    {
        bool wasSet = m_ticTacToe->SetCell(0, 0, m_ticTacToe->CurrentSymbol());
        EXPECT_THAT(wasSet, Eq(true));
    }

    {
        bool wasSet = m_ticTacToe->SetCell(0, 0, m_ticTacToe->CurrentSymbol());
        EXPECT_THAT(wasSet, Eq(false));
    }
}

TEST_F(TicTacToeFixture, CannotGoOutOfBounds)
{
    {
        bool wasSet = m_ticTacToe->SetCell(-1, 0, m_ticTacToe->CurrentSymbol());
        EXPECT_THAT(wasSet, Eq(false));
    }

    {
        bool wasSet = m_ticTacToe->SetCell(1, 4, m_ticTacToe->CurrentSymbol());
        EXPECT_THAT(wasSet, Eq(false));
    }
}

TEST_F(TicTacToeFixture, NoughtsWin)
{
    m_ticTacToe->SetCell(0, 0, m_ticTacToe->CurrentSymbol());
    m_ticTacToe->SetCell(1, 0, m_ticTacToe->CurrentSymbol());
    m_ticTacToe->SetCell(2, 0, m_ticTacToe->CurrentSymbol());
    m_ticTacToe->SetCell(1, 1, m_ticTacToe->CurrentSymbol());
    m_ticTacToe->SetCell(2, 2, m_ticTacToe->CurrentSymbol());
    m_ticTacToe->SetCell(1, 2, m_ticTacToe->CurrentSymbol());

    EXPECT_THAT(m_ticTacToe->GameOver(), Eq(true));
    EXPECT_THAT(m_ticTacToe->CurrentState(), Eq(TicTacToe::State::NoughtWins));
}

TEST_F(TicTacToeFixture, CrossesWin)
{
    m_ticTacToe->SetCell(2, 0, m_ticTacToe->CurrentSymbol());
    m_ticTacToe->SetCell(0, 0, m_ticTacToe->CurrentSymbol());
    m_ticTacToe->SetCell(2, 1, m_ticTacToe->CurrentSymbol());
    m_ticTacToe->SetCell(0, 2, m_ticTacToe->CurrentSymbol());
    m_ticTacToe->SetCell(2, 2, m_ticTacToe->CurrentSymbol());

    EXPECT_THAT(m_ticTacToe->GameOver(), Eq(true));
    EXPECT_THAT(m_ticTacToe->CurrentState(), Eq(TicTacToe::State::CrossWins));
}

TEST_F(TicTacToeFixture, CellFromSymbol)
{
    EXPECT_THAT(m_ticTacToe->FromSymbol(TicTacToe::Symbol::Cross), Eq(TicTacToe::Cell::Cross));
    EXPECT_THAT(m_ticTacToe->FromSymbol(TicTacToe::Symbol::Nought), Eq(TicTacToe::Cell::Nought));
}

TEST_F(TicTacToeFixture, GameDrawsIfNoMoreMoves)
{
    m_ticTacToe->SetCell(0, 0, m_ticTacToe->CurrentSymbol());
    m_ticTacToe->SetCell(0, 1, m_ticTacToe->CurrentSymbol());
    m_ticTacToe->SetCell(0, 2, m_ticTacToe->CurrentSymbol());
    m_ticTacToe->SetCell(1, 1, m_ticTacToe->CurrentSymbol());
    m_ticTacToe->SetCell(1, 0, m_ticTacToe->CurrentSymbol());
    m_ticTacToe->SetCell(1, 2, m_ticTacToe->CurrentSymbol());
    m_ticTacToe->SetCell(2, 1, m_ticTacToe->CurrentSymbol());
    m_ticTacToe->SetCell(2, 0, m_ticTacToe->CurrentSymbol());
    EXPECT_THAT(m_ticTacToe->GameOver(), Eq(false));
    m_ticTacToe->SetCell(2, 2, m_ticTacToe->CurrentSymbol());
    EXPECT_THAT(m_ticTacToe->CurrentState(), Eq(TicTacToe::State::Draw));
    EXPECT_THAT(m_ticTacToe->GameOver(), Eq(true));
}

TEST_F(TicTacToeFixture, CrossStartingPlayerAfterReset)
{
    m_ticTacToe->SetCell(0, 0, m_ticTacToe->CurrentSymbol());
    m_ticTacToe->SetCell(0, 2, m_ticTacToe->CurrentSymbol());
    m_ticTacToe->SetCell(1, 1, m_ticTacToe->CurrentSymbol());
    m_ticTacToe->SetCell(1, 2, m_ticTacToe->CurrentSymbol());
    m_ticTacToe->SetCell(2, 0, m_ticTacToe->CurrentSymbol());
    m_ticTacToe->SetCell(2, 2, m_ticTacToe->CurrentSymbol());
    EXPECT_THAT(m_ticTacToe->GameOver(), Eq(true));
    m_ticTacToe->Reset();
    EXPECT_THAT(m_ticTacToe->CurrentState(), Eq(TicTacToe::State::Playing));
    EXPECT_THAT(m_ticTacToe->CurrentSymbol(), Eq(TicTacToe::Symbol::Cross));
    EXPECT_THAT(m_ticTacToe->MovesRemaining(), Eq(TicTacToe::CellCount()));
}
