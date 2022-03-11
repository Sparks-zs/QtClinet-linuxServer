#ifndef FILEBUTTON_H
#define FILEBUTTON_H

#include <QIcon>
#include <QWidget>
#include <QPushButton>
#include <QVector>
#include <QString>
#include <QMouseEvent>
#include <QMenu>
#include <QAction>
#include <QMap>

typedef enum{
    UNKNOW = -1,
    TEXT,
    IMAGE,
    VIDEO
}FileType;


class FileAbstract : public QPushButton
{
Q_OBJECT
public:
    FileAbstract(QWidget* parent=nullptr);
    void setFileName(QString filename);
    QString getFileName() { return fileName_; }
    int getFileType();
    bool isFolder();

private:
    QString fileName_;

    static const QMap<QString, int> SUFFIX_TYPE;
};


class Folder : public FileAbstract
{
    Q_OBJECT
public:
    explicit Folder(const QString& foldername);
    ~Folder();

signals:
    void clicked(Folder*);

private slots:
    void onClicked();
};

class File : public FileAbstract
{
    Q_OBJECT
public:
    explicit File(const QString& filename);
    void setContent(QByteArray content);
    QByteArray getContent() const;
    int getContentType();

signals:
    void clicked(File*);

private slots:
    void onClicked();

private:
    QByteArray content_;

};



#endif // FILEBUTTON_H
