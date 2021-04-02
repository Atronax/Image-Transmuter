#ifndef LOGIC_H
#define LOGIC_H

#include <QObject>

#include <QImage>
#include <QFont>
#include <QList>


#include "Helpers/imageconversionthread.h"
#include "types.h"

class Logic : public QObject
{
    Q_OBJECT
public:
    explicit Logic(QObject *parent = nullptr);
    ~Logic();

private:
    // Where various pieces of old code float...
    void Void();

    // Objects to store input image and output image.
    // We find the input file using QFileDialog and load image using one of its constructor.
    // Then set up different values, that are used to convert it and save the result as an output image.
    void ConvertImage();
    QImage m_imageInput;
    QImage m_imageOutput;

    // Settings, that are used to convert input image to output.
    // If we use text to cipher onto the image, m_text should not be empty. Otherwise, defaults will be used.
    // We also can choose font parameters using QFontDialog static method to use on symbols.
    // Scale makes every pixel larger by that value, so we can put larger letters in their positions.
    // Threshold is used to show, which pixels should be changed, and which are not.
    // Currently we stop on RGB values, but HSV could me more interesting.
    void SetFont (const QFont& font);
    void SetScale (uint s);
    void SetThresholds (int tR, int tG, int tB);
    bool AllDataIsSet();

    QString m_text;
    Font m_font;
    Scale m_scale;
    Threshold m_threshold;

    // Lets try use threads to parallelize the conversion job.
    // First, we must create several workers. For this we can instantiate objects
    // of type ImageConversionThread and pass them all the needed data to finish the job.
    // Next we traverse the list and start threads using run() method. When all threads
    // finish their job, we should store the results and gather them to connect into a
    // single image again. QPainter will do that task for us.
    void SetupConversionThreads();
    void StartConversionThreads();
    void InitializeImage(QImage& image);
    void GatherConversionResults(QImage& image);
    void ClearConversionThreads();

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
