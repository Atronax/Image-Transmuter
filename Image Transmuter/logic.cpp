#include "logic.h"

#include <QApplication>
#include <QFileDialog>
#include <QFontDialog>
#include <QTextStream>

#include <QPainter>
#include <QDebug>

#include "Helpers/imagepreviewwidget.h"

Logic::Logic(QObject *parent) : QObject(parent)
{

}

Logic::~Logic()
{
    clearConversionThreads();
}

void Logic::SetScale(uint s)
{
    if (s >= m_scale.MIN && s <= m_scale.MAX)
        m_scale.value = s;
    else
        qDebug() << "Scale value should be between 1 and 10";
}

void Logic::SetQuality(int q)
{
    if (q == -1 || (q > 0 && q < 100))
        m_quality = q;
}

void Logic::ConvertImage()
{
    emit UpdateStatus("Processing started! Please wait...");

    // We start with changing of gui element message to let user know of current procedure.
    // If we are drawing using text, we should choose the corresponding file on our drive.
    // Then m

    setupConversionThreads();
    startConversionThreads();
    m_imageOutput = gatherConversionResults();

    qDebug() << QString("Resulting image has these parameters. W: %1. H: %2.").arg(m_imageOutput.width())
                                                                              .arg(m_imageOutput.height());
    emit ImageConverted(m_imageOutput);

    /*
    SetupOutputImage();

    QPainter painter;
    painter.begin(&m_imageOutput);
        painter.setFont(m_font.font);
        painter.setRenderHint(QPainter::Antialiasing);

        int charIdx = 0;
        QColor curColor;

        for (int h = 0; h < m_imageInput.height(); ++h)
        {
            for (int w = 0; w < m_imageInput.width(); ++w)
            {
                curColor = QColor::fromRgb(m_imageInput.pixel(w, h));

                if (curColor.red() > m_threshold.tR && curColor.green() > m_threshold.tG && curColor.blue() > m_threshold.tB)
                {
                    painter.setPen(QColor::fromRgb(m_imageInput.pixel(w, h)));

                    if (!m_text.isEmpty())
                        painter.drawText(w*m_scale.value, h*m_scale.value, m_text.at(charIdx++));
                    else
                        painter.drawText(w*m_scale.value, h*m_scale.value, QString(0x2309)); // or QString::number(rand()%2) etc.
                }
                else
                    m_imageOutput.setPixelColor(w, h, curColor);

                qApp->processEvents();

            }

            emit UpdateStatus(QString("Left: %1 percent").arg(100 - 100.0 * h / m_imageInput.height()));
        }

    painter.end();

    emit ImageConverted(m_imageOutput);
    */
}

void Logic::SetupOutputImage()
{
    int width = m_imageInput.width() * m_scale.value;
    int height = m_imageInput.height() * m_scale.value;
    QImage::Format format = QImage::Format_RGB888;

    m_imageOutput = QImage (width, height, format);
}

void Logic::setupConversionThreads()
{
    // Lets say there are four threads.

    int threadsCount = 4;

    for (int i = 0; i < threadsCount; ++i)
    {
        float from = i / (float)threadsCount;
        float to = (i < threadsCount) ? (i + 1) / (float) threadsCount : (float) threadsCount;

        qDebug() << QString("For image part %1: from = %2, to = %3").arg(i).arg(from).arg(to);

        ImageConversionThread* thread = new ImageConversionThread(m_imageInput, from, to, m_scale, m_threshold, m_font, m_text);
        threads.insert(i, thread);
    }
}

void Logic::startConversionThreads()
{
    foreach (ImageConversionThread* thread, threads)
    {
        thread->run();
        thread->wait();
    }
}

