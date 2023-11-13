#include "urcontrolpannel.h"
#include "ui_urcontrolpannel.h"

#include <QDateTime>

#define maxSpeed 0.02  // mm/s
#define maxAcc 0.5
#define maxRadSpeed maxSpeed*5  // rad/s
#define maxRadAcc maxAcc * 2

#include <QDebug>
URControlPannel::URControlPannel(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::URControlPannel)
    , m_pTCPSocketObject_RealTime(nullptr)
    , m_pTCPSocketObject_Dashboard(nullptr)
{
    ui->setupUi(this);
    m_pTCPSocketObject_RealTime = new TCPSocketObject;
    m_pTCPSocketObject_Dashboard = new TCPSocketObject;

    this->initConnection();
    ui->lineEdit_IP->setText("192.168.49.128");
    ui->horizontalSlider_MoveSpeed->setValue(100);
    ui->horizontalSlider_Accelerations->setValue(100);
}

URControlPannel::~URControlPannel()
{
    if( m_pTCPSocketObject_RealTime != nullptr)
        delete m_pTCPSocketObject_RealTime;
    if( m_pTCPSocketObject_Dashboard != nullptr)
        delete m_pTCPSocketObject_Dashboard;
    delete ui;
}

void URControlPannel::mousePressEvent(QMouseEvent *event)
{
    if(event->button() == Qt::LeftButton)
    {
        mLazyUpdate = true;
    }
    QWidget::mousePressEvent(event);
}

void URControlPannel::mouseReleaseEvent(QMouseEvent *event)
{
    if(event->button() == Qt::LeftButton)
    {
        mLazyUpdate = false;
    }
    QWidget::mouseReleaseEvent(event);
}

void URControlPannel::initConnection()
{
    connect(m_pTCPSocketObject_RealTime, &TCPSocketObject::sig_connected, this, &URControlPannel::slot_sockedConnect);
    connect(m_pTCPSocketObject_RealTime, &TCPSocketObject::sig_disconnected, this, &URControlPannel::slot_socketDisConnect);
    connect(m_pTCPSocketObject_RealTime, &TCPSocketObject::sig_readyRead, this, &URControlPannel::slot_socketReadReady_RealTime);
    connect(m_pTCPSocketObject_Dashboard, &TCPSocketObject::sig_connected, this, &URControlPannel::slot_sockedConnect);
    connect(m_pTCPSocketObject_Dashboard, &TCPSocketObject::sig_disconnected, this, &URControlPannel::slot_socketDisConnect);
    connect(m_pTCPSocketObject_Dashboard, &TCPSocketObject::sig_readyRead, this, &URControlPannel::slot_socketReadReady_Dashboard);
}

