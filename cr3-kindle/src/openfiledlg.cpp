#include "openfiledlg.h"
#include "ui_openfiledlg.h"

#include <QDir>
#include <QShortcut>

OpenFileDlg::OpenFileDlg(QWidget *parent, CR3View * docView):
    QDialog(parent),
    m_ui(new Ui::OpenFileDlg),
    m_docview(docView)
{
    m_ui->setupUi(this);

    addAction(m_ui->actionGoToBegin);
    addAction(m_ui->actionNextPage);
    addAction(m_ui->actionPrevPage);
    addAction(m_ui->actionGoToFirstPage);
    addAction(m_ui->actionGoToLastPage);

    QAction *actionRemoveFile = m_ui->actionRemoveFile;
    QShortcut* kbd = new QShortcut(Qt::Key_AltGr, this); // quick hack to delete files on K4NT with KBD key
    connect(kbd, SIGNAL(activated()), actionRemoveFile, SLOT(trigger()));
    addAction(actionRemoveFile);

    QAction *actionSelect = m_ui->actionSelectFile;
    actionSelect->setShortcut(Qt::Key_Select);
    addAction(actionSelect);

    folder = QIcon(":/icons/folder.png");
    file = QIcon(":/icons/book.png");
    arrowUp = QIcon(":/icons/arrow_top.png");

    m_ui->FileList->setItemDelegate(new FileListDelegate());

    QString lastPathName;
    QString lastName;
    if(!docView->GetLastPathName(&lastPathName))
#ifdef i386
        CurrentDir = "/home/";
#else
        CurrentDir = "/mnt/us/documents/";
#endif
    else {
        int pos = lastPathName.lastIndexOf("/");
        CurrentDir = lastPathName.mid(0, pos+1);
        lastName = lastPathName.mid(pos+1);
    }
    do {
        QDir Dir(CurrentDir);
        if(Dir.exists()) break;
        // trim last "/"
        CurrentDir.chop(1);
        int pos = CurrentDir.lastIndexOf("/");
        CurrentDir = CurrentDir.mid(0, pos+1);
        lastName = "";
    } while(true);

    FillFileList();
    m_ui->FileList->setCurrentRow(0);
    // showing last opened page
    int rc = docView->rowCount*2;
    curPage=0;
    if(!lastName.isEmpty()) {
        int pos = curFileList.indexOf(lastName)+1;
        if(pos!=0 && pos>rc) {
            curPage = (pos/rc)-1;
            if(pos%rc) curPage+=1;
        }
    }
    ShowPage(1);
    // selecting last opened book
    if(!lastName.isEmpty()) {
        QList<QListWidgetItem*> searchlist = m_ui->FileList->findItems(lastName, Qt::MatchExactly);
        if(searchlist.count())
            m_ui->FileList->setCurrentItem(searchlist.at(0));
    }
}

bool OpenFileDlg::showDlg(QWidget * parent, CR3View * docView)
{
    OpenFileDlg *dlg = new OpenFileDlg(parent, docView);
    dlg->showMaximized();
    return true;
}

OpenFileDlg::~OpenFileDlg()
{
    delete m_ui;
}

void OpenFileDlg::FillFileList()
{
    if(titleMask.isEmpty())
        titleMask = windowTitle();
    curFileList.clear();
    m_ui->FileList->clear();

    QDir::Filters filters;

#ifdef i386
    if(CurrentDir=="/") filters = QDir::AllDirs|QDir::NoDotAndDotDot;
#else
    if(CurrentDir=="/mnt/us/") filters = QDir::AllDirs|QDir::NoDotAndDotDot;
#endif
    else filters = QDir::AllDirs|QDir::NoDot;
    QDir Dir(CurrentDir);
    curFileList=Dir.entryList(filters, QDir::Name);
    dirCount=curFileList.count();

    QStringList Filter;
    Filter << "*.fb2" << "*.zip" << "*.epub" << "*.rtf" << "*.txt" \
           << "*.html" << "*.htm" << "*.tcr" << "*.pdb" << "*.chm" << "*.mobi" << "*.doc" << "*.azw";
    curFileList+= Dir.entryList(Filter, QDir::Files, QDir::Name);

    int count = curFileList.count();
    pageCount = 1;
    int rc = m_docview->rowCount*2;
    if(count>rc) {
        pageCount = count/rc;
        if(count%rc) pageCount+=1;
    }
}

