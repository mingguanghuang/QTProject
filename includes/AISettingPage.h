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

class AISettingPage : public QWidget
{
    Q_OBJECT
public:
    explicit AISettingPage(QWidget *parent = nullptr);

private slots:
    void onSendMessage();
    void onReplyFinished(QNetworkReply *reply);

private:
    void setupUI();
    void sendRequestToModel(const QString &message);

    // UI控件
    QComboBox *modelComboBox;
    QLineEdit *addressLineEdit;
    QLineEdit *portLineEdit;
    QTextEdit *chatTextEdit;
    QLineEdit *messageLineEdit;
    QPushButton *sendButton;
    
    // 网络管理器
    QNetworkAccessManager *networkManager;
};

#endif // AISETTINGPAGE_H