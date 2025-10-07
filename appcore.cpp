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
    if(QDir().exists("/opt/revolver")){
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
    else if(QDir().exists("/opt/wren")){
        m_deviceName = "wren";
        m_appName = "wren_gui";
        m_appScriptName = "S41wren_gui";
        m_appScriptLiveName = "wren_gui_alive";
        m_appFs1Name = "mmcblk0p3";
        m_appFs2Name = "mmcblk0p8";
        // m_rootFs1Name;
        // m_rootFs2Name;
        m_currentDevice = DEVICE::WREN;
    }
    else{
        m_currentDevice = DEVICE::UNDEFINED;
    }
}

void AppCore::setFilePatch(const QString &filePath)
{
    qDebug()<<"File patch"<<filePath;
    m_timer->stop();
    m_filePath = filePath;
    emit setUpdateMode();
}

int AppCore::getActiveBoot()
{
    return getActiveBoot();
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
    QTimer::singleShot(100, [this]() {
        emit timerText(tr("Restarting..."));
    });
    QTimer::singleShot(400, [this]() {
        system(QString("/usr/bin/killall %1 >/dev/null 2>&1").arg(m_appName).toStdString().c_str());
        system(QString("/etc/init.d/%1 start > /dev/null").arg(m_appScriptName).toStdString().c_str());
        exit(2);
    });
}

void AppCore::boot()
{
    m_timer->stop();
    QTimer::singleShot(100, [this]() {
    emit timerText(tr("Booting..."));
    });
    QTimer::singleShot(400, []() {
        exit(1);
    });
}

void AppCore::update()
{

    //return;
QTimer::singleShot(200, [this]() {

    QString command;
    int activeboot = 0;


    QProcess proc;
    proc.start("/usr/sbin/fw_printenv activeboot");
    proc.waitForFinished(-1);
    QString proc_stdout = proc.readAllStandardOutput();
    if(proc_stdout.contains("activeboot=")){
        activeboot = proc_stdout.split("=").last().remove("\n").toInt();
    }
    proc.kill();

    system("/bin/mkdir /tmp/softwareUpdate");
    if(activeboot == 1){
        // system("/bin/mount /dev/mmcblk0p8 /tmp/softwareUpdate");
        system(QString("/bin/mount /dev/%1 /tmp/softwareUpdate").arg(m_appFs2Name).toStdString().c_str());
    }
    else{
        // system("/bin/mount /dev/mmcblk0p3 /tmp/softwareUpdate");
        system(QString("/bin/mount /dev/%1 /tmp/softwareUpdate").arg(m_appFs1Name).toStdString().c_str());
    }

    // updateGui(proc);
    // updateGuiRestart(proc);
    // updateGuiAlive(proc);

    updateBin(proc);
    updateTranslations(proc);
    updateImages(proc);
    updateInitD(proc);

    if(m_currentDevice == DEVICE::WREN){
        updateVendorNames(proc);
    }


    proc.kill();

    system("/bin/umount /tmp/softwareUpdate");
    system("/bin/rmdir /tmp/softwareUpdate");

    command = "rm -r "+m_filePath;
    system(command.toStdString().c_str());
    command = "rm "+m_filePath+".tar";
    system(command.toStdString().c_str());
    system("/sbin/poweroff");
    });

}

int AppCore::activeBoot()
{
    m_activeBoot = 2;
    return  m_activeBoot;
}

void AppCore::updateBin(QProcess &proc)
{
    QString command;
    command = QString("/usr/bin/killall %1").arg(m_appName);   // >/dev/null 2>&1"; /// wren_gui
    proc.start(command);
    proc.waitForFinished(-1);

    /// updating current partition
    command = QString("rsync -av --checksum %1/bin/ /opt/%2/usr/local/bin/").arg(m_filePath, m_deviceName);
    proc.start(command);
    qDebug()<<"____updateBin_command_1"<<command<<proc.waitForFinished(-1);

    /// updating another partition
    command = QString("rsync -av --checksum %1/bin/ /tmp/softwareUpdate/usr/local/bin/").arg(m_filePath);
    proc.start(command);
    qDebug()<<"____updateBin_command_2"<<command<<proc.waitForFinished(-1);

}

void AppCore::updateTranslations(QProcess &proc)
{
    QString command;

    /// updating current partition
    command = QString("rsync -av --checksum %1/translations/ /opt/%2/usr/local/resource/").arg(m_filePath, m_deviceName);
    proc.start(command);
    qDebug()<<"____updateTranslations_command_1"<<command<<proc.waitForFinished(-1);

    /// updating another partition
    command = QString("rsync -av --checksum %1/translations/ /tmp/softwareUpdate/usr/local/resource/").arg(m_filePath);
    proc.start(command);
    qDebug()<<"____updateTranslations_command_2"<<command<<proc.waitForFinished(-1);
}