void OpenFileDlg::ShowPage(int updown)
{
    Device::forceFullScreenUpdate();

    if(updown>0) {
        if(curPage+1>pageCount) curPage=0;
        curPage+=1;
    } else {
        if(curPage-1<=0) curPage=pageCount+1;
        curPage-=1;
    }
    m_ui->FileList->clear();
    setWindowTitle(titleMask + " (" + QString::number(curPage) + "/" + QString::number(pageCount) + ")");

    int rc = m_docview->rowCount*2;
    int h = (m_docview->height() -2 - (qApp->font().pointSize() + rc))/rc;
    QListWidgetItem *item = new QListWidgetItem();
    item->setSizeHint(QSize(item->sizeHint().width(), h));
    QListWidgetItem *pItem;

    int i=0;
    int startPos = ((curPage-1)*rc);
    if(startPos==0 && curFileList.at(0)=="..") {
        pItem = item->clone();
        pItem->setText("..");
        pItem->setIcon(arrowUp);
        m_ui->FileList->addItem(pItem);
        i++;
        startPos++;
    }

    for(int k=startPos; (k<curFileList.count()) && (i<rc); ++k, ++i) {
        if(k<dirCount) item->setIcon(folder);
        else item->setIcon(file);

        pItem = item->clone();
        pItem->setText(curFileList[k]);
        m_ui->FileList->addItem(pItem);
    }
    m_ui->FileList->setCurrentRow(0);
}

