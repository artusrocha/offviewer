#include "glwidget.h"
#include <iostream>
using namespace std;

QString filename = "" ;

//light
double px=3,py=3,pz=3;
QVector4D lightpos = QVector4D(px, py, pz, 0.0);
QVector4D ambient = QVector4D(0.2, 0.2, 0.2, 1.0);
QVector4D diffuse = QVector4D(1.0, 1.0, 1.0, 1.0);
QVector4D specular = QVector4D(1.0, 1.0, 1.0, 1.0);

//material
QVector4D m_ambient = QVector4D(0.2, 0.2, 0.2, 1.0);
QVector4D m_diffuse = QVector4D(0.8, 0.8, 0.8, 1.0);
QVector4D m_specular = QVector4D(1.0, 1.0, 1.0, 1.0);
float m_shininess = 10000.0;

//camera
QVector3D eye = QVector3D(0.0, 0.0, 1.0);
QVector3D at  = QVector3D(0.0, 0.0, 0.0);
QVector3D up  = QVector3D(0.0, 1.0, 0.0);

GLWidget::GLWidget(QWidget *parent) :
    QGLWidget(parent)
{
    m_shaderProgram = NULL;
    m_vertexShader = NULL;
    m_fragmentShader = NULL;
    m_vboVertices = NULL;
    m_vboNormals = NULL;
    m_vboColors = NULL;
    m_vboIndices = NULL;
    verticefile = NULL;
    indicesfile = NULL;
    normalfile = NULL;
    rotateX = 0;
    rotateY = 0;
    rotateZ = 0;

    QTimer *timer = new QTimer(this);
    connect(timer, SIGNAL(timeout()), this, SLOT(animate()));
    timer->start(0);

}

GLWidget::~GLWidget()
{
    if(m_vertexShader)   delete m_vertexShader;
    if(m_fragmentShader) delete m_fragmentShader;
    if(m_shaderProgram)   delete m_shaderProgram;
    if(m_vboVertices)    delete m_vboVertices;
    if(m_vboIndices)     delete m_vboIndices;
}



void GLWidget::initializeGL()
{
    glEnable(GL_DEPTH_TEST);
    ModelView.setToIdentity();
    ModelView.lookAt(eye,at,up);
    NormalMatrix = ModelView.normalMatrix();
    Projection.setToIdentity();
    Projection.ortho(-2,2,-2,2,-4,4);
    this->initialize_file();
}

void GLWidget::resizeGL(int w, int h)
{
    glViewport(0,0,w,h);
}

void GLWidget::paintGL()
{
    glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
    glClearColor(1,1,1,1);
    ModelView.setToIdentity();
    ModelView.lookAt(eye,at,up);
    ModelView.rotate(rotateX,1,0,0);
    ModelView.rotate(rotateY,0,1,0);
    ModelView.rotate(rotateZ,0,0,1);
    NormalMatrix = ModelView.normalMatrix();
    m_shaderProgram->setUniformValue("Projection", Projection);
    m_shaderProgram->setUniformValue("ModelView", ModelView);
    m_shaderProgram->setUniformValue("NormalMatrix", NormalMatrix);
    run_file();
}

void GLWidget::initialize_file()
{

    this->setshaders(":/shaders/vshader.glsl",":/shaders/fshader.glsl");
    this->file();
    this->build_normal_faces_file();
    QVector4D *flatvertices = new QVector4D[this->n_faces*3];
    QVector3D *flatnormals = new QVector3D[this->n_faces*3];
    for(unsigned int i = 0; i < this->n_faces; i++)
    {
        flatvertices[3*i] = this->verticefile[this->indicesfile[3*i]];
        flatvertices[3*i+1] = this->verticefile[this->indicesfile[3*i+1]];
        flatvertices[3*i+2] = this->verticefile[this->indicesfile[3*i+2]];
        flatnormals[3*i] = this->normalfile[i];
        flatnormals[3*i+1] = this->normalfile[i];
        flatnormals[3*i+2] = this->normalfile[i];
    }

    if(m_vboVertices) delete m_vboVertices;
    m_vboVertices = new QGLBuffer(QGLBuffer::VertexBuffer);
    m_vboVertices->create();
    m_vboVertices->bind();
    m_vboVertices->setUsagePattern(QGLBuffer::StaticDraw);
    m_vboVertices->allocate(flatvertices, this->n_faces*3*sizeof(QVector4D));
    delete []flatvertices;
    flatvertices = NULL;

    if(m_vboNormals) delete m_vboNormals;
    m_vboNormals = new QGLBuffer(QGLBuffer::VertexBuffer);
    m_vboNormals->create();
    m_vboNormals->bind();
    m_vboNormals->setUsagePattern(QGLBuffer::StaticDraw);
    m_vboNormals->allocate(flatnormals, this->n_faces*3*sizeof(QVector3D));
    delete []flatnormals;
    flatnormals = NULL;
    delete []normalfile;
    normalfile = NULL;

    QVector4D ambient_product  = ambient*m_ambient;
    QVector4D diffuse_product  = diffuse*m_diffuse;
    QVector4D specular_product = specular*m_specular;

    m_shaderProgram->setUniformValue("AmbientProduct",ambient_product);
    m_shaderProgram->setUniformValue("DiffuseProduct",diffuse_product);
    m_shaderProgram->setUniformValue("SpecularProduct",specular_product);
    m_shaderProgram->setUniformValue("Shininess",m_shininess);
    m_shaderProgram->setUniformValue("LightPosition", lightpos);

}

