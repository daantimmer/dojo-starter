#include <algorithm>
#include <array>
#include <bits/ranges_algo.h>
#include <cctype>
#include <cstdlib>
#include <iostream>
#include <memory>
#include <ranges>
#include <string>
#include <variant>

namespace
{
    constexpr auto width = 8;
    constexpr auto height = 8;

    struct Tile;
    struct Board;

    struct Dir
    {
        const int x;
        const int y;
    };

    constexpr Dir dirNW{ -1, -1 };
    constexpr Dir dirN{ 0, -1 };
    constexpr Dir dirNE{ 1, -1 };

    constexpr Dir dirW{ -1, 0 };
    constexpr Dir dirE{ 1, 0 };

    constexpr Dir dirSW{ -1, 1 };
    constexpr Dir dirS{ 0, 1 };
    constexpr Dir dirSE{ 1, 1 };

    constexpr std::array allDirections = { dirNW, dirN, dirNE, dirW, dirE, dirSW, dirS, dirSE };

    struct PlaceTileStrategy
    {
        virtual ~PlaceTileStrategy() = default;
        virtual void Place(Board& board, int x, int y) const = 0;
    };

    struct CheckNeighboursStrategy : PlaceTileStrategy
    {
    };

    struct NoOpCheckNeighboursStrategy : CheckNeighboursStrategy
    {
        void Place(Board& board, int x, int y) const override;
    };

    struct CheckForBlackNeighboursStrategy : CheckNeighboursStrategy
    {
        void Place(Board& board, int x, int y) const override;
    };

    struct CheckForWhiteNeighboursStrategy : CheckNeighboursStrategy
    {
        void Place(Board& board, int x, int y) const override;
    };

    struct InvalidCheckNeighboursStrategy : CheckNeighboursStrategy
    {
        void Place(Board& board, int x, int y) const override;
    };

    struct UpdateStrategy
    {
        virtual ~UpdateStrategy() = default;
        virtual void Update(Board& board, int x, int y) = 0;
    };

    struct NoOpUpdateStrategy : UpdateStrategy
    {
        void Update(Board& board, int x, int y) override;
    };

    struct UpdateToWhiteStrategy : UpdateStrategy
    {
        void Update(Board& board, int x, int y) override;
    };

    struct UpdateToBlackStrategy : UpdateStrategy
    {
        void Update(Board& board, int x, int y) override;
    };

    struct CanPlaceAroundStrategy : PlaceTileStrategy
    {
    };

    struct CanNotPlaceAroundStrategy : CanPlaceAroundStrategy
    {
        void Place(Board& board, int x, int y) const override
        {}
    };

    struct CanPlaceWhiteStrategy : CanPlaceAroundStrategy
    {
        void Place(Board& board, int x, int y) const override;
    };

    struct CanPlaceBlackstrategy : CanPlaceAroundStrategy
    {
        void Place(Board& board, int x, int y) const override;
    };

    struct GameStateStrategy
    {
        virtual ~GameStateStrategy() = default;
        virtual void PrintResults(Board& board) = 0;
    };

    struct EndGameStrategy : GameStateStrategy
    {
        void PrintResults(Board& board) override;
    };

    struct ContinueGameStrategy : GameStateStrategy
    {
        void PrintResults(Board& board) override
        {}
    };

    struct Tile
    {
        Tile(Board& board, int x, int y)
            : board{ board }
            , x{ x }
            , y{ y }
        {}

        virtual const char Value() const = 0;

        virtual std::unique_ptr<CanPlaceAroundStrategy> IsWhite(std::unique_ptr<CanPlaceAroundStrategy> strategy) = 0;
        virtual std::unique_ptr<CanPlaceAroundStrategy> IsBlack(std::unique_ptr<CanPlaceAroundStrategy> strategy) = 0;

