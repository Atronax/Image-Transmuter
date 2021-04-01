#ifndef IMAGECONVERSIONTHREAD_H
#define IMAGECONVERSIONTHREAD_H

#include <QThread>
#include <QImage>
#include <QString>

#include "types.h"

class ImageConversionThread : public QThread
{
    Q_OBJECT;

public:
    ImageConversionThread(const QImage& reference,
                          float from, float to,
                          Scale& scale, Threshold& threshold,
                          Font& font, QString& text);
    ~ImageConversionThread();

    void setImageFragment (const QImage& reference, float from, float to);
    void processImageFragment ();

    bool isDone();
    const QImage &result();
    float From();
    float To();

    void run();

private:
    // Range of original image height, that is used to crop the fragment.
    float m_from;
    float m_to;

    // Fragments: unconverted and converted.
    QImage originalFragment;
    QImage resultingFragment;

    // Parameters of conversion.
    Scale& m_scale;
    Threshold& m_threshold;
    Font& m_font;
    QString& m_text;

    // Done flag.
    bool bDone;

signals:
    void UpdateImage (const QImage& image);

};

#endif // IMAGECONVERSIONTHREAD_H
