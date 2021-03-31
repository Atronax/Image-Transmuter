#ifndef GUI_H
#define GUI_H

#include <QWidget>

#include <QLabel>
#include <QTextEdit>
#include <QLineEdit>
#include <QPushButton>
#include <QPair>
#include <QGridLayout>

#include <QIntValidator>

#include "Helpers/imagepreviewwidget.h"

enum ControlElement
{
    ChooseImage = 0,
    ChooseText = 1,
    ChooseFont = 2,
    ChooseScale = 3,
    ChooseThreshold = 4,
    ButtonConvert = 5,
    ButtonSave = 6
};

class GUI : public QWidget
{
    Q_OBJECT
public:
    explicit GUI(QWidget *parent = nullptr);
    ~GUI();

private:
    ControlElement m_currentUIElement;

    void makeGUI();
    void setupLayout();
    void setupValidators();
    void setupConnections();
    void disableControls();


    void updateUI(bool showControls);
    void tuneOtherUIElements(ControlElement& element);
    QString constraintThreshold(const QString& input);

    // Status Bar. Here we will see all the current processes.
    QLabel *m_lStatus;

    // Preview Widgets. Show previews of input and output images.
    ImagePreviewWidget *m_ipwInput, *m_ipwOutput;

    // Toggle Button to unveil/hide control block.
    QPushButton *m_pbToggleControls;

    // Controls:
    QList<QPair<QString, QWidget*>> m_controls;
    QPair<QString, QWidget*> controlFor (const QString& name, QWidget* widget);



    // Buttons for choosing input image, text and font and their statuses.
    QPushButton *m_pbChooseImage, *m_pbChooseText, *m_pbChooseFont;
    QLabel *m_lChooseImage, *m_lChooseText, *m_lChooseFont;

    // Editors for scale and threshold with regexp validator and status.
    QIntValidator *m_leScaleValidator;
    QRegExpValidator *m_leThresholdValidator;
    QLineEdit *m_leScale, *m_leThreshold;
    QLabel *m_lScale, *m_lThreshold;

    // Buttons to start conversion and save the resulting image;
    QPushButton *m_pbConvert, *m_pbSave;

    // Text edit, where user can see the loaded text data.
    QTextEdit *m_teText;

    // Layouts.
    QGridLayout *m_mainLayout, *m_ctrlLayout, *m_blankLayout;
    QWidget *m_dummyWidget;

signals:
    void ImageChanged(const QString& filename);
    void TextChanged(const QString& text);
    void FontChanged(const QFont& font);
    void ScaleChanged(const QString& scale);
    void ThresholdChanged(const QString& threshold);
    void ClickedConvert();
    void ClickedSave(const QString& path);

public slots:
    void OnToggleControls();
    void OnChooseImage();
    void OnChooseText();
    void OnChooseFont();
    void OnScaleChanged();
    void OnThresholdChanged();
    void OnUpdateStatus(const QString& status);
    void OnImageConverted(const QImage& image);
    void OnTextChanged();
    void OnConvertButton();
    void OnSaveButton();
};

#endif // GUI_H
