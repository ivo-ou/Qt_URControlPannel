#include "tcpsocketobject.h"
#include <QThread>
#include <QNetworkProxy>
#include <QDebug>

TCPSocketObject::TCPSocketObject()
    : m_pSocket(nullptr)
{
    m_pThread = new QThread();

    QNetworkProxyFactory::setUseSystemConfiguration(false);     // 解决虚拟机下无法连接
}

TCPSocketObject::~TCPSocketObject()
{
    if(m_pThread->isRunning())
    {
        m_pThread->exit(0);
        while (m_pThread->isRunning()) {

        }
        delete m_pThread;
        qDebug() <<"delete thread";
    }
}

QTcpSocket* TCPSocketObject::getSocket()
{
    return m_pSocket;
}

void TCPSocketObject::setIP(QString ip)
{
    m_IP = ip;
}

void TCPSocketObject::setPort(uint port)
{
    m_port = port;
}

void TCPSocketObject::connect(QString ip, uint port)
{
    this->setIP(ip);
    this->setPort(port);
    this->connect();
}

void TCPSocketObject::connect()
{
    this->moveToThread(m_pThread);
    QObject::connect(m_pThread, &QThread::started, this, &TCPSocketObject::slot_doConnect);
    QObject::connect(m_pThread, &QThread::finished, this, &TCPSocketObject::slot_doDisConnect);
    m_pThread->start();
}

void TCPSocketObject::disConnect()
{
//    m_pSocket->abort();
//    while (m_pSocket->state() == QTcpSocket::ConnectedState) {

//    }
    //emit sig_disconnected();
    m_pThread->quit();
}

void TCPSocketObject::write(QByteArray array)
{
//    if( m_pSocket != nullptr)
//        m_pSocket->write(array);
    emit sig_write(array);
}

void TCPSocketObject::slot_ReadRead()
{

    emit sig_readyRead(m_pSocket);
}

void TCPSocketObject::slot_doConnect()
{
    m_pSocket = new QTcpSocket;
    this->initConnection();


    m_pSocket->connectToHost(m_IP, m_port);
    if(!m_pSocket->waitForConnected())
    {
        emit sig_refused();
        m_pThread->quit();
    }
}

void TCPSocketObject::slot_doDisConnect()
{
    m_pSocket->disconnectFromHost();
}

void TCPSocketObject::slot_write(QByteArray array)
{
    if( m_pSocket != nullptr)
        m_pSocket->write(array);
}

void TCPSocketObject::initConnection()
{
    QObject::connect(m_pSocket, &QTcpSocket::readyRead,this, &TCPSocketObject::slot_ReadRead);
    QObject::connect(m_pSocket, &QTcpSocket::disconnected,this,&TCPSocketObject::sig_disconnected);
    QObject::connect(m_pSocket, &QTcpSocket::connected,this,&TCPSocketObject::sig_connected);
    QObject::connect(this, &TCPSocketObject::sig_write,this,&TCPSocketObject::slot_write);
    //QObject::connect(this, SIGNAL(sig_write(QByteArray)),m_pSocket, SLOT(write(const char*)));
}
