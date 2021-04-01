#include "gui.h"

#include <QApplication>
#include <QFileDialog>
#include <QFontDialog>
#include <QTextStream>
#include <QValidator>

#include <QResizeEvent>

#include <QDebug>

GUI::GUI(QWidget *parent) : QWidget(parent)
{
    makeGUI();
}

GUI::~GUI()
{
    m_hlStatus->deleteLater();
    m_ipwInput->deleteLater();
    m_ipwOutput->deleteLater();

    m_leScale->deleteLater();
    m_leThreshold->deleteLater();
    m_pbToggleControls->deleteLater();
    m_pbQuit->deleteLater();
    m_pbChooseImage->deleteLater();
    m_pbChooseText->deleteLater();
    m_pbChooseFont->deleteLater();
    m_pbConvert->deleteLater();
    m_pbSave->deleteLater();

    m_leScaleValidator->deleteLater();
    m_leThresholdValidator->deleteLater();

    m_controlsLayout->deleteLater();
    m_mainLayout->deleteLater();
}

void GUI::makeGUI()
{
    m_hlStatus = new HistoryLabel("", this);
    m_hlStatus->AddMessage("Status");

    m_ipwInput = new ImagePreviewWidget(this);
    m_ipwOutput = new ImagePreviewWidget(this);

    m_pbToggleControls = new QPushButton("Turn ON", this);
    m_pbToggleControls->setCheckable(true);
    m_pbToggleControls->setChecked(true);

    m_teText = new QTextEdit(this);
    m_teText->setFixedHeight(textboxFixedHeight);

    m_leScale = new QLineEdit ("", this);
    m_leThreshold = new QLineEdit ("", this);
    m_leScale->setPlaceholderText("Scale: {1-13}");
    m_leThreshold->setPlaceholderText("Threshold:{0-255,0-255,0-255}");

    m_pbQuit = new QPushButton("X", this);
    m_pbChooseImage = new QPushButton("Choose Image", this);
    m_pbChooseText = new QPushButton("Choose Text", this);
    m_pbChooseFont = new QPushButton("Choose Font", this);
    m_pbConvert = new QPushButton ("Convert", this);
    m_pbSave = new QPushButton ("Save", this);

    m_pbToggleControls->setFixedSize(2*buttonControls,buttonControls);
    m_pbQuit->setFixedSize(buttonSmall, buttonSmall);
    m_pbChooseImage->setFixedWidth(buttonFixedWidth);
    m_pbChooseText->setFixedWidth(buttonFixedWidth);
    m_pbChooseFont->setFixedWidth(buttonFixedWidth);
    m_pbConvert->setFixedWidth(buttonFixedWidth);
    m_pbSave->setFixedWidth(buttonFixedWidth);

    disableControls();
    setupValidators();
    setupConnections();
    setupLayout();

    updateUI(false);
}

void GUI::setupLayout()
{
    m_mainLayout = new QGridLayout(this);

    m_mainLayout->addWidget(m_pbQuit, 0, 3, 1, 1, Qt::AlignRight);
    m_mainLayout->addWidget(m_hlStatus, 0, 0, 1, 4);

    m_mainLayout->addWidget(m_ipwInput, 1, 0, 5, 2);
    m_mainLayout->addWidget(m_ipwOutput, 1, 2, 5, 2);
    m_mainLayout->addWidget(m_pbToggleControls, 6, 0, 1, 4, Qt::AlignCenter);

    m_controlsLayout = new QGridLayout(this);
    m_controlsLayout->setSpacing(1);
    m_controlsLayout->setColumnStretch(0, 1);
    m_controlsLayout->setColumnStretch(1, 1);
    m_controlsLayout->setMargin(10);

    m_controlsLayout->addWidget(m_teText,        0, 0, 2, 4);

    m_controlsLayout->addWidget(m_pbChooseImage, 2, 0, 1, 1);
    m_controlsLayout->addWidget(m_pbChooseText,  2, 1, 1, 1);
    m_controlsLayout->addWidget(m_pbChooseFont,  2, 2, 1, 1);
    m_controlsLayout->addWidget(m_pbConvert,     2, 3, 1, 1);

    m_controlsLayout->addWidget(m_leScale,       3, 0, 1, 2);
    m_controlsLayout->addWidget(m_leThreshold,   3, 2, 1, 2);

    m_controlsLayout->addWidget(m_pbSave,        4, 0, 1, 4, Qt::AlignCenter);

    m_mainLayout->addLayout(m_controlsLayout,    8, 0, 5, 4);

    setLayout(m_mainLayout);
}




