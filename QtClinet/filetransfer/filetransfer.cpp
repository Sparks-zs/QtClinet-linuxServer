#include "filetransfer.h"
#include "../mainwindow/mainwindow.h"
#include "flowlayout.h"
#include <QDebug>
#include <QThread>

FileTransfer* FileTransfer::instance_ = nullptr;

const QMap<QString, int> FileTransfer::customCode = {
    {"GET_ShareDIR", 0},
    {"DOWNLOAD_FILE", 1},
    {"UPLOAD_FILE", 2}
};

FileTransfer::FileTransfer(QWidget *parent)
    : QWidget(parent), loadingIndex_(0), rootFolder_(nullptr),
      file_(nullptr), preIndex_(-1), removeFlag_(false)
{
    initGUI_();
    initTcp_();
}

FileTransfer* FileTransfer::getInstance()
{
    if(instance_ == nullptr)
        instance_ = new FileTransfer;
    return instance_;
}

void FileTransfer::initGUI_()
{
    layout_ = new QVBoxLayout;
    setLayout(layout_);
    stackWidget_ = new QStackedWidget;
    layout_->addWidget(stackWidget_);

    /*toolButton_ = new ToolButton(this);
    connect(toolButton_, SIGNAL(clicked()), this, SLOT(showToolWidget()));
    toolWidget = new QWidget;
    QHBoxLayout* toolLayout = new QHBoxLayout;
    toolWidget->resize(200, 100);
    toolWidget->hide();
    toolWidget->setLayout(toolLayout);
    toolWidget->setWindowFlag(Qt::WindowStaysOnTopHint);
    toolWidget->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed)*/;

//    uploadButton_ = new QPushButton("文件上传");
//    downloadButton_ = new QPushButton("文件下载");
//    toolLayout->addWidget(uploadButton_);
//    toolLayout->addWidget(downloadButton_);
//    connect(uploadButton_, SIGNAL(clicked()), this, SLOT(onClickedUploadButton()));
//    connect(downloadButton_, SIGNAL(clicked()), this, SLOT(onClickedDownloadButton()));

    upLoadAct_ = new QAction(tr("&upload file"), this);
    connect(upLoadAct_, SIGNAL(triggered()), this, SLOT(onClickedUploadButton()));

    // 加载
    loadWidget_ = new QWidget;
    loadLayout_ = new QVBoxLayout;
    loadWidget_->setLayout(loadLayout_);
    loadStackWidget_ = new QStackedWidget;
    loadLayout_->addWidget(loadStackWidget_);

    loadingBar_ = new QProgressBar;
    loadingBar_->setOrientation(Qt::Horizontal);
    loadingBar_->setMinimum(0);
    loadingBar_->setMaximum(100);
    loadStackWidget_->addWidget(loadingBar_);

    timer_ = new QTimer;
    loadingLabel_ = new QLabel;
//    loadStackWidget_->addWidget(loadingLabel_);
//    timer_->setInterval(500);    // 设定超时时间100毫秒
//    connect(timer_, SIGNAL(timeout()), this, SLOT(updateLoading()));


}

void FileTransfer::initDirFile_(const QStringList &list)
{
    if(list.empty())
        return ;

    if(rootFolder_)
        delete rootFolder_;
    rootFolder_ = new Folder("");
    curFolder_ = rootFolder_;

    for(auto &line : list){
        setupDirFile(line.split("/", QString::SkipEmptyParts), 0, rootFolder_);
    }

    addFile2Stack(rootFolder_);
}

void FileTransfer::setupDirFile(const QStringList& list, int idx, Folder* parent)
{
    if(idx >= list.length())
        return;

    QString filename = list[idx];
    FileAbstract* item = parent->getSubFile(filename);
    if(!item){      // 新文件加入
        if(isDir(filename)){        // 文件夹
            Folder* subFolder = new Folder(filename, parent);
            parent->addSubFile(subFolder);
            connect(subFolder, SIGNAL(clicked(Folder*)), this, SLOT(addFile2Stack(Folder*)));
            setupDirFile(list, ++idx, subFolder);
        }
        else{                       // 普通文件
            File* subFile = new File(filename, parent);
            connect(subFile, SIGNAL(clicked(File*)), this, SLOT(onFileClicked_(File*)));
            connect(subFile, SIGNAL(downLoad(File*)), this, SLOT(onClickedDownloadButton(File*)));
            parent->addSubFile(subFile);
        }
    }
    else if(item->isFolder()){
            setupDirFile(list, ++idx, dynamic_cast<Folder*>(item));
    }
}

