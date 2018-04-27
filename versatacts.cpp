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

#include "versatacts.h"

Versatacts::Versatacts(QWidget *parent)
    : QMainWindow(parent)
{
    QLabel *contactsPathLabel = new QLabel;
    contactsPathLabel->setText(tr("Input Path:"));
    contactsPathLabel->setStyleSheet("QLabel {font-size:11px; font-weight:700;}");
    contactsPathLabel->setAlignment(Qt::AlignVCenter | Qt::AlignRight);

    contactsPathLineEdit = new QLineEdit;
    contactsPathLineEdit->setReadOnly(true);

    selectFileButton = new QToolButton;
    selectFileButton->setIcon(QIcon(":/images/selectFile48.png"));
    selectFileButton->setToolTip(tr("Select the file to import"));
    selectFileButton->setAutoRaise(true);

    selectFolderButton = new QToolButton;
    selectFolderButton->setIcon(QIcon(":/images/selectFolder48.png"));
    selectFolderButton->setToolTip(tr("Select the folder containing vcf files to merge"));
    selectFolderButton->setAutoRaise(true);

    QHBoxLayout *contactsPathLayout = new QHBoxLayout;
    contactsPathLayout->addWidget(contactsPathLabel);
    contactsPathLayout->addWidget(contactsPathLineEdit, 1);
    contactsPathLayout->addWidget(selectFileButton);
    contactsPathLayout->addWidget(selectFolderButton);

    totalLabel = new QLabel;
    totalLabel->setText(tr("Total Records: 0"));
    totalLabel->setStyleSheet("QLabel {font-size:11px; font-weight:700;}");

    QHBoxLayout *labelLayout = new QHBoxLayout;
    labelLayout->addStretch(1);
    labelLayout->addWidget(totalLabel, 0, Qt::AlignCenter);
    labelLayout->addStretch(1);

    vcfTextEdit = new QTextEdit;

    QHBoxLayout *vcfTextLayout = new QHBoxLayout;
    vcfTextLayout->addWidget(vcfTextEdit, 1);

    resetButton = new QToolButton;
    resetButton->setIcon(QIcon(":/images/reset48.png"));
    resetButton->setToolTip(tr("Reset window"));
    resetButton->setAutoRaise(true);
    resetButton->setIconSize(QSize(32, 32));

    reverseButton = new QToolButton;
    reverseButton->setIcon(QIcon(":/images/reverse48.png"));
    reverseButton->setToolTip(tr("Reverse order of names (first <-> last)"));
    reverseButton->setAutoRaise(true);
    reverseButton->setIconSize(QSize(32, 32));

    importButton = new QToolButton;
    importButton->setIcon(QIcon(":/images/import32.png"));
    importButton->setToolTip(tr("Import contacts"));
    importButton->setAutoRaise(true);
    importButton->setIconSize(QSize(32, 32));

    saveButton = new QToolButton;
    saveButton->setIcon(QIcon(":/images/save48.png"));
    saveButton->setToolTip(tr("Save as vcf"));
    saveButton->setAutoRaise(true);
    saveButton->setIconSize(QSize(32, 32));

    QHBoxLayout *mainButtonBoxLayout = new QHBoxLayout;
    mainButtonBoxLayout->addWidget(resetButton, 0, Qt::AlignLeft | Qt::AlignBottom);
    mainButtonBoxLayout->addStretch(1);
    //mainButtonBoxLayout->addWidget(importButton, 0, Qt::AlignRight | Qt::AlignBottom);
    mainButtonBoxLayout->addWidget(reverseButton, 0, Qt::AlignRight | Qt::AlignBottom);
    mainButtonBoxLayout->addWidget(saveButton, 0, Qt::AlignRight | Qt::AlignBottom);

    QVBoxLayout *mainLayout = new QVBoxLayout;
    mainLayout->addLayout(contactsPathLayout);
    mainLayout->addSpacing(15);
    mainLayout->addLayout(labelLayout);
    mainLayout->addSpacing(15);
    mainLayout->addLayout(vcfTextLayout, 1);
    mainLayout->addLayout(mainButtonBoxLayout);

    QWidget *centralWidget = new QWidget;
    centralWidget->setLayout(mainLayout);
    setCentralWidget(centralWidget);

    connectEvents();
    setMinimumSize(500, 500);
    setWindowTitle("Versatacts v0.2");
}

