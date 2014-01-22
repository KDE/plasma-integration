/*  This file is part of the KDE libraries
 *  Copyright 2013 Aleix Pol Gonzalez <aleixpol@blue-systems.com>
 *
 *  This library is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU Lesser General Public License as published by
 *  the Free Software Foundation; either version 2 of the License or ( at
 *  your option ) version 3 or, at the discretion of KDE e.V. ( which shall
 *  act as a proxy as in section 14 of the GPLv3 ), any later version.
 *
 *  This library is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 *  Library General Public License for more details.
 *
 *  You should have received a copy of the GNU Lesser General Public License
 *  along with this library; see the file COPYING.LIB.  If not, write to
 *  the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 *  Boston, MA 02110-1301, USA.
 */

#include <QApplication>
#include <QFileDialog>
#include <QCommandLineParser>
#include <QDebug>

int main(int argc, char **argv)
{
    QApplication app(argc, argv);
    QCommandLineParser parser;
    parser.addHelpOption();
    parser.addOption(QCommandLineOption(QStringList(QStringLiteral("acceptMode")), QStringLiteral("File dialog acceptMode: 'open' or 'save'"), QStringLiteral("type"), QStringLiteral("open")));
    parser.addOption(QCommandLineOption(QStringList(QStringLiteral("fileMode")), QStringLiteral("File dialog fileMode: 'AnyFile' or 'ExistingFile' or 'Directory' or 'ExistingFiles'"), QStringLiteral("type")));
    parser.addOption(QCommandLineOption(QStringList(QStringLiteral("filter")), QStringLiteral("Dialog filter"), QStringLiteral("filter"), QStringLiteral("Everything (*)")));
    parser.addOption(QCommandLineOption(QStringList(QStringLiteral("modal")), QStringLiteral("Test modal dialog"), QStringLiteral("modality"), QStringLiteral("on")));
    parser.addOption(QCommandLineOption(QStringList(QStringLiteral("selectFile")), QStringLiteral("selectFile(<filename>)"), QStringLiteral("filename")));
    parser.process(app);
    
    QFileDialog dialog;
    dialog.setAcceptMode(
        parser.value(QStringLiteral("acceptMode")) == QStringLiteral("open")
        ? QFileDialog::AcceptOpen
        : QFileDialog::AcceptSave);
    
    QString fileModeValue = parser.value(QStringLiteral("fileMode"));
    if (fileModeValue.isEmpty())
    {
        // do nothing which uses the default value
    }
    else if (fileModeValue == QStringLiteral("AnyFile"))
    {
        qDebug() << "setFileMode";
        dialog.setFileMode(QFileDialog::AnyFile);
    }
    else
    {
        qDebug() << "Not implemented or not valid:" << fileModeValue ;
        exit(0);
    }
    
    dialog.setNameFilter(parser.value(QStringLiteral("filter")));
    
    dialog.selectFile(parser.value(QStringLiteral("selectFile")));

    int ret;
    if (parser.value(QStringLiteral("modal")) == QStringLiteral("off")) {
        dialog.show();
        ret = app.exec();
    } else {
        ret = dialog.exec();
    }

    if (dialog.result() == QDialog::Accepted) {
        qDebug() << "selected files" << dialog.selectedFiles();
        qDebug() << "selected urls" << dialog.selectedUrls();
        qDebug() << "selected name filter" << dialog.selectedNameFilter();
    }
    return ret;
}
