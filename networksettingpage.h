#ifndef NETWORKSETTINGPAGE_H
#define NETWORKSETTINGPAGE_H

#include <QWidget>
#include <QVBoxLayout>
#include <QLabel>
#include <QPushButton>

class NetworkSettingPage : public QWidget
{
    Q_OBJECT
public:
    explicit NetworkSettingPage(QWidget *parent = nullptr);

private:
    void setupUI();
};

#endif // NETWORKSETTINGPAGE_H