void FileTransfer::addFile2Stack(Folder* parent)
{
    curFolder_ = parent;
    QWidget* subWidget = new QWidget;
    FlowLayout* subLayout = new FlowLayout;
    subWidget->setLayout(subLayout);

    QVector<FileAbstract*>* subFile = parent->getSubFile();
    if(subFile->size() > 0){
        QVector<FileAbstract*>::iterator iter;
        for(iter = subFile->begin(); iter!=subFile->end(); iter++){
            if((*iter)->isFolder()){
                subLayout->addWidget(dynamic_cast<Folder*>(*iter));
            }
            else if(!(*iter)->isFolder()){
                subLayout->addWidget(dynamic_cast<File*>(*iter));
            }
        }
    }
    onSwitchWidget(subWidget);
}

//void FileTransfer::showToolWidget()
//{
//    QPropertyAnimation *animation1 = new QPropertyAnimation(toolWidget, "geometry");
//    QPropertyAnimation *animation2 = new QPropertyAnimation(toolWidget, "windowOpacity");

//    QRect startRect, endRect;
//    int startOpacity, endOpacity;

//    if(toolWidget->isHidden()){
//        toolWidget->show();
//        disconnect(animation1, SIGNAL(finished()), toolWidget, SLOT(hide()));
//        startRect.setRect(0, this->height(), toolWidget->width(), toolWidget->height());
//        endRect.setRect(0, this->height() - toolWidget->height(), toolWidget->width(), toolWidget->height());
//        startOpacity = 0;
//        endOpacity = 1;
//    }
//    else{
//        startRect.setRect(0, this->height() - toolWidget->height(), toolWidget->width(), toolWidget->height());
//        endRect.setRect(0, this->height(), toolWidget->width(), toolWidget->height());
//        connect(animation1, SIGNAL(finished()), toolWidget, SLOT(hide()));
//        startOpacity = 1;
//        endOpacity = 0;
//    }
//    animation1->setDuration(1000);
//    animation1->setStartValue(startRect);
//    animation1->setEndValue(endRect);
//    animation2->setDuration(1000);
//    animation2->setStartValue(startOpacity);
//    animation2->setEndValue(endOpacity);

//    QParallelAnimationGroup *group = new QParallelAnimationGroup;
//    group->addAnimation(animation1);
//    group->addAnimation(animation2);
//    group->start();
//}

void FileTransfer::onClickedDownloadButton()
{
    if(!file_)
        return;

    if(file_->getContent().isEmpty()){
        onFileClicked_(file_);
    }

    QString dirpath = QDir::currentPath() + file_->getDirPath();
    qDebug() << dirpath;

    QDir dir;
    if(!dir.mkpath(dirpath))
        return;

    QFile *qfile = new QFile(dirpath + "/" + file_->getFileName());
    if(!qfile->open(QFile::WriteOnly))
    {
        qDebug()<<"client：open file error!";
        qDebug() << "path:" << dirpath + "/" + file_->getFileName();
        return;
    }
    qfile->write(file_->getContent());
    qfile->close();

//    showToolWidget();
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
        QString filepath = curFolder_->getFilePath() + '/' + filename.split('/').back();
        tcp_->addHeader("CustomCode", "UPLOAD_FILE");
        tcp_->sendFile(filepath, file->readAll());
        tcp_->write("POST");
    }
    file->close();

//    showToolWidget();
}

void FileTransfer::onFileClicked_(File* file)
{
    file_ = file;
    qDebug() << "file name: " << file_->getFileName();
    sendFilePath_(file_->getFilePath()); // 就从服务器获取内容
    startLoading();
}

