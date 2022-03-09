#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QEvent>
#include <QKeyEvent>
#include <QWidget>
#include <QStackedWidget>
#include <QCoreApplication>
#include <QPropertyAnimation>
#include <QParallelAnimationGroup>

#include "mylayout.h"
#include "switchbutton.h"
#include "../filetransfer/filetransfer.h"
#include "../serverm/serverm.h"

class MainWindow : public QMainWindow
{
    Q_OBJECT
public:

    typedef enum{
        MainWindowPage = 0,
        FileWidgetPage,
        ServerMPage
    }PageIndex;

    static MainWindow* getInstance();
    ~MainWindow() = default;
    int getCurrentPageIndex() { return m_stackWidget->currentIndex(); }

public slots:
    void onSwitchWidget(QWidget* widget);
    void onSwitchPage(int index);

protected slots:
    void onAnimationFinished();
    bool event(QEvent *event) override;

private:
    MainWindow(QWidget *parent = nullptr);
    static MainWindow* instance_;
    void init();

    QStackedWidget* m_stackWidget;
    QWidget* m_widget;
    MyLayout* m_layout;
    SwitchButton* m_fileTransfer;
    SwitchButton* m_serverM;

    int m_preIndex;     //上一个界面索引
    int m_pageIndex;    // 当前显示的界面

};
#endif // MAINWINDOW_H
