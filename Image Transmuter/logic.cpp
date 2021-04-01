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
    ClearConversionThreads();
}

void Logic::SetScale(uint s)
{
    if (s >= m_scale.MIN && s <= m_scale.MAX)
        m_scale.value = s;
}

void Logic::TuneOutputImage(QImage& image)
{
    int width = m_imageInput.width() * m_scale.value;
    int height = m_imageInput.height() * m_scale.value;
    QImage::Format format = QImage::Format_RGB888;

    image = QImage (width, height, format);
}

void Logic::ConvertImage()
{
    // We start with changing of gui element message to let user know of current procedure.
    // Then we setup threads, that divide the whole job in several parts and start them.
    // When they will finish the task, gather the resulting fragments into single output image.

    emit UpdateStatus("Processing started!");

    SetupConversionThreads();
    StartConversionThreads();
    GatherConversionResults(m_imageOutput);

    // qDebug() << QString("Resulting image has these parameters. W: %1. H: %2.").arg(m_imageOutput.width()).arg(m_imageOutput.height());
    emit ImageConverted(m_imageOutput);    
}

void Logic::SetupConversionThreads()
{
    // Lets say there are four threads. For each of them we should:
    // 1. Calculate from value, that is indicating at what height percentage of input image the thread should begin its job.
    // 2. Calculate to value, that is indicating at what height percentage of input image the thread should stop its job.
    // 3. Instantiate thread and pass all needed data as parameters. Some values are passed as references, so we haven't unneccesary copies.
    // 4. Put it into threads list for later use.

    emit UpdateStatus("Preparing data and creating threads.");

    int threadsCount = 4;

    for (int i = 0; i < threadsCount; ++i)
    {
        qDebug() << QString("Image's height is %1. If divided into %2 parts, we will have %2 fragments with heights of %3.").arg(m_imageInput.height()).arg(threadsCount).arg((float) m_imageInput.height() / threadsCount);

        float from = i / (float)threadsCount;
        float to = (i < threadsCount) ? (i + 1) / (float) threadsCount : (float) threadsCount;

        qDebug() << QString("For image part %1: from = %2, to = %3").arg(i).arg(from).arg(to);

        ImageConversionThread* thread = new ImageConversionThread(m_imageInput, from, to, m_scale, m_threshold, m_font, m_text);
        threads.insert(i, thread);
    }
}

void Logic::StartConversionThreads()
{
    emit UpdateStatus("Threads starting their jobs.");

    foreach (ImageConversionThread* thread, threads)    
        thread->run();
}

void Logic::GatherConversionResults(QImage& image)
{    
    emit UpdateStatus("Waiting until threads do their jobs.");

    // Wait while threads do their jobs. Move out of for loop, if there is at least one thread, that is still doing its task.
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

    // qDebug() << QString("Thread have done their jobs: %1").arg(threadsDoneJobs ? "true" : "false");

    emit UpdateStatus("Gathering updated fragments into single output image.");

    // Traverse across all the threads, fetch updated fragment and draw it using painter on corresponding place of output image.
    foreach (ImageConversionThread* thread, threads)
    {
        QImage fragment = thread->result();        

        int hFrom = thread->From() * image.height();
        int hTo = thread->To() * image.height();

        if (hFrom != 0) ++hFrom;

        qDebug() << QString("Current fragment has sizes {%1, %2} and will be put into height range {%3, %4}.").arg(fragment.width())
                                                                                                              .arg(fragment.height())
                                                                                                              .arg(hFrom).arg(hTo);

        QPainter painter(&image);
            painter.drawImage(0, hFrom, fragment);
        painter.end();
    }
}

void Logic::ClearConversionThreads()
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

void Logic::SetFont(const QFont &font)
{
    m_font.font = font;
}

void Logic::OnImageChanged(const QString &filename)
{
    // When user chooses
    // We should not only instantiate new QImage object using filename parameter,
    // But also setup output image, tuning its size and format variables.

    emit UpdateStatus ("New input image arrived. Setting the parameters of output image.");

    m_imageInput = QImage(filename);
    TuneOutputImage(m_imageOutput);

    emit UpdateStatus ("Operations on output image are done.");
}

