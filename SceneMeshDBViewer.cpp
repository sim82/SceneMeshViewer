#include "SceneMeshDBViewer.h"
#include "capnp/serialize.h"
#include "scene.capnp.h"
#include <iostream>

SceneMeshDBViewer::SceneMeshDBViewer()
    : dbFile("/tmp/shadermesh.capnp")
{
    QOpenGLFunctions::initializeOpenGLFunctions();
    dbFile.open(QFile::ReadOnly);

    size = dbFile.size();
    ptr  = dbFile.map(0, size);
}

SceneMeshDBViewer::~SceneMeshDBViewer() {}

void SceneMeshDBViewer::draw()
{
    glFrontFace(GL_CW);

    capnp::ReaderOptions ro;
    ro.traversalLimitInWords = 1024 * 1024 * 1024;
    capnp::FlatArrayMessageReader reader(kj::ArrayPtr<capnp::word>((capnp::word *)ptr, size / sizeof(capnp::word)), ro);

    cp::scene::ShaderMeshDB::Reader dbReader = reader.getRoot<cp::scene::ShaderMeshDB>();

    for (int i = 0; i < dbReader.getMeshes().size(); ++i)
    {
        cp::scene::ShaderMesh::Reader shadermeshReader = dbReader.getMeshes()[i];

        if (!shadermeshReader.hasLattice())
        {
            continue;
        }

        capnp::List<cp::scene::ShaderMesh::Lattice::Mesh>::Reader meshlistReader =
            shadermeshReader.getLattice().getMeshes();

        if (meshlistReader.size() == 0)
        {
            continue;
        }
        glEnableClientState(GL_INDEX_ARRAY);
        glEnableClientState(GL_VERTEX_ARRAY);
        for (int k = 0; k < meshlistReader.size(); ++k)
        {
            cp::scene::AttributeArrayInterleaved::Reader arrayReader = meshlistReader[k].getArray();
            capnp::List<cp::scene::AttributeArrayInterleaved::Attribute>::Reader attributeList =
                arrayReader.getAttributes();

            bool breakOuter = false;
            for (int j = 0; j < attributeList.size(); ++j)
            {
                cp::scene::AttributeArrayInterleaved::Attribute::Reader attribute = attributeList[j];
                if (attribute.getName() == "a_position")
                {
                    uint32_t stride   = arrayReader.getAttributeStride();
                    uint32_t offset   = attribute.getOffset();
                    uint32_t numIndex = arrayReader.getNumIndex();

                    int meshkey = i + k * 1000000;

                    QOpenGLBuffer vertexBuffer;
                    QOpenGLBuffer indexBuffer;
                    {

                        auto it = elementArrays_.find(meshkey);

                        if (it == elementArrays_.end())
                        {
                            QOpenGLBuffer buffer(QOpenGLBuffer::VertexBuffer);
                            buffer.create();
                            buffer.bind();

                            capnp::Data::Reader attributearrayReader = arrayReader.getAttributeArray();
                            //auto buf = (float const*)attributearrayReader.begin();
                            buffer.allocate(attributearrayReader.begin(), attributearrayReader.size());

                            elementArrays_[meshkey] = buffer;
                            vertexBuffer            = buffer;
                        }
                        else
                        {
                            vertexBuffer = it.value();
                            vertexBuffer.bind();
                        }
                    }

                   {
                        auto it = indexArrays_.find(meshkey);

                        if (it == indexArrays_.end())
                        {
                            QOpenGLBuffer buffer(QOpenGLBuffer::IndexBuffer);
                            buffer.create();
                            buffer.bind();

                            capnp::Data::Reader indexarrayReader = arrayReader.getIndexArray();
                            auto array = (uint const*) indexarrayReader.begin();
                            auto size = indexarrayReader.size();
                            buffer.allocate(indexarrayReader.begin(), indexarrayReader.size());
                            indexArrays_[meshkey] = buffer;
                        }
                        else
                        {
                            indexBuffer = it.value();
                            indexBuffer.bind();
                        }
                    }

                    {
                        // glPolygonMode( GL_FRONT_AND_BACK, GL_LINE );
                        // glPolygonMode( GL_FRONT, GL_LINE );
                        glCullFace(GL_BACK);
                        glEnable(GL_CULL_FACE);
                        //glDisable(GL_CULL_FACE);

                        glEnableVertexAttribArray(0);
                        glVertexAttribPointer(
                            0, // attribute 0. No particular reason for 0, but must match the layout in the shader.
                            3, // size
                            GL_FLOAT,      // type
                            GL_FALSE,      // normalized?
                            stride,        // stride
                            (void *)offset // array buffer offset
                            );

                        //glIndexPointer(GL_INT, 0, (GLvoid*)nullptr);
                        {
                            qsrand(meshkey);

                            glColor3f(0.2 + 0.2 * (qrand() / float(RAND_MAX)), qrand() / float(RAND_MAX),
                                      0.2 + 0.2 * (qrand() / float(RAND_MAX)));


                            //glColor3f( 0.2, 1.0, 0.2 );
                            glPolygonMode(GL_FRONT, GL_LINE);

                            //glDrawElements(GL_TRIANGLES, numIndex, GL_UNSIGNED_INT, (GLvoid *)nullptr);
                            capnp::Data::Reader indexarrayReader = arrayReader.getIndexArray();
                            //glBindBuffer(GL_ARRAY_BUFFER, 0);
                            glDrawElements(GL_TRIANGLES, numIndex, GL_UNSIGNED_INT, (GLvoid *)nullptr);
                        }

                        if(/* DISABLES CODE */ (false))
                        {
                            glEnable(GL_POLYGON_OFFSET_FILL);
                            glPolygonOffset(1.0, 1.0);
                            glColor3f(0, 0, 0);
                            glPolygonMode(GL_FRONT, GL_FILL);
                            glDrawElements(GL_TRIANGLES, numIndex, GL_UNSIGNED_INT, (GLvoid *)nullptr);
                            glDisable(GL_POLYGON_OFFSET_FILL);
                        }
                        glDisableVertexAttribArray(0);
                    }

                    breakOuter = true;
                    break;
                }
            }
            if (breakOuter)
            {
                // break;
            }
        }
    }
    if (glGetError() != GL_NO_ERROR)
    {
        std::cout << "meeeeeeeeep\n";
    }
}
