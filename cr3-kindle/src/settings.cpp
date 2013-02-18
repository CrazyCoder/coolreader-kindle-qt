#include "settings.h"
#include "ui_settings.h"

static bool initDone = false;

static int cr_interline_spaces[] = { 70, 75, 80, 85, 90, 95, 100, 105, 110, 115, 120, 125, 130, 135, 140, 145, 150 };
static int def_margin[] = { 0, 1, 2, 3, 4, 5, 8, 10, 12, 14, 15, 16, 20, 25, 30 };

SettingsDlg::SettingsDlg(QWidget *parent, CR3View * docView ) :
    QDialog(parent),
    m_ui(new Ui::SettingsDlg),
    m_docview( docView )
{
    initDone = false;

    m_ui->setupUi(this);

    addAction(m_ui->actionSaveSettings);

    if (!Device::isTouch()) { // don't use PageUp/PageDown for tab switching on touch devices (mapped to spinboxes)
        addAction(m_ui->actionNextTab);
        addAction(m_ui->actionPrevTab);
    }

    m_props = m_docview->getOptions();
    optionToUi(PROP_FOOTNOTES, m_ui->ShowFootNotes);
    optionToUi(PROP_SHOW_BATTERY, m_ui->ShowBattery);
    optionToUi(PROP_SHOW_BATTERY_PERCENT, m_ui->ShowBatteryInPercent);
    optionToUi(PROP_SHOW_TIME, m_ui->ShowClock);
    optionToUi(PROP_SHOW_TITLE, m_ui->ShowBookName);
    optionToUi(PROP_TXT_OPTION_PREFORMATTED, m_ui->cbTxtPreFormatted);
    optionToUi(PROP_EMBEDDED_STYLES, m_ui->cbEnableDocumentStyles);
    optionToUi(PROP_EMBEDDED_FONTS, m_ui->cbEnableEmbeddedFonts);
    m_ui->cbEnableEmbeddedFonts->setEnabled(m_props->getIntDef(PROP_EMBEDDED_STYLES, 1) == 1 ? Qt::Checked : Qt::Unchecked);
    optionToUi(PROP_FLOATING_PUNCTUATION, m_ui->FloatingPunctuation);
    optionToUi(PROP_STATUS_CHAPTER_MARKS, m_ui->ChapterMarks);
    optionToUi(PROP_SHOW_POS_PERCENT, m_ui->PositionPercent);
    optionToUi(PROP_FONT_WEIGHT_EMBOLDEN, m_ui->cbEmbolden);
    optionToUi(PROP_FONT_KERNING_ENABLED, m_ui->cbKerning);
    optionToUi(PROP_REPLACE_SCREENSAVER, m_ui->cbCoverScreensaver);

    int state1 = m_props->getIntDef(PROP_SHOW_PAGE_NUMBER, 1);
    int state2 = m_props->getIntDef(PROP_SHOW_PAGE_COUNT, 1);
    m_ui->PageNumber->setCheckState(state1==1 || state2==1? Qt::Checked : Qt::Unchecked);

    optionToUiInversed(PROP_STATUS_LINE, m_ui->ShowPageHeader);
    bool checked = !(m_props->getIntDef(PROP_STATUS_LINE, 1)==1);
    SwitchCBs(checked);

    m_ui->cbStartupAction->setCurrentIndex(m_props->getIntDef(PROP_APP_START_ACTION, 0 ));

    int lp = m_props->getIntDef(PROP_LANDSCAPE_PAGES, 2);
    m_ui->cbViewMode->setCurrentIndex(lp==1 ? 0 : 1);

    on_cbMarginSide_currentIndexChanged(0);

    QStringList faceList;
    crGetFontFaceList(faceList);

    // fonts that are known to cause problems
    QStringList blackListFonts = QStringList() << "HYGothic" << "HYMyeongJo" << "MYing Hei T" << "Song T";
    QString toRemove;
    foreach (toRemove, blackListFonts) {
        faceList.removeAll(toRemove);
    }

    m_ui->cbTextFontFace->addItems(faceList);
    m_ui->cbTitleFontFace->addItems(faceList);

    m_ui->sbTextFontSize->setMinimum(MIN_CR_FONT_SIZE);
    m_ui->sbTextFontSize->setMaximum(MAX_CR_FONT_SIZE);
    m_ui->sbTitleFontSize->setMinimum(MIN_CR_HEADER_FONT_SIZE);
    m_ui->sbTitleFontSize->setMaximum(MAX_CR_HEADER_FONT_SIZE);

    const char * defFontFace = "Arial";
    static const char * goodFonts[] = { "Arial", "Verdana", NULL };
    for(int i=0; goodFonts[i]; i++) {
        if(faceList.indexOf(QString(goodFonts[i]))>=0) {
            defFontFace = goodFonts[i];
            break;
        }
    }

    fontToUi(PROP_FONT_FACE, PROP_FONT_SIZE, m_ui->cbTextFontFace, m_ui->sbTextFontSize, defFontFace);
    fontToUi(PROP_STATUS_FONT_FACE, PROP_STATUS_FONT_SIZE, m_ui->cbTitleFontFace, m_ui->sbTitleFontSize, defFontFace);

    m_ui->sbInterlineSpace->setValue(m_props->getIntDef(PROP_INTERLINE_SPACE, 100));

    m_ui->sbSpaceCond->setValue(m_props->getIntDef(PROP_FORMAT_MIN_SPACE_CONDENSING_PERCENT, DEF_MIN_SPACE_CONDENSING_PERCENT));
    m_ui->cbAA->setCurrentIndex(m_props->getIntDef(PROP_FONT_ANTIALIASING, font_aa_all));
    m_ui->cbHinting->setCurrentIndex(m_props->getIntDef(PROP_FONT_HINTING, HINTING_MODE_AUTOHINT));

    int updateEvery = m_props->getIntDef(PROP_DISPLAY_FULL_UPDATE_INTERVAL, 1);
    int upIndex = m_ui->cbFullUpdateEvery->findText(QString::number(updateEvery));
    m_ui->cbFullUpdateEvery->setCurrentIndex(upIndex != -1 ? upIndex : 1);

    // translations
    QString v = m_props->getStringDef(PROP_WINDOW_LANG, "English");
    QDir Dir(qApp->applicationDirPath() + QDir::toNativeSeparators(QString("/data/i18n")));
    QStringList filter;
    filter << "*.qm";
    QStringList files = Dir.entryList(filter, QDir::Files, QDir::Name);
    m_ui->cbLanguage->addItem("English");
    foreach(const QString &str, files) {
        if (str.contains("_")) continue; // ignore qt_ translations
        QString s = str;
        s.chop(3);
        m_ui->cbLanguage->addItem(s);
    }

    int ind = m_ui->cbLanguage->findText(v);
    m_ui->cbLanguage->setCurrentIndex(ind != -1 ? ind : 1);

    int hi = -1;
    v = m_props->getStringDef(PROP_HYPHENATION_DICT,"@algorithm");
    for ( int i=0; i<HyphMan::getDictList()->length(); i++ ) {
        HyphDictionary * item = HyphMan::getDictList()->get( i );
        if (v == cr2qt(item->getFilename() ) ||  v == cr2qt(item->getId() ))
            hi = i;
        QString s = cr2qt( item->getTitle() );
        if( item->getType()==HDT_NONE)
            s = tr("[No hyphenation]");
        else if( item->getType()==HDT_ALGORITHM)
            s = tr("[Algorythmic hyphenation]");
        m_ui->cbHyphenation->addItem(s);
    }
    m_ui->cbHyphenation->setCurrentIndex(hi>=0 ? hi : 1);

    double fgamma = m_props->getDoubleDef(PROP_FONT_GAMMA, "1");
    m_ui->sbFontGamma->setValue(fgamma);

    lString16 testPhrase = qt2cr(tr("The quick brown fox jumps over the lazy dog. "));
    m_ui->crSample->getDocView()->createDefaultDocument(lString16(), testPhrase+testPhrase+testPhrase+testPhrase+testPhrase+testPhrase);

    updateStyleSample();

    // required for QComboBox QAbstractItemView::item to work in .qss
    QStyledItemDelegate* itd = new QStyledItemDelegate();
    QList<QComboBox*> list = this->findChildren<QComboBox *>();
    foreach(QComboBox *w, list) {
        w->setItemDelegate(itd);
    }
    // for translations
    m_ui->buttonBox->button(QDialogButtonBox::Save)->setText(tr("Save"));
    m_ui->buttonBox->button(QDialogButtonBox::Cancel)->setText(tr("Cancel"));
    // restore last open tab
    m_ui->tabWidget->setCurrentIndex(m_props->getIntDef(PROP_LAST_TAB, 0));

    initDone = true;
}

