#include "homepage.h"
#include "../utils.h"
#include <QFormLayout>

HomePage::HomePage(QWidget *parent)
    : QWidget(parent),
      m_layout(new QVBoxLayout),
      m_cpuMonitorWidget(new MonitorWidget),
      m_memoryMonitorWidget(new MonitorWidget),
      m_diskMonitorWidget(new MonitorWidget),
      m_monitorThread(new MonitorThread)
{
    m_systemInfo = new QLabel(tr("SYSTEM INFO"));
    m_platform = new QLabel;
    m_distribution = new QLabel;
    m_bootTime = new QLabel;
    m_kernel = new QLabel;
    m_cpuModel = new QLabel;
    m_networkInfo = new QLabel(tr("NETWORK"));
    m_uploadLabel = new QLabel("0.0 B/s");
    m_uploadTotalLabel = new QLabel;
    m_downloadTotalLabel = new QLabel;
    m_downloadLabel = new QLabel("0.0 B/s");
    m_processInfo = new QLabel(tr("PROCESS"));
    m_allProcessLabel = new QLabel(tr("Loadding..."));

    initTopLayout();
    initBottomLayout();
    initUI();

    setLayout(m_layout);

    connect(m_monitorThread, &MonitorThread::updateCpuPercent, this, &HomePage::updateCpuPercent);
    connect(m_monitorThread, &MonitorThread::updateMemory, this, &HomePage::updateMemory);
    connect(m_monitorThread, &MonitorThread::updateDisk, this, &HomePage::updateDisk);
    connect(m_monitorThread, &MonitorThread::updateNetworkSpeed, this, &HomePage::updateNetworkSpeed);
    connect(m_monitorThread, &MonitorThread::updateNetworkTotal, this, &HomePage::updateNetworkTotal);
    // connect(m_monitorThread, &MonitorThread::updateProcessNumber, this, &HomePage::updateProcessNumber);
}

void HomePage::startMonitor()
{
    m_monitorThread->start();
}

void HomePage::stopMonitor()
{
    // Don't update the data when switching to other pages.
    if (m_monitorThread->isRunning()) {
        m_monitorThread->terminate();
    }
}

void HomePage::initTopLayout()
{
    QHBoxLayout *topLayout = new QHBoxLayout;
    topLayout->addSpacing(20);
    topLayout->addWidget(m_cpuMonitorWidget);
    topLayout->addStretch();
    topLayout->addWidget(m_memoryMonitorWidget);
    topLayout->addStretch();
    topLayout->addWidget(m_diskMonitorWidget);
    topLayout->addStretch();

    m_layout->addLayout(topLayout);
    m_layout->addStretch();
}

void HomePage::initBottomLayout()
{
    QWidget *widget = new QWidget;
    QVBoxLayout *layout = new QVBoxLayout;

    widget->setLayout(layout);

    QFormLayout *infoLayout = new QFormLayout;
    infoLayout->addRow(tr("Platform"), m_platform);
    infoLayout->addRow(tr("Distribution"), m_distribution);
    infoLayout->addRow(tr("Startup time"), m_bootTime);
    infoLayout->addRow(tr("Kernal Release"), m_kernel);
    infoLayout->addRow(tr("Processor"), m_cpuModel);
    // infoLayout->setVerticalSpacing(10);
    infoLayout->setHorizontalSpacing(30);
    infoLayout->setLabelAlignment(Qt::AlignLeft);

    // network layout.
//    QLabel *networkIcon = new QLabel;
//    QPixmap networkPixmap = Utils::renderSVG(":/resources/network.svg", QSize(16, 16));
//    networkIcon->setFixedSize(16, 16);
//    networkIcon->setPixmap(networkPixmap);

//    QLabel *uploadIcon = new QLabel;
//    QPixmap uploadPixmap = Utils::renderSVG(":/resources/upload.svg", QSize(16, 16));
//    uploadIcon->setFixedSize(16, 16);
//    uploadIcon->setPixmap(uploadPixmap);

//    QLabel *downloadIcon = new QLabel;
//    QPixmap downloadPixmap = Utils::renderSVG(":/resources/download.svg", QSize(16, 16));
//    downloadIcon->setFixedSize(16, 16);
//    downloadIcon->setPixmap(downloadPixmap);

    QWidget *uploadWidget = new QWidget;
    QHBoxLayout *uploadLayout = new QHBoxLayout(uploadWidget);
    uploadLayout->setMargin(0);
    // uploadLayout->addWidget(uploadIcon);
    uploadLayout->addWidget(m_uploadLabel);
    uploadLayout->addWidget(m_uploadTotalLabel);

    QWidget *downloadWidget = new QWidget;
    QHBoxLayout *downloadLayout = new QHBoxLayout(downloadWidget);
    downloadLayout->setMargin(0);
    // downloadLayout->addWidget(downloadIcon);
    downloadLayout->addWidget(m_downloadLabel);
    downloadLayout->addWidget(m_downloadTotalLabel);

    uploadWidget->setFixedWidth(200);
    downloadWidget->setFixedWidth(200);

    // add widgets
    infoLayout->addRow(tr("Upload"), uploadWidget);
    infoLayout->addRow(tr("Download"), downloadWidget);

    layout->addSpacing(20);
    layout->addLayout(infoLayout);
    layout->addSpacing(25);

    layout->setContentsMargins(QMargins(30, 20, 20, 20));

    m_layout->addWidget(widget);
    m_layout->addStretch();
}

