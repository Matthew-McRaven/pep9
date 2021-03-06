// File: pep.h
/*
    Pep9 is a virtual machine for writing machine language and assembly
    language programs.
    
    Copyright (C) 2009  J. Stanley Warford, Pepperdine University

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
*/

#ifndef PEP_H
#define PEP_H

#include <QMap>
#include <QSet>
#include <QString>
#include <QStringList>
#include <QVector>
#include <QColor>

#include "enu.h"

class Pep
{
public:
    // Fonts:
    static const QString codeFont;
    static const int codeFontSize;
    static const int ioFontSize;
    static const QString labelFont;
    static const int labelFontSize;

    // Default redefine mnemonics
    static const QString defaultUnaryMnemonic0;
    static const QString defaultUnaryMnemonic1;
    static const QString defaultNonUnaryMnemonic0;
    static const int defaultMnemon0AddrModes;
    static const QString defaultNonUnaryMnemonic1;
    static const int defaultMnemon1AddrModes;
    static const QString defaultNonUnaryMnemonic2;
    static const int defaultMnemon2AddrModes;
    static const QString defaultNonUnaryMnemonic3;
    static const int defaultMnemon3AddrModes;
    static const QString defaultNonUnaryMnemonic4;
    static const int defaultMnemon4AddrModes;


    // Functions for computing instruction specifiers
    static int aaaAddressField(Enu::EAddrMode addressMode);
    static int aAddressField(Enu::EAddrMode addressMode);
    static QString intToAddrMode(Enu::EAddrMode addressMode);
    static QString addrModeToCommaSpace(Enu::EAddrMode addressMode);

    // Function to read text from a resource file
    static QString resToString(QString fileName);
    // Function for getting the system we're running on
    static QString getSystem();

    // Maps between mnemonic enums and strings
    static QMap<Enu::EMnemonic, QString> enumToMnemonMap;
    static QMap<QString, Enu::EMnemonic> mnemonToEnumMap;
    static void initEnumMnemonMaps();

    // Maps to characterize each instruction
    static QMap<Enu::EMnemonic, int> opCodeMap;
    static QMap<Enu::EMnemonic, bool> isUnaryMap;
    static QMap<Enu::EMnemonic, bool> addrModeRequiredMap;
    static QMap<Enu::EMnemonic, bool> isTrapMap;
    static void initMnemonicMaps();

    // Map to specify legal addressing modes for each instruction
    static QMap<Enu::EMnemonic, int> addrModesMap;
    static void initAddrModesMap();

    // The symbol table
    static QMap<QString, int> symbolTable;
    static QMap<QString, bool> adjustSymbolValueForBurn;

    // The trace tag tables
    static QMap<QString, Enu::ESymbolFormat> symbolFormat;
    static QMap<QString, int> symbolFormatMultiplier;

    // This map is for global structs. The key is the symbol defined on the .BLOCK line
    // and QStringList contains the list of symbols from the symbol tags in the .BLOCK comment.
    static QMap<QString, QStringList> globalStructSymbols;

    // This map is used to map the program counter to the stringList of tags on the corresponding line
    // For example, line corresponds to 0x12, and has the comment ; Allocate #next #data
    // The stringlist would contain next and data
    static QMap<int, QStringList> symbolTraceList;

    static QStringList blockSymbols;
    static QStringList equateSymbols;

    // Map from instruction memory address to assembler listing line
    // These pointers are set to the addresses of the program or OS maps
    // depending on whether the program or OS is being assembled
    static QMap<int, int> *memAddrssToAssemblerListing;
    static QMap<int, Qt::CheckState> *listingRowChecked;

    static QMap<int, int> memAddrssToAssemblerListingProg;
    static QMap<int, Qt::CheckState> listingRowCheckedProg;

    static QMap<int, int> memAddrssToAssemblerListingOS;
    static QMap<int, Qt::CheckState> listingRowCheckedOS;

    // Decoder tables
    static QVector<Enu::EMnemonic> decodeMnemonic;
    static QVector<Enu::EAddrMode> decodeAddrMode;
    static void initDecoderTables();

    // .BURN and the ROM state
    static int byteCount;
    static int burnCount;
    static int dotBurnArgument;
    static int romStartAddress;

    // Memory trace state
    static bool traceTagWarning;
};

#endif // PEP_H