void URControlPannel::praseHexData(QByteArray array)
{
    // https://www.universal-robots.cn/articles/ur/interface-communication/remote-control-via-tcpip/
    QByteArray msgLengh = ArraySub(array, 0, 4);
    int msglengh = QByteArrayToInt(msgLengh);
    if (msglengh == array.size())
    {

        // 状态懒刷新 8ms * 50 = 0.4s刷新一次
        static int lazy_update = 0;
        if((lazy_update == 50 && mLazyUpdate) || !mLazyUpdate)
        {
            auto parseData = PraseArray(array.toHex());
            // 状态返回
            if( parseData.size() >=60)
            {
                // 坐标
                mURPosition.SetX(parseData[55]);
                mURPosition.SetY(parseData[56]);
                mURPosition.SetZ(parseData[57]);
                mURPosition.SetRX(parseData[58]);
                mURPosition.SetRY(parseData[59]);
                mURPosition.SetRZ(parseData[60]);
            }

            // Joint
            if( parseData.size() >=36)
            {
                mURPosition.SetJoint1(parseData[31]);
                mURPosition.SetJoint2(parseData[32]);
                mURPosition.SetJoint3(parseData[33]);
                mURPosition.SetJoint4(parseData[34]);
                mURPosition.SetJoint5(parseData[35]);
                mURPosition.SetJoint6(parseData[36]);
                this->updateURPosition();
            }
            // 坐标速度
            if( parseData.size() >=66)
            {
                ui->label_x_speed->setText(QString::number(parseData[61], 'f', 3));
                ui->label_y_speed->setText(QString::number(parseData[62], 'f', 3));
                ui->label_z_speed->setText(QString::number(parseData[63], 'f', 3));
                ui->label_rx_speed->setText(QString::number(parseData[64], 'f', 3));
                ui->label_ry_speed->setText(QString::number(parseData[65], 'f', 3));
                ui->label_ry_speed->setText(QString::number(parseData[66], 'f', 3));
            }

            //  Joint 温度
            if( parseData.size() >=91)
            {
                ui->label_joint1_temperature->setText(QString::number(parseData[86], 'f', 1));
                ui->label_joint2_temperature->setText(QString::number(parseData[87], 'f', 1));
                ui->label_joint3_temperature->setText(QString::number(parseData[88], 'f', 1));
                ui->label_joint4_temperature->setText(QString::number(parseData[89], 'f', 1));
                ui->label_joint5_temperature->setText(QString::number(parseData[90], 'f', 1));
                ui->label_joint6_temperature->setText(QString::number(parseData[91], 'f', 1));
            }

            //  Joint 电压
            if( parseData.size() >=129)
            {
                ui->label_joint1_voltage->setText(QString::number(parseData[124], 'f', 1));
                ui->label_joint2_voltage->setText(QString::number(parseData[125], 'f', 1));
                ui->label_joint3_voltage->setText(QString::number(parseData[126], 'f', 1));
                ui->label_joint4_voltage->setText(QString::number(parseData[127], 'f', 1));
                ui->label_joint5_voltage->setText(QString::number(parseData[128], 'f', 1));
                ui->label_joint6_voltage->setText(QString::number(parseData[129], 'f', 1));
            }

            //  Joint 电流
            if( parseData.size() > 49)
            {
                ui->label_joint1_current->setText(QString::number(qAbs(parseData[43]), 'f', 1));
                ui->label_joint2_current->setText(QString::number(qAbs(parseData[44]), 'f', 1));
                ui->label_joint3_current->setText(QString::number(qAbs(parseData[45]), 'f', 1));
                ui->label_joint4_current->setText(QString::number(qAbs(parseData[46]), 'f', 1));
                ui->label_joint5_current->setText(QString::number(qAbs(parseData[47]), 'f', 1));
                ui->label_joint6_current->setText(QString::number(qAbs(parseData[48]), 'f', 1));
            }

            // 电流电压
            if( parseData.size() >=123)
            {
                ui->lineEdit_Voltage->setText(QString::number(parseData[122], 'f', 1));
                ui->lineEdit_Current->setText(QString::number(parseData[123], 'f', 1));
            }
            if( parseData.size() >=101)
            {
                ui->label_RobotMode->setText(CreateHtmlStr(RobotMode[parseData[94]], (parseData[94]==3)?QColor(Qt::red):QColor(Qt::black)) );
                ui->label_SafetyMode->setText(CreateHtmlStr(SafetyMode[parseData[101]], (parseData[101]>1)?QColor(Qt::red):QColor(Qt::black)) );
            }
            lazy_update = 0;
        }
        lazy_update++;
    }

    if (!ui->checkBox_Pause_Hex->isChecked())
    {
        this->appendMessage(MessageType::Recive, array.toHex().toUpper());
    }
}

void URControlPannel::sendCommand(QString command, bool dashboard)
{
    QByteArray command_array = command.toUtf8();
    if(dashboard)
    {
        m_pTCPSocketObject_Dashboard->write(command_array);
    }
    else
    {
        m_pTCPSocketObject_RealTime->write(command_array);
    }
    this->appendMessage(MessageType::Send, command_array);
}

