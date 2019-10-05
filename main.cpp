#include "gtest/gtest.h"
#include "gmock/gmock.h"
#include <memory>

using namespace testing;

int main(int argc, char **argv) {
    InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}

// OnlyValidInputAccepted
// HandlePlayerMoveOutOfBounds
// GameOverWithRowOfThree
// PlayersAlternate

// One to many - when dealing with a collection, only think of one, and then think about multiple
// Don't go for the Gold - work around the edge cases, generate cases, easiest cases
// Cleanliness of the test code is as important as the production code
// Only write Red tests

class TicTacToe
{
public:
    enum class Symbol
    {
        Empty,
        Cross,
        Nought
    };

    enum class Player
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

    int64_t CellCount() const { return 9; }

    Player CurrentPlayer() const { return m_currentPlayer; }

    Symbol GetCell(int64_t r, int64_t c)
    {
        return m_cells[rc(r, c)];
    }

    int64_t rc(int64_t r, int64_t c)
    {
        return r * 3 + c;
    }

    Player NextPlayer() const
    {
        return m_currentPlayer == Player::Cross
            ? Player::Nought
            : Player::Cross;
    }

    Symbol NextSymbol() const
    {
        return m_currentSymbol == Symbol::Cross
            ? Symbol::Nought
            : Symbol::Cross;
    }

    bool SetCell(int64_t r, int64_t c, Symbol cell)
    {
        if (r < 0 || r > 2 || c < 0 || c > 2) {
            return false;
        }

        bool wasSet = false;
        if (GetCell(r, c) == Symbol::Empty)
        {
            m_cells[rc(r, c)] = cell;
            wasSet = true;
        }
        
        if (wasSet)
        {
            m_state = CheckBoard();
            
            if (!GameOver())
            {
                m_currentPlayer = NextPlayer();
                m_currentSymbol = NextSymbol();
            }
        }

        return wasSet;
    }