SettingsDlg::~SettingsDlg()
{
    delete m_ui;
}

bool SettingsDlg::showDlg(QWidget * parent, CR3View * docView)
{
    SettingsDlg * dlg = new SettingsDlg(parent, docView);
    dlg->showMaximized();
    return true;
}

void SettingsDlg::changeEvent(QEvent *e)
{
    switch (e->type()) {
    case QEvent::LanguageChange:
        m_ui->retranslateUi(this);
        break;
    default:
        break;
    }
}

void SettingsDlg::optionToUi( const char * optionName, QCheckBox * cb )
{
    int state = m_props->getIntDef(optionName, 1);
    cb->setCheckState(state==1 ? Qt::Checked : Qt::Unchecked);
}

void SettingsDlg::optionToUiInversed( const char * optionName, QCheckBox * cb )
{
    int state = m_props->getIntDef(optionName, 1);
    cb->setCheckState(state==1 ? Qt::Unchecked : Qt::Checked);
}

void SettingsDlg::setCheck(const char * optionName, bool checkState) {
    m_props->setInt(optionName, checkState);
}

void SettingsDlg::on_cbViewMode_currentIndexChanged(int index)
{
    if(!initDone) return;
    m_props->setInt(PROP_LANDSCAPE_PAGES, index + 1);
    m_props->setInt(PROP_PAGE_VIEW_MODE, 1);
}

