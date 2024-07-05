
#include "game.hpp"

namespace mc2d {


        Game::Game() : m_gameState(GameState::UNINITIALIZED)
        {}


        // Terminates the game (if not already done)
        Game::~Game()
        {
                if(m_gameState != GameState::UNINITIALIZED)
                        terminate();
        }


        // Attempts to initialize all the resources needed by the game
        void Game::init()
        {
                if(m_gameState != GameState::UNINITIALIZED)
                {
                        logWarn("Game::init() failed, game has already been initialized!");
                        return;
                }

                // TODO: Initialize stuff..

                m_gameState = GameState::INITIALIZED;
        }


        // Terminates all the resources initialized in the init method
        void Game::terminate()
        {
                if(m_gameState == GameState::UNINITIALIZED)
                {
                        logWarn("Game::terminate() failed, game has already been terminated!");
                        return;
                }

                // TODO: Terminate stuff..
       
                m_gameState = GameState::UNINITIALIZED;                
        }


        // Starts the main game loop
        void Game::run()
        {
                if(m_gameState != GameState::INITIALIZED)
                {
                        logError("Game::run() failed, game has not been intialized correctly or is already running!");
                        return;
                }

                // TODO: Implement main game loop
                logInfo("       EXECUTING GAME LOOP ");

                m_gameState = GameState::QUITTED;
        }

}
