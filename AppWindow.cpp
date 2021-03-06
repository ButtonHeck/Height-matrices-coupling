#include "AppWindow.h"
#include "ui_AppWindow.h"

#include <QMessageBox>
#include <QTime>

AppWindow::AppWindow( QWidget * parent )
    : QMainWindow(parent)
    , ui( new Ui::AppWindow )
    , masterMatrix( 0, 0, 1, HeightMatrix::MASTER )
    , targetMatrix( 0, 0, 1, HeightMatrix::TARGET )
{
    //initialize ui and randomizer
    randomizer.seed( QTime::currentTime().msec() );
    ui->setupUi(this);

    //initialize master and target matrices settings widgets (width, height, precision)
    initializeMatrixSettingsWidgets( ui->comboBoxMasterMatW, ui->comboBoxMasterMatH, ui->comboBoxMasterMatPrec );
    initializeMatrixSettingsWidgets( ui->comboBoxTargetMatW, ui->comboBoxTargetMatH, ui->comboBoxTargetMatPrec );
    //setting up side selector
    ui->comboBoxSide->addItem( "Left", (int)COMPARISON_SIDE::LEFT );
    ui->comboBoxSide->addItem( "Right", (int)COMPARISON_SIDE::RIGHT );
    ui->comboBoxSide->addItem( "Top", (int)COMPARISON_SIDE::TOP );
    ui->comboBoxSide->addItem( "Bottom", (int)COMPARISON_SIDE::BOTTOM );
    ui->comboBoxSide->setCurrentIndex(1);

    //grid visibility
    connect( ui->checkBoxMasterShowGrid, SIGNAL( toggled(bool) ), ui->OGL_MasterMatWidget, SLOT( setShowFlatGrid(bool) ) );
    connect( ui->checkBoxTargetShowGrid, SIGNAL( toggled(bool) ), ui->OGL_TargetMatWidget, SLOT( setShowFlatGrid(bool) ) );
}

AppWindow::~AppWindow()
{
    delete ui;
}

/**
 * @brief fills given matrix with randomized height values
 * @param matrix matrix to fill
 */
void AppWindow::fillMatrix( HeightMatrix & matrix )
{
    std::uniform_real_distribution<qreal> heightDistribution( 0.0f, HeightMatrix::MAX_HEIGHT );
    for ( HeightMatrix::ColumnIterator columnIter = matrix.columnBegin(0); columnIter.isValid(); columnIter++ )
    {
        for ( HeightMatrix::RowIterator rowIter = matrix.rowBegin( columnIter.getCurrentIndex() ); rowIter.isValid(); rowIter++ )
        {
            *rowIter = heightDistribution(randomizer);
        }
    }
}

/**
 * @brief calculates corresponding side for target matrix based on a side for a master matrix
 * @param side master matrix' side to couple with
 * @return target corresponding side
 */
COMPARISON_SIDE AppWindow::getSideForTargetMatrix( COMPARISON_SIDE side )
{
    if ( side == COMPARISON_SIDE::RIGHT )
    {
        side = COMPARISON_SIDE::LEFT;
    }
    else if ( side == COMPARISON_SIDE::LEFT )
    {
        side = COMPARISON_SIDE::RIGHT;
    }
    else if ( side == COMPARISON_SIDE::TOP )
    {
        side = COMPARISON_SIDE::BOTTOM;
    }
    else if ( side == COMPARISON_SIDE::BOTTOM )
    {
        side = COMPARISON_SIDE::TOP;
    }
    return side;
}

/**
 * @brief creates master matrix and updates its view widget
 */
void AppWindow::on_pushButtonMasterMat_clicked()
{
    size_t width = ui->comboBoxMasterMatW->currentText().toInt();
    size_t height = ui->comboBoxMasterMatH->currentText().toInt();
    double precision = ui->comboBoxMasterMatPrec->itemData( ui->comboBoxMasterMatPrec->currentIndex() ).toDouble();
    masterMatrix = HeightMatrix( width, height, precision, HeightMatrix::MASTER );
    fillMatrix(masterMatrix);
    COMPARISON_SIDE side = HeightMatrix::sideFrom( ui->comboBoxSide->currentIndex() );

    //update 3D representation
    updateMatrixView( ui->OGL_MasterMatWidget, masterMatrix, side );

    //update profile view
    updateProfileView( masterMatrix, side );

    //check if the target matrix could bo arranged with new master
    arrangeButtonCheckEnabled();
}

/**
 * @brief creates target matrix and update its view widget
 */
void AppWindow::on_pushButtonTargetMat_clicked()
{
    size_t width = ui->comboBoxTargetMatW->currentText().toInt();
    size_t height = ui->comboBoxTargetMatH->currentText().toInt();
    double precision = ui->comboBoxTargetMatPrec->itemData( ui->comboBoxTargetMatPrec->currentIndex() ).toDouble();
    targetMatrix = HeightMatrix( width, height, precision, HeightMatrix::TARGET );
    fillMatrix(targetMatrix);
    COMPARISON_SIDE side = getSideForTargetMatrix( HeightMatrix::sideFrom( ui->comboBoxSide->currentIndex() ) );

    //update 3D representation
    updateMatrixView( ui->OGL_TargetMatWidget, targetMatrix, side );

    //update profile view
    updateProfileView( targetMatrix, side );

    //check if the new matrix could be arranged with master
    arrangeButtonCheckEnabled();
}