void URControlPannel::updateURPosition()
{
    ui->lineEdit_X->setText(mURPosition.GetXStr());
    ui->lineEdit_Y->setText(mURPosition.GetYStr());
    ui->lineEdit_Z->setText(mURPosition.GetZStr());
    ui->lineEdit_RX->setText(mURPosition.GetRXStr());
    ui->lineEdit_RY->setText(mURPosition.GetRYStr());
    ui->lineEdit_RZ->setText(mURPosition.GetRZStr());

    // Joint
    ui->lineEdit_Joint1->setText(mURPosition.GetJoint1Str());
    ui->lineEdit_Joint2->setText(mURPosition.GetJoint2Str());
    ui->lineEdit_Joint3->setText(mURPosition.GetJoint3Str());
    ui->lineEdit_Joint4->setText(mURPosition.GetJoint4Str());
    ui->lineEdit_Joint5->setText(mURPosition.GetJoint5Str());
    ui->lineEdit_Joint6->setText(mURPosition.GetJoint6Str());
}

void URControlPannel::appendMessage(MessageType type, QString message)
{
    QString title = QDateTime::currentDateTime().toString("[yyyy-MM-dd hh:mm:ss:zzz]# ");
    QColor color;
    switch(type)
    {
        case MessageType::Recive:
            title.append(QString("RECV>"));
            color = QColor(Qt::cyan);
            break;
        case MessageType::Send:
            title.append(QString("SEND>"));
            color = QColor(Qt::magenta);
            break;
        case MessageType::Message:
            title.append(QString("MESSAGE>"));
            color = QColor(Qt::blue);
            break;
        case MessageType::Succeed:
            title.append(QString("SUCCEED>"));
            color = QColor(Qt::green);
            break;
        case MessageType::Fail:
            title.append(QString("FAIL>"));
            color = QColor(Qt::red);
            break;
    }
    ui->plainTextEdit_Recive->appendHtml(CreateHtmlStr(title, QColor(Qt::gray)));
    ui->plainTextEdit_Recive->appendHtml(CreateHtmlStr(message+"\n", color));
    if(ui->checkBox_AutoRoll->isChecked())
        ui->plainTextEdit_Recive->moveCursor(QTextCursor::End);
}

void URControlPannel::slot_sockedConnect()
{
    QString msg;
    TCPSocketObject* socket = static_cast<TCPSocketObject*>(this->sender());
    if(socket == m_pTCPSocketObject_Dashboard)
        msg = "DashBoard Connected!";
    else
        msg = "Real Time Connected!";
    this->appendMessage(MessageType::Succeed, msg);
}

void URControlPannel::slot_socketDisConnect()
{
    QString msg;
    TCPSocketObject* socket = static_cast<TCPSocketObject*>(this->sender());
    if(socket == m_pTCPSocketObject_Dashboard)
        msg = "DashBoard DisConnected!";
    else if( socket == m_pTCPSocketObject_RealTime)
        msg = "Real Time DisConnected!";
    ui->pushButton_Connect2Server->setChecked(false);
    this->appendMessage(MessageType::Fail, msg);
}

void URControlPannel::slot_socketReadReady_RealTime(QTcpSocket *socket)
{
    if (socket->bytesAvailable() < 4)
        return;

    QByteArray data;
    data.append(socket->read(4));
    data.append(socket->read(HexToInt(data.toHex())-4));
    this->praseHexData(data);
}

void URControlPannel::slot_socketReadReady_Dashboard(QTcpSocket *socket)
{
    this->appendMessage(MessageType::Message, socket->readAll());
}

void URControlPannel::on_pushButton_Connect2Server_toggled(bool checked)
{
    if(checked)
    {
        ui->pushButton_Connect2Server->setText("断开");
        QString ip = ui->lineEdit_IP->text();
        qDebug() << "IP:" << ip;
        m_pTCPSocketObject_RealTime->connect(ip, 30003);

        m_pTCPSocketObject_Dashboard->connect(ip, 29999);
    }
    else
    {
        ui->pushButton_Connect2Server->setText("连接");
        m_pTCPSocketObject_RealTime->disConnect();
        m_pTCPSocketObject_Dashboard->disConnect();
    }
}


void URControlPannel::on_pushButton_ClearLog_clicked()
{
    ui->plainTextEdit_Recive->clear();
}


