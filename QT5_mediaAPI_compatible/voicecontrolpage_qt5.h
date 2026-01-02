// #ifndef VOICECONTROLPAGE_QT5_H
// #define VOICECONTROLPAGE_QT5_H

// #include <QWidget>
// #include <QVBoxLayout>
// #include <QLabel>
// #include <QPushButton>
// #include <QTimer>
// #include <QJsonDocument>
// #include <QJsonObject>
// #include <QJsonArray>
// #include <QIcon>
// #include <QPainter>
// #include <QAudioInput>
// #include <QAudioDeviceInfo>
// #include <QAudioFormat>
// #include "vosk_api.h"

// class VoiceControlPage : public QWidget
// {
//     Q_OBJECT
// public:
//     explicit VoiceControlPage(QWidget *parent = nullptr);
//     ~VoiceControlPage();

// private slots:
//     void toggleVoiceRecognition();
//     void processAudioData();
//     void updateRecognitionText(const QString &text);

// private:
//     void setupUI();
//     void startVoiceRecognition();
//     void stopVoiceRecognition();
//     void loadIcons();
//     QString parseRecognitionResult(const char *result);

//     QPushButton *micButton;
//     QLabel *recognitionLabel;
//     QLabel *statusLabel;
    
//     // 音频相关（QT5 API）
//     QAudioInput *audioInput;  // QT5中使用QAudioInput
//     QIODevice *audioIODevice;
//     QTimer *audioTimer;

//     // Vosk相关
//     VoskModel *model;
//     VoskRecognizer *recognizer;
    
//     // 图标
//     QIcon micIcon;
//     QIcon listeningIcon;
    
//     bool isListening;
// };

// #endif // VOICECONTROLPAGE_QT5_H