        virtual std::unique_ptr<CheckNeighboursStrategy> SetWhite() = 0;
        virtual std::unique_ptr<CheckNeighboursStrategy> SetBlack() = 0;

        virtual std::unique_ptr<UpdateStrategy> UpdateToWhite(Board& board, Dir dir) = 0;
        virtual std::unique_ptr<UpdateStrategy> UpdateToBlack(Board& board, Dir dir) = 0;

        virtual std::unique_ptr<GameStateStrategy> GetGameStateStrategy(std::unique_ptr<GameStateStrategy> strategy) = 0;

        virtual std::uint32_t IncrementWhiteResult(std::uint32_t value) = 0;
        virtual std::uint32_t IncrementBlackResult(std::uint32_t value) = 0;

        Board& board;
        int x;
        int y;
    };

    struct BorderTile : Tile
    {
        BorderTile(Board& board, const char value)
            : Tile{ board, 0, 0 }
            , value{ value }
        {}

        const char Value() const override
        {
            return value;
        };

        std::unique_ptr<CanPlaceAroundStrategy> IsWhite(std::unique_ptr<CanPlaceAroundStrategy> strategy) override
        {
            return strategy;
        }

        std::unique_ptr<CanPlaceAroundStrategy> IsBlack(std::unique_ptr<CanPlaceAroundStrategy> strategy) override
        {
            return strategy;
        }

        std::unique_ptr<CheckNeighboursStrategy> SetWhite() override
        {
            return std::make_unique<NoOpCheckNeighboursStrategy>();
        }

        std::unique_ptr<CheckNeighboursStrategy> SetBlack() override
        {
            return std::make_unique<NoOpCheckNeighboursStrategy>();
        }

        std::unique_ptr<UpdateStrategy> UpdateToWhite(Board& board, Dir dir) override
        {
            return std::make_unique<NoOpUpdateStrategy>();
        }

        std::unique_ptr<UpdateStrategy> UpdateToBlack(Board& board, Dir dir) override
        {
            return std::make_unique<NoOpUpdateStrategy>();
        }

        std::unique_ptr<GameStateStrategy> GetGameStateStrategy(std::unique_ptr<GameStateStrategy> strategy) override
        {
            return strategy;
        }

        std::uint32_t IncrementWhiteResult(std::uint32_t value) override
        {
            return value;
        };

        std::uint32_t IncrementBlackResult(std::uint32_t value) override
        {
            return value;
        };

        const char value;
    };

    struct EmptyTile : Tile
    {
        using Tile::Tile;

        const char Value() const override
        {
            return ' ';
        };

        std::unique_ptr<CanPlaceAroundStrategy> IsWhite(std::unique_ptr<CanPlaceAroundStrategy> strategy) override
        {
            return strategy;
        }

        std::unique_ptr<CanPlaceAroundStrategy> IsBlack(std::unique_ptr<CanPlaceAroundStrategy> strategy) override
        {
            return strategy;
        }

        std::unique_ptr<CheckNeighboursStrategy> SetWhite()
        {
            return std::make_unique<CheckForBlackNeighboursStrategy>();
        };

        std::unique_ptr<CheckNeighboursStrategy> SetBlack()
        {
            return std::make_unique<CheckForWhiteNeighboursStrategy>();
        };

        std::unique_ptr<UpdateStrategy> UpdateToWhite(Board& board, Dir dir) override
        {
            return std::make_unique<NoOpUpdateStrategy>();
        }

        std::unique_ptr<UpdateStrategy> UpdateToBlack(Board& board, Dir dir) override
        {
            return std::make_unique<NoOpUpdateStrategy>();
        }

        std::unique_ptr<GameStateStrategy> GetGameStateStrategy(std::unique_ptr<GameStateStrategy> strategy) override
        {
            return std::make_unique<ContinueGameStrategy>();
        }

        std::uint32_t IncrementWhiteResult(std::uint32_t value) override
        {
            return value;
        };