void FileTransfer::showFileContent()
{
    QWidget* subWidget = new QWidget;
    QVBoxLayout* layout = new QVBoxLayout;
    subWidget->setLayout(layout);

    QPushButton* downloadButton = new QPushButton("下载");
    connect(downloadButton, SIGNAL(clicked()), this, SLOT(onClickedDownloadButton()));

    if(file_->getFileType() == FileType::TEXT) {
        QTextEdit* text = new QTextEdit;
        text->setReadOnly(true);
        text->setText(file_->getContent());
        layout->addWidget(text);
    }
    else if(file_->getFileType() == FileType::IMAGE){
        QPixmap img;
        if(img.loadFromData(file_->getContent())){
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
    else if(file_->getFileType() == FileType::VIDEO){

    }
    else if(file_->getFileType() == FileType::UNKNOW){
    }

    layout->addWidget(downloadButton);
    loadStackWidget_->addWidget(subWidget);
    loadStackWidget_->setCurrentWidget(subWidget);
}

void FileTransfer::initTcp_()
{
    tcp_ = new TcpSocket;
    QThread *thread = new QThread;
    tcp_->moveToThread(thread);

    connect(thread, SIGNAL(started()), this, SLOT(connectServer_()));
    connect(tcp_, SIGNAL(connected()), this, SLOT(getRootFile_()));
    connect(tcp_, SIGNAL(readFinished()), this, SLOT(parseDatafromServer()));
    connect(tcp_, SIGNAL(error(int, QString)), this, SLOT(errorDisplay(int, QString)));

    thread->start();
}

void FileTransfer::connectServer_()
{
    tcp_->tcpInit();
    tcp_->connectServer("192.168.43.153", 8888);
}

void FileTransfer::sendFilePath_(QString path)
{
    QString content = "FilePath=" + path + "&";
    tcp_->addHeader("CustomCode", "DOWNLOAD_FILE");
    tcp_->sendMsg(content);
    tcp_->write("POST");
}

void FileTransfer::getRootFile_()
{
    tcp_->addHeader("CustomCode", "GET_SHAREDIR");
    tcp_->sendMsg("");
    tcp_->write("GET");
}

void FileTransfer::parseDatafromServer()
{
    int code = customCode[tcp_->get("CustomCode")];
    switch (code)
    {
    case 0:
        initDirFile_(QString(tcp_->get("ShareDir")).split(";"));
        break;
    case 1:
        file_->setContent(tcp_->get("FileContent"));
        stopLoading();
        showFileContent();
        break;
    }
}

void FileTransfer::setProgressBarValue(int value)
{
    float total = tcp_->get("Content-Length").toInt() + tcp_->get("File-Length").toInt();
    float provalue = value / total * 100;
    loadingBar_->setValue(provalue);
}

void FileTransfer::startLoading()
{
    onSwitchWidget(loadWidget_);
    connect(tcp_, SIGNAL(readedLength(int)), this, SLOT(setProgressBarValue(int)));
//    startTimer();
}

void FileTransfer::stopLoading()
{
//    stopTimer();
    disconnect(tcp_, SIGNAL(readedLength(int)), this, SLOT(setProgressBarValue(int)));
    loadStackWidget_->removeWidget(loadingBar_);
}

void FileTransfer::startTimer()
{
    timer_->start();
}

void FileTransfer::stopTimer()
{
    timer_->stop();
}

bool FileTransfer::isDir(const QString &filename)
{
    if(filename.count('.') > 0)
        return false;
    return true;
}

void FileTransfer::updateLoading()
{
    // 若当前图标下标超过8表示到达末尾，重新计数。
    loadingIndex_++;
    if (loadingIndex_ > 50)
        loadingIndex_ = 0;
//    loadingBar_->setValue(loadingIndex_);

    QPixmap pixmap(QString(":/loading/resources/loading/loading-%1.jpg").arg(loadingIndex_));
    loadingLabel_->setPixmap(pixmap);
}


void FileTransfer::contextMenuEvent(QContextMenuEvent *event)
{
    QMenu *menu = new QMenu(this);
    menu->addAction(upLoadAct_);

    menu->move(cursor().pos());
    menu->show();
}

void FileTransfer::onSwitchWidget(QWidget* widget)
{
    // 首次加载则不加载滑动动画
    if(stackWidget_->count() == 0){
        stackWidget_->addWidget(widget);
        return;
    }

    // 当前界面
    QPropertyAnimation *animation1 = new QPropertyAnimation(stackWidget_->currentWidget(),"geometry");
    animation1->setDuration(200);
    animation1->setStartValue(QRect(0, 0, this->width(), this->height()));
    animation1->setEndValue(QRect(-this->width(), 0, this->width(), this->height()));

    preIndex_ = stackWidget_->currentIndex();
    if(stackWidget_->indexOf(widget) == -1)     // 不在stackwidget则添加
        stackWidget_->addWidget(widget);
    stackWidget_->setCurrentWidget(widget);
    widget->show();    // 确保当前界面在动画结束前不被隐藏

    if(stackWidget_->currentIndex() < preIndex_)
        removeFlag_ = true;    // 若返回上一级界面则删除当前界面
    else
        removeFlag_ = false;   // 若返回下一级界面则不删除当前界面

    // 下一个界面
    QPropertyAnimation *animation2 = new QPropertyAnimation(widget, "geometry");
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
    // 隐藏或移除上一个画面
    if(preIndex_ >= 0){
        QWidget* preWidget = stackWidget_->widget(preIndex_);
        if(removeFlag_){
            stackWidget_->removeWidget(preWidget);
        }
        else
            preWidget->hide();
    }
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
            int cur_index = stackWidget_->currentIndex();
            if(cur_index <= 0){   // 返回主界面
//                MainWindow::getInstance()->onSwitchWidget(MainWindow::getInstance()->getMainWidget());
                MainWindow::getInstance()->onSwitchPage(MainWindow::MainWindowPage);
            }
            else {    // 其他界面返回
                onSwitchWidget(stackWidget_->widget(--cur_index));
            }
            return true;
        }
        break;
    default:
        break;
    }
    return QWidget::event(event);
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