void AppCore::updateInitD(QProcess &proc)
{
    QString command;
    command = QString("rsync -av --checksum %1/init.d/ /etc/init.d/").arg(m_filePath);
    proc.start(command);
    qDebug()<<"____updateInitD_command"<<command<<proc.waitForFinished(-1);
}

void AppCore::updateGui(QProcess &proc)
{
    QString command;
    command = QString("/usr/bin/killall %1").arg(m_appName);   // >/dev/null 2>&1"; /// wren_gui
    proc.start(command);
    proc.waitForFinished(-1);

    command = QString("rm /opt/%1/usr/local/bin/%2").arg(m_deviceName, m_appName);
    proc.start(command);
    proc.waitForFinished(-1);

    // command = "/bin/cp "+m_filePath+"/bin/revolver_gui /opt/revolver/usr/local/bin/";   // >/dev/null 2>&1"; /// wren_gui
    command = QString("/bin/cp %1/bin/%2 /opt/%3/usr/local/bin/").arg(m_filePath, m_appName, m_deviceName);
    proc.start(command);
    proc.waitForFinished(-1);

    // command = "rm /tmp/revolver/usr/local/bin/revolver_gui"; /// wren_gui
    command = QString("rm /tmp/softwareUpdate/usr/local/bin/%1").arg(m_appName);
    proc.start(command);
    proc.waitForFinished(-1);

    command = "/bin/cp "+m_filePath+"/bin/revolver_gui /tmp/softwareUpdate/usr/local/bin/";
    proc.start(command);
    proc.waitForFinished(-1);
}

void AppCore::updateGuiRestart(QProcess &proc)
{
    QString command;
    // command = "rm /tmp/revolver/usr/local/bin/revolver_gui_restart"; /// wren_gui
    command = QString("rm /tmp/softwareUpdate/usr/local/bin/%1").arg(m_appName);
    proc.start(command);
    proc.waitForFinished(-1);

    // command = "/bin/cp "+m_filePath+"/bin/revolver_gui_restart /tmp/revolver/usr/local/bin/"; /// wren_gui
    command = QString("/bin/cp %1/bin/gui_system_recovery /tmp/softwareUpdate/usr/local/bin/").arg(m_filePath);
    proc.start(command);
    proc.waitForFinished(-1);
}

void AppCore::updateGuiAlive(QProcess &proc)
{
    QString command;
    // command = "rm /tmp/revolver/usr/local/bin/revolver_gui_alive"; /// wren_gui
    command = QString("rm /tmp/softwareUpdate/usr/local/bin/%1").arg(m_appScriptLiveName);
    proc.start(command);
    proc.waitForFinished(-1);

    // command = "/bin/cp "+m_filePath+"/bin/revolver_gui_alive /tmp/revolver/usr/local/bin/"; /// wren_gui_alive
    command = QString("/bin/cp %1/bin/%2 /tmp/%3/usr/local/bin/").arg(m_filePath, m_appScriptLiveName, m_deviceName);
    proc.start(command);
    proc.waitForFinished(-1);
}


void AppCore::updateVendorNames(QProcess &proc)
{
    QString command;
    // command = "rm /tmp/wren/usr/local/resource/vendorNamesCompact.json";
    command = QString("rm /tmp/softwareUpdate/usr/local/resource/vendorNamesCompact.json");
    proc.start(command);
    proc.waitForFinished(-1);

    // command = "rm /opt/wren/usr/local/resource/vendorNamesCompact.json";
    command = QString("rm /opt/%1/usr/local/resource/vendorNamesCompact.json").arg(m_deviceName);
    proc.start(command);
    proc.waitForFinished(-1);


    // command = "/bin/cp "+m_filePath+"/resource/vendorNamesCompact.json /opt/wren/usr/local/resource/";
    command = QString("/bin/cp %1/resource/vendorNamesCompact.json /opt/%2/usr/local/resource/").arg(m_filePath, m_deviceName);
    proc.start(command);
    proc.waitForFinished(-1);

    // command = "/bin/cp "+m_filePath+"/resource/vendorNamesCompact.json /tmp/wren/usr/local/resource/";
    command = QString("/bin/cp %1/resource/vendorNamesCompact.json /tmp/softwareUpdate/usr/local/resource/").arg(m_filePath);
    proc.start(command);
    proc.waitForFinished(-1);
}

void AppCore::updateImages(QProcess &proc)
{
    QString command;
    /// updating current partition
    command = QString("rsync -av --checksum %1/images/ /opt/%2/usr/local/images/").arg(m_filePath, m_deviceName);
    proc.start(command);
    qDebug()<<"____updateImages_command_1"<<command<<proc.waitForFinished(-1);

    /// updating another partition
    command = QString("rsync -av --checksum %1/images/ /tmp/softwareUpdate/usr/local/images/").arg(m_filePath);
    proc.start(command);
    qDebug()<<"____updateImages_command_2"<<command<<proc.waitForFinished(-1);
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


