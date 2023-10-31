#include <algorithm>
#include <array>
#include <bits/ranges_algo.h>
#include <cctype>
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
        virtual void Update(Board& board, int x, int y, Dir dir) = 0;
    };

    struct NoOpUpdateStrategy : UpdateStrategy
    {
        void Update(Board& board, int x, int y, Dir dir) override;
    };

    struct UpdateToWhiteStrategy : UpdateStrategy
    {
        void Update(Board& board, int x, int y, Dir dir) override;
    };

    struct UpdateToBlackStrategy : UpdateStrategy
    {
        void Update(Board& board, int x, int y, Dir dir) override;
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

    struct Tile
    {
        virtual const char* Value() const = 0;

        virtual std::unique_ptr<CanPlaceAroundStrategy> IsWhite(std::unique_ptr<CanPlaceAroundStrategy> strategy) = 0;
        virtual std::unique_ptr<CanPlaceAroundStrategy> IsBlack(std::unique_ptr<CanPlaceAroundStrategy> strategy) = 0;

        virtual std::unique_ptr<CheckNeighboursStrategy> SetWhite() = 0;
        virtual std::unique_ptr<CheckNeighboursStrategy> SetBlack() = 0;

        virtual std::unique_ptr<UpdateStrategy> UpdateToWhite(Board& board, int x, int y, Dir dir) = 0;
        virtual std::unique_ptr<UpdateStrategy> UpdateToBlack(Board& board, int x, int y, Dir dir) = 0;
    };

    struct BorderTile : Tile
    {
        const char* Value() const override
        {
            return "X";
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

        std::unique_ptr<UpdateStrategy> UpdateToWhite(Board& board, int x, int y, Dir dir) override
        {
            return std::make_unique<NoOpUpdateStrategy>();
        }

        std::unique_ptr<UpdateStrategy> UpdateToBlack(Board& board, int x, int y, Dir dir) override
        {
            return std::make_unique<NoOpUpdateStrategy>();
        }
    };

    struct EmptyTile : Tile
    {
        const char* Value() const override
        {
            return " ";
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

        std::unique_ptr<UpdateStrategy> UpdateToWhite(Board& board, int x, int y, Dir dir) override
        {
            return std::make_unique<NoOpUpdateStrategy>();
        }

        std::unique_ptr<UpdateStrategy> UpdateToBlack(Board& board, int x, int y, Dir dir) override
        {
            return std::make_unique<NoOpUpdateStrategy>();
        }
    };

    struct WhiteTile : Tile
    {
        const char* Value() const override
        {
            return "W";
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

        std::unique_ptr<UpdateStrategy> UpdateToWhite(Board& board, int x, int y, Dir dir) override;

        std::unique_ptr<UpdateStrategy> UpdateToBlack(Board& board, int x, int y, Dir dir) override;
    };

    struct BlackTile : Tile
    {
        const char* Value() const override
        {
            return "B";
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

        std::unique_ptr<UpdateStrategy> UpdateToWhite(Board& board, int x, int y, Dir dir) override;

        std::unique_ptr<UpdateStrategy> UpdateToBlack(Board& board, int x, int y, Dir dir) override;
    };

    struct Board
    {
        explicit Board()
        {
            // fill first and last row with BorderTiles
            std::ranges::for_each(rows.front(), [](std::unique_ptr<Tile>& tile)
                {
                    tile = std::make_unique<BorderTile>();
                });
            std::ranges::for_each(rows.back(), [](std::unique_ptr<Tile>& tile)
                {
                    tile = std::make_unique<BorderTile>();
                });

            // fill first and last column with BorderTiles
            std::ranges::for_each(rows, [](std::array<std::unique_ptr<Tile>, width + 2>& row)
                {
                    row.front() = std::make_unique<BorderTile>();
                    row.back() = std::make_unique<BorderTile>();
                });

            std::ranges::for_each(rows | std::views::drop(1) | std::views::take(height),
                [](std::array<std::unique_ptr<Tile>, width + 2>& row)
                {
                    std::ranges::for_each(row | std::views::drop(1) | std::views::take(width), [](std::unique_ptr<Tile>& tile)
                        {
                            tile = std::make_unique<EmptyTile>();
                        });
                });

            // fill starting tiles
            InitTile<WhiteTile>(3, 3);
            InitTile<WhiteTile>(4, 4);

            InitTile<BlackTile>(3, 4);
            InitTile<BlackTile>(4, 3);

            Print();
            PlaceBlack(2, 3);
            Print();

            PlaceWhite(2, 4);
            Print();

            PlaceBlack(3, 5);
            Print();

            PlaceWhite(2, 2);
            Print();
        }

        void Print()
        {
            std::ranges::for_each(rows, PrintRow);
        }

        void PlaceWhite(int x, int y)
        {
            std::cout << "PlaceWhite(" << x << "," << y << ")\n";
            Get(x, y)->SetWhite()->Place(*this, x, y);
        }

        void PlaceBlack(int x, int y)
        {
            std::cout << "PlaceBlack(" << x << "," << y << ")\n";
            Get(x, y)->SetBlack()->Place(*this, x, y);
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
            Get(x, y) = std::make_unique<WhiteTile>();
        }

        void UpdateToBlack(int x, int y)
        {
            Get(x, y) = std::make_unique<BlackTile>();
        }

    private:
        template<class T>
        void InitTile(int x, int y)
        {
            rows[y + 1][x + 1] = std::make_unique<T>();
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

    void NoOpCheckNeighboursStrategy::Place(Board& board, int x, int y) const
    {
    }

    void CheckForBlackNeighboursStrategy::Place(Board& board, int x, int y) const
    {
        std::unique_ptr<CanPlaceAroundStrategy> strategy = std::make_unique<CanNotPlaceAroundStrategy>();

        strategy = board.Get(x, y, dirNW)->IsBlack(std::move(strategy));
        strategy = board.Get(x, y, dirN)->IsBlack(std::move(strategy));
        strategy = board.Get(x, y, dirNE)->IsBlack(std::move(strategy));

        strategy = board.Get(x, y, dirW)->IsBlack(std::move(strategy));
        strategy = board.Get(x, y, dirE)->IsBlack(std::move(strategy));

        strategy = board.Get(x, y, dirSW)->IsBlack(std::move(strategy));
        strategy = board.Get(x, y, dirS)->IsBlack(std::move(strategy));
        strategy = board.Get(x, y, dirSE)->IsBlack(std::move(strategy));

        strategy->Place(board, x, y);
    }

    void CheckForWhiteNeighboursStrategy::Place(Board& board, int x, int y) const
    {
        std::unique_ptr<CanPlaceAroundStrategy> strategy = std::make_unique<CanNotPlaceAroundStrategy>();

        strategy = board.Get(x, y, dirNW)->IsWhite(std::move(strategy));
        strategy = board.Get(x, y, dirN)->IsWhite(std::move(strategy));
        strategy = board.Get(x, y, dirNE)->IsWhite(std::move(strategy));

        strategy = board.Get(x, y, dirW)->IsWhite(std::move(strategy));
        strategy = board.Get(x, y, dirE)->IsWhite(std::move(strategy));

        strategy = board.Get(x, y, dirSW)->IsWhite(std::move(strategy));
        strategy = board.Get(x, y, dirS)->IsWhite(std::move(strategy));
        strategy = board.Get(x, y, dirSE)->IsWhite(std::move(strategy));

        strategy->Place(board, x, y);
    }

    void InvalidCheckNeighboursStrategy::Place(Board& board, int x, int y) const
    {
        throw -1;
    }

    void NoOpUpdateStrategy::Update(Board& board, int x, int y, Dir dir)
    {}

    void UpdateToWhiteStrategy::Update(Board& board, int x, int y, Dir dir)
    {
        board.UpdateToWhite(x, y);
    }

    void UpdateToBlackStrategy::Update(Board& board, int x, int y, Dir dir)
    {
        board.UpdateToBlack(x, y);
    }

    void CanPlaceWhiteStrategy::Place(Board& board, int x, int y) const
    {
        board.UpdateToWhite(x, y);

        const auto getX = [](int x, Dir dir)
        {
            return x + dir.x;
        };
        const auto getY = [](int y, Dir dir)
        {
            return y + dir.y;
        };

        board.Get(x, y, dirNW)->UpdateToWhite(board, getX(x, dirNW), getY(y, dirNW), dirNW);
        board.Get(x, y, dirN)->UpdateToWhite(board, getX(x, dirN), getY(y, dirN), dirN);
        board.Get(x, y, dirNE)->UpdateToWhite(board, getX(x, dirNE), getY(y, dirNE), dirNE);

        board.Get(x, y, dirW)->UpdateToWhite(board, getX(x, dirW), getY(y, dirW), dirW);
        board.Get(x, y, dirE)->UpdateToWhite(board, getX(x, dirE), getY(y, dirE), dirE);

        board.Get(x, y, dirSW)->UpdateToWhite(board, getX(x, dirSW), getY(y, dirSW), dirSW);
        board.Get(x, y, dirS)->UpdateToWhite(board, getX(x, dirS), getY(y, dirS), dirS);
        board.Get(x, y, dirSE)->UpdateToWhite(board, getX(x, dirSE), getY(y, dirSE), dirSE);
    }

    void CanPlaceBlackstrategy::Place(Board& board, int x, int y) const
    {
        board.UpdateToBlack(x, y);

        const auto getX = [](int x, Dir dir)
        {
            return x + dir.x;
        };
        const auto getY = [](int y, Dir dir)
        {
            return y + dir.y;
        };

        board.Get(x, y, dirNW)->UpdateToBlack(board, getX(x, dirNW), getY(y, dirNW), dirNW);
        board.Get(x, y, dirN)->UpdateToBlack(board, getX(x, dirN), getY(y, dirN), dirN);
        board.Get(x, y, dirNE)->UpdateToBlack(board, getX(x, dirNE), getY(y, dirNE), dirNE);

        board.Get(x, y, dirW)->UpdateToBlack(board, getX(x, dirW), getY(y, dirW), dirW);
        board.Get(x, y, dirE)->UpdateToBlack(board, getX(x, dirE), getY(y, dirE), dirE);

        board.Get(x, y, dirSW)->UpdateToBlack(board, getX(x, dirSW), getY(y, dirSW), dirSW);
        board.Get(x, y, dirS)->UpdateToBlack(board, getX(x, dirS), getY(y, dirS), dirS);
        board.Get(x, y, dirSE)->UpdateToBlack(board, getX(x, dirSE), getY(y, dirSE), dirSE);
    }

    std::unique_ptr<UpdateStrategy> WhiteTile::UpdateToWhite(Board& board, int x, int y, Dir dir)
    {
        return std::make_unique<UpdateToWhiteStrategy>();
    }

    std::unique_ptr<UpdateStrategy> WhiteTile::UpdateToBlack(Board& board, int x, int y, Dir dir)
    {
        auto strategy = board.Get(x, y, dir)->UpdateToBlack(board, x, y, dir);
        strategy->Update(board, x, y, dir);
        return strategy;
    }

    std::unique_ptr<UpdateStrategy> BlackTile::UpdateToWhite(Board& board, int x, int y, Dir dir)
    {
        auto strategy = board.Get(x, y, dir)->UpdateToWhite(board, x, y, dir);
        strategy->Update(board, x, y, dir);
        return strategy;
    }

    std::unique_ptr<UpdateStrategy> BlackTile::UpdateToBlack(Board& board, int x, int y, Dir dir)
    {
        return std::make_unique<UpdateToBlackStrategy>();
    }
}

int main(int argc, const char* argv[])
{
    Board board;
    // Input input;

    // std::function<void(int, int)> assign{};

    // std::function<void(int, int)> assignW{};
    // std::function<void(int, int)> assignB{};

    // assignW = [&board, &assign, &assignB](int column, int row)
    // {
    //     board.AssignW(column, row);
    //     assign = assignB;
    // };

    // assignB = [&board, &assign, &assignW](int column, int row)
    // {
    //     board.AssignB(column, row);
    //     assign = assignW;
    // };

    // assign = assignW;

    // while (true)
    // {
    board.Print();
    //     const auto [column, row] = input.Get();
    //     assign(column, row);
    // }

    return 0;
}