Versatacts::~Versatacts()
{

}

void Versatacts::connectEvents()
{
    connect(selectFileButton, SIGNAL(clicked()), this, SLOT(selectContactsFile()));
    connect(selectFolderButton, SIGNAL(clicked()), this, SLOT(selectVcfFolder()));
    connect(resetButton, SIGNAL(clicked()), this, SLOT(resetAll()));
    connect(importButton, SIGNAL(clicked()), this, SLOT(importRecords()));
    connect(saveButton, SIGNAL(clicked()), this, SLOT(saveVCF()));
    connect(reverseButton, SIGNAL(clicked()), this, SLOT(reverseNames()));
    connect(contactsPathLineEdit, SIGNAL(textChanged(QString)), this, SLOT(importRecords()));
}

void Versatacts::resetAll()
{
    // must disconnect before resetting text both otherwise importRecords
    // will be triggered
    disconnect(contactsPathLineEdit, SIGNAL(textChanged(QString)), this, SLOT(importRecords()));
    contactsPathLineEdit->clear();
    connect(contactsPathLineEdit, SIGNAL(textChanged(QString)), this, SLOT(importRecords()));
    vcfTextEdit->clear();
    totalLabel->setText(tr("Total Records: 0"));
    records.clear();
    totalRecords = -1;
}

void Versatacts::selectContactsFile()
{
    QString path = QFileDialog::getOpenFileName(this,
                                                tr("Select contacts file:"),
                                                QDir::currentPath(),
                                                tr("Contacts (*.pbb *.monosim)"));
    if (path.isEmpty()) return;
    contactsPathLineEdit->setText(path);
}

void Versatacts::selectVcfFolder()
{
    QString path = QFileDialog::getExistingDirectory(this,
                                                     tr("Select folder containing vcf files to merge"),
                                                     QDir::currentPath(),
                                                     QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks);
    if (path.isEmpty()) return;
    contactsPathLineEdit->setText(path);
}

int Versatacts::importRecords()
{
    records.clear();
    vcfTextEdit->clear();
    totalLabel->setText(tr("Total Records: 0"));

    QString contactsPath = contactsPathLineEdit->text();
    if (contactsPath.isEmpty()) {
        QMessageBox::information(this, tr("Versatacts"), tr("Please select the input source."));
        return records.count();
    }

    // convert local file url to local path
    if (contactsPath.left(5) == "file:") {
        QUrl contactsUrl(contactsPath);
        contactsPath = contactsUrl.toLocalFile();
        // dragging and dropping a file onto the contactsPath text box
        // will add two extra spaces to the end of the file url.
        // we can change the file url to a file path without any
        // problems but the extra spaces will be preserved so we
        // have to strip them here. otherwise it will appear as if
        // the file doesn't exist due to the extra spaces in the
        // file name.
        contactsPath.remove(QRegExp("\\s+$"));
        contactsPathLineEdit->setText(contactsPath);
    }

    // get file extension
    QFile contactsFile(contactsPath);
    QFileInfo fi(contactsFile);
    QString ext = fi.suffix().toLower();

    if (!contactsFile.exists()) {
        QMessageBox::information(this, tr("Versatacts"), tr("The input source cannot be found. Please try again."));
        return records.count();
    }

    if (fi.isDir()) {
        mergeRecords(contactsPath);
        generateVCF();
        return records.count();
    }

    if (!contactsFile.open(QIODevice::ReadOnly)) {
        QMessageBox::information(this, tr("Versatacts"), tr("The input source cannot be opened. Please try again."));
        contactsFile.close();
        contactsFile.deleteLater();
        return records.count();
    }

    if (ext == "monosim") {
        importMonosim(&contactsFile);
    } else if (ext == "pbb") {
        importPBB(&contactsFile);
        sanitizeRecords();
    }

    contactsFile.close();
    contactsFile.deleteLater();

    generateVCF();
    return records.count();
}

