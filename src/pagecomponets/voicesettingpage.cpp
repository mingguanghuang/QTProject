#include ".\includes\voicesettingpage.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGroupBox>
#include <QLabel>
#include <QComboBox>
#include <QSlider>
#include <QPushButton>
#include <QTextToSpeech>
#include <QSettings>
#include <QMessageBox>

VoiceSettingPage::VoiceSettingPage(QWidget *parent) : QWidget(parent)
{
    // 初始化语音引擎
    speech = new QTextToSpeech(this);
    settings = new QSettings("HMG", "VoiceControl", this);
    
    setupUI();
    loadVoiceSettings();
}

void VoiceSettingPage::setupUI()
{
    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    
    // 语音设置组
    QGroupBox *voiceGroup = new QGroupBox("语音设置", this);
    QVBoxLayout *voiceLayout = new QVBoxLayout(voiceGroup);
    
    // 语音选择
    QHBoxLayout *voiceSelectLayout = new QHBoxLayout();
    QLabel *voiceLabel = new QLabel("语音选择:", this);
    voiceComboBox = new QComboBox(this);
    voiceSelectLayout->addWidget(voiceLabel);
    voiceSelectLayout->addWidget(voiceComboBox);
    voiceSelectLayout->addStretch();
    
    // 语速设置
    QHBoxLayout *rateLayout = new QHBoxLayout();
    QLabel *rateTextLabel = new QLabel("语速:", this);
    rateSlider = new QSlider(Qt::Horizontal, this);
    rateSlider->setRange(-10, 10);
    rateSlider->setValue(0);
    rateLabel = new QLabel("0", this);
    rateLabel->setFixedWidth(30);
    rateLayout->addWidget(rateTextLabel);
    rateLayout->addWidget(rateSlider);
    rateLayout->addWidget(rateLabel);
    
    // 音调设置
    QHBoxLayout *pitchLayout = new QHBoxLayout();
    QLabel *pitchTextLabel = new QLabel("音调:", this);
    pitchSlider = new QSlider(Qt::Horizontal, this);
    pitchSlider->setRange(-10, 10);
    pitchSlider->setValue(0);
    pitchLabel = new QLabel("0", this);
    pitchLabel->setFixedWidth(30);
    pitchLayout->addWidget(pitchTextLabel);
    pitchLayout->addWidget(pitchSlider);
    pitchLayout->addWidget(pitchLabel);
    
    // 音量设置
    QHBoxLayout *volumeLayout = new QHBoxLayout();
    QLabel *volumeTextLabel = new QLabel("音量:", this);
    volumeSlider = new QSlider(Qt::Horizontal, this);
    volumeSlider->setRange(0, 100);
    volumeSlider->setValue(50);
    volumeLabel = new QLabel("50", this);
    volumeLabel->setFixedWidth(30);
    volumeLayout->addWidget(volumeTextLabel);
    volumeLayout->addWidget(volumeSlider);
    volumeLayout->addWidget(volumeLabel);
    
    // 测试按钮
    testButton = new QPushButton("测试语音", this);
    
    // 添加到语音设置组
    voiceLayout->addLayout(voiceSelectLayout);
    voiceLayout->addLayout(rateLayout);
    voiceLayout->addLayout(pitchLayout);
    voiceLayout->addLayout(volumeLayout);
    voiceLayout->addWidget(testButton);
    
    // 添加到主布局
    mainLayout->addWidget(voiceGroup);
    mainLayout->addStretch();
    
    // 连接信号槽
    connect(voiceComboBox, QOverload<int>::of(&QComboBox::currentIndexChanged),
            this, &VoiceSettingPage::onVoiceChanged);
    connect(rateSlider, &QSlider::valueChanged, this, &VoiceSettingPage::onRateChanged);
    connect(pitchSlider, &QSlider::valueChanged, this, &VoiceSettingPage::onPitchChanged);
    connect(volumeSlider, &QSlider::valueChanged, this, &VoiceSettingPage::onVolumeChanged);
    connect(testButton, &QPushButton::clicked, this, &VoiceSettingPage::onTestVoiceClicked);
    
    // 填充语音列表
    populateVoiceList();
    
    // 设置样式
    this->setStyleSheet("background-color: #1a1a1a; color: #ffffff;");
    voiceGroup->setStyleSheet(
        "QGroupBox {"
        "   color: #ffffff;"
        "   font-size: 14px;"
        "   border: 1px solid #555555;"
        "   border-radius: 5px;"
        "   margin-top: 10px;"
        "   padding-top: 10px;"
        "}"
        "QGroupBox::title {"
        "   subcontrol-origin: margin;"
        "   subcontrol-position: top center;"
        "   padding: 0 5px;"
        "}"
    );
    
    voiceLabel->setStyleSheet("color: #cccccc;");
    rateTextLabel->setStyleSheet("color: #cccccc;");
    pitchTextLabel->setStyleSheet("color: #cccccc;");
    volumeTextLabel->setStyleSheet("color: #cccccc;");
    
    voiceComboBox->setStyleSheet(
        "QComboBox {"
        "   background-color: #2a2a2a;"
        "   color: #ffffff;"
        "   border: 1px solid #555555;"
        "   border-radius: 3px;"
        "   padding: 5px;"
        "}"
        "QComboBox::drop-down {"
        "   border: none;"
        "}"
        "QComboBox::down-arrow {"
        "   image: none;"
        "   border-left: 1px solid #555555;"
        "   width: 15px;"
        "}"
    );
    
    rateSlider->setStyleSheet(
        "QSlider::groove:horizontal {"
        "   border: 1px solid #555555;"
        "   height: 5px;"
        "   background: #2a2a2a;"
        "   border-radius: 2px;"
        "}"
        "QSlider::handle:horizontal {"
        "   background: #4CAF50;"
        "   border: 1px solid #4CAF50;"
        "   width: 15px;"
        "   margin: -5px 0;"
        "   border-radius: 7px;"
        "}"
    );
    
    pitchSlider->setStyleSheet(
        "QSlider::groove:horizontal {"
        "   border: 1px solid #555555;"
        "   height: 5px;"
        "   background: #2a2a2a;"
        "   border-radius: 2px;"
        "}"
        "QSlider::handle:horizontal {"
        "   background: #2196F3;"
        "   border: 1px solid #2196F3;"
        "   width: 15px;"
        "   margin: -5px 0;"
        "   border-radius: 7px;"
        "}"
    );
    
    volumeSlider->setStyleSheet(
        "QSlider::groove:horizontal {"
        "   border: 1px solid #555555;"
        "   height: 5px;"
        "   background: #2a2a2a;"
        "   border-radius: 2px;"
        "}"
        "QSlider::handle:horizontal {"
        "   background: #FF9800;"
        "   border: 1px solid #FF9800;"
        "   width: 15px;"
        "   margin: -5px 0;"
        "   border-radius: 7px;"
        "}"
    );
    
    testButton->setStyleSheet(
        "QPushButton {"
        "   background-color: #4CAF50;"
        "   color: white;"
        "   border: none;"
        "   border-radius: 5px;"
        "   padding: 8px 16px;"
        "   font-size: 14px;"
        "}"
        "QPushButton:hover {"
        "   background-color: #45a049;"
        "}"
        "QPushButton:pressed {"
        "   background-color: #3d8b40;"
        "}"
    );
    
    rateLabel->setStyleSheet("color: #4CAF50;");
    pitchLabel->setStyleSheet("color: #2196F3;");
    volumeLabel->setStyleSheet("color: #FF9800;");
}

