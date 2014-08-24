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
    app.setApplicationName(QStringLiteral("QFileDialogTest"));
    QCommandLineParser parser;
    parser.addHelpOption();
    parser.addOption(QCommandLineOption(QStringList(QStringLiteral("staticFunction")), QStringLiteral("Test one of the static convenience function: 'getOpenFileUrl', 'getExistingDirectory'"), QStringLiteral("function name")));
    parser.addOption(QCommandLineOption(QStringList(QStringLiteral("acceptMode")), QStringLiteral("File dialog acceptMode: 'open' or 'save'"), QStringLiteral("type"), QStringLiteral("open")));
    parser.addOption(QCommandLineOption(QStringList(QStringLiteral("fileMode")), QStringLiteral("File dialog fileMode: 'AnyFile' or 'ExistingFile' or 'Directory' or 'ExistingFiles'"), QStringLiteral("type")));
    parser.addOption(QCommandLineOption(QStringList(QStringLiteral("nameFilter")), QStringLiteral("Dialog nameFilter, e. g. 'cppfiles (*.cpp *.h *.hpp)', can be specified multiple times"), QStringLiteral("nameFilter"), QStringLiteral("Everything (*)")));
    // add option mimeTypeFilter later
    parser.addOption(QCommandLineOption(QStringList(QStringLiteral("selectNameFilter")), QStringLiteral("Initially selected nameFilter"), QStringLiteral("selectNameFilter")));
    parser.addOption(QCommandLineOption(QStringList(QStringLiteral("selectFile")), QStringLiteral("Initially selected file"), QStringLiteral("filename")));
    parser.addOption(QCommandLineOption(QStringList(QStringLiteral("selectDirectory")), QStringLiteral("Initially selected directory"), QStringLiteral("dirname")));
    parser.addOption(QCommandLineOption(QStringList(QStringLiteral("modal")), QStringLiteral("Test modal dialog"), QStringLiteral("modality"), QStringLiteral("on")));
    parser.process(app);

    const QString staticFunction = parser.value(QStringLiteral("staticFunction"));
    if (staticFunction == QLatin1String("getExistingDirectory")) {
        QString dir = QFileDialog::getExistingDirectory(nullptr, QStringLiteral("getExistingDirectory test"), QStringLiteral("/tmp"));
        qDebug() << dir;
        return 0;
    } else if (staticFunction == QLatin1String("getOpenFileUrl")) {
        qDebug() << QFileDialog::getOpenFileUrl(Q_NULLPTR, QStringLiteral("getOpenFileUrl test"), QUrl::fromLocalFile(QDir::homePath()));
        return 0;
    }

    QFileDialog dialog;
    dialog.setAcceptMode(
        parser.value(QStringLiteral("acceptMode")) == QStringLiteral("open")
        ? QFileDialog::AcceptOpen
        : QFileDialog::AcceptSave);
    
    QString fileModeValue = parser.value(QStringLiteral("fileMode"));
    if (fileModeValue == QStringLiteral("AnyFile")) {
        dialog.setFileMode(QFileDialog::AnyFile);
    }
    else if (!fileModeValue.isEmpty()) {
        qDebug() << "Not implemented or not valid:" << fileModeValue ;
        exit(0);
    }
    
    QStringList nameFilterList = parser.values(QStringLiteral("nameFilter"));
    if (nameFilterList.size() == 1) {
        dialog.setNameFilter(nameFilterList.first());
    }
    else {
        dialog.setNameFilters(nameFilterList);
    }

    QString selectNameFilter = parser.value(QStringLiteral("selectNameFilter"));
    if (!selectNameFilter.isEmpty()) {
        dialog.selectNameFilter(selectNameFilter);
    }

    dialog.setDirectory(parser.value(QStringLiteral("selectDirectory")));
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
        qDebug() << "selected name nameFilter" << dialog.selectedNameFilter();
    }
    return ret;
}
