#ifndef BRIGHTNESSCONTROL_H
#define BRIGHTNESSCONTROL_H

#include <QDialog>
#include <QAbstractButton>
#include <QFile>
#include <QProcess>
#include <QDebug>
#include <device.h>

#define INTENSITY_DEVICE     "/sys/devices/system/fl_tps6116x/fl_tps6116x0/fl_intensity"
#define INTENSITY_DEVICE_PW2 "/sys/class/backlight/max77696-bl/brightness"

typedef QList<int> IntList;

namespace Ui {
class BrightnessControl;
}

class BrightnessControl : public QDialog
{
    Q_OBJECT

public:
    explicit BrightnessControl(QWidget *parent = 0);
    ~BrightnessControl();
    static const IntList RAW_LEVELS;
    static const IntList RAW_LEVELS_PW2;

    static int smoothToRaw(int level);
    static int rawToSmooth(int level);
    static const IntList *getLevels();

    void setRawLevel(int level);
    void setSmoothLevel(int level);

    int getRawLevel();
    int getSmoothLevel();

    void fixZeroLevel();
protected:
    bool eventFilter(QObject *object, QEvent *event);
    void showEvent(QShowEvent *);
    void hideEvent(QHideEvent *);
private slots:
    void on_btnPlus_pressed();
    void on_btnMinus_pressed();
    void on_progressBar_valueChanged(int value);
private:
    int maxHeight, maxLevel;
    Ui::BrightnessControl *ui;
    bool initDone, isActive;
    QFile *backlightFile;
    QFile *getBackLightFile();

    void saveLevel();
};

#endif // BRIGHTNESSCONTROL_H
