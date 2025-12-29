#ifndef SECURITYSETTINGPAGE_H
#define SECURITYSETTINGPAGE_H

#include <QWidget>
#include <QVBoxLayout>
#include <QLabel>
#include <QPushButton>

class SecuritySettingPage : public QWidget
{
    Q_OBJECT
public:
    explicit SecuritySettingPage(QWidget *parent = nullptr);

private:
    void setupUI();
};

#endif // SECURITYSETTINGPAGE_H