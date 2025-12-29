#include "imageprocessor.h"
#include "imagetransform.h"
#include "zoomwindow.h"
#include <QHBoxLayout>
#include <QMenuBar>
#include <QFileDialog>
#include <QDebug>
#include <QPainter>
#include <QInputDialog>

ImageProcessor::ImageProcessor(QWidget *parent)
    : QMainWindow(parent), isSelecting(false), zoomFactor(2.0)
{
    setWindowTitle(tr("影像處理"));
    central = new QWidget();
    QHBoxLayout *mainLayout = new QHBoxLayout(central);
    imgWin = new QLabel();
    QPixmap *initPixmap = new QPixmap(300,200);
    gWin = new ImageTransform();
    initPixmap->fill(QColor(255,255,255));
    imgWin->resize(300,200);
    imgWin->setScaledContents(true);
    imgWin->setPixmap(*initPixmap);
    mainLayout->addWidget(imgWin);
    setCentralWidget(central);
    createActions();
    createMenus();
    createToolBars();

    statusLabel = new QLabel;
    statusLabel->setText(tr("指標位置:"));
    statusLabel->setFixedWidth(100);
    MousePosLabel = new QLabel;
    MousePosLabel->setText(tr(" "));
    MousePosLabel->setFixedWidth(100);
    statusBar()->addPermanentWidget(statusLabel);
    statusBar()->addPermanentWidget(MousePosLabel);
    setMouseTracking(true);
    imgWin->setMouseTracking(true);
    central->setMouseTracking(true);

}

ImageProcessor::~ImageProcessor() {}

void ImageProcessor::createActions()
{
    openFileAction = new QAction(tr("開啟檔案&O"),this);
    openFileAction->setShortcut(tr("Ctrl+O"));
    openFileAction->setStatusTip(tr("開啟影像檔案"));
    connect(openFileAction,SIGNAL(triggered(bool)),this,SLOT(showOpenFile()));

    exitAction = new QAction(tr("結束&Q"),this);
    exitAction->setShortcut(tr("Ctrl+Q"));
    exitAction->setStatusTip(tr("退出程式"));
    connect(exitAction,SIGNAL(triggered(bool)),this,SLOT(close()));

    big = new QAction(tr("放大"),this);
    exitAction->setStatusTip(tr("放大"));
    connect(big,SIGNAL(triggered(bool)),this,SLOT(bigFile()));

    small = new QAction(tr("縮小"),this);
    exitAction->setStatusTip(tr("縮小"));
    connect(small,SIGNAL(triggered(bool)),this,SLOT(smallFile()));

    geometryAction = new QAction(tr("幾何轉換"),this);
    geometryAction->setShortcut(tr("Ctrl+G"));
    geometryAction->setStatusTip(tr("影像幾何轉換"));
    connect(geometryAction,SIGNAL(triggered(bool)),this,SLOT(showGeometryTransform()));
    connect(exitAction,SIGNAL(triggered(bool)),gWin,SLOT(close()));
    
    zoomFactorAction = new QAction(tr("設定放大倍率"),this);
    zoomFactorAction->setStatusTip(tr("設定拖曳選取範圍的放大倍率"));
    connect(zoomFactorAction,SIGNAL(triggered(bool)),this,SLOT(setZoomFactor()));
}

void ImageProcessor::createMenus()
{
    fileMenu = menuBar()->addMenu(tr("檔案&F"));
    fileMenu->addAction(openFileAction);
    fileMenu->addAction(exitAction);

    fileMenu = menuBar()->addMenu(tr("工具&T"));
    fileMenu->addAction(big);
    fileMenu->addAction(small);

    fileMenu->addAction(geometryAction);
    fileMenu->addAction(zoomFactorAction);
}

void ImageProcessor::createToolBars()
{
    fileTool = addToolBar("file");
    fileTool->addAction(openFileAction);

    fileTool = addToolBar("tool");
    fileTool->addAction(big);
    fileTool->addAction(small);

    fileTool->addAction(geometryAction);
}

void ImageProcessor::loadFile(QString filename)
{
    qDebug()<<QString("file name:%1").arg(filename);
    QByteArray ba = filename.toLatin1();
    printf("FN:%s\n",(char *) ba.data());
    img.load(filename);
    imgWin->setPixmap(QPixmap::fromImage(img));
}

void ImageProcessor::showOpenFile()
{
    filename = QFileDialog::getOpenFileName(this,tr("開啟影像"),tr("."),"bmp(*.bmp);;png(*.png)"";;Jpng(*.jpg)");
    if(!filename.isEmpty()){
        if(img.isNull()){
            loadFile(filename);
        }else{
            ImageProcessor *newIPWin = new ImageProcessor();
            newIPWin->show();
            newIPWin->loadFile(filename);
        }
    }
}

void ImageProcessor::bigFile()
{
    if(!filename.isEmpty()){
        ImageProcessor *newIPWin = new ImageProcessor();
        newIPWin->show();
        newIPWin->loadFile(filename);
    }
}
void ImageProcessor::smallFile()
{
    if(!filename.isEmpty()){
        ImageProcessor *newIPWin = new ImageProcessor();
        newIPWin->show();
        newIPWin->loadFile(filename);
    }
}

void ImageProcessor::showGeometryTransform(){
    if(!img.isNull())
        gWin->srcImg=img;
    gWin->inWin->setPixmap(QPixmap::fromImage(gWin->srcImg));
    gWin->show();
}