    State CheckBoard()
    {
        for (int64_t r = 0; r < 3; ++r)
        {
            bool cross = false;
            bool nought = false;
            for (int64_t c = 0; c < 3; ++c)
            {
                if (GetCell(r, c) == Symbol::Cross)
                {
                    if (!cross && c == 0)
                    {
                        cross = true;
                    }
                }
                else
                {
                    if (cross)
                    {
                        cross = false;
                    }
                }

                if (GetCell(r, c) == Symbol::Nought)
                {
                    if (!nought && c == 0)
                    {
                        nought = true;
                    }
                }
                else
                {
                    if (nought)
                    {
                        nought = false;
                    }
                }
            }

            if (cross || nought)
            {
                return cross ? State::CrossWins : State::NoughtWins;
            }
        }

        for (int64_t c = 0; c < 3; ++c)
        {
            bool cross = false;
            bool nought = false;
            for (int64_t r = 0; r < 3; ++r)
            {
                if (GetCell(r, c) == Symbol::Cross)
                {
                    if (!cross && r == 0)
                    {
                        cross = true;
                    }
                }
                else
                {
                    if (cross)
                    {
                        cross = false;
                    }
                }

                if (GetCell(r, c) == Symbol::Nought)
                {
                    if (!nought && r == 0)
                    {
                        nought = true;
                    }
                }
                else
                {
                    if (nought)
                    {
                        nought = false;
                    }
                }
            }

            if (nought || cross)
            {
                return cross ? State::CrossWins : State::NoughtWins;
            }
        }

        {
            bool cross = false;
            bool nought = false;
            for (int64_t c = 0, r = 0; r < 3 && c < 3; ++c, ++r)
            {
                if (GetCell(r, c) == Symbol::Cross)
                {
                    if (!cross && c == 0)
                    {
                        cross = true;
                    }
                }
                else
                {
                    if (cross)
                    {
                        cross = false;
                    }
                }

                if (GetCell(r, c) == Symbol::Nought)
                {
                    if (!nought && c == 0)
                    {
                        nought = true;
                    }
                }
                else
                {
                    if (nought)
                    {
                        nought = false;
                    }
                }
            }
            
            if (cross || nought)
            {
                return cross ? State::CrossWins : State::NoughtWins;
            }
        }

        {
            bool cross = false;
            bool nought = false;
            for (int64_t c = 0, r = 2; r >= 0 && c < 3; ++c, --r)
            {
                if (GetCell(r, c) == Symbol::Cross)
                {
                    if (!cross && c == 0)
                    {
                        cross = true;
                    }
                }
                else
                {
                    if (cross)
                    {
                        cross = false;
                    }
                }

                if (GetCell(r, c) == Symbol::Nought)
                {
                    if (!nought && c == 0)
                    {
                        nought = true;
                    }
                }
                else
                {
                    if (nought)
                    {
                        nought = false;
                    }
                }
            }

            if (cross || nought)
            {
                return cross ? State::CrossWins : State::NoughtWins;
            }
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
        memset(m_cells, 0, sizeof(Symbol) * CellCount());
        m_state = State::Playing;
    }

    void PrintBoard()
    {
        for (int64_t r = 0; r < 3; ++r)
        {
            for (int64_t c = 0; c < 3; ++c)
            {
                printf("%s", GetCell(r, c) != Symbol::Empty ? "[x]" : "[-]");
            }

            printf("%s", "\n");
        }
    }

    Symbol CurrentSymbol() const {
        return m_currentSymbol;
    }

    Symbol m_cells[9] {};
    State m_state = State::Playing;
    Player m_currentPlayer = Player::Cross;
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

TEST_F(TicTacToeFixture, PlayerXBeginsGame)
{
    EXPECT_THAT(m_ticTacToe->CurrentPlayer(), Eq(TicTacToe::Player::Cross));
}

TEST_F(TicTacToeFixture, ZeroCellIsEmpty)
{
    EXPECT_THAT(m_ticTacToe->GetCell(0, 0), Eq(TicTacToe::Symbol::Empty));
}

TEST_F(TicTacToeFixture, AllCellsAreEmptyAtStart)
{
    for (int64_t r = 0; r < 3; ++r)
    {
        for (int64_t c = 0; c < 3; ++c)
        {
            EXPECT_THAT(m_ticTacToe->GetCell(r, c), Eq(TicTacToe::Symbol::Empty));
        }
    }
}

TEST_F(TicTacToeFixture, Row1Col1IsSetToNought)
{
    m_ticTacToe->SetCell(1, 1, TicTacToe::Symbol::Nought);
    EXPECT_THAT(m_ticTacToe->GetCell(1, 1), Eq(TicTacToe::Symbol::Nought));
}

TEST_F(TicTacToeFixture, Row1Col2IsSetToCross)
{
    m_ticTacToe->SetCell(1, 2, TicTacToe::Symbol::Cross);
    EXPECT_THAT(m_ticTacToe->GetCell(1, 2), Eq(TicTacToe::Symbol::Cross));
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
    for (int64_t r = 0; r < 3; ++r)
    {
        for (int64_t c = 0; c < 3; ++c)
        {
            m_ticTacToe->SetCell(r, c, TicTacToe::Symbol::Cross);
        }

        EXPECT_THAT(m_ticTacToe->GameOver(), Eq(true));
        m_ticTacToe->Reset();
    }
}

TEST_F(TicTacToeFixture, FullRowNoughtWinGame)
{
    for (int64_t r = 0; r < 3; ++r)
    {
        for (int64_t c = 0; c < 3; ++c)
        {
            m_ticTacToe->SetCell(r, c, TicTacToe::Symbol::Nought);
        }

        EXPECT_THAT(m_ticTacToe->GameOver(), Eq(true));
        m_ticTacToe->Reset();
    }
}

TEST_F(TicTacToeFixture, FullColCrossWinGame)
{
    for (int64_t c = 0; c < 3; ++c)
    {
        for (int64_t r = 0; r < 3; ++r)
        {
            m_ticTacToe->SetCell(r, c, TicTacToe::Symbol::Cross);
        }

        EXPECT_THAT(m_ticTacToe->GameOver(), Eq(true));
        m_ticTacToe->Reset();
    }
}

TEST_F(TicTacToeFixture, DiagonalTLBRCrossWinGame)
{
    for (int64_t c = 0, r = 0; r < 3 && c < 3; ++c, ++r)
    {
        m_ticTacToe->SetCell(r, c, TicTacToe::Symbol::Cross);
    }

    EXPECT_THAT(m_ticTacToe->GameOver(), Eq(true));
}

TEST_F(TicTacToeFixture, DiagonalBLTRCrossWinGame)
{
    for (int64_t c = 0, r = 2; r >= 0 && c < 3; ++c, --r)
    {
        m_ticTacToe->SetCell(r, c, TicTacToe::Symbol::Cross);
    }

    EXPECT_THAT(m_ticTacToe->GameOver(), Eq(true));
}

TEST_F(TicTacToeFixture, DiagonalTLBRNoughtWinGame)
{
    for (int64_t c = 0, r = 0; r < 3 && c < 3; ++c, ++r)
    {
        m_ticTacToe->SetCell(r, c, TicTacToe::Symbol::Nought);
    }

    EXPECT_THAT(m_ticTacToe->GameOver(), Eq(true));
}

TEST_F(TicTacToeFixture, DiagonalBLTRNoughtWinGame)
{
    for (int64_t c = 0, r = 2; r >= 0 && c < 3; ++c, --r)
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
    EXPECT_THAT(m_ticTacToe->CurrentPlayer(), Eq(TicTacToe::Player::Nought));
    EXPECT_THAT(m_ticTacToe->CurrentSymbol(), Eq(TicTacToe::Symbol::Nought));
}

TEST_F(TicTacToeFixture, StartingSymbolIsCorrect)
{
    EXPECT_THAT(m_ticTacToe->CurrentSymbol(), Eq(TicTacToe::Symbol::Cross));
    m_ticTacToe->SetCell(0, 0, m_ticTacToe->CurrentSymbol());
    EXPECT_THAT(m_ticTacToe->GetCell(0, 0), Eq(TicTacToe::Symbol::Cross));
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