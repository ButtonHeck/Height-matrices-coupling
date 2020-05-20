#pragma once

#include <QMainWindow>
#include <random>

#include <HeightMatrix.h>

namespace Ui {
class AppWindow;
}

class QComboBox;
class MatrixWidget;

/**
 * @brief Program's window representation class, contains ui object, randomizer engine and both master and target matrices
 */
class AppWindow: public QMainWindow
{
    Q_OBJECT
public:
    explicit AppWindow( QWidget * parent = nullptr );
    ~AppWindow();

private slots:
    void on_pushButtonMasterMat_clicked();
    void on_pushButtonTargetMat_clicked();
    void on_comboBoxSide_currentIndexChanged( int sideIndex );
    void on_pushButtonArrange_clicked();
    void arrangeButtonCheckEnabled();

private:
    void initializeMatrixSettingsWidgets( QComboBox * widthComboBox,
                                          QComboBox * heightComboBox,
                                          QComboBox * precisionComboBox );
    void fillMatrix( HeightMatrix & matrix );
    COMPARISON_SIDE getSideForTargetMatrix( COMPARISON_SIDE side );
    void updateMatrixView( MatrixWidget * matrixWidget,
                           const HeightMatrix & MATRIX,
                           COMPARISON_SIDE side );
    void updateProfileView( const HeightMatrix & MATRIX,
                            COMPARISON_SIDE side );

private:
    Ui::AppWindow * ui;
    HeightMatrix masterMatrix;
    HeightMatrix targetMatrix;
    std::default_random_engine randomizer;
};
