#include "TargetMatrixWidget.h"

TargetMatrixWidget::TargetMatrixWidget( QWidget * parent )
    : MatrixWidget(parent)
{}

void TargetMatrixWidget::setClearColor()
{
    functions->glClearColor( 0.0f, 0.0f, 0.1f, 1.0f );
}

QVector3D TargetMatrixWidget::getEyePosition()
{
    return QVector3D( -( grid->getWidth() ), std::min( grid->getWidth(), grid->getHeight() ), grid->getHeight() );
}
