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

void FileAbstract::setFileName(QString filename)
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

Folder::Folder(const QString& foldername)
{
    setFileName(foldername);
    connect(this, SIGNAL(clicked()), this, SLOT(onClicked()));  
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
}

void Folder::onClicked()
{
    emit(clicked(this));
}

File::File(const QString& filename)
{
    setFileName(filename);
    connect(this, SIGNAL(clicked()), this, SLOT(onClicked()));

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

void File::setContent(QByteArray content)
{
    content_ = content;
}

QByteArray File::getContent() const
{
    return content_;
}

void File::onClicked()
{
    emit clicked(this);
}
