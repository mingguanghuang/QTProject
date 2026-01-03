#include ".\includes\voicemanager.h"

VoiceManager* VoiceManager::instance = nullptr;

VoiceManager* VoiceManager::getInstance()
{
    if (!instance) {
        instance = new VoiceManager();
    }
    return instance;
}

VoiceManager::VoiceManager(QObject *parent) : QObject(parent)
{
    speech = new QTextToSpeech(this);
    settings = new QSettings("HMG", "VoiceControl", this);
    loadSettings();
}

VoiceManager::~VoiceManager()
{
    saveSettings();
}

void VoiceManager::loadSettings()
{
    QString voiceName = settings->value("voice/name", "").toString();
    double rate = settings->value("voice/rate", 0.0).toDouble();
    double pitch = settings->value("voice/pitch", 0.0).toDouble();
    double volume = settings->value("voice/volume", 0.5).toDouble();
    
    speech->setRate(rate);
    speech->setPitch(pitch);
    speech->setVolume(volume);
    
    // 设置语音
    if (!voiceName.isEmpty()) {
        QVector<QVoice> voices = speech->availableVoices();
        for (const QVoice &voice : voices) {
            if (voice.name() == voiceName) {
                speech->setVoice(voice);
                break;
            }
        }
    }
}

void VoiceManager::saveSettings()
{
    if (speech->voice().name().isEmpty()) {
        settings->setValue("voice/name", "");
    } else {
        settings->setValue("voice/name", speech->voice().name());
    }
    settings->setValue("voice/rate", speech->rate());
    settings->setValue("voice/pitch", speech->pitch());
    settings->setValue("voice/volume", speech->volume());
}

void VoiceManager::setVoice(const QString &voiceName)
{
    QVector<QVoice> voices = speech->availableVoices();
    for (const QVoice &voice : voices) {
        if (voice.name() == voiceName) {
            speech->setVoice(voice);
            saveSettings();
            break;
        }
    }
}

void VoiceManager::setRate(double rate)
{
    speech->setRate(rate);
    saveSettings();
}

void VoiceManager::setPitch(double pitch)
{
    speech->setPitch(pitch);
    saveSettings();
}

void VoiceManager::setVolume(double volume)
{
    speech->setVolume(volume);
    saveSettings();
}

void VoiceManager::say(const QString &text)
{
    speech->say(text);
}

QVector<QVoice> VoiceManager::getAvailableVoices()
{
    return speech->availableVoices();
}

QString VoiceManager::getCurrentVoiceName() const
{
    return speech->voice().name();
}

double VoiceManager::getRate() const
{
    return speech->rate();
}

double VoiceManager::getPitch() const
{
    return speech->pitch();
}

double VoiceManager::getVolume() const
{
    return speech->volume();
}