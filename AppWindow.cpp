#include "AppWindow.h"
#include "ui_AppWindow.h"

AppWindow::AppWindow(QWidget *parent) :
  QMainWindow(parent),
  ui(new Ui::AppWindow),
  masterMatrix(0, 0, 1),
  targetMatrix(0, 0, 1)
{
  randomizer.seed(QTime::currentTime().msec());
  ui->setupUi(this);

  //setting up master matrix parameters
  ui->comboBoxMasterMatW->addItem("10");
  ui->comboBoxMasterMatW->addItem("20");
  ui->comboBoxMasterMatW->addItem("30");
  ui->comboBoxMasterMatW->setCurrentIndex(1);
  ui->comboBoxMasterMatH->addItem("10");
  ui->comboBoxMasterMatH->addItem("20");
  ui->comboBoxMasterMatH->addItem("30");
  ui->comboBoxMasterMatH->setCurrentIndex(1);
  ui->comboBoxMasterMatPrec->addItem("1");
  ui->comboBoxMasterMatPrec->addItem("2");
  ui->comboBoxMasterMatPrec->addItem("4");
  ui->comboBoxMasterMatPrec->setCurrentIndex(1);

  //setting up target matrix parameters
  ui->comboBoxTargetMatW->addItem("10");
  ui->comboBoxTargetMatW->addItem("20");
  ui->comboBoxTargetMatW->addItem("30");
  ui->comboBoxTargetMatW->setCurrentIndex(1);
  ui->comboBoxTargetMatH->addItem("10");
  ui->comboBoxTargetMatH->addItem("20");
  ui->comboBoxTargetMatH->addItem("30");
  ui->comboBoxTargetMatH->setCurrentIndex(1);
  ui->comboBoxTargetMatPrec->addItem("1");
  ui->comboBoxTargetMatPrec->addItem("2");
  ui->comboBoxTargetMatPrec->addItem("4");
  ui->comboBoxTargetMatPrec->setCurrentIndex(1);

  //setting up side selector
  ui->comboBoxSide->addItem("Left", SIDE::LEFT);
  ui->comboBoxSide->addItem("Right", SIDE::RIGHT);
  ui->comboBoxSide->addItem("Top", SIDE::TOP);
  ui->comboBoxSide->addItem("Bottom", SIDE::BOTTOM);
  ui->comboBoxSide->setCurrentIndex(1);

  //explicitly set arrangement condition that target matrix should not be less precised than the master matrix
  connect(ui->comboBoxMasterMatPrec, SIGNAL(currentIndexChanged(int)), this, SLOT(arrangeButtonCheckEnabled()));
  connect(ui->comboBoxTargetMatPrec, SIGNAL(currentIndexChanged(int)), this, SLOT(arrangeButtonCheckEnabled()));
}

AppWindow::~AppWindow()
{
  delete ui;
}

void AppWindow::fillMatrix(HeightMatrix &matrix)
{
  std::uniform_real_distribution<float> heightDistribution(0.0f, HeightMatrix::MAX_HEIGHT);
  for (HeightMatrix::ColumnIterator columnIter = matrix.columnBegin(0); columnIter.isValid(); columnIter++)
    {
      for (HeightMatrix::RowIterator rowIter = matrix.rowBegin(columnIter.getCurrentIndex()); rowIter.isValid(); rowIter++)
        *rowIter = heightDistribution(randomizer);
    }
}

int AppWindow::getSideForTargetMatrix(int side)
{
  if (side == SIDE::RIGHT)
    side = SIDE::LEFT;
  else if (side == SIDE::LEFT)
    side = SIDE::RIGHT;

  if (side == SIDE::TOP)
    side = SIDE::BOTTOM;
  else if (side == SIDE::BOTTOM)
    side = SIDE::TOP;

  return side;
}

