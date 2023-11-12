#pragma once
#include <QByteArray>
#include <QString>
#include <QVector>
#include <QtMath>
#include <QMap>
#include <QColor>

#define Pose "p[x,y,z,rx,ry,rz]"
#define JointPose "[j1,j2,j3,j4,j5,j6]"
#define stopFunc "stop(1)\n"
#define sleepFunc "sleep(1)\n"
#define moveJFunc(JointPos, acc, speed) QString("movej(%1,a=%2,v=%3,t=0,r=0)\n").arg(JointPos).arg(acc).arg(speed)
#define moveLFunc(Pos, acc, speed) QString("movel(%1,a=%2,v=%3,t=0,r=0)\n").arg(Pos).arg(acc).arg(speed)
#define movePFunc(Pos, acc, speed) QString("movep(%1,a=%2,v=%3,r=0)\n").arg(Pos).arg(acc).arg(speed)


const static QMap<int, QString> RobotMode = {
    {-1, "NO_CONTROLLER"},
    {0, "DISCONNECTED"},
    {1, "CONFIRM_SAFETY"},
    {2, "BOOTING"},
    {3, "POWER_OFF"},
    {4, "POWER_ON"},
    {5, "IDLE"},
    {6, "BACKDRIVE"},
    {7, "RUNNING"},
    {8, "UPDATING_FIRMWARE"}

};

const static QMap<int, QString> SafetyMode{
    {11, "UNDEFINED_SAFETY_MODE"},
    {10, "VALIDATE_JOINT_ID"},
    {9, "FAULT"},
    {8, "VIOLATION"},
    {7, "ROBOT_EMERGENCY_STOP"},
    {6, "SYSTEM_EMERGENCY_STOP"},
    {5, "SAFEGUARD_STOP"},
    {4, "RECOVERY"},
    {3, "PROTECTIVE_STOP"},
    {2, "REDUCED"},
    {1, "NORMAL"}

};

inline QString CreateHtmlStr(const QString& str, const QColor color)
{
    QString htmlStr;
    htmlStr = QString("<p><span style='color:%1;'>%2</span></p>").arg(color.name()).arg(str);
    return htmlStr;
}

inline int HexToInt(QByteArray array)
{
	QString qnum = array.data();
	int num;
	bool ok;
	num = qnum.toInt(&ok, 16);
	if (!(qnum.at(0) >= '0'&&qnum.at(0) <= '7')) {//非正数
		num = num & 0x7FFF;       //清除符号位
		num = 32768 - num;                   //反码
		num = num * -1;                //符号位
	}
	return num;
}

inline double HexToDouble(QByteArray array)
{
    qulonglong hex_long = array.toULongLong(nullptr, 16);
    double Ddata = *(double*)&hex_long;
    return Ddata;
}

inline int QByteArrayToInt(QByteArray array)
{
	return  array.toHex().toInt(nullptr, 16);
}

inline double QByteArrayToDouble(QByteArray array)
{
	QString input_hex = array.toHex();
	qulonglong hex_long = input_hex.toULongLong(nullptr, 16);
	double Ddata = *(double*)&hex_long;
	return Ddata;
}

inline QByteArray ArraySub(QByteArray recv, int start, int lenght = 8) {
	QByteArray output;
	for (int i = start; i < start + lenght; i++) {
		output.append(recv[i]);
	}
	return output;
}

inline QVector<double> PraseArray(QByteArray array)
{
	QVector<double> double_vec;
	// 前四个字节是数据长度
	int messageSize = 4;
	int length = HexToInt(array.left(messageSize * 2));
	for( int i = messageSize * 2; i < length * 2; i+=8 * 2)
	{
		double_vec.push_back(HexToDouble(array.mid(i, 8 * 2)));
	}
	return double_vec;
}


class URPosition
{
public:
    void SetX(double value) { X = value; };
    void SetY(double value) { Y = value; };
    void SetZ(double value) { Z = value; };
    void SetRX(double value) {RX = value; };
    void SetRY(double value) {RY = value; };
    void SetRZ(double value) {RZ = value; };
    QString GetXStr() { return QString::number(X * 1000, 'd', 2); };
    QString GetYStr() { return QString::number(Y * 1000, 'd', 2); };
    QString GetZStr() { return QString::number(Z * 1000, 'd', 2); };
    QString GetRXStr() { return QString::number(RX, 'd', 3); };
    QString GetRYStr() { return QString::number(RY, 'd', 3); };
    QString GetRZStr() { return QString::number(RZ, 'd', 3); };
    // q actual joint
    void SetJoint1(double value) { Joint1 = value; };
    void SetJoint2(double value) { Joint2 = value; };
    void SetJoint3(double value) { Joint3 = value; };
    void SetJoint4(double value) { Joint4 = value; };
    void SetJoint5(double value) { Joint5 = value; };
    void SetJoint6(double value) { Joint6 = value; };
    QString GetJoint1Str() { return QString::number(Joint1 * 180 / M_PI, 'd', 3); };
    QString GetJoint2Str() { return QString::number(Joint2 * 180 / M_PI, 'd', 3); };
    QString GetJoint3Str() { return QString::number(Joint3 * 180 / M_PI, 'd', 3); };
    QString GetJoint4Str() { return QString::number(Joint4 * 180 / M_PI, 'd', 3); };
    QString GetJoint5Str() { return QString::number(Joint5 * 180 / M_PI, 'd', 3); };
    QString GetJoint6Str() { return QString::number(Joint6 * 180 / M_PI, 'd', 3); };

    QString GetJointPosStr(double j1 = 0.0, double j2 = 0.0, double j3 = 0.0, double j4 = 0.0, double j5 = 0.0, double j6 = 0.0)
    {
        QString str = QString("[%1,%2,%3,%4,%5,%6]").arg(Joint1 + j1).arg(Joint2 + j2).arg(Joint3 + j3).arg(Joint4 + j4).arg(Joint5 + j5).arg(Joint6 + j6);
        return str;
    };

    QString GetJointPosAllStr(double j1 = 0.0, double j2 = 0.0, double j3 = 0.0, double j4 = 0.0, double j5 = 0.0, double j6 = 0.0)
    {
        QString str = QString("%1, a=%7, v=%8").arg(GetJointPosStr(j1, j2, j3, j4, j5, j6)).arg(MoveSpeed).arg(Accelerations);
        return str;
    };

    QString GetPosStr(double off_x = 0.0, double off_y = 0.0, double off_z = 0.0, double off_rx = 0.0, double off_ry = 0.0, double off_rz = 0.0)
    {
        QString str = QString("p[%1,%2,%3,%4,%5,%6]").arg(X + off_x).arg(Y + off_y).arg(Z + off_z).arg(RX + off_rx).arg(RY + off_ry).arg(RZ + off_rz);
        return str;
    };

    QString GetPosAllStr(double off_x = 0.0, double off_y = 0.0, double off_z = 0.0, double off_rx = 0.0, double off_ry = 0.0, double off_rz = 0.0)
    {
        QString str = QString("%1, a=%7, v=%8").arg(GetPosStr(off_x, off_y, off_z, off_rx, off_ry, off_rz)).arg(MoveSpeed).arg(Accelerations);
        return str;
    };

private:
    double X;
    double Y;
    double Z;
    double RX;
    double RY;
    double RZ;
    double MoveSpeed;
    double Accelerations;
    double Joint1;
    double Joint2;
    double Joint3;
    double Joint4;
    double Joint5;
    double Joint6;
};
