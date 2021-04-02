#ifndef IMAGEWIDGET_H
#define IMAGEWIDGET_H

#include <QWidget>
#include <QPainter>

#include <QLabel>
#include <QHBoxLayout>

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
    void setImage(const QImage& image);
    bool hasImage();

private:    
    void init();
    void makeGUI();
    void makeConnections();

    void setScaleFactor(float value);
    bool isAllowedScaleFactor(float value);

    bool m_bClickable, m_bFree;
    const int m_width = 300;
    const int m_height = 300;
    const int m_widthMax = 2048;
    const int m_heightMax = 2048;

    bool m_bImageFullSet = false;
    QImage m_imageFull;
    QImage m_imagePreview;
    QPainter m_painter;
    QPen m_pen;

    QImage m_scaledFragment;
    const float m_scaleFactorMin = 1.0f;
    const float m_scaleFactorMax = 3.0f;
    const float m_scaleFactorStep = 0.1f;
    float m_scaleFactor;


    QLabel *m_lSize;
    QHBoxLayout *m_layout;

signals:
    void UserScaledTheImage(bool zoomIn);

public slots:
    void OnUserScaledTheImage(bool zoomIn);
};

#endif // IMAGEWIDGET_H
