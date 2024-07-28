
// Defines some utility functions

#ifndef UTILITY_H
#define UTILITY_H

#include <iostream>
#include <type_traits>

namespace mc2d {


        // Performs intersection test on two rectangles (named A and B below)
        // @*x: x coordinate of the top left vertex of the * rectangle
        // @*y: y coordinate of the top left vertex of the * rectangle
        // @*Width: width of the * rectangle
        // @*Height: height of the * rectangle
        // @returns: true if there is an intersection between the given rectangles, false otherwise
        template <typename T>
        bool doesRectsIntersect(const T ax , const T ay, const T aWidth, const T aHeight,
                const T bx , const T by, const T bWidth, const T bHeight)
        {
                static_assert(std::is_integral<T>::value || std::is_floating_point<T>::value, "doesRectsIntersects() requires a integral type value!");
                // Check for non intersection on x axis
                if(ax > bx)
                {
                        if(ax >= bx + bWidth)
                                return false;
                } else {
                        if(ax + aWidth <= bx)
                                return false;
                }

                // Check for non intersection on y axis
                if(ay > by)
                {
                        if(ay - aHeight >= by)
                                return false;
                } else {
                        if(ay <= by - bHeight)
                                return false;
                }

                return true;
        }

}

#endif // UTILITY_H

