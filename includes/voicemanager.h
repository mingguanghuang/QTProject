#ifndef VOICEMANAGER_H
#define VOICEMANAGER_H

#include <QObject>
#include <QTextToSpeech>
#include <QSettings>

class VoiceManager : public QObject
{
    Q_OBJECT

public:
    static VoiceManager* getInstance();
    
    void setVoice(const QString &voiceName);
    void setRate(double rate);
    void setPitch(double pitch);
    void setVolume(double volume);
    
    void say(const QString &text);
    QVector<QVoice> getAvailableVoices();
    
    // 获取当前设置
    QString getCurrentVoiceName() const;
    double getRate() const;
    double getPitch() const;
    double getVolume() const;

private:
    explicit VoiceManager(QObject *parent = nullptr);
    ~VoiceManager();
    void loadSettings();
    void saveSettings();

    static VoiceManager* instance;
    QTextToSpeech *speech;
    QSettings *settings;
};

#endif // VOICEMANAGER_H