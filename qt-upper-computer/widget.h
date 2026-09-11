#ifndef WIDGET_H
#define WIDGET_H

#include <QWidget>
#include <QComboBox>
#include <QPushButton>
#include <QLCDNumber>
#include <QLabel>
#include <QGroupBox>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGridLayout>
#include <QCalendarWidget>
#include <QTimer>
#include <QDateTime>
#include <QSerialPort>
#include <QSerialPortInfo>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QTextEdit>

class Widget : public QWidget
{
    Q_OBJECT
public:
    explicit Widget(QWidget *parent = nullptr);
    ~Widget() override;
protected:
    void paintEvent(QPaintEvent *event) override;
private slots:
    void onOpenSerial();
    void readSerialData();
    void onCalendarChanged();
    void ctrlLivingOn();
    void ctrlLivingOff();
    void ctrlBedOn();
    void ctrlBedOff();
    void ctrlAcOn();
    void ctrlAcOff();
    void ctrlAlarmOn();
    void ctrlAlarmOff();
    void getWeather();
    void replyFinished(QNetworkReply* reply);
    // 记事本新增槽
    void onDateSelectChanged();
    void saveCurrentNote();
private:
    // 串口控件
    QComboBox* m_cmbPort;
    QPushButton* m_btnSerial;
    QPushButton* m_btnNet;

    // 温湿度、姿态、光照LCD
    QLCDNumber* m_lcdTemp;
    QLCDNumber* m_lcdHumi;
    QLCDNumber* m_lcdAtt;
    QLCDNumber* m_lcdLight;

    // 天气、时间标签
    QLabel* m_lblTime;
    QLabel* m_lblWeaCity;
    QLabel* m_lblWeaDesc;
    QLabel* m_lblWeaWind;
    QLabel* m_lblWeaUpdate;

    // 通知发布
    QTextEdit* m_txtNotice;
    QPushButton* m_btnPublish;

    // 日历、定时器、串口、网络
    QCalendarWidget* m_calendar;
    QTimer* m_timeTimer;
    QSerialPort* m_serial;
    QNetworkAccessManager* m_netWeather;
    // 新增控件
    QTextEdit* m_noteEdit;
    // 存储所有日期备注 key=日期字符串 yyyy-MM-dd
    QMap<QString, QString> m_dateNoteMap;
};

#endif // WIDGET_H
