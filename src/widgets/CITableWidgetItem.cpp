//
// Created by 25415 on 2024/8/12.
//

#include "CITableWidgetItem.h"
#include "Utils.h"

CITableWidgetItem::CITableWidgetItem(SortType sortType) {
    this->sortType = sortType;
}

bool CITableWidgetItem::operator<(const QTableWidgetItem &other) const {
    QString str1 = data(0).toString();
    QString str2 = other.data(0).toString();
    if (str1.isEmpty() || str2.isEmpty()) return true;
    if (sortType == SortType::Size) {
        double a = str1.chopped(3).toDouble();
        double b = str2.chopped(3).toDouble();
        return a > b;
    }
    if (sortType == SortType::Resolution) {
        QList<QString> list1 = str1.split("x");
        QList<QString> list2 = str2.split("x");
        long a = list1[0].toInt() * list1[1].toInt();
        long b = list2[0].toInt() * list2[1].toInt();
        return a > b;
    }
    return QTableWidgetItem::operator<(other);
}
