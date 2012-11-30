#ifndef TOUCHSCREEN_H
#define TOUCHSCREEN_H

#include <QObject>
#include <QTimer>
#include <QSettings>

#include <device.h>

#define MIN_SWIPE         10 // % of screen width
#define LONG_TAP_ZONE     7
#define LONG_TAP_INTERVAL 500
#define TOUCH_CONFIG      "data/touch.ini"

#define TOUCH_ZONES 9
#define SWIPE_TYPES 4

class TouchScreen: public QObject
{
    Q_OBJECT

public:
    TouchScreen();

    typedef enum TOUCH_TYPE {
        TAP_SINGLE = 0,
        TAP_TWO,
        TAP_TWO_READER,
        TAP_LONG,
        TAP_LONG_READER
    } TouchType;

    typedef enum SWIPE_TYPE {
        SWIPE_ONE = 0,
        SWIPE_ONE_READER
    } SwipeType;

    typedef enum SWIPE_GESTURE {
        SWIPE_UP = 0,
        SWIPE_RIGHT,
        SWIPE_DOWN,
        SWIPE_LEFT,
        SWIPE_UNRECOGNIZED = 0x01ffffff
    } SwipeGesture;

    typedef enum AREA { // left to right, top to bootom
        TOP_LEFT = 0,
        TOP_MIDDLE,
        TOP_RIGHT,
        MIDDLE_LEFT,
        CENTER,
        MIDDLE_RIGHT,
        BOTTOM_LEFT,
        BOTTOM_MIDDLE,
        BOTTOM_RIGHT
    } Area;

    Area getPointArea(int x, int y);
    Qt::Key getAreaAction(int x, int y, TouchType t);
    Qt::Key getSwipeAction(int x, int y, int oldX, int oldY, SwipeType t);
    bool isGesture(int x, int y, int oldX, int oldY);
    bool filter(QWSMouseEvent* pme, bool focusInReader);
    bool enableGesture(bool enable);
    void loadConfiguration();

private:
    bool isGestureEnabled, wasGestureEnabled;
    bool wasFocusInReader;
    bool isLongTapHandled;
    bool isSimulatedClick;

    int RIGHT_MARGIN, rpx;
    int LEFT_MARGIN, lpx;
    int TOP_MARGIN, tpx;
    int BOTTOM_MARGIN, bpx;

    int buttonState;
    int newButtonState;
    int lastEvent;
    int oldX;
    int oldY;

    int w;
    int h;

    static Qt::Key TAP_ACTIONS[][TOUCH_ZONES];
    static Qt::Key SWIPE_ACTIONS[][SWIPE_TYPES];

    int MIN_SWIPE_PIXELS, LONG_TAP_ZONE_PIXELS;

    QTimer *longTapTimer;
private slots:
    void longTap();
};

#endif // TOUCHSCREEN_H
