#ifndef IMAGEWIDGET_H
#define IMAGEWIDGET_H

#include <QWidget>
#include <QPainter>

class ImagePreviewWidget : public QWidget
{
    Q_OBJECT

public:
    explicit ImagePreviewWidget(QWidget* parent = nullptr, bool fixedSize = true);

    void paintEvent(QPaintEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;

    QPen getPenFor(const QColor& color, const Qt::BrushStyle& brushStyle, int width);
    void setImage(const QImage& image);
    bool hasImage();

private:
    QImage m_imageFull;
    QImage m_imagePreview;

    QPainter m_painter;
    QPen m_pen;

    const int m_width = 300;
    const int m_height = 300;
};

#endif // IMAGEWIDGET_H
