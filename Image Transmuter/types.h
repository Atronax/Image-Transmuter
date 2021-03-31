#ifndef TYPES_H
#define TYPES_H

#include <QFont>

struct Threshold
{
    bool isSet()
    {
        if (tR >= 0 && tR <= 255 && tG >= 0 && tG <= 255 && tB >= 0 && tB <= 255)
            return true;
        else
            return false;
    }

    int tR;
    int tG;
    int tB;
};

struct Scale
{
    bool isSet()
    {
        if (value >= MIN && value <= MAX)
            return true;
        else
            return false;
    }

    static const uint MIN = 1;
    static const uint MAX = 13;
    uint value = 1;
};

struct Font
{
    bool isSet()
    {
        if (font != QFont())
            return true;
        else
            return false;
    }

    QFont font;
};

#endif // TYPES_H