        std::uint32_t IncrementBlackResult(std::uint32_t value) override
        {
            return value;
        };
    };

    struct WhiteTile : Tile
    {
        using Tile::Tile;

        const char Value() const override
        {
            return 'W';
        };

        std::unique_ptr<CanPlaceAroundStrategy> IsWhite(std::unique_ptr<CanPlaceAroundStrategy> strategy) override
        {
            return std::make_unique<CanPlaceBlackstrategy>();
        }

        std::unique_ptr<CanPlaceAroundStrategy> IsBlack(std::unique_ptr<CanPlaceAroundStrategy> strategy) override
        {
            return strategy;
        }

        std::unique_ptr<CheckNeighboursStrategy> SetWhite() override
        {
            return std::make_unique<InvalidCheckNeighboursStrategy>();
        };

        std::unique_ptr<CheckNeighboursStrategy> SetBlack() override
        {
            return std::make_unique<InvalidCheckNeighboursStrategy>();
        };

        std::unique_ptr<UpdateStrategy> UpdateToWhite(Board& board, Dir dir) override;

        std::unique_ptr<UpdateStrategy> UpdateToBlack(Board& board, Dir dir) override;

        std::unique_ptr<GameStateStrategy> GetGameStateStrategy(std::unique_ptr<GameStateStrategy> strategy) override
        {
            return strategy;
        }

        std::uint32_t IncrementWhiteResult(std::uint32_t value) override
        {
            return value + 1;
        };

        std::uint32_t IncrementBlackResult(std::uint32_t value) override
        {
            return value;
        };
    };

    struct BlackTile : Tile
    {
        using Tile::Tile;

        const char Value() const override
        {
            return 'B';
        };

        std::unique_ptr<CanPlaceAroundStrategy> IsWhite(std::unique_ptr<CanPlaceAroundStrategy> strategy) override
        {
            return strategy;
        }

        std::unique_ptr<CanPlaceAroundStrategy> IsBlack(std::unique_ptr<CanPlaceAroundStrategy> strategy) override
        {
            return std::make_unique<CanPlaceWhiteStrategy>();
        }

        std::unique_ptr<CheckNeighboursStrategy> SetWhite()
        {
            return std::make_unique<InvalidCheckNeighboursStrategy>();
        };

        std::unique_ptr<CheckNeighboursStrategy> SetBlack()
        {
            return std::make_unique<InvalidCheckNeighboursStrategy>();
        };

        std::unique_ptr<UpdateStrategy> UpdateToWhite(Board& board, Dir dir) override;

        std::unique_ptr<UpdateStrategy> UpdateToBlack(Board& board, Dir dir) override;

        std::unique_ptr<GameStateStrategy> GetGameStateStrategy(std::unique_ptr<GameStateStrategy> strategy) override
        {
            return strategy;
        }

        std::uint32_t IncrementWhiteResult(std::uint32_t value) override
        {
            return value;
        };

        std::uint32_t IncrementBlackResult(std::uint32_t value) override
        {
            return value + 1;
        };
    };

    struct Board
    {
        explicit Board()
        {
            const char* rowinput = " ABCDEFGH ";
            const char* colinput = " 01234567 ";

            // fill first and last row with BorderTiles
            std::ranges::for_each(rows.front(), [this, &rowinput, index = 0](std::unique_ptr<Tile>& tile) mutable
                {
                    tile = std::make_unique<BorderTile>(*this, rowinput[index++]);
                });
            std::ranges::for_each(rows.back(), [this, &rowinput, index = 0](std::unique_ptr<Tile>& tile) mutable
                {
                    tile = std::make_unique<BorderTile>(*this, rowinput[index++]);
                });

            // fill first and last column with BorderTiles
            std::ranges::for_each(rows, [this, &colinput, index = 0](std::array<std::unique_ptr<Tile>, width + 2>& row) mutable
                {
                    row.front() = std::make_unique<BorderTile>(*this, colinput[index]);
                    row.back() = std::make_unique<BorderTile>(*this, colinput[index++]);
                });

            std::ranges::for_each(rows | std::views::drop(1) | std::views::take(height),
                [this](std::array<std::unique_ptr<Tile>, width + 2>& row)
                {
                    std::ranges::for_each(row | std::views::drop(1) | std::views::take(width), [this](std::unique_ptr<Tile>& tile)
                        {
                            tile = std::make_unique<EmptyTile>(*this, 0, 0);
                        });
                });

            // fill starting tiles
            InitTile<WhiteTile>(3, 3);
            InitTile<WhiteTile>(4, 4);

            InitTile<BlackTile>(3, 4);
            InitTile<BlackTile>(4, 3);

            Print();
        }

