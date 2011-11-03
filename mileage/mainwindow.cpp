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

#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QtCore/QCoreApplication>
#include <QDateTime>
#include <QDir>
#include <QMessageBox>

#define CONFIG_DIR  (".mileage")
#define CONFIG_FILE ("mileage.conf")

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent), ui(new Ui::MainWindow), config(QDir::homePath() + "/" + CONFIG_DIR + "/" + CONFIG_FILE), stream(&config)
{
    ui->setupUi(this);

    /* Open config file (and create if required). */
    if(!config.open(QIODevice::ReadWrite | QIODevice::Text))
    {
        /* Failed to open, create the directory and try again. */
        QDir home(QDir::homePath());
        home.mkdir(CONFIG_DIR);

        if(!config.open(QIODevice::QIODevice::ReadWrite | QIODevice::Text))
        {
            return;
        }
    }

    /* Read each line of the file and add items to the list view. */
    QString line;
    while((line = stream.readLine()), !line.isNull() )
    {
        /* Add the line to the list. */
        ui->mileageList->insertItem(0, fileToGuiItem(line));
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
    /* Create and show about dialog. */
    QMessageBox::about(this, tr("About Mileage"),
    tr("<center><h1>Mileage 0.0.1</h1>"\
    "<p>The current version of Mileage can be found at:<p>"\
    "<p><a href=\"https://github.com/acecil/mileage/\">https://github.com/acecil/mileage/</a></p>"\
    "<p>&copy; 2011 Andrew Gascoyne-Cecil<p>"\
    "<p>&lt;gascoyne@gmail.com&gt;</p>"\
    "<p>This program is free software - "\
    "License: <a href=\"http://www.gnu.org/licenses/gpl-3.0.html\">GNU GPL 3</a> or later.</p></center>"));
}

void MainWindow::on_addButton_clicked()
{
    QString fileItem;
    QTextStream itemStream(&fileItem);

    itemStream << QDate::currentDate().toString("yyyy-MM-dd") << " ";
    itemStream << ui->milesEdit->text() << " ";
    itemStream << ui->litresEdit->text() << " ";
    itemStream << ui->costEdit->text();

    /* Add to GUI. */
    ui->mileageList->insertItem(0, fileToGuiItem(fileItem));

    /* Add to file. */
    stream << fileItem << "\n";
}

QString MainWindow::fileToGuiItem(QString& fileItem)
{
    QString guiItem;
    QTextStream guiStream(&guiItem);
    QTextStream itemStream(&fileItem);

    /* Get values from file item. */
    QString date;
    double miles, litres, cost;
    itemStream >> date >> miles >> litres >> cost;

    /* Calculate mpg. */
    double mpg = miles / (0.219969157 * litres);

    /* Calculate pence per mile. */
    double pencePerMile = cost * 100 / miles;

    guiStream << date << ": " << mpg << " mpg ";
    guiStream << pencePerMile << " ppm";

    return guiItem;
}

void MainWindow::on_actionClear_History_triggered()
{
    QMessageBox msgBox;
    msgBox.setText("Are you sure you want to clear the history?");
    msgBox.setStandardButtons(QMessageBox::Yes | QMessageBox::No);
    msgBox.setDefaultButton(QMessageBox::No);
    if( msgBox.exec() == QMessageBox::Yes )
    {
        QDir d(QDir::homePath() + "/" + CONFIG_DIR);

        /* Rename config file to backup name. */
        if(d.rename(CONFIG_FILE, QDateTime::currentDateTime().toString("yyyyMMddhhmmsszzz") + "-mileage.config"))
        {
            /* Clear list. */
            ui->mileageList->clear();
        }
    }
}
