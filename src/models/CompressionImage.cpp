//
// Created by 25415 on 2024/9/20.
//
#define qout qDebug() << __FILE__ << " " << __LINE__ << " "

#include "Utils.h"
#include "CompressionImage.h"
#include <opencv2/opencv.hpp>
#include <QtConcurrent>
#include <QFuture>
#include <QLabel>
#include <QTableWidget>
#include <QTextDocument>
#include <QTextCursor>

CompressionImage::CompressionImage(QAbstractItemModel *tableModel, CompressionOptions &compressionOptions) {
    this->tableModel = tableModel;
    this->compressionOptions = compressionOptions;
}

int CompressionImage::compress() {
    this->futureWatcher = new QFutureWatcher<void>();
    if (this->tableModel == nullptr) {
        qout << "tableModel is nullptr";
        return -1;
    }
    for (int i = 0; i < this->tableModel->rowCount(); i++) {
        this->futureWatcher->setFuture(compress1(i));
    }
    return 1;
}

QFuture<void> CompressionImage::compress1(int index) {
    return QtConcurrent::run([=]() {
        QString path = this->tableModel->index(index, 3).data().toString();
        QString fileName = this->tableModel->index(index, 0).data().toString();
        QString suffix = fileName.split(".").last();
        cv::Mat img = cv::imread(path.toLocal8Bit().toStdString());
        /* 读取为空 */
        if (img.empty()) {
            this->tableModel->setData(this->tableModel->index(index, 4), "不是图片");
            return;
        }
        double width = compressionOptions.width;
        double height = compressionOptions.height;
        /* 判断是否保持原大小 */
        if (!compressionOptions.keepOriginalSize) {
            if (this->compressionOptions.sizeMode == SizeModeOptions::PERCENTAGE) {
                cv::resize(img, img, cv::Size(0, 0), width / 100.0, height / 100.0, cv::INTER_AREA);
            } else {
                double scale = static_cast<double>(img.rows) / img.cols;
                /* 判断是否保持原比例 */
                if (this->compressionOptions.keepOriginalScale) {
                    if (this->compressionOptions.SizeFitMode == SizeFitMode::FIT_TO_WIDTH) {
                        height = width * scale;
                    } else if (this->compressionOptions.SizeFitMode == SizeFitMode::FIT_TO_HEIGHT) {
                        width = height / scale;
                    }
                }
                cv::resize(img, img, cv::Size(width, height), 0, 0, cv::INTER_AREA);
            }
        }
        /* 压缩 */
        std::vector<int> compression_params;
        if (suffix == "png") {
            compression_params.push_back(cv::IMWRITE_PNG_COMPRESSION);
            compression_params.push_back(this->compressionOptions.pngQuality); // 0-9, where 9 is the highest compression
        } else if (suffix == "jpg" || suffix == "jpeg") {
            compression_params.push_back(cv::IMWRITE_JPEG_QUALITY);
            compression_params.push_back(this->compressionOptions.jpegQuality); // 0-100, where 100 is the highest quality
        } else if (suffix == "webp") {
            compression_params.push_back(cv::IMWRITE_WEBP_QUALITY);
            compression_params.push_back(this->compressionOptions.webpQuality); // 1-100, where 100 is the highest quality
        } else {
            return;
        }
        QString outputPath = this->compressionOptions.outputPath;
        if (this->compressionOptions.isOriginalDirOutput) {
            outputPath = QFileInfo(path).dir().path();
        }

        /* 输出 */
        auto *parent = dynamic_cast<QTableWidget *>(this->tableModel->parent());
        parent->item(index,0)->setIcon(QIcon(":/ui/icon/compressing.svg"));
        QString outputFullPath = outputPath + "/" + fileName;
        this->tableModel->setData(this->tableModel->index(index, 5), "压缩中");
        cv::imwrite(outputFullPath.toLocal8Bit().toStdString(), img, compression_params);
        this->tableModel->setData(this->tableModel->index(index, 5), "压缩完成");
        parent->item(index,0)->setIcon(QIcon(":/ui/icon/compressed.svg"));
        this->tableModel->setData(this->tableModel->index(index, 4), outputFullPath);
    });
}
