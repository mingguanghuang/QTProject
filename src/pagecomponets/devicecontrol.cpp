#include ".\includes\devicecontrol.h"
#include <QTextToSpeech>
#include <QPixmap>
#include <QMessageBox>
#include <QJsonObject>
#include <QJsonDocument>

ControlPage::ControlPage(QWidget *parent) : QWidget(parent)
{
    // 初始化状态变量
    isLed1On = false;
    isLed2On = false;
    isBuzzerOn = false;
    isFanOn = false;
    currentTemperature = 0.0;
    
    mqttClient = new MQTTClient(this);
    
    setupUI();
    setupMQTT();
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
    
    // 状态显示区域
    QGroupBox *statusGroup = new QGroupBox("设备状态", this);
    QVBoxLayout *statusLayout = new QVBoxLayout(statusGroup);
    
    // 温度显示
    temperatureLabel = new QLabel("当前温度: -- °C", this);
    temperatureLabel->setStyleSheet("font-size: 18px; color: #ffffff; padding: 10px;");
    temperatureLabel->setAlignment(Qt::AlignCenter);
    statusLayout->addWidget(temperatureLabel);
    
    // MQTT状态显示
    statusLabel = new QLabel("MQTT: 未连接", this);
    statusLabel->setStyleSheet("font-size: 14px; color: #ff4444; padding: 5px;");
    statusLabel->setAlignment(Qt::AlignCenter);
    statusLayout->addWidget(statusLabel);
    
    statusGroup->setStyleSheet("QGroupBox { color: white; border: 1px solid gray; border-radius: 5px; margin-top: 6px; }"
                              "QGroupBox::title { subcontrol-origin: margin; left: 7px; padding: 0px 5px 0px 5px; }");
    mainLayout->addWidget(statusGroup);
    
    // 设备控制区域
    QGroupBox *controlGroup = new QGroupBox("设备控制", this);
    QVBoxLayout *controlLayout = new QVBoxLayout(controlGroup);  // 改为垂直布局
    
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
    buttonLayout->addWidget(buzzerButton);
    buttonLayout->addWidget(fanButton);
    buttonLayout->setSpacing(15);  // 设置按钮间距
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
    controlLayout->setSpacing(20);  // 设置垂直间距
    
    controlGroup->setStyleSheet("QGroupBox { color: white; border: 1px solid gray; border-radius: 5px; margin-top: 6px; }"
                               "QGroupBox::title { subcontrol-origin: margin; left: 7px; padding: 0px 5px 0px 5px; font-size: 16px; }");
    mainLayout->addWidget(controlGroup);
    
    // 连接信号槽
    connect(led1Button, &QPushButton::clicked, this, &ControlPage::toggleLED1);
    connect(led2Button, &QPushButton::clicked, this, &ControlPage::toggleLED2);
    connect(buzzerButton, &QPushButton::clicked, this, &ControlPage::toggleBuzzer);
    connect(fanButton, &QPushButton::clicked, this, &ControlPage::toggleFan);
    connect(sendDataButton, &QPushButton::clicked, this, &ControlPage::sendData);
    
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
    if (!mqttClient->connectToBroker("mqtt.yyzlab.com.cn", 1883)) {
        statusLabel->setText("MQTT: 连接失败");
        statusLabel->setStyleSheet("font-size: 14px; color: #ff4444; padding: 5px;");
    }
    
    // 订阅温度主题
    mqttClient->subscribeToTopic("chenkaijie");
}

void ControlPage::updateTemperatureDisplay(double temperature)
{
    currentTemperature = temperature;
    
    QString styleSheet = "font-size: 18px; color: #ffffff; padding: 10px;";
    QString text = QString("当前温度: %1 °C").arg(temperature, 0, 'f', 1);
    
    // 温度超过阈值显示红色警报
    if (temperature > 37.5) {
        styleSheet = "font-size: 18px; color: #ff4444; font-weight: bold; padding: 10px; background-color: #330000; border: 2px solid #ff4444; border-radius: 5px;";
        text += " 🔥高温警报!";
    }
    
    temperatureLabel->setStyleSheet(styleSheet);
    temperatureLabel->setText(text);
}

void ControlPage::toggleLED1()
{
    isLed1On = !isLed1On;
    led1Button->setStyleSheet(getButtonStyle(isLed1On));
    
    QJsonObject message;
    message["command"] = isLed1On ? "LED1_ON" : "LED1_OFF";
    message["parameters"] = QJsonObject();
    message["message_id"] = QString("msg_led1_%1").arg(QDateTime::currentDateTime().toString("yyyyMMddhhmmss"));
    
    mqttClient->publishMessage("huangmingguang", message);
    
    QTextToSpeech* speech = new QTextToSpeech(this);
    speech->say(isLed1On ? "LED1已打开" : "LED1已关闭");
    qDebug() << (isLed1On ? "LED1已打开" : "LED1已关闭");
}