void AppWindow::on_pushButtonMasterMat_clicked()
{
  size_t width = ui->comboBoxMasterMatW->currentText().toInt();
  size_t height = ui->comboBoxMasterMatH->currentText().toInt();
  double precision = ui->comboBoxMasterMatPrec->currentText().toDouble();
  masterMatrix = HeightMatrix(width, height, precision);
  fillMatrix(masterMatrix);
  int side = ui->comboBoxSide->currentIndex();

  //update 3D representation
  ui->OGLWidgetMasterMat->makeCurrent();
  ui->OGLWidgetMasterMat->updateMatrixData(masterMatrix, side);

  //update profile view
  ui->OGLWidgetProfileView->makeCurrent();
  ui->OGLWidgetProfileView->updateProfileBuffer(masterMatrix, side, MATRIX_TYPE::MASTER);

  update();
}

void AppWindow::on_pushButtonTargetMat_clicked()
{
  size_t width = ui->comboBoxTargetMatW->currentText().toInt();
  size_t height = ui->comboBoxTargetMatH->currentText().toInt();
  double precision = ui->comboBoxTargetMatPrec->currentText().toDouble();
  targetMatrix = HeightMatrix(width, height, precision);
  fillMatrix(targetMatrix);
  int side = getSideForTargetMatrix(ui->comboBoxSide->currentIndex());

  //update 3D representation
  ui->OGLWidgetTargetMat->makeCurrent();
  ui->OGLWidgetTargetMat->updateMatrixData(targetMatrix, side);

  //update profile view
  ui->OGLWidgetProfileView->makeCurrent();
  ui->OGLWidgetProfileView->updateProfileBuffer(targetMatrix, side, MATRIX_TYPE::TARGET);

  update();
}

void AppWindow::on_checkBoxMasterShowGrid_stateChanged(int stateIsOn)
{
  ui->OGLWidgetMasterMat->makeCurrent();
  ui->OGLWidgetMasterMat->setShowGrid(stateIsOn);
  update();
}

void AppWindow::on_checkBoxTargetShowGrid_stateChanged(int stateIsOn)
{
  ui->OGLWidgetTargetMat->makeCurrent();
  ui->OGLWidgetTargetMat->setShowGrid(stateIsOn);
  update();
}

void AppWindow::on_comboBoxSide_currentIndexChanged(int side)
{
  //update 3D represenation for master matrix
  ui->OGLWidgetMasterMat->makeCurrent();
  ui->OGLWidgetMasterMat->updateMatrixData(masterMatrix, side);

  //update 3D representation for target matrix
  int targetSide = getSideForTargetMatrix(side);
  ui->OGLWidgetTargetMat->makeCurrent();
  ui->OGLWidgetTargetMat->updateMatrixData(targetMatrix, targetSide);

  //update profiles view
  ui->OGLWidgetProfileView->makeCurrent();
  ui->OGLWidgetProfileView->updateProfileBuffer(masterMatrix, side, MATRIX_TYPE::MASTER);
  ui->OGLWidgetProfileView->updateProfileBuffer(targetMatrix, targetSide, MATRIX_TYPE::TARGET);

  update();
}

void AppWindow::on_pushButtonArrange_clicked()
{
  if (masterMatrix.getWidth() == 0 || targetMatrix.getWidth() == 0)
    {
      QMessageBox::warning(this, "Warning", "Create matrices first");
      return;
    }

  //update arrangement view
  int masterSide = ui->comboBoxSide->currentIndex();
  int targetSide = getSideForTargetMatrix(masterSide);
  ui->OGLWidgetArrangementView->makeCurrent();
  ui->OGLWidgetArrangementView->updateProfilesData(masterMatrix, targetMatrix, masterSide, targetSide);

  //update 3D representation of target matrix after arrangement applied
  ui->OGLWidgetTargetMat->makeCurrent();
  ui->OGLWidgetTargetMat->updateMatrixData(targetMatrix, targetSide);

  update();
}

void AppWindow::arrangeButtonCheckEnabled()
{
  double masterMatrixPrecision = ui->comboBoxMasterMatPrec->currentText().toDouble();
  double targetMatrixPrecision = ui->comboBoxTargetMatPrec->currentText().toDouble();
  ui->pushButtonArrange->setEnabled(masterMatrixPrecision >= targetMatrixPrecision);
}
