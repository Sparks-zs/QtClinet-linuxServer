#include "filetransfer.h"
#include "../mainwindow/mainwindow.h"
#include "flowlayout.h"
#include <QDebug>
#include <QThread>

const QMap<QString, int> FileTransfer::customCode = {
    {"GET_SHAREDIR", 0},
    {"DOWNLOAD_FILE", 1},
    {"UPLOAD_FILE", 2}
};

FileTransfer::FileTransfer(QWidget *parent)
    : QWidget(parent), isFileContent(false),
      tcp_(nullptr), curFile_(nullptr), curPath_({})
{
    initGUI_();
    initTcp_();
}

FileTransfer::~FileTransfer()
{
}

void FileTransfer::initGUI_()
{
    prevWidget_ = nullptr;
    curWidget_ = nullptr;
    layout_ = new QVBoxLayout;
    setLayout(layout_);
    stackedWidget_ = new QStackedWidget;

    upLoadAct_ = new QAction(tr("&upload file"), this);
    connect(upLoadAct_, SIGNAL(triggered()), this, SLOT(onClickedUploadButton()));
}

void FileTransfer::initTcp_()
{
    tcp_ = new TcpSocket;
    QThread *thread = new QThread;
    tcp_->moveToThread(thread);

    connect(thread, SIGNAL(started()), this, SLOT(connectServer_()));
    connect(tcp_, SIGNAL(connected()), this, SLOT(onConnected()));
    connect(tcp_, SIGNAL(readFinished()), this, SLOT(parseDatafromServer()));
    connect(tcp_, SIGNAL(error(int, QString)), this, SLOT(errorDisplay(int, QString)));

    thread->start();
}

void FileTransfer::connectServer_()
{
    tcp_->tcpInit();
    tcp_->connectServer("192.168.43.153", 8888);
}

void FileTransfer::onConnected()
{
    getDirectoryFromServer("/");
    curPath_.push_back("/");
}

void FileTransfer::getDirectoryFromServer(const QString& path)
{
    tcp_->addHeader("CustomCode", "GET_SHAREDIR");
    tcp_->sendMsg("DirPath=" + path + "&");
    tcp_->write("GET");
}

void FileTransfer::parseDatafromServer()
{
    int code = customCode[tcp_->get("CustomCode")];
    switch (code)
    {
    case 0:
        setupDirectory(QString(tcp_->get("ShareDir")).split(";", QString::SkipEmptyParts));
        break;
    case 1:
        curFile_->setContent(tcp_->get("FileContent"));
        showFileContent();
        break;
    }
}

void FileTransfer::onClickedFile_(File* file)
{
    curFile_ = file;
    tcp_->addHeader("CustomCode", "DOWNLOAD_FILE");
    tcp_->sendMsg("FilePath=" + joinPath(getCurPath(), curFile_->getFileName()));
    tcp_->write("GET");
}

void FileTransfer::onClickedFolder_(Folder* folder)
{
    getDirectoryFromServer(joinPath(getCurPath(), folder->getFileName()));
    curPath_.push_back(folder->getFileName());
}

void FileTransfer::onClickedDownloadButton()
{
    if(!curFile_)
        return;

//    if(curFile_->getContent().isEmpty()){
//        onClickedFile_(curFile_);
//    }

    QString dirpath = joinPath(QDir::currentPath(), getCurPath());
    qDebug() << dirpath;

    QDir dir;
    if(!dir.mkpath(dirpath))
        return;

    QFile *qfile = new QFile(joinPath(dirpath, curFile_->getFileName()));
    if(!qfile->open(QFile::WriteOnly))
    {
        qDebug()<<"client：open file error!";
        qDebug() << "path:" << dirpath + "/" + curFile_->getFileName();
        return;
    }
    qfile->write(curFile_->getContent());
    qfile->close();
}

void FileTransfer::onClickedUploadButton()
{
    QString filename = QFileDialog::getOpenFileName(this);
    if(filename.isEmpty())
        return;

    QFile *file = new QFile(filename);
    if(!file->open(QFile::ReadOnly))
    {
        qDebug()<<"client：open file error!";
        return;
    }
    qDebug() << "filesize: " << file->size();
    if(file->exists()){
        QString filepath = joinPath(getCurPath(), filename.split('/').back());
        tcp_->addHeader("CustomCode", "UPLOAD_FILE");
        tcp_->sendFile(filepath, file->readAll());
        tcp_->write("POST");
    }
    file->close();
}

void FileTransfer::setupDirectory(const QStringList &list)
{
    QWidget* widget = new QWidget;
    FlowLayout* layout = new FlowLayout;
    widget->setLayout(layout);

    int len = list.length();
    for(int i=0; i<len; i++){
        QString name = list[i];
        if(isDir(name)){
            Folder* folder = new Folder(name);
            layout->addWidget(folder);
            connect(folder, SIGNAL(clicked(Folder*)), this, SLOT(onClickedFolder_(Folder*)));
        }
        else{
            File* file = new File(name);
            layout->addWidget(file);
            connect(file, SIGNAL(clicked(File*)), this, SLOT(onClickedFile_(File*)));
        }
    }

    onSwitchWidget(widget);
}

