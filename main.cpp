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
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    Versatacts w;
    w.show();

    return a.exec();
}
