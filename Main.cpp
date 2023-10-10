#include <algorithm>
#include <array>
#include <iostream>
#include <string>
#include <variant>

namespace
{
    constexpr auto width = 8;
    constexpr auto height = 8;

    struct Tile
    {
        void InitW()
        {
            entry.emplace<W>();
        }

        void InitB()
        {
            entry.emplace<B>();
        }

        void AssignW()
        {
            std::visit([this](auto& entry)
                {
                    entry.AssignW(this->entry);
                },
                entry);
        }

        void AssignB()
        {
            std::visit([this](auto& entry)
                {
                    entry.AssignB(this->entry);
                },
                entry);
        }

        void AssignAvailable()
        {
            std::visit([this](auto& entry)
                {
                    entry.AssignAvailable(this->entry);
                },
                entry);
        }

        void Print()
        {
            std::visit([](auto& entry)
                {
                    entry.Print();
                },
                entry);
        }

    private:
        struct Empty;
        struct Available;
        struct W;
        struct B;

        using TileVariant = std::variant<Empty, Available, W, B>;

        struct Entry
        {
            virtual void AssignW(TileVariant&)
            {
                throw -1;
            }

            virtual void AssignB(TileVariant&)
            {
                throw -1;
            }

            virtual void AssignAvailable(TileVariant&)
            {
                throw -1;
            }

            virtual void Print()
            {
                throw -1;
            }
        };

        struct Empty : Entry
        {
            void AssignAvailable(TileVariant& entry) override
            {
                entry.emplace<Available>();
            }

            void Print() override
            {
                std::cout << ".";
            }
        };

        struct Available : Empty
        {
            void AssignW(TileVariant& entry) override
            {
                entry.emplace<W>();
            }

            void AssignB(TileVariant& entry) override
            {
                entry.emplace<B>();
            }

            void AssignAvailable(TileVariant&) override
            {
            }
        };

        struct W : Entry
        {
            void AssignAvailable(TileVariant&) override
            {
            }

            void Print() override
            {
                std::cout << "W";
            }
        };

        struct B : Entry
        {
            void AssignAvailable(TileVariant&) override
            {
            }

            void Print() override
            {
                std::cout << "B";
            }
        };

        TileVariant entry;
    };

    struct Board
    {
        void AssignAvailableOrIgnore(int column, int row)
        {
            std::array<std::function<void(int, int)>, 2> assignOrIgnore = {
                [](auto, auto) {},
                [this](auto column, auto row)
                {
                    rows.at(row).at(column).AssignAvailable();
                }
            };

            bool columnValid = column >= 0 && column < (width);
            bool rowValid = row >= 0 && row < (height);

            assignOrIgnore[static_cast<int>(columnValid && rowValid)](column, row);
        }

        Board()
        {
            InitW(3, 3);
            InitB(4, 3);

            InitB(3, 4);
            InitW(4, 4);
        }

        void Print()
        {
            std::cout << "  ABCDEFGH\n";
            auto rowNr = 0;

            for (auto& row : rows)
            {
                std::cout << rowNr << " ";
                for (auto& entry : row)
                {
                    entry.Print();
                }
                std::cout << "\n";
                ++rowNr;
            }
        }

        void AssignW(int x, int y)
        {
            AssignAvailableOrIgnore(x - 1, y - 1);
            AssignAvailableOrIgnore(x, y - 1);
            AssignAvailableOrIgnore(x + 1, y - 1);

            AssignAvailableOrIgnore(x - 1, y);
            rows.at(y).at(x).AssignW();
            AssignAvailableOrIgnore(x + 1, y);

            AssignAvailableOrIgnore(x - 1, y + 1);
            AssignAvailableOrIgnore(x, y + 1);
            AssignAvailableOrIgnore(x + 1, y + 1);
        }

        void AssignB(int x, int y)
        {
            AssignAvailableOrIgnore(x - 1, y - 1);
            AssignAvailableOrIgnore(x, y - 1);
            AssignAvailableOrIgnore(x + 1, y - 1);

            AssignAvailableOrIgnore(x - 1, y);
            rows.at(y).at(x).AssignB();
            AssignAvailableOrIgnore(x + 1, y);

            AssignAvailableOrIgnore(x - 1, y + 1);
            AssignAvailableOrIgnore(x, y + 1);
            AssignAvailableOrIgnore(x + 1, y + 1);
        }

        void InitW(int x, int y)
        {
            AssignAvailableOrIgnore(x - 1, y - 1);
            AssignAvailableOrIgnore(x, y - 1);
            AssignAvailableOrIgnore(x + 1, y - 1);

            AssignAvailableOrIgnore(x - 1, y);
            rows.at(y).at(x).InitW();
            AssignAvailableOrIgnore(x + 1, y);

            AssignAvailableOrIgnore(x - 1, y + 1);
            AssignAvailableOrIgnore(x, y + 1);
            AssignAvailableOrIgnore(x + 1, y + 1);
        }

        void InitB(int x, int y)
        {
            AssignAvailableOrIgnore(x - 1, y - 1);
            AssignAvailableOrIgnore(x, y - 1);
            AssignAvailableOrIgnore(x + 1, y - 1);

            AssignAvailableOrIgnore(x - 1, y);
            rows.at(y).at(x).InitB();
            AssignAvailableOrIgnore(x + 1, y);

            AssignAvailableOrIgnore(x - 1, y + 1);
            AssignAvailableOrIgnore(x, y + 1);
            AssignAvailableOrIgnore(x + 1, y + 1);
        }

        std::array<std::array<Tile, width>, height> rows;
    };

    struct Input
    {
        auto Get()
        {
            std::cout << "> ";
            std::string input;
            std::cin >> input;

            auto column = input[0] - 'A';
            auto row = input[1] - '0';

            return std::make_pair(column, row);
        }
    };
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
        board.AssignW(column, row);
        assign = assignB;
    };

    assignB = [&board, &assign, &assignW](int column, int row)
    {
        board.AssignB(column, row);
        assign = assignW;
    };

    assign = assignW;

    while (true)
    {
        board.Print();
        const auto [column, row] = input.Get();
        assign(column, row);
    }

    return 0;
}