void GUI::setupValidators()
{
    // Here we set regular expression validator for our line edits
    // For scale we should not take any letters and signs, just single digit from 1 to 9 or pair of them
    // For threshold we should take several values from 0 to 255, so we separate them with some sign and
    // Put some constraints.

    m_leScaleValidator = new QIntValidator(1, 13, this);
    m_leThresholdValidator = new QRegExpValidator(QRegExp("[0-9]{1,3},[0-9]{1,3},[0-9]{1,3}"));

    m_leScale->setValidator(m_leScaleValidator);
    m_leThreshold->setValidator(m_leThresholdValidator);
}

void GUI::setupConnections()
{
    // Here we connect signals, emitted by widgets when they receive click, return pressed
    // or text changed events, with corresponding slots in this class.
    connect(m_pbToggleControls, SIGNAL(clicked()), this, SLOT(OnToggleControls()));
    connect(m_pbQuit, SIGNAL(clicked()), this, SLOT(OnQuit()));
    connect(m_pbChooseImage, SIGNAL(clicked()), this, SLOT(OnChooseImage()));
    connect(m_pbChooseText, SIGNAL(clicked()), this, SLOT(OnChooseText()));
    connect(m_pbChooseFont, SIGNAL(clicked()), this, SLOT(OnChooseFont()));    
    connect(m_pbConvert, SIGNAL(clicked()), this, SLOT(OnConvertButton()));
    connect(m_pbSave, SIGNAL(clicked()), this, SLOT(OnSaveButton()));
    connect(m_leScale, SIGNAL(returnPressed()), this, SLOT(OnScaleChanged()));
    connect(m_leThreshold, SIGNAL(returnPressed()), this, SLOT(OnThresholdChanged()));
    connect(m_teText, SIGNAL(textChanged()), this, SLOT(OnTextChanged()));
    connect(this, SIGNAL(UpdateStatus(const QString&)), this, SLOT(OnUpdateStatus(const QString&)));
}

void GUI::disableControls()
{
    // Disable all controls to let user choose parameters step by step.

    // m_pbChooseImage is enabled by default
    m_pbChooseText->setEnabled(false);
    m_pbChooseFont->setEnabled(false);
    m_leScale->setEnabled(false);
    m_leThreshold->setEnabled(false);
    m_pbConvert->setEnabled(false);
    m_pbSave->setEnabled(false);
}

void GUI::updateUI(bool showControls)
{
    qDebug() << QString("There are %1 items in the controls list").arg(m_controlsLayout->count());

    if (showControls)
    {
        m_mainLayout->addLayout(m_controlsLayout, 8, 0, 7, 4);

        for (int idx = 0; idx < m_controlsLayout->count(); ++idx)
        {
            QLayoutItem* item = m_controlsLayout->itemAt(idx);
            item->widget()->show();
        }
    }
    else
    {
        m_mainLayout->removeItem(m_controlsLayout);

        for (int idx = 0; idx < m_controlsLayout->count(); ++idx)
        {
            QLayoutItem* item = m_controlsLayout->itemAt(idx);
            item->widget()->hide();
        }
    }

    QRect g = geometry();
    g.setWidth(m_mainLayout->minimumSize().width());
    g.setHeight(m_mainLayout->minimumSize().height());
    setGeometry(g);

    // resize(m_mainLayout->minimumSize());
    updateGeometry();
    repaint();
}

