#include "controlpannel.h"
#include "ui_controlpannel.h"
#include "exception.h"
#include <string>
#include <QMessageBox>
#include <QFileDialog>
#include <QFile>
ControlPannel::ControlPannel(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::ControlPannel)
{
    ui->setupUi(this);
    this->fileName.clear();
    this->frequence.resize(256);
    connect(ui->openFileBtn, &QToolButton::clicked, this, [this](){
        QString fileName =
                QFileDialog::getOpenFileName(NULL,"请选择打开文件","/home/vick","*");
        if(fileName.isEmpty()){
            QMessageBox::information(NULL,"提示","请选择压缩文件保存位置");
            return;
        }
        ui->fileNameLid->setText(fileName);
        this->fileName = fileName;
    });

    connect(ui->compressBtn, &QToolButton::clicked, this, [this](){
        QString outFileName =
                QFileDialog::getSaveFileName(NULL, "请选择保存位置", "/home/vick","*");
        if(outFileName.isEmpty()){
            QMessageBox::critical(NULL,"错误","请选择压缩文件保存位置");
            return;
        }
        try{
            compress(outFileName);
        }catch (Exception &e){
            if(e.getCode() == 1){
                QMessageBox::critical(NULL,"错误","文件打开失败");
            }
        }
    });

    connect(ui->decompressBtn, &QToolButton::clicked,this,[this](){
        QString outFileName =
                QFileDialog::getSaveFileName(NULL, "请选择保存位置", "/home/vick","*");
        if(outFileName.isEmpty()){
            QMessageBox::critical(NULL,"错误","请选择解压缩文件保存位置");
            return;
        }
        try{
            decompress(outFileName);
        }catch(Exception &e){
            if(e.getCode() == 1){
                QMessageBox::critical(NULL,"错误","文件打开失败");
            }
        }
    });
}

ControlPannel::~ControlPannel(){
    delete ui;
}

void ControlPannel::compress(QString outFileName){
    //初始化
    QFile file(this->fileName);
    if(!file.open(QIODevice::ReadOnly)){
        throw Exception(FAIL_TO_OPEN_FILE, "can not open file");
    }
    QFile outFile(outFileName);
    if(!outFile.open(QIODevice::WriteOnly)){
        throw Exception(FAIL_TO_OPEN_FILE, "can not open file");
    }
    quint64 totalBytes = 0;
    quint64 bytesToRead = 0;
    quint64 bytesRead = 0;
    quint64 loadSize = 0;
    quint32 readBuffSize = 0;
    quint8 codeBuffSize = 0;
    QByteArray readBuff;
    QByteArray writeBuff;
    QString codeBuff;

    //计算频度
    totalBytes = file.size();
    bytesToRead = totalBytes;
    bytesRead = 0;
    loadSize = 1024*4;
    readBuff.clear();
    while(bytesRead != totalBytes){
        readBuff = file.read(qMin(bytesToRead,loadSize));
        readBuffSize = readBuff.size();
        bytesToRead -= readBuff.size();
        bytesRead += readBuff.size();
        for(int i = 0; i < readBuffSize; ++i){
            frequence[(quint8)readBuff[i]]++;
        }
        readBuff.clear();
    }

    //基于huffman算法获取编码
    constructTree();
    getCodes();

    //编码文件
    file.seek(0);
    totalBytes = file.size();
    bytesToRead = totalBytes;
    bytesRead = 0;
    loadSize = 1024*4;
    readBuffSize = 0;
    codeBuffSize = 0;
    readBuff.clear();
    writeBuff.clear();
    codeBuff.clear();

    quint8 nextByte = 0;
    quint8 bitCounter = 0;
    outFile.write((char*)frequence.data(),sizeof(quint32)*frequence.size());
    while(bytesRead != totalBytes){
        readBuff = file.read(qMin(bytesToRead, loadSize));
        readBuffSize = readBuff.size();
        bytesToRead -= readBuffSize;
        bytesRead += readBuffSize;
        //编码
        for(int i = 0; i < readBuffSize; ++i){
            //编码单字节
            codeBuff = codes[readBuff[i]];
            codeBuffSize = codeBuff.size();
            for(int j = 0; j < codeBuffSize; ++j){
                if(bitCounter == 8){
                    writeBuff.push_back(nextByte);
                    bitCounter = 0;
                    nextByte = 0;
                }
                if(codeBuff[j] == '1'){
                    nextByte |= 0x01 << bitCounter;
                }
                bitCounter++;
            }
        }
        outFile.write(writeBuff);
        readBuff.clear();
        writeBuff.clear();
    }
    if(bitCounter != 0){
        outFile.write((char*)&nextByte,1);
    }
}

