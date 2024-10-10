//
// Created by 25415 on 2024/8/2.
//

#define qout qDebug() << __FILE__ << " " << __LINE__ << " "

#include <QHeaderView>
#include <QSplitter>
#include <QFileDialog>
#include <QImageReader>
#include <QDesktopServices>
#include <QLabel>
#include <QScrollBar>
#include <QtConcurrent>
#include <opencv2/opencv.hpp>
#include <QMessageBox>
#include <windows.h>
#include <QLineEdit>
#include <QComboBox>
#include <QSettings>
#include "mainWidget.h"
#include "QPushButton"
#include "QMenuBar"
#include "QVBoxLayout"
#include "QTableWidget"
#include "TableItemData.h"
#include "QGraphicsView"
#include "ImageGraphicsView.h"
#include "CITableWidgetItem.h"
#include "Utils.h"
#include "CompressionImage.h"

mainWidget::mainWidget(QWidget *parent) : QWidget(parent) {
    /*设置窗口大小*/
    HDC screen_dc = GetDC(nullptr);
    // 获取设备独立像素 (DIP) 的分辨率
    int width = GetDeviceCaps(screen_dc, DESKTOPHORZRES);
    int height = GetDeviceCaps(screen_dc, DESKTOPVERTRES);
    // 释放设备上下文
    ReleaseDC(nullptr, screen_dc);
    this->resize(width / 1.5, height / 1.5);
    /*引入qrc*/
    QFile styleFile(":/style/ui.qss");
    styleFile.open(QFile::ReadOnly);
    /*设置窗口背景色*/
    QPalette qPalette = this->palette();
    qPalette.setColor(this->backgroundRole(), Qt::white);
    this->setPalette(qPalette);
    /*主布局*/
    auto *vbox = new QVBoxLayout(this);
    vbox->setContentsMargins(2, 0, 2, 2);
    /*菜单栏*/
    auto *menuBar = new QMenuBar(this);
    menuBar->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    auto *fileMenu = new QMenu("文件", this);
    QAction *openFileAct = fileMenu->addAction("打开文件");
    fileMenu->addSeparator();
    QAction *openDirAct = fileMenu->addAction("打开文件夹");
    auto *other = new QMenu("其他");
    other->addAction(openDirAct);
    menuBar->addMenu(fileMenu);
    menuBar->addSeparator();
    menuBar->addMenu(other);
    vbox->addWidget(menuBar);

    /*打开文件*/
    connect(openFileAct, &QAction::triggered, [=]() {
        if (const QString fileName = QFileDialog::getOpenFileName(this); !fileName.isEmpty()) {
            this->preCompressionTable->clearContents();
            this->preCompressionTable->setRowCount(0);
            initTableFromFile(this->preCompressionTable, fileName);
        }
    });
    /*打开文件夹*/
    connect(openDirAct, &QAction::triggered, [=]() {
        QFileDialog fileDialog;
        QString dir = QFileDialog::getExistingDirectory(this);
        this->preCompressionTable->clearContents();
        this->preCompressionTable->setRowCount(0);
        initTableFromFile(this->preCompressionTable, dir);
    });

    /**
     * 主分割窗
     */
    auto *mainSplitter = new QSplitter(Qt::Vertical, this);
    mainSplitter->setChildrenCollapsible(false);
    mainSplitter->setStyleSheet(styleFile.readAll());
    vbox->addWidget(mainSplitter);
    connect(mainSplitter, &QSplitter::splitterMoved, this, &mainWidget::splitterMoved);
    /**
     * 第一个分割窗
     */
    auto *firstSpl = new QSplitter(Qt::Horizontal, mainSplitter);
    firstSpl->setChildrenCollapsible(false);
    firstSpl->setMinimumHeight(this->height() / 3);
    firstSpl->setStyleSheet("QSplitter::handle{background-image:url(:/ui/splitter_handle_v.png);}");
    this->preCompressionTable = new QTableWidget(firstSpl);
    this->preCompressionTable->setMinimumWidth(this->width() / 3);
    this->preCompressionTable->setColumnCount(6);
    this->preCompressionTable->horizontalHeader()->setSectionResizeMode(QHeaderView::Interactive);
    this->preCompressionTable->horizontalHeader()->setStretchLastSection(true);
    this->preCompressionTable->horizontalHeader()->setMinimumSectionSize(50);
    this->preCompressionTable->setColumnWidth(0, 150);
    this->preCompressionTable->setColumnWidth(1, 100);
    this->preCompressionTable->setColumnWidth(3, 200);
    this->preCompressionTable->setHorizontalHeaderLabels(QStringList() << "文件名" << "大小" << "分辨率" << "路径" << "压缩状态" << "保存路径");
    this->preCompressionTable->setContextMenuPolicy(Qt::CustomContextMenu); //右键菜单
    this->preCompressionTable->setEditTriggers(QAbstractItemView::NoEditTriggers); //禁止编辑
    this->preCompressionTable->horizontalHeader()->setSortIndicatorShown(true);
    this->preCompressionTable->setSelectionBehavior(QAbstractItemView::SelectRows);
    /*初始化点击表头排序*/
    int c = this->preCompressionTable->columnCount();
    this->sortOrder = new Qt::SortOrder[c];
    for (int i = 0; i < c; i++) {
        this->sortOrder[i] = Qt::AscendingOrder;
    }
    /*选中行时展示预览图*/
    connect(this->preCompressionTable, &QTableWidget::cellClicked, this, &mainWidget::setPreviewImage);
    /*表格右键菜单*/
    connect(this->preCompressionTable, &QTableWidget::customContextMenuRequested, this, &mainWidget::setTableContextMenu);
    /*双击打开文件*/
    connect(this->preCompressionTable, &QTableWidget::cellDoubleClicked, this, &mainWidget::setTableDoubleClick);
    /*表头点击排序*/
    connect(this->preCompressionTable->horizontalHeader(), &QHeaderView::sectionClicked, this, &mainWidget::setHeaderClickSort);

    /**
     * 压缩选项窗
     */
    auto *firstSpl_secFrame = new QFrame(firstSpl);
    firstSpl_secFrame->setMinimumWidth(this->width() / 3);
    auto *firstSpl_secFrame_layout = new QGridLayout(firstSpl_secFrame);
    firstSpl_secFrame_layout->setContentsMargins(0, 0, 0, 0);
    auto *tabWidget = new QTabWidget();
    /* 压缩选项页 */
    auto *compressionOptWidget = new QWidget(tabWidget);
    auto *compressOptionLayout = new QVBoxLayout(compressionOptWidget);

    /* png 质量*/
    auto *pngQualityHLayout1 = new QHBoxLayout();
    auto *pngQualityHLayout2 = new QHBoxLayout();
    auto *pngQualityHLayout3 = new QHBoxLayout();
    compressOptionLayout->addLayout(pngQualityHLayout1);
    compressOptionLayout->addLayout(pngQualityHLayout2);
    compressOptionLayout->addLayout(pngQualityHLayout3);
    auto *spacerItem = new QSpacerItem(0, 0, QSizePolicy::Expanding, QSizePolicy::Minimum);
    auto *pngQualityLabel = new QLabel("PNG质量");
    pngQualityLabel->setSizePolicy(QSizePolicy::Maximum, QSizePolicy::Minimum);
    auto *pngQualityLabelLine = new QFrame();
    pngQualityLabelLine->setFrameShape(QFrame::HLine);
    pngQualityLabelLine->setFixedHeight(20);
    this->pngQualitySpinBox = new QSpinBox();
    this->pngQualitySpinBox->setFixedWidth(50);
    this->pngQualitySpinBox->setMaximum(9);
    this->pngQualitySlider = new QSlider(Qt::Horizontal);
    this->pngQualitySlider->setMaximum(9);
    this->pngQualitySlider->setTickPosition(QSlider::TicksBelow);
    this->pngQualitySlider->setTickInterval(1);
    pngQualityHLayout1->addWidget(pngQualityLabel);
    pngQualityHLayout1->addSpacerItem(spacerItem);
    pngQualityHLayout1->addWidget(this->pngQualitySpinBox);
    pngQualityHLayout2->addWidget(this->pngQualitySlider);
    //    pngQualityHLayout3->addWidget(pngQualityLabelLine);
    connect(this->pngQualitySpinBox, &QSpinBox::valueChanged, this, &mainWidget::setPngQuality);
    connect(this->pngQualitySlider, &QSlider::valueChanged, this, &mainWidget::setPngQuality);
    this->pngQualitySpinBox->setValue(5);
    this->pngQualitySlider->setValue(5);
    /* jpg 质量*/
    auto *jpgQualityHLayout1 = new QHBoxLayout();
    auto *jpgQualityHLayout2 = new QHBoxLayout();
    auto *jpgQualityHLayout3 = new QHBoxLayout();
    compressOptionLayout->addLayout(jpgQualityHLayout1);
    compressOptionLayout->addLayout(jpgQualityHLayout2);
    compressOptionLayout->addLayout(jpgQualityHLayout3);
    auto *jpgQualityLabel = new QLabel("JPG质量");
    jpgQualityLabel->setSizePolicy(QSizePolicy::Maximum, QSizePolicy::Minimum);
    auto *jpgQualityLabelLine = new QFrame();
    jpgQualityLabelLine->setFrameShape(QFrame::HLine);
    jpgQualityLabelLine->setFixedHeight(20);
    this->jpegQualitySpinBox = new QSpinBox();
    this->jpegQualitySpinBox->setFixedWidth(50);
    this->jpegQualitySpinBox->setMaximum(100);
    this->jpegQualitySlider = new QSlider(Qt::Horizontal);
    this->jpegQualitySlider->setMaximum(100);
    this->jpegQualitySlider->setTickPosition(QSlider::TicksBelow);
    this->jpegQualitySlider->setTickInterval(1);
    jpgQualityHLayout1->addWidget(jpgQualityLabel);
    jpgQualityHLayout1->addSpacerItem(spacerItem);
    jpgQualityHLayout1->addWidget(this->jpegQualitySpinBox);
    jpgQualityHLayout2->addWidget(this->jpegQualitySlider);
    //    jpgQualityHLayout3->addWidget(jpgQualityLabelLine);
    connect(jpegQualitySpinBox, &QSpinBox::valueChanged, this, &mainWidget::setJpegQuality);
    connect(jpegQualitySlider, &QSlider::valueChanged, this, &mainWidget::setJpegQuality);
    this->jpegQualitySpinBox->setValue(50);
    this->jpegQualitySlider->setValue(50);
    /* webp 质量*/
    auto *webpQualityHLayout1 = new QHBoxLayout();
    auto *webpQualityHLayout2 = new QHBoxLayout();
    auto *webpQualityHLayout3 = new QHBoxLayout();
    compressOptionLayout->addLayout(webpQualityHLayout1);
    compressOptionLayout->addLayout(webpQualityHLayout2);
    compressOptionLayout->addLayout(webpQualityHLayout3);
    auto *webpQualityLabel = new QLabel("webp质量");
    webpQualityLabel->setSizePolicy(QSizePolicy::Maximum, QSizePolicy::Minimum);
    auto *webpQualityLabelLine = new QFrame();
    webpQualityLabelLine->setFrameShape(QFrame::HLine);
    webpQualityLabelLine->setFixedHeight(20);
    this->webpQualitySpinBox = new QSpinBox();
    this->webpQualitySpinBox->setFixedWidth(50);
    this->webpQualitySpinBox->setMaximum(100);
    this->webpQualitySpinBox->setMinimum(1);
    this->webpQualitySlider = new QSlider(Qt::Horizontal);
    this->webpQualitySlider->setMaximum(100);
    this->webpQualitySlider->setMinimum(1);
    this->webpQualitySlider->setTickPosition(QSlider::TicksBelow);
    this->webpQualitySlider->setTickInterval(1);
    webpQualityHLayout1->addWidget(webpQualityLabel);
    webpQualityHLayout1->addSpacerItem(spacerItem);
    webpQualityHLayout1->addWidget(this->webpQualitySpinBox);
    webpQualityHLayout2->addWidget(this->webpQualitySlider);
    webpQualityHLayout3->addWidget(webpQualityLabelLine);
    connect(this->webpQualitySpinBox, &QSpinBox::valueChanged, this, &mainWidget::setWebpQuality);
    connect(this->webpQualitySlider, &QSlider::valueChanged, this, &mainWidget::setWebpQuality);
    this->webpQualitySlider->setValue(50);
    this->webpQualitySpinBox->setValue(50);
    /* 大小选项 */
    auto *resizeHLayout1 = new QHBoxLayout();
    auto *resizeHLayout2 = new QHBoxLayout();
    auto *resizeHLayout3 = new QHBoxLayout();
    auto *resizeHLayout4 = new QHBoxLayout();
    resizeHLayout2->setAlignment(Qt::AlignLeft);
    resizeHLayout3->setAlignment(Qt::AlignLeft);
    compressOptionLayout->addLayout(resizeHLayout1);
    compressOptionLayout->addLayout(resizeHLayout2);
    compressOptionLayout->addLayout(resizeHLayout3);
    compressOptionLayout->addLayout(resizeHLayout4);
    auto *resizeLabel = new QLabel("调整大小");
    auto *sizeComboBox = new QComboBox();
    sizeComboBox->setFixedWidth(80);
    sizeComboBox->addItem("百分比", 0);
    sizeComboBox->addItem("像素", 1);

    auto *resizeWidthLabel = new QLabel("宽:");
    resizeWidthLabel->setFixedWidth(20);
    this->resizeWidthSpinBox = new QSpinBox();
    this->resizeWidthSpinBox->setSuffix("%");
    this->resizeWidthSpinBox->setMaximum(100);
    this->resizeWidthSpinBox->setMinimum(1);
    this->resizeWidthSpinBox->setFixedWidth(100);
    auto *resizeHeightLabel = new QLabel("高:");
    resizeHeightLabel->setFixedWidth(20);
    this->resizeHeightSpinBox = new QSpinBox();
    this->resizeHeightSpinBox->setSuffix("%");
    this->resizeHeightSpinBox->setMaximum(100);
    this->resizeHeightSpinBox->setMinimum(1);
    this->resizeHeightSpinBox->setFixedWidth(100);
    this->keepOriginSizeCheckBox = new QCheckBox("保持原大小");
    this->keepOriginScaleCheckBox = new QCheckBox("保持宽高比");

    auto *resizeLabelLine = new QFrame();
    resizeLabelLine->setFrameShape(QFrame::HLine);
    resizeLabelLine->setFixedHeight(20);
    resizeHLayout1->addWidget(resizeLabel);
    resizeHLayout1->addWidget(sizeComboBox);
    resizeHLayout2->addWidget(resizeWidthLabel);
    resizeHLayout2->addWidget(this->resizeWidthSpinBox);
    resizeHLayout2->addWidget(this->keepOriginSizeCheckBox);
    resizeHLayout3->addWidget(resizeHeightLabel);
    resizeHLayout3->addWidget(this->resizeHeightSpinBox);
    resizeHLayout3->addWidget(this->keepOriginScaleCheckBox);
    resizeHLayout4->addWidget(resizeLabelLine);
    connect(sizeComboBox, &QComboBox::currentIndexChanged, this, &mainWidget::onSizeComboBoxChanged);
    connect(this->resizeWidthSpinBox, &QSpinBox::valueChanged, this, &mainWidget::onResizeWidthSpinBoxChanged);
    connect(this->resizeHeightSpinBox, &QSpinBox::valueChanged, this, &mainWidget::onResizeHeightSpinBoxChanged);
    connect(this->keepOriginSizeCheckBox, &QCheckBox::stateChanged, this, &mainWidget::onKeepOriginSizeCheckBoxChanged);
    connect(this->keepOriginScaleCheckBox, &QCheckBox::stateChanged, this, &mainWidget::onKeepOriginScaleCheckBoxChanged);
    this->keepOriginSizeCheckBox->setCheckState(Qt::Checked);
    this->keepOriginScaleCheckBox->setCheckState(Qt::Checked);
    this->resizeWidthSpinBox->setValue(100);
    this->resizeHeightSpinBox->setValue(100);
    /* 留白 */
    compressOptionLayout->addStretch();
    /* 输出选项页 */
    auto *outputOptWidget = new QWidget(tabWidget);
    auto *outputOptLayout = new QVBoxLayout(outputOptWidget);
    outputOptWidget->setLayout(outputOptLayout);
    auto *outPathHLayout1 = new QHBoxLayout();
    auto *outPathHLayout2 = new QHBoxLayout();
    outputOptLayout->addLayout(outPathHLayout1);
    outputOptLayout->addLayout(outPathHLayout2);
    auto *outPathLabel = new QLabel("输出路径：", outputOptWidget);
    auto *getOutPathBtn = new QPushButton("选择", outputOptWidget);
    auto *outPathLineEdit = new QLineEdit(this->compressionOptions.outputPath, outputOptWidget);
    /* 读取上一次路径 */
    QSettings settings("config.ini", QSettings::IniFormat);
    outPathLineEdit->setText(settings.value("lastOutputPath", QDir::currentPath()).toString());
    connect(getOutPathBtn, &QPushButton::clicked, [=]() {
        this->compressionOptions.outputPath = QFileDialog::getExistingDirectory(outputOptWidget, "选择输出路径", QDir::currentPath());
        outPathLineEdit->setText(this->compressionOptions.outputPath);
    });
    connect(outPathLineEdit, &QLineEdit::textChanged, [=](const QString &text) { this->compressionOptions.outputPath = text; });
    outPathHLayout1->addWidget(outPathLabel);
    outPathHLayout2->addWidget(getOutPathBtn);
    outPathHLayout2->addWidget(outPathLineEdit);
    outputOptLayout->addStretch();
    /* 压缩按钮组 */
    auto *compressBtnWidget = new QWidget();
    auto *btnHLayout = new QHBoxLayout(compressBtnWidget);
    btnHLayout->setContentsMargins(0, 0, 2, 0);
    compressBtnWidget->setLayout(btnHLayout);
    compressBtnWidget->setMinimumHeight(22);
    auto *btn = new QPushButton("压缩", compressBtnWidget);
    auto *isOriginalDirOutputCheckBox = new QCheckBox("原文件夹输出", outputOptWidget);
    btnHLayout->addStretch();
    btnHLayout->addWidget(isOriginalDirOutputCheckBox);
    btnHLayout->addWidget(btn);
    connect(btn, &QPushButton::clicked, this, &mainWidget::compressImage);
    connect(isOriginalDirOutputCheckBox, &QCheckBox::stateChanged, [=](int state) {
        if (state == Qt::Checked) {
            this->compressionOptions.isOriginalDirOutput = true;
        } else {
            this->compressionOptions.isOriginalDirOutput = false;
            this->compressionOptions.outputPath = outPathLineEdit->text();
        }
    });
    /**
     * 第二个分割窗
     */
    auto *secondSpl = new QSplitter(Qt::Horizontal, mainSplitter);
    secondSpl->setChildrenCollapsible(false);
    secondSpl->setMinimumHeight(this->height() / 3);
    secondSpl->setStyleSheet("QSplitter::handle{background-image:url(:/ui/splitter_handle_v.png);}");
    connect(secondSpl, &QSplitter::splitterMoved, this, &mainWidget::splitterMoved);
    this->originalImage = new ImageGraphicsView(secondSpl);
    this->originalImage->fitInView(this->originalImage->rect(), Qt::KeepAspectRatio);
    this->originalImage->setRenderHint(QPainter::LosslessImageRendering);
    this->originalImage->setDragMode(QGraphicsView::ScrollHandDrag);
    this->originalImage->setMinimumWidth(this->width() / 3);
    this->originalImage->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    this->originalImage->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    this->compressedImage = new ImageGraphicsView(secondSpl);
    this->compressedImage->fitInView(this->compressedImage->rect(), Qt::KeepAspectRatio);
    this->compressedImage->setRenderHint(QPainter::LosslessImageRendering);
    this->compressedImage->setDragMode(QGraphicsView::ScrollHandDrag);
    this->compressedImage->setMinimumWidth(this->width() / 3);
    this->compressedImage->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    this->compressedImage->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    // connect(this->originalImage, SIGNAL(zoomChange(QWheelEvent * )), this->compressedImage,SLOT(setScaleFactor(QWheelEvent * )));
    // connect(this->compressedImage, SIGNAL(zoomChange(QWheelEvent * )), this->originalImage,SLOT(setScaleFactor(QWheelEvent * )));
    connect(this->originalImage, &ImageGraphicsView::zoomChange, this->compressedImage, &ImageGraphicsView::setScaleFactor);
    connect(this->compressedImage, &ImageGraphicsView::zoomChange, this->originalImage, &ImageGraphicsView::setScaleFactor);
    connect(this->originalImage->verticalScrollBar(), &QAbstractSlider::valueChanged, this->compressedImage, &ImageGraphicsView::setVScrollBarValue);
    connect(this->originalImage->horizontalScrollBar(), &QAbstractSlider::valueChanged, this->compressedImage, &ImageGraphicsView::setHScrollBarValue);
    connect(this->compressedImage->verticalScrollBar(), &QAbstractSlider::valueChanged, this->originalImage, &ImageGraphicsView::setVScrollBarValue);
    connect(this->compressedImage->horizontalScrollBar(), &QAbstractSlider::valueChanged, this->originalImage, &ImageGraphicsView::setHScrollBarValue);
    initTableFromFile(this->preCompressionTable, R"(G:\Desktop\opencv-test\input)");

    firstSpl->addWidget(this->preCompressionTable);
    firstSpl->addWidget(firstSpl_secFrame);
    tabWidget->addTab(compressionOptWidget, "压缩选项");
    tabWidget->addTab(outputOptWidget, "输出路径");
    compressionOptWidget->setLayout(compressOptionLayout);
    firstSpl_secFrame->setLayout(firstSpl_secFrame_layout);
    firstSpl_secFrame_layout->addWidget(tabWidget, 0, 0);
    firstSpl_secFrame_layout->addWidget(compressBtnWidget, 1, 0);

    secondSpl->addWidget(this->originalImage);
    secondSpl->addWidget(this->compressedImage);
}


