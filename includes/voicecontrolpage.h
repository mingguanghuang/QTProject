#ifndef VOICECONTROLPAGE_H
#define VOICECONTROLPAGE_H

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
#include <QAudioSource>
#include <QMediaDevices>
#include <QAudioFormat>
#include "vosk_api.h"
#include "aimodelmanager.h"

class VoiceControlPage : public QWidget
{
    Q_OBJECT
public:
    explicit VoiceControlPage(QWidget *parent = nullptr);
    ~VoiceControlPage();

private slots:
    void toggleVoiceRecognition();
    void processAudioData();
    void onAiResponseReceived(const QString& response);
    void onErrorOccurred(const QString& error);

private:
    void setupUI();
    void startVoiceRecognition();
    void stopVoiceRecognition();
    void loadIcons();
    QString parseRecognitionResult(const char *result);
    void updateRecognitionText(const QString &text);
    void sendToAI(const QString &text);

    QPushButton *micButton;
    QLabel *recognitionLabel;
    QLabel *statusLabel;
    QLabel *aiResponseLabel; // 新增：显示AI回复的标签
    
    // 音频相关（Qt 6 API）
    QAudioSource *audioSource;
    QIODevice *audioIODevice;
    QTimer *audioTimer;

    // Vosk相关
    VoskModel *model;
    VoskRecognizer *recognizer;
    
    // AI模型管理器
    AIModelManager* aiManager;
    
    // 图标
    QIcon micIcon;
    QIcon listeningIcon;
    
    bool isListening;
};

#endif // VOICECONTROLPAGE_H