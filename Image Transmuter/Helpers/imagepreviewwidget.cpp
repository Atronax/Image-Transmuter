#include "imagepreviewwidget.h"

#include <QMouseEvent>

ImagePreviewWidget::ImagePreviewWidget(QWidget* parent, bool fixedSize) : QWidget(parent)
{
    if (fixedSize)
        setFixedSize(m_width, m_height);

    m_pen = getPenFor(Qt::black, Qt::Dense4Pattern, 4);
}

void ImagePreviewWidget::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event);

    m_painter.begin(this);
        if (!m_imagePreview.isNull())
            m_painter.drawImage(rect(), m_imagePreview);
        m_painter.setPen(m_pen);
        m_painter.drawRect(rect());
    m_painter.end();
}

void ImagePreviewWidget::mouseMoveEvent(QMouseEvent *event)
{
    if (event->type() == QMouseEvent::Enter)
        setFixedSize(m_width + 20, m_height + 20);

    if (event->type() == QEvent::Leave)
        setFixedSize(m_width,m_height);
}

QPen ImagePreviewWidget::getPenFor(const QColor &color, const Qt::BrushStyle &brushStyle, int width)
{
    return QPen(QBrush(color, brushStyle), width);
}

void ImagePreviewWidget::setImage(const QImage &image)
{
    m_imageFull = image;
    m_imagePreview = image.scaled(m_width, m_height);

    repaint();
}

bool ImagePreviewWidget::hasImage()
{
    return !m_imagePreview.isNull();
}
