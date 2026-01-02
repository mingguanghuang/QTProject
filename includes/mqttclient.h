#ifndef MQTTCLIENT_H
#define MQTTCLIENT_H

#include <QObject>
#include <QMqttClient>
#include <QJsonObject>
#include <QJsonDocument>
#include <QTimer>
#include <QDateTime>

class MQTTClient : public QObject
{
    Q_OBJECT

public:
    explicit MQTTClient(QObject *parent = nullptr);
    ~MQTTClient();

    // 连接MQTT服务器
    bool connectToBroker(const QString &host = "localhost", quint16 port = 1883);
    
    // 断开连接
    void disconnectFromBroker();
    
    // 发布消息
    void publishMessage(const QString &topic, const QJsonObject &message);
    
    // 订阅主题
    void subscribeToTopic(const QString &topic);
    
    // 取消订阅
    void unsubscribeFromTopic(const QString &topic);
    
    // 获取连接状态
    bool isConnected() const;

signals:
    // 接收到消息的信号
    void messageReceived(const QString &topic, const QJsonObject &message);
    
    // 连接状态变化的信号
    void connectionStatusChanged(bool connected);
    
    // 错误信号
    void errorOccurred(const QString &error);

private slots:
    void onConnected();
    void onDisconnected();
    void onMessageReceived(const QByteArray &message, const QMqttTopicName &topic);
    void onError(QMqttClient::ClientError error);

private:
    QMqttClient *m_client;
    QTimer *m_reconnectTimer;
    QString m_host;
    quint16 m_port;
};

#endif // MQTTCLIENT_H