void Versatacts::mergeRecords(QString path)
{
    QDir dir(path);
    dir.setNameFilters(QStringList() << "*.vcf");
    QStringList fileList = dir.entryList();

    int totalSuccessful = 0;

    QString line;
    QStringList record,names;

    records.clear();
    vcfTextEdit->clear();
    totalLabel->setText(tr("Total Records: 0"));

    QProgressDialog progress("Importing contacts", "Abort", 0, fileList.count(), this);
    progress.setWindowModality(Qt::WindowModal);
    progress.setMinimumDuration(0);
    progress.show();

    for (int i=0; i<fileList.count(); i++) {
        progress.setValue(i);
        if (progress.wasCanceled()) break;

        QFile contactsFile(path + QDir::separator() + fileList[i]);
        if (!contactsFile.exists() || !contactsFile.open(QIODevice::ReadOnly | QIODevice::Text)) {
            QMessageBox::information(this, tr("Versatacts"), fileList[i] + tr(" cannot be opened. Please try again."));
            contactsFile.close();
            contactsFile.deleteLater();
            continue;
        }

        QTextStream inStream(&contactsFile);

        while (!inStream.atEnd()) {
            if (progress.wasCanceled()) break;

            line = inStream.readLine();

            // remove whitespace at beginning and end of line
            line = line.trimmed();

            if (line.isEmpty() ||
                line.startsWith("BEGIN:", Qt::CaseInsensitive) ||
                line.startsWith("VERSION:", Qt::CaseInsensitive) ||
                line.startsWith("n:", Qt::CaseInsensitive)) continue;

            if (line.startsWith("END:", Qt::CaseInsensitive)) {
                if (record.count() > 0) {
                    records << record;
                    record.clear();
                    totalSuccessful++;
                }
                continue;
            }

            if (line.startsWith("FN:", Qt::CaseInsensitive)) {
                line.remove(0,3);
                names.clear();
                names = line.split(" ", QString::SkipEmptyParts);
                record << "F:" + names[0];
                // we don't attempt to detect names other than first, last.
                // so we pop the first since it was saved above and join
                // the remaining as the last name. it won't always be
                // accurate but the reverse names feature can fix it.
                names.pop_front();
                if (names.count() > 0) record << "L:" + names.join(" ");
                continue;
            }
            record << line;
        }

        if (record.count() > 0) {
            records << record;
            record.clear();
            totalSuccessful++;
        }

        contactsFile.close();
        contactsFile.deleteLater();
    }

    progress.setValue(fileList.count());
    totalLabel->setText(tr("Total Records: ") + QString::number(totalSuccessful));
}

void Versatacts::importMonosim(QFile *file)
{
    QString line;
    QString detectPhone = "^[\\#\\+]?\\d{2,11}$";
    QStringList record,names;

    // records is a public list so always clear it
    records.clear();
    vcfTextEdit->clear();
    totalLabel->setText(tr("Total Records: 0"));

    QTextStream inStream(file);

    QProgressDialog progress("Importing contacts", "Abort", 0, file->size(), this);
    progress.setWindowModality(Qt::WindowModal);
    progress.setMinimumDuration(0);
    progress.show();

    while (!inStream.atEnd()) {
        line = inStream.readLine();

        progress.setValue(progress.value() + line.count());
        if (progress.wasCanceled()) break;

        // remove whitespace at beginning and end of line
        line = line.trimmed();

        if (line.isEmpty()) continue;

        if (line.contains(QRegExp(detectPhone))) {
            record << "TEL;TYPE=CELL:" + line;
            records << record;
            record.clear();
        } else {
            names.clear();
            names = line.split(" ", QString::SkipEmptyParts);
            record << "F:" + names[0];
            // we don't attempt to detect names other than first, last.
            // so we pop the first since it was saved above and join
            // the remaining as the last name. it won't always be
            // accurate but the reverse names feature can fix it.
            names.pop_front();
            if (names.count() > 0) record << "L:" + names.join(" ");
        }
    }

    progress.setValue(file->size());
    totalLabel->setText(tr("Total Records: ").append(QString::number(records.count())));
}

