/********************************************************************

Name: Versatacts
Homepage: http://github.com/ae5chylu5/versatacts
Author: ae5chylu5
Description: A versatile gui application capable of extracting and
             converting contacts to/from a variety of popular
             mobile formats.

Copyright (C) 2016 ae5chylu5

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <http://www.gnu.org/licenses/>.

********************************************************************/

#ifndef VERSATACTS_H
#define VERSATACTS_H

#include <QDateTime>
#include <QDebug>
#include <QDir>
#include <QFile>
#include <QFileDialog>
#include <QHBoxLayout>
#include <QIODevice>
#include <QLabel>
#include <QLineEdit>
#include <QMainWindow>
#include <QMessageBox>
#include <QProgressDialog>
#include <QRegExp>
#include <QTextEdit>
#include <QTextStream>
#include <QToolButton>
#include <QUrl>
#include <QVBoxLayout>

class Versatacts : public QMainWindow
{
    Q_OBJECT

public:
    Versatacts(QWidget *parent = 0);
    ~Versatacts();
    QList<QStringList> records;
    int totalRecords;

private slots:
    void selectContactsFile();
    void selectVcfFolder();
    void resetAll();
    int importRecords();
    void saveVCF();
    void reverseNames();

private:
    void connectEvents();
    void sanitizeRecords();
    void mergeRecords(QString path);
    void generateVCF();
    void importPBB(QFile *pbbFile);
    void importMonosim(QFile *file);
    QLabel *totalLabel;
    QLineEdit *contactsPathLineEdit;
    QTextEdit *vcfTextEdit;
    QToolButton *selectFileButton;
    QToolButton *selectFolderButton;
    QToolButton *resetButton;
    QToolButton *importButton;
    QToolButton *saveButton;
    QToolButton *reverseButton;
};

#endif // VERSATACTS_H
