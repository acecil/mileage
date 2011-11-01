/*
    This file is part of "mileage".

    "mileage" is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    Foobar is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with Foobar.  If not, see <http://www.gnu.org/licenses/>.
*/

#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QtCore/QCoreApplication>
#include <QDateTime>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent), ui(new Ui::MainWindow), config(QString("mileage.conf")), stream(&config)
{
    ui->setupUi(this);

    /* Open config file (and create if required). */
    config.open(QIODevice::ReadWrite);

    /* Read each line of the file and add items to the list view. */
    QString line;
    while((line = stream.readLine()), !line.isNull() )
    {
        /* Add the line to the list. */
        ui->mileageList->insertItem(0, line);
    }

}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::setOrientation(ScreenOrientation orientation)
{
#if defined(Q_OS_SYMBIAN)
    // If the version of Qt on the device is < 4.7.2, that attribute won't work
    if (orientation != ScreenOrientationAuto) {
        const QStringList v = QString::fromAscii(qVersion()).split(QLatin1Char('.'));
        if (v.count() == 3 && (v.at(0).toInt() << 16 | v.at(1).toInt() << 8 | v.at(2).toInt()) < 0x040702) {
            qWarning("Screen orientation locking only supported with Qt 4.7.2 and above");
            return;
        }
    }
#endif // Q_OS_SYMBIAN

    Qt::WidgetAttribute attribute;
    switch (orientation) {
#if QT_VERSION < 0x040702
    // Qt < 4.7.2 does not yet have the Qt::WA_*Orientation attributes
    case ScreenOrientationLockPortrait:
        attribute = static_cast<Qt::WidgetAttribute>(128);
        break;
    case ScreenOrientationLockLandscape:
        attribute = static_cast<Qt::WidgetAttribute>(129);
        break;
    default:
    case ScreenOrientationAuto:
        attribute = static_cast<Qt::WidgetAttribute>(130);
        break;
#else // QT_VERSION < 0x040702
    case ScreenOrientationLockPortrait:
        attribute = Qt::WA_LockPortraitOrientation;
        break;
    case ScreenOrientationLockLandscape:
        attribute = Qt::WA_LockLandscapeOrientation;
        break;
    default:
    case ScreenOrientationAuto:
        attribute = Qt::WA_AutoOrientation;
        break;
#endif // QT_VERSION < 0x040702
    };
    setAttribute(attribute, true);
}

void MainWindow::showExpanded()
{
#if defined(Q_OS_SYMBIAN) || defined(Q_WS_SIMULATOR)
    showFullScreen();
#elif defined(Q_WS_MAEMO_5)
    showMaximized();
#else
    show();
#endif
}

void MainWindow::on_action_About_triggered()
{

}

void MainWindow::on_addButton_clicked()
{
    /* Get values from edit lines. */
    double miles = ui->milesEdit->text().toDouble();
    double litres = ui->litresEdit->text().toDouble();
    double cost = ui->costEdit->text().toDouble();

    /* Calculate mpg. */
    double mpg = miles / (0.219969157 * litres);
    QString mpgString;
    mpgString.setNum(mpg, 'g', 5);

    /* Calculate pence per mile. */
    double pencePerMile = cost * 100 / miles;
    QString pencePerMileString;
    pencePerMileString.setNum(pencePerMile, 'g', 4);

    QString item = QDate::currentDate().toString("yyyy-MM-dd") + ": " + mpgString + " mpg  " + pencePerMileString + " pence per mile";

    /* Add to GUI. */
    ui->mileageList->insertItem(0, item);

    /* Add to file. */
    stream << item << "\n";
}