void Versatacts::importPBB(QFile *pbbFile)
{    
    bool ok;

    int dataSize;
    int bufSize = 1;
    totalRecords = -1; // total number of contacts as displayed in input file
    int recordIndex = 1; // index of current record - starts at 0 but we start at 1 since there's no easy way to detect record 0
    int isLastBlank = 0; // 0=false, 1=true, 2=split into new line

    char *buf = new char[bufSize];

    QByteArray line;
    QStringList name;
    QStringList record;

    // records is a public list so always clear it
    records.clear();
    vcfTextEdit->clear();
    totalLabel->setText(tr("Total Records: 0"));

    QProgressDialog progress("Importing contacts", "Abort", 0, pbbFile->size(), this);
    progress.setWindowModality(Qt::WindowModal);
    progress.setMinimumDuration(0);
    progress.show();

    while (!pbbFile->atEnd()) {
        progress.setValue(progress.value() + bufSize);
        if (progress.wasCanceled()) break;

        dataSize = pbbFile->read(buf, bufSize);
        QByteArray ba = QByteArray::fromRawData(buf, bufSize);

        // current character is 00
        if (ba.left(1).toHex().toInt(&ok, 16) == 0) {
            // if the last character was also blank then increase isLastBlank.
            // after 2 consecutive blank characters isLastBlank will equal 2
            // while will trigger a new line when the next non-blank character
            // is detected.
            if (isLastBlank < 2) isLastBlank++;
            // nothing to do with blank characters so skip ahead
            continue;
        }

        // we need the last character of the first line which tells
        // us how many records the pbb contains. the rest of line 1
        // is junk. we display the total in a qlabel here.
        if (isLastBlank == 2 && totalRecords < 0 && records.count() == 0 && record.count() == 0) {
                totalRecords = line.right(1).toHex().toInt(&ok, 16);
                //totalLabel->setText(tr("Total Records: ").append(QString::number(totalRecords)));
                // we found the total so no need to retain the current line
                line.clear();
                line.append(ba); // save current character
                isLastBlank = 0; // reset blank character detection
                continue;
        }

        // detect database record separators here (010102, 020102, 030102, etc)
        if (isLastBlank == 2 && line.count() < 4 && line.left(1).toHex().toInt(&ok, 16) == recordIndex && (recordIndex > 2 || line.count() > 1)) {
            name.clear();
            for (int i=0; i<2; i++) {
                if (record.count() < 1) break; // make sure record has entries since we may be removing some

                // make sure last line is not email, phone or url. we're trying to
                // retrieve first and last name here from end of previous record.
                if (record.last().contains("@") ||
                    record.last().contains("://") ||
                    record.last().contains(QRegExp("^[\\#\\+]?\\d{2,11}$"))) break;

                name << record.last();
                record.pop_back();
            }

            // if record contains values save it and clear it. time to move on to new record.
            if (record.count() > 0) {
                records << record;
                record.clear();
            }

            // if we found a name above then save it to new record
            if (name.count() > 0) record << name;

            // the current line represents a database separator. no need to save the line.
            line.clear();
            if (records.count() > 0) recordIndex++; // ready for next record
            line.append(ba); // save current character
            isLastBlank = 0; // current character isn't blank so reset blank detection
            continue;
        }

        if (isLastBlank == 2 && line.count() > 0) {
            // save line to current record
            record << line;
            line.clear();
            line.append(ba); // save current character
            isLastBlank = 0; // reset blank detection
            continue;
        }

        // if we've made it this far then we know there is a valid character.
        // append it to the current line and reset isLastBlank.
        line.append(ba);
        isLastBlank = 0;
    }

    if (line.count() > 0) record << line; // grab final line of file since it isn't triggered in while loop
    if (record.count() > 0) records << record;

    progress.setValue(pbbFile->size());

    // if there are more than 255 records the totalRecords value may be
    // incorrect. always use records.count() for qlabel total and alert
    // user via console if totals do not match. mismatch is unlikely
    // since there is a limit on how many contacts you can store on a
    // sim card.
    totalLabel->setText(tr("Total Records: ").append(QString::number(records.count())));
    if (totalRecords != records.count()) {
        qDebug() << "Records total in pbb file does not match number of records detected!";
        qDebug() << "Total:" << totalRecords << " Detected:" << records.count();
    }

    // prevents compiler warning
    Q_UNUSED(dataSize);
}