void VoiceSettingPage::populateVoiceList()
{
    voiceComboBox->clear();
    QVector<QVoice> voices = speech->availableVoices();
    
    for (const QVoice &voice : voices) {
        voiceComboBox->addItem(voice.name(), QVariant::fromValue(voice));
    }
    
    if (voices.isEmpty()) {
        voiceComboBox->addItem("默认语音");
    }
}

void VoiceSettingPage::loadVoiceSettings()
{
    // 加载保存的设置,语速（绝对值）,音高（绝对值）,音量（百分比）,这里语速和音高进行了归一化处理
    QString voiceName = settings->value("voice/name", "").toString();
    double rate = settings->value("voice/rate", 0.0).toDouble();
    double pitch = settings->value("voice/pitch", 0.0).toDouble();
    double volume = settings->value("voice/volume", 0.5).toDouble();
    
    // 应用设置到语音引擎
    speech->setRate(rate);
    speech->setPitch(pitch);
    speech->setVolume(volume);
    
    // 更新UI
    rateSlider->setValue(rate * 10); // 转换为滑块值
    pitchSlider->setValue(pitch * 10);
    volumeSlider->setValue(volume * 100);
    
    rateLabel->setText(QString::number(rate));
    pitchLabel->setText(QString::number(pitch));
    volumeLabel->setText(QString::number(volume));
    
    // 设置语音
    if (!voiceName.isEmpty()) {
        for (int i = 0; i < voiceComboBox->count(); ++i) {
            if (voiceComboBox->itemText(i) == voiceName) {
                voiceComboBox->setCurrentIndex(i);
                break;
            }
        }
    }
}

void VoiceSettingPage::saveVoiceSettings()
{
    // 保存当前设置
    if (voiceComboBox->currentIndex() >= 0) {
        settings->setValue("voice/name", voiceComboBox->currentText());
    }
    settings->setValue("voice/rate", speech->rate());
    settings->setValue("voice/pitch", speech->pitch());
    settings->setValue("voice/volume", speech->volume());
}

void VoiceSettingPage::onVoiceChanged(int index)
{
    if (index >= 0) {
        QVoice voice = voiceComboBox->itemData(index).value<QVoice>();
        speech->setVoice(voice);
        saveVoiceSettings();
    }
}

void VoiceSettingPage::onRateChanged(int value)
{
    double rate = value / 10.0;
    speech->setRate(rate);
    rateLabel->setText(QString::number(rate));
    saveVoiceSettings();
}

void VoiceSettingPage::onPitchChanged(int value)
{
    double pitch = value / 10.0;
    speech->setPitch(pitch);
    pitchLabel->setText(QString::number(pitch));
    saveVoiceSettings();
}

void VoiceSettingPage::onVolumeChanged(int value)
{
    double volume = value / 100.0;
    speech->setVolume(volume);
    volumeLabel->setText(QString::number(volume));
    saveVoiceSettings();
}

void VoiceSettingPage::onTestVoiceClicked()
{
    speech->say("这是语音设置测试，当前语速、音调和音量设置已生效");
}