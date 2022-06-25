#include "minheap.h"

MinHeap::MinHeap(quint32 size){
    this->m_root = new Node*[size];
    this->m_size = 0;
}

MinHeap::~MinHeap(){
    delete []this->m_root;
}

void MinHeap::push(Node *newNode){

    //自下而上调整。当父节点比新结点大时，父节点向下调整，直到currIndx == 0为止
    quint32 currIndx = m_size++;
    while(currIndx > 0 && *m_root[(currIndx-1)/2] > *newNode){
        m_root[currIndx]  = m_root[(currIndx-1)/2];
        currIndx  =  (currIndx-1)/2;
    }
    m_root[currIndx] =  newNode;
}

void MinHeap::pop(){
    Node * lastNode =  m_root[m_size-1];
    m_size--;
    //自上而下调整。当子结点比尾结点小是，子结点向上调整，直到currIndx指向叶子结点
    //当currIndx >= m_size 表明该结点没有左结点，完全二叉树没有左结点一定是叶子结点
    quint32 currIndx = 0;
    quint32 childIndx = 1;
    while(currIndx*2+1 < m_size){
        //判断较小子结点
        if(*m_root[childIndx] > *m_root[childIndx+1]){
            childIndx += 1;
        }
        if(*m_root[childIndx] > *lastNode){
            break;
        }
        m_root[currIndx] = m_root[childIndx];
        currIndx = childIndx;
        childIndx = childIndx*2 +1;
    }
    m_root[currIndx] = lastNode;
}

Node *MinHeap::top(){
    return this->m_root[0];
}

quint64 MinHeap::getSize(){
    return this->m_size;
}
