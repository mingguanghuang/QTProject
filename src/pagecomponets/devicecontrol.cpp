#include ".\includes\devicecontrol.h"
#include ".\includes\voicemanager.h"
#include <QTextToSpeech>
#include <QPixmap>
#include <QMessageBox>
#include <QJsonObject>
#include <QJsonDocument>
#include <QDateTime>
#include <QGridLayout>

ControlPage::ControlPage(QWidget *parent) : QWidget(parent)
{
    // 初始化状态变量
    isLed1On = false;
    isLed2On = false;
    isLed3On = false;
    isBuzzerOn = false;
    isFanOn = false;
    currentTemperature = 0.0;
    
    mqttClient = new MQTTClient(this);
    
    setupUI();
    // setupMQTT();
}

ControlPage::~ControlPage()
{
    if (mqttClient) {
        mqttClient->disconnectFromBroker();
    }
}

void ControlPage::setupUI()
{
    // 创建主布局
    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    
    // 传感器数据显示区域
    QGroupBox *sensorGroup = new QGroupBox("传感器数据", this);
    QGridLayout *sensorLayout = new QGridLayout(sensorGroup);
    
    // 温度显示
    temperatureLabel = new QLabel("温度: -- °C", this);
    temperatureLabel->setStyleSheet("font-size: 16px; color: #ffffff; padding: 8px;");
    sensorLayout->addWidget(temperatureLabel, 0, 0);
    
    // 湿度显示
    humidityLabel = new QLabel("湿度: -- %", this);
    humidityLabel->setStyleSheet("font-size: 16px; color: #ffffff; padding: 8px;");
    sensorLayout->addWidget(humidityLabel, 0, 1);
    
    // 电压显示
    voltageLabel = new QLabel("电压: -- V", this);
    voltageLabel->setStyleSheet("font-size: 16px; color: #ffffff; padding: 8px;");
    sensorLayout->addWidget(voltageLabel, 1, 0);
    
    // 电流显示
    currentLabel = new QLabel("电流: -- A", this);
    currentLabel->setStyleSheet("font-size: 16px; color: #ffffff; padding: 8px;");
    sensorLayout->addWidget(currentLabel, 1, 1);
    
    // 光照强度显示
    intensityLabel = new QLabel("光照: -- lux", this);
    intensityLabel->setStyleSheet("font-size: 16px; color: #ffffff; padding: 8px;");
    sensorLayout->addWidget(intensityLabel, 2, 0);
    
    // 人员检测显示
    peopleLabel = new QLabel("人员: --", this);
    peopleLabel->setStyleSheet("font-size: 16px; color: #ffffff; padding: 8px;");
    sensorLayout->addWidget(peopleLabel, 2, 1);
    
    // 火焰检测显示
    flameLabel = new QLabel("火焰: --", this);
    flameLabel->setStyleSheet("font-size: 16px; color: #ffffff; padding: 8px;");
    sensorLayout->addWidget(flameLabel, 3, 0);
    
    // 光敏传感器显示
    lightSensorLabel = new QLabel("光敏: --", this);
    lightSensorLabel->setStyleSheet("font-size: 16px; color: #ffffff; padding: 8px;");
    sensorLayout->addWidget(lightSensorLabel, 3, 1);
    
    sensorGroup->setStyleSheet("QGroupBox { color: white; border: 1px solid gray; border-radius: 5px; margin-top: 6px; }"
                              "QGroupBox::title { subcontrol-origin: margin; left: 7px; padding: 0px 5px 0px 5px; font-size: 16px; }");
    mainLayout->addWidget(sensorGroup);
    
    // MQTT状态显示
    QGroupBox *statusGroup = new QGroupBox("连接状态", this);
    QVBoxLayout *statusLayout = new QVBoxLayout(statusGroup);
    
    statusLabel = new QLabel("MQTT: 未连接", this);
    statusLabel->setStyleSheet("font-size: 14px; color: #ff4444; padding: 5px;");
    statusLabel->setAlignment(Qt::AlignCenter);
    
    statusLayout->addWidget(statusLabel);
    //断开连接按钮
    disconnectButton = new QPushButton("断开连接", this);
    disconnectButton->setStyleSheet(
        "QPushButton { background-color: #f44336; color: white; border: none; border-radius: 5px; padding: 8px 16px; font-size: 14px; }"
        "QPushButton:hover { background-color: #d32f2f; }"
        "QPushButton:pressed { background-color: #b71c1c; }"
        "QPushButton:disabled { background-color: #666666; color: #999999; }"
    );
    disconnectButton->setFixedSize(100, 40);
    disconnectButton->setEnabled(false); // 初始状态禁用
    
    // 使用布局对齐方式居中按钮
    statusLayout->addWidget(disconnectButton, 0, Qt::AlignCenter);
    
    statusGroup->setStyleSheet("QGroupBox { color: white; border: 1px solid gray; border-radius: 5px; margin-top: 6px; }"
                              "QGroupBox::title { subcontrol-origin: margin; left: 7px; padding: 0px 5px 0px 5px; }");
    mainLayout->addWidget(statusGroup);
    
    // 设备控制区域
    QGroupBox *controlGroup = new QGroupBox("设备控制", this);
    QVBoxLayout *controlLayout = new QVBoxLayout(controlGroup);
    
    // 创建水平布局用于放置开关按钮
    QHBoxLayout *buttonLayout = new QHBoxLayout();
    
    // LED1按钮
    led1Button = new QPushButton("LED1", this);
    led1Button->setStyleSheet(getButtonStyle(false));
    led1Button->setFixedSize(100, 60);
    
    // LED2按钮
    led2Button = new QPushButton("LED2", this);
    led2Button->setStyleSheet(getButtonStyle(false));
    led2Button->setFixedSize(100, 60);
    
    // LED3按钮
    led3Button = new QPushButton("LED3", this);
    led3Button->setStyleSheet(getButtonStyle(false));
    led3Button->setFixedSize(100, 60);
    
    // 蜂鸣器按钮
    buzzerButton = new QPushButton("蜂鸣器", this);
    buzzerButton->setStyleSheet(getButtonStyle(false));
    buzzerButton->setFixedSize(100, 60);
    
    // 风扇按钮
    fanButton = new QPushButton("风扇", this);
    fanButton->setStyleSheet(getButtonStyle(false));
    fanButton->setFixedSize(100, 60);
    
    // 将按钮添加到水平布局
    buttonLayout->addWidget(led1Button);
    buttonLayout->addWidget(led2Button);
    buttonLayout->addWidget(led3Button);
    buttonLayout->addWidget(buzzerButton);
    buttonLayout->addWidget(fanButton);
    buttonLayout->setSpacing(10);
    buttonLayout->setAlignment(Qt::AlignCenter);
    
    // 数据发送区域
    QHBoxLayout *dataLayout = new QHBoxLayout();
    QLabel *dataLabel = new QLabel("数据发送:", this);
    dataLabel->setStyleSheet("color: white; font-size: 14px;");
    dataLabel->setFixedWidth(80);
    
    dataInput = new QLineEdit(this);
    dataInput->setPlaceholderText("输入数字或字符");
    dataInput->setStyleSheet("QLineEdit { background-color: #2a2a2a; color: white; border: 1px solid #555555; border-radius: 3px; padding: 8px; font-size: 14px; }");
    
    sendDataButton = new QPushButton("发送", this);
    sendDataButton->setStyleSheet("QPushButton { background-color: #4CAF50; color: white; border: none; border-radius: 5px; padding: 10px; font-size: 14px; }"
                                 "QPushButton:hover { background-color: #45a049; }"
                                 "QPushButton:pressed { background-color: #3d8b40; }");
    sendDataButton->setFixedSize(80, 40);
    
    dataLayout->addWidget(dataLabel);
    dataLayout->addWidget(dataInput);
    dataLayout->addWidget(sendDataButton);
    dataLayout->setSpacing(10);
    
    // 将水平布局添加到垂直布局
    controlLayout->addLayout(buttonLayout);
    controlLayout->addLayout(dataLayout);
    controlLayout->setSpacing(20);
    
    controlGroup->setStyleSheet("QGroupBox { color: white; border: 1px solid gray; border-radius: 5px; margin-top: 6px; }"
                               "QGroupBox::title { subcontrol-origin: margin; left: 7px; padding: 0px 5px 0px 5px; font-size: 16px; }");
    mainLayout->addWidget(controlGroup);
    
    // 连接信号槽
    connect(led1Button, &QPushButton::clicked, this, &ControlPage::toggleLED1);
    connect(led2Button, &QPushButton::clicked, this, &ControlPage::toggleLED2);
    connect(led3Button, &QPushButton::clicked, this, &ControlPage::toggleLED3);
    connect(buzzerButton, &QPushButton::clicked, this, &ControlPage::toggleBuzzer);
    connect(fanButton, &QPushButton::clicked, this, &ControlPage::toggleFan);
    connect(sendDataButton, &QPushButton::clicked, this, &ControlPage::sendData);
    connect(disconnectButton, &QPushButton::clicked, this, &ControlPage::disconnectMQTT);
    
    this->setStyleSheet("background-color: #1a1a1a;");
}

