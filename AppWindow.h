#ifndef APPWINDOW_H
#define APPWINDOW_H

#include <QMainWindow>
#include <QTime>
#include <QOpenGLWidget>
#include <QMessageBox>
#include <HeightMatrix.h>
#include <random>

namespace Ui {
  class AppWindow;
}

class AppWindow : public QMainWindow
{
  Q_OBJECT

public:
  explicit AppWindow(QWidget *parent = nullptr);
  ~AppWindow();

private slots:
  void on_pushButtonMasterMat_clicked();
  void on_pushButtonTargetMat_clicked();
  void on_checkBoxMasterShowGrid_stateChanged(int stateIsOn);
  void on_checkBoxTargetShowGrid_stateChanged(int stateIsOn);
  void on_comboBoxSide_currentIndexChanged(int side);
  void on_pushButtonArrange_clicked();
  void arrangeButtonCheckEnabled();

private:
  void fillMatrix(HeightMatrix& matrix);
  int getSideForTargetMatrix(int side);

  Ui::AppWindow *ui;
  HeightMatrix masterMatrix;
  HeightMatrix targetMatrix;
  std::default_random_engine randomizer;
};

#endif // APPWINDOW_H