void URControlPannel::on_pushButton_AddVoidfunc_clicked()
{
    QString cmd = QString("def voidfunc():\n\t" \
                          "end\n");
    ui->textEdit_Send->insertPlainText(cmd);
}


void URControlPannel::on_pushButton_AddCurpos_clicked()
{
    ui->textEdit_Send->insertPlainText(mURPosition.GetPosStr());
}


void URControlPannel::on_pushButton_AddStopfunc_clicked()
{
    QString cmd = stopFunc;
    ui->textEdit_Send->insertPlainText(cmd);
}


void URControlPannel::on_pushButton_AddSleepfunc_clicked()
{
    QString cmd = sleepFunc;
    ui->textEdit_Send->insertPlainText(cmd);
}


void URControlPannel::on_pushButton_AddFolderPos_clicked()
{
    QString cmd = QString("[0,-100,160,120,-90,0]*3.14159/180.0");
    ui->textEdit_Send->insertPlainText(cmd);
}


void URControlPannel::on_pushButton_AddZeroPos_clicked()
{
    QString cmd = QString("[0,-1.57,0,-1.57,0,0]");
    ui->textEdit_Send->insertPlainText(cmd);
}


void URControlPannel::on_pushButton_Clearsend_clicked()
{
    ui->textEdit_Send->clear();
}


void URControlPannel::on_pushButton_Command_Send_clicked()
{
    QString cmd = ui->textEdit_Send->toPlainText().replace("\\n", "\n");
    this->sendCommand(cmd, ui->checkBox_DashBoard_Command->isChecked());
}


void URControlPannel::on_pushButton_Quick_Command_Insert_clicked()
{
    QString cmd;
    if (ui->comboBox_Quick_Command->currentIndex() == 0)
        cmd = moveJFunc(JointPose,1,1);
    else if (ui->comboBox_Quick_Command->currentIndex() == 1)
        cmd = moveLFunc(Pose,1,1);
    else
        cmd = movePFunc(Pose, 1, 1);
    ui->textEdit_Send->insertPlainText(cmd);
}


void URControlPannel::on_pushButton_Online_clicked()
{
    QString cmd = "power on\n";
    this->sendCommand(cmd,true);
    this->on_pushButton_Init_clicked();
}


void URControlPannel::on_pushButton_Init_clicked()
{
    QString cmd = "load installation default.installation\n";
    this->sendCommand(cmd,true);
}


void URControlPannel::on_pushButton_Offline_clicked()
{
    QString cmd = "power off\n";
    this->sendCommand(cmd,true);
}


void URControlPannel::on_pushButton_CutPower_clicked()
{
    QString cmd = "shutdown\n";
    this->sendCommand(cmd,true);
}



void URControlPannel::on_pushButton_ToZero_clicked()
{
    QString cmd = QString("movej([0,-1.57,0,-1.57,0,0],%1,%2)\n").arg(ui->horizontalSlider_MoveSpeed->value()/100.0*maxRadSpeed).arg(ui->horizontalSlider_Accelerations->value()/100.0*maxRadAcc);
    this->sendCommand(cmd,false);
}


void URControlPannel::on_pushButton_ToFolder_clicked()
{
    QString cmd = QString("movej([0,-100,160,120,-90,0]*3.14159/180.0,%1,%2)\n").arg(ui->horizontalSlider_MoveSpeed->value()/100.0*maxRadSpeed).arg(ui->horizontalSlider_Accelerations->value()/100.0*maxRadAcc);
    this->sendCommand(cmd,false);
}


void URControlPannel::on_pushButton_UnlockProtect_clicked()
{
    QString cmd = "unlock protective stop\n";
    this->sendCommand(cmd,true);
}


void URControlPannel::on_pushButton_UnlockBrake_clicked()
{
    QString cmd = "brake release\n";
    this->sendCommand(cmd,true);
}


void URControlPannel::on_pushButton_StopProgram_clicked()
{
    QString cmd = "pause\n";
    this->sendCommand(cmd,true);
}


void URControlPannel::on_pushButton_StartProgram_clicked()
{
    QString cmd = "play\n";
    this->sendCommand(cmd,true);
    cmd = "get loaded program\n";
    this->sendCommand(cmd,true);
}