QString ControlPage::getButtonStyle(bool isOn)
{
    if (isOn) {
        return "QPushButton { background-color: #4CAF50; color: white; border: none; border-radius: 5px; }"
               "QPushButton:hover { background-color: #45a049; }"
               "QPushButton:pressed { background-color: #3d8b40; }";
    } else {
        return "QPushButton { background-color: #555555; color: white; border: none; border-radius: 5px; }"
               "QPushButton:hover { background-color: #666666; }"
               "QPushButton:pressed { background-color: #444444; }";
    }
}

void ControlPage::setupMQTT()
{
    // 连接MQTT信号槽
    connect(mqttClient, &MQTTClient::messageReceived, this, &ControlPage::onMQTTMessageReceived);
    connect(mqttClient, &MQTTClient::connectionStatusChanged, this, &ControlPage::onMQTTConnectionStatusChanged);
    connect(mqttClient, &MQTTClient::errorOccurred, this, &ControlPage::onMQTTError);
    
    // 连接MQTT服务器
    // 连接MQTT服务器
    if (mqttClient->connectToBroker("mqtt.yyzlab.com.cn", 1883)) {
        qDebug() << "MQTT: 连接中...";
        
    } else {
        qDebug() << "MQTT: 连接失败";
        
    }
    
    // 订阅主题
    mqttClient->subscribeToTopic("chenkaijie");
}



