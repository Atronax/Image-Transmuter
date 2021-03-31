#include "gui.h"

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
    m_lStatus->deleteLater();
    m_ipwInput->deleteLater();
    m_ipwOutput->deleteLater();

    m_pbToggleControls->deleteLater();

    m_pbChooseImage->deleteLater();
    m_lChooseImage->deleteLater();

    m_pbChooseText->deleteLater();
    m_lChooseText->deleteLater();

    m_pbChooseFont->deleteLater();
    m_lChooseFont->deleteLater();

    m_leScaleValidator->deleteLater();
    m_leThresholdValidator->deleteLater();

    m_leScale->deleteLater();
    m_leThreshold->deleteLater();

    m_pbConvert->deleteLater();
    m_pbSave->deleteLater();

    m_ctrlLayout->deleteLater();
    m_mainLayout->deleteLater();

    m_dummyWidget->deleteLater();
    m_blankLayout->deleteLater();
}

void GUI::makeGUI()
{
    setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);

    m_lStatus = new QLabel("Status", this);

    m_ipwInput = new ImagePreviewWidget(this);
    m_ipwOutput = new ImagePreviewWidget(this);

    m_pbToggleControls = new QPushButton("ON", this);
    m_pbToggleControls->setFixedWidth(64);
    m_pbToggleControls->setCheckable(true);
    m_pbToggleControls->setChecked(false);

    m_teText = new QTextEdit(this);
    m_teText->setFixedHeight(150);

    m_pbChooseImage = new QPushButton("Choose Image", this);
    m_lChooseImage = new QLabel("Image File: ", this);
    // m_lChooseImage->setFixedWidth(m_mainLayout->geometry().width() / 2.0f);
    m_lChooseImage->setWordWrap(true);

    m_pbChooseText = new QPushButton("Choose Text", this);
    m_lChooseText = new QLabel("Text File: ", this);
    m_lChooseText->setWordWrap(true);

    m_pbChooseFont = new QPushButton("Choose Font", this);
    m_lChooseFont = new QLabel("Font: ", this);
    m_lChooseFont->setWordWrap(true);

    m_leScale = new QLineEdit ("Put scale here (1-13).", this);
    m_lScale = new QLabel ("Scale: ", this);

    m_leThreshold = new QLineEdit ("Put threshold for rgb components here (0-255,0-255,0-255)", this);
    m_lThreshold = new QLabel ("Threshold: ", this);

    m_pbConvert = new QPushButton ("Convert", this);

    m_pbSave = new QPushButton ("Save", this);

    disableControls();
    setupValidators();
    setupConnections();
    setupLayout();
}

