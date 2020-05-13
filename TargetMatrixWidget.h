#ifndef TARGETMATRIXWIDGET_H
#define TARGETMATRIXWIDGET_H

#include "MatrixWidget.h"

class TargetMatrixWidget : public MatrixWidget
{
public:
  TargetMatrixWidget(QWidget *parent = 0);

private:
  void setClearColor() override;
  QVector3D getEyePosition() override;
};

#endif // TARGETMATRIXWIDGET_H