void GUI::updateUIElement(ControlElement &element)
{
    // This switch statement is used for moving the user step by step across all
    // the stages of process. When everything is set, he may change settings.

    switch (element)
    {
        case ControlElement::ChooseImage:            
            m_pbChooseImage->setEnabled(false);
            m_pbChooseText->setEnabled(true);
            break;

        case ControlElement::ChooseText:
            m_pbChooseText->setEnabled(false);
            m_pbChooseFont->setEnabled(true);
            break;

        case ControlElement::ChooseFont:
            m_pbChooseFont->setEnabled(false);
            m_leScale->setEnabled(true);
            break;

        case ControlElement::ChooseScale:
            m_leScale->setEnabled(false);
            m_leThreshold->setEnabled(true);
            break;

        case ControlElement::ChooseThreshold:            
            m_pbChooseImage->setEnabled(true);
            m_pbChooseText->setEnabled(true);
            m_pbChooseFont->setEnabled(true);
            m_leScale->setEnabled(true);
            m_leThreshold->setEnabled(true);
            m_pbConvert->setEnabled(true);
            m_pbSave->setEnabled(false);
            break;

        case ControlElement::ButtonConvert:
            m_pbSave->setEnabled(true);
            break;

        case ControlElement::ButtonSave:
            break;
    }
}

void GUI::OnChooseImage()
{
    // First, we find input image on drive using QFileDialog static method.
    QString path = QFileDialog::getOpenFileName(this, "Choose Input Image", "D:/", "*.jpg *.png");

    // If path is empty, meaning we canceled, do nothing.
    if (path.isEmpty())
        return;

    // Send signal over airmail, someone will find it sometimes.
    emit ImageChanged(path);

    // Update status and set preview image.
    emit UpdateStatus(QString("New input image has been set: %1").arg(path));
    m_ipwInput->setImage(QImage(path));

    // Hide other UI elements to make user do it step by step.
    m_currentUIElement = ControlElement::ChooseImage;
    updateUIElement(m_currentUIElement);
}

void GUI::OnChooseText()
{
    // Find file with text we want to cipher.
    QString path = QFileDialog::getOpenFileName(0, "Choose Text File to Cipher", "D:/", "*.txt");

    // If path is empty string, do nothing.
    if (path.isEmpty())
        return;

    // If file on that path exists, open it as text in read only way.
    // Make stream instance and read all the data with its' help.
    if (QFile::exists(path))
    {
        QFile file (path);
        if (file.open(QIODevice::ReadOnly | QIODevice::Text))
        {
            QTextStream stream (&file);
            QString text = stream.readAll();

            // Show the text is corresponding textedit.
            m_teText->setText(text);

            // Yaaarrgh, we're sailing forwards. Update status.
            emit UpdateStatus(QString("New text has been set: %1").arg(path));

            // Send pigeon with letter tied to leg. Hope it won't be shot.
            emit TextChanged(text);

            // Don't forget to close the file stream, when we don't need it anymore.
            file.close();
        }
    }

    // Hide other UI elements.
    m_currentUIElement = ControlElement::ChooseText;
    updateUIElement(m_currentUIElement);
}

void GUI::OnChooseFont()
{
    // Here we are calling font dialog box to choose the font. The method getFont returns boolean value,
    // That indicates whether the result value is new font or default.
    bool bOk;
    QFont font = QFontDialog::getFont(&bOk);

    if (bOk)
        emit UpdateStatus("New font has been chosen.");
    else
        emit UpdateStatus("Default font has been chosen.");

    // Here we gather information into a string to pass it to the label,
    // So that we could see the parameters of the chosen font.
    QString fontItalic = (font.italic()) ? "true" : "false";
    QString fontBold = (font.bold()) ? "true" : "false";
    QString fontParametersText = QString("%1: S-%2 I-%3 B-%4.").arg(font.family()).arg(font.pointSize()).arg(fontItalic).arg(fontBold);

    // What would we do without statuses update?
    emit UpdateStatus(QString("Workflow font has been set to: %1").arg(fontParametersText));

    // Also we emit the font, so other class could use it later.
    // Kinda lot of emitting, strange...
    emit(FontChanged(font));

    // This extremely hard step again.
    m_currentUIElement = ControlElement::ChooseFont;
    updateUIElement(m_currentUIElement);
}

