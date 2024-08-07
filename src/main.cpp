
#include "game.hpp"
#include <cstdint>

int main()
{

        mc2d::GameSettings settings;
        mc2d::Game minecraft2D;

        minecraft2D.init(settings);
        minecraft2D.run();
        minecraft2D.terminate();

        return 0;
}