void mainWidget::initTableFromFile(QTableWidget *table, const QString &path) {
    QFileInfo file(path);
    if (file.isFile()) {
        QString resolution = "";
        auto *imgReader = new QImageReader(file.filePath());
        imgReader->setAutoTransform(true);
        if (imgReader->canRead()) {
            imgReader->setAutoTransform(true);
            resolution = QString("%1x%2").arg(imgReader->size().width()).arg(imgReader->size().height());
        }
        delete imgReader;
        TableItemData tableItemData(file.fileName(), file.size(), resolution, path);
        int row = table->rowCount();
        addRow(table, row, tableItemData);
    } else if (file.isDir()) {
        QDir dir(path);
        QStringList list = dir.entryList(QDir::Dirs | QDir::Files | QDir::NoDotAndDotDot);
        for (const QString &l: list) {
            QFileInfo f(dir, l);
            initTableFromFile(table, f.absoluteFilePath());
        }
    }
}

int mainWidget::addRow(QTableWidget *table, int row, const TableItemData &data) {
    double size = static_cast<double>(data.getSize()) / 1024.0 / 1024.0;
    QStringList rowData = QStringList() << data.getName() << QString::asprintf("%.4f MB", size) << data.getResolution() << data.getPath() << "未压缩" << nullptr;
    while (table->rowCount() - 1 < row) {
        table->insertRow(row);
    }
    for (int i = 0; i < table->columnCount(); i++) {
        auto *item = new QTableWidgetItem(rowData.at(i));
        table->setItem(row, i, item);
    }
    return row;
}

