#ifndef TOUCHSCREEN_H
#define TOUCHSCREEN_H

#include "device.h"

class TouchScreen
{
public:
    TouchScreen();

    typedef enum TOUCH_TYPE {
        SINGLE_TAP,
        DOUBLE_TAP
    } TouchType;

    typedef enum AREA { // left to right, top to bootom
        TOP_LEFT = 0,
        TOP_MIDDLE,
        TOP_RIGHT,
        LEFT,
        CENTER,
        RIGHT,
        BOTTOM_MIDDLE
    } Area;

    Area getPointArea(int x, int y);
    Qt::Key getAreaAction(int x, int y, TouchType t);
private:
    int rightMargin, rpx;
    int leftMargin, lpx;
    int topMargin, tpx;
    int bottomMargin, bpx;

    int w;
    int h;

    static const Qt::Key DOUBLE_TAP_ACTIONS[];
    static const Qt::Key SINGLE_TAP_ACTIONS[];
};

#endif // TOUCHSCREEN_H
