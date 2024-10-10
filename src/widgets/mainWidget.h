//
// Created by 25415 on 2024/8/2.
//

#ifndef UNTITLED_TEST_H
#define UNTITLED_TEST_H

#include <QWidget>
#include <QTableWidget>
#include <QSpinBox>
#include <QSlider>
#include <QCheckBox>
#include "TableItemData.h"
#include "ImageGraphicsView.h"
#include "QMouseEvent"
#include "Utils.h"

class mainWidget : public QWidget {
    Q_OBJECT

    QTableWidget *preCompressionTable;
    QMap<int, QString> compressedMap;
    ImageGraphicsView *originalImage;
    ImageGraphicsView *compressedImage;
    Qt::SortOrder *sortOrder;
    CompressionOptions compressionOptions;
    QSpinBox *pngQualitySpinBox;
    QSlider *pngQualitySlider;
    QSpinBox *jpegQualitySpinBox;
    QSlider *jpegQualitySlider;
    QSpinBox *webpQualitySpinBox;
    QSlider *webpQualitySlider;
    bool isOriginalDirOutput = false;

    SizeModeOptions sizeMode = SizeModeOptions::PERCENTAGE;
    QSpinBox *resizeWidthSpinBox;
    QSpinBox *resizeHeightSpinBox;
    QCheckBox *keepOriginSizeCheckBox;
    QCheckBox *keepOriginScaleCheckBox;

public:
    explicit mainWidget(QWidget *parent = nullptr);

    ~mainWidget() override;

    static int addRow(QTableWidget *table, int row, const TableItemData &data);

    static void initTableFromFile(QTableWidget *table, const QString &path);


    void resizeEvent(QResizeEvent *event) override;

    void contextMenuEvent(QContextMenuEvent *event) override;

public slots:
    void setTableContextMenu(QPoint pos);

    void setHeaderClickSort(int col);

    void setPreviewImage(int row, int col);

    void setTableDoubleClick(int row, int col);

    void compressImage();

    void splitterMoved();

    void setPngQuality(int value);

    void setJpegQuality(int value);

    void setWebpQuality(int value);

    void onCompressFinished();

    void onSizeComboBoxChanged(int index);

    void onResizeWidthSpinBoxChanged(int value);

    void onResizeHeightSpinBoxChanged(int value);

    void onKeepOriginSizeCheckBoxChanged(int value);

    void onKeepOriginScaleCheckBoxChanged(int value);
};


#endif //UNTITLED_TEST_H
