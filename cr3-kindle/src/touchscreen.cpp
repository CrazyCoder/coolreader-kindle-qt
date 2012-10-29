#include "touchscreen.h"

const Qt::Key TouchScreen::TAP_ACTIONS[][7] = {
    { // single tap when reading
      Qt::Key_Menu, Qt::Key_BrightnessAdjust, Qt::Key_Menu,
      Qt::Key_PageUp, Qt::Key_PageDown, Qt::Key_PageDown,
      Qt::Key_PageDown
    },
    { // two finger tap
      Qt::Key_Escape, Qt::Key_Home, Qt::Key_Select,
      Qt::Key_Up, Qt::Key_O, Qt::Key_Down,
      Qt::Key_Menu
    },
    { // two finger tap while reading
      Qt::Key_Escape, Qt::Key_Home, Qt::Key_Select,
      Qt::Key_Up, Qt::Key_O, Qt::Key_Down,
      Qt::Key_Menu
    },
    { // long tap
      Qt::Key_Select, Qt::Key_Select, Qt::Key_Select,
      Qt::Key_Select, Qt::Key_Select, Qt::Key_Select,
      Qt::Key_Select
    },
    { // long tap while reading
      Qt::Key_Minus, Qt::Key_Select, Qt::Key_Plus,
      Qt::Key_unknown, Qt::Key_O, Qt::Key_unknown,
      Qt::Key_Menu
    }
};

const Qt::Key TouchScreen::SWIPE_ACTIONS[][4] = { // UP / RIGHT / DOWN / LEFT
    { // swipe while not in reader
      Qt::Key_PageDown, Qt::Key_Home, Qt::Key_PageUp, Qt::Key_Escape
    },
    { // swipe when in reader
      Qt::Key_PageDown, Qt::Key_PageUp, Qt::Key_PageUp, Qt::Key_PageDown
    }
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

    buttonState = 0;
    newButtonState = 0;
    lastEvent = 0;
    isFocusInReader = true;
    oldX = 0;
    oldY = 0;
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
    return TAP_ACTIONS[t][getPointArea(x, y)];
}

Qt::Key TouchScreen::getSwipeAction(int x, int y, int oldX, int oldY, TouchScreen::SwipeType t)
{
    if (!isGesture(x, y, oldX, oldY)) {
        return Qt::Key_unknown; // too short swipe
    }

    SwipeGesture g = SWIPE_UNRECOGNIZED;

    bool isHorizontal = abs(x - oldX) > abs(y - oldY);

    if (isHorizontal) {
        if (oldX > x) {
            g = SWIPE_LEFT;
        } else {
            g = SWIPE_RIGHT;
        }
    } else { // vertical
        if (y > oldY) {
            g = SWIPE_DOWN;
        } else {
            g = SWIPE_UP;
        }
    }

    return g == SWIPE_UNRECOGNIZED ? Qt::Key_unknown : SWIPE_ACTIONS[t][g];
}

bool TouchScreen::isGesture(int x, int y, int oldX, int oldY)
{
    return (abs(y - oldY) > MIN_SWIPE_PIXELS || abs(x - oldX) > MIN_SWIPE_PIXELS);
}

bool TouchScreen::filter(QWSMouseEvent *pme, bool focusInReader)
{
    newButtonState = pme->simpleData.state;

    // save focus state when button was pressed
    if (newButtonState > 0) {
        isFocusInReader = focusInReader;
        oldX = pme->simpleData.x_root;
        oldY = pme->simpleData.y_root;
    }

    qDebug("mouse: x: %d, y: %d, state: %d, time: %d, %d", pme->simpleData.x_root, pme->simpleData.y_root, pme->simpleData.state, pme->simpleData.time - lastEvent, isFocusInReader);

    // touch released
    if (newButtonState == 0 && buttonState != 0) {
        int x = pme->simpleData.x_root;
        int y = pme->simpleData.y_root;

        TouchType t = TAP_SINGLE;
        if (buttonState == Qt::RightButton) {
            t = isFocusInReader ? TAP_TWO_READER : TAP_TWO;
        }

        bool isSingleFinger = buttonState == Qt::LeftButton;
        bool isGesture = this->isGesture(x, y, oldX, oldY) && isSingleFinger;
        bool isLongTap = lastEvent != 0 && pme->simpleData.time - lastEvent > LONG_TAP_INTERVAL && !isGesture && isSingleFinger;

        if (isSingleFinger && !isGesture && !isLongTap && !isFocusInReader) return false; // handle custom single tap actions only when in reader

        Qt::Key key = Qt::Key_unknown;

        if (isGesture) {
            qDebug("* gesture detected");
            SwipeType st = isFocusInReader ? SWIPE_ONE_READER : SWIPE_ONE;
            key = getSwipeAction(pme->simpleData.x_root, pme->simpleData.y_root, oldX, oldY, st);
        } else {
            if (isLongTap) t = isFocusInReader ? TAP_LONG_READER : TAP_LONG;
            key = getAreaAction(pme->simpleData.x_root, pme->simpleData.y_root, t);
        }

        if (key != Qt::Key_unknown) {
            QWSServer::sendKeyEvent(-1, key, Qt::NoModifier, true, false);
            QWSServer::sendKeyEvent(-1, key, Qt::NoModifier, false, false);
            buttonState = newButtonState;
            return true;
        } else {
            qDebug("-- area action not defined");
        }
    }
    lastEvent = pme->simpleData.time;
    buttonState = newButtonState;
    return false;
}
