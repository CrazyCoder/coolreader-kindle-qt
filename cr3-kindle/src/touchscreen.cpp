#include "touchscreen.h"

Qt::Key TouchScreen::TAP_ACTIONS[][TOUCH_ZONES] = {
    { // single tap when reading
      Qt::Key_Menu, Qt::Key_BrightnessAdjust, Qt::Key_Menu,
      Qt::Key_PageUp, Qt::Key_PageDown, Qt::Key_PageDown,
      Qt::Key_PageUp, Qt::Key_PageDown, Qt::Key_PageDown
    },
    { // two finger tap
      Qt::Key_Escape, Qt::Key_Home, Qt::Key_Select,
      Qt::Key_Up, Qt::Key_O, Qt::Key_Down,
      Qt::Key_Up, Qt::Key_Menu, Qt::Key_Down
    },
    { // two finger tap while reading
      Qt::Key_Escape, Qt::Key_Home, Qt::Key_Select,
      Qt::Key_Up, Qt::Key_O, Qt::Key_Down,
      Qt::Key_Up, Qt::Key_Menu, Qt::Key_Down
    },
    { // long tap
      Qt::Key_Select, Qt::Key_Select, Qt::Key_Select,
      Qt::Key_Select, Qt::Key_Select, Qt::Key_Select,
      Qt::Key_Select, Qt::Key_Select, Qt::Key_Select
    },
    { // long tap while reading
      Qt::Key_Minus, Qt::Key_Select, Qt::Key_Plus,
      Qt::Key_unknown, Qt::Key_O, Qt::Key_unknown,
      Qt::Key_unknown, Qt::Key_Menu, Qt::Key_unknown
    }
};

Qt::Key TouchScreen::SWIPE_ACTIONS[][SWIPE_TYPES] = { // UP / RIGHT / DOWN / LEFT
    { // swipe while not in reader
      Qt::Key_PageDown, Qt::Key_Home, Qt::Key_PageUp, Qt::Key_Escape
    },
    { // swipe when in reader
      Qt::Key_PageDown, Qt::Key_PageUp, Qt::Key_PageUp, Qt::Key_PageDown
    }
};

template <typename T, size_t N>
inline
int ARRAY_SIZE( const T(&)[ N ] )
{
  return N;
}

TouchScreen::TouchScreen()
{
    longTapTimer = new QTimer(this);
    longTapTimer->setSingleShot(true);
    connect(longTapTimer, SIGNAL(timeout()), this, SLOT(longTap()));

    loadConfiguration();

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

    wasFocusInReader = true;
    isGestureEnabled = true;
    wasGestureEnabled = true;
    isLongTapHandled = false;

    oldX = 0;
    oldY = 0;
}