QImage Logic::gatherConversionResults()
{
    QImage result = QImage(m_imageOutput.width(), m_imageOutput.height(), QImage::Format_RGB888);

    qDebug() << QString("The result image has these sizes: W=%1, H=%2").arg(result.width()).arg(result.height());

    // Wait until all threads will do their jobs.
    bool threadsDoneJobs = false;
    while (!threadsDoneJobs)
    {
        threadsDoneJobs = true;

        for (int i = 0; i < threads.count(); ++i)
        {
            if (!threads.at(i)->isDone())
            {
                threadsDoneJobs = false;
                break;
            }
        }
    }
    qDebug() << QString("Thread have done their jobs: %1").arg(threadsDoneJobs ? "true" : "false");



    foreach (ImageConversionThread* thread, threads)
    {
        float from = thread->From();
        float to = thread->To();

        QImage fragment = thread->result();

        qDebug() << QString("Current thread returned fragment of image with these parameters. From: %1. To: %2. W: %3. H: %4").arg(from).arg(to).arg(fragment.width()).arg(fragment.height());
            ImagePreviewWidget *wgt = new ImagePreviewWidget(nullptr, false);
            wgt->setImage(fragment);
            wgt->show();
        qDebug() << QString("Current Thread Data. From: %1. To: %2. Image width: %3. Image height: %4.").arg(from).arg(to).arg(fragment.width()).arg(fragment.height());

        int hFrom = from*result.height();
        int hTo = to*result.height();
        // int currentHeight = hFrom;

        qDebug() << QString("From Height: %1. To Height: %2.").arg(hFrom).arg(hTo);
        qDebug() << QString("0, %1, %2, %3").arg(hFrom).arg(fragment.width()).arg((to-from)*result.height());

        QPainter painter(&result);
            painter.drawImage(QRect(0, hFrom, fragment.width(), ceil((to-from)*result.height())), fragment);
        painter.end();


        /*
        for (int h = hFrom; h < hTo; ++h)
        {
            for (int w = 0; w < result.width(); ++w)
            {
                QRgb pixel = fragment.pixel(w, h);
                result.setPixel(w, h, pixel);
            }
        }
        */
    }

    return result;
}

void Logic::clearConversionThreads()
{
    foreach (ImageConversionThread* thread, threads)
    {
        thread->quit();

        delete thread;
        thread = nullptr;
    }

    threads.clear();
}

// put -1 as value, if you want no changes
void Logic::SetThresholds(int tR, int tG, int tB)
{
    if (tR >= 0)
        m_threshold.tR = tR;

    if (tG >= 0)
        m_threshold.tG = tG;

    if (tB >= 0)
        m_threshold.tB = tB;
}

void Logic::OnImageChanged(const QString &filename)
{
    // We should not only instantiate new QImage object using filename parameter,
    // But also setup output image, tuning its size and format variables.

    emit UpdateStatus ("Input Image has Changed.");

    m_imageInput = QImage(filename);
    SetupOutputImage();

    QString isNull = (m_imageInput.isNull()) ? "true" : "false";
    qDebug() << QString("Image Input is empty: %1").arg(isNull);
}

void Logic::OnTextChanged(const QString &text)
{
    // When text arrives to logic instance, we should check if its parameters have values we need.
    // If its size is less than size of image, we need repeat the text or set some other.

    emit UpdateStatus("Text has changed.");
    if (m_imageInput.isNull())
        return;

    int sizeText = text.size();
    int sizeImage = m_imageInput.width() * m_imageInput.height();

    // Here we just repeating the text until its size will be greater or equal to pixel count.
    m_text = text;
    while (sizeText < sizeImage)
    {
        m_text += text + " ";
        sizeText = m_text.size();
    }
}

void Logic::OnFontChanged(const QFont &font)
{
    emit UpdateStatus("Font has changed.");

    m_font.font = font;
}

void Logic::OnScaleChanged(const QString &scale)
{
    emit UpdateStatus("Scale has changed.");

    int s = scale.toInt();
    SetScale(s);

    emit UpdateStatus(QString("Scale set to: %1.").arg(s));
}

void Logic::OnThresholdChanged(const QString &threshold)
{
    emit UpdateStatus("Threshold has changed.");

    QStringList terms = threshold.split(",");

    int tR = terms.at(0).toInt();
    int tG = terms.at(1).toInt();
    int tB = terms.at(2).toInt();

    SetThresholds(tR, tG, tB);

    emit UpdateStatus(QString("Thresholds set to: R-%1, G-%2, B-%3.").arg(tR).arg(tG).arg(tB));
}

void Logic::OnClickedConvert()
{
    emit UpdateStatus("Checking Parameters");

    if (CheckParameters())
    {
        emit UpdateStatus("All needed parameters are set. Beginning the process of image conversion.");

        ConvertImage();

        emit UpdateStatus("Image conversion has passed successfuly.");
    }
    else
        emit UpdateStatus("Not all needed parameters are set. Please check what is left.");
}

bool Logic::CheckParameters()
{
    if (!m_text.isEmpty() && !m_imageInput.isNull() && m_font.isSet() && m_scale.isSet() && m_threshold.isSet())
        return true;
    else
        return false;
}

void Logic::OnClickedSave(const QString& filename)
{
    emit UpdateStatus("Wait. Output image is saving now.");

    if (!m_imageOutput.isNull())
        m_imageOutput.save(filename, "png", 50);
    else
        emit UpdateStatus("There is no Output Image!");

    emit UpdateStatus("Saving is over. Thank you for using our application.");
}
