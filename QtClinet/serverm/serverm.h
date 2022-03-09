#ifndef SERVERMONITORING_H
#define SERVERMONITORING_H

#include <QWidget>
#include <QLabel>

class ServerM : public QWidget
{
    Q_OBJECT
public:
    static ServerM* getInstance();

private:
   ServerM(QWidget *parent = nullptr);
   void init_();

   static ServerM* instance_;
   QLabel* label;

};

#endif // SERVERMONITORING_H
