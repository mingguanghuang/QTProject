#ifndef LEDPAGE_H
#define LEDPAGE_H

#include <QWidget>
#include <QVBoxLayout>
#include <QLabel>
#include <QPushButton>

class LEDPage : public QWidget
{
    Q_OBJECT
public:
    explicit LEDPage(QWidget *parent = nullptr);

private:
    void setupUI();
};

#endif // LEDPAGE_H