void URControlPannel::on_pushButton_FreeDriveMode_toggled(bool checked)
{
    QString cmd;
    if (checked)
        cmd = QString("def freedriveMode():\n" \
            "    freedrive_mode()\n" \
            "    while (True) :\n" \
            "        sync()\n" \
            "    end\n"\
            "    end\n");
    else
        cmd = QString("end_freedrive_mode()\n");
    this->sendCommand(cmd);
}


void URControlPannel::on_pushButton_Stop_clicked()
{
    QString cmd = "stop\n";
    this->sendCommand(cmd,true);
}


void URControlPannel::on_pushButton_X_P_pressed()
{
    QString cmd;
    cmd = QString("speedl([%1,0,0,0,0,0], %2, 0.016)\n").arg(QString::number(ui->horizontalSlider_MoveSpeed->value()/100.00 * maxSpeed, 'f', 3), QString::number(ui->horizontalSlider_Accelerations->value()/100.00 * maxAcc, 'f', 3));
    this->sendCommand(cmd);
}


void URControlPannel::on_pushButton_X_N_pressed()
{
    QString cmd;
    cmd = QString("speedl([-%1,0,0,0,0,0], -%2, 0.016)\n").arg(QString::number(ui->horizontalSlider_MoveSpeed->value()/100.00 * maxSpeed, 'f', 3), QString::number(ui->horizontalSlider_Accelerations->value()/100.00 * maxAcc, 'f', 3));
    this->sendCommand(cmd);
}


void URControlPannel::on_pushButton_Y_P_pressed()
{
    QString cmd;
    cmd = QString("speedl([0,%1,0,0,0,0], %2, 0.016)\n").arg(QString::number(ui->horizontalSlider_MoveSpeed->value()/100.00 * maxSpeed, 'f', 3), QString::number(ui->horizontalSlider_Accelerations->value()/100.00 * maxAcc, 'f', 3));
    this->sendCommand(cmd);
}


void URControlPannel::on_pushButton_Y_N_pressed()
{
    QString cmd;
    cmd = QString("speedl([0,-%1,0,0,0,0], -%2, 0.016)\n").arg(QString::number(ui->horizontalSlider_MoveSpeed->value()/100.00 * maxSpeed, 'f', 3), QString::number(ui->horizontalSlider_Accelerations->value()/100.00 * maxAcc, 'f', 3));
    this->sendCommand(cmd);
}


void URControlPannel::on_pushButton_Z_P_pressed()
{
    QString cmd;
    cmd = QString("speedl([0,0,%1,0,0,0], %2, 0.016)\n").arg(QString::number(ui->horizontalSlider_MoveSpeed->value()/100.00 * maxSpeed, 'f', 3), QString::number(ui->horizontalSlider_Accelerations->value()/100.00 * maxAcc, 'f', 3));
    this->sendCommand(cmd);
}


void URControlPannel::on_pushButton_Z_N_pressed()
{
    QString cmd;
    cmd = QString("speedl([0,0,-%1,0,0,0], -%2, 0.016)\n").arg(QString::number(ui->horizontalSlider_MoveSpeed->value()/100.00 * maxSpeed, 'f', 3), QString::number(ui->horizontalSlider_Accelerations->value()/100.00 * maxAcc, 'f', 3));
    this->sendCommand(cmd);
}


void URControlPannel::on_pushButton_RX_P_pressed()
{
    QString cmd;
    cmd = QString("speedl([0,0,0,%1,0,0], %2, 0.016)\n").arg(QString::number(ui->horizontalSlider_MoveSpeed->value()/100.00 * maxRadSpeed, 'f', 3), QString::number(ui->horizontalSlider_Accelerations->value()/100.00 * maxRadAcc, 'f', 3));
    this->sendCommand(cmd);
}


