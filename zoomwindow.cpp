#include "zoomwindow.h"
#include <QFileDialog>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QPainter>
#include <QScrollArea>
#include <QMessageBox>

ZoomWindow::ZoomWindow(const QImage &image, QWidget *parent)
    : QMainWindow(parent), currentColor(Qt::red), brushSize(3), isDrawing(false)
{
    setWindowTitle(tr("放大視窗 - 可編輯"));
    
    // Create central widget with scroll area
    QWidget *centralWidget = new QWidget(this);
    QVBoxLayout *mainLayout = new QVBoxLayout(centralWidget);
    
    QScrollArea *scrollArea = new QScrollArea(this);
    imageLabel = new QLabel();
    imageLabel->setMouseTracking(true);
    scrollArea->setWidget(imageLabel);
    scrollArea->setWidgetResizable(false);
    
    mainLayout->addWidget(scrollArea);
    setCentralWidget(centralWidget);
    
    setImage(image);
    createActions();
    createToolBars();
    
    resize(800, 600);
}

ZoomWindow::~ZoomWindow() {}

void ZoomWindow::setImage(const QImage &image)
{
    currentImage = image;
    drawingImage = image.copy();
    updateImageLabel();
}

void ZoomWindow::createActions()
{
    saveAction = new QAction(tr("另存新檔"), this);
    saveAction->setStatusTip(tr("儲存編輯後的影像"));
    connect(saveAction, &QAction::triggered, this, &ZoomWindow::saveImage);
    
    colorAction = new QAction(tr("選擇顏色"), this);
    colorAction->setStatusTip(tr("選擇畫筆顏色"));
    connect(colorAction, &QAction::triggered, this, &ZoomWindow::selectColor);
    
    clearAction = new QAction(tr("清除塗鴉"), this);
    clearAction->setStatusTip(tr("清除所有塗鴉"));
    connect(clearAction, &QAction::triggered, this, &ZoomWindow::clearDrawing);
}

void ZoomWindow::createToolBars()
{
    toolBar = addToolBar(tr("工具"));
    toolBar->addAction(saveAction);
    toolBar->addSeparator();
    
    // Add brush size control
    QLabel *brushLabel = new QLabel(tr("筆刷大小:"), this);
    toolBar->addWidget(brushLabel);
    
    brushSizeSpinBox = new QSpinBox(this);
    brushSizeSpinBox->setMinimum(1);
    brushSizeSpinBox->setMaximum(50);
    brushSizeSpinBox->setValue(brushSize);
    connect(brushSizeSpinBox, QOverload<int>::of(&QSpinBox::valueChanged), 
            [this](int value) { brushSize = value; });
    toolBar->addWidget(brushSizeSpinBox);
    
    toolBar->addSeparator();
    toolBar->addAction(colorAction);
    toolBar->addAction(clearAction);
}

void ZoomWindow::updateImageLabel()
{
    imageLabel->setPixmap(QPixmap::fromImage(drawingImage));
    imageLabel->adjustSize();
}

void ZoomWindow::mousePressEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton) {
        // Convert window coordinates to label coordinates
        QPoint labelPos = imageLabel->mapFrom(this, event->pos());
        
        // Check if the click is within the image bounds
        if (imageLabel->rect().contains(labelPos)) {
            isDrawing = true;
            lastPoint = labelPos;
        }
    }
}

void ZoomWindow::mouseMoveEvent(QMouseEvent *event)
{
    if (isDrawing && (event->buttons() & Qt::LeftButton)) {
        QPoint labelPos = imageLabel->mapFrom(this, event->pos());
        
        // Check if the position is within the image bounds
        if (imageLabel->rect().contains(labelPos)) {
            // Draw line on the image
            QPainter painter(&drawingImage);
            painter.setPen(QPen(currentColor, brushSize, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin));
            painter.drawLine(lastPoint, labelPos);
            
            lastPoint = labelPos;
            updateImageLabel();
        }
    }
}

void ZoomWindow::mouseReleaseEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton && isDrawing) {
        isDrawing = false;
    }
}

void ZoomWindow::paintEvent(QPaintEvent *event)
{
    QMainWindow::paintEvent(event);
}

void ZoomWindow::saveImage()
{
    QString fileName = QFileDialog::getSaveFileName(this, 
                                                      tr("另存新檔"), 
                                                      "", 
                                                      tr("PNG Files (*.png);;JPEG Files (*.jpg);;BMP Files (*.bmp)"));
    if (!fileName.isEmpty()) {
        if (drawingImage.save(fileName)) {
            QMessageBox::information(this, tr("成功"), tr("影像已成功儲存"));
        } else {
            QMessageBox::warning(this, tr("錯誤"), tr("無法儲存影像"));
        }
    }
}

void ZoomWindow::selectColor()
{
    QColor color = QColorDialog::getColor(currentColor, this, tr("選擇畫筆顏色"));
    if (color.isValid()) {
        currentColor = color;
    }
}

void ZoomWindow::clearDrawing()
{
    drawingImage = currentImage.copy();
    updateImageLabel();
}
