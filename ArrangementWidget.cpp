#include "ArrangementWidget.h"

ArrangementWidget::ArrangementWidget(QWidget *parent)
    :
      QOpenGLWidget(parent),
      functions(),
      vboDataValid(false),
      projectionHorizontalDistance(0)
{}

ArrangementWidget::~ArrangementWidget()
{
    functions.glDeleteBuffers(1, &vbo);
}

void ArrangementWidget::updateProfilesData(const HeightMatrix &masterMatrix, HeightMatrix &targetMatrix, SIDE masterSide, SIDE targetSide)
{
    //first update "unarranged" line data
    updateSourceProfile(targetMatrix, targetSide);

    //then arrange target line segment with adjacent segment of master line
    updateArrangedProfile(masterMatrix, targetMatrix, masterSide);

    //make sure both source and arranged profiles are the same size
    adjustSourceAndArrangedProfiles();

    //renew target matrix comparison line
    updateTargetMatrix(targetMatrix, targetSide);

    //add both source and processed lines data to one storage used by VBO during rendering
    mergeSourceAndArrangedVertices();

    //set validation flag to false to signal that VBO data should be updated before rendering
    vboDataValid = false;
}

void ArrangementWidget::initializeGL()
{
    functions.initializeOpenGLFunctions();
    functions.glGenBuffers(1, &vbo);
    functions.glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

    QOpenGLShader vGridShader(QOpenGLShader::Vertex);
    vGridShader.compileSourceFile(":/Shaders/grid/vGrid.glsl");
    QOpenGLShader fGridShader(QOpenGLShader::Fragment);
    fGridShader.compileSourceFile(":/Shaders/grid/fGrid.glsl");
    shader.addShader(&vGridShader);
    shader.addShader(&fGridShader);
    if (!shader.link())
        qWarning("Unable to link grid shader program");
}

