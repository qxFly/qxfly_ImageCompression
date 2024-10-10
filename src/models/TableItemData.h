//
// Created by 25415 on 2024/8/3.
//

#ifndef UNTITLED_TABLEITEMDATA_H
#define UNTITLED_TABLEITEMDATA_H


#include <QString>

//
// Created by 25415 on 2024/8/3.
//
class TableItemData {
private:
    QString name;
    long long int size;
    QString resolution;
    QString path;

public:
    TableItemData(const QString &name, long long int size, QString resolution, const QString &path)
        : name(name), size(size), resolution(resolution), path(path) {
    }

    const QString &getName() const {
        return name;
    }

    void setName(const QString &name) {
        TableItemData::name = name;
    }

    const long long int &getSize() const {
        return size;
    }

    void setSize(const long long int &size) {
        TableItemData::size = size;
    }

    const QString &getPath() const {
        return path;
    }

    void setPath(const QString &path) {
        TableItemData::path = path;
    }

    const QString &getResolution() const {
        return resolution;
    }

    void setResolution(const QString &resolution) {
        TableItemData::resolution = resolution;
    }
};


#endif //UNTITLED_TABLEITEMDATA_H
