#include "filepropsdlg.h"
#include "ui_filepropsdlg.h"

FilePropsDialog::FilePropsDialog(QWidget *parent, CR3View * docView ) :
    QDialog(parent),
    m_ui(new Ui::FilePropsDialog)
  ,_cr3v(docView)
  ,_docview(docView->getDocView())
{
    m_ui->setupUi(this);

    setWindowTitle(QString(CR_ENGINE_VERSION) + " // " + windowTitle());
    m_ui->tableWidget->setItemDelegate(new FilePropsListDelegate());
    m_ui->tableWidget->verticalHeader()->setResizeMode(QHeaderView::ResizeToContents);

    fillItems();
}

FilePropsDialog::~FilePropsDialog()
{
    delete m_ui;
}

bool FilePropsDialog::showDlg( QWidget * parent, CR3View * docView )
{
    FilePropsDialog *dlg = new FilePropsDialog(parent, docView);
    dlg->showMaximized();
    return true;
}

QString FilePropsDialog::getDocText( const char * path, const char * delim )
{
    ldomDocument * doc = _docview->getDocument();
    lString16 res;
    for ( int i=0; i<100; i++ ) {
        lString8 p = lString8(path) + "[" + lString8::itoa(i+1) + "]";
        lString16 p16 = Utf8ToUnicode(p);
        ldomXPointer ptr = doc->createXPointer( p16 );
        if ( ptr.isNull() )
            break;
        lString16 s = ptr.getText( L' ' );
        if ( s.empty() )
            continue;
        if ( !res.empty() && delim!=NULL )
            res << Utf8ToUnicode( lString8( delim ) );
        res << s;
    }
    return cr2qt(res);
}

QString FilePropsDialog::getDocAuthors( const char * path, const char * delim )
{
    lString16 res;
    for ( int i=0; i<100; i++ ) {
        lString8 p = lString8(path) + "[" + lString8::itoa(i+1) + "]";

        lString16 firstName = qt2cr(getDocText( (p + "/first-name").c_str(), " " ));
        lString16 lastName = qt2cr(getDocText( (p + "/last-name").c_str(), " " ));
        lString16 middleName = qt2cr(getDocText( (p + "/middle-name").c_str(), " " ));
        lString16 nickName = qt2cr(getDocText( (p + "/nickname").c_str(), " " ));
        lString16 homePage = qt2cr(getDocText( (p + "/homepage").c_str(), " " ));
        lString16 email = qt2cr(getDocText( (p + "/email").c_str(), " " ));
        lString16 s = firstName;
        if ( !middleName.empty() )
            s << " " << middleName;
        if ( !lastName.empty() ) {
            if ( !s.empty() )
                s << " ";
            s << lastName;
        }
        if ( !nickName.empty() ) {
            if ( !s.empty() )
                s << " ";
            s << nickName;
        }
        if ( !homePage.empty() ) {
            if ( !s.empty() )
                s << " ";
            s << homePage;
        }
        if ( !email.empty() ) {
            if ( !s.empty() )
                s << " ";
            s << email;
        }
        if ( s.empty() )
            continue;
        if ( !res.empty() && delim!=NULL )
            res << Utf8ToUnicode( lString8( delim ) );
        res << s;
    }
    return cr2qt(res);
}

void FilePropsDialog::addPropLine(QString name, QString v)
{
    if(!v.isEmpty()) {
        if(!_value.isEmpty()) _value += "\n";

        _value += name + ": " + v;
    }
}

