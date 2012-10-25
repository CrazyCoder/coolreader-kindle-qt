#include "touchscreen.h"

const Qt::Key TouchScreen::DOUBLE_TAP_ACTIONS[] = {
    Qt::Key_Escape, Qt::Key_Home, Qt::Key_Select,
    Qt::Key_Up, Qt::Key_O, Qt::Key_Down,
                 Qt::Key_Menu
};

const Qt::Key TouchScreen::SINGLE_TAP_ACTIONS[] = {
    Qt::Key_unknown, Qt::Key_unknown, Qt::Key_unknown,
    Qt::Key_PageUp, Qt::Key_PageDown, Qt::Key_PageDown,
                    Qt::Key_unknown
};

TouchScreen::TouchScreen()
{
    topMargin = 30; // margins in %
    bottomMargin = 30;
    rightMargin = 30;
    leftMargin = 30;

    w = Device::getWidth();
    h = Device::getHeight();

    lpx = w * leftMargin / 100;
    rpx = w * rightMargin / 100;
    tpx = h * topMargin / 100;
    bpx = h * bottomMargin / 100;
}

TouchScreen::Area TouchScreen::getPointArea(int x, int y)
{
    if (y < tpx) {
        if (x < lpx) return TOP_LEFT;
        else if (x >= lpx && x < w - rpx) return TOP_MIDDLE;
        else if (x >= w - rpx) return TOP_RIGHT;
    } else if (y >= tpx) {
        if (x < lpx) return LEFT;
        else if (x >= w - rpx) return RIGHT;
        else if (x >= lpx && x < w - rpx) {
            if (y >= h - bpx) return BOTTOM_MIDDLE;
            else return CENTER;
        }
    }
    qDebug("UNCOVERED AREA: %d, %d", x, y);
    return CENTER;
}

Qt::Key TouchScreen::getAreaAction(int x, int y, TouchScreen::TouchType t)
{
    switch(t) {
    case SINGLE_TAP:
        return SINGLE_TAP_ACTIONS[getPointArea(x, y)];
    case DOUBLE_TAP:
        return DOUBLE_TAP_ACTIONS[getPointArea(x, y)];
    }
    return Qt::Key_unknown;
}
