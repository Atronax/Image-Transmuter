#include "imageconversionthread.h"

#include <QApplication>
#include <QPainter>
#include <QDebug>

#include "imagepreviewwidget.h"

ImageConversionThread::ImageConversionThread(const QImage& reference, float from, float to,
                                             Scale& scale, Threshold& threshold, Font& font, QString& text)
    : m_from(from), m_to(to),
      m_scale(scale), m_threshold(threshold), m_font(font),
      bDone(false)
{
    setImageFragment(reference, m_from, m_to);
    setTextFragment(text, m_from, m_to);
}

ImageConversionThread::~ImageConversionThread()
{
}

void ImageConversionThread::setImageFragment(const QImage& reference, float from, float to)
{
    // TO DO:
    // Когда изображение делим на 4 куска для потоков, они теряют полосы на границах.
    // 1-2, 2-3, 3-4. Нужно посмотреть, как исправить это. Завтра.

    // Reference: base image we use to separate into jobs for threads.
    // From: percentage of image we're beginning our processing at.
    // To  : percentage of image we're stopping  out processing at.
    // For example 0-25, 25-50, 50-75, 75-100 of height.

    // If to = 0.5, from = 0.25, then to - from = 0.5 - 0.25 = 0.25,
    // the original fragment will gave height of 25% of reference.height().

    int hFrom = from*reference.height();
    int hTo   = to*reference.height();

    if (hFrom != 0) hFrom += m_scale.value;
    int height = hTo - hFrom;

    qDebug() << QString("hFrom = %1. hTo = %2. fragment width = %3. fragment height = %4.").arg(hFrom).arg(hTo).arg(reference.width()).arg(height);


    originalFragment = QImage(reference.width(), height, QImage::Format_RGB888);

    QPainter painter;
    painter.begin(&originalFragment);
        qDebug() << QRect(QPoint(0, hFrom), QPoint(reference.width(), hTo));
        painter.drawImage(originalFragment.rect(), reference, QRect(QPoint(0, hFrom), QPoint(reference.width(), hTo)));
    painter.end();

    /*
    int idx = 0, curw = 0, curh = 0;
    for (int h = hFrom; h < hTo; ++h)
    {
        for (int w = 0; w < reference.width(); ++w)
        {
            QRgb pixel = reference.pixel(w, h);

            curw = idx % w;
            curh = idx / w;
            qDebug() << QString("Current W: %1. Current H: %2.").arg(curw).arg(curh);

            // originalFragment.setPixel(curw, curh, pixel);

            ++idx;
        }
    }
    */

    // Lets see what we have in our fragment image:
    // showImageFragment(originalFragment);

    resultingFragment = QImage(originalFragment.width()*m_scale.value, originalFragment.height()*m_scale.value, QImage::Format_RGB888);
    qDebug() << QString("Resulting fragment has values: W%1, H%2").arg(resultingFragment.width()).arg(resultingFragment.height());
}

void ImageConversionThread::setTextFragment(const QString &text, float from, float to)
{
    int position  = from * text.size();
    int charCount = originalFragment.width() * originalFragment.height();

    m_text = text.mid(position, charCount);
}

void ImageConversionThread::processImageFragment()
{
    // qDebug() << "In ImageConversionThread::processImageFragment";

    // qDebug() << QString("Original  Fragment has these parameters. W: %1. H: %2.").arg( originalFragment.width()).arg( originalFragment.height());
    // qDebug() << QString("Resulting Fragment has these parameters. W: %1. H: %2.").arg(resultingFragment.width()).arg(resultingFragment.height());

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
                        painter.drawText(w*m_scale.value, h*m_scale.value, QString::number(rand()%2));
                }
                else
                    resultingFragment.setPixelColor(w*m_scale.value, h*m_scale.value, curColor);

                qApp->processEvents();

            }
        }

    painter.end();

    // showImageFragment(resultingFragment);
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

void ImageConversionThread::showImageFragment(const QImage &fragment)
{
    qDebug() << fragment.width() << fragment.height();

    ImagePreviewWidget *wgt = new ImagePreviewWidget(nullptr, false);
    wgt->updateImage(fragment);
    wgt->setFixedSize(fragment.size());
    wgt->show();
}
