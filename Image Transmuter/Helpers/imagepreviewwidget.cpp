#include "imagepreviewwidget.h"

#include <QMouseEvent>
#include <QDebug>

ImagePreviewWidget::ImagePreviewWidget(QWidget* parent, bool clickable, bool free)
    : QWidget(parent),
      m_bClickable(clickable),
      m_bFree(free)
{
    init();
    makeGUI();
    makeConnections();
}

ImagePreviewWidget::~ImagePreviewWidget()
{
    m_lSize->deleteLater();
    m_layout->deleteLater();
}

void ImagePreviewWidget::init()
{
    m_pen = getPenFor(Qt::black, Qt::Dense4Pattern, 4);

    setScaleFactor(m_scaleFactorMin);
}

void ImagePreviewWidget::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event);

    m_painter.begin(this);
        if (!m_bFree && !m_imagePreview.isNull())
            m_painter.drawImage(rect(), m_imagePreview);

        if (m_bFree && !m_imageFull.isNull())
            m_painter.drawImage(rect(), m_imageFull);
        m_painter.setPen(m_pen);
        m_painter.drawRect(rect());
    m_painter.end();
}

void ImagePreviewWidget::mouseDoubleClickEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton)
    {
        if (m_bClickable && !m_imageFull.isNull())
        {
            ImagePreviewWidget* preview = new ImagePreviewWidget(nullptr, false, true);
            preview->setImage(m_imageFull);
            preview->show();
        }
    }
}

void ImagePreviewWidget::wheelEvent(QWheelEvent *ev)
{
    int movement = ev->angleDelta().ry();

    if (movement > 0)
    {
        // qDebug() << "UP";
        emit UserScaledTheImage(true);
    }
    else
    {
        // qDebug() << "DOWN";
        emit UserScaledTheImage(false);
    }

    ev->accept();
}

QPen ImagePreviewWidget::getPenFor(const QColor &color, const Qt::BrushStyle &brushStyle, int width)
{
    return QPen(QBrush(color, brushStyle), width);
}

void ImagePreviewWidget::setImage(const QImage &image)
{
    // qDebug() << QString("IW=%1, IH=%2").arg(image.width()).arg(image.height());

    if (!m_bImageFullSet)
    {
        m_imageFull = image;
        // if (m_imageFull.width() > m_widthMax || m_imageFull.height() > m_heightMax)
        //     m_imageFull = m_imageFull.scaledToWidth(m_widthMax);

        m_bImageFullSet = true;
    }
    m_imagePreview = image.scaled(m_width, m_height);
    m_lSize->setText(QString("%1x%2").arg(m_imageFull.width()).arg(m_imageFull.height()));

    repaint();
}

bool ImagePreviewWidget::hasImage()
{
    return !m_imagePreview.isNull();
}

void ImagePreviewWidget::makeGUI()
{
    if (!m_bFree)
        setFixedSize(m_width, m_height);
    else
        setFixedSize(m_imageFull.width(), m_imageFull.height());

    m_lSize = new QLabel("", this);
    m_lSize->setFixedSize(100, 30);

    m_layout = new QHBoxLayout(this);
    m_layout->addWidget(m_lSize, 0, Qt::AlignRight | Qt::AlignBottom);
    setLayout(m_layout);
}

void ImagePreviewWidget::makeConnections()
{
    // Do this just for free instances of ImagePreviewWidget
    if (m_bFree)
        connect(this, SIGNAL(UserScaledTheImage(bool)), this, SLOT(OnUserScaledTheImage(bool)));
}

void ImagePreviewWidget::setScaleFactor(float value)
{
    if (isAllowedScaleFactor(value))
        m_scaleFactor = value;
}

bool ImagePreviewWidget::isAllowedScaleFactor(float value)
{
    if (value >= m_scaleFactorMin && value <= m_scaleFactorMax)
        return true;
    else
        return false;
}

void ImagePreviewWidget::OnUserScaledTheImage(bool zoomIn)
{
    if (zoomIn)
    {
        qDebug() << m_scaleFactor;

        if (isAllowedScaleFactor(m_scaleFactor + m_scaleFactorStep))
        {
            m_scaleFactor += m_scaleFactorStep;

            m_scaledFragment = m_imageFull.scaled(m_scaleFactor*m_imageFull.width(),m_scaleFactor*m_imageFull.height());
            setFixedSize(m_scaledFragment.size());
            setImage(m_scaledFragment);
        }
    }
    else
    {
         qDebug() << m_scaleFactor;

        if (isAllowedScaleFactor(m_scaleFactor - m_scaleFactorStep))
        {
            m_scaleFactor -= m_scaleFactorStep;

            if (m_scaleFactor == m_scaleFactorMin)
                setImage(m_imageFull);
            else
            {
                m_scaledFragment = m_imageFull.scaled(m_scaleFactor*m_imageFull.width(),m_scaleFactor*m_imageFull.height());
                setFixedSize(m_scaledFragment.size());
                setImage(m_scaledFragment);
            }
        }
    }
}