void GUI::setupLayout()
{
    m_mainLayout = new QGridLayout(this);

    m_mainLayout->addWidget(m_lStatus, 0, 0, 1, 2);
    m_mainLayout->addWidget(m_ipwInput, 1, 0, 5, 1);
    m_mainLayout->addWidget(m_ipwOutput, 1, 1, 5, 1);
    m_mainLayout->addWidget(m_pbToggleControls, 7, 0, 1, 2, Qt::AlignCenter);

    m_ctrlLayout = new QGridLayout(this);
    m_ctrlLayout->setSpacing(1);
    m_ctrlLayout->setColumnStretch(0, 1);
    m_ctrlLayout->setColumnStretch(1, 1);
    m_ctrlLayout->setMargin(10);

    m_ctrlLayout->addWidget(m_teText, 0, 0, 2, 2);

    m_ctrlLayout->addWidget(m_pbChooseImage, 2, 0, 1, 1);
    m_ctrlLayout->addWidget(m_lChooseImage, 2, 1, 1, 1);

    m_ctrlLayout->addWidget(m_pbChooseText, 3, 0, 1, 1);
    m_ctrlLayout->addWidget(m_lChooseText, 3, 1, 1, 1);

    m_ctrlLayout->addWidget(m_pbChooseFont, 4, 0, 1, 1);
    m_ctrlLayout->addWidget(m_lChooseFont, 4, 1, 1, 1);

    m_ctrlLayout->addWidget(m_leScale, 5, 0, 1, 1);
    m_ctrlLayout->addWidget(m_lScale, 5, 1, 1, 1);

    m_ctrlLayout->addWidget(m_leThreshold, 6, 0, 1, 1);
    m_ctrlLayout->addWidget(m_lThreshold, 6, 1, 1, 1);

    m_ctrlLayout->addWidget(m_pbConvert, 7, 0, 1, 1);
    m_ctrlLayout->addWidget(m_pbSave, 7, 1, 1, 1);

    m_mainLayout->addLayout(m_ctrlLayout, 8, 0, 7, 2);

    m_dummyWidget = new QWidget(this);
    m_dummyWidget->setFixedHeight(1);

    m_blankLayout = new QGridLayout(this);
    m_blankLayout->addWidget(m_dummyWidget, 8, 0, 1, 2);

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
    connect(m_pbToggleControls, SIGNAL(clicked()), this, SLOT(OnToggleControls()));
    connect(m_pbChooseImage, SIGNAL(clicked()), this, SLOT(OnChooseImage()));
    connect(m_pbChooseText, SIGNAL(clicked()), this, SLOT(OnChooseText()));
    connect(m_pbChooseFont, SIGNAL(clicked()), this, SLOT(OnChooseFont()));
    connect(m_leScale, SIGNAL(returnPressed()), this, SLOT(OnScaleChanged()));
    connect(m_leThreshold, SIGNAL(returnPressed()), this, SLOT(OnThresholdChanged()));
    connect(m_pbConvert, SIGNAL(clicked()), this, SLOT(OnConvertButton()));
    connect(m_pbSave, SIGNAL(clicked()), this, SLOT(OnSaveButton()));
    connect(m_teText, SIGNAL(textChanged()), this, SLOT(OnTextChanged()));
}

void GUI::disableControls()
{
    m_pbChooseText->setEnabled(false);
    m_pbChooseFont->setEnabled(false);
    m_leScale->setEnabled(false);
    m_leThreshold->setEnabled(false);
    m_pbConvert->setEnabled(false);
    m_pbConvert->setEnabled(false);
}

void GUI::updateUI(bool showControls)
{
    qDebug() << QString("There are %1 items in the controls list").arg(m_ctrlLayout->count());

    if (showControls)
    {
        m_mainLayout->removeItem(m_blankLayout);
        m_mainLayout->addLayout(m_ctrlLayout, 8, 0, 7, 2);

        for (int idx = 0; idx < m_ctrlLayout->count(); ++idx)
        {
            QLayoutItem* item = m_ctrlLayout->itemAt(idx);
            item->widget()->show();
        }
    }
    else
    {
        m_mainLayout->removeItem(m_ctrlLayout);
        m_mainLayout->addLayout(m_blankLayout, 8, 0, 1, 1);

        for (int idx = 0; idx < m_ctrlLayout->count(); ++idx)
        {
            QLayoutItem* item = m_ctrlLayout->itemAt(idx);
            item->widget()->hide();
        }
    }

}

void GUI::tuneOtherUIElements(ControlElement &element)
{
    switch (element)
    {
        case ControlElement::ChooseImage:
            m_pbSave->setEnabled(false);
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
            m_leThreshold->setEnabled(false);
            m_pbConvert->setEnabled(true);
            break;

        case ControlElement::ButtonConvert:
            m_pbConvert->setEnabled(true);
            m_pbChooseImage->setEnabled(true);
            m_pbChooseText->setEnabled(true);
            m_pbChooseFont->setEnabled(true);
            m_leScale->setEnabled(true);
            m_leThreshold->setEnabled(true);
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

    // Update statuses labels and set preview image.
    m_lChooseImage->setText(QString("Image File: %1").arg(path));
    m_lStatus->setText(QString("New input image has been set: %1").arg(path));
    m_ipwInput->setImage(QImage(path));

    // Hide other UI elements to make user do it step by step.
    m_currentUIElement = ControlElement::ChooseImage;
    tuneOtherUIElements(m_currentUIElement);
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
            m_lStatus->setText(QString("New text has been set: %1").arg(path));
            m_lChooseText->setText(QString("Text File: %1").arg(path));

            // Send pigeon with letter tied to leg. Hope it won't be shot.
            emit TextChanged(text);

            // Don't forget to close the file stream, when we don't need it anymore.
            file.close();
        }
    }

    // Hide other UI elements.
    m_currentUIElement = ControlElement::ChooseText;
    tuneOtherUIElements(m_currentUIElement);
}

