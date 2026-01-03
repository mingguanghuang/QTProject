#include ".\includes\voicecontrolpage_qt5.h"
#include <QMessageBox>
#include <QDir>
#include <QPixmap>
#include <QTextToSpeech>
#include <QVBoxLayout>
#include ".\includes\voicemanager.h"
#include "devicecontrol.h"  // 添加ControlPage头文件

VoiceControlPage::VoiceControlPage(QWidget *parent) : QWidget(parent)
{
    // 初始化变量
    audioInput = nullptr;
    audioIODevice = nullptr;
    audioTimer = nullptr;
    model = nullptr;
    recognizer = nullptr;
    isListening = false;
    mqttClient = nullptr;
    
    // 初始化AI模型管理器
    aiManager = AIModelManager::getInstance();
    connect(aiManager, &AIModelManager::aiResponseReceived, this, &VoiceControlPage::onAiResponseReceived);
    connect(aiManager, &AIModelManager::errorOccurred, this, &VoiceControlPage::onErrorOccurred);
    
    setupUI();
    loadIcons();
    setupMQTT();
}

VoiceControlPage::~VoiceControlPage()
{
    stopVoiceRecognition();
    if (recognizer) {
        vosk_recognizer_free(recognizer);
    }
    if (model) {
        vosk_model_free(model);
    }
    if (mqttClient) {
        mqttClient->disconnectFromBroker();
        delete mqttClient;
    }
}

void VoiceControlPage::setupUI()
{
    QVBoxLayout *layout = new QVBoxLayout(this);
    
    // 添加顶部间距
    layout->addStretch();
    
    // 麦克风按钮
    micButton = new QPushButton(this);
    micButton->setFixedSize(100, 100);
    micButton->setStyleSheet(
        "QPushButton {"
        "   border: 3px solid #4CAF50;"
        "   border-radius: 50px;"
        "   background-color: #2a2a2a;"
        "}"
        "QPushButton:hover {"
        "   background-color: #3a3a3a;"
        "   border-color: #66BB6A;"
        "}"
        "QPushButton:pressed {"
        "   background-color: #4a4a4a;"
        "}"
    );
    layout->addWidget(micButton, 0, Qt::AlignCenter);
    
    // 状态标签
    statusLabel = new QLabel("点击麦克风开始语音识别", this);
    statusLabel->setStyleSheet("font-size: 14px; color: #888888; margin: 10px;");
    statusLabel->setAlignment(Qt::AlignCenter);
    layout->addWidget(statusLabel);
    
    // 识别结果显示标签
    recognitionLabel = new QLabel("", this);
    recognitionLabel->setStyleSheet(
        "font-size: 16px; color: #ffffff;"
        "background-color: #2a2a2a;"
        "border: 1px solid #555555;"
        "border-radius: 10px;"
        "padding: 15px;"
        "min-height: 60px;"
        "margin: 20px;"
    );
    recognitionLabel->setAlignment(Qt::AlignCenter);
    recognitionLabel->setWordWrap(true);
    layout->addWidget(recognitionLabel);
    
    // 新增：AI回复显示标签
    aiResponseLabel = new QLabel("", this);
    aiResponseLabel->setStyleSheet(
        "font-size: 14px; color: #2196F3;"
        "background-color: #1a2a2a;"
        "border: 1px solid #2196F3;"
        "border-radius: 10px;"
        "padding: 15px;"
        "min-height: 80px;"
        "margin: 20px;"
    );
    aiResponseLabel->setAlignment(Qt::AlignCenter);
    aiResponseLabel->setWordWrap(true);
    layout->addWidget(aiResponseLabel);
    
    // 添加底部间距
    layout->addStretch();
    
    // 连接按钮点击信号
    connect(micButton, &QPushButton::clicked, this, &VoiceControlPage::toggleVoiceRecognition);
    
    this->setStyleSheet("background-color: #1a1a1a;");
}

void VoiceControlPage::loadIcons()
{
    // 创建简单的图标（可以使用图片资源进行替换，这里我懒得找图片了，直接让ai生成了一个icon图标）
    QPixmap micPixmap(80, 80);
    micPixmap.fill(Qt::transparent);
    QPainter painter(&micPixmap);
    painter.setRenderHint(QPainter::Antialiasing);
    painter.setPen(QPen(QColor(76, 175, 80), 4));
    painter.setBrush(QBrush(QColor(76, 175, 80)));
    
    // 麦克风图标
    painter.drawEllipse(10, 10, 60, 60);
    painter.setBrush(QBrush(QColor(42, 42, 42)));
    painter.drawEllipse(20, 20, 40, 40);
    painter.drawRect(35, 70, 10, 20);
    
    micIcon = QIcon(micPixmap);
    
    // 正在收听图标（红色）
    QPixmap listeningPixmap(80, 80);
    listeningPixmap.fill(Qt::transparent);
    QPainter painter2(&listeningPixmap);
    painter2.setRenderHint(QPainter::Antialiasing);
    painter2.setPen(QPen(QColor(244, 67, 54), 4));
    painter2.setBrush(QBrush(QColor(244, 67, 54)));
    
    // 脉动效果的麦克风图标
    painter2.drawEllipse(10, 10, 60, 60);
    painter2.setBrush(QBrush(QColor(42, 42, 42)));
    painter2.drawEllipse(20, 20, 40, 40);
    painter2.drawRect(35, 70, 10, 20);
    
    // 添加脉动效果
    painter2.setPen(QPen(QColor(244, 67, 54, 100), 2));
    painter2.setBrush(Qt::NoBrush);
    painter2.drawEllipse(5, 5, 70, 70);
    painter2.drawEllipse(0, 0, 80, 80);
    
    listeningIcon = QIcon(listeningPixmap);
    
    micButton->setIcon(micIcon);
    micButton->setIconSize(QSize(80, 80));
}

