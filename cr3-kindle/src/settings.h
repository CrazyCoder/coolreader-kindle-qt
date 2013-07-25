#ifndef SETTINGSDLG_H
#define SETTINGSDLG_H

#include <QDialog>
#include <QCheckBox>
#include <QComboBox>
#include <QSpinBox>
#include <QPushButton>
#include <QWSEvent>
#include <QColorDialog>
#include <QStyleFactory>
#include <QDir>
#include <QMessageBox>
#include <QTranslator>
#include <QKeyEvent>
#include <QApplication>
#include <QStyledItemDelegate>

#include <device.h>

#include "crqtutil.h"
#include "cr3widget.h"

namespace Ui {
class SettingsDlg;
}

#define PROP_APP_START_ACTION      "cr3.app.start.action"
#define PROP_WINDOW_LANG           "window.language"
#define PROP_REPLACE_SCREENSAVER   "cr3.kindle.replace.screensaver"
#define PROP_LAST_TAB              "cr3.kindle.last.settings.tab"
#define PROP_UPDIR_ON_EVERY_PAGE   "cr3.kindle.updir.on.every.page"
#define PROP_HIDE_EXTENSION        "cr3.kindle.hide.extension"
#define PROP_KEEP_HISTORY_MINOR    "cr3.kindle.keep.history.on.minor.changes"
#define PROP_CYCLIC_LIST_PAGES     "cr3.kindle.cyclic.list.pages"

// Filter Home, PageUp and PageDown in SpinBoxes, allows to Save settings
// when focus is in widget and to change value on touch Kindles
// if up/down swipes are mapped to PageUp/PageDown (default in settings)
static bool spinKeyFilter(QAbstractSpinBox *sb, QKeyEvent *event) {
    switch(event->key()) {
    case Qt::Key_Home:
        QApplication::sendEvent(sb->parentWidget(), event);
        break;
    case Qt::Key_PageUp:
        sb->stepUp();
        break;
    case Qt::Key_PageDown:
        sb->stepDown();
        break;
    default:
        return false;
    }
    return true;
}

class FilteredSpinBox : public QSpinBox
{
public:
    FilteredSpinBox(QWidget *parent = 0) : QSpinBox(parent) { }
protected:
    void keyPressEvent(QKeyEvent *event)
    {
        if (!spinKeyFilter(this, event)) QAbstractSpinBox::keyPressEvent(event);
    }
};

class FilteredDoubleSpinBox : public QDoubleSpinBox
{
public:
    FilteredDoubleSpinBox(QWidget *parent = 0) : QDoubleSpinBox(parent) { }
protected:
    void keyPressEvent(QKeyEvent *event)
    {
        if (!spinKeyFilter(this, event)) QAbstractSpinBox::keyPressEvent(event);
    }
};

class CR3View;
class SettingsDlg : public QDialog {
    Q_OBJECT
    Q_DISABLE_COPY(SettingsDlg)
public:
    virtual ~SettingsDlg();
    static bool showDlg(QWidget * parent, CR3View * docView);
protected:
    explicit SettingsDlg(QWidget *parent, CR3View * docView );
    virtual void changeEvent(QEvent *e);

    void setCheck(const char * optionName, bool checkState);
    void optionToUi( const char * optionName, QCheckBox * cb );
    void setCheckInversed( const char * optionName, int checkState );
    void optionToUiInversed( const char * optionName, QCheckBox * cb );
    void fontToUi( const char * faceOptionName, const char * sizeOptionName, QComboBox * faceCombo, QSpinBox * sizeSpinBox, const char * defFontFace );
    void updateStyleSample();
private:
    Ui::SettingsDlg *m_ui;
    CR3View * m_docview;
    PropsRef m_props;

    void SwitchCBs(bool state);
private slots:
    void on_cbStartupAction_currentIndexChanged(int index);
    void on_cbHyphenation_currentIndexChanged(int index);
    void on_cbTextFontFace_currentIndexChanged(QString);
    void on_cbTitleFontFace_currentIndexChanged(QString);
    void on_cbViewMode_currentIndexChanged(int index);
    void on_cbLanguage_currentIndexChanged(const QString &arg1);

    void on_ShowFootNotes_toggled(bool checked);
    void on_ShowBattery_toggled(bool checked);
    void on_ShowBatteryInPercent_toggled(bool checked);
    void on_ShowClock_toggled(bool checked);
    void on_ShowBookName_toggled(bool checked);
    void on_ShowPageHeader_toggled(bool checked);
    void on_cbTxtPreFormatted_toggled(bool checked);
    void on_cbEnableEmbeddedFonts_toggled(bool checked);
    void on_cbEnableDocumentStyles_toggled(bool checked);
    void on_FloatingPunctuation_toggled(bool checked);
    void on_ChapterMarks_toggled(bool checked);
    void on_PositionPercent_toggled(bool checked);
    void on_PageNumber_toggled(bool checked);

    void on_sbFontGamma_valueChanged(double arg1);
    void on_sbTextFontSize_valueChanged(int arg1);
    void on_sbTitleFontSize_valueChanged(int arg1);
    void on_sbInterlineSpace_valueChanged(int arg1);

    void on_actionSaveSettings_triggered();
    void on_actionNextTab_triggered();
    void on_actionPrevTab_triggered();
    void on_cbMarginSide_currentIndexChanged(int index);
    void on_sbMargin_valueChanged(int arg1);
    void on_cbFullUpdateEvery_currentIndexChanged(const QString &arg1);
    void on_cbEmbolden_toggled(bool checked);
    void on_cbKerning_toggled(bool checked);
    void on_cbAA_currentIndexChanged(int index);
    void on_cbHinting_currentIndexChanged(int index);
    void on_sbSpaceCond_valueChanged(int arg1);
    void on_cbCoverScreensaver_toggled(bool checked);
    void on_buttonBox_accepted();
    void on_buttonBox_rejected();
    void on_tabWidget_currentChanged(int index);
};

#endif // SETTINGSDLG_H
