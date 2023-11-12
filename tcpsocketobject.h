#ifndef TCPSOCKETOBJECT_H
#define TCPSOCKETOBJECT_H

#include <QObject>
#include <QTcpSocket>

class QThread;

class TCPSocketObject : public QObject
{
    Q_OBJECT
public:
    TCPSocketObject();
    ~TCPSocketObject();
    QTcpSocket* getSocket();
    void setIP(QString ip);
    void setPort(uint port);
    void connect(QString ip, uint port);
    void connect();
    void disConnect();
    void write(QByteArray array);

signals:
    void sig_disconnected();
    void sig_refused();
    void sig_connected();
    void sig_readyRead(QTcpSocket*);
    void sig_readReadUnDeal();// 需要自己获取Socket进行读取
    void sig_write(QByteArray array);

private slots:
    void slot_ReadRead();
    void slot_doConnect();
    void slot_doDisConnect();
    void slot_write(QByteArray array);
    void initConnection();

private:
    QThread* m_pThread;
    QTcpSocket* m_pSocket;
    QString m_IP;
    uint m_port;

};

#endif // TCPSOCKETOBJECT_H
