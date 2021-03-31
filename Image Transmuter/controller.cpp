#include "controller.h"

#include <QFile>
#include <QTextStream>
#include <QHBoxLayout>

// for debugging purposes
#include <QDebug>
#include <QTimer>

Controller::Controller(QWidget *parent) : QWidget(parent)
{
    Init();
    MakeConnections();
}

Controller::~Controller()
{
    delete m_logic;
    m_logic = nullptr;

    delete m_gui;
    m_gui = nullptr;
}

void Controller::Init()
{
    m_logic = new Logic(this);
    m_gui = new GUI (this);

    setWindowTitle("Image Transmuter");
    setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);
    // setFixedSize(700,800);

    // connect(&m_timer, SIGNAL(timeout()), this, SLOT(OnTimer()));
    m_timer.setInterval(1000);
    m_timer.start();
    LoadQSS("d:/s1.qss");

    m_layout = new QHBoxLayout (this);
    m_layout->addWidget(m_gui);
    setLayout(m_layout);
}

void Controller::MakeConnections()
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

    // User clicked convert button, Logic should start convertation and send the resulting image back to gui
    connect(m_gui, SIGNAL(ClickedConvert()), m_logic, SLOT(OnClickedConvert()));
    connect(m_logic, SIGNAL(UpdateStatus(const QString&)), m_gui, SLOT(OnUpdateStatus(const QString&)));
    connect(m_logic, SIGNAL(ImageConverted(const QImage&)), m_gui, SLOT(OnImageConverted(const QImage&)));

    // User clicked save button, Logic should try save the output image into png format file
    connect(m_gui, SIGNAL(ClickedSave(const QString&)), m_logic, SLOT(OnClickedSave(const QString&)));

}

void Controller::LoadQSS(const QString& filename)
{
    qDebug() << QString("Filename is: %1").arg(filename);
    QFile qssFile (filename);
    if (qssFile.open(QIODevice::ReadOnly))
    {
        QTextStream stream(&qssFile);
        m_qss = stream.readAll();
        qDebug() << QString("m_qss is: %1").arg(m_qss);

        setStyleSheet(m_qss);

        qssFile.close();
    }
}

void Controller::OnTimer()
{
    LoadQSS("d:/s1.qss");
}
