#include "appcore.h"

AppCore::AppCore(QObject *parent)
    : QObject{parent}
{
    m_timer = new QTimer(this);

    connect(m_timer, &QTimer::timeout, this, &AppCore::onTimeout);


    QString activeboot = "";
    char buf[16];

    FILE *fp = popen("/usr/sbin/fw_printenv activeboot 2>&1", "r");
    if (fp)
    {
        while (!feof(fp))
            if (fgets(buf, 16, fp) != NULL) activeboot.append(buf);
        pclose(fp);

        QString value = activeboot.at(11);
        if (QString::compare(value, "1") == 0){
            m_activeBoot = 1;

        }
        else if (QString::compare(value, "2") == 0){
            m_activeBoot = 2;
        }
    }
    if(QDir().exists("/opt/wren")){
        m_deviceName = "wren";
        m_appName = "wren_gui";
        m_appScriptName = "S41wren_gui";
        m_appScriptLiveName = "wren_gui_alive";
        m_appFs1Name = "mmcblk0p3";
        m_appFs2Name = "mmcblk0p8";
        m_rootFs1Name = "mmcblk0p2";
        m_rootFs2Name = "mmcblk0p7";
        m_currentDevice = DEVICE::WREN;
    }
    else if(QDir().exists("/opt/revolver")){
        m_deviceName = "revolver";
        m_appName = "revolver_gui";
        m_appScriptName = "S41revolver_gui";
        m_appScriptLiveName = "revolver_gui_alive";
        m_appFs1Name = "mmcblk0p7";
        m_appFs2Name = "mmcblk0p10";
        // m_rootFs1Name = "mmcblk0p6";
        // m_rootFs2Name = "mmcblk0p6";
        m_currentDevice = DEVICE::NETEXPERTXG2;
    }

    else{
        m_currentDevice = DEVICE::UNDEFINED;
    }
}

void AppCore::setFilePatch(const QString &filePath)
{
    // qDebug()<<"File patch"<<filePath;
    m_timer->stop();
    m_filePath = filePath;
    emit setUpdateMode();
}

void AppCore::startTimer(int time)
{
    m_timer->start(1000);
    m_SecondsToEnd = time;
    emit currentTime(time);
}

void AppCore::restart()
{
    m_timer->stop();
    QTimer::singleShot(100, this, [this]() {
        emit timerText(tr("Restarting..."));
    });
    QTimer::singleShot(400, this, [this]() {
        system(QString("/usr/bin/killall %1 >/dev/null 2>&1").arg(m_appName).toStdString().c_str());
        system(QString("/etc/init.d/%1 start > /dev/null").arg(m_appScriptName).toStdString().c_str());
        exit(2);
    });
}

void AppCore::boot()
{
    m_timer->stop();
    QTimer::singleShot(100, this, [this]() {
    emit timerText(tr("Booting..."));
    });
    QTimer::singleShot(400, this, []() {
        exit(1);
    });
}

void AppCore::update()
{

    //return;
    QTimer::singleShot(200, this, [this]() {
        QString command;
        // int activeboot = 0;


        QProcess proc;

        // system("/bin/mkdir /tmp/softwareUpdate");
        // if(/*activeboot*/m_activeBoot == 1){
        //     // qDebug()<<"____boot_1"<<QString("/bin/mount /dev/%1 /tmp/softwareUpdate").arg(m_appFs2Name).toStdString().c_str();
        //     system(QString("/bin/mount /dev/%1 /tmp/softwareUpdate").arg(m_appFs2Name).toStdString().c_str());
        // }
        // else{
        //     // qDebug()<<"____boot_2"<<QString("/bin/mount /dev/%1 /tmp/softwareUpdate").arg(m_appFs1Name).toStdString().c_str();
        //     system(QString("/bin/mount /dev/%1 /tmp/softwareUpdate").arg(m_appFs1Name).toStdString().c_str());
        // }

        updateUsr(proc);
        updateEtc(proc);


        proc.kill();

        // system("/bin/umount /tmp/softwareUpdate");
        // system("/bin/rmdir /tmp/softwareUpdate");

        command = "rm -r "+m_filePath;
        system(command.toStdString().c_str());
        command = "rm "+m_filePath+".tar";
        system(command.toStdString().c_str());
        system("/sbin/poweroff");
    });

}

