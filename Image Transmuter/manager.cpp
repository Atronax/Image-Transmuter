#include "manager.h"

#include <QFile>
#include <QTextStream>
#include <QHBoxLayout>

// for debugging purposes
#include <QDebug>
#include <QTimer>

Manager::Manager(QWidget *parent) : QWidget(parent)
{
    Init();
    MakeConnections();
}

Manager::~Manager()
{
    delete m_logic;
    m_logic = nullptr;

    delete m_gui;
    m_gui = nullptr;
}

void Manager::mousePressEvent(QMouseEvent *event)
{
    // If user clicked on left mouse button, we should set new value
    // for current mouse position in terms of window frame.
    if (event->button() == Qt::LeftButton)
    {
        m_mousePosition = event->globalPos() - frameGeometry().topLeft();
        event->accept();
    }
}

void Manager::mouseMoveEvent(QMouseEvent *event)
{
    if (event->buttons() & Qt::LeftButton)
    {
        move(event->globalPos() - m_mousePosition);
        event->accept();
    }
}

void Manager::Init()
{
    m_logic = new Logic(this);
    m_gui = new GUI (this);

    setWindowTitle("Image Transmuter");
    setWindowFlag(Qt::FramelessWindowHint);
    setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);

    connect(&m_timer, SIGNAL(timeout()), this, SLOT(OnTimer()));
    m_timer.setInterval(1000);
    m_timer.start();
    LoadQSS("d:/s1.qss");

    m_layout = new QHBoxLayout (this);
    m_layout->setSpacing(0);
    m_layout->setMargin(0);

    m_layout->addWidget(m_gui);
    setLayout(m_layout);

    // MakeRoundedCorners(30, 50, 70, 90);
}

void Manager::MakeConnections()
{
    // Controller connects separated classes of GUI and Logic using signal-slot subsystem.

    // Updater for graphical user interface
    connect(m_gui, SIGNAL(UpdateSize(const QString&)), this, SLOT(OnUpdateSize(const QString&)));

    // User chose new image in file dialog of GUI, Logic should set new image for image conversion.
    connect(m_gui, SIGNAL(ImageChanged(const QString&)), m_logic, SLOT(OnImageChanged(const QString&)));

    // User chose new text file in dialog of GUI, Logic should set new text for image conversion.
    connect(m_gui, SIGNAL(TextChanged(const QString&)), m_logic, SLOT(OnTextChanged(const QString&)));

    // User chose new font in corresponding dialog box, Logic should set new font for image conversion.
    connect(m_gui, SIGNAL(FontChanged(const QFont&)), m_logic, SLOT(OnFontChanged(const QFont&)));

    // User chose new scale, Logic should set it for image conversion.
    connect(m_gui, SIGNAL(ScaleChanged(const QString&)), m_logic, SLOT(OnScaleChanged(const QString&)));

    // User chose new threshold, Logic should use it for image conversion.
    connect(m_gui, SIGNAL(ThresholdChanged(const QString&)), m_logic, SLOT(OnThresholdChanged(const QString&)));

    // User clicked convert button, Logic should start convertation and send the resulting image back to gui.
    connect(m_gui, SIGNAL(ClickedConvert()), m_logic, SLOT(OnClickedConvert()));
    connect(m_logic, SIGNAL(UpdateStatus(const QString&)), m_gui, SLOT(OnUpdateStatus(const QString&)));
    connect(m_logic, SIGNAL(ImageConverted(const QImage&)), m_gui, SLOT(OnImageConverted(const QImage&)));

    // User clicked save button, Logic should try save the output image into png format file.
    connect(m_gui, SIGNAL(ClickedSave(const QString&)), m_logic, SLOT(OnClickedSave(const QString&)));

    // User clicked on unveil/hide controls button, manager should react by setting new size of window.
    connect(m_gui, SIGNAL(UpdateUI(const QString&)), this, SLOT(OnUpdateUI(const QString&)));
}

void Manager::LoadQSS(const QString& filename)
{
    // qDebug() << QString("Filename is: %1").arg(filename);
    QFile qssFile (filename);
    if (qssFile.open(QIODevice::ReadOnly))
    {
        QTextStream stream(&qssFile);
        m_qss = stream.readAll();
        // qDebug() << QString("m_qss is: %1").arg(m_qss);

        setStyleSheet(m_qss);

        qssFile.close();
    }
}

void Manager::MakeRoundedCorners(int radius_tl, int radius_tr, int radius_bl, int radius_br)
{
    QRegion region;
    QRegion round;
    QRegion corner;

    region = QRegion(0, 0, width(), height(), QRegion::Rectangle);

    // top left
    round  = QRegion(0, 0, 2*radius_tl, 2*radius_tl, QRegion::Ellipse);
    corner = QRegion(0, 0,   radius_tl,   radius_tl, QRegion::Rectangle);
    region = region.subtracted(corner.subtracted(round)); // вычитаем из угла кусок эллипса

    // top right
    round  = QRegion(width()-2*radius_tr, 0, 2*radius_tr, 2*radius_tr, QRegion::Ellipse);
    corner = QRegion(width()-  radius_tr, 0,   radius_tr,   radius_tr, QRegion::Rectangle);
    region = region.subtracted(corner.subtracted(round));

    // bottom right
    round  = QRegion(width()-2*radius_br, height()-2*radius_br, 2*radius_br, 2*radius_br, QRegion::Ellipse);
    corner = QRegion(width()-  radius_br, height()-  radius_br,   radius_br,   radius_br, QRegion::Rectangle);
    region = region.subtracted(corner.subtracted(round));

    // bottom left
    round  = QRegion(0, height()-2*radius_bl, 2*radius_bl, 2*radius_bl, QRegion::Ellipse);
    corner = QRegion(0, height()-  radius_bl,   radius_bl,   radius_bl, QRegion::Rectangle);
    region = region.subtracted(corner.subtracted(round));

    setMask(region);
}

void Manager::OnTimer()
{
    LoadQSS("d:/s1.qss");
}

void Manager::OnUpdateUI(const QString& size)
{
    // When we receive new size string of format {w,h}, we should split it into two terms using QStringList instance.
    // Having terms in form of string, we must convert them to integer values and use those as parameters o setFixedSize method.
    QStringList sizes = size.split(",");

    int width = sizes.at(0).toInt();
    int height = sizes.at(1).toInt();
    setFixedSize(width, height);
}