void ControlPage::updateSensorDataDisplay(const QJsonObject &sensorData)
{
    // 更新温度显示
    if (sensorData.contains("temp")) {
        double temperature = sensorData["temp"].toDouble();
        temperatureLabel->setText(QString("温度: %1 °C").arg(temperature, 0, 'f', 1));
    }
    
    // 更新湿度显示
    if (sensorData.contains("hum")) {
        double humidity = sensorData["hum"].toDouble();
        humidityLabel->setText(QString("湿度: %1 %").arg(humidity, 0, 'f', 1));
    }
    
    // 更新电压显示
    if (sensorData.contains("voltage")) {
        double voltage = sensorData["voltage"].toDouble();
        voltageLabel->setText(QString("电压: %1 V").arg(voltage, 0, 'f', 1));
    }
    
    // 更新电流显示
    if (sensorData.contains("current")) {
        double current = sensorData["current"].toDouble();
        currentLabel->setText(QString("电流: %1 A").arg(current, 0, 'f', 1));
    }
    
    // 更新光照强度显示
    if (sensorData.contains("intensity")) {
        double intensity = sensorData["intensity"].toDouble();
        intensityLabel->setText(QString("光照: %1 lux").arg(intensity, 0, 'f', 0));
    }
    
    // 更新人员检测显示
    if (sensorData.contains("people")) {
        bool peopleDetected = sensorData["people"].toBool();
        QString peopleText = peopleDetected ? "有人 🟢" : "无人 🔴";
        peopleLabel->setText(QString("人员: %1").arg(peopleText));
    }
    
    // 更新火焰检测显示
    if (sensorData.contains("flame")) {
        bool flameDetected = sensorData["flame"].toBool();
        QString flameText = flameDetected ? "有火焰 🔥" : "无火焰 ✅";
        QString flameStyle = flameDetected ? "color: #ff4444; font-weight: bold;" : "color: #ffffff;";
        flameLabel->setText(QString("火焰: %1").arg(flameText));
        flameLabel->setStyleSheet(QString("font-size: 16px; %1 padding: 8px;").arg(flameStyle));
    }
    
    // 更新光敏传感器显示
    if (sensorData.contains("light_sensor")) {
        bool lightSensor = sensorData["light_sensor"].toBool();
        QString lightText = lightSensor ? "有光照 ☀️" : "无光照 🌙";
        lightSensorLabel->setText(QString("光敏: %1").arg(lightText));
    }
}


