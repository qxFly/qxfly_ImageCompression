#include "ImageGraphicsView.h"
#include <QMouseEvent>
#include <QGraphicsItem>
#include <QScrollBar>

ImageGraphicsView::ImageGraphicsView(QWidget *parent)
    : QGraphicsView(parent) {
    this->graphicsScene = new QGraphicsScene();
    this->setScene(this->graphicsScene);
}

void ImageGraphicsView::showPixmap(const QPixmap &pixmap) {
    this->originPreImgSize = pixmap.size();
    this->changedPreImgSize = pixmap.size();
    this->graphicsScene->clear();
    this->graphicsScene->addPixmap(pixmap);
    this->graphicsScene->setSceneRect(this->graphicsScene->itemsBoundingRect());
    this->fitInView(pixmap.rect(), Qt::KeepAspectRatio);
}


void ImageGraphicsView::wheelEvent(QWheelEvent *event) {
    this->zooming = true;
    this->setScaleFactor(event);
    emit this->zoomChange(event);
    this->zooming = false;
    emit this->horizontalScrollBar()->valueChanged(this->horizontalScrollBar()->value());
    emit this->verticalScrollBar()->valueChanged(this->verticalScrollBar()->value());
}

void ImageGraphicsView::setScaleFactor(QWheelEvent *event) {
    const ViewportAnchor anchor = transformationAnchor();
    setTransformationAnchor(QGraphicsView::AnchorViewCenter);
    double ratio = static_cast<double>(changedPreImgSize.width()) / originPreImgSize.width();
    if (event->angleDelta().ry() > 0) {
        if (ratio < 30) {
            scale(SCALE_UP, SCALE_UP);
            changedPreImgSize.setWidth(changedPreImgSize.width() * SCALE_UP);
        }
    } else {
        if (ratio > 0.1) {
            scale(SCALE_DOWN, SCALE_DOWN);
            changedPreImgSize.setWidth(changedPreImgSize.width() * SCALE_DOWN);
        }
    }
    setTransformationAnchor(anchor);
}

void ImageGraphicsView::setVScrollBarValue(int value) {
    if (!zooming) {
        this->verticalScrollBar()->setValue(value);
    }
}

void ImageGraphicsView::setHScrollBarValue(int value) {
    if (!zooming) {
        this->horizontalScrollBar()->setValue(value);
    }
}
