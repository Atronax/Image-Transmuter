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
#include "historylabel.h"

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
    void LoadImage(const QString& path);
    void LoadText(const QString& path);
    void SetFont (const QFont& font);
    void SetScale (const QString& scale);
    void SetThreshold (const QString& threshold);

    ControlElement m_currentUIElement;

    void makeGUI();
    void setupLayout();
    void setupValidators();
    void makeConnections();
    void disableControls();
    void defaults();
    void setRoundedCorners(int radius_tl, int radius_tr, int radius_bl, int radius_br);

    void updateUI(bool showControls);
    void updateUIElement(ControlElement& element);
    QString constraintThreshold(const QString& input);

    // Status Bar. Here we will see all the current processes.
    HistoryLabel *m_hlStatus;

    // Preview Widgets. Show previews of input and output images.
    ImagePreviewWidget *m_ipwInput;
    ImagePreviewWidget *m_ipwOutput;    

    // Toggle Button to unveil/hide control block.
    QPushButton *m_pbToggleControls;

    // Constraint values for input image
    const int maxWidth = 2048;
    const int maxHeight = 2048;

    // Constraint values for gui elements.
    const int buttonSmall = 24;
    const int buttonControls = 40;
    const int buttonFixedWidth = 140;
    const int textboxFixedHeight = 120;
    bool stepBYstep = true;

    // Buttons for choosing input image, text and font and their statuses.
    // Buttons for starting conversion and saving the resulting image.
    // Editors for scale and threshold with regexp validator and status.
    QPushButton *m_pbQuit;
    QPushButton *m_pbChooseImage;
    QPushButton *m_pbChooseText;
    QPushButton *m_pbChooseFont;
    QPushButton *m_pbConvert;
    QPushButton *m_pbSave;   

    QLineEdit *m_leScale;
    QLineEdit *m_leThreshold;
    QIntValidator *m_leScaleValidator;
    QRegExpValidator *m_leThresholdValidator;

    // Text edit, where user can see the loaded text data.
    QTextEdit *m_teText;

    // Layouts.
    QGridLayout *m_mainLayout;
    QGridLayout *m_controlsLayout;

signals:
    void ImageChanged(const QString& filename);
    void TextChanged(const QString& text);
    void FontChanged(const QFont& font);
    void ScaleChanged(const QString& scale);
    void ThresholdChanged(const QString& threshold);
    void ClickedConvert();
    void ClickedSave(const QString& path);
    void UpdateStatus(const QString& status);
    void UpdateUI(const QString& size);

public slots:
    void OnToggleControls();
    void OnQuit();
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