void ControlPage::onMQTTSensorDataReceived(const QString &topic, const QJsonObject &sensorData)
{
    qDebug() << "收到传感器数据，主题:" << topic << "内容:" << sensorData;
    
    // 更新所有传感器数据显示
    updateSensorDataDisplay(sensorData);
    
    // // 语音播报温度
    // if (sensorData.contains("temp")) {
    //     double temperature = sensorData["temp"].toDouble();
    //      VoiceManager* speech = VoiceManager::getInstance();
    //     speech->say(QString("当前温度%1度").arg(temperature));
    // }
    
    // // 播报人员状态
    // if (sensorData.contains("people")) {
    //     bool peopleDetected = sensorData["people"].toBool();
    //      VoiceManager* speech = VoiceManager::getInstance();
    //     speech->say(peopleDetected ? "检测到人员" : "未检测到人员");
    // }
    
    // // 播报火焰状态
    // if (sensorData.contains("flame")) {
    //     bool flameDetected = sensorData["flame"].toBool();
    //     if (flameDetected) {
    //          VoiceManager* speech = VoiceManager::getInstance();
    //         speech->say("警告！检测到火焰");
    //     }
    // }
}

void ControlPage::toggleLED1()
{
    isLed1On = !isLed1On;
    led1Button->setStyleSheet(getButtonStyle(isLed1On));
    
    // 使用.cfg文件中的message_id格式
    QJsonObject message;
    message["command"] = isLed1On ? "LED_ON" : "LED_OFF";
    message["parameters"] = QJsonObject{{"led_id", 1}};
    message["message_id"] = isLed1On ? "msg_005" : "msg_006";
    
    mqttClient->publishMessage("huangmingguang", message);
    
    VoiceManager* voiceManager = VoiceManager::getInstance();
    voiceManager->say(isLed1On ? "LED1已打开" : "LED1已关闭");
    qDebug() << (isLed1On ? "LED1已打开" : "LED1已关闭");
}

void ControlPage::toggleLED2()
{
    isLed2On = !isLed2On;
    led2Button->setStyleSheet(getButtonStyle(isLed2On));
    
    // 使用.cfg文件中的message_id格式
    QJsonObject message;
    message["command"] = isLed2On ? "LED_ON" : "LED_OFF";
    message["parameters"] = QJsonObject{{"led_id", 2}};
    message["message_id"] = isLed2On ? "msg_005" : "msg_006";
    
    mqttClient->publishMessage("huangmingguang", message);
    
     VoiceManager* voiceManager = VoiceManager::getInstance();
    voiceManager->say(isLed2On ? "LED2已打开" : "LED2已关闭");
    qDebug() << (isLed2On ? "LED2已打开" : "LED2已关闭");
}

void ControlPage::toggleLED3()
{
    isLed3On = !isLed3On;
    led3Button->setStyleSheet(getButtonStyle(isLed3On));
    
    // 使用.cfg文件中的message_id格式
    QJsonObject message;
    message["command"] = isLed3On ? "LED_ON" : "LED_OFF";
    message["parameters"] = QJsonObject{{"led_id", 3}};
    message["message_id"] = isLed3On ? "msg_005" : "msg_006";
    
    mqttClient->publishMessage("huangmingguang", message);
    
     VoiceManager* voiceManager = VoiceManager::getInstance();
    voiceManager->say(isLed3On ? "LED3已打开" : "LED3已关闭");
    qDebug() << (isLed3On ? "LED3已打开" : "LED3已关闭");
}

void ControlPage::toggleBuzzer()
{
    isBuzzerOn = !isBuzzerOn;
    buzzerButton->setStyleSheet(getButtonStyle(isBuzzerOn));
    
    // 使用.cfg文件中的message_id格式
    QJsonObject message;
    message["command"] = isBuzzerOn ? "BEEPER_ON" : "BEEPER_OFF";
    message["parameters"] = QJsonObject();
    message["message_id"] = isBuzzerOn ? "msg_001" : "msg_002";
    
    mqttClient->publishMessage("huangmingguang", message);
    
     VoiceManager* voiceManager = VoiceManager::getInstance();
    voiceManager->say(isBuzzerOn ? "蜂鸣器已打开" : "蜂鸣器已关闭");
    qDebug() << (isBuzzerOn ? "蜂鸣器已打开" : "蜂鸣器已关闭");
}

