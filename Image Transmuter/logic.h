#ifndef LOGIC_H
#define LOGIC_H

#include <QObject>

#include <QImage>
#include <QFont>
#include <QList>

#include "types.h"
#include "Helpers/imageconversionthread.h"

class Logic : public QObject
{
    Q_OBJECT
public:
    explicit Logic(QObject *parent = nullptr);
    ~Logic();

    void SetScale (uint s);
    void SetQuality (int q);
    void SetThresholds (int tR, int tG, int tB);

    void ConvertImage();

private:
    bool CheckParameters();
    void SetupOutputImage();

    // Objects to store input image and output image.
    // We find the input file using QFileDialog and load image using one of its constructor.
    // Then set up different values, that are used to convert it and save the result as an output image.

    QImage m_imageInput;
    QImage m_imageOutput;

    // If we use text to cipher onto the image, m_text should not be empty.
    // String holds the text to cipher. It should have size(textToCipher) >= size(imageInput).

    QString m_text;

    // Settings, that are used to convert input image to output.
    // Quality should be in range 0 to 100. -1 is default value. It represents the quality of output image.
    // Threshold is used to show, which pixels should be changed, and which are not.
    // Currently we stop on RGB values, but HSV could me more interesting.
    // Scale makes every pixel larger by that value, so we can put larger letters in their positions.
    // We also can choose font to use on symbols instead of the image pixels.

    int m_quality = -1;
    Threshold m_threshold;
    Scale m_scale;
    Font m_font;

    // Lets try use threads to parallelize the conversion job.
    // First, we must create several workers. For this we can instantiate objects
    // of type ImageConversionThread and pass them all the needed data to finish the job.
    // Next we traverse the list and start threads using run() method.
    // After that we gather the results and connect the fragments them into single image;
    void setupConversionThreads();
    void startConversionThreads();
    QImage gatherConversionResults();
    void clearConversionThreads();

    QList<ImageConversionThread*> threads;

signals:
    void ImageConverted(const QImage& image);
    void UpdateStatus(const QString& status);

public slots:
    void OnImageChanged(const QString& filename);
    void OnTextChanged(const QString& text);
    void OnFontChanged(const QFont& font);
    void OnScaleChanged(const QString& scale);
    void OnThresholdChanged(const QString& threshold);
    void OnClickedConvert();
    void OnClickedSave(const QString& filename);


};

#endif // LOGIC_H