void SettingsDlg::on_ShowPageHeader_toggled(bool checked)
{
    if(!initDone) return;
    setCheck(PROP_STATUS_LINE, !checked);
    SwitchCBs(checked);
}

void SettingsDlg::on_ShowBookName_toggled(bool checked)
{
    if(!initDone) return;
    setCheck(PROP_SHOW_TITLE, checked);
}

void SettingsDlg::on_ShowClock_toggled(bool checked)
{
    if(!initDone) return;
    setCheck(PROP_SHOW_TIME, checked);
}

void SettingsDlg::on_ShowBattery_toggled(bool checked)
{
    if(!initDone) return;
    setCheck(PROP_SHOW_BATTERY, checked);
}

void SettingsDlg::on_ShowBatteryInPercent_toggled(bool checked)
{
    if(!initDone) return;
    setCheck(PROP_SHOW_BATTERY_PERCENT, checked);
}

void SettingsDlg::on_ShowFootNotes_toggled(bool checked)
{
    if(!initDone) return;
    setCheck(PROP_FOOTNOTES, checked);
}

void SettingsDlg::updateStyleSample()
{
    if (initDone) Device::forceFullScreenUpdate();

    int i;
    m_props->getInt(PROP_FONT_SIZE, i);
    m_ui->crSample->propsApply(m_props);

    LVDocView *dv = m_ui->crSample->getDocView();
    dv->setShowCover(false);
    dv->setViewMode(DVM_SCROLL, 1);
    dv->getPageImage(0);
}

void SettingsDlg::on_cbTitleFontFace_currentIndexChanged(QString s)
{
    if(!initDone) return;
    m_props->setString(PROP_STATUS_FONT_FACE, s);
}

void SettingsDlg::on_cbTextFontFace_currentIndexChanged(QString s)
{
    if(!initDone) return;
    m_props->setString(PROP_FONT_FACE, s);
    updateStyleSample();
}

void SettingsDlg::fontToUi( const char * faceOptionName, const char * sizeOptionName, QComboBox * faceCombo, QSpinBox * sizeSpinBox, const char * defFontFace )
{
    QString faceName = m_props->getStringDef(faceOptionName, defFontFace);
    int faceIndex = faceCombo->findText(faceName);
    if(faceIndex>=0) faceCombo->setCurrentIndex(faceIndex);

    int size;
    m_props->getInt(sizeOptionName, size);
    sizeSpinBox->setValue(size);
}

void SettingsDlg::on_cbHyphenation_currentIndexChanged(int index)
{
    if(!initDone) return;
    const QStringList & dl(m_docview->getHyphDicts());
    QString s = dl[index < dl.count() ? index : 1];
    m_props->setString(PROP_HYPHENATION_DICT, s);
}

void SettingsDlg::on_cbStartupAction_currentIndexChanged(int index)
{
    if(!initDone) return;
    m_props->setInt(PROP_APP_START_ACTION, index);
}

void SettingsDlg::on_cbLanguage_currentIndexChanged(const QString &arg1)
{
    if(!initDone) return;
    m_props->setString(PROP_WINDOW_LANG, arg1);
}

void SettingsDlg::on_sbFontGamma_valueChanged(double arg1)
{
    if(!initDone) return;
    m_props->setString(PROP_FONT_GAMMA, QString::number(arg1));
    updateStyleSample();
}

void SettingsDlg::on_actionSaveSettings_triggered()
{
#ifndef i386
    Device::setFullScreenUpdateEvery(m_props->getIntDef(PROP_DISPLAY_FULL_UPDATE_INTERVAL, 1));
#endif

    m_docview->setOptions(m_props);
    close();
}

