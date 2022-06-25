#ifndef MINHEAP_H
#define MINHEAP_H

#include "node.h"
#include <QObject>
#include <QString>

//priority queue
class MinHeap
{
private:
    Node    **m_root;
    qint32  m_size;
public:
    MinHeap(quint32 size);
    ~MinHeap();
    void push(Node *node);
    void pop();
    Node *top();
    quint64 getSize();
};

#endif // MINHEAP_H
