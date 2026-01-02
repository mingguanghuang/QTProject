#ifndef AIMODELMANAGER_H
#define AIMODELMANAGER_H

#include <QObject>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QJsonObject>
#include <QJsonDocument>
#include <QSettings>

class AIModelManager : public QObject
{
    Q_OBJECT

public:
    static AIModelManager* getInstance();
    
    // 模型设置
    void setModelSettings(const QString& model, const QString& address, const QString& port);
    void getModelSettings(QString& model, QString& address, QString& port);
    
    // 发送消息到AI模型
    void sendMessageToModel(const QString& message);
    
    // 保存和加载设置
    void saveSettings();
    void loadSettings();

signals:
    void aiResponseReceived(const QString& response);
    void errorOccurred(const QString& error);

private:
    explicit AIModelManager(QObject *parent = nullptr);
    ~AIModelManager();
    
    static AIModelManager* instance;
    
    QString currentModel;
    QString serverAddress;
    QString serverPort;
    QNetworkAccessManager* networkManager;
    QSettings* settings;
};

#endif // AIMODELMANAGER_H