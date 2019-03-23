#include "mainwindow.h"
#include "dtitlebar.h"
#include <QVBoxLayout>
#include <QApplication>
#include <QImageReader>
#include <QPushButton>

static QPixmap renderSVG(const QString &path, const QSize &size)
{
    QImageReader reader;
    QPixmap pixmap;
    reader.setFileName(path);

    if (reader.canRead()) {
        const qreal ratio = qApp->devicePixelRatio();
        reader.setScaledSize(size * ratio);
        pixmap = QPixmap::fromImage(reader.read());
        pixmap.setDevicePixelRatio(ratio);
    } else {
        pixmap.load(path);
    }

    return pixmap;
}

MainWindow::MainWindow(QWidget *parent)
    : DMainWindow(parent),
      m_stackedLayout(new QStackedLayout),
      m_homePage(new HomePage),
      m_networkManager(new NetworkManager),
      m_spinner(new DSpinner),
      m_statusLabel(new QLabel),
      m_resultLabel(new QLabel)
{
    QWidget *centralWidget = new QWidget;
    centralWidget->setLayout(m_stackedLayout);

    QWidget *spinnerPage = new QWidget;
    QVBoxLayout *spinnerLayout = new QVBoxLayout;
    spinnerPage->setLayout(spinnerLayout);
    spinnerLayout->addStretch();
    spinnerLayout->addWidget(m_spinner, 0, Qt::AlignHCenter);
    spinnerLayout->addSpacing(20);
    spinnerLayout->addWidget(m_statusLabel, 0, Qt::AlignHCenter);
    spinnerLayout->addStretch();
    m_spinner->setFixedSize(50, 50);
    m_spinner->stop();

    QWidget *resultPage = new QWidget;
    QVBoxLayout *resultLayout = new QVBoxLayout;
    QLabel *successIconLabel = new QLabel;
    QPixmap iconPixmap = renderSVG(":/images/success.svg", QSize(128, 128));
    QPushButton *returnBtn = new QPushButton("返回首页");
    successIconLabel->setPixmap(iconPixmap);
    resultLayout->addSpacing(30);
    resultLayout->addWidget(successIconLabel, 0, Qt::AlignHCenter);
    resultLayout->addSpacing(10);
    resultLayout->addWidget(m_resultLabel, 0, Qt::AlignHCenter);
    resultLayout->addSpacing(30);
    resultLayout->addWidget(returnBtn, 0, Qt::AlignHCenter);
    resultLayout->addStretch();
    resultPage->setLayout(resultLayout);

    QWidget *failedPage = new QWidget;
    QVBoxLayout *failedLayout = new QVBoxLayout;
    QLabel *failedIconLabel = new QLabel;
    QPixmap failedIconPixmap = renderSVG(":/images/failed.svg", QSize(128, 128));
    QLabel *failedTipsLabel = new QLabel("网络连接失败，请检查您的网络");
    QPushButton *failedReturnBtn = new QPushButton("返回首页");

    failedIconLabel->setPixmap(failedIconPixmap);
    failedPage->setLayout(failedLayout);

    failedLayout->addSpacing(30);
    failedLayout->addWidget(failedIconLabel, 0, Qt::AlignHCenter);
    failedLayout->addSpacing(10);
    failedLayout->addWidget(failedTipsLabel, 0, Qt::AlignHCenter);
    failedLayout->addSpacing(30);
    failedLayout->addWidget(failedReturnBtn, 0, Qt::AlignHCenter);
    failedLayout->addStretch();

    m_stackedLayout->addWidget(m_homePage);
    m_stackedLayout->addWidget(spinnerPage);
    m_stackedLayout->addWidget(resultPage);
    m_stackedLayout->addWidget(failedPage);

    if (titlebar()) {
        titlebar()->setBackgroundTransparent(true);
        titlebar()->setIcon(QIcon(":/images/icon.svg"));
        titlebar()->setTitle("");
    }

    setWindowIcon(QIcon(":/images/icon.svg"));
    setCentralWidget(centralWidget);
    setWindowRadius(16);
    setWindowTitle("网络测速");
    setFixedSize(500, 420);

    connect(returnBtn, &QPushButton::clicked, this, &MainWindow::switchToHomePage);
    connect(failedReturnBtn, &QPushButton::clicked, this, &MainWindow::switchToHomePage);
    connect(m_homePage, &HomePage::startButtonClicked, this, &MainWindow::handleTestBtnClicked);
    connect(m_networkManager, &NetworkManager::statusChanged, this, &MainWindow::updateStatus);
    connect(m_networkManager, &NetworkManager::testFailed, this, &MainWindow::switchToFailedPage);
    connect(m_networkManager, &NetworkManager::testSuccess, this, &MainWindow::handleSuccess);
}

MainWindow::~MainWindow()
{

}

void MainWindow::switchToHomePage()
{
    m_stackedLayout->setCurrentIndex(0);
    m_spinner->stop();
}

void MainWindow::switchToFailedPage()
{
    m_stackedLayout->setCurrentIndex(3);
    m_spinner->stop();
}

void MainWindow::handleTestBtnClicked()
{
    m_networkManager->startTest();
    m_stackedLayout->setCurrentIndex(1);
    m_spinner->start();

    m_statusLabel->setText("");
}

void MainWindow::handleSuccess(quint64 speed, QString speedStr)
{
    Q_UNUSED(speed);

    m_stackedLayout->setCurrentIndex(2);
    m_spinner->stop();
    m_resultLabel->setText(QString("您的上网速度为 %1").arg(speedStr));
}

void MainWindow::updateStatus(QString speed)
{
    m_statusLabel->setText(speed);
}
