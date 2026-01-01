#ifndef AISETTINGPAGE_H
#define AISETTINGPAGE_H

#include <QWidget>
#include <QVBoxLayout>
#include <QLabel>
#include <QPushButton>

class AISettingPage : public QWidget
{
    Q_OBJECT
public:
    explicit AISettingPage(QWidget *parent = nullptr);

private:
    void setupUI();
};

#endif // AISETTINGPAGE_H
