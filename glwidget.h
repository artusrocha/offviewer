#ifndef GLWIDGET_H
#define GLWIDGET_H

#include <QGLWidget>
#include <string>
#include <iostream>
#include <QTimer>
#include <QGLShader>
#include <QGLShaderProgram>
#include <QGLBuffer>
#include <QMessageBox>
#include <qmath.h>
#include <QVector4D>
#include <QKeyEvent>
#include <iostream>
#include <QMatrix4x4>
#include <fstream>
#include <QTextStream>
#include <QFile>
#include <QFileDialog>
#include <QResource>


class GLWidget : public QGLWidget
{
    Q_OBJECT
public:
    explicit GLWidget(QWidget *parent = 0);
    ~GLWidget();
    
signals:
    
public slots:
    void animate();
    void setX(double x);
    void setY(double y);
    void setZ(double x);
    void letsgo();
    void mvlightxy(int x);
    void mvlightxz(int x);
    void giroy(int y);
    void girox(int x);

protected:
    void paintGL();
    void initializeGL();
    void resizeGL(int w, int h);
    void initshaders();
    void createScene();

    QGLShader *m_vertexShader;
    QGLShader *m_fragmentShader;
    QGLShaderProgram *m_shaderProgram;

    QGLBuffer *m_vboVertices;
    QGLBuffer *m_vboNormals;
    QGLBuffer *m_vboColors;
    QGLBuffer *m_vboIndices;

    void setshaders(std::string const &s1, std::string const & s2);
    void file();
    void build_normal_vertices_file();
    void build_normal_faces_file();
    QVector4D *verticefile;
    QVector3D *normalfile;
    unsigned int *indicesfile, n_points_file, n_faces;
    QMatrix4x4  Projection, ModelView;
    QMatrix3x3 NormalMatrix;
    void initialize_file();
    void run_file(void);
    double rotateX;
    double rotateY;
    double rotateZ;
    double X, Y, Z;
};

#endif // GLWIDGET_H
