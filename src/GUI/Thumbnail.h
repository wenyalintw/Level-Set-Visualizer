//
// Created by 温雅 on 2019-05-15.
//

#ifndef LEVELSET_VISUALIZER_THUMBNAIL_H
#define LEVELSET_VISUALIZER_THUMBNAIL_H


#include <QLabel>
#include <QWidget>
#include <Qt>

class Thumbnail : public QLabel {
    Q_OBJECT

public:
    explicit Thumbnail(QWidget* parent = Q_NULLPTR, Qt::WindowFlags f = Qt::WindowFlags());
    ~Thumbnail();

    signals:
            void clicked();

protected:
    void mousePressEvent(QMouseEvent* event);

public:
    // 3 thumbnail share clickable status, use static
    static bool clickable;

};

#endif //LEVELSET_VISUALIZER_THUMBNAIL_H