void SettingsDlg::on_actionNextTab_triggered()
{
    int CurrInd = m_ui->tabWidget->currentIndex();
    if(CurrInd+1 == m_ui->tabWidget->count())
        m_ui->tabWidget->setCurrentIndex(0);
    else
        m_ui->tabWidget->setCurrentIndex(CurrInd+1);
}

void SettingsDlg::on_actionPrevTab_triggered()
{
    int CurrInd = m_ui->tabWidget->currentIndex();
    if(CurrInd == 0)
        m_ui->tabWidget->setCurrentIndex(m_ui->tabWidget->count()-1);
    else
        m_ui->tabWidget->setCurrentIndex(CurrInd-1);
}

void SettingsDlg::on_sbTextFontSize_valueChanged(int arg1)
{
    if(!initDone) return;
    m_props->setInt(PROP_FONT_SIZE, arg1);
    updateStyleSample();
}

void SettingsDlg::on_sbTitleFontSize_valueChanged(int arg1)
{
    if(!initDone) return;
    m_props->setInt(PROP_STATUS_FONT_SIZE, arg1);
    updateStyleSample();
}

void SettingsDlg::on_sbInterlineSpace_valueChanged(int arg1)
{
    if(!initDone) return;

    int newValue = m_docview->GetArrayNextValue(cr_interline_spaces, sizeof(cr_interline_spaces)/sizeof(int),
                                                m_props->getIntDef(PROP_INTERLINE_SPACE, 100), arg1);

    if(newValue!=arg1) m_ui->sbInterlineSpace->setValue(newValue);

    m_props->setInt(PROP_INTERLINE_SPACE, newValue);
    updateStyleSample();
}

void SettingsDlg::on_cbEmbolden_toggled(bool checked)
{
    if(!initDone) return;
    setCheck(PROP_FONT_WEIGHT_EMBOLDEN, checked);
    updateStyleSample();
}

void SettingsDlg::on_cbKerning_toggled(bool checked)
{
    if(!initDone) return;
    setCheck(PROP_FONT_KERNING_ENABLED, checked);
    updateStyleSample();
}

void SettingsDlg::on_cbTxtPreFormatted_toggled(bool checked)
{
    if(!initDone) return;
    setCheck(PROP_TXT_OPTION_PREFORMATTED, checked);
}

void SettingsDlg::on_cbEnableEmbeddedFonts_toggled(bool checked)
{
    if(!initDone) return;
    setCheck(PROP_EMBEDDED_FONTS, checked ? Qt::Checked : Qt::Unchecked);
}

void SettingsDlg::on_cbEnableDocumentStyles_toggled(bool checked)
{
    if(!initDone) return;
    setCheck(PROP_EMBEDDED_STYLES, checked ? Qt::Checked : Qt::Unchecked);
    m_ui->cbEnableEmbeddedFonts->setEnabled(checked);
}

void SettingsDlg::on_FloatingPunctuation_toggled(bool checked)
{
    if(!initDone) return;
    setCheck(PROP_FLOATING_PUNCTUATION, checked);
    updateStyleSample();
}

void SettingsDlg::on_ChapterMarks_toggled(bool checked)
{
    if(!initDone) return;
    setCheck(PROP_STATUS_CHAPTER_MARKS, checked);
}

void SettingsDlg::on_PositionPercent_toggled(bool checked)
{
    if(!initDone) return;
    setCheck(PROP_SHOW_POS_PERCENT, checked);
}

void SettingsDlg::on_PageNumber_toggled(bool checked)
{
    if(!initDone) return;
    setCheck(PROP_SHOW_PAGE_NUMBER, checked);
    setCheck(PROP_SHOW_PAGE_COUNT, checked);
}

void SettingsDlg::SwitchCBs(bool state)
{
    m_ui->ShowBattery->setEnabled(state);
    m_ui->ShowBatteryInPercent->setEnabled(state);
    m_ui->ShowClock->setEnabled(state);
    m_ui->ShowBookName->setEnabled(state);
    m_ui->ChapterMarks->setEnabled(state);
    m_ui->PositionPercent->setEnabled(state);
    m_ui->PageNumber->setEnabled(state);
}

#define DEF_MARGIN	8

int GetMaxMargin(PropsRef props)
{
    int top = props->getIntDef(PROP_PAGE_MARGIN_TOP, DEF_MARGIN);
    int bottom = props->getIntDef(PROP_PAGE_MARGIN_BOTTOM, DEF_MARGIN);
    int left = props->getIntDef(PROP_PAGE_MARGIN_LEFT, DEF_MARGIN);
    int right = props->getIntDef(PROP_PAGE_MARGIN_RIGHT, DEF_MARGIN);
    // get maximum
    int value = top>bottom ? top : bottom;
    value = value>left ? value : left;
    value = value>right ? value : right;
    return value;
}