TouchScreen::Area TouchScreen::getPointArea(int x, int y)
{
    if (y < tpx) {
        if (x < lpx) return TOP_LEFT;
        else if (x >= w - rpx) return TOP_RIGHT;
        else return TOP_MIDDLE;
    } else if (y >= tpx && y < h - bpx) {
        if (x < lpx) return MIDDLE_LEFT;
        else if (x >= w - rpx) return MIDDLE_RIGHT;
        else return CENTER;
    } else {
        if (x < lpx) return BOTTOM_LEFT;
        else if (x >= w - rpx) return BOTTOM_RIGHT;
        else return BOTTOM_MIDDLE;
    }
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
    int x = pme->simpleData.x_root;
    int y = pme->simpleData.y_root;

    // save focus state when button was pressed
    if (newButtonState > 0) {
        // start gesture tracking
        if (buttonState == 0) {
            wasFocusInReader = focusInReader;
            oldX = x;
            oldY = y;
            wasGestureEnabled = isGestureEnabled;
            // start timer for the new long tap
            if (newButtonState == Qt::LeftButton)  {
                longTapTimer->start(LONG_TAP_INTERVAL);
            }
        } else {
            // cancel long tap on position change (swipe)
            longTapTimer->stop();
        }
    }

    // filter events from UI until finger is released, but don't filter if gestures are disabled
    bool isFiltered = wasGestureEnabled && newButtonState > 0 && buttonState == newButtonState;

    qDebug("mouse: x: %d, y: %d, state: %d, time: %d, focus: %d", x, y, newButtonState, pme->simpleData.time - lastEvent, wasFocusInReader);

    // touch released
    if (newButtonState == 0 && buttonState != 0) {
        longTapTimer->stop(); // cancel long tap timer when touch is released

        TouchType t = TAP_SINGLE;
        if (buttonState == Qt::RightButton) {
            t = wasFocusInReader ? TAP_TWO_READER : TAP_TWO;
        }

        bool isSingleFinger = buttonState == Qt::LeftButton;
        bool isGesture = wasGestureEnabled && this->isGesture(x, y, oldX, oldY) && isSingleFinger;

        // let Qt handle touch events outside of the reader
        bool isHandled = isSingleFinger && !isGesture && !wasFocusInReader;
        // long taps are handled via timer
        if (isLongTapHandled) {
            isHandled = true;
            isLongTapHandled = false;
        }

        Qt::Key key = Qt::Key_unknown;

        if (!isHandled) {
            if (isGesture) {
                qDebug("* gesture detected");
                SwipeType st = wasFocusInReader ? SWIPE_ONE_READER : SWIPE_ONE;
                key = getSwipeAction(x, y, oldX, oldY, st);
            } else {
                key = getAreaAction(x, y, t);
            }

            if (key != Qt::Key_unknown) {
                QWSServer::sendKeyEvent(-1, key, Qt::NoModifier, true, false);
                QWSServer::sendKeyEvent(-1, key, Qt::NoModifier, false, false);
                buttonState = newButtonState;
                lastEvent = pme->simpleData.time;
                return true;
            } else {
                qDebug("-- area action not defined");
            }
        }
    }
    lastEvent = pme->simpleData.time;
    buttonState = newButtonState;

    return isFiltered;
}

bool TouchScreen::enableGesture(bool enable)
{
    bool oldValue = isGestureEnabled;
    isGestureEnabled = enable;
    return oldValue;
}

void TouchScreen::loadConfiguration()
{
    QSettings settings(TOUCH_CONFIG, QSettings::IniFormat);
    const QStringList groups = settings.childGroups();
    foreach (const QString &group, groups) {
        bool ok, isSwipe = false;
        int g;
        if (group.startsWith('s')) {
            isSwipe = true;
            g = group.right(group.length() - 1).toInt(&ok);
        } else {
            g = group.toInt(&ok);
        }
        if (!ok || g < 0) continue;

        settings.beginGroup(group);
        const QStringList keys = settings.childKeys();
        foreach (const QString &key, keys) {
            bool kok, vok;
            int k = key.toInt(&kok);
            if (!kok || k < 0) continue;

            QString val = settings.value(key).toString();
            Qt::Key cmd = static_cast<Qt::Key>(val.toInt(&vok, 16));
            if (!vok) continue;

            if (isSwipe && g < ARRAY_SIZE(SWIPE_ACTIONS) && k < ARRAY_SIZE(SWIPE_ACTIONS[0])) {
                SWIPE_ACTIONS[g][k] = cmd;
                qDebug("&swipe[%d][%d]=%x", g, k, cmd);
            } else if (g < ARRAY_SIZE(TAP_ACTIONS) && k < ARRAY_SIZE(TAP_ACTIONS[0])) {
                TAP_ACTIONS[g][k] = cmd;
                qDebug("&key[%d][%d]=%x", g, k, cmd);
            } else {
                qDebug("&unknown[%d][%d]=%x", g, k, cmd);
            }
        }
        settings.endGroup();
    }
}

void TouchScreen::longTap()
{
    qDebug("** long tap");
    Qt::Key key = getAreaAction(oldX, oldY, wasFocusInReader ? TAP_LONG_READER : TAP_LONG);
    if (key != Qt::Key_unknown) {
        QWSServer::sendKeyEvent(-1, key, Qt::NoModifier, true, false);
        QWSServer::sendKeyEvent(-1, key, Qt::NoModifier, false, false);
    } else {
        qDebug("-- area action not defined for long tap");
    }
    isLongTapHandled = true;
}
