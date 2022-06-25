#include "exception.h"

Exception::Exception(qint32 code, QString info){
    this->code = code;
    this->info = info;
}

quint32 Exception::getCode(){
    return code;
}

void Exception::print(){

}

QString Exception::getFormatString(){
    return QString("错误代码: %1\n错误信息: %2").arg(code).arg(info);
}
