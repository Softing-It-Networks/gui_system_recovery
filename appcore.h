#ifndef APPCORE_H
#define APPCORE_H

#include <QObject>
#include <QTimer>
#include <QProcess>
#include <QFile>
#include <QCryptographicHash>
#include <QDirIterator>
#include <QDebug>

class AppCore : public QObject
{
    Q_OBJECT
public:
    typedef enum {
        UNDEFINED,
        NETEXPERTXG2,
        WREN
    } DEVICE;
    explicit AppCore(QObject *parent = nullptr);
    void setFilePatch(const QString &filePath);
    Q_INVOKABLE void startTimer(int time);
    Q_INVOKABLE void restart();
    Q_INVOKABLE void boot();
    Q_INVOKABLE void update();
    Q_INVOKABLE int activeBoot();
private:
    void updateUsr(QProcess &proc);
    void updateEtc(QProcess &proc);
    int m_SecondsToEnd = 0;
    QString m_filePath;
    QTimer *m_timer;
    int m_activeBoot= 0;
    QString m_deviceName;
    QString m_appName;
    QString m_appScriptName;
    QString m_appScriptLiveName;
    QString m_appFs1Name;
    QString m_appFs2Name;
    // QString m_rootFs1Name;
    // QString m_rootFs2Name;
    DEVICE m_currentDevice;

signals:
    void currentTime(int time);
    void timerText(QString text);
    void setUpdateMode();

private slots:
    void onTimeout();
};

#endif // APPCORE_H