void ControlPannel::decompress(QString outFileName){
    //打开文件
    QFile file(this->fileName);
    if(!file.open(QIODevice::ReadOnly)){
        throw Exception(FAIL_TO_OPEN_FILE, "can not open file");
    }
    QFile outFile(outFileName);
    if(!outFile.open(QIODevice::WriteOnly)){
        throw Exception(FAIL_TO_OPEN_FILE, "can not open file");
    }
    quint64 totalBytes = 0;
    quint64 bytesToRead = 0;
    quint64 bytesRead = 0;
    quint64 loadSize = 0;
    quint32 readBuffSize = 0;
    quint8 codeBuffSize = 0;
    QByteArray readBuff;
    QByteArray writeBuff;
    QString codeBuff;

    //获取频度信息
    file.read((char*)frequence.data(),sizeof(quint32)*frequence.size());

    //基于huffman算法获取编码
    constructTree();
    getCodes();
    auto it = codes.begin();
    auto itEnd = codes.end();
    for(; it != itEnd; ++it){
        decodes.insert(it.value(),it.key());
    }

    //解码文件
    totalBytes = file.size()-frequence.size()*sizeof(quint32);
    bytesToRead = totalBytes;
    bytesRead = 0;
    loadSize = 1024*4;
    readBuffSize = 0;
    codeBuffSize = 0;
    readBuff.clear();
    writeBuff.clear();
    codeBuff.clear();

    quint8 nextByte = 0;
    while(bytesRead != totalBytes){
        readBuff = file.read(qMin(bytesToRead,loadSize));
        readBuffSize = readBuff.size();
        bytesToRead -= readBuffSize;
        bytesRead += readBuffSize ;
        for(int i = 0; i < readBuffSize; ++i){
            nextByte = readBuff[i];
            for(int j = 0; j < 8; ++j){
                if((nextByte >> j) & 0x01){
                    codeBuff += '1';
                }else{
                    codeBuff += '0';
                }
                if(decodes.contains(codeBuff)){
                    writeBuff.push_back(decodes[codeBuff]);
                    codeBuff.clear();
                }
            }
        }
        outFile.write(writeBuff);
        writeBuff.clear();
    }
}


void ControlPannel::constructTree(){
    MinHeap  minHeap(256);
    //字符入堆
    for(quint32 i = 0; i < 256; ++i){
        if(frequence[i] != 0){
            minHeap.push(new Node({i,frequence[i],NULL, NULL}));
        }
    }

    //huffman算法建树
    Node *n1;
    Node *n2;
    Node *merged;
    while(minHeap.getSize()>1){
        n1 = minHeap.top();
        minHeap.pop();
        n2 = minHeap.top();
        minHeap.pop();
        merged = new Node({0,n1->frequence+n2->frequence,n1,n2});
        minHeap.push(merged);
    }
    root =  minHeap.top();
}

void ControlPannel::destorysTree(){


}

void ControlPannel::getCodes(){
    struct Param{
        Node *node;
        bool islchild;
        bool lsolved;
        bool rsolved;
    };
    QStack<Param> paramStack;
    QString code;
    Param tmp = {root,false,false,false};
    paramStack.push(tmp);
    while(true){
        Param  &top = paramStack.top();
        if(top.lsolved == false  || top.rsolved == false){
            if(top.node->lchild == NULL && top.node->rchild == NULL){
                top.lsolved = true;
                top.rsolved = true;
                codes[top.node->data] = code;
                continue;
            }
            if(top.lsolved == false){
                code += '0';
                tmp = {top.node->lchild, true,false,false};
                paramStack.push(tmp);
                continue;
            }
            if(top.rsolved == false){
                code += '1';
                tmp = {top.node->rchild, false,false,false};
                paramStack.push(tmp);
                continue;
            }
        }else{
            bool islchild = top.islchild;
            paramStack.pop();
            if(paramStack.isEmpty()){
                break;
            }
            if(islchild){
                paramStack.top().lsolved = true;
            }else{
                paramStack.top().rsolved = true;
            }
            code.remove(code.size()-1,1);
        }
    }
}