void mainWidget::resizeEvent(QResizeEvent *event) {
}

mainWidget::~mainWidget() {
    deleteLater();
}

void mainWidget::contextMenuEvent(QContextMenuEvent *event) {
}

void mainWidget::setTableContextMenu(QPoint pos) {
    auto *menu = new QMenu(this);
    auto *openFile = new QAction("打开", menu);
    menu->addAction(openFile);
    auto *openInExplorer = new QAction("在资源管理器中显示", menu);
    menu->addAction(openInExplorer);
    connect(openFile, &QAction::triggered, this, [=]() {
        auto item = this->preCompressionTable->itemAt(pos);
        if (item) {
            int row = this->preCompressionTable->rowAt(pos.y());
            QString path = this->preCompressionTable->item(row, 3)->text();
            QDesktopServices::openUrl(QUrl::fromLocalFile(path));
        }
    });
    connect(openInExplorer, &QAction::triggered, this, [=]() {
        if (QTableWidgetItem *item = this->preCompressionTable->itemAt(pos)) {
            int row = this->preCompressionTable->rowAt(pos.y());
            QString path = this->preCompressionTable->item(row, 3)->text();
            QStringList param;
            param << QLatin1String("/select,") << QDir::toNativeSeparators(path);
            QProcess::startDetached("explorer", param);
        }
    });
    menu->exec(this->preCompressionTable->viewport()->mapToGlobal(pos));
}