int AppCore::activeBoot()
{
    return  m_activeBoot;
}

void AppCore::updateUsr(QProcess &proc)
{
    QString command;

    system("/bin/mkdir /tmp/softwareUpdate");
    if(/*activeboot*/m_activeBoot == 1){
        // qDebug()<<"____boot_1"<<QString("/bin/mount /dev/%1 /tmp/softwareUpdate").arg(m_appFs2Name).toStdString().c_str();
        system(QString("/bin/mount /dev/%1 /tmp/softwareUpdate").arg(m_appFs2Name).toStdString().c_str());
    }
    else{
        // qDebug()<<"____boot_2"<<QString("/bin/mount /dev/%1 /tmp/softwareUpdate").arg(m_appFs1Name).toStdString().c_str();
        system(QString("/bin/mount /dev/%1 /tmp/softwareUpdate").arg(m_appFs1Name).toStdString().c_str());
    }


    command = QString("/usr/bin/killall %1").arg(m_appName);   // >/dev/null 2>&1"; /// wren_gui
    proc.start(command);
    // qDebug()<<"____updateUsr_commanr_1"<<command<<
        proc.waitForFinished(-1);

    /// updating current partition
    command = QString("rsync -av --checksum %1/usr/ /opt/%2/usr/").arg(m_filePath, m_deviceName);
    proc.start(command);
    // qDebug()<<"____updateUsr_commanr_2"<<command<<
        proc.waitForFinished(-1);

    /// updating another partition
    command = QString("rsync -av --checksum %1/usr/ /tmp/softwareUpdate/usr/").arg(m_filePath);
    proc.start(command);
    // qDebug()<<"____updateUsr_commanr_1"<<command<<
        proc.waitForFinished(-1);


    system("/bin/umount /tmp/softwareUpdate");
    system("/bin/rmdir /tmp/softwareUpdate");

}

void AppCore::updateEtc(QProcess &proc)
{
    QString command;
    command = QString("rsync -av --checksum %1/etc/ /etc/").arg(m_filePath);
    proc.start(command);
    // qDebug()<<"____updateEtc_commanr_1"<<command<<
        proc.waitForFinished(-1);

    if(m_currentDevice == DEVICE::WREN){
        system("/bin/mkdir /tmp/rootFsUpdate");
        if(m_activeBoot == 1){
            system(QString("/bin/mount /dev/%1 /tmp/rootFsUpdate").arg(m_rootFs2Name).toStdString().c_str());
        }
        else{
            system(QString("/bin/mount /dev/%1 /tmp/rootFsUpdate").arg(m_rootFs1Name).toStdString().c_str());
        }

        command = QString("rsync -av --checksum %1/etc/ /tmp/rootFsUpdate/etc/").arg(m_filePath);
        proc.start(command);
        proc.waitForFinished(-1);
        command = "/bin/mount /tmp/rootFsUpdate";
        proc.start(command);
        proc.waitForFinished(-1);
        command = "rm -r /tmp/rootFsUpdate";
        proc.start(command);
        proc.waitForFinished(-1);
    }
}

void AppCore::onTimeout()
{
    emit currentTime(m_SecondsToEnd);
    if(m_SecondsToEnd <= 0){
        // m_timer->stop();
        // // system("/usr/bin/killall revolver_gui >/dev/null 2>&1"); /// wren_gui
        // system(QString("/usr/bin/killall %1 >/dev/null 2>&1").arg(m_appName).toStdString().c_str()); /// wren_gui
        // // system("/etc/init.d/S41revolver_gui start > /dev/null"); /// S41wren_gui
        // system(QString("/etc/init.d/%1 start > /dev/null").arg(m_appScriptName).toStdString().c_str()); /// S41wren_gui
        // exit(3);
        restart();
    }
    --m_SecondsToEnd;
}


