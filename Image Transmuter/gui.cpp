#include "gui.h"

#include <QApplication>
#include <QFileDialog>
#include <QFontDialog>
#include <QTextStream>
#include <QValidator>

#include <QDebug>
#include "types.h"

GUI::GUI(QWidget *parent) : QWidget(parent)
{
    makeGUI();
    // defaults();
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
    // m_hlStatus->AddMessage("Status");

    m_ipwInput = new ImagePreviewWidget(this, true, false);
    m_ipwOutput = new ImagePreviewWidget(this, true, false);

    m_pbToggleControls = new QPushButton("Turn ON", this);
    m_pbToggleControls->setCheckable(true);
    m_pbToggleControls->setChecked(true);

    m_teText = new QTextEdit(this);
    m_teText->setFixedHeight(textboxFixedHeight);    

    m_leScale = new QLineEdit ("", this);
    m_leScale->setPlaceholderText("Scale: {1-13}");

    m_leThreshold = new QLineEdit ("", this);   
    m_leThreshold->setPlaceholderText("Threshold: {0-255,0-255,0-255}");

    m_pbQuit = new QPushButton("X", this);
    m_pbChooseImage = new QPushButton("Choose Image", this);
    m_pbChooseText = new QPushButton("Choose Text", this);
    m_pbChooseFont = new QPushButton("Choose Font", this);
    m_pbConvert = new QPushButton ("Convert", this);
    m_pbSave = new QPushButton ("Save", this);

    m_pbChooseImage->setToolTip("<b>Input Image.</b> When image file is chosen, preview should be visible in top left widget.");
    m_pbChooseText->setToolTip("<b>Text.</b> If size of the text is not enough, it would be repeated until needed size.");
    m_pbChooseFont->setToolTip("<b>Font.</b> You can choose between system fonts. If canceled, default one is set.");
    m_leThreshold->setToolTip("<b>Threshold.</b> Set for rgb components of pixels of input image. If their value is less then set threshold, input pixels won't be changed to output characters.");
    m_leScale->setToolTip("<b>Scale.</b> Output image pixels' size is calculated by multiplying input image pixels' size by scale factor. Scale factor should preferably be equal to point size + {1,2,..} to get nice results.");
    m_pbConvert->setToolTip("<b>Convert.</b> Starts the procedure of image conversion.");
    m_pbSave->setToolTip("<b>Save.</b> Saves the output image in png format.");


    m_pbToggleControls->setFixedSize(2*buttonControls,buttonControls);
    m_pbQuit->setFixedSize(buttonSmall, buttonSmall);
    m_pbChooseImage->setFixedWidth(buttonFixedWidth);
    m_pbChooseText->setFixedWidth(buttonFixedWidth);
    m_pbChooseFont->setFixedWidth(buttonFixedWidth);    
    m_pbConvert->setFixedWidth(buttonFixedWidth);
    m_pbSave->setFixedWidth(buttonFixedWidth);

    setupValidators();
    makeConnections();
    disableControls();
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

    m_leScaleValidator = new QIntValidator(Scale::MIN, Scale::MAX, this);
    m_leThresholdValidator = new QRegExpValidator(QRegExp("[0-9]{1,3},[0-9]{1,3},[0-9]{1,3}"));

    m_leScale->setValidator(m_leScaleValidator);
    m_leThreshold->setValidator(m_leThresholdValidator);
}

void GUI::makeConnections()
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

void GUI::defaults()
{
    LoadImage("D:/05 grechka.jpg");
    LoadText("D:/pxc.txt");
    SetFont(QFont("Times New Roman", 5));
    SetScale("6");
    SetThreshold("0,0,0");
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
}

void GUI::updateUIElement(ControlElement &element)
{
    // This switch statement is used for moving the user step by step across all
    // the stages of process. When everything is set, he may change settings.

    // TO DO: only if first time
    if (!stepBYstep)
        return;

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
            stepBYstep = false;
            break;

        case ControlElement::ButtonSave:
            break;
    }
}

void GUI::OnChooseImage()
{
    // First, we find input image on drive using QFileDialog static method.
    // If path is empty, meaning we canceled, do nothing.
    QString path = QFileDialog::getOpenFileName(this, "Choose Input Image", "D:/", "*.jpg *.png");    
    if (path.isEmpty())
        return;

    LoadImage(path);
}

void GUI::LoadImage(const QString &path)
{
    QImage image(path);
    if (image.width() < maxWidth && image.height() < maxHeight)
    {
        // Send signal over airmail, someone will find it sometimes.
        emit ImageChanged(path);
        emit UpdateStatus(QString("New input image has been set: %1").arg(path));

        m_ipwInput->updateImage(image);

        // Hide other UI elements to make user do it step by step.
        m_currentUIElement = ControlElement::ChooseImage;
        updateUIElement(m_currentUIElement);
    }
    else
        emit UpdateStatus(QString("Please, choose image with width less than %1 and height less than %2.").arg(maxWidth).arg(maxHeight));
}

void GUI::OnChooseText()
{
    // Find file with text we want to cipher.
    // If path is empty string, do nothing.
    QString path = QFileDialog::getOpenFileName(0, "Choose Text File to Cipher", "D:/", "*.txt");
    if (path.isEmpty())
        return;

    LoadText(path);
}

void GUI::LoadText(const QString &path)
{
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

    SetFont(font);
}

void GUI::SetFont(const QFont &font)
{
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
    QString scale = static_cast<QLineEdit*>(sender())->text();

    SetScale(scale);
}

void GUI::SetScale(const QString &scale)
{
    // Here again. Meet the StAtUsEs!
    emit UpdateStatus(QString("Pixel scale parameter has been set to: %1").arg(scale));
    m_leScale->setText(scale);

    // Scale is emitting. Savvy?
    emit ScaleChanged(scale);

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

    SetThreshold(unconstrained);
}

void GUI::SetThreshold(const QString &threshold)
{
    QString constrained = constraintThreshold(threshold);

    // Return resulting string of constrained values back to lineedit. And update statuses, of course.
    emit UpdateStatus(QString("RGB Threshold values have been set to: %1").arg(threshold));
    m_leThreshold->setText(threshold);

    // Something in the air. Do you hear?
    emit ThresholdChanged(threshold);

    // Here we go again.
    m_currentUIElement = ControlElement::ChooseThreshold;
    updateUIElement(m_currentUIElement);
}

void GUI::OnImageConverted(const QImage &image)
{
    // When this slot receives image, we update status and set preview item.
    emit UpdateStatus("Received Output Image. Trying to show its preview.");
    m_ipwOutput->updateImage(image);

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
