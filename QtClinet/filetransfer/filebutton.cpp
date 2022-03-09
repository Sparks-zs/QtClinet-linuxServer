#include "filebutton.h"

const QMap<QString, int> FileAbstract::SUFFIX_TYPE = {
    { ".html",  FileType::TEXT },
    { ".xml",   FileType::TEXT },
    { ".xhtml", FileType::TEXT },
    { ".txt",   FileType::TEXT },
    { ".rtf",   FileType::TEXT },
    { ".pdf",   FileType::TEXT },
    { ".word",  FileType::TEXT },
    { ".png",   FileType::IMAGE },
    { ".jpg",   FileType::IMAGE },
    { ".jpeg",  FileType::IMAGE },
    { ".mpeg",  FileType::VIDEO },
    { ".mpg",   FileType::VIDEO },
    { ".avi",   FileType::VIDEO },
};

FileAbstract::FileAbstract(QWidget* parent)
    : QPushButton(parent)
{
   setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
}

void FileAbstract::setFileName(const QString &filename)
{
    fileName_ = filename;
    setText(filename);
}

int FileAbstract::getFileType()
{
    QString suffix = '.' + fileName_.split('.').back();
    if(SUFFIX_TYPE.count(suffix) == 0)
        return FileType::UNKNOW;
    return SUFFIX_TYPE[suffix];
}

bool FileAbstract::isFolder()
{
    if(fileName_.count('.') > 0)
        return false;
    return true;
}

Folder::Folder(const QString& foldername, Folder* parent)
    : parent_(parent)
{
    setFileName(foldername);
    connect(this, SIGNAL(clicked()), this, SLOT(onClicked()));  

    subFile_.reserve(5);
    setStyleSheet(
        "Folder{"
        "background:transparent;"
        "background-image:url(:/images/folder.png);"    //背景色
        "height:200px;"
        "width:200px;"
        "text-align: bottom;"
        "}"
    );
}

Folder::~Folder()
{
    subFile_.clear();
}

void Folder::onClicked()
{
    emit(clicked(this));
}
bool Folder::addSubFile(FileAbstract *child)
{
    if(subFile_.count(child) > 0)
        return false;
    subFile_.push_back(child);
    return true;
}

QVector<FileAbstract*>* Folder::getSubFile()
{
    return &subFile_;
}

FileAbstract* Folder::getSubFile(const QString& filename)
{
    if(subFile_.size() == 0)
        return nullptr;
    QVector<FileAbstract*>::iterator iter;
    for(iter = subFile_.begin(); iter!=subFile_.end(); iter++){
        if((*iter)->getFileName() == filename)
            return *iter;
    }
    return nullptr;
}

QString Folder::getFilePath()
{
    QString filename = getFileName();
    if(filename == "")
        return "";

    QString path;
    path.push_front(getFileName());
    path.push_front("/");

    Folder* parent = parent_;
    while(parent){
        path.push_front(parent->getFileName());
        if(parent->parent())
            path.push_front("/");
        parent = parent->parent();
    };

    return path;
}

QString Folder::getDirPath()
{
    QString path;
    Folder* parent = parent_;
    while(parent){
        path.push_front(parent->getFileName());
        if(parent->parent())
            path.push_front("/");
        parent = parent->parent();
    };
    return path;
}

File::File(const QString& filename, Folder *parent)
    : parent_(parent)
{
    setFileName(filename);
    connect(this, SIGNAL(clicked()), this, SLOT(clicked()));

    setStyleSheet(
        "File{"
        "background:transparent;"
        "background-image:url(:/images/file.png);"    //背景色
        "height:200px;"
        "width:200px;"
        "text-align: bottom;"
        "}"
    );


}

void File::setContent(const QByteArray &content)
{
    content_ = content;
}

const QByteArray& File::getContent() const
{
    return content_;
}

void File::clicked()
{
    emit clicked(this);
}

QString File::getFilePath()
{
    QString path;
    path.push_front(getFileName());
    path.push_front("/");
    path.push_front(getDirPath());

    return path;
}

QString File::getDirPath()
{
    QString path;
    Folder* parent = parent_;
    while(parent){
        path.push_front(parent->getFileName());
        if(parent->parent())
            path.push_front("/");
        parent = parent->parent();
    };
    return path;
}
