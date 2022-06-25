#ifndef CONTROLPANNEL_H
#define CONTROLPANNEL_H
#include "minheap.h"
#include "node.h"
#include <QVector>
#include <QObject>
#include <QWidget>
#include <QMap>
#include <QStack>
QT_BEGIN_NAMESPACE
namespace Ui { class ControlPannel; }
QT_END_NAMESPACE

class ControlPannel : public QWidget
{
    Q_OBJECT
private:
    Ui::ControlPannel *ui;
    QString     fileName;
    Node *root;
    QVector<quint32> frequence;
    QMap<quint8, QString> codes;
    QMap<QString, quint8> decodes;
public:
    ControlPannel(QWidget *parent = nullptr);
    ~ControlPannel();
    void compress(QString outFileName);
    void decompress(QString outFileName);
    void constructTree();
    void destorysTree();
    void getCodes();

};
#endif // CONTROLPANNEL_H