void ControlPage::toggleFan()
{
    isFanOn = !isFanOn;
    fanButton->setStyleSheet(getButtonStyle(isFanOn));
    
    // 使用.cfg文件中的message_id格式
    QJsonObject message;
    message["command"] = isFanOn ? "FAN_ON" : "FAN_OFF";
    message["parameters"] = isFanOn ? QJsonObject{{"speed", 200}} : QJsonObject();
    message["message_id"] = isFanOn ? "msg_003" : "msg_004";
    
    mqttClient->publishMessage("huangmingguang", message);
    
     VoiceManager* voiceManager = VoiceManager::getInstance();
    voiceManager->say(isFanOn ? "风扇已打开" : "风扇已关闭");
    qDebug() << (isFanOn ? "风扇已打开" : "风扇已关闭");
}

void ControlPage::sendData()
{
    QString data = dataInput->text().trimmed();
    if (data.isEmpty()) {
        QMessageBox::warning(this, "警告", "请输入要发送的数据");
        return;
    }
    
    QJsonObject message;
    message["command"] = "DISPLAY_CONTROL";
    
    // 判断数据类型：数字或字符串
    bool isNumber;
    data.toInt(&isNumber);
    if (isNumber) {
        message["parameters"] = QJsonObject{{"display_num", data.toInt()}};
        message["message_id"] = "msg_007";
    } else {
        message["parameters"] = QJsonObject{{"display_str", data}};
        message["message_id"] = "msg_008";
    }
    
    mqttClient->publishMessage("huangmingguang", message);
    
     VoiceManager* voiceManager = VoiceManager::getInstance();
    voiceManager->say("数据已发送");
    qDebug() << "数据已发送:" << data;
    
    dataInput->clear();
}

