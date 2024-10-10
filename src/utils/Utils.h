//
// Created by 25415 on 2024/9/20.
//

#ifndef UNTITLED_UTILS_H
#define UNTITLED_UTILS_H

#include <QString>
#include "Utils.h"

enum SortType {
    Size = 0,
    Resolution = 1,
};

enum SizeModeOptions {
    PERCENTAGE = 0,
    PIXELS = 1,
};

typedef struct CompressionOptions {
    QString outputPath;
    QString inputPath;
    QString basePath;
    QString suffix;
    QString fileName;
    SizeModeOptions sizeMode = SizeModeOptions::PERCENTAGE;
    bool isOriginalDirOutput = false;
    bool keepOriginalSize = true;
    bool keepOriginalScale = true;
    bool fitToWidth = false;
    bool fitToHeight = true;
    int jpegQuality;
    int pngQuality;
    int webpQuality;
    int width;
    int height;
} CompressionOptions;
#endif //UNTITLED_UTILS_H
