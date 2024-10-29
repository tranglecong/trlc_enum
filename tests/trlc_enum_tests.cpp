#include "common/enum.hpp"

#include <iostream>

TRLC_ENUM(Color,
          RED,
          GREEN,
          BLUE)

int main()
{
    std::cout << Color::dump() << std::endl;
}
