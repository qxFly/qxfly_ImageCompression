//
// Created by 25415 on 2024/9/20.
//

#ifndef UNTITLED_COMPRESSIONIMAGE_H
#define UNTITLED_COMPRESSIONIMAGE_H

#include "Utils.h"
#include <QFuture>
#include <QAbstractItemModel>
#include <QFutureWatcher>

class CompressionImage : public QObject {
    Q_OBJECT

private:
    QFutureWatcher<void> *futureWatcher;
    QAbstractItemModel *tableModel;
    CompressionOptions compressionOptions;
    QVector<CompressionOptions *> tableItemDataList;

    QFuture<void> compress1(int index) const;

public:
    CompressionImage(QAbstractItemModel *tableModel, CompressionOptions &compressionOptions);

    int compress();

public slots:
};


#endif //UNTITLED_COMPRESSIONIMAGE_H
