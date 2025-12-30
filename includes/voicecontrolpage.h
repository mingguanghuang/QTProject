#ifndef VOICECONTROLPAGE_H
#define VOICECONTROLPAGE_H

#include <QWidget>
#include <QVBoxLayout>
#include <QLabel>
#include <QPushButton>

class VoiceControlPage : public QWidget
{
    Q_OBJECT
public:
    explicit VoiceControlPage(QWidget *parent = nullptr);

private:
    void setupUI();
};

#endif // VOICECONTROLPAGE_H