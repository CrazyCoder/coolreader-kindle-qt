#ifndef SETTINGSDLG_H
#define SETTINGSDLG_H

#include <QDialog>
#include <QCheckBox>
#include <QComboBox>
#include <QSpinBox>
#include <QWSEvent>
#include <QColorDialog>
#include <QStyleFactory>
#include <QDir>
#include <QMessageBox>
#include <QTranslator>

#include "crqtutil.h"
#include "cr3widget.h"

namespace Ui {
    class SettingsDlg;
}

#define PROP_APP_START_ACTION			"cr3.app.start.action"
#define PROP_WINDOW_LANG				"window.language"

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
    void on_TxtPreFormatted_toggled(bool checked);
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
};

#endif // SETTINGSDLG_H
