/*
    This file is part of Mileage.

    Mileage is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    Mileage is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with Mileage.  If not, see <http://www.gnu.org/licenses/>.
*/

#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QtGui/QMainWindow>
#include <QFile>
#include <QTextStream>

namespace Ui {
    class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT
public:
    enum ScreenOrientation {
        ScreenOrientationLockPortrait,
        ScreenOrientationLockLandscape,
        ScreenOrientationAuto
    };

    explicit MainWindow(QWidget *parent = 0);
    virtual ~MainWindow();

    // Note that this will only have an effect on Symbian and Fremantle.
    void setOrientation(ScreenOrientation orientation);

    void showExpanded();

private slots:
    void on_action_About_triggered();

    void on_addButton_clicked();
    
    void on_combineButton_clicked();

    void on_actionClear_History_triggered();

private:
    Ui::MainWindow *ui;
    QFile config;
    QString lastFileItem;
    QTextStream stream;

    QString fileToGuiItem(QString& fileItem);
    bool extractItemsFromFileLine(QString& line, QDateTime& dateTime, 
                                  double& miles, double& litres, double& cost)
};

#endif // MAINWINDOW_H
