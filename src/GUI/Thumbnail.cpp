//
// Created by 温雅 on 2019-05-15.
//

#include "Thumbnail.h"

bool Thumbnail::clickable = false;

Thumbnail::Thumbnail(QWidget* parent, Qt::WindowFlags f)
        : QLabel(parent) {
}

Thumbnail::~Thumbnail() {}

void Thumbnail::mousePressEvent(QMouseEvent* event) {
    if(clickable){
        emit clicked();
    }
}