        void Print()
        {
            std::ranges::for_each(rows, PrintRow);
        }

        void EndOrContinue()
        {
            std::unique_ptr<GameStateStrategy> strategy = std::make_unique<EndGameStrategy>();

            std::ranges::for_each(rows | std::views::drop(1) | std::views::take(height),
                [&strategy](std::array<std::unique_ptr<Tile>, width + 2>& row)
                {
                    std::ranges::for_each(row | std::views::drop(1) | std::views::take(width), [&strategy](std::unique_ptr<Tile>& tile)
                        {
                            strategy = tile->GetGameStateStrategy(std::move(strategy));
                        });
                });

            strategy->PrintResults(*this);
        }

        void PrintResults()
        {
            std::uint32_t whiteCount = 0;
            std::uint32_t blackCount = 0;

            std::ranges::for_each(rows | std::views::drop(1) | std::views::take(height),
                [&whiteCount, &blackCount](std::array<std::unique_ptr<Tile>, width + 2>& row)
                {
                    std::ranges::for_each(row | std::views::drop(1) | std::views::take(width), [&whiteCount, &blackCount](std::unique_ptr<Tile>& tile)
                        {
                            whiteCount = tile->IncrementWhiteResult(whiteCount);
                            blackCount = tile->IncrementBlackResult(blackCount);
                        });
                });

            std::cout << "white score: " << whiteCount << "\n"
                      << "black score: " << blackCount << "\n";
        }

        void
        PlaceWhite(int x, int y)
        {
            Get(x, y)->SetWhite()->Place(*this, x, y);

            Print();
            PrintResults();
            EndOrContinue();
        }

        void PlaceBlack(int x, int y)
        {
            Get(x, y)->SetBlack()->Place(*this, x, y);

            Print();
            PrintResults();
            EndOrContinue();
        }

        std::unique_ptr<Tile>& Get(int x, int y)
        {
            return rows[y + 1][x + 1];
        }

        std::unique_ptr<Tile>& Get(int x, int y, Dir dir)
        {
            return Get(x + dir.x, y + dir.y);
        }

        void UpdateToWhite(int x, int y)
        {
            Get(x, y) = std::make_unique<WhiteTile>(*this, x, y);
        }

        void UpdateToBlack(int x, int y)
        {
            Get(x, y) = std::make_unique<BlackTile>(*this, x, y);
        }

    private:
        template<class T>
        void InitTile(int x, int y)
        {
            Get(x, y) = std::make_unique<T>(*this, x, y);
        }

        static void PrintTile(const std::unique_ptr<Tile>& tile)
        {
            std::cout << tile->Value();
        }

        static void PrintRow(const std::array<std::unique_ptr<Tile>, width + 2>& row)
        {
            std::ranges::for_each(row, PrintTile);
            std::cout << "\n";
        }

        std::array<std::array<std::unique_ptr<Tile>, width + 2>, height + 2> rows;
    };

    struct Input
    {
        auto Get()
        {
            std::cout << "> ";
            std::string input;
            std::cin >> input;

            auto column = std::clamp(std::tolower(input[0]) - 'a', 0, 7);
            auto row = std::clamp(input[1] - '0', 0, 7);

            return std::make_pair(column, row);
        }
    };

