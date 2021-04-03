#ifndef IMAGEWIDGET_H
#define IMAGEWIDGET_H

#include <QWidget>
#include <QPainter>

#include <QLabel>
#include <QHBoxLayout>
#include <QGraphicsView>
#include <QGraphicsPixmapItem>

class ImagePreviewWidget : public QWidget
{
    Q_OBJECT

public:
    explicit ImagePreviewWidget(QWidget* parent = nullptr, bool clickable = false, bool free = false);
    ~ImagePreviewWidget();

    void paintEvent(QPaintEvent *event) override;
    // void mousePressEvent(QMouseEvent *event) override;
    void mouseDoubleClickEvent(QMouseEvent *event) override;
    void wheelEvent(QWheelEvent *event) override;

    QPen getPenFor(const QColor& color, const Qt::BrushStyle& brushStyle, int width);
    void updateImage(const QImage& image);
    bool hasImage();

private:    
    void init();
    void makeGUI();
    void makeConnections();

    void setScaleFactor(float value);
    bool isAllowedScaleFactor(float value);

    void cropFragment(QImage& original, QImage& fragment, QRect rect);

    // Boolean values representing state of instance:
    // - if it can be double clicked, m_bClickable should be true;
    // - if it has no parent widget, m_bFree is true.
    bool m_bClickable, m_bFree;    

    // Original image. We set it only once using boolean guard value.
    bool m_bImageFullSet = false;
    QImage m_imageFull;

    // Preview related variables:
    // - width and height of this widget instance;
    // - preview image to paint on widget surface;
    // - painter object and its settings.
    const int m_width = 300;
    const int m_height = 300;
    QImage m_imagePreview;
    QPainter m_painter;
    QPen m_pen;

    // Scaling related variables:
    // - cropped fragment and its scaled variant;
    // - scale factor min and max variables, also step;
    // - current scale factor.
    QImage m_imageFragment, m_scaledFragment;
    const float m_scaleFactorMin = 1.0f;
    const float m_scaleFactorMax = 1.5f;
    const float m_scaleFactorStep = 0.1f;
    float m_scaleFactor;

    // GUI related variables:
    // - size label, that shows full image width and height.
    QLabel *m_lSize;
    QHBoxLayout *m_layout;

signals:
    void UserScaledTheImage(bool zoomIn);

public slots:
    void OnUserScaledTheImage(bool zoomIn);
};

#endif // IMAGEWIDGET_H
