#include "ledpage.h"


LEDPage::LEDPage(QWidget *parent) : QWidget(parent)
{
    setupUI();
}

void LEDPage::setupUI()
{
    QVBoxLayout *layout = new QVBoxLayout(this);
    
    
    QLabel *content = new QLabel("这是LED控制页面\n功能待实现...");
    content->setStyleSheet("font-size: 16px; color: #cccccc;");
    content->setAlignment(Qt::AlignCenter);
    
    layout->addStretch();
    layout->addWidget(content);
    layout->addStretch();
    
    this->setStyleSheet("background-color: #2a2a2a;");
}