void ImageProcessor::mouseDoubleClickEvent(QMouseEvent *event){
    QString str = "(" + QString::number(event->x()) +", " + QString::number(event->y()) + ")";
    statusBar()->showMessage(tr("雙擊:")+str,1000);
}
void ImageProcessor::mouseMoveEvent(QMouseEvent *event){
    int gray = qGray(img.pixel(event->x(),event->y()));
    QString str = "(" + QString::number(event->x()) +", " + QString::number(event->y()) + ")" + " = "+QString::number(gray);

    MousePosLabel->setText(str);
    
    // Update selection rectangle while dragging
    if (isSelecting) {
        selectionEnd = event->pos();
        update(); // Trigger paintEvent to draw the selection rectangle
    }
}
void ImageProcessor::mousePressEvent(QMouseEvent *event){
    QString str = "(" + QString::number(event->x()) +", " + QString::number(event->y()) + ")";
    if(event->button()==Qt::LeftButton){
        statusBar()->showMessage(tr("左鍵:")+str,1000);
        
        // Start drag selection if we have an image
        if (!img.isNull()) {
            isSelecting = true;
            selectionStart = event->pos();
            selectionEnd = event->pos();
        }
    }
    else if(event->button()==Qt::RightButton){
        statusBar()->showMessage(tr("右鍵:")+str,1000);
    }
    else if(event->button()==Qt::MiddleButton){
        statusBar()->showMessage(tr("中鍵:")+str,1000);
    }
}
void ImageProcessor::mouseReleaseEvent(QMouseEvent *event){
    QString str = "(" + QString::number(event->x()) +", " + QString::number(event->y()) + ")";
    statusBar()->showMessage(tr("釋放:")+str,1000);
    
    if (event->button() == Qt::LeftButton && isSelecting) {
        isSelecting = false;
        
        // Get the selection coordinates relative to the image label
        QPoint labelStart = imgWin->mapFrom(this, selectionStart);
        QPoint labelEnd = imgWin->mapFrom(this, event->pos());
        
        // Ensure the points are within the image label bounds
        QRect labelRect = imgWin->rect();
        labelStart.setX(qBound(0, labelStart.x(), labelRect.width() - 1));
        labelStart.setY(qBound(0, labelStart.y(), labelRect.height() - 1));
        labelEnd.setX(qBound(0, labelEnd.x(), labelRect.width() - 1));
        labelEnd.setY(qBound(0, labelEnd.y(), labelRect.height() - 1));
        
        // Calculate the selection rectangle
        int x = qMin(labelStart.x(), labelEnd.x());
        int y = qMin(labelStart.y(), labelEnd.y());
        int w = qAbs(labelEnd.x() - labelStart.x());
        int h = qAbs(labelEnd.y() - labelStart.y());
        
        // Check if we have a valid selection
        if (w > 5 && h > 5 && !img.isNull()) {
            // Map the label coordinates to the actual image coordinates
            double scaleX = (double)img.width() / (double)labelRect.width();
            double scaleY = (double)img.height() / (double)labelRect.height();
            
            int imgX = (int)(x * scaleX);
            int imgY = (int)(y * scaleY);
            int imgW = (int)(w * scaleX);
            int imgH = (int)(h * scaleY);
            
            // Ensure we don't go out of bounds
            imgX = qBound(0, imgX, img.width() - 1);
            imgY = qBound(0, imgY, img.height() - 1);
            imgW = qMin(imgW, img.width() - imgX);
            imgH = qMin(imgH, img.height() - imgY);
            
            if (imgW > 0 && imgH > 0) {
                // Extract the selected region
                QImage selectedRegion = img.copy(imgX, imgY, imgW, imgH);
                
                // Apply zoom factor
                int zoomedWidth = (int)(imgW * zoomFactor);
                int zoomedHeight = (int)(imgH * zoomFactor);
                QImage zoomedImage = selectedRegion.scaled(zoomedWidth, zoomedHeight, 
                                                           Qt::KeepAspectRatio, 
                                                           Qt::SmoothTransformation);
                
                // Create and show the zoom window
                ZoomWindow *zoomWin = new ZoomWindow(zoomedImage);
                zoomWin->show();
            }
        }
        
        update(); // Clear the selection rectangle
    }
}

void ImageProcessor::paintEvent(QPaintEvent *event)
{
    QMainWindow::paintEvent(event);
    
    if (isSelecting) {
        QPainter painter(this);
        painter.setPen(QPen(Qt::red, 2, Qt::DashLine));
        
        int x = qMin(selectionStart.x(), selectionEnd.x());
        int y = qMin(selectionStart.y(), selectionEnd.y());
        int w = qAbs(selectionEnd.x() - selectionStart.x());
        int h = qAbs(selectionEnd.y() - selectionStart.y());
        
        painter.drawRect(x, y, w, h);
    }
}

void ImageProcessor::setZoomFactor()
{
    bool ok;
    double newZoom = QInputDialog::getDouble(this, 
                                             tr("設定放大倍率"),
                                             tr("請輸入放大倍率 (1.0 - 10.0):"),
                                             zoomFactor,
                                             1.0,
                                             10.0,
                                             1,
                                             &ok);
    if (ok) {
        zoomFactor = newZoom;
        statusBar()->showMessage(tr("放大倍率已設定為: ") + QString::number(zoomFactor), 2000);
    }
}