void GUI::OnChooseFont()
{
    // Here we are calling font dialog box to choose the font. The method getFont returns boolean value,
    // That indicates whether the result value is new font or default.
    bool bOk;
    QFont font = QFontDialog::getFont(&bOk);

    if (bOk)
        m_lStatus->setText("New font has been chosen.");
    else
        m_lStatus->setText("Default font has been chosen.");

    // Here we gather information into a string to pass it to the label,
    // So that we could see the parameters of the chosen font.
    QString fontItalic = (font.italic()) ? "true" : "false";
    QString fontBold = (font.bold()) ? "true" : "false";
    QString fontParametersText = QString("Family: %1. Size: %2. Italic: %3. Bold: %4 ").arg(font.family()).arg(font.pointSize())
                                                                                       .arg(fontItalic).arg(fontBold);

    // What would we do without statuses update?
    m_lChooseFont->setText(fontParametersText);
    m_lStatus->setText(QString("Workflow font has been set to: %1").arg(fontParametersText));

    // Also we emit the font, so other class could use it later.
    // Kinda lot of emitting, strange...
    emit(FontChanged(font));

    // This extremely hard step again.
    m_currentUIElement = ControlElement::ChooseFont;
    tuneOtherUIElements(m_currentUIElement);
}

void GUI::OnScaleChanged()
{
    // Sender is an QObject base class, so lets cast it using such a handy tool!
    // Then we can take its text and store it in QString variable.
    QString string = static_cast<QLineEdit*>(sender())->text();

    // Here again. Meet the StAtUsEs!
    m_lScale->setText(QString("Scale: %1").arg(string));
    m_lStatus->setText(QString("Pixel scale parameter has been set to: %1").arg(string));

    // Scale is emitting. Savvy?
    emit ScaleChanged(string);

    // Once more.
    m_currentUIElement = ControlElement::ChooseScale;
    tuneOtherUIElements(m_currentUIElement);
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
    m_leThreshold->setText(constrained);
    m_lThreshold->setText(QString("Threshold: %1").arg(constrained));
    m_lStatus->setText(QString("RGB Threshold values have been set to: %1").arg(constrained));

    // Something in the air. Do you hear?
    emit ThresholdChanged(constrained);

    // Here we go again.
    m_currentUIElement = ControlElement::ChooseThreshold;
    tuneOtherUIElements(m_currentUIElement);
}

void GUI::OnImageConverted(const QImage &image)
{
    // When this slot receives image, we update status and set preview item.
    m_lStatus->setText("Received Output Image. Trying to show its preview.");
    m_ipwOutput->setImage(image);

    // Also tune other UI elements
    m_currentUIElement = ControlElement::ButtonConvert;
    tuneOtherUIElements(m_currentUIElement);
}

void GUI::OnUpdateStatus(const QString& status)
{
    // This is the receiver slot for updates, sent by logic class.
    m_lStatus->setText(status);
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

QPair<QString, QWidget *> GUI::controlFor(const QString &name, QWidget *widget)
{
    QPair<QString, QWidget*> pair;

    pair.first = name;
    pair.second = widget;

    return pair;
}

void GUI::OnToggleControls()
{
    qDebug() << "In GUI::OnToggleControls";

    QPushButton* button = static_cast<QPushButton*>(sender());
    if (button->isCheckable())
    {
        if (!button->isChecked())
        {
            qDebug() << "Button is not checked.";

            button->setText("ON");
            updateUI(true);
        }

        if (button->isChecked())
        {
            qDebug() << "Button is checked.";

            button->setText("OFF");
            updateUI(false);
        }
    }
}