void VoiceControlPage::toggleVoiceRecognition()
{
    if (isListening) {
        stopVoiceRecognition();
    } else {
        startVoiceRecognition();
    }
}

void VoiceControlPage::startVoiceRecognition()
{
    VoiceManager* voiceManager = VoiceManager::getInstance();
    voiceManager->say("正在加载模型");
    
    // 检查Vosk模型路径
    QString modelPath = "E:/vosk-model-cn/vosk-model-cn-0.22"; // 请根据实际路径修改
    if (!QDir(modelPath).exists()) {
        QMessageBox::warning(this, "错误", "Vosk模型路径不存在，请检查路径配置");
        return;
    }
    
    // 加载Vosk模型
    model = vosk_model_new(modelPath.toLocal8Bit().constData());
    if (!model) {
        QMessageBox::warning(this, "错误", "无法加载Vosk模型");
        return;
    }
    
    // 创建识别器
    recognizer = vosk_recognizer_new(model, 16000.0);
    if (!recognizer) {
        QMessageBox::warning(this, "错误", "无法创建语音识别器");
        vosk_model_free(model);
        model = nullptr;
        return;
    }
    
    // 设置音频格式（QT5 API）
    QAudioFormat format;
    format.setSampleRate(16000);
    format.setChannelCount(1);
    format.setSampleSize(16);
    format.setCodec("audio/pcm");
    format.setByteOrder(QAudioFormat::LittleEndian);
    format.setSampleType(QAudioFormat::SignedInt);
    
    // 获取音频设备（QT5 API）
    QAudioDeviceInfo audioDeviceInfo = QAudioDeviceInfo::defaultInputDevice();
    if (audioDeviceInfo.isNull()) {
        QMessageBox::warning(this, "错误", "无法获取音频输入设备");
        stopVoiceRecognition();
        return;
    }
    
    // 检查设备是否支持所需格式
    if (!audioDeviceInfo.isFormatSupported(format)) {
        QMessageBox::warning(this, "错误", "音频设备不支持所需的格式");
        stopVoiceRecognition();
        return;
    }
    
    // 创建音频输入（QT5 API）
    audioInput = new QAudioInput(audioDeviceInfo, format, this);
    audioIODevice = audioInput->start();
    
    if (!audioIODevice) {
        QMessageBox::warning(this, "错误", "无法启动音频输入设备");
        stopVoiceRecognition();
        return;
    }
    
    // 创建定时器处理音频数据
    audioTimer = new QTimer(this);
    connect(audioTimer, &QTimer::timeout, this, &VoiceControlPage::processAudioData);
    audioTimer->start(100); // 每100ms处理一次音频数据
    
    // 更新UI状态
    isListening = true;
    micButton->setIcon(listeningIcon);
    statusLabel->setText("正在收听...");
    statusLabel->setStyleSheet("font-size: 14px; color: #4CAF50; margin: 10px;");
    recognitionLabel->setText("请开始说话...");
    voiceManager->say("请开始说话");
}

void VoiceControlPage::stopVoiceRecognition()
{
    if (audioTimer) {
        audioTimer->stop();
        delete audioTimer;
        audioTimer = nullptr;
    }
    
    if (audioInput) {
        audioInput->stop();
        delete audioInput;
        audioInput = nullptr;
        audioIODevice = nullptr;
    }
    
    if (recognizer) {
        vosk_recognizer_free(recognizer);
        recognizer = nullptr;
    }
    
    if (model) {
        vosk_model_free(model);
        model = nullptr;
    }
    
    // 更新UI状态
    isListening = false;
    micButton->setIcon(micIcon);
    statusLabel->setText("点击麦克风开始语音识别");
    VoiceManager* voiceManager = VoiceManager::getInstance();
    voiceManager->say("模型已关闭");
    statusLabel->setStyleSheet("font-size: 14px; color: #888888; margin: 10px;");
    recognitionLabel->setText("");
}

