//
// Created by 25415 on 2024/8/11.
//

#ifndef MYQGRAPHICSVIEW_H
#define MYQGRAPHICSVIEW_H


#include <QGraphicsView>

class ImageGraphicsView : public QGraphicsView {
    Q_OBJECT

private:
    QGraphicsScene *graphicsScene;
    double SCALE_UP = 1.05;
    double SCALE_DOWN = 0.95;
    QSize originPreImgSize;
    QSize changedPreImgSize;
    bool zooming = false;

public:
    explicit ImageGraphicsView(QWidget *parent = nullptr);

    void wheelEvent(QWheelEvent *event) override;

    void showPixmap(const QPixmap &pixmap);

public slots:
    void setScaleFactor(QWheelEvent *event);

    void setVScrollBarValue(int value);

    void setHScrollBarValue(int value);

signals:
    void zoomChange(QWheelEvent *event);
};


#endif //MYQGRAPHICSVIEW_H
