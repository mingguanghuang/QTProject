#ifndef VOICECONTROLPAGE_QT5_H
#define VOICECONTROLPAGE_QT5_H

#include <QWidget>
#include <QVBoxLayout>
#include <QLabel>
#include <QPushButton>
#include <QTimer>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QIcon>
#include <QPainter>
#include <QAudioInput>
#include <QAudioDeviceInfo>
#include <QAudioFormat>
#include "vosk_api.h"
#include "aimodelmanager.h"
#include "mqttclient.h"

// 前向声明
class ControlPage;

class VoiceControlPage : public QWidget
{
    Q_OBJECT
public:
    explicit VoiceControlPage(QWidget *parent = nullptr);
    ~VoiceControlPage();

    // 添加设置ControlPage的方法
    void setControlPage(ControlPage* controlPage);

private slots:
    void toggleVoiceRecognition();
    void processAudioData();
    void onAiResponseReceived(const QString& response);
    void onErrorOccurred(const QString& error);
    void onMQTTMessageReceived(const QString &topic, const QJsonObject &message);  // 添加MQTT消息接收槽函数

private:
    void setupUI();
    void startVoiceRecognition();
    void stopVoiceRecognition();
    void loadIcons();
    QString parseRecognitionResult(const char *result);
    void updateRecognitionText(const QString &text);
    void sendToAI(const QString &text);
    void handleVoiceShutdownCommand();  // 处理语音关闭命令
    void setupMQTT();  // MQTT设置函数

    QPushButton *micButton;
    QLabel *recognitionLabel;
    QLabel *statusLabel;
    QLabel *aiResponseLabel; // 新增：显示AI回复的标签
    
    // 音频相关（QT5 API）
    QAudioInput *audioInput;  // QT5中使用QAudioInput
    QIODevice *audioIODevice;
    QTimer *audioTimer;

    // Vosk相关
    VoskModel *model;
    VoskRecognizer *recognizer;
    
    // AI模型管理器
    AIModelManager* aiManager;
    
    // MQTT客户端
    MQTTClient* mqttClient;
    
    // 图标
    QIcon micIcon;
    QIcon listeningIcon;
    
    bool isListening;
};

#endif // VOICECONTROLPAGE_QT5_H