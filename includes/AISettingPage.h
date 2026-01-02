#ifndef AISETTINGPAGE_H
#define AISETTINGPAGE_H

#include <QWidget>
#include <QVBoxLayout>
#include <QLabel>
#include <QPushButton>
#include <QComboBox>
#include <QLineEdit>
#include <QTextEdit>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QJsonObject>
#include <QJsonDocument>
#include <QMessageBox>
#include "aimodelmanager.h"

class AISettingPage : public QWidget
{
    Q_OBJECT
public:
    explicit AISettingPage(QWidget *parent = nullptr);

private slots:
    void onSendMessage();
    void onAiResponseReceived(const QString& response);
    void onErrorOccurred(const QString& error);

private:
    void setupUI();
    void updateModelSettings();

    // UI控件
    QComboBox *modelComboBox;
    QLineEdit *addressLineEdit;
    QLineEdit *portLineEdit;
    QTextEdit *chatTextEdit;
    QLineEdit *messageLineEdit;
    QPushButton *sendButton;
    
    // AI模型管理器
    AIModelManager* aiManager;
};

#endif // AISETTINGPAGE_H