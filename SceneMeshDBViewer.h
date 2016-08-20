#ifndef SCENEMESHDBVIEWER_H
#define SCENEMESHDBVIEWER_H
#include "GLNavigatable.h"
#include "scene.capnp.h"

#include <QFile>
#include <QMap>
#include <QOpenGLBuffer>
#include <QOpenGLFunctions>

class SceneMeshDBViewer : public GLNavigatable, protected QOpenGLFunctions
{
public:
    SceneMeshDBViewer();
    ~SceneMeshDBViewer();

    void draw() override;

private:
    void drawElementsFallback(cp::scene::AttributeArrayInterleaved::Reader, uint attributeIndex);

    QFile dbFile;
    qint64 size;
    uchar *ptr;

    QMap<int, QOpenGLBuffer> elementArrays_;
    QMap<int, QOpenGLBuffer> indexArrays_;
};

#endif // SCENEMESHDBVIEWER_H
