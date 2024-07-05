
// Contains definition of the Game class, this class encapsulates the game state

#ifndef GAME_H
#define GAME_H

#include "logging.hpp"


namespace mc2d {


        class Game {
        public:
                Game();
                ~Game();

                void    init();
                void    terminate();
                void    run();

        private:

                enum class GameState {
                        UNINITIALIZED,  // Game has not been initialized (init has not been called or an error occurred during initialization) or has been terminated
                        INITIALIZED,    // Game has been initialized correctly and is ready for run
                        RUNNING,        // Game is running
                        PAUSED,         // Game has been paused
                        QUITTED         // Game has been quitted
                };

                GameState       m_gameState;
        };

}

#endif // GAME_H