void SettingsDlg::on_cbMarginSide_currentIndexChanged(int index)
{
    switch(index) {
    case 0:
        m_ui->sbMargin->setValue(m_props->getIntDef(PROP_PAGE_MARGIN_TOP, DEF_MARGIN));
        break;
    case 1:
        m_ui->sbMargin->setValue(m_props->getIntDef(PROP_PAGE_MARGIN_BOTTOM, DEF_MARGIN));
        break;
    case 2:
        m_ui->sbMargin->setValue(m_props->getIntDef(PROP_PAGE_MARGIN_LEFT, DEF_MARGIN));
        break;
    case 3:
        m_ui->sbMargin->setValue(m_props->getIntDef(PROP_PAGE_MARGIN_RIGHT, DEF_MARGIN));
        break;
    case 4:
        m_ui->sbMargin->setValue(GetMaxMargin(m_props));
    }
    on_sbMargin_valueChanged(m_ui->sbMargin->value());
}

void SettingsDlg::on_sbMargin_valueChanged(int arg1)
{
    if(!initDone) return;

    int prevValue = 0;
    int index = m_ui->cbMarginSide->currentIndex();
    switch(index) {
    case 0:
        prevValue = m_props->getIntDef(PROP_PAGE_MARGIN_TOP, 8);
        break;
    case 1:
        prevValue = m_props->getIntDef(PROP_PAGE_MARGIN_BOTTOM, 8);
        break;
    case 2:
        prevValue = m_props->getIntDef(PROP_PAGE_MARGIN_LEFT, 8);
        break;
    case 3:
        prevValue = m_props->getIntDef(PROP_PAGE_MARGIN_RIGHT, 8);
        break;
    case 4:
        prevValue = GetMaxMargin(m_props);
    }

    int newValue = m_docview->GetArrayNextValue(def_margin, sizeof(def_margin)/sizeof(int), prevValue, arg1);
    if(newValue!=arg1)
        m_ui->sbMargin->setValue(newValue);

    switch(index) {
    case 0:
        m_props->setInt(PROP_PAGE_MARGIN_TOP, newValue);
        break;
    case 1:
        m_props->setInt(PROP_PAGE_MARGIN_BOTTOM, newValue);
        break;
    case 2:
        m_props->setInt(PROP_PAGE_MARGIN_LEFT, newValue);
        break;
    case 3:
        m_props->setInt(PROP_PAGE_MARGIN_RIGHT, newValue);
        break;
    case 4: {
        m_props->setInt(PROP_PAGE_MARGIN_TOP, newValue);
        m_props->setInt(PROP_PAGE_MARGIN_BOTTOM, newValue);
        m_props->setInt(PROP_PAGE_MARGIN_LEFT, newValue);
        m_props->setInt(PROP_PAGE_MARGIN_RIGHT, newValue);
    }
    }
}

void SettingsDlg::on_cbFullUpdateEvery_currentIndexChanged(const QString &arg1)
{
    if(!initDone) return;
    m_props->setInt(PROP_DISPLAY_FULL_UPDATE_INTERVAL, arg1.toInt());
}

void SettingsDlg::on_cbAA_currentIndexChanged(int index)
{
    if(!initDone) return;
    m_props->setInt(PROP_FONT_ANTIALIASING, index);
    updateStyleSample();
}

void SettingsDlg::on_cbHinting_currentIndexChanged(int index)
{
    if(!initDone) return;
    m_props->setInt(PROP_FONT_HINTING, index);
    updateStyleSample();
}

void SettingsDlg::on_sbSpaceCond_valueChanged(int arg1)
{
    if(!initDone) return;
    m_props->setInt(PROP_FORMAT_MIN_SPACE_CONDENSING_PERCENT, arg1);
    updateStyleSample();
}

void SettingsDlg::on_cbCoverScreensaver_toggled(bool checked)
{
    if(!initDone) return;
    setCheck(PROP_REPLACE_SCREENSAVER, checked);
}

void SettingsDlg::on_buttonBox_accepted()
{
    on_actionSaveSettings_triggered();
}

void SettingsDlg::on_buttonBox_rejected()
{
    close();
}

void SettingsDlg::on_tabWidget_currentChanged(int index)
{
    if(!initDone) return;
    Device::forceFullScreenUpdate();
    m_props->setInt(PROP_LAST_TAB, index);
}