void GLWidget::run_file()
{

    glPolygonMode( GL_FRONT_AND_BACK, GL_FILL );
    m_vboVertices->bind();
    m_shaderProgram->setUniformValue("LightPosition", lightpos);//mod
    m_shaderProgram->enableAttributeArray("vPosition");
    m_shaderProgram->setAttributeBuffer("vPosition",GL_FLOAT,0,4,0);
    m_vboNormals->bind();
    m_shaderProgram->enableAttributeArray("vNormal");
    m_shaderProgram->setAttributeBuffer("vNormal",GL_FLOAT,0,3,0);
    glDrawArrays( GL_TRIANGLES, 0, this->n_faces*3 );
    this->m_vboNormals->release();
    this->m_vboVertices->release();
}

void GLWidget::file()
{
    QFile tfinput(filename);
    tfinput.open(QFile::ReadOnly);
    QTextStream finput(&tfinput);
    if (verticefile) delete verticefile;
    if (indicesfile) delete indicesfile;
    QString garb;
    finput >> garb;
    finput >> n_points_file;
    finput >> n_faces;
    finput >> garb;
    verticefile = new QVector4D[n_points_file];
    indicesfile = new unsigned int[3*n_faces];
    float x,y,z;
    for (unsigned int i = 0; i < n_points_file; i++)
    {
        finput >> x >> y >> z;
        verticefile[i] = QVector4D(x,y,z,1);
    }
    unsigned int a, b, c;
    for (unsigned int i = 0; i < n_faces; i++)
    {
        finput >> garb >> a >> b >> c ;
        indicesfile[3*i]   = a;
        indicesfile[3*i+1] = b;
        indicesfile[3*i+2] = c;
    }
    tfinput.close();
}

void GLWidget::build_normal_vertices_file()
{

    if (normalfile) delete []normalfile;
    normalfile = new QVector3D[this->n_points_file];
    for(unsigned int i = 0; i < n_points_file; i++)
        normalfile[i] = this->verticefile[i].toVector3D();
}

void GLWidget::build_normal_faces_file()
{
    if (normalfile) delete []normalfile;
    normalfile = new QVector3D[this->n_faces];
    for(unsigned int i = 0; i < n_faces; i++)
    {

        int i0 = this->indicesfile[3*i];
        int i1 = this->indicesfile[3*i+1];
        int i2 = this->indicesfile[3*i+2];

        QVector3D v1 = (this->verticefile[i1] - this->verticefile[i0]).toVector3D();
        QVector3D v2 = (this->verticefile[i2] - this->verticefile[i0]).toVector3D();

        normalfile[i] = QVector3D::crossProduct(v1,v2).normalized();
    }
}

void GLWidget::setshaders(std::string const &s1, std::string const &s2)
{
    m_shaderProgram->release();
    if (m_vertexShader)   delete m_vertexShader;
    if (m_fragmentShader) delete m_fragmentShader;
    m_vertexShader = new QGLShader(QGLShader::Vertex);
    m_fragmentShader = new QGLShader(QGLShader::Fragment);
    if(!m_vertexShader->compileSourceFile(s1.c_str()))
        qWarning() << m_vertexShader->log();
    if(!m_fragmentShader->compileSourceFile(s2.c_str()))
        qWarning() << m_fragmentShader->log();
    if(m_shaderProgram) delete m_shaderProgram;
    m_shaderProgram = new QGLShaderProgram;
    m_shaderProgram->addShader(m_vertexShader);
    m_shaderProgram->addShader(m_fragmentShader);
    if(!m_shaderProgram->link())
        qWarning() << m_shaderProgram->log() << endl;
    else
        m_shaderProgram->bind();
}

void GLWidget::animate()
{
    rotateX = (rotateX<360 ? rotateX+X : 360-rotateX);
    rotateY = (rotateY<360 ? rotateY+Y : 360-rotateY);
    rotateZ = (rotateZ<360 ? rotateZ+Z : 360-rotateZ);
    updateGL();
}

void GLWidget::letsgo()
{
    QString path = QFileDialog::getOpenFileName(this, tr("Open File"), "", tr("Arquivo vetorial OFF (*.off)"));
    filename = path;
    initialize_file();
//    updateGL(); // not necessary, because the QTimer run a updateGL() in animate()
}

void GLWidget::setX(double x)
{
    X = x;
}

void GLWidget::setY(double y)
{
    Y = y;
}

void GLWidget::setZ(double z)
{
    Z = z;
}

void GLWidget::mvlightxy(int x)
{
    if (x > 0 )
    {
        if (x < 90)
        {
            px = (3.0/90)*x;
            py = 3.0-px;
        }
        else
        {
            py = (3.0/90)*((x-90)*-1);
            px = 3.0-(py*-1);
        }
    }
    else
    {
        if (x > -90)
        {
            px = (3.0/90)*((x*-1)*-1);
            py = 3.0+px;
        }
        else
        {
            py = (3.0/90)*(((x*-1)-90)*-1);
            px = -3.0-py;
        }

    }
    lightpos = QVector4D(px, py, pz, 0.0);
    updateGL();
}

void GLWidget::mvlightxz(int x)
{
    if (x > 0 )
    {
        if (x < 90)
        {
            px = (3.0/90)*x;
            pz = 3.0-px;
        }
        else
        {
            pz = (3.0/90)*((x-90)*-1);
            px = 3.0-(pz*-1);
        }
    }
    else
    {
        if (x > -90)
        {
            px = (3.0/90)*((x*-1)*-1);
            pz = 3.0+px;
        }
        else
        {
            pz = (3.0/90)*(((x*-1)-90)*-1);
            px = -3.0-pz;
        }
    }
    lightpos = QVector4D(px, py, pz, 0.0);
    updateGL();
}

void GLWidget::giroy(int y)
{
    rotateY = y;
    updateGL();
}

void GLWidget::girox(int x)
{
    rotateX = x ;
    updateGL();
}