void OpenFileDlg::on_actionRemoveFile_triggered()
{
    QListWidgetItem *item  = m_ui->FileList->currentItem();
    QString ItemText = item->text();
    if(ItemText == "..")
    {
        return;
    } else {
        bool isFileRemoved = false;

        int current_row = m_ui->FileList->currentRow();
        if (ItemText.length()==0) return;
        QString fileName = CurrentDir + ItemText;
        QFileInfo FileInfo(fileName);
        if(FileInfo.isDir()) {
            QDir::Filters filters;
            filters = QDir::AllDirs|QDir::NoDotAndDotDot;
            QDir Dir(fileName);
            QStringList curFileList1 = Dir.entryList(filters, QDir::Name);
            QStringList Filter;
            Filter << "*.fb2" << "*.zip" << "*.epub" << "*.rtf" << "*.txt" \
                   << "*.html" << "*.htm" << "*.tcr" << "*.pdb" << "*.chm" << "*.mobi" << "*.doc" << "*.azw" << "*.*";

            curFileList1 += Dir.entryList(Filter, QDir::Files, QDir::Name);
            if(curFileList1.count()>0) {
                QMessageBox * mb = new QMessageBox( QMessageBox::Information, tr("Info"), tr("Directory ")+ItemText+tr(" is not empty."), QMessageBox::Close, this );
                mb->exec();
            } else {
                QMessageBox * mb = new QMessageBox(QMessageBox::Information,"","", QMessageBox::Yes | QMessageBox::No, this);
                if(mb->question(this, tr("Remove directory"), tr("Do you really want to remove directory ")+ItemText+"?", QMessageBox::Yes | QMessageBox::No, QMessageBox::Yes ) == QMessageBox::Yes) {
                    QDir Dir_file(QDir::toNativeSeparators(CurrentDir));
                    isFileRemoved = Dir_file.rmdir(ItemText);
                }
            }
        } else {
            // Remove file dialog
            if(QMessageBox::question(this, tr("Remove file"), tr("Do you really want to remove file ")+ItemText+"?", QMessageBox::Yes | QMessageBox::No, QMessageBox::Yes ) == QMessageBox::Yes) {
                // Удаление из истории и файла кеша
                LVPtrVector<CRFileHistRecord> & files = m_docview->getDocView()->getHistory()->getRecords();
                int num=-1;
                QString filenamepath;
                // ищем в истории файл с таким путём и названием архива
                for(int i=0;i<files.length();i++) {
                    filenamepath = cr2qt(files.get(i)->getFilePathName());
                    if(fileName == filenamepath) num = i;
                }
                // Найдена запись в истории, удаляем саму запись и нужный файл кеша
                if(num>-1){
                    // делаем активным найденный документ чтобы узнать его истинное название файла
                    if(num>0)
                        m_docview->loadDocument(cr2qt(files[num]->getFilePathName()));
                    // Уточняем CRC удаляемого файла
                    lUInt32 crc = m_docview->getDocView()->getDocProps()->getIntDef(DOC_PROP_FILE_CRC32, 0);
                    QString cachePattern;
                    cachePattern.sprintf("*.%08x*.cr3", crc);
                    // делаем активным опять предыдущий документ
                    if(files.length()>1) {
                        m_docview->loadDocument(cr2qt(files[1]->getFilePathName()));
                    } else {
                        QMessageBox * mb = new QMessageBox( QMessageBox::Information, tr("Info"), tr("You can't remove last book that you are reading now. Please, choose other active book then try to remove file again."), QMessageBox::Close, this );
                        mb->exec();
                        return;
                    }

                    // trim file extension, need for archive files
                    QDir Dir(qApp->applicationDirPath() + QDir::toNativeSeparators(QString("/data/cache/")));
                    QStringList fileList1 = Dir.entryList(QStringList() << cachePattern, QDir::Files);
                    if(fileList1.count())
                        Dir.remove(fileList1.at(0));

                    // Удаление записи истории
                    if(files.length()>1) {
                        files.remove(1);
                    }
                }

                QDir Dir_file(QDir::toNativeSeparators(CurrentDir));
                QStringList fileList = Dir_file.entryList(QStringList() << ItemText, QDir::Files);
                if(fileList.count())
                    isFileRemoved = Dir_file.remove(fileList.at(0));
            }
        }

        if(isFileRemoved){
            for(int k=0; k<curFileList.count(); ++k) {
                if(curFileList.at(k) == ItemText) {
                    curFileList.removeAt(k);
                    break;
                }
            }

            m_ui->FileList->clear();
            setWindowTitle(titleMask + " (" + QString::number(curPage) + "/" + QString::number(pageCount) + ")");

            int rc = m_docview->rowCount*2;
            int h = (m_docview->height() -2 - (qApp->font().pointSize() + rc))/rc;
            QListWidgetItem *item = new QListWidgetItem();
            item->setSizeHint(QSize(item->sizeHint().width(), h));
            QListWidgetItem *pItem;

            int i=0;
            int startPos = ((curPage-1)*rc);
            if(startPos==0 && curFileList.at(0)=="..") {
                pItem = item->clone();
                pItem->setText("..");
                pItem->setIcon(arrowUp);
                m_ui->FileList->addItem(pItem);
                i++;
                startPos++;
            }

            for(int k=startPos; (k<curFileList.count()) && (i<rc); ++k, ++i) {
                if(k<dirCount) item->setIcon(folder);
                else item->setIcon(file);

                pItem = item->clone();
                pItem->setText(curFileList[k]);
                m_ui->FileList->addItem(pItem);
            }
            m_ui->FileList->setCurrentRow(0);

            int count = curFileList.count();
            if(count>current_row)
                m_ui->FileList->setCurrentRow(current_row);
            else
                m_ui->FileList->setCurrentRow(current_row-1);
        }
        return;
    }
}

void OpenFileDlg::on_actionGoToBegin_triggered()
{
    m_ui->FileList->setCurrentRow(0);
}

void OpenFileDlg::on_actionSelectFile_triggered()
{
    QListWidgetItem *item  = m_ui->FileList->currentItem();
    QString ItemText = item->text();

    if(ItemText == "..")
    {
        int i;
        for(i=CurrentDir.length()-1; i>1; i--) {
            if(CurrentDir[i-1] == QChar('/')) break;
        }
        QString prevDir = CurrentDir.mid(i);
        prevDir.resize(prevDir.count()-1);

        CurrentDir.resize(i);
        FillFileList();
        // showing previous opened page
        int rc = m_docview->rowCount*2;
        curPage=0;
        if(!prevDir.isEmpty()) {
            int pos = curFileList.indexOf(prevDir)+1;
            if(pos!=0 && pos>rc) {
                curPage = (pos/rc)-1;
                if(pos%rc) curPage+=1;
            }
        }
        ShowPage(1);
        // selecting previous opened dir
        if(!prevDir.isEmpty()) {
            QList<QListWidgetItem*> searchlist = m_ui->FileList->findItems(prevDir, Qt::MatchExactly);
            if(searchlist.count())
                m_ui->FileList->setCurrentItem(searchlist.at(0));
        }
    } else {
        if (ItemText.length()==0) return;

        QString fileName = CurrentDir + ItemText;
        QFileInfo FileInfo(fileName);

        if(FileInfo.isDir()) {
            CurrentDir = fileName + QString("/");
            FillFileList();
            curPage=0;
            ShowPage(1);
        } else {
            // Удаление из истории и файла кеша
            LVPtrVector<CRFileHistRecord> & files = m_docview->getDocView()->getHistory()->getRecords();
            int num=-1;
            QString filenamepath;
            // ищем в истории файл с таким путём и названием архива
            for(int i=0;i<files.length();i++) {
                filenamepath = cr2qt(files.get(i)->getFilePathName());
                if(fileName == filenamepath) num = i;
            }
            lvpos_t file_size1;
            // Проверка - не лежит ли в истории файл с таким же путём и названием
            if(fileName.length()>0){
                // Найдена запись в истории, удаляем саму запись и нужный файл кеша
                if(num>-1){
                    file_size1 = files.get(num)->getFileSize();

                    // делаем активным найденный документ чтобы узнать его истинное название файла
                    if(num>=0) {
                        QString filename1;
                        filename1 = cr2qt(files.get(num)->getFileName());
                        // Уточняем CRC удаляемого файла
                        // делаем активным опять предыдущий документ
                        // trim file extension, need for archive files
                        int pos = filename1.lastIndexOf(".");
                        if(pos != -1) filename1.resize(pos);
                        // remove cache file
                        QDir Dir(qApp->applicationDirPath() + QDir::toNativeSeparators(QString("/data/cache/")));
                        QStringList fileList1 = Dir.entryList(QStringList() << filename1 + "*.cr3", QDir::Files);
                        if(fileList1.count())
                            Dir.remove(fileList1.at(0));
                        fileList1 = Dir.entryList(QStringList() << filename1 + "*.cr3", QDir::Files);
                        if(fileList1.count())
                            Dir.remove(fileList1.at(0));

                        // Удаление записи истории

                        // Загружаем новую книгу
                        m_docview->loadDocument(fileName);

                        LVPtrVector<CRFileHistRecord> & files1 = m_docview->getDocView()->getHistory()->getRecords();
                        lvpos_t file_size2 = files.get(0)->getFileSize();
                        if(num>-1){
                            // Удаление записи истории
                            if (file_size1 != file_size2) {
                                QMessageBox * mb = new QMessageBox( QMessageBox::Information, tr("Info"), tr("Other File with such FilePath in history"), QMessageBox::Close, this );
                                mb->exec();
                                files1.remove(num+1);
                            }
                        }
                    }
                    // Удаление из истории
                    // ищем в истории файл с таким путём и названием архива
                    // Найдена запись в истории, удаляем саму запись
                }
            }

            // Загружаем новую книгу
            m_docview->loadDocument(fileName);
            // Зачем вообще нужен файл кеша? Все равно потом появится сам :-(
            close();
        }
    }
}

void OpenFileDlg::on_actionNextPage_triggered()
{
    ShowPage(1);
}

void OpenFileDlg::on_actionPrevPage_triggered()
{
    ShowPage(-1);
}

void OpenFileDlg::on_actionGoToLastPage_triggered()
{
    if(pageCount-1==0) return;
    curPage=pageCount-1;
    ShowPage(1);
}

void OpenFileDlg::on_actionGoToFirstPage_triggered()
{
    if(curPage==1) return;
    curPage=0;
    ShowPage(1);
}