void VoiceControlPage::processAudioData()
{
    if (!audioIODevice || !recognizer) return;
    
    // 读取音频数据
    QByteArray data = audioIODevice->readAll();
    if (data.size() > 0) {
        // 处理音频数据
        if (vosk_recognizer_accept_waveform(recognizer, data.constData(), data.size())) {
            // 获取最终识别结果
            const char *result = vosk_recognizer_result(recognizer);
            QString text = parseRecognitionResult(result);
            if (!text.isEmpty()) {
                updateRecognitionText(text);
            }
        } else {
            // 获取部分识别结果
            const char *partial = vosk_recognizer_partial_result(recognizer);
            QString text = parseRecognitionResult(partial);
            if (!text.isEmpty()) {
                recognitionLabel->setText(text + " ...");
            }
        }
    }
}

QString VoiceControlPage::parseRecognitionResult(const char *result)
{
    if (!result) return "";
    
    QJsonDocument doc = QJsonDocument::fromJson(result);
    if (doc.isObject()) {
        QJsonObject obj = doc.object();
        if (obj.contains("text")) {
            return obj["text"].toString();
        } else if (obj.contains("partial")) {
            return obj["partial"].toString();
        }
    }
    return "";
}

void VoiceControlPage::updateRecognitionText(const QString &text)
{
    recognitionLabel->setText(text);
    
    // 如果识别到有效文本，自动发送给AI模型
    if (!text.isEmpty() && text != "请开始说话..." && !text.endsWith("...")) {
        sendToAI(text);
    }
}

void VoiceControlPage::sendToAI(const QString &text)
{
    // 显示发送状态
    aiResponseLabel->setText("正在发送到AI模型...");
    aiResponseLabel->setStyleSheet(
        "font-size: 14px; color: #FFA500;"
        "background-color: #2a2a1a;"
        "border: 1px solid #FFA500;"
        "border-radius: 10px;"
        "padding: 15px;"
        "min-height: 80px;"
        "margin: 20px;"
    );
    
    // 发送给AI模型
    aiManager->sendMessageToModel(text);
}

void VoiceControlPage::onAiResponseReceived(const QString& response)
{
    // 更新AI回复显示
    aiResponseLabel->setText(response);
    aiResponseLabel->setStyleSheet(
        "font-size: 14px; color: #2196F3;"
        "background-color: #1a2a2a;"
        "border: 1px solid #2196F3;"
        "border-radius: 10px;"
        "padding: 15px;"
        "min-height: 80px;"
        "margin: 20px;"
    );
}

void VoiceControlPage::onErrorOccurred(const QString& error)
{
    // 显示错误信息
    aiResponseLabel->setText("错误: " + error);
    aiResponseLabel->setStyleSheet(
        "font-size: 14px; color: #f44336;"
        "background-color: #2a1a1a;"
        "border: 1px solid #f44336;"
        "border-radius: 10px;"
        "padding: 15px;"
        "min-height: 80px;"
        "margin: 20px;"
    );
}

void VoiceControlPage::setupMQTT()
{
    // 创建MQTT客户端
    mqttClient = new MQTTClient(this);
    
    // 连接MQTT服务器
    if (mqttClient->connectToBroker()) {
        // 订阅主题
        mqttClient->subscribeToTopic("chenkaijie");
        
        // 连接消息接收信号
        connect(mqttClient, &MQTTClient::messageReceived, 
                this, &VoiceControlPage::onMQTTMessageReceived);
    }
}

void VoiceControlPage::setControlPage(ControlPage* controlPage)
{
    // 如果使用ControlPage的MQTT客户端，可以在这里实现
    // 当前版本使用独立的MQTT客户端
    Q_UNUSED(controlPage);
}

void VoiceControlPage::onMQTTMessageReceived(const QString &topic, const QJsonObject &message)
{
    // 检查是否为语音关闭命令
    if (topic == "chenkaijie" && 
        message.contains("command") && 
        message["command"].toString() == "VOICE_SHUTDOWN" &&
        message.contains("message_id")) {
        
        QString messageId = message["message_id"].toString();
        // 支持两种消息ID格式
        if (messageId == "message_voice" || messageId == "msg_voice") {
            qDebug() << "VoiceControlPage: 收到语音关闭命令";
            handleVoiceShutdownCommand();
        }
    }
}

void VoiceControlPage::handleVoiceShutdownCommand()
{
    // 停止语音识别
    if (isListening) {
        stopVoiceRecognition();
    }
    
    // 显示状态信息
    statusLabel->setText("语音功能已关闭");
    statusLabel->setStyleSheet("font-size: 14px; color: #f44336; margin: 10px;");
    recognitionLabel->setText("收到远程关闭指令，语音功能已禁用");
    aiResponseLabel->setText("语音功能已被远程关闭");
    aiResponseLabel->setStyleSheet(
        "font-size: 14px; color: #f44336;"
        "background-color: #2a1a1a;"
        "border: 1px solid #f44336;"
        "border-radius: 10px;"
        "padding: 15px;"
        "min-height: 80px;"
        "margin: 20px;"
    );
    
    // 禁用麦克风按钮
    micButton->setEnabled(false);
    
    // 语音提示
    VoiceManager* voiceManager = VoiceManager::getInstance();
    voiceManager->say("语音功能已被远程关闭");
    
    qDebug() << "VoiceControlPage: 语音功能已关闭";
}