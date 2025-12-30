#ifndef VOICESETTINGPAGE_H
#define VOICESETTINGPAGE_H

#include <QWidget>
#include <QVBoxLayout>
#include <QLabel>
#include <QPushButton>

class VoiceSettingPage : public QWidget
{
    Q_OBJECT
public:
    explicit VoiceSettingPage(QWidget *parent = nullptr);

private:
    void setupUI();
};

#endif // VOICESETTINGPAGE_H