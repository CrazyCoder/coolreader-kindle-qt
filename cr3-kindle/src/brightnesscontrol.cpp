#include "brightnesscontrol.h"
#include "ui_brightnesscontrol.h"

// Kindle uses hardcoded table to convert from logarithmic scale to linear and vice versa.
// Driver operates raw levels, user interface shows "smooth" scale (0 - 24).
// Note that "0" value in Kindle UI corresponds to "1" raw driver value, hence no ability
// to turn off the light completely in the original UI, but this control can do it.

const IntList BrightnessControl::RAW_LEVELS = IntList()
        << 0  // Kindle returns 1 here and light remains, return 0 to turn it off
        << 2
        << 3
        << 4
        << 5
        << 6
        << 7
        << 8
        << 9
        << 10
        << 13
        << 19
        << 26
        << 36
        << 47
        << 60
        << 75
        << 90
        << 107
        << 126
        << 147
        << 170
        << 196
        << 224
        << 254 // 24
        ;

int BrightnessControl::smoothToRaw(int level)
{
    if (level >= 0 && level < RAW_LEVELS.size()) {
        return RAW_LEVELS.at(level);
    } else {
        return 0; // should never happen
    }
}

int BrightnessControl::rawToSmooth(int level)
{
    if (RAW_LEVELS.contains(level)) {
        return RAW_LEVELS.indexOf(level);
    } else {
        return 0; // if raw is set to 1 (kindle level 0), return 0 to turn it off
    }
}

BrightnessControl::BrightnessControl(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::BrightnessControl)
{
    initDone = false;

    setWindowFlags(Qt::Popup);

    ui->setupUi(this);

    maxLevel = RAW_LEVELS.size() - 1;
    ui->progressBar->setMaximum(maxLevel);

    backlightFile = new QFile(INTENSITY_DEVICE);

    if (!backlightFile->open(QIODevice::WriteOnly)) {
        qDebug("Backlight intensity file could not been opened.");
        backlightFile = 0;
    }
    initDone = true;
    isActive = false;
}

BrightnessControl::~BrightnessControl()
{
    if (backlightFile) backlightFile->close();
    initDone = false;
    delete ui;
}

bool BrightnessControl::eventFilter(QObject *object, QEvent *event)
{
    if (!initDone || !isVisible()) return false;

    QPoint p;
    if (event->type() == QEvent::MouseMove || event->type() == QEvent::MouseButtonPress) {
        p = ui->progressBar->mapFromGlobal(QCursor::pos());
        // qDebug("pos: %d, %d", p.y(), event->type());

        QPoint dp = this->mapFromGlobal(QCursor::pos()); // close dialog when clicked outside
        if (dp.x() < 0 || dp.y() < 0 || dp.x() > width() || dp.y() > height()) {
            saveLevel();
            close();
            return true;
        }

        maxHeight = ui->progressBar->height();
        int val = 0;
        int y = maxHeight - p.y();

        if (y < 0) val = 0;
        else if (y > maxHeight) val = maxLevel;
        else val = (maxLevel * y) / maxHeight;

        ui->progressBar->setValue(val);
        return false;
    }

    return false;
}

void BrightnessControl::showEvent(QShowEvent *)
{
    if (!isActive) {
        isActive = true;
        installEventFilter(this);
        grabMouse();
        ui->progressBar->setFocus();
        ui->progressBar->setValue(getSmoothLevel());
    }
}

void BrightnessControl::hideEvent(QHideEvent *)
{
    if (isActive) {
        isActive = false;
        removeEventFilter(this);
        releaseMouse();
    }
}

void BrightnessControl::on_btnPlus_pressed()
{
    ui->progressBar->setValue(ui->progressBar->value() + 1);
}

void BrightnessControl::on_btnMinus_pressed()
{
    ui->progressBar->setValue(ui->progressBar->value() - 1);
}

void BrightnessControl::on_progressBar_valueChanged(int value)
{
    if (!initDone) return;
    // qDebug("value: %d=>%d", value, smoothToRaw(value));
    setRawLevel(smoothToRaw(value));
}

void BrightnessControl::saveLevel()
{
    int smoothLevel = ui->progressBar->value();
    // save the value so that it's restored by framework when turning on
    setSmoothLevel(smoothLevel);
    // override Kindle zero level and turn off the light completely
    if (smoothLevel == 0) {
        setRawLevel(0);
    }
}

// quickly set raw level (doesn't survive on/off)
void BrightnessControl::setRawLevel(int level)
{
    if (backlightFile) {
        backlightFile->write(QString::number(level).toAscii());
        backlightFile->flush();
    }
}

// store level in framework properties so that it survives on/off cycle
void BrightnessControl::setSmoothLevel(int level)
{
    QProcess::execute(QString("lipc-set-prop com.lab126.powerd flIntensity %1").arg(level));
}

int BrightnessControl::getRawLevel()
{
    QFile backRead(INTENSITY_DEVICE);
    if (!backRead.open(QIODevice::ReadOnly)) return 0;
    QByteArray array = backRead.readLine();
    backRead.close();

    array.truncate(array.indexOf("\n"));
    QString output = QString(array);
    return output.section(QLatin1Char('='), 1, 1).toInt();
}

int BrightnessControl::getSmoothLevel()
{
    return rawToSmooth(getRawLevel());
}

// called on start, when returning from minimized state and from screensaver
void BrightnessControl::fixZeroLevel()
{
    if (getSmoothLevel() == 0) {
        qDebug("*** FIXED LIGHT ***");
        setRawLevel(0);
    }
}
