#include "imageconversionthread.h"

#include <QApplication>
#include <QPainter>
#include <QDebug>

ImageConversionThread::ImageConversionThread(const QImage& reference, float from, float to,
                                             Scale& scale, Threshold& threshold, Font& font, QString& text)
    : m_from(from), m_to(to),
      m_scale(scale), m_threshold(threshold), m_font(font), m_text(text)
{
    setImageFragment(reference, from, to);
}

ImageConversionThread::~ImageConversionThread()
{
}

void ImageConversionThread::setImageFragment(const QImage& reference, float from, float to)
{
    // Reference: base image we use to separate into jobs for threads.
    // From: percentage of image we're beginning our processing at.
    // To  : percentage of image we're stopping  out processing at.
    // For example 0-25, 25-50, 50-75, 75-100 of height.

    // If to = 0.5, from = 0.25, then to - from = 0.5 - 0.25 = 0.25,
    // the original fragment will gave height of 25% of reference.height().

    originalFragment = QImage(reference.width(), (to - from)*reference.height(), QImage::Format_RGB888);


    for (int h = from*reference.height(), ho = 0; h < to*reference.height(), ho < originalFragment.height(); ++h, ++ho)
    {
        for (int w = 0; w < reference.width(); ++w)
        {
            QRgb pixel = reference.pixel(w, h);
            originalFragment.setPixel(w, ho, pixel);
        }
    }

    resultingFragment = QImage(originalFragment);
}

void ImageConversionThread::processImageFragment()
{
    qDebug() << "In ImageConversionThread::processImageFragment";

    qDebug() << QString("Original  Fragment has these parameters. W: %1. H: %2.").arg( originalFragment.width()).arg( originalFragment.height());
    qDebug() << QString("Resulting Fragment has these parameters. W: %1. H: %2.").arg(resultingFragment.width()).arg(resultingFragment.height());

    QPainter painter;
    painter.begin(&resultingFragment);
        painter.setFont(m_font.font);
        painter.setRenderHint(QPainter::Antialiasing);

        int charIdx = 0;
        QColor curColor;

        for (int h = 0; h < originalFragment.height(); ++h)
        {
            for (int w = 0; w < originalFragment.width(); ++w)
            {
                curColor = QColor::fromRgb(originalFragment.pixel(w, h));

                if (curColor.red() > m_threshold.tR && curColor.green() > m_threshold.tG && curColor.blue() > m_threshold.tB)
                {
                    painter.setPen(QColor::fromRgb(originalFragment.pixel(w, h)));

                    if (!m_text.isEmpty())
                        painter.drawText(w*m_scale.value, h*m_scale.value, m_text.at(charIdx++));
                    else
                        painter.drawText(w*m_scale.value, h*m_scale.value, QString(0x2309)); // or QString::number(rand()%2) etc.
                }
                else
                    resultingFragment.setPixelColor(w, h, curColor);

                // qApp->processEvents();

            }
        }

        painter.end();

        bDone = true;
}

bool ImageConversionThread::isDone()
{
    return bDone;
}

float ImageConversionThread::From()
{
    return m_from;
}

float ImageConversionThread::To()
{
    return m_to;
}

const QImage &ImageConversionThread::result()
{
    return resultingFragment;
}

void ImageConversionThread::run()
{
    processImageFragment();
}