void URControlPannel::on_pushButton_RX_N_pressed()
{
    QString cmd;
    cmd = QString("speedl([0,0,0,-%1,0,0], -%2, 0.016)\n").arg(QString::number(ui->horizontalSlider_MoveSpeed->value()/100.00 * maxRadSpeed, 'f', 3), QString::number(ui->horizontalSlider_Accelerations->value()/100.00 * maxRadAcc, 'f', 3));
    this->sendCommand(cmd);
}


void URControlPannel::on_pushButton_RY_P_pressed()
{
    QString cmd;
    cmd = QString("speedl([0,0,0,0,%1,0], %2, 0.016)\n").arg(QString::number(ui->horizontalSlider_MoveSpeed->value()/100.00 * maxRadSpeed, 'f', 3), QString::number(ui->horizontalSlider_Accelerations->value()/100.00 * maxRadAcc, 'f', 3));
    this->sendCommand(cmd);
}


void URControlPannel::on_pushButton_RY_N_pressed()
{
    QString cmd;
    cmd = QString("speedl([0,0,0,0,-%1,0], -%2, 0.016)\n").arg(QString::number(ui->horizontalSlider_MoveSpeed->value()/100.00 * maxRadSpeed, 'f', 3), QString::number(ui->horizontalSlider_Accelerations->value()/100.00 * maxRadAcc, 'f', 3));
    this->sendCommand(cmd);
}


void URControlPannel::on_pushButton_RZ_P_pressed()
{
    QString cmd;
    cmd = QString("speedl([0,0,0,0,0,%1], %2, 0.016)\n").arg(QString::number(ui->horizontalSlider_MoveSpeed->value()/100.00 * maxRadSpeed, 'f', 3), QString::number(ui->horizontalSlider_Accelerations->value()/100.00 * maxRadAcc, 'f', 3));
    this->sendCommand(cmd);
}


void URControlPannel::on_pushButton_RZ_N_pressed()
{
    QString cmd;
    cmd = QString("speedl([0,0,0,0,0,-%1], -%2, 0.016)\n").arg(QString::number(ui->horizontalSlider_MoveSpeed->value()/100.00 * maxRadSpeed, 'f', 3), QString::number(ui->horizontalSlider_Accelerations->value()/100.00 * maxRadAcc, 'f', 3));
    this->sendCommand(cmd);
}


void URControlPannel::on_pushButton_Joint1_P_pressed()
{
    QString cmd;
    cmd = QString("speedj([%1,0,0,0,0,0], %2, 0.016)\n").arg(QString::number(ui->horizontalSlider_MoveSpeed->value()/100.00 * maxRadSpeed, 'f', 3), QString::number(ui->horizontalSlider_Accelerations->value()/100.00 * maxRadAcc, 'f', 3));
    this->sendCommand(cmd);
}


void URControlPannel::on_pushButton_Joint1_N_pressed()
{
    QString cmd;
    cmd = QString("speedj([-%1,0,0,0,0,0], -%2, 0.016)\n").arg(QString::number(ui->horizontalSlider_MoveSpeed->value()/100.00 * maxRadSpeed, 'f', 3), QString::number(ui->horizontalSlider_Accelerations->value()/100.00 * maxRadAcc, 'f', 3));
    this->sendCommand(cmd);
}


void URControlPannel::on_pushButton_Joint2_P_pressed()
{
    QString cmd;
    cmd = QString("speedj([0,%1,0,0,0,0], %2, 0.016)\n").arg(QString::number(ui->horizontalSlider_MoveSpeed->value()/100.00 * maxRadSpeed, 'f', 3), QString::number(ui->horizontalSlider_Accelerations->value()/100.00 * maxRadAcc, 'f', 3));
    this->sendCommand(cmd);
}


void URControlPannel::on_pushButton_Joint2_N_pressed()
{
    QString cmd;
    cmd = QString("speedj([0,-%1,0,0,0,0], -%2, 0.016)\n").arg(QString::number(ui->horizontalSlider_MoveSpeed->value()/100.00 * maxRadSpeed, 'f', 3), QString::number(ui->horizontalSlider_Accelerations->value()/100.00 * maxRadAcc, 'f', 3));
    this->sendCommand(cmd);
}


