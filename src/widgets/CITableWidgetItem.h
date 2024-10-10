//
// Created by 25415 on 2024/8/12.
//

#ifndef UNTITLED_MYQTABLEWIDGETITEM_H
#define UNTITLED_MYQTABLEWIDGETITEM_H


#include <QTableWidgetItem>
#include "Utils.h"

class CITableWidgetItem : public QTableWidgetItem {
private:
    SortType sortType;

public:
    explicit CITableWidgetItem(SortType sortType);

    bool operator<(const QTableWidgetItem &other) const override;
};


#endif //UNTITLED_MYQTABLEWIDGETITEM_H