void mainWidget::setHeaderClickSort(int col) {
    QString label = this->preCompressionTable->horizontalHeaderItem(col)->text();
    if (label == "大小") {
        for (int i = 0; i < this->preCompressionTable->rowCount(); i++) {
            auto *item = new CITableWidgetItem(SortType::Size);
            item->setData(Qt::DisplayRole, QVariant(this->preCompressionTable->item(i, col)->text()));
            this->preCompressionTable->setItem(i, col, item);
        }
    } else if (label == "分辨率") {
        for (int i = 0; i < this->preCompressionTable->rowCount(); i++) {
            auto *item = new CITableWidgetItem(SortType::Resolution);
            item->setData(Qt::DisplayRole, QVariant(this->preCompressionTable->item(i, col)->text()));
            this->preCompressionTable->setItem(i, col, item);
        }
    }
    Qt::SortOrder order = this->sortOrder[col] == Qt::AscendingOrder ? Qt::DescendingOrder : Qt::AscendingOrder;
    this->sortOrder[col] = order;
    this->preCompressionTable->sortItems(col, order);
}

void mainWidget::setPreviewImage(int row, int col) {
    //    this->preCompressionTable->selectRow(row);
    /*压缩前预览图像*/
    QString path = this->preCompressionTable->item(row, 3)->text();
    auto *imageReader = new QImageReader(path);
    imageReader->setAutoTransform(true);
    QPixmap img = QPixmap::fromImageReader(imageReader);
    this->originalImage->showPixmap(img);
    /*压缩后预览图像*/
    QTableWidgetItem *item = this->preCompressionTable->item(row, 5);
    if (item == nullptr || item->text().isEmpty() || item->text().isNull() || item->text() == "") {
        QPixmap img1;
        this->compressedImage->showPixmap(img1);
        return;
    };
    QString path1 = item->text();
    auto *imageReader1 = new QImageReader(path1);
    imageReader1->setAutoTransform(true);
    QPixmap img1 = QPixmap::fromImageReader(imageReader1);
    this->compressedImage->showPixmap(img1);
}