void URControlPannel::on_pushButton_Joint3_P_pressed()
{
    QString cmd;
    cmd = QString("speedj([0,0,%1,0,0,0], %2, 0.016)\n").arg(QString::number(ui->horizontalSlider_MoveSpeed->value()/100.00 * maxRadSpeed, 'f', 3), QString::number(ui->horizontalSlider_Accelerations->value()/100.00 * maxRadAcc, 'f', 3));
    this->sendCommand(cmd);
}


void URControlPannel::on_pushButton_Joint3_N_pressed()
{
    QString cmd;
    cmd = QString("speedj([0,0,-%1,0,0,0], -%2, 0.016)\n").arg(QString::number(ui->horizontalSlider_MoveSpeed->value()/100.00 * maxRadSpeed, 'f', 3), QString::number(ui->horizontalSlider_Accelerations->value()/100.00 * maxRadAcc, 'f', 3));
    this->sendCommand(cmd);
}


void URControlPannel::on_pushButton_Joint4_P_pressed()
{
    QString cmd;
    cmd = QString("speedj([0,0,0,%1,0,0], %2, 0.016)\n").arg(QString::number(ui->horizontalSlider_MoveSpeed->value()/100.00 * maxRadSpeed, 'f', 3), QString::number(ui->horizontalSlider_Accelerations->value()/100.00 * maxRadAcc, 'f', 3));
    this->sendCommand(cmd);
}


void URControlPannel::on_pushButton_Joint4_N_pressed()
{
    QString cmd;
    cmd = QString("speedj([0,0,0,-%1,0,0], -%2, 0.016)\n").arg(QString::number(ui->horizontalSlider_MoveSpeed->value()/100.00 * maxRadSpeed, 'f', 3), QString::number(ui->horizontalSlider_Accelerations->value()/100.00 * maxRadAcc, 'f', 3));
    this->sendCommand(cmd);
}


void URControlPannel::on_pushButton_Joint5_P_pressed()
{
    QString cmd;
    cmd = QString("speedj([0,0,0,0,%1,0], %2, 0.016)\n").arg(QString::number(ui->horizontalSlider_MoveSpeed->value()/100.00 * maxRadSpeed, 'f', 3), QString::number(ui->horizontalSlider_Accelerations->value()/100.00 * maxRadAcc, 'f', 3));
    this->sendCommand(cmd);
}


void URControlPannel::on_pushButton_Joint5_N_pressed()
{
    QString cmd;
    cmd = QString("speedj([0,0,0,0,-%1,0], -%2, 0.016)\n").arg(QString::number(ui->horizontalSlider_MoveSpeed->value()/100.00 * maxRadSpeed, 'f', 3), QString::number(ui->horizontalSlider_Accelerations->value()/100.00 * maxRadAcc, 'f', 3));
    this->sendCommand(cmd);
}


void URControlPannel::on_pushButton_Joint6_P_pressed()
{
    QString cmd;
    cmd = QString("speedj([0,0,0,0,0,%1], %2, 0.016)\n").arg(QString::number(ui->horizontalSlider_MoveSpeed->value()/100.00 * maxRadSpeed, 'f', 3), QString::number(ui->horizontalSlider_Accelerations->value()/100.00 * maxRadAcc, 'f', 3));
    this->sendCommand(cmd);
}


void URControlPannel::on_pushButton_Joint6_N_pressed()
{
    QString cmd;
    cmd = QString("speedj([0,0,0,0,0,-%1], -%2, 0.016)\n").arg(QString::number(ui->horizontalSlider_MoveSpeed->value()/100.00 * maxRadSpeed, 'f', 3), QString::number(ui->horizontalSlider_Accelerations->value()/100.00 * maxRadAcc, 'f', 3));
    this->sendCommand(cmd);
}


void URControlPannel::on_horizontalSlider_MoveSpeed_valueChanged(int value)
{
    ui->lineEdit_MoveSpeed->setText(QString::number(value));
}


void URControlPannel::on_horizontalSlider_Accelerations_valueChanged(int value)
{
    ui->lineEdit_Accelerations->setText(QString::number(value));
}