    /////////////////////////////////

    void EndGameStrategy::PrintResults(Board& board)
    {
        std::exit(0);
    }

    void NoOpCheckNeighboursStrategy::Place(Board& board, int x, int y) const
    {
    }

    void CheckForBlackNeighboursStrategy::Place(Board& board, int x, int y) const
    {
        std::unique_ptr<CanPlaceAroundStrategy> strategy = std::make_unique<CanNotPlaceAroundStrategy>();

        for (const auto dir : allDirections)
        {
            strategy = board.Get(x, y, dir)->IsBlack(std::move(strategy));
        }

        strategy->Place(board, x, y);
    }

    void CheckForWhiteNeighboursStrategy::Place(Board& board, int x, int y) const
    {
        std::unique_ptr<CanPlaceAroundStrategy> strategy = std::make_unique<CanNotPlaceAroundStrategy>();

        for (const auto dir : allDirections)
        {
            strategy = board.Get(x, y, dir)->IsWhite(std::move(strategy));
        }

        strategy->Place(board, x, y);
    }

    void InvalidCheckNeighboursStrategy::Place(Board& board, int x, int y) const
    {
        throw -1;
    }

    void NoOpUpdateStrategy::Update(Board& board, int x, int y)
    {}

    void UpdateToWhiteStrategy::Update(Board& board, int x, int y)
    {
        board.UpdateToWhite(x, y);
    }

    void UpdateToBlackStrategy::Update(Board& board, int x, int y)
    {
        board.UpdateToBlack(x, y);
    }

    void CanPlaceWhiteStrategy::Place(Board& board, int x, int y) const
    {
        board.UpdateToWhite(x, y);

        for (const auto dir : allDirections)
        {
            board.Get(x, y, dir)->UpdateToWhite(board, dir);
        }
    }

    void CanPlaceBlackstrategy::Place(Board& board, int x, int y) const
    {
        board.UpdateToBlack(x, y);

        for (const auto dir : allDirections)
        {
            board.Get(x, y, dir)->UpdateToBlack(board, dir);
        }
    }

    std::unique_ptr<UpdateStrategy> WhiteTile::UpdateToWhite(Board& board, Dir dir)
    {
        return std::make_unique<UpdateToWhiteStrategy>();
    }

    std::unique_ptr<UpdateStrategy> WhiteTile::UpdateToBlack(Board& board, Dir dir)
    {
        auto strategy = board.Get(x, y, dir)->UpdateToBlack(board, dir);
        strategy->Update(board, x, y);
        return strategy;
    }

    std::unique_ptr<UpdateStrategy> BlackTile::UpdateToWhite(Board& board, Dir dir)
    {
        auto strategy = board.Get(x, y, dir)->UpdateToWhite(board, dir);
        strategy->Update(board, x, y);
        return strategy;
    }

    std::unique_ptr<UpdateStrategy> BlackTile::UpdateToBlack(Board& board, Dir dir)
    {
        return std::make_unique<UpdateToBlackStrategy>();
    }
}

int main(int argc, const char* argv[])
{
    Board board;
    Input input;

    std::function<void(int, int)> assign{};

    std::function<void(int, int)> assignW{};
    std::function<void(int, int)> assignB{};

    assignW = [&board, &assign, &assignB](int column, int row)
    {
        board.PlaceWhite(column, row);
        assign = assignB;
    };

    assignB = [&board, &assign, &assignW](int column, int row)
    {
        board.PlaceBlack(column, row);
        assign = assignW;
    };

    assign = assignW;

    while (true)
    {
        {
            std::cout << "W";
            const auto [column, row] = input.Get();
            assign(column, row);
        }
        {
            std::cout << "B";
            const auto [column, row] = input.Get();
            assign(column, row);
        }
    }

    return 0;
}
