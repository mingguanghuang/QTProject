#ifndef DEVICEADDPAGE_H
#define DEVICEADDPAGE_H

#include <QWidget>
#include <QVBoxLayout>
#include <QLabel>
#include <QPushButton>

class DeviceAddPage : public QWidget
{
    Q_OBJECT
public:
    explicit DeviceAddPage(QWidget *parent = nullptr);

private:
    void setupUI();
};

#endif // DEVICEADDPAGE_H