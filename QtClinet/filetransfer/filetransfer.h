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
#include <QLayout>
#include <QFile>
#include <QFileDialog>
#include <QMessageBox>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QStackedWidget>
#include <QProgressBar>
#include <QPropertyAnimation>
#include <QParallelAnimationGroup>

#include "filebutton.h"
#include "tcp/tcpsocket.h"
#include "toolbutton.h"

class FileTransfer : public QWidget
{
    Q_OBJECT
public:
    static FileTransfer* getInstance();
    bool isDir(const QString& filename_);
    void initDirFile_(const QStringList& list);

    bool event(QEvent *event) override;
    void contextMenuEvent(QContextMenuEvent *event) override;

private slots:
    // 界面切换
    void addFile2Stack(Folder* parent);
    void onSwitchWidget(QWidget* widget);
    void onAnimationFinished();
    void showFileContent();
//    void showToolWidget();
    void onClickedUploadButton();
    void onClickedDownloadButton();

    // 网络响应
    void connectServer_();
    void onFileClicked_(File* file);
    void sendFilePath_(QString filename);
    void getRootFile_();
    void parseDatafromServer();
    void errorDisplay(int socketError, const QString& message);

    // 动画加载
    void setProgressBarValue(int value);
    void startLoading();
    void stopLoading();
    void updateLoading();
    void startTimer();
    void stopTimer();

private:
    FileTransfer(QWidget *parent = nullptr);
    void initGUI_();
    void setupDirFile(const QStringList& lines, int idx, Folder* parent);
    void initTcp_();

    static FileTransfer* instance_;
    const static QMap<QString, int> customCode;

    QStackedWidget* stackWidget_;
    QVBoxLayout* layout_;

    // 下载相关
//    QWidget* toolWidget;
//    QPushButton* uploadButton_;
//    QPushButton* downloadButton_;
//    ToolButton* toolButton_;
    QAction* upLoadAct_;

    // 加载相关
    QWidget* loadWidget_;
    QVBoxLayout* loadLayout_;
    QStackedWidget* loadStackWidget_;
    QProgressBar* loadingBar_;
    QTimer* timer_;
    QLabel* loadingLabel_;
    int loadingIndex_;

    Folder* rootFolder_;  // 根目录
    File* file_;        // 打开的文件
    Folder* curFolder_;
    int preIndex_;      // 上一级界面索引
    bool removeFlag_;  // 是否删除移除的画面
    TcpSocket* tcp_;

};

#endif // FILECONTROL_H
