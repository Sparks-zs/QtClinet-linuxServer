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
    void setFileName(const QString& filename);
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
    explicit Folder(const QString& foldername, Folder *parent=nullptr);
    ~Folder();
    QVector<FileAbstract*>* getSubFile();
    FileAbstract* getSubFile(const QString& foldername);
    bool addSubFile(FileAbstract* child);
//    void pushFileName(QString path);
    QString getFilePath();
    Folder* parent() {return parent_;}
    QString getDirPath();

signals:
    void clicked(Folder*);

protected slots:
    void onClicked();

private:
    QVector<FileAbstract*> subFile_;
    Folder* parent_;
};

class File : public FileAbstract
{
    Q_OBJECT
public:
    explicit File(const QString& filename, Folder *parent=nullptr);
    void setContent(const QByteArray& content);
    const QByteArray& getContent() const;
    int getContentType();
    QString getFilePath();
    QString getDirPath();

signals:
    void clicked(File*);
    void downLoad(File*);

private slots:
    void clicked();

private:
    QByteArray content_;     // 文件内容
    Folder* parent_;
};



#endif // FILEBUTTON_H
