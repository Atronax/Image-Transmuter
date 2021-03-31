#include <QApplication>

#include "controller.h"

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    Controller c;
    c.show();    

    return app.exec();
}

/*
QWidget *nothing = new QWidget;
nothing->show();

QStringList arguments = app.arguments();
if (arguments.size() >= 5)
{
    nothing->setWindowTitle("Обработка изображения начата! Пожалуйста подождите...");
    QString src_path = arguments[1];
    QString dst_path = arguments[2];
    uint scale = arguments[3].toUInt();

    // from text block
    bool from_text = false;
    if (arguments[4].toLower() == "y")
        from_text = true;

    int current_char = 0;
    QString text = "";

    if (from_text)
    {
        QString text_path = arguments[5];
        QFile file (text_path);
        if (file.open(QIODevice::ReadOnly | QIODevice::Text))
        {
            QTextStream stream (&file);
            stream.setCodec("windows-1251");
            text = stream.readAll();
        }
    }
    // from text block

    QImage src = QImage(src_path).scaled(640, 480);
    QImage res = QImage(src.width()*scale, src.height()*scale, QImage::Format_RGBA8888);
    QPainter painter;

    painter.begin(&res);
    bool bOk;
    QFont font = QFontDialog::getFont(&bOk);
    if (bOk)
        painter.setFont(font);
        painter.setRenderHint(QPainter::Antialiasing);

        QColor color;
        for (int h = 0; h < src.height(); ++h)
        {
            for (int w = 0; w < src.width(); ++w)
            {
                color = QColor::fromRgb(src.pixel(w,h));

                int threshold = 123;
                if (color.red() > threshold && color.green() > threshold && color.blue() > threshold)
                {
                    painter.setPen(QColor::fromRgb(src.pixel(w,h)));
                    if (from_text)
                        painter.drawText(w*scale,h*scale,text.at(++current_char));
                    else
                        painter.drawText(w*scale,h*scale,QString(0x2309));// QString::number(rand()%2));
                }
                else
                    res.setPixelColor(w, h, color);

                qApp->processEvents();
            }
            nothing->setWindowTitle(QString("Осталось: %1 процентов").arg(100 - 100.0*h/src.height()));
        }
    painter.end();

    nothing->setWindowTitle("Подождите. Идет сохранение файла.");
        res.save(dst_path, "jpg");
    nothing->setWindowTitle("Сохранение завершено. Спасибо за использование нашей программы.");
}
else
    nothing->setWindowTitle("HOW use it: imagetransmuter.exe sourcePath destPath scaling fromTextOrNo textPath");

// nothing->close();
*/
