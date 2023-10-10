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
        struct Empty;
        struct W;
        struct B;

        struct Entry
        {
            virtual void AssignW(std::variant<Empty, W, B>&)
            {
                throw -1;
            }

            virtual void AssignB(std::variant<Empty, W, B>&)
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
            void AssignW(std::variant<Empty, W, B>& entry) override
            {
                entry.emplace<W>();
            }

            void AssignB(std::variant<Empty, W, B>& entry) override
            {
                entry.emplace<B>();
            }

            void Print() override
            {
                std::cout << ".";
            }
        };

        struct W : Entry
        {
            void Print() override
            {
                std::cout << "W";
            }
        };

        struct B : Entry
        {
            void Print() override
            {
                std::cout << "B";
            }
        };

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

        void Print()
        {
            std::visit([](auto& entry)
                {
                    entry.Print();
                },
                entry);
        }

    private:
        std::variant<Empty, W, B> entry;
    };

    struct Board
    {
        Board()
        {
            AssignW(3, 3);
            AssignB(4, 3);

            AssignB(3, 4);
            AssignW(4, 4);
        }

        void Print()
        {
            for (auto& row : rows)
            {
                for (auto& entry : row)
                {
                    entry.Print();
                }
                std::cout << "\n";
            }
        }

        void AssignW(int x, int y)
        {
            rows.at(y).at(x).AssignW();
        }

        void AssignB(int x, int y)
        {
            rows.at(y).at(x).AssignB();
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
