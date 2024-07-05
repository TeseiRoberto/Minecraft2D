
// Contains definitions of macros for console output messages

#ifndef LOGGING_H
#define LOGGING_H

#include <stdio.h>

#define GREEN_TEXT_COLOR        "\033[0;32m"
#define YELLOW_TEXT_COLOR       "\033[0;33m"
#define RED_TEXT_COLOR          "\033[0;31m"
#define WHITE_TEXT_COLOR        "\033[0;37m"

#define logInfo(msg, ...)       fprintf(stdout, GREEN_TEXT_COLOR  "[ INFO ] " msg WHITE_TEXT_COLOR "\n", #__VA_ARGS__);
#define logWarn(msg, ...)       fprintf(stderr, YELLOW_TEXT_COLOR "[ WARNING ] " msg WHITE_TEXT_COLOR "\n", #__VA_ARGS__);
#define logError(msg, ...)      fprintf(stderr, RED_TEXT_COLOR "[ ERROR ] " msg WHITE_TEXT_COLOR "\n", #__VA_ARGS__);

#endif // LOGGING_H
