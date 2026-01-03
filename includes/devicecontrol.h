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

class VoiceManager;

class ControlPage : public QWidget
{
    Q_OBJECT

public:
    explicit ControlPage(QWidget *parent = nullptr);
    ~ControlPage();
    
    // 添加公共方法供Widget调用
    void setupMQTT();
    MQTTClient* getMQTTClient() { return mqttClient; }  // 添加获取MQTT客户端的方法

private:
    void setupUI();
    void updateSensorDataDisplay(const QJsonObject &sensorData);
    QString getButtonStyle(bool isOn);
    void disconnectMQTT(); // 添加断开连接方法

    // UI组件
    QPushButton *led1Button;
    QPushButton *led2Button;
    QPushButton *led3Button;  
    QPushButton *buzzerButton;
    QPushButton *fanButton;
    QPushButton *sendDataButton;
    QPushButton *disconnectButton; // 添加断开连接按钮
    QLineEdit *dataInput;
    QLabel *temperatureLabel;
    QLabel *humidityLabel;
    QLabel *voltageLabel;
    QLabel *currentLabel;
    QLabel *intensityLabel;
    QLabel *peopleLabel;
    QLabel *flameLabel;
    QLabel *lightSensorLabel;
    QLabel *statusLabel;
    
    // MQTT客户端
    MQTTClient *mqttClient;
    
    // 状态变量
    bool isLed1On;
    bool isLed2On;
    bool isLed3On;  
    bool isBuzzerOn;
    bool isFanOn;
    double currentTemperature;

private slots:
    void toggleLED1();
    void toggleLED2();
    void toggleLED3();  
    void toggleBuzzer();
    void toggleFan();
    void sendData();
    void onMQTTMessageReceived(const QString &topic, const QJsonObject &message);
    void onMQTTSensorDataReceived(const QString &topic, const QJsonObject &sensorData);
    void onMQTTConnectionStatusChanged(bool connected);
    void onMQTTError(const QString &error);
};

#endif // DEVICECONTROL_H