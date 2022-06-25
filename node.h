#ifndef NODE_H
#define NODE_H

#include <QObject>
#include <QString>
struct Node
{
public:
    quint8  data;
    quint32 frequence;
    Node *lchild;
    Node *rchild;
public:
    bool operator>(Node &other);
};

#endif // NODE_H
