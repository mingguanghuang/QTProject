#ifndef DEVICECONTROL_H
#define DEVICECONTROL_H

#include <QWidget>
#include <QVBoxLayout>
#include <QLabel>
#include <QPushButton>
#include <QDebug>
class ControlPage : public QWidget
{
    Q_OBJECT
public:
    explicit ControlPage(QWidget *parent = nullptr);

private:
    void setupUI();

    bool is_open = false;

private slots:
    void LEDController(void);


};

#endif // DEVICECONTROL_H
