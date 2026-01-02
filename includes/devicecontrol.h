#ifndef DEVICECONTROL_H
#define DEVICECONTROL_H

#include <QWidget>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QPushButton>
#include <QLineEdit>
#include <QTextEdit>
#include <QGroupBox>
#include <QDebug>
#include "mqttclient.h"

class ControlPage : public QWidget
{
    Q_OBJECT

public:
    explicit ControlPage(QWidget *parent = nullptr);
    ~ControlPage();

private:
    void setupUI();
    void setupMQTT();
    void updateTemperatureDisplay(double temperature);
    QString getButtonStyle(bool isOn);

    // UI组件
    QPushButton *led1Button;
    QPushButton *led2Button;
    QPushButton *buzzerButton;
    QPushButton *fanButton;  // 添加风扇按钮
    QPushButton *sendDataButton;
    QLineEdit *dataInput;
    QLabel *temperatureLabel;
    QLabel *statusLabel;
    
    // MQTT客户端
    MQTTClient *mqttClient;
    
    // 状态变量
    bool isLedOn;
    bool isLed1On;
    bool isLed2On;
    bool isBuzzerOn;
    bool isFanOn;  // 添加风扇状态变量
    double currentTemperature;

private slots:
    void toggleLED1();
    void toggleLED2();
    void toggleBuzzer();
    void toggleFan();  // 添加风扇控制槽函数
    void sendData();
    void onMQTTMessageReceived(const QString &topic, const QJsonObject &message);
    void onMQTTConnectionStatusChanged(bool connected);
    void onMQTTError(const QString &error);
};

#endif // DEVICECONTROL_H