void HomePage::initUI()
{
    QString strCpuModel;
    QString strCpuCore;
    Utils::getCpuInfo(strCpuModel, strCpuCore);

    strCpuModel = strCpuModel.trimmed();

    m_platform->setText(Utils::getPlatform());
    m_distribution->setText(Utils::getDistribution());
    m_bootTime->setText(Utils::getBootTime());
    m_kernel->setText(Utils::getKernelVersion());
    m_cpuModel->setText(QString("%1 x %2").arg(strCpuModel).arg(strCpuCore));

    QFont font;
    font.setPointSize(18);
    m_systemInfo->setFont(font);
    m_processInfo->setFont(font);
    m_networkInfo->setFont(font);

    font.setPointSize(11);
    m_platform->setFont(font);
    m_distribution->setFont(font);
    m_bootTime->setFont(font);
    m_kernel->setFont(font);
    m_cpuModel->setFont(font);
    m_uploadLabel->setFont(font);
    m_downloadLabel->setFont(font);
    m_allProcessLabel->setFont(font);
    m_downloadTotalLabel->setFont(font);
    m_uploadTotalLabel->setFont(font);

    m_systemInfo->setStyleSheet("QLabel { color: #4088C6 }");
    m_networkInfo->setStyleSheet("QLabel { color: #2CA7F8 }");
    m_processInfo->setStyleSheet("QLabel { color: #31A38C }");

    // init monitor widgets.
    m_cpuMonitorWidget->setTitle("CPU");
    m_cpuMonitorWidget->setTips(tr("CPU Idle"));
    m_memoryMonitorWidget->setTitle(tr("MEMORY"));
    m_diskMonitorWidget->setTitle(tr("DISK"));

    m_cpuMonitorWidget->setColor(QColor("#2CA7F8"));
    m_memoryMonitorWidget->setColor(QColor("#18BD9B"));
    m_diskMonitorWidget->setColor(QColor("#6F5BEC"));
}

void HomePage::updateCpuPercent(float cpuPercent)
{
    m_cpuMonitorWidget->setPercentValue(cpuPercent);

    if (cpuPercent > 0 && cpuPercent < 50) {
        m_cpuMonitorWidget->setTips(tr("CPU Idle"));
    } else if (cpuPercent >= 50 && cpuPercent < 100) {
        m_cpuMonitorWidget->setTips(tr("CPU Busy"));
    }
}

void HomePage::updateMemory(QString memory, float percent)
{
    m_memoryMonitorWidget->setTips(memory);
    m_memoryMonitorWidget->setPercentValue(percent);
}

void HomePage::updateDisk(QString disk, float percent)
{
    m_diskMonitorWidget->setTips(disk);
    m_diskMonitorWidget->setPercentValue(percent);
}

void HomePage::updateNetworkSpeed(QString upload, QString download)
{
    m_uploadLabel->setText(upload + "/s");
    m_downloadLabel->setText(download + "/s");
}

void HomePage::updateNetworkTotal(QString upload, QString download)
{
    m_uploadTotalLabel->setText(tr("total") + " " + upload);
    m_downloadTotalLabel->setText(tr("total") + " " + download);
}

void HomePage::updateProcessNumber(int num)
{
    m_allProcessLabel->setText(tr("%1 processes are running").arg(num));
}