void ArrangementWidget::paintGL()
{
    functions.glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    if (!shader.bind())
        return;

    //check whether vbo data is outdated
    if (!vboDataValid)
    {
        bufferToVBO();
        vboDataValid = true;
    }

    //update projection matrix
    QMatrix4x4 projectionMatrix;
    projectionMatrix.ortho(0.0f, projectionHorizontalDistance, 0.0f, HeightMatrix::MAX_HEIGHT, 0.1f, 10.0f);
    shader.setUniformValue(shader.uniformLocation("u_projection"), projectionMatrix);

    //update view matrix
    QMatrix4x4 viewMatrix;
    viewMatrix.lookAt(QVector3D(0.0f, 0.0f, 1.0f), QVector3D(0.0f, 0.0f, 0.0f), QVector3D(0.0f, 1.0f, 0.0f));
    shader.setUniformValue(shader.uniformLocation("u_view"), viewMatrix);

    //render source comparison line (before arrangement is applied)
    shader.setUniformValue(shader.uniformLocation("u_color"), QVector4D(0.0f, 0.0f, 1.0f, 1.0f));
    functions.glBindBuffer(GL_ARRAY_BUFFER, vbo);
    functions.glEnableVertexAttribArray(0);
    functions.glDrawArrays(GL_LINE_STRIP, 0, profileVerticesCount);
    functions.glEnable(GL_PROGRAM_POINT_SIZE);
    functions.glDrawArrays(GL_POINTS, 0, profileVerticesCount);

    //render comparison line with arrangement applied
    shader.setUniformValue(shader.uniformLocation("u_color"), QVector4D(1.0f, 0.0f, 1.0f, 1.0f));
    functions.glDrawArrays(GL_LINE_STRIP, profileVerticesCount, profileVerticesCount);
    functions.glDrawArrays(GL_POINTS, profileVerticesCount, profileVerticesCount);

    functions.glDisableVertexAttribArray(0);
    functions.glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void ArrangementWidget::resizeGL(int w, int h)
{
    functions.glViewport(0, 0, w, h);
}

void ArrangementWidget::updateSourceProfile(const HeightMatrix& matrix, SIDE side)
{
    profileVerticesCount = 0;
    sourceProfileVertices.clear();

    if (side == SIDE::LEFT || side == SIDE::RIGHT)
    {
        HeightMatrix::ConstColumnIterator column = (side == SIDE::LEFT) ? matrix.columnBegin(0) : matrix.columnBegin(matrix.getWidth() - 1);
        for (; column.isValid(); column++)
        {
            sourceProfileVertices.emplace_back(column.getCurrentIndex());
            sourceProfileVertices.emplace_back(*column);
            profileVerticesCount++;
        }
        projectionHorizontalDistance = matrix.getHeight();
    }
    else
    {
        HeightMatrix::ConstRowIterator row = (side == SIDE::TOP) ? matrix.rowBegin(0) : matrix.rowBegin(matrix.getHeight() - 1);
        for (; row.isValid(); row++)
        {
            sourceProfileVertices.emplace_back(row.getCurrentIndex());
            sourceProfileVertices.emplace_back(*row);
            profileVerticesCount++;
        }
        projectionHorizontalDistance = matrix.getWidth();
    }
}

void ArrangementWidget::updateArrangedProfile(const HeightMatrix &mMatrix, HeightMatrix &tMatrix, SIDE mSide)
{
    arrangedProfileVertices.clear();
    float mPrecision = (float)mMatrix.getPrecision();
    float tPrecision = (float)tMatrix.getPrecision();
    unsigned int lerpSteps = (int)(mPrecision / tPrecision);

    //comparing LEFT side of master matrix with RIGHT side of target matrix or vice versa
    if (mSide == SIDE::LEFT || mSide == SIDE::RIGHT)
    {
        HeightMatrix::ConstColumnIterator mColumn = (mSide == SIDE::LEFT) ? mMatrix.columnBegin(0) : mMatrix.columnBegin(mMatrix.getWidth() - 1);
        size_t mHeight = mMatrix.getHeight();

        //fill storage for target matrix arranged side
        for (size_t mLineIndex = 0; mLineIndex < mHeight - 1; mLineIndex++)
        {
            float mHeightAtThisIndex = *mColumn;
            mColumn++;
            float mHeightAtNextIndex = *mColumn;
            createInterpolants(mHeightAtThisIndex, mHeightAtNextIndex, lerpSteps, mLineIndex * lerpSteps);
        }

        //add master side last vertex explicitly
        arrangedProfileVertices.emplace_back((mHeight - 1) * lerpSteps);
        arrangedProfileVertices.emplace_back(*mColumn);
    }

    //comparing TOP side of master matrix with BOTTOM side of target matrix or vice versa
    else
    {
        HeightMatrix::ConstRowIterator mRow = (mSide == SIDE::TOP) ? mMatrix.rowBegin(0) : mMatrix.rowBegin(mMatrix.getHeight() - 1);
        size_t mWidth = mMatrix.getWidth();

        //fill storage for target matrix arranged side
        for (size_t mLineIndex = 0; mLineIndex < mWidth - 1; mLineIndex++)
        {
            float mHeightAtThisIndex = *mRow;
            mRow++;
            float mHeightAtNextIndex = *mRow;
            createInterpolants(mHeightAtThisIndex, mHeightAtNextIndex, lerpSteps, mLineIndex * lerpSteps);
        }

        //add master side last vertex explicitly
        arrangedProfileVertices.emplace_back((mWidth - 1) * lerpSteps);
        arrangedProfileVertices.emplace_back(*mRow);
    }
}

void ArrangementWidget::mergeSourceAndArrangedVertices()
{
    profilesVertices.clear();
    profilesVertices.reserve(sourceProfileVertices.size() + arrangedProfileVertices.size());
    profilesVertices.insert(profilesVertices.end(), sourceProfileVertices.begin(), sourceProfileVertices.end());
    profilesVertices.insert(profilesVertices.end(), arrangedProfileVertices.begin(), arrangedProfileVertices.end());
}

void ArrangementWidget::bufferToVBO()
{
    functions.glBindBuffer(GL_ARRAY_BUFFER, vbo);
    functions.glBufferData(GL_ARRAY_BUFFER, profilesVertices.size() * sizeof(float), profilesVertices.data(), GL_STATIC_DRAW);
    functions.glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 0, 0);
    functions.glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void ArrangementWidget::adjustSourceAndArrangedProfiles()
{
    if (arrangedProfileVertices.size() >= sourceProfileVertices.size())
        arrangedProfileVertices.resize(sourceProfileVertices.size());
    else
        arrangedProfileVertices.insert(arrangedProfileVertices.end(),
                                       sourceProfileVertices.begin() + arrangedProfileVertices.size(),
                                       sourceProfileVertices.end());
}

void ArrangementWidget::createInterpolants(float value1, float value2, unsigned int steps, size_t x)
{
    float stepDistance = 1.0f / steps;
    for (size_t step = 0; step < steps; step++)
    {
        float interpolation = stepDistance * step;
        float tHeight = (1.0f - interpolation) * value1
                        +
                        interpolation * value2;
        arrangedProfileVertices.emplace_back(x + step); //x
        arrangedProfileVertices.emplace_back(tHeight);  //y
    }
}

void ArrangementWidget::updateTargetMatrix(HeightMatrix& matrix, SIDE side)
{
    if (side == SIDE::LEFT || side == SIDE::RIGHT)
    {
        HeightMatrix::ColumnIterator column = (side == SIDE::LEFT) ? matrix.columnBegin(0) : matrix.columnBegin(matrix.getWidth() - 1);
        //all odd indices for each pair in profile vector represents point height, thus +1 is applied
        for (; column.isValid(); column++)
            *column = arrangedProfileVertices[column.getCurrentIndex() * 2 + 1];
    }
    else
    {
        HeightMatrix::RowIterator row = (side == SIDE::TOP) ? matrix.rowBegin(0) : matrix.rowBegin(matrix.getHeight() - 1);
        //all odd indices for each pair in profile vector represents point height, thus +1 is applied
        for (; row.isValid(); row++)
            *row = arrangedProfileVertices[row.getCurrentIndex() * 2 + 1];
    }
}