void GUI::OnScaleChanged()
{
    // Sender is an QObject base class, so lets cast it using such a handy tool!
    // Then we can take its text and store it in QString variable.
    QString string = static_cast<QLineEdit*>(sender())->text();

    // Here again. Meet the StAtUsEs!
    emit UpdateStatus(QString("Pixel scale parameter has been set to: %1").arg(string));

    // Scale is emitting. Savvy?
    emit ScaleChanged(string);

    // Once more.
    m_currentUIElement = ControlElement::ChooseScale;
    updateUIElement(m_currentUIElement);
}

void GUI::OnThresholdChanged()
{
    // When we clicked enter on threshold lineedit with regular expression validator,
    // It may send values from 0 to 999, but since components of color are constrained
    // in range 0 to 255, we make this trick to leave behind the need to write validator subclass.
    // We use constraintThreshold method to clamp the values into range we need.
    QString unconstrained = static_cast<QLineEdit*>(sender())->text();
    QString constrained = constraintThreshold(unconstrained);

    // Return resulting string of constrained values back to lineedit. And update statuses, of course.
    emit UpdateStatus(QString("RGB Threshold values have been set to: %1").arg(constrained));
    m_leThreshold->setText(constrained);

    // Something in the air. Do you hear?
    emit ThresholdChanged(constrained);

    // Here we go again.
    m_currentUIElement = ControlElement::ChooseThreshold;
    updateUIElement(m_currentUIElement);
}

void GUI::OnImageConverted(const QImage &image)
{
    // When this slot receives image, we update status and set preview item.
    emit UpdateStatus("Received Output Image. Trying to show its preview.");
    m_ipwOutput->setImage(image);

    // Also tune other UI elements
    m_currentUIElement = ControlElement::ButtonConvert;
    updateUIElement(m_currentUIElement);
}

void GUI::OnUpdateStatus(const QString& status)
{
    // This is the receiver slot for updates, sent by gui and logic class.
    m_hlStatus->AddMessage(status);
}

void GUI::OnTextChanged()
{
    emit TextChanged(static_cast<QTextEdit*>(sender())->toPlainText());
}

void GUI::OnConvertButton()
{
    emit ClickedConvert();
}

void GUI::OnSaveButton()
{
    QString path = QFileDialog::getSaveFileName(this, "Choose Output Image Filename", "D:/", "*.png");

    emit ClickedSave(path);
}

QString GUI::constraintThreshold(const QString& input)
{
    QStringList inputList = input.split(",");

    int term1 = inputList.at(0).toInt(); if (term1 > 255) term1 = 255;
    int term2 = inputList.at(1).toInt(); if (term2 > 255) term2 = 255;
    int term3 = inputList.at(2).toInt(); if (term3 > 255) term3 = 255;

    QString compoundString = QString("%1,%2,%3").arg(term1).arg(term2).arg(term3);
    return compoundString;
}

void GUI::OnToggleControls()
{
    // When user clicks toggle button, that is responsible for showing/hiding of controls, we do next:
    // If the button is in checked state, set its text to "Turn ON" and redraw UI using updateUI method with showControls = false.
    // If the button is not in checked state, set its text to "Turn OFF" and redraw UI using updateUI method with showControls = true.

    QPushButton* button = static_cast<QPushButton*>(sender());
    if (button->isCheckable())
    {
        if (button->isChecked())
        {
            button->setText("Turn ON");
            updateUI(false);
        }

        if (!button->isChecked())
        {
            button->setText("Turn OFF");
            updateUI(true);
        }        
    }

    int width = m_mainLayout->minimumSize().width();
    int height = m_mainLayout->minimumSize().height();
    emit UpdateUI(QString("%1,%2").arg(width).arg(height));
}

void GUI::OnQuit()
{
    qApp->quit();
}