/**
 * @brief changes matrices view widgets and profile view according to a chosen side
 * @param sideIndex index of the side in corresponding checkbox
 */
void AppWindow::on_comboBoxSide_currentIndexChanged( int sideIndex )
{
    //update 3D represenation for master matrix
    COMPARISON_SIDE masterSide = HeightMatrix::sideFrom(sideIndex);
    updateMatrixView( ui->OGL_MasterMatWidget, masterMatrix, masterSide, true );

    //update 3D representation for target matrix
    COMPARISON_SIDE targetSide = getSideForTargetMatrix(masterSide);
    updateMatrixView( ui->OGL_TargetMatWidget, targetMatrix, targetSide, true );

    //update profiles view
    ui->OGL_ProfileViewWidget->makeCurrent();
    ui->OGL_ProfileViewWidget->updateProfileBuffer( masterMatrix, masterSide );
    ui->OGL_ProfileViewWidget->updateProfileBuffer( targetMatrix, targetSide );
    ui->OGL_ProfileViewWidget->update();
}

/**
 * @brief arranges two matrices on its corresponding sides, updates arrangement view
 */
void AppWindow::on_pushButtonArrange_clicked()
{
    if ( masterMatrix.getWidth() == 0 || targetMatrix.getWidth() == 0 )
    {
        QMessageBox::warning( this, "Warning", "Create matrices first" );
        return;
    }

    //update arrangement view
    COMPARISON_SIDE masterSide = HeightMatrix::sideFrom( ui->comboBoxSide->currentIndex() );
    COMPARISON_SIDE targetSide = getSideForTargetMatrix(masterSide);
    ui->OGL_ArrangementViewWidget->makeCurrent();
    ui->OGL_ArrangementViewWidget->updateProfilesData( masterMatrix, targetMatrix, masterSide, targetSide );
    ui->OGL_ArrangementViewWidget->update();

    //update 3D representation of target matrix after arrangement applied
    updateMatrixView( ui->OGL_TargetMatWidget, targetMatrix, targetSide );
}

/**
 * @brief updates matrix widget with new data and updates its view
 * @param matrixWidget widget to update
 * @param matrix matrix containing heights data
 * @param side side to arrange
 * @param comparisonOnly flag indicating that only comparison line data should be updated
 */
void AppWindow::updateMatrixView( MatrixWidget * matrixWidget,
                                  const HeightMatrix & MATRIX,
                                  COMPARISON_SIDE side,
                                  bool comparisonOnly )
{
    matrixWidget->makeCurrent();
    matrixWidget->updateMatrixData( MATRIX, side, comparisonOnly );
    matrixWidget->update();
}

/**
 * @brief updates profile view with appropriate matrix profile of a given side
 * @param matrix matrix to update profile from
 * @param side side of the matrix
 */
void AppWindow::updateProfileView( const HeightMatrix & MATRIX,
                                   COMPARISON_SIDE side )
{
    ui->OGL_ProfileViewWidget->makeCurrent();
    ui->OGL_ProfileViewWidget->updateProfileBuffer( MATRIX, side );
    ui->OGL_ProfileViewWidget->update();
}

/**
 * @brief checks whether arrange is possible for current precisions of matrices
 */
void AppWindow::arrangeButtonCheckEnabled()
{
    ui->pushButtonArrange->setEnabled( masterMatrix.getPrecision() >= targetMatrix.getPrecision() );
    if ( ui->pushButtonArrange->isEnabled() )
    {
        ui->pushButtonArrange->setToolTip("");
    }
    else
    {
        ui->pushButtonArrange->setToolTip("Target matrix should be no less precise than master");
    }
}

/**
 * @brief initializes items for matrices comboboxes
 * @param widthComboBox combobox with width values
 * @param heightComboBox combobox with height values
 * @param precisionComboBox combobox with precision setting
 */
void AppWindow::initializeMatrixSettingsWidgets( QComboBox * widthComboBox,
                                                 QComboBox * heightComboBox,
                                                 QComboBox * precisionComboBox )
{
    widthComboBox->addItem("10");
    widthComboBox->addItem("20");
    widthComboBox->addItem("30");
    widthComboBox->setCurrentIndex(1);
    heightComboBox->addItem("10");
    heightComboBox->addItem("20");
    heightComboBox->addItem("30");
    heightComboBox->setCurrentIndex(1);
    precisionComboBox->addItem("1:1", 1);
    precisionComboBox->addItem("1:2", 2);
    precisionComboBox->addItem("1:4", 4);
    precisionComboBox->setCurrentIndex(1);
}