void Logic::OnTextChanged(const QString &text)
{
    // When user chooses new text:
    // 1. We store it in local variable.
    // 2. Check if input image is null, and do nothing if it is so.
    // 3. Calculate size of the text and image.
    // If they are not the same, equalize it.
    // 4. Don't forget about birdies and messages.

    emit UpdateStatus("New text arrived. Aligning its size with pixel count of the image.");

    m_text = text;


    // QString isNull = (m_imageInput.isNull()) ? "true" : "false";
    // qDebug() << QString("Image Input is empty: %1").arg(isNull);
    if (m_imageInput.isNull())
        return;

    int sizeText = text.size();
    int sizeImage = m_imageInput.width() * m_imageInput.height();

    while (sizeText < sizeImage)
    {
        m_text += text + " ";
        sizeText = m_text.size();
    }

    emit UpdateStatus("Aligning is done. Text may be used for image conversion.");
}

void Logic::OnFontChanged(const QFont &font)
{
    // Whenever user chooses new font in dialog or clicks cancel,
    // we receive new font object - set by him or default.
    // We'll use it as a font for our drawable symbols later.

    emit UpdateStatus("Font has changed.");

    SetFont(font);
}

void Logic::OnScaleChanged(const QString &scale)
{
    // When user sets scale value and presses enter, we:
    // 1. Send message somewhere.
    // 2. Call SetScale method. Since scale is a string, we need to convert it to integer.
    // 3. Update status message.

    emit UpdateStatus("Scale has changed.");

    SetScale(scale.toInt());

    emit UpdateStatus(QString("Scale set to: %1.").arg(scale));
}

void Logic::OnThresholdChanged(const QString &threshold)
{
    // When user sets different values for threshold and presses enter, we:
    // 1. Send status message.
    // 2. Split received string into list items called terms.
    //    - at(0) - tR - threshold value for red component;
    //    - at(1) - tG - threshold value for green component;
    //    - at(2) - tB - threshold value for blue component;
    // If value of current pixels' component is greater than set value,
    // We change that pixel into letter of the same color,
    // Otherwise, just draw pixel, scaled if needed.
    // 3. Set values using SetThresholds method.
    // 4. Send pigeon with mail.

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
    // When user clicks on convert button, we:
    // 1. Emit update status.
    // 2. Check if all parameters are set correctly.
    // 3. Send messages and call a method to convert image.
    // 4. If not all parameters were set, we send another message.

    emit UpdateStatus("Checking Parameters");

    if (AllDataIsSet())
    {
        emit UpdateStatus("All needed parameters are set. Beginning the process of image conversion.");

        ConvertImage();

        emit UpdateStatus("Image conversion has passed successfuly.");
    }
    else
        emit UpdateStatus("Not all needed parameters are set. Please check what is left.");
}

bool Logic::AllDataIsSet()
{
    // If all parameters are set, we allow conversion and vice versa.
    if (!m_text.isEmpty() && !m_imageInput.isNull() && m_font.isSet() && m_scale.isSet() && m_threshold.isSet())
        return true;
    else
        return false;
}

void Logic::OnClickedSave(const QString& filename)
{
    // When user clicks on save button, we:
    // 1. Emit update status.
    // 2. If there is an actual image, save it.
    // 3. If there is no image, send error status.
    // 4. Send message, that everything is ok.

    emit UpdateStatus("Wait. Output image is saving now.");

    if (!m_imageOutput.isNull())
        m_imageOutput.save(filename, "png", 50);
    else
        emit UpdateStatus("There is no Output Image!");

    emit UpdateStatus("Saving is over. Thank you for using our application.");
}

void Logic::Void()
{
    // Previews for image fragments. Used by GatherConvertionResults method.
    /*
    qDebug() << QString("Current thread returned fragment of image with these parameters. From: %1. To: %2. W: %3. H: %4").arg(from).arg(to).arg(fragment.width()).arg(fragment.height());
    ImagePreviewWidget *wgt = new ImagePreviewWidget(nullptr, false);
    wgt->setImage(fragment);
    wgt->show();
    qDebug() << QString("Current Thread Data. From: %1. To: %2. Image width: %3. Image height: %4.").arg(from).arg(to).arg(fragment.width()).arg(fragment.height());
    */

    // Single thread version of conversion. Used by ConvertImage method.
    /*
     SetupOutputImage(); // W, H, F

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