void FilePropsDialog::addInfoSection(QString name)
{
    if(_value.isEmpty()) return;

    int y = m_ui->tableWidget->rowCount();
    m_ui->tableWidget->setRowCount(y+2);

    QFont fontBold = m_ui->tableWidget->font();
    fontBold.setBold(true);

    QFont fontItalic = m_ui->tableWidget->font();
    fontItalic.setItalic(true);

    QTableWidgetItem * item1 = new QTableWidgetItem(name);
    item1->setFont(fontBold);
    m_ui->tableWidget->setItem(y, 0, item1);

    QTableWidgetItem * item2 = new QTableWidgetItem(_value);
    item2->setData(Qt::UserRole, QVariant(2));
    item2->setFont(fontItalic);
    m_ui->tableWidget->setItem(y+1, 0, item2);

    _value.clear();
}

void FilePropsDialog::fillItems()
{
    _docview->savePosition();
    CRPropRef props = _docview->getDocProps();

    addPropLine(tr("Archive name"), cr2qt(props->getStringDef(DOC_PROP_ARC_NAME)) );
    addPropLine(tr("Archive path"), cr2qt(props->getStringDef(DOC_PROP_ARC_PATH)) );
    addPropLine(tr("Archive size"), cr2qt(props->getStringDef(DOC_PROP_ARC_SIZE)) );
    addPropLine(tr("File name"), cr2qt(props->getStringDef(DOC_PROP_FILE_NAME)) );
    addPropLine(tr("File path"), cr2qt(props->getStringDef(DOC_PROP_FILE_PATH)) );
    addPropLine(tr("File size"), cr2qt(props->getStringDef(DOC_PROP_FILE_SIZE)) );
    addPropLine(tr("File format"), cr2qt(props->getStringDef(DOC_PROP_FILE_FORMAT)) );

    addInfoSection(tr("File info"));

    addPropLine(tr("Title"), cr2qt(props->getStringDef(DOC_PROP_TITLE)) );
    addPropLine(tr("Author(s)"), cr2qt(props->getStringDef(DOC_PROP_AUTHORS)) );
    addPropLine(tr("Series name"), cr2qt(props->getStringDef(DOC_PROP_SERIES_NAME)) );
    addPropLine(tr("Series number"), cr2qt(props->getStringDef(DOC_PROP_SERIES_NUMBER)) );
    addPropLine(tr("Date"), getDocText( "/FictionBook/description/title-info/date", ", " ) );
    addPropLine(tr("Genres"), getDocText( "/FictionBook/description/title-info/genre", ", " ) );
    addPropLine(tr("Translator"), getDocText( "/FictionBook/description/title-info/translator", ", " ) );
    addInfoSection(tr("Book info") );

    addPropLine(tr("Document author"), getDocAuthors( "/FictionBook/description/document-info/author", " " ) );
    addPropLine(tr("Document date"), getDocText( "/FictionBook/description/document-info/date", " " ) );
    addPropLine(tr("Document source URL"), getDocText( "/FictionBook/description/document-info/src-url", " " ) );
    addPropLine(tr("OCR by"), getDocText( "/FictionBook/description/document-info/src-ocr", " " ) );
    addPropLine(tr("Document version"), getDocText( "/FictionBook/description/document-info/version", " " ) );
    addInfoSection( tr("Document info") );

    addPropLine(tr("Publication name"), getDocText( "/FictionBook/description/publish-info/book-name", " " ) );
    addPropLine(tr("Publisher"), getDocText( "/FictionBook/description/publish-info/publisher", " " ) );
    addPropLine(tr("Publisher city"), getDocText( "/FictionBook/description/publish-info/city", " " ) );
    addPropLine(tr("Publication year"), getDocText( "/FictionBook/description/publish-info/year", " " ) );
    addPropLine(tr("ISBN"), getDocText( "/FictionBook/description/publish-info/isbn", " " ) );
    addInfoSection(tr("Publication info"));

    addPropLine(tr("Custom info"), getDocText( "/FictionBook/description/custom-info", " " ) );
}

void FilePropsDialog::changeEvent(QEvent *e)
{
    QDialog::changeEvent(e);
    switch (e->type()) {
    case QEvent::LanguageChange:
        m_ui->retranslateUi(this);
        break;
    default:
        break;
    }
}