void ControlPage::toggleLED2()
{
    isLed2On = !isLed2On;
    led2Button->setStyleSheet(getButtonStyle(isLed2On));
    
    QJsonObject message;
    message["command"] = isLed2On ? "LED2_ON" : "LED2_OFF";
    message["parameters"] = QJsonObject();
    message["message_id"] = QString("msg_led2_%1").arg(QDateTime::currentDateTime().toString("yyyyMMddhhmmss"));
    
    mqttClient->publishMessage("huangmingguang", message);
    
    QTextToSpeech* speech = new QTextToSpeech(this);
    speech->say(isLed2On ? "LED2已打开" : "LED2已关闭");
    qDebug() << (isLed2On ? "LED2已打开" : "LED2已关闭");
}

void ControlPage::toggleBuzzer()
{
    isBuzzerOn = !isBuzzerOn;
    buzzerButton->setStyleSheet(getButtonStyle(isBuzzerOn));
    
    QJsonObject message;
    message["command"] = isBuzzerOn ? "BUZZER_ON" : "BUZZER_OFF";
    message["parameters"] = QJsonObject();
    message["message_id"] = QString("msg_buzzer_%1").arg(QDateTime::currentDateTime().toString("yyyyMMddhhmmss"));
    
    mqttClient->publishMessage("huangmingguang", message);
    
    QTextToSpeech* speech = new QTextToSpeech(this);
    speech->say(isBuzzerOn ? "蜂鸣器已打开" : "蜂鸣器已关闭");
    qDebug() << (isBuzzerOn ? "蜂鸣器已打开" : "蜂鸣器已关闭");
}

void ControlPage::toggleFan()
{
    isFanOn = !isFanOn;
    fanButton->setStyleSheet(getButtonStyle(isFanOn));
    
    QJsonObject message;
    message["command"] = isFanOn ? "FAN_ON" : "FAN_OFF";
    message["parameters"] = QJsonObject();
    message["message_id"] = QString("msg_fan_%1").arg(QDateTime::currentDateTime().toString("yyyyMMddhhmmss"));
    
    mqttClient->publishMessage("huangmingguang", message);
    
    QTextToSpeech* speech = new QTextToSpeech(this);
    speech->say(isFanOn ? "风扇已打开" : "风扇已关闭");
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
    message["command"] = "SEND_DATA";
    message["parameters"] = QJsonObject{{"data", data}};
    message["message_id"] = QString("msg_data_%1").arg(QDateTime::currentDateTime().toString("yyyyMMddhhmmss"));
    
    mqttClient->publishMessage("huangmingguang", message);
    
    QTextToSpeech* speech = new QTextToSpeech(this);
    speech->say("数据已发送");
    qDebug() << "数据已发送:" << data;
    
    dataInput->clear();
}

void ControlPage::onMQTTMessageReceived(const QString &topic, const QJsonObject &message)
{
    qDebug() << "收到MQTT消息，主题:" << topic << "内容:" << message;
    
    if (topic == "chenkaijie") {
        // 处理温度数据
        if (message.contains("command") && message["command"].toString() == "TEMPERATURE_UPDATE") {
            if (message.contains("result") && message["result"].isObject()) {
                QJsonObject result = message["result"].toObject();
                if (result.contains("temperature")) {
                    double temperature = result["temperature"].toDouble();
                    updateTemperatureDisplay(temperature);
                    
                    QTextToSpeech* speech = new QTextToSpeech(this);
                    speech->say(QString("当前温度%1度").arg(temperature));
                }
            }
        }
        
        // 处理设备状态响应
        if (message.contains("status") && message["status"].toString() == "success") {
            QString command = message["command"].toString();
            qDebug() << "命令执行成功:" << command;
        }
    }
}

void ControlPage::onMQTTConnectionStatusChanged(bool connected)
{
    if (connected) {
        statusLabel->setText("MQTT: 已连接");
        statusLabel->setStyleSheet("font-size: 14px; color: #4CAF50; padding: 5px;");
        qDebug() << "MQTT连接状态: 已连接";
    } else {
        statusLabel->setText("MQTT: 未连接");
        statusLabel->setStyleSheet("font-size: 14px; color: #ff4444; padding: 5px;");
        qDebug() << "MQTT连接状态: 未连接";
    }
}

void ControlPage::onMQTTError(const QString &error)
{
    statusLabel->setText("MQTT: 错误");
    statusLabel->setStyleSheet("font-size: 14px; color: #ff4444; padding: 5px;");
    QMessageBox::warning(this, "MQTT错误", error);
    qDebug() << "MQTT错误:" << error;
}