void mainWidget::setTableDoubleClick(int row, int col) {
    QString path = this->preCompressionTable->item(row, 3)->text();
    QDesktopServices::openUrl(QUrl::fromLocalFile(path));
}

void mainWidget::compressImage() {
    if (this->compressionOptions.outputPath.isEmpty() || this->compressionOptions.outputPath.isNull() || this->compressionOptions.outputPath == "") {
        QMessageBox::critical(this, "Error", "请选择输出路径！");
        return;
    }
    QSettings settings("config.ini", QSettings::IniFormat);
    settings.setValue("lastOutputPath", this->compressionOptions.outputPath);
    QAbstractItemModel *model = this->preCompressionTable->model();
    auto *compressionImage = new CompressionImage(model, this->compressionOptions);
    compressionImage->compress();
}

void mainWidget::splitterMoved() {
    //    this->originalImage->fitInView(this->originalImage->scene()->itemsBoundingRect(), Qt::KeepAspectRatio);
    //    this->compressedImage->fitInView(this->compressedImage->scene()->itemsBoundingRect(), Qt::KeepAspectRatio);
}

void mainWidget::setPngQuality(int value) {
    this->compressionOptions.pngQuality = 9 - value;
    this->pngQualitySpinBox->setValue(value);
    this->pngQualitySlider->setValue(value);
}

