#ifndef LEDPAGE_H
#define LEDPAGE_H

#include <QWidget>
#include <QVBoxLayout>
#include <QLabel>
#include <QPushButton>
#include <QDebug>
class LEDPage : public QWidget
{
    Q_OBJECT
public:
    explicit LEDPage(QWidget *parent = nullptr);

private:
    void setupUI();

    bool is_open = false;

private slots:
    void LEDController(void);


};

#endif // LEDPAGE_H
