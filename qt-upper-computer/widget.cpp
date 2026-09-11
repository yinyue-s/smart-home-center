#include "widget.h"
#include <QApplication>
#include <QDebug>
#include <QPainter>
#include <QPixmap>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QMessageBox>
#include <QTabWidget>
#include <QScrollBar>
#include <QMap>
#include <QDate>
#include <QDateTime>
Widget::Widget(QWidget *parent)
    : QWidget(parent)
{
    this->setWindowTitle("智能家居监控上位机");
    this->resize(1350, 900);
    this->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    this->setMinimumSize(1000, 650);
    this->setStyleSheet("font-family: 'Microsoft YaHei';font-size:20px;");

    // 顶层分页容器
    QTabWidget* tabMain = new QTabWidget(this);
    tabMain->setStyleSheet(R"(
    QTabWidget::pane{border:1px solid #ddd; border-radius:6px;}
    QTabBar::tab{
        background:#e8e2d9; padding:8px 20px;
        border-top-left-radius:6px; border-top-right-radius:6px;
        margin-right:4px;
    }
    QTabBar::tab:selected{background:#ffffff; color:#5C4B3E; font-weight:bold;}
    )");

    // ===================== 页面1：智能家居控制页 =====================
    QWidget* pageCtrl = new QWidget;
    QVBoxLayout* layCtrl = new QVBoxLayout(pageCtrl);
    layCtrl->setSpacing(12);
    layCtrl->setContentsMargins(10,10,10,10);

    // 顶部串口栏
    QHBoxLayout* topLayout = new QHBoxLayout;
    m_cmbPort = new QComboBox;
    m_btnSerial = new QPushButton("打开串口");
    m_btnNet = new QPushButton("网络服务端");
    foreach(const QSerialPortInfo& info, QSerialPortInfo::availablePorts()) {
        m_cmbPort->addItem(info.portName());
    }
    topLayout->addWidget(new QLabel("串口选择："));
    topLayout->addWidget(m_cmbPort, 1);
    topLayout->addWidget(m_btnSerial);
    topLayout->addStretch();
    topLayout->addWidget(m_btnNet);
    topLayout->setContentsMargins(5,5,5,5);
    layCtrl->addLayout(topLayout);

    // 左侧整块：环境监测 + 设备控制 + 通知发布
    QVBoxLayout* leftLayout = new QVBoxLayout;
    leftLayout->setSpacing(10);

    // 环境监测
    QGroupBox* groupEnv = new QGroupBox("环境监测");
    QGridLayout* envLayout = new QGridLayout(groupEnv);
    envLayout->setSpacing(12);
    envLayout->setContentsMargins(15,15,15,15);
    QLabel* labTemp = new QLabel("🌡 温度：");
    QLabel* labHumi = new QLabel("💧 湿度：");
    m_lcdTemp = new QLCDNumber;
    m_lcdHumi = new QLCDNumber;
    m_lcdTemp->setDigitCount(3);
    m_lcdHumi->setDigitCount(3);
    m_lcdTemp->setStyleSheet("color: #5C4B3E;");
    m_lcdHumi->setStyleSheet("color: #5C4B3E;");
    envLayout->addWidget(labTemp,0,0);
    envLayout->addWidget(m_lcdTemp,0,1);
    envLayout->addWidget(labHumi,1,0);
    envLayout->addWidget(m_lcdHumi,1,1);
    leftLayout->addWidget(groupEnv,1);

    // 灯光空调控制
    QGroupBox* groupCtrl = new QGroupBox("设备控制");
    QVBoxLayout* ctrlLayout = new QVBoxLayout(groupCtrl);
    ctrlLayout->setSpacing(8);
    ctrlLayout->setContentsMargins(15,15,15,15);
    QPushButton *btnLivOn = new QPushButton("客厅灯 开灯");
    QPushButton *btnLivOff = new QPushButton("客厅灯 关灯");
    QPushButton *btnBedOn = new QPushButton("卧室灯 开灯");
    QPushButton *btnBedOff = new QPushButton("卧室灯 关灯");
    QPushButton *btnAcOn = new QPushButton("空调 开");
    QPushButton *btnAcOff = new QPushButton("空调 关");
    QString btnStyle = R"(
    QPushButton{background-color:#E8E2D9;color:#5C4B3E;border:none;padding:9px 0;border-radius:8px;
     font-size:16px;}
    QPushButton:hover{background-color:#DDD5C9;}
    QPushButton:pressed{background-color:#C9BEB0;padding-top:10px;}
    QPushButton:disabled{background:#E2E2E2;color:#999999;}
    )";
    btnLivOn->setStyleSheet(btnStyle);
    btnLivOff->setStyleSheet(btnStyle);
    btnBedOn->setStyleSheet(btnStyle);
    btnBedOff->setStyleSheet(btnStyle);
    btnAcOn->setStyleSheet(btnStyle);
    btnAcOff->setStyleSheet(btnStyle);
    ctrlLayout->addWidget(btnLivOn);
    ctrlLayout->addWidget(btnLivOff);
    ctrlLayout->addWidget(btnBedOn);
    ctrlLayout->addWidget(btnBedOff);
    ctrlLayout->addWidget(btnAcOn);
    ctrlLayout->addWidget(btnAcOff);
    leftLayout->addWidget(groupCtrl,2);

    // 通知发布
    QGroupBox* groupNotice = new QGroupBox("通知发布");
    groupNotice->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    QHBoxLayout* noticeLayout = new QHBoxLayout(groupNotice);
    m_txtNotice = new QTextEdit;
    m_btnPublish = new QPushButton("发布通知");
    m_btnPublish->setStyleSheet(btnStyle);
    noticeLayout->addWidget(m_txtNotice,1);
    noticeLayout->addWidget(m_btnPublish);
    leftLayout->addWidget(groupNotice,1);

    layCtrl->addLayout(leftLayout);
    tabMain->addTab(pageCtrl, "🏠 智能家居控制");

    // ===================== 页面2：日历日程+记事本 =====================
    QWidget* pageCalendar = new QWidget;
    QVBoxLayout* layCalendar = new QVBoxLayout(pageCalendar);
    layCalendar->setContentsMargins(10,10,10,10);
    layCalendar->setSpacing(10);

    // 上部：日历
    m_calendar = new QCalendarWidget;
    m_calendar->setSelectedDate(QDate::currentDate());
    m_calendar->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    m_calendar->setStyleSheet(R"(
    QCalendarWidget {background:rgba(255,255,255,230);border-radius:8px;border:1px solid #ddd;}
    QCalendarWidget QWidget#qt_calendar_navigationbar{background:#E8E2D9;min-height:52px;}
    QCalendarWidget QToolButton{color:#5C4B3E;height:42px;font-size:17px;}
    QCalendarWidget QToolButton:hover{background:#D4C9BC;}
    QCalendarWidget QWidget#qt_calendar_calendarheader QLabel{font-size:19px;font-weight:bold;color:#5C4B3E;}
    QCalendarWidget QHeaderView::section{background:transparent;border:none;color:#5C4B3E;padding:7px;font-size:16px;font-weight:600;}
    QCalendarWidget QTableView{gridline-color:#eee;}
    QCalendarWidget QTableView::item{padding:9px;color:#333;font-size:17px;min-height:65px;}
    QCalendarWidget QTableView::item:selected{background:#E8E2D9;color:#5C4B3E;border-radius:4px;font-weight:bold;}
    QCalendarWidget QTableView::item[dayOfWeek="6"],QCalendarWidget QTableView::item[dayOfWeek="7"]{color:#d92121;}
    QCalendarWidget QTableView::item:disabled{color:#bbb;background:#f8f8f8;font-size:16px;}
    )");
    layCalendar->addWidget(m_calendar, 3); // 比例3份高度

    // 下部：记事本区域
    QGroupBox* noteBox = new QGroupBox("📝 当日记事本");
    QVBoxLayout* noteLayout = new QVBoxLayout(noteBox);
    noteLayout->setContentsMargins(12,12,12,12);
    m_noteEdit = new QTextEdit;
    m_noteEdit->setPlaceholderText("在这里填写当天日程、备注、待办事项...");
    QString noteStyle = R"(
    QTextEdit{border:1px solid #e0d8cc;border-radius:8px;padding:8px;font-size:15px;background:#fff;}
    QGroupBox{font-size:17px;color:#5C4B3E;font-weight:bold;}
    )";
    noteBox->setStyleSheet(noteStyle);
    noteLayout->addWidget(m_noteEdit);
    layCalendar->addWidget(noteBox, 1); // 比例1份高度

    tabMain->addTab(pageCalendar, "📅 日历日程");

    // 绑定切换日期信号：切换时保存旧备注、加载新日期备注
    connect(m_calendar, &QCalendarWidget::selectionChanged, this, &Widget::onDateSelectChanged);
    // 文本编辑变化自动保存
    connect(m_noteEdit, &QTextEdit::textChanged, this, &Widget::saveCurrentNote);

    // ===================== 页面3：设备信息监测页 =====================
    QWidget* pageMonitor = new QWidget;
    QVBoxLayout* layMonitor = new QVBoxLayout(pageMonitor);
    layMonitor->setSpacing(12);
    layMonitor->setContentsMargins(15,15,15,15);

    QGroupBox* groupInfo = new QGroupBox("设备信息面板");
    QVBoxLayout* infoLayout = new QVBoxLayout(groupInfo);
    infoLayout->setSpacing(12);
    infoLayout->setContentsMargins(15,15,15,15);

    // 姿态、光照
    QLabel* labAtt = new QLabel("📶 设备姿态：");
    QLabel* labLight = new QLabel("💡 光照强度：");
    m_lcdAtt = new QLCDNumber;
    m_lcdLight = new QLCDNumber;
    m_lcdAtt->setDigitCount(3);
    m_lcdLight->setDigitCount(3);
    m_lcdAtt->setStyleSheet("color: #5C4B3E;");
    m_lcdLight->setStyleSheet("color: #5C4B3E;");
    infoLayout->addWidget(labAtt);
    infoLayout->addWidget(m_lcdAtt);
    infoLayout->addWidget(labLight);
    infoLayout->addWidget(m_lcdLight);

    // 系统时间
    m_lblTime = new QLabel;
    m_lblTime->setStyleSheet("font-size:18px;color:#5C4B3E;font-weight:bold;");
    infoLayout->addWidget(new QLabel("⏰ 当前系统时间："));
    infoLayout->addWidget(m_lblTime);

    m_timeTimer = new QTimer(this);
    m_timeTimer->setInterval(1000);
    connect(m_timeTimer, &QTimer::timeout, this, [this](){
        QString timeStr = QDateTime::currentDateTime().toString("yyyy-MM-dd HH:mm:ss dddd");
        m_lblTime->setText(timeStr);
    });
    m_timeTimer->start();
    m_lblTime->setText(QDateTime::currentDateTime().toString("yyyy-MM-dd HH:mm:ss dddd"));

    // 天气模块
    QGroupBox* weatherBox = new QGroupBox("天气信息");
    QVBoxLayout* weatherLayout = new QVBoxLayout(weatherBox);
    weatherLayout->setSpacing(6);
    QString labStyle = "color:#5C4B3E;font-size:18px;";
    m_lblWeaCity = new QLabel("城市：天津");
    m_lblWeaDesc = new QLabel("天气：加载中...");
    m_lblWeaWind = new QLabel("风速：-- km/h");
    m_lblWeaUpdate = new QLabel("更新时间：--");
    m_lblWeaCity->setStyleSheet(labStyle);
    m_lblWeaDesc->setStyleSheet(labStyle);
    m_lblWeaWind->setStyleSheet(labStyle);
    m_lblWeaUpdate->setStyleSheet(labStyle);
    weatherLayout->addWidget(m_lblWeaCity);
    weatherLayout->addWidget(m_lblWeaDesc);
    weatherLayout->addWidget(m_lblWeaWind);
    weatherLayout->addWidget(m_lblWeaUpdate);
    QPushButton *btnRefreshWea = new QPushButton("🔄 刷新天气");
    btnRefreshWea->setStyleSheet(btnStyle);
    connect(btnRefreshWea, &QPushButton::clicked, this, &Widget::getWeather);
    weatherLayout->addWidget(btnRefreshWea);
    infoLayout->addWidget(weatherBox);

    // 火警警报
    QGroupBox* alarmBox = new QGroupBox("火警警报");
    QHBoxLayout* alarmLayout = new QHBoxLayout(alarmBox);
    QPushButton *btnAlarmOn = new QPushButton("警报开启");
    QPushButton *btnAlarmOff = new QPushButton("警报关闭");
    btnAlarmOn->setStyleSheet(btnStyle);
    btnAlarmOff->setStyleSheet(btnStyle);
    alarmLayout->addWidget(btnAlarmOn);
    alarmLayout->addWidget(btnAlarmOff);
    infoLayout->addWidget(alarmBox);

    // 家居资讯
    QGroupBox* newsBox = new QGroupBox("家居资讯新闻");
    newsBox->setStyleSheet(R"(
    QGroupBox{background:rgba(255,255,245);border-radius:12px;border:1px solid #e0d8cc;padding-top:14px;margin-top:8px;}
    QGroupBox::title{color:#6b5c4c;font-size:14px;font-weight:bold;subcontrol-origin:margin;left:18px;}
    )");
    QVBoxLayout* newsLayout = new QVBoxLayout(newsBox);
    newsLayout->setSpacing(8);
    newsLayout->setContentsMargins(12,12,12,12);
    QTextEdit* m_edtNews = new QTextEdit;
    m_edtNews->setReadOnly(true);
    m_edtNews->setStyleSheet(R"(
    QTextEdit{background:#fff;border:1px solid #e0d8cc;border-radius:6px;padding:6px;color:#444;font-size:11px;}
    )");
    m_edtNews->setText("【智能家居快讯】\n1、新款低功耗温湿度传感器全面适配本上位机；\n2、远程灯光控制优化，响应延迟降低至0.2s；\n3、系统安全升级，新增设备异常自动告警记录。");
    QPushButton* btnRefreshNews = new QPushButton("刷新资讯");
    btnRefreshNews->setStyleSheet(btnStyle);
    connect(btnRefreshNews, &QPushButton::clicked, this, [m_edtNews](){
        m_edtNews->setText("【实时更新·家居资讯】\n1、今日全屋联动算法优化上线；\n2、火警报警灵敏度档位可自定义调节；\n3、光照、姿态双传感器数据同步精度提升。");
    });
    newsLayout->addWidget(m_edtNews);
    newsLayout->addWidget(btnRefreshNews);
    infoLayout->addWidget(newsBox);

    layMonitor->addWidget(groupInfo);
    tabMain->addTab(pageMonitor, "📡 设备信息监测");

    // 根布局
    QVBoxLayout* rootLayout = new QVBoxLayout(this);
    rootLayout->setContentsMargins(0,0,0,0);
    rootLayout->addWidget(tabMain);

    // 网络、串口初始化
    m_netWeather = new QNetworkAccessManager(this);
    connect(m_netWeather, &QNetworkAccessManager::finished, this, &Widget::replyFinished);
    m_serial = new QSerialPort(this);

    // 信号槽绑定
    connect(m_btnSerial, &QPushButton::clicked, this, &Widget::onOpenSerial);
    connect(m_serial, &QSerialPort::readyRead, this, &Widget::readSerialData);
    connect(m_calendar, &QCalendarWidget::selectionChanged, this, &Widget::onCalendarChanged);

    connect(btnLivOn, &QPushButton::clicked, this, &Widget::ctrlLivingOn);
    connect(btnLivOff, &QPushButton::clicked, this, &Widget::ctrlLivingOff);
    connect(btnBedOn, &QPushButton::clicked, this, &Widget::ctrlBedOn);
    connect(btnBedOff, &QPushButton::clicked, this, &Widget::ctrlBedOff);
    connect(btnAcOn, &QPushButton::clicked, this, &Widget::ctrlAcOn);
    connect(btnAcOff, &QPushButton::clicked, this, &Widget::ctrlAcOff);
    connect(btnAlarmOn, &QPushButton::clicked, this, &Widget::ctrlAlarmOn);
    connect(btnAlarmOff, &QPushButton::clicked, this, &Widget::ctrlAlarmOff);

    // 发布通知Lambda（无头文件槽声明报错）
    connect(m_btnPublish, &QPushButton::clicked, this, [this](){
        QString txt = m_txtNotice->toPlainText().trimmed();
        if(txt.isEmpty())
        {
            QMessageBox::warning(this,"提示","请输入内容！");
            return;
        }
        QMessageBox::information(this,"成功","通知已发布！");
        m_txtNotice->append("--------------------------------");
        m_txtNotice->append("✅ " + txt);
    });
    connect(m_txtNotice, &QTextEdit::textChanged, this, [this](){
        m_txtNotice->verticalScrollBar()->setValue(m_txtNotice->verticalScrollBar()->maximum());
    });

    getWeather();
}

Widget::~Widget()
{
    if(m_serial->isOpen()) m_serial->close();
    if(m_timeTimer) m_timeTimer->stop();
}

void Widget::onOpenSerial()
{
    if(m_serial->isOpen()) {
        m_serial->close();
        m_btnSerial->setText("打开串口");
    } else {
        m_serial->setPortName(m_cmbPort->currentText());
        m_serial->setBaudRate(QSerialPort::Baud9600);
        m_serial->setDataBits(QSerialPort::Data8);
        m_serial->setParity(QSerialPort::NoParity);
        m_serial->setStopBits(QSerialPort::OneStop);
        m_serial->setFlowControl(QSerialPort::NoFlowControl);
        if(m_serial->open(QIODevice::ReadWrite)) {
            m_btnSerial->setText("关闭串口");
            QMessageBox::information(this, "提示", "串口打开成功");
        } else {
            QMessageBox::warning(this, "错误", "串口打开失败！");
        }
    }
}

void Widget::readSerialData()
{
    static QByteArray frameBuf;
    frameBuf.append(m_serial->readAll());
    while(frameBuf.size() >= 10)
    {
        int headIdx = frameBuf.indexOf((char)0xFF);
        if(headIdx == -1) { frameBuf.clear(); break; }
        if(headIdx + 9 >= frameBuf.size()) break;
        QByteArray frame = frameBuf.mid(headIdx,10);
        frameBuf.remove(0, headIdx+10);
        if(frame.at(9) == (char)0xAA)
        {
            uint8_t temp = frame.at(1);
            uint8_t humi = frame.at(2);
            int pitch = (int8_t)frame.at(3);
            if (temp > 35)
            {
                m_lcdTemp->setStyleSheet("color: red;");
                m_txtNotice->append(QString("⚠️ 高温预警！当前温度：%1℃").arg(temp));
            }
            else if (temp < 10)
            {
                m_lcdTemp->setStyleSheet("color: #0066cc;");
                m_txtNotice->append(QString("❄️ 低温提醒！当前温度：%1℃").arg(temp));
            }
            else
            {
                m_lcdTemp->setStyleSheet("color: #5C4B3E;");
            }
            if (humi > 85)
            {
                m_lcdHumi->setStyleSheet("color: red;");
                m_txtNotice->append(QString("💧 湿度过高！当前湿度：%1%").arg(humi));
            }
            else
            {
                m_lcdHumi->setStyleSheet("color: #5C4B3E;");
            }
            m_lcdTemp->display(temp);
            m_lcdHumi->display(humi);
            m_lcdAtt->display(pitch);
        }
    }
}

void Widget::onCalendarChanged()
{
    QDate d = m_calendar->selectedDate();
    m_txtNotice->append("📅 选中日期：" + d.toString("yyyy-MM-dd"));
}

void Widget::ctrlLivingOn()  { if(m_serial->isOpen()) m_serial->write("\x02"); }
void Widget::ctrlLivingOff() { if(m_serial->isOpen()) m_serial->write("\x03"); }
void Widget::ctrlBedOn()     { if(m_serial->isOpen()) m_serial->write("\x04"); }
void Widget::ctrlBedOff()    { if(m_serial->isOpen()) m_serial->write("\x05"); }
void Widget::ctrlAcOn()      { if(m_serial->isOpen()) m_serial->write("\x06"); }
void Widget::ctrlAcOff()     { if(m_serial->isOpen()) m_serial->write("\x07"); }
void Widget::ctrlAlarmOn()   { if(m_serial->isOpen()) m_serial->write("\x08"); }
void Widget::ctrlAlarmOff()  { if(m_serial->isOpen()) m_serial->write("\x09"); }

void Widget::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event);
    QPainter painter(this);
    QPixmap bg(":/img/1.jpg");
    QPixmap fitBg = bg.scaled(size(), Qt::KeepAspectRatioByExpanding, Qt::SmoothTransformation);
    int x = (width() - fitBg.width()) / 2;
    int y = (height() - fitBg.height()) / 2;
    painter.drawPixmap(x, y, fitBg);
}

void Widget::getWeather()
{
    qDebug() << "=== 发起天津天气网络请求 ===";
    QUrl url("http://wttr.in/tianjin?format=j1");
    QNetworkRequest request(url);
    m_netWeather->get(request);
}

void Widget::replyFinished(QNetworkReply *reply)
{
    if(reply->error() != QNetworkReply::NoError)
    {
        QString errMsg = reply->errorString();
        qDebug() << "天气请求失败：" << errMsg;
        m_lblWeaDesc->setText(QString("请求失败：%1").arg(errMsg));
        reply->deleteLater();
        return;
    }
    QByteArray data = reply->readAll();
    QJsonDocument doc = QJsonDocument::fromJson(data);
    QJsonObject root = doc.object();
    QJsonArray currentArr = root["current_condition"].toArray();
    if(currentArr.isEmpty())
    {
        m_lblWeaDesc->setText("天气数据解析为空");
        reply->deleteLater();
        return;
    }
    QJsonObject weatherObj = currentArr.first().toObject();
    QString temp = weatherObj["temp_C"].toString();
    QString humidity = weatherObj["humidity"].toString();
    QString windSpeed = weatherObj["windspeedKmph"].toString();
    QJsonArray descArr = weatherObj["weatherDesc"].toArray();
    QString weaDesc = "未知";
    if(!descArr.isEmpty())
    {
        QJsonObject descObj = descArr.first().toObject();
        weaDesc = descObj["value"].toString();
    }
    m_lblWeaCity->setText("城市：天津");
    m_lblWeaDesc->setText(QString("天气：%1 温度：%2℃ 湿度：%3%").arg(weaDesc,temp,humidity));
    m_lblWeaWind->setText(QString("风速：%1 km/h").arg(windSpeed));
    QDateTime now = QDateTime::currentDateTime();
    QString timeStr = now.toString("yyyy-MM-dd HH:mm:ss");
    m_lblWeaUpdate->setText(QString("更新于：%1").arg(timeStr));
    qDebug() << "===== 天津天气解析完成 =====";
    qDebug() << "温度：" << temp << "℃";
    qDebug() << "天气：" << weaDesc;
    qDebug() << "湿度：" << humidity << "%";
    qDebug() << "风速：" << windSpeed << "km/h";
    reply->deleteLater();
}
// 切换选中日期时触发
void Widget::onDateSelectChanged()
{
    // 先把当前正在编辑的内容保存
    saveCurrentNote();
    // 获取新选中日期
    QDate selectDate = m_calendar->selectedDate();
    QString dateKey = selectDate.toString("yyyy-MM-dd");
    // 加载该日期备注
    QString content = m_dateNoteMap.value(dateKey, "");
    // 阻塞信号，避免赋值触发重复save
    m_noteEdit->blockSignals(true);
    m_noteEdit->setPlainText(content);
    m_noteEdit->blockSignals(false);
}

// 实时保存当前日期的记事本内容
void Widget::saveCurrentNote()
{
    QDate curDate = m_calendar->selectedDate();
    QString dateKey = curDate.toString("yyyy-MM-dd");
    QString text = m_noteEdit->toPlainText();
    if(text.isEmpty())
    {
        m_dateNoteMap.remove(dateKey);
    }
    else
    {
        m_dateNoteMap.insert(dateKey, text);
    }
}