void ControlPage::onMQTTMessageReceived(const QString &topic, const QJsonObject &message)
{
    qDebug() << "收到MQTT消息，主题:" << topic << "内容:" << message;
    VoiceManager* voiceManager = VoiceManager::getInstance();

    // 检查是否为传感器数据消息（包含传感器数据字段）
    if (message.contains("temp") || message.contains("hum") || message.contains("voltage") ||
        message.contains("current") || message.contains("intensity") || message.contains("people") ||
        message.contains("flame") || message.contains("light_sensor")) {

        qDebug() << "检测到传感器数据，转发到onMQTTSensorDataReceived";
        onMQTTSensorDataReceived(topic, message);
        return;
    }
    
    // 处理设备状态响应
    if (message.contains("status") && message["status"].toString() == "success") {
        QString command = message["command"].toString();
        QString messageId = message["message_id"].toString();
        
        qDebug() << "命令执行成功:" << command << "消息ID:" << messageId;
        
        // 根据message_id和command更新按钮状态
        if (messageId == "msg_005_response" || messageId == "msg_006_response") {
            // LED控制响应
            if (command == "LED_ON") {
                // 根据command中的led_id确定是哪个LED
                if (message.contains("parameters")) {
                    QJsonObject params = message["parameters"].toObject();
                    if (params.contains("led_id")) {
                        int ledId = params["led_id"].toInt();
                        switch (ledId) {
                            case 1:
                                isLed1On = true;
                                led1Button->setStyleSheet(getButtonStyle(true));
                                qDebug() << "LED1状态已更新为打开";
                                voiceManager->say("LED1状态已更新为打开");
                                break;
                            case 2:
                                isLed2On = true;
                                led2Button->setStyleSheet(getButtonStyle(true));
                                qDebug() << "LED2状态已更新为打开";
                                voiceManager->say("LED2状态已更新为打开");
                                break;
                            case 3:
                                isLed3On = true;
                                led3Button->setStyleSheet(getButtonStyle(true));
                                qDebug() << "LED3状态已更新为打开";
                                voiceManager->say("LED3状态已更新为打开");
                                break;
                        }
                    }
                }
            } else if (command == "LED_OFF") {
                // 根据command中的led_id确定是哪个LED
                if (message.contains("parameters")) {
                    QJsonObject params = message["parameters"].toObject();
                    if (params.contains("led_id")) {
                        int ledId = params["led_id"].toInt();
                        switch (ledId) {
                            case 1:
                                isLed1On = false;
                                led1Button->setStyleSheet(getButtonStyle(false));
                                qDebug() << "LED1状态已更新为关闭";
                                voiceManager->say("LED1状态已更新为关闭");
                                break;
                            case 2:
                                isLed2On = false;
                                led2Button->setStyleSheet(getButtonStyle(false));
                                qDebug() << "LED2状态已更新为关闭";
                                voiceManager->say("LED2状态已更新为关闭");
                                break;
                            case 3:
                                isLed3On = false;
                                led3Button->setStyleSheet(getButtonStyle(false));
                                qDebug() << "LED3状态已更新为关闭";
                                voiceManager->say("LED3状态已更新为关闭");
                                break;
                        }
                    }
                }
            }
        } else if (messageId == "msg_001_response" || messageId == "msg_002_response") {
            // 蜂鸣器响应
            if (command == "BEEPER_ON") {
                isBuzzerOn = true;
                buzzerButton->setStyleSheet(getButtonStyle(true));
                qDebug() << "蜂鸣器状态已更新为打开";
                voiceManager->say("蜂鸣器状态已更新为打开");
            } else if (command == "BEEPER_OFF") {
                isBuzzerOn = false;
                buzzerButton->setStyleSheet(getButtonStyle(false));
                qDebug() << "蜂鸣器状态已更新为关闭";
                voiceManager->say("蜂鸣器状态已更新为关闭");
            }
        }else if (messageId == "msg_003_response" || messageId == "msg_004_response") {
            //风扇控制
            if (command == "FAN_ON") {
                isFanOn = true;
                fanButton->setStyleSheet(getButtonStyle(true));
                qDebug() << "风扇状态已更新为打开";
                voiceManager->say("风扇状态已更新为打开");
            } else if (command == "FAN_OFF") {
                isFanOn = false;
                fanButton->setStyleSheet(getButtonStyle(false));
                qDebug() << "风扇状态已更新为关闭";
                voiceManager->say("风扇状态已更新为关闭");
            }
        }else if (messageId == "msg_007_response" || messageId == "msg_008_response") {
            // 数据显示控制响应
            qDebug() << "数据显示命令执行成功:" << command;
        }
        // // 语音播报成功状态
        //  VoiceManager* speech = VoiceManager::getInstance();
        // speech->say(QString("%1命令执行成功").arg(command));
    else if (message.contains("status") && message["status"].toString() == "error") {
        // 处理错误响应
        QString errorMsg = message["error_message"].toString();
        QString command = message["command"].toString();
        qDebug() << "命令执行失败:" << command << "错误信息:" << errorMsg;
        
        QMessageBox::warning(this, "命令执行失败", 
                            QString("命令 %1 执行失败：%2").arg(command,errorMsg));
        }
    }
}

void ControlPage::onMQTTConnectionStatusChanged(bool connected)
{
    if (connected) {
        statusLabel->setText("MQTT: 已连接");
        statusLabel->setStyleSheet("font-size: 14px; color: #4CAF50; padding: 5px;");
        disconnectButton->setEnabled(true);
        
        VoiceManager* voiceManager = VoiceManager::getInstance();
        voiceManager->say("MQTT连接成功");
        qDebug() << "MQTT连接成功";
    } else {
        statusLabel->setText("MQTT: 已断开");
        statusLabel->setStyleSheet("font-size: 14px; color: #ff4444; padding: 5px;");
        disconnectButton->setEnabled(false);
        
        VoiceManager* voiceManager = VoiceManager::getInstance();
        voiceManager->say("MQTT连接已断开");
        qDebug() << "MQTT连接已断开";
    }
}

void ControlPage::disconnectMQTT(){
    if(mqttClient){
        mqttClient->disconnectFromBroker();
    }
}

void ControlPage::onMQTTError(const QString &error)
{
    statusLabel->setText("MQTT: 错误");
    statusLabel->setStyleSheet("font-size: 14px; color: #ff4444; padding: 5px;");
    QMessageBox::warning(this, "MQTT错误", error);
    qDebug() << "MQTT错误:" << error;
}