void Versatacts::sanitizeRecords()
{
    QString invalidChars = "[^\\w\\.\\+ \\#\\@\\-\\,\\:\\/]";
    QString invalidNameChars = "[]";
    QString invalidPhoneChars = "[]";
    QString invalidEmailChars = "[]";
    QString invalidUrlChars = "[]";
    QString invalidAddressChars = "[]";

    QString trimEdges = "^[^\\w\\+\\#]+|\\W+$";

    QString detectName = "^[A-Za-z_\\- \\.]+$";
    QString detectPhone = "^[\\#\\+]?\\d{2,11}$";
    QString detectEmail = "@";
    QString detectUrl = "://";
    QString detectAddress = "^\\d+ \\w+";

    QStringList telTypes;
    telTypes << "CELL" << "HOME" << "WORK" << "OTHER";
    QStringList emailTypes;
    emailTypes << "HOME" << "WORK" << "OTHER";

    int telTotal,emailTotal;

    QProgressDialog progress("Sanitizing contacts", "Abort", 0, records.count(), this);
    progress.setWindowModality(Qt::WindowModal);
    progress.setMinimumDuration(0);
    progress.show();

    for (int i=0; i<records.count(); i++) {
        progress.setValue(i);
        if (progress.wasCanceled()) break;

        telTotal = 0;
        emailTotal = 0;
        for (int j=0; j<records[i].count(); j++) {
            if (progress.wasCanceled()) break;

            records[i][j].remove(QRegExp(invalidChars));
            records[i][j].remove(QRegExp(trimEdges));

            if (records[i][j].isEmpty()) {
                records[i].removeAt(j);
                j--; // back up if record was removed to avoid skipping any
                continue;
            }

            if (j == 0) { // first entry is always name
                records[i][j].prepend("F:");
            } else if (j == 1 && records[i][j].contains(QRegExp(detectName))) { // second entry might also be name
                records[i][j].prepend("L:");
            } else if (records[i][j].contains(QRegExp(detectPhone))) {
                records[i][j].prepend(tr("TEL;TYPE=%1:").arg(telTypes[telTotal]));
                if (telTotal < telTypes.count() - 1) telTotal++;
            } else if (records[i][j].contains(QRegExp(detectEmail))) {
                records[i][j].prepend(tr("EMAIL;TYPE=%1:").arg(emailTypes[emailTotal]));
                if (emailTotal < emailTypes.count() - 1) emailTotal++;
            } else if (records[i][j].contains(QRegExp(detectUrl))) {
                records[i][j].prepend("URL:");
            } else if (records[i][j].contains(QRegExp(detectAddress))) {
                records[i][j].prepend("ADR:");
            } else {
                records[i][j].prepend("NOTE:");
            }
        }
    }
    progress.setValue(records.count());
}

