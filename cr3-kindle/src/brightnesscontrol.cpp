#include "brightnesscontrol.h"
#include "ui_brightnesscontrol.h"

// Kindle uses hardcoded table to convert from logarithmic scale to linear and vice versa.
// Driver operates raw levels, user interface shows "smooth" scale (0 - 24).
// Note that "0" value in Kindle UI corresponds to "1" raw driver value, hence no ability
// to turn off the light completely in the original UI, but this control can do it.

const IntList BrightnessControl::RAW_LEVELS = IntList()
        << 0  // Kindle returns 1 here and light remains, return 0 to turn it off
        << 2 << 3 << 4 << 5 << 6 << 7 << 8 << 9 << 10 << 13 << 19 << 26 << 36 << 47
        << 60 << 75 << 90 << 107 << 126 << 147 << 170 << 196 << 224 << 254 // 24
        ;

// Kindle Paperwhite 2 (2013) uses different driver and values scale
const IntList BrightnessControl::RAW_LEVELS_PW2 = IntList()
        << 0  // Kindle returns 1 here and light remains, return 0 to turn it off
        << 2 << 3 << 4 << 6 << 14 << 28 << 49 << 78 << 120 << 167 << 245 << 336 << 465
        << 607 << 775 << 969 << 1162 << 1382 << 1627 << 1898 << 2195 << 2531 << 2893 << 3280 // 24
        ;

int BrightnessControl::smoothToRaw(int level)
{
    if (level >= 0 && level < getLevels()->size()) {
        return getLevels()->at(level);
    } else {
        return 0; // should never happen
    }
}

int BrightnessControl::rawToSmooth(int level)
{
    if (getLevels()->contains(level)) {
        return getLevels()->indexOf(level);
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

    maxLevel = getLevels()->size() - 1;
    ui->progressBar->setMaximum(maxLevel);

    backlightFile = getBackLightFile();

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
    if (backlightFile) setRawLevel(smoothToRaw(value));
    else setSmoothLevel(value);
}

QFile *BrightnessControl::getBackLightFile()
{
    QFile *bl;
    if (Device::getModel() == Device::KPW2) {
        bl = new QFile(INTENSITY_DEVICE_PW2);
    } else {
        bl = new QFile(INTENSITY_DEVICE);
    }
    return bl;
}

const IntList *BrightnessControl::getLevels()
{
    if (Device::getModel() == Device::KPW2) {
        return &RAW_LEVELS_PW2;
    } else {
        return &RAW_LEVELS;
    }
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
    QFile *backRead = getBackLightFile();

    if (!backRead->open(QIODevice::ReadOnly)) {
        qDebug("Can't open backlight file");
        return 0;
    }
    QByteArray array = backRead->readLine();
    backRead->close();

    array.truncate(array.indexOf("\n"));
    QString output = QString(array);

    if (Device::getModel() == Device::KPW2) {
        return output.toInt();
    } else {
        return output.section(QLatin1Char('='), 1, 1).toInt();
    }
}

int BrightnessControl::getSmoothLevel()
{
    if (backlightFile) {
        return rawToSmooth(getRawLevel());
    } else {
        int level = 0;
        QStringList list;
        QProcess *myProcess = new QProcess();
        list << "com.lab126.powerd" << "flIntensity";
        myProcess->start("/usr/bin/lipc-get-prop", list);
        if (myProcess->waitForReadyRead(1000)) {
            QByteArray array = myProcess->readAll();
            array.truncate(array.indexOf("\n"));
            level = array.toInt();
        }
        myProcess->close();
        return level;
    }
}

// called on start, when returning from minimized state and from screensaver
void BrightnessControl::fixZeroLevel()
{
    if (getSmoothLevel() == 0) {
        qDebug("*** FIXED LIGHT ***");
        setRawLevel(0);
    }
}
