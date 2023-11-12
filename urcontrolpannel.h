#ifndef URCONTROLPANNEL_H
#define URCONTROLPANNEL_H

#include <QWidget>
#include "tcpsocketobject.h"
#include "GlobalDeclaration.h"

QT_BEGIN_NAMESPACE
namespace Ui { class URControlPannel; }
QT_END_NAMESPACE

class URControlPannel : public QWidget
{
    Q_OBJECT

    enum MessageType
    {
        Recive,
        Send,
        Message,
        Succeed,
        Fail
    };

public:
    URControlPannel(QWidget *parent = nullptr);
    ~URControlPannel();

private:
    void initConnection();
    void praseHexData(QByteArray array);
    void sendCommand(QString command, bool dashboard = false);
    void updateURPosition();
    void appendMessage(MessageType type, QString message);

private slots:
    void slot_sockedConnect();
    void slot_socketDisConnect();
    void slot_socketReadReady_RealTime(QTcpSocket* socket);
    void slot_socketReadReady_Dashboard(QTcpSocket* socket);

    void on_pushButton_Connect2Server_toggled(bool checked);

    void on_pushButton_ClearLog_clicked();

    void on_pushButton_AddVoidfunc_clicked();

    void on_pushButton_AddCurpos_clicked();

    void on_pushButton_AddStopfunc_clicked();

    void on_pushButton_AddSleepfunc_clicked();

    void on_pushButton_AddFolderPos_clicked();

    void on_pushButton_AddZeroPos_clicked();

    void on_pushButton_AddHomePos_clicked();

    void on_pushButton_Clearsend_clicked();

    void on_pushButton_Command_Send_clicked();

    void on_pushButton_Quick_Command_Insert_clicked();

    void on_pushButton_Online_clicked();

    void on_pushButton_Init_clicked();

    void on_pushButton_Offline_clicked();

    void on_pushButton_CutPower_clicked();

    void on_pushButton_ToZero_clicked();

    void on_pushButton_ToFolder_clicked();

    void on_pushButton_UnlockProtect_clicked();

    void on_pushButton_UnlockBrake_clicked();

    void on_pushButton_StopProgram_clicked();

    void on_pushButton_StartProgram_clicked();

    void on_pushButton_FreeDriveMode_toggled(bool checked);

    void on_pushButton_Stop_clicked();

    void on_pushButton_X_P_pressed();

    void on_pushButton_X_N_pressed();

    void on_pushButton_Y_P_pressed();

    void on_pushButton_Y_N_pressed();

    void on_pushButton_Z_P_pressed();

    void on_pushButton_Z_N_pressed();

    void on_pushButton_RX_P_pressed();

    void on_pushButton_RX_N_pressed();

    void on_pushButton_RY_P_pressed();

    void on_pushButton_RY_N_pressed();

    void on_pushButton_RZ_P_pressed();

    void on_pushButton_RZ_N_pressed();

    void on_pushButton_Joint1_P_pressed();

    void on_pushButton_Joint1_N_pressed();

    void on_pushButton_Joint2_P_pressed();

    void on_pushButton_Joint2_N_pressed();

    void on_pushButton_Joint3_P_pressed();

    void on_pushButton_Joint3_N_pressed();

    void on_pushButton_Joint4_P_pressed();

    void on_pushButton_Joint4_N_pressed();

    void on_pushButton_Joint5_P_pressed();

    void on_pushButton_Joint5_N_pressed();

    void on_pushButton_Joint6_P_pressed();

    void on_pushButton_Joint6_N_pressed();

    void on_horizontalSlider_MoveSpeed_valueChanged(int value);

    void on_horizontalSlider_Accelerations_valueChanged(int value);


private:
    Ui::URControlPannel *ui;
    URPosition mURPosition;
    TCPSocketObject* m_pTCPSocketObject_RealTime;
    TCPSocketObject* m_pTCPSocketObject_Dashboard;
};
#endif // URCONTROLPANNEL_H