void Versatacts::generateVCF()
{
    QProgressDialog progress("Generating VCF", "Abort", 0, records.count(), this);
    progress.setWindowModality(Qt::WindowModal);
    progress.setMinimumDuration(0);
    progress.show();

    QString ntype;
    QStringList names;
    bool isNameOutput = false;
    vcfTextEdit->clear();

    for (int i=0; i<records.count(); i++) {
        progress.setValue(i);
        if (progress.wasCanceled()) break;

        ntype = "";
        names.clear();
        isNameOutput = false;
        vcfTextEdit->append("BEGIN:VCARD\nVERSION:3.0");
        for (int j=0; j<records[i].count(); j++) {
            if (progress.wasCanceled()) break;

            ntype = records[i][j].left(2);
            if (ntype == "F:" || ntype == "L:") {
                names << records[i][j].mid(2);
                if (records[i].count() > j + 1) continue;
            }

            // isNameOutput keeps track of whether we've displayed the name
            // in the text box
            if (!isNameOutput) {
                if (names.count() < 2) names << "";
                vcfTextEdit->append(tr("n:%1;%2;;;;").arg(names[1]).arg(names[0]));
                vcfTextEdit->append(tr("FN:%1 %2").arg(names[0]).arg(names[1]));
                isNameOutput = true;
            }

            if (ntype != "F:" && ntype != "L:") {
                vcfTextEdit->append(records[i][j]);
            }
        }
        vcfTextEdit->append("END:VCARD");
    }

    progress.setValue(records.count());
}

void Versatacts::reverseNames()
{
    // there's no point in reversing names if we don't have any vcf text.
    // just generate the vcf here without prompting user if there are
    // records or import records if there are none.
    if (vcfTextEdit->toPlainText().isEmpty()) {
        if (records.count() > 0) {
            generateVCF();
        } else if (importRecords() < 1) {
            QMessageBox::information(this, tr("Versatacts"), tr("There are no records to save!"));
            return;
        }
    }

    int fnIndex,lnIndex;
    QString ntype,fname,lname;
    for (int i=0; i<records.count(); i++) {
        fnIndex = -1;
        lnIndex = -1;
        ntype = "";
        fname = "";
        lname = "";
        for (int j=0; j<records[i].count(); j++) {
            ntype = records[i][j].left(2);
            if (ntype == "F:") {
                fnIndex = j;
            } else if (ntype == "L:") {
                lnIndex = j;
            }
        }
        if (fnIndex > -1 && lnIndex > -1){
            fname = records[i][fnIndex].mid(2);
            lname = records[i][lnIndex].mid(2);
            records[i][fnIndex] = "F:" + lname;
            records[i][lnIndex] = "L:" + fname;
        }
    }
    generateVCF();
}

void Versatacts::saveVCF()
{
    // there's no point in reversing names if we don't have any vcf text.
    // just generate the vcf here without prompting user if there are
    // records or import records if there are none.
    if (vcfTextEdit->toPlainText().isEmpty()) {
        if (records.count() > 0) {
            generateVCF();
        } else if (importRecords() < 1) {
            QMessageBox::information(this, tr("Versatacts"), tr("There are no records to save!"));
            return;
        }
    }

    qint64 tstamp = QDateTime::currentMSecsSinceEpoch();
    QString vcfName = QDir::currentPath() + QDir::separator() + "contacts_" + QString::number(tstamp) + ".vcf";

    QString savePath = QFileDialog::getSaveFileName(this,
                        tr("Save vcf file:"), vcfName, tr("vCard (*.vcf)"));
    if (savePath.isEmpty()) return;

    QFile vcfFile(savePath);
    if (vcfFile.exists()) {
        // qfiledialog automatically prompts before overwriting so we
        // don't need to display the messagebox below
        /*if (QMessageBox::information(this, tr("Versatacts"),
            tr("Are you sure you want to overwrite this file?"),
            QMessageBox::Ok | QMessageBox::Cancel) == QMessageBox::Cancel) return;*/
        vcfFile.remove();
    }

    if (!vcfFile.open(QIODevice::WriteOnly | QIODevice::Text)) {
        vcfFile.close();
        vcfFile.deleteLater();
        QMessageBox::information(this, tr("Versatacts"), tr("The output file cannot be opened for writing. Please try again."));
        return;
    }

    vcfFile.write(vcfTextEdit->toPlainText().toLocal8Bit().data());

    vcfFile.close();
    vcfFile.deleteLater();

    QMessageBox::information(this, tr("Versatacts"), tr("Success!"));
}