void mainWidget::setJpegQuality(int value) {
    this->compressionOptions.jpegQuality = value;
    this->jpegQualitySpinBox->setValue(value);
    this->jpegQualitySlider->setValue(value);
}

void mainWidget::setWebpQuality(int value) {
    this->compressionOptions.webpQuality = value;
    this->webpQualitySpinBox->setValue(value);
    this->webpQualitySlider->setValue(value);
}

void mainWidget::onCompressFinished() {
    qout << "finished";
}

void mainWidget::onSizeComboBoxChanged(int index) {
    if (index == 0) {
        sizeMode = SizeModeOptions::PERCENTAGE;
        this->compressionOptions.sizeMode = SizeModeOptions::PERCENTAGE;
        this->resizeHeightSpinBox->setSuffix("%");
        this->resizeHeightSpinBox->setMaximum(100);
        this->resizeHeightSpinBox->setMinimum(1);
        this->resizeHeightSpinBox->setValue(100);
        this->resizeWidthSpinBox->setSuffix("%");
        this->resizeWidthSpinBox->setMaximum(100);
        this->resizeWidthSpinBox->setMinimum(1);
        this->resizeWidthSpinBox->setValue(100);
    } else if (index == 1) {
        sizeMode = SizeModeOptions::PIXELS;
        this->compressionOptions.sizeMode = SizeModeOptions::PIXELS;
        this->resizeHeightSpinBox->setSuffix("px");
        this->resizeHeightSpinBox->setMaximum(99999);
        this->resizeHeightSpinBox->setMinimum(1);
        this->resizeHeightSpinBox->setValue(1000);
        this->resizeWidthSpinBox->setSuffix("px");
        this->resizeWidthSpinBox->setMaximum(99999);
        this->resizeWidthSpinBox->setMinimum(1);
        this->resizeWidthSpinBox->setValue(1000);
    }
}

void mainWidget::onResizeWidthSpinBoxChanged(int value) {
    if (this->keepOriginScaleCheckBox->isChecked())
        this->resizeHeightSpinBox->setValue(value);
    this->compressionOptions.width = value;
}

void mainWidget::onResizeHeightSpinBoxChanged(int value) {
    if (this->keepOriginScaleCheckBox->isChecked())
        this->resizeWidthSpinBox->setValue(value);
    this->compressionOptions.height = value;
}

void mainWidget::onKeepOriginSizeCheckBoxChanged(int value) {
    if (value == 0) {
        compressionOptions.keepOriginalSize = false;
        this->resizeHeightSpinBox->setEnabled(true);
        this->resizeWidthSpinBox->setEnabled(true);
    } else {
        compressionOptions.keepOriginalSize = true;
        this->resizeHeightSpinBox->setEnabled(false);
        this->resizeWidthSpinBox->setEnabled(false);
    }
}

void mainWidget::onKeepOriginScaleCheckBoxChanged(int value) {
    compressionOptions.keepOriginalScale = value != 0;
    this->resizeWidthSpinBox->setValue(this->resizeHeightSpinBox->value());
}
