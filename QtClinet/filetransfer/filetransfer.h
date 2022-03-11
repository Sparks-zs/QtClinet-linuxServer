#ifndef FILECONTROL_H
#define FILECONTROL_H

#include <QWidget>
#include <QLabel>
#include <QMap>
#include <QMenu>
#include <QAction>
#include <QByteArray>
#include <QTimer>
#include <QPixmap>
#include <QTextEdit>
#include <QFile>
#include <QFileDialog>
#include <QMessageBox>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QStackedWidget>
#include <QProgressBar>
#include <QPropertyAnimation>
#include <QParallelAnimationGroup>

#include "flowlayout.h"
#include "filebutton.h"
#include "tcp/tcpsocket.h"
#include "toolbutton.h"

class FileTransfer : public QWidget
{
    Q_OBJECT
public:
    FileTransfer(QWidget *parent = nullptr);
    ~FileTransfer();
    void initGUI_();
    void initTcp_();
    void setupDirectory(const QStringList& lines);

private slots:
    // 界面切换
    void onSwitchWidget(QWidget* widget);
    void onAnimationFinished();
    void showFileContent();

    // 网络响应
    void connectServer_();
    void onConnected();
    void getDirectoryFromServer(const QString& path);    // 从服务器获取目录
    void onClickedFile_(File* file);
    void onClickedFolder_(Folder* folder);
    void parseDatafromServer();
    void errorDisplay(int socketError, const QString& message);
    void onClickedUploadButton();
    void onClickedDownloadButton();

    QString getCurPath();   // 当前目录
    QString getUpPath();    // 上一个目录
    bool isDir(const QString& filename_);
    QString joinPath(const QString& p1, const QString& p2);
    bool event(QEvent *event) override;
    void contextMenuEvent(QContextMenuEvent *event) override;

private:
    QVBoxLayout* layout_;
    QAction* upLoadAct_;
    QWidget* curWidget_;
    QWidget* prevWidget_;
    QStackedWidget* stackedWidget_;
    bool isFileContent;     // 当前显示页面为文件内容
    TcpSocket* tcp_;
    File* curFile_;        // 打开的文件
    QVector<QString> curPath_;   // 当前界面路径

    // 自定义协议
    const static QMap<QString, int> customCode;
};

#endif // FILECONTROL_H
