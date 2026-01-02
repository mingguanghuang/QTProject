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

class VoiceControlPage : public QWidget
{
    Q_OBJECT
public:
    explicit VoiceControlPage(QWidget *parent = nullptr);
    ~VoiceControlPage();

private slots:
    void toggleVoiceRecognition();
    void processAudioData();
    void updateRecognitionText(const QString &text);

private:
    void setupUI();
    void startVoiceRecognition();
    void stopVoiceRecognition();
    void loadIcons();
    // void LoaddingStatusUI();
    QString parseRecognitionResult(const char *result);

    QPushButton *micButton;
    QLabel *recognitionLabel;
    QLabel *statusLabel;
    
    // 音频相关（Qt 6 API）
    QAudioSource *audioSource; //在QT5中使用QAudioInput *audioInput 替换QAudioSource
    QIODevice *audioIODevice;
    QTimer *audioTimer;

    // Vosk相关
    VoskModel *model;
    VoskRecognizer *recognizer;
    
    // 图标
    QIcon micIcon;
    QIcon listeningIcon;
    
    bool isListening;
};

#endif // VOICECONTROLPAGE_H
