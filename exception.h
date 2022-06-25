#ifndef EXCEPTION_H
#define EXCEPTION_H
#include <QString>
#include <QObject>

#define FAIL_TO_OPEN_FILE 1
class Exception
{
private:
    QString info;
    quint32 code;
public:
    Exception(qint32, QString info);
    quint32 getCode();
    void print();
    QString getFormatString();
};

#endif // EXCEPTION_H
