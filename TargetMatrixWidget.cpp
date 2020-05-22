#include "TargetMatrixWidget.h"

TargetMatrixWidget::TargetMatrixWidget( QWidget * parent )
    : MatrixWidget(parent)
{
    eyePosition.setX( eyePosition.x() * -1 );
}

void TargetMatrixWidget::setClearColor()
{
    functions.glClearColor( 0.0f, 0.0f, 0.1f, 1.0f );
}
