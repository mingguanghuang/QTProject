#ifndef VOICESETTINGPAGE_H
#define VOICESETTINGPAGE_H

#include <QWidget>
#include <QVBoxLayout>
#include <QLabel>
#include <QPushButton>
#include <QComboBox>
#include <QSlider>
#include <QGroupBox>
#include <QTextToSpeech>
#include <QSettings>

class VoiceSettingPage : public QWidget
{
    Q_OBJECT
public:
    explicit VoiceSettingPage(QWidget *parent = nullptr);

private slots:
    void onVoiceChanged(int index);
    void onRateChanged(int value);
    void onPitchChanged(int value);
    void onVolumeChanged(int value);
    void onTestVoiceClicked();

private:
    void setupUI();
    void loadVoiceSettings();
    void saveVoiceSettings();
    void populateVoiceList();

    QTextToSpeech *speech;
    QSettings *settings;

    QComboBox *voiceComboBox;
    QSlider *rateSlider;
    QSlider *pitchSlider;
    QSlider *volumeSlider;
    QLabel *rateLabel;
    QLabel *pitchLabel;
    QLabel *volumeLabel;
    QPushButton *testButton;
};

#endif // VOICESETTINGPAGE_H