void FileTransfer::showFileContent()
{
    QWidget* widget = new QWidget;
    QVBoxLayout* layout = new QVBoxLayout;
    widget->setLayout(layout);

    QPushButton* downloadButton = new QPushButton("下载");
    downloadButton->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    connect(downloadButton, SIGNAL(clicked()), this, SLOT(onClickedDownloadButton()));

    if(curFile_->getFileType() == FileType::TEXT) {
        QTextEdit* text = new QTextEdit;
        text->setReadOnly(true);
        text->setText(curFile_->getContent());
        layout->addWidget(text);
    }
    else if(curFile_->getFileType() == FileType::IMAGE){
        QPixmap img;
        if(img.loadFromData(curFile_->getContent())){
            QLabel* label = new QLabel("图片加载成功");
            QLabel* imgLabel = new QLabel;
            imgLabel->setPixmap(img);
            layout->addWidget(imgLabel);
            layout->addWidget(label);
        }
        else {
            QLabel* label = new QLabel("图片加载失败");
            layout->addWidget(label);
        }
    }
    else if(curFile_->getFileType() == FileType::VIDEO){

    }
    else if(curFile_->getFileType() == FileType::UNKNOW){
    }

    layout->addWidget(downloadButton);
    onSwitchWidget(widget);
    isFileContent = true;
}

void FileTransfer::onSwitchWidget(QWidget* widget)
{
    if(prevWidget_ == nullptr && curWidget_ == nullptr){
        layout_->addWidget(widget);
        curWidget_ = widget;
        return;
    }
    prevWidget_ = curWidget_;
    curWidget_ = widget;

    // 当前界面
    QPropertyAnimation *animation1 = new QPropertyAnimation(prevWidget_,"geometry");
    animation1->setDuration(200);
    animation1->setStartValue(QRect(0, 0, this->width(), this->height()));
    animation1->setEndValue(QRect(-this->width(), 0, this->width(), this->height()));

    // 下一个界面
    QPropertyAnimation *animation2 = new QPropertyAnimation(curWidget_, "geometry");
    animation2->setDuration(200);
    animation2->setStartValue(QRect(this->width(), 0, this->width(), this->height()));
    animation2->setEndValue(QRect(0, 0, this->width(), this->height()));

    QParallelAnimationGroup *group = new QParallelAnimationGroup;
    group->addAnimation(animation1);
    group->addAnimation(animation2);
    group->start();
    connect(group, SIGNAL(finished()), this, SLOT(onAnimationFinished()));
}

void FileTransfer::onAnimationFinished()
{
    if(prevWidget_){
        layout_->removeWidget(prevWidget_);
//        delete prevWidget_;
    }
    layout_->addWidget(curWidget_);
}

bool FileTransfer::event(QEvent *event)
{
    if(MainWindow::getInstance()->getCurrentPageIndex() != MainWindow::FileWidgetPage)
        return QWidget::event(event);
    QEvent::Type t = event->type();
    switch(t)
    {
    case QEvent::KeyRelease:
        if(((static_cast<QKeyEvent*>(event))->key() == Qt::Key_Back)
            || ((static_cast<QKeyEvent*>(event))->key() == Qt::Key_F1))
        {
            event->accept();
            if(isFileContent){
                onSwitchWidget(prevWidget_);
                isFileContent = false;
            }
            else{
                QString path = getUpPath();
                qDebug() << "up path: " << path;
                if(path.isEmpty()){   // 返回主界面
                    MainWindow::getInstance()->onSwitchPage(MainWindow::MainWindowPage);
                }
                else {    // 返回上一级界面
                    getDirectoryFromServer(path);
                }
            }
            return true;
        }
        break;
    default:
        break;
    }
    return QWidget::event(event);
}

void FileTransfer::contextMenuEvent(QContextMenuEvent *event)
{
    QMenu *menu = new QMenu(this);
    menu->addAction(upLoadAct_);

    menu->move(cursor().pos());
    menu->show();
}

void FileTransfer::errorDisplay(int socketError, const QString& message)
{
    switch (socketError) {
    case QAbstractSocket::HostNotFoundError:
        QMessageBox::information(this, tr("Blocking Fortune Client"),
                                 tr("The host was not found. Please check the "
                                    "host and port settings."));
        break;
    case QAbstractSocket::ConnectionRefusedError:
        QMessageBox::information(this, tr("Blocking Fortune Client"),
                                 tr("The connection was refused by the peer. "
                                    "Make sure the fortune server is running, "
                                    "and check that the host name and port "
                                    "settings are correct."));
        break;
    default:
        QMessageBox::information(this, tr("Blocking Fortune Client"),
                                 tr("The following error occurred: %1.")
                                 .arg(message));
    }
}

bool FileTransfer::isDir(const QString &filename)
{
    if(filename.count('.') > 0)
        return false;
    return true;
}

QString FileTransfer::getCurPath()
{
    if(curPath_.isEmpty())
        return "";

    QString path;
    int len = curPath_.length();
    for(int i=0; i<len; i++){
        path.push_back(curPath_[i]);
        if(i < len - 1)
            path.push_back("/");
    }
    return path;
}

QString FileTransfer::getUpPath()
{
    curPath_.pop_back();
    return getCurPath();
}

QString FileTransfer::joinPath(const QString& p1, const QString& p2)
{
    if(p1.isEmpty())
        return p2;
    if(p2.isEmpty())
        return p1;
    if(p1.back() == '/' && p2.front() == '/')
        return p1.left(p1.length() - 1) + p2;
    else if(p1.back() == '/' || p2.front() == '/')
        return p1 + p2;
    else
        return p1 + "/" + p2;
}
