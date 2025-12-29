#ifndef ZOOMWINDOW_H
#define ZOOMWINDOW_H

#include <QMainWindow>
#include <QLabel>
#include <QImage>
#include <QToolBar>
#include <QAction>
#include <QSpinBox>
#include <QColorDialog>
#include <QPushButton>
#include <QMouseEvent>

class ZoomWindow : public QMainWindow
{
    Q_OBJECT

public:
    ZoomWindow(const QImage &image, QWidget *parent = nullptr);
    ~ZoomWindow();

    void setImage(const QImage &image);

protected:
    void mousePressEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;
    void paintEvent(QPaintEvent *event) override;

private slots:
    void saveImage();
    void selectColor();
    void clearDrawing();

private:
    void createActions();
    void createToolBars();
    void updateImageLabel();

    QImage currentImage;
    QImage drawingImage;
    QLabel *imageLabel;
    QToolBar *toolBar;
    QAction *saveAction;
    QAction *colorAction;
    QAction *clearAction;
    QSpinBox *brushSizeSpinBox;
    QColor currentColor;
    int brushSize;
    bool isDrawing;
    QPoint lastPoint;
};

#endif // ZOOMWINDOW_H
