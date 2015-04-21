#ifndef SCENEMESHDBVIEWER_H
#define SCENEMESHDBVIEWER_H
#include "GLNavigatable.h"
#include <QMap>
#include <QFile>
#include <QOpenGLBuffer>
#include <QOpenGLFunctions>

class SceneMeshDBViewer
        : public GLNavigatable
        , protected QOpenGLFunctions
{
public:
    SceneMeshDBViewer();
    ~SceneMeshDBViewer();

    void draw() override;

private:

    QFile dbFile;
    qint64 size;
    uchar *ptr;

    QMap<int, QOpenGLBuffer> elementArrays_;
    QMap<int, QOpenGLBuffer> indexArrays_;
};

#endif // SCENEMESHDBVIEWER_H
