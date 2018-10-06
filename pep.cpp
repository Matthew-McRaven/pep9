// File: pep.cpp
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
#include <QFile>
#include <QTextStream>
#include "pep.h"

using namespace Enu;

// Fonts
const QString Pep::codeFont = getSystem() == "Windows" ? "Courier New" : (getSystem() == "Mac" ? "Courier" : "Courier New");
const int Pep::codeFontSize = getSystem() == "Windows" ? 9 : (getSystem() == "Mac" ? 12 : 10);
const int Pep::ioFontSize = getSystem() ==  "Windows" ? 10 : (getSystem() == "Mac" ? 13 : 10);
const QString Pep::labelFont = getSystem() == "Windows" ? "Verdana" : (getSystem() == "Mac" ? "Lucida Grande" : "Ubuntu");
const int Pep::labelFontSize = getSystem() == "Windows" ? 10 : (getSystem() == "Mac" ? 13 : 10);

// Default redefine mnemonics
const QString Pep::defaultUnaryMnemonic0 = "NOP0";
const QString Pep::defaultUnaryMnemonic1 = "NOP1";
const QString Pep::defaultNonUnaryMnemonic0 = "NOP";
const int Pep::defaultMnemon0AddrModes = I;
const QString Pep::defaultNonUnaryMnemonic1 = "DECI";
const int Pep::defaultMnemon1AddrModes = ALL&(~I);
const QString Pep::defaultNonUnaryMnemonic2 = "DECO";
const int Pep::defaultMnemon2AddrModes = ALL;
const QString Pep::defaultNonUnaryMnemonic3 = "HEXO";
const int Pep::defaultMnemon3AddrModes = ALL;
const QString Pep::defaultNonUnaryMnemonic4 = "STRO";
const int Pep::defaultMnemon4AddrModes = D|N|S|SF|X;


int Pep::aaaAddressField(EAddrMode addressMode)
{
    if (addressMode == I) return 0;
    if (addressMode == D) return 1;
    if (addressMode == N) return 2;
    if (addressMode == S) return 3;
    if (addressMode == SF) return 4;
    if (addressMode == X) return 5;
    if (addressMode == SX) return 6;
    if (addressMode == SFX) return 7;
    return -1; // Should not occur;
}

int Pep::aAddressField(EAddrMode addressMode)
{
    if (addressMode == I) return 0;
    if (addressMode == X) return 1;
    return -1; // Should not occur;
}

QString Pep::intToAddrMode(EAddrMode addressMode) {
    if (addressMode == I) return "i";
    if (addressMode == D) return "d";
    if (addressMode == N) return "n";
    if (addressMode == S) return "s";
    if (addressMode == SF) return "sf";
    if (addressMode == X) return "x";
    if (addressMode == SX) return "sx";
    if (addressMode == SFX) return "sfx";
    return ""; // Should not occur
}

QString Pep::addrModeToCommaSpace(EAddrMode addressMode) {
    if (addressMode == NONE) return "";
    if (addressMode == I) return ", i";
    if (addressMode == D) return ", d";
    if (addressMode == N) return ", n";
    if (addressMode == S) return ", s";
    if (addressMode == SF) return ", sf";
    if (addressMode == X) return ", x";
    if (addressMode == SX) return ", sx";
    if (addressMode == SFX) return ", sfx";
    return ""; // Should not occur
}

// Function to read text from a resource file
QString Pep::resToString(QString fileName) {
    QFile file(fileName);
    file.open(QIODevice::ReadOnly | QIODevice::Text);
    QTextStream in(&file);
    QString inString = "";
    while (!in.atEnd()) {
        QString line = in.readLine();
        inString.append(line + "\n");
    }
    return inString;
}

QString Pep::getSystem() {
    #ifdef Q_OS_LINUX
    return QString("Linux");
    #endif

    #ifdef Q_OS_OSX
    return QString("Mac");
    #endif

    #ifdef Q_OS_WIN
    return QString("Windows");
    #endif

    return QString("No OS");
}

// Maps between mnemonic enums and strings
QMap<Enu::EMnemonic, QString> Pep::enumToMnemonMap;
QMap<QString, Enu::EMnemonic> Pep::mnemonToEnumMap;
void Pep::initEnumMnemonMaps()
{
    enumToMnemonMap.clear(); mnemonToEnumMap.clear(); // Can be called from Redefine Mnemonics

    QMetaObject meta = Enu::staticMetaObject;
    QMetaEnum metaEnum = meta.enumerator(meta.indexOfEnumerator("EMnemonic"));
    QString tempqs;
    for(int it = 0; it < metaEnum.keyCount(); it++)
    {
        EMnemonic tempi = static_cast<EMnemonic>(metaEnum.value(it));
        tempqs = QString(metaEnum.key(it)).toUpper();
        enumToMnemonMap.insert(tempi, tempqs); mnemonToEnumMap.insert(tempqs, tempi);
    }

    //Lastly, override whatever the above enumerator put in for the redefinable mnemonics
    enumToMnemonMap.insert(NOP, defaultNonUnaryMnemonic0); mnemonToEnumMap.insert(defaultNonUnaryMnemonic0, NOP);
    enumToMnemonMap.insert(NOP0, defaultUnaryMnemonic0); mnemonToEnumMap.insert(defaultUnaryMnemonic0, NOP0);
    enumToMnemonMap.insert(NOP1, defaultUnaryMnemonic1); mnemonToEnumMap.insert(defaultUnaryMnemonic1, NOP1);
    enumToMnemonMap.insert(DECI, defaultNonUnaryMnemonic1); mnemonToEnumMap.insert(defaultNonUnaryMnemonic1, DECI);
    enumToMnemonMap.insert(DECO, defaultNonUnaryMnemonic2); mnemonToEnumMap.insert(defaultNonUnaryMnemonic2, DECO);
    enumToMnemonMap.insert(HEXO, defaultNonUnaryMnemonic3); mnemonToEnumMap.insert(defaultNonUnaryMnemonic3, HEXO);
    enumToMnemonMap.insert(STRO, defaultNonUnaryMnemonic4); mnemonToEnumMap.insert(defaultNonUnaryMnemonic4, STRO);
}

// Maps to characterize each instruction
QMap<Enu::EMnemonic, int> Pep::opCodeMap;
QMap<Enu::EMnemonic, bool> Pep::isUnaryMap;
QMap<Enu::EMnemonic, bool> Pep::addrModeRequiredMap;
QMap<Enu::EMnemonic, bool> Pep::isTrapMap;
void initMnemMapHelper(EMnemonic mnemon,int start,bool unary,bool addrModeReq,bool isTrap)
{
    Pep::opCodeMap.insert(mnemon, start); Pep::isUnaryMap.insert(mnemon, unary);
    Pep::addrModeRequiredMap.insert(mnemon, addrModeReq); Pep::isTrapMap.insert(mnemon, isTrap);
}

void Pep::initMnemonicMaps()
{
    initMnemMapHelper(ADDA, 96, false, true, false);
    initMnemMapHelper(ADDX, 104, false, true, false);
    initMnemMapHelper(ADDSP, 80, false, true, false);
    initMnemMapHelper(ANDA, 128, false, true, false);
    initMnemMapHelper(ANDX, 136, false, true, false);
    initMnemMapHelper(ASLA, 10, true, false, false);
    initMnemMapHelper(ASLX, 11, true, false, false);
    initMnemMapHelper(ASRA, 12, true, false, false);
    initMnemMapHelper(ASRX, 13, true, false, false);

    initMnemMapHelper(BR, 18, false, false, false);
    initMnemMapHelper(BRC, 34, false, false, false);
    initMnemMapHelper(BREQ, 24, false, false, false);
    initMnemMapHelper(BRGE, 28, false, false, false);
    initMnemMapHelper(BRGT, 30, false, false, false);
    initMnemMapHelper(BRLE, 20, false, false, false);
    initMnemMapHelper(BRLT, 22, false, false, false);
    initMnemMapHelper(BRNE, 26, false, false, false);
    initMnemMapHelper(BRV, 32, false, false, false);

    initMnemMapHelper(CALL, 36, false, false, false);
    initMnemMapHelper(CPBA, 176, false, true, false);
    initMnemMapHelper(CPBX, 184, false, true, false);
    initMnemMapHelper(CPWA, 160, false, true, false);
    initMnemMapHelper(CPWX, 168, false, true, false);

    initMnemMapHelper(DECI, 48, false, true, true);
    initMnemMapHelper(DECO, 56, false, true, true);

    initMnemMapHelper(HEXO, 64, false, true, true);

    initMnemMapHelper(LDBA, 208, false, true, false);
    initMnemMapHelper(LDBX, 216, false, true, false);
    initMnemMapHelper(LDWA, 192, false, true, false);
    initMnemMapHelper(LDWX, 200, false, true, false);

#pragma message("I'm not sure these instructions should have a required addressing mode - Matthew McRaven 6/26/2018")
    initMnemMapHelper(MOVAFLG, 5, true, false, false);
    initMnemMapHelper(MOVFLGA, 4, true, false, false);
    initMnemMapHelper(MOVSPA, 3, true, false, false);

    //opCodeMap.insert(MOVAFLG, 5); isUnaryMap.insert(MOVAFLG, true); addrModeRequiredMap.insert(MOVAFLG, true); isTrapMap.insert(MOVAFLG, false);
    //opCodeMap.insert(MOVFLGA, 4); isUnaryMap.insert(MOVFLGA, true); addrModeRequiredMap.insert(MOVFLGA, true); isTrapMap.insert(MOVFLGA, false);
    //opCodeMap.insert(MOVSPA, 3); isUnaryMap.insert(MOVSPA, true); addrModeRequiredMap.insert(MOVSPA, true); isTrapMap.insert(MOVSPA, false);
    initMnemMapHelper(NEGA, 8, true, false, false);
    initMnemMapHelper(NEGX, 9, true, false, false);
    initMnemMapHelper(NOP, 40, false, true, true);
    initMnemMapHelper(NOP0, 38, true, false, true);
    initMnemMapHelper(NOP1, 39, true, false, true);
    initMnemMapHelper(NOTA, 6, true, false, false);
    initMnemMapHelper(NOTX, 7, true, false, false);

    initMnemMapHelper(ORA, 144, false, true, true);
    initMnemMapHelper(ORX, 152, false, true, true);

    initMnemMapHelper(RET, 1, true, false, false);
    initMnemMapHelper(RETTR, 2, true, false, false);
    initMnemMapHelper(ROLA, 14, false, true, false);
    initMnemMapHelper(ROLX, 15, true, false, false);
    initMnemMapHelper(RORA, 16, true, false, false);
    initMnemMapHelper(RORX, 17, true, false, false);

    initMnemMapHelper(STBA, 240, false, true, false);
    initMnemMapHelper(STBX, 248, false, true, false);
    initMnemMapHelper(STWA, 224, false, true, false);
    initMnemMapHelper(STWX, 232, false, true, false);
    initMnemMapHelper(STOP, 0, true, false, false);
    initMnemMapHelper(STRO, 72, false, true, true);
    initMnemMapHelper(SUBA, 112, false, true, false);
    initMnemMapHelper(SUBX, 120, false, true, false);
    initMnemMapHelper(SUBSP, 88, false, true, false);
}

// Map to specify legal addressing modes for each instruction
QMap<Enu::EMnemonic, int > Pep::addrModesMap;
void Pep::initAddrModesMap()
{
    // Nonunary instructions
    addrModesMap.insert(ADDA, ALL);
    addrModesMap.insert(ADDX, ALL);
    addrModesMap.insert(ADDSP, ALL);
    addrModesMap.insert(ANDA, ALL);
    addrModesMap.insert(ANDX, ALL);
    addrModesMap.insert(BR, I | X);
    addrModesMap.insert(BRC, I | X);
    addrModesMap.insert(BREQ, I | X);
    addrModesMap.insert(BRGE, I | X);
    addrModesMap.insert(BRGT, I | X);
    addrModesMap.insert(BRLE, I | X);
    addrModesMap.insert(BRLT, I | X);
    addrModesMap.insert(BRNE, I | X);
    addrModesMap.insert(BRV, I | X);
    addrModesMap.insert(CALL, I | X);
    addrModesMap.insert(CPBA, ALL);
    addrModesMap.insert(CPBX, ALL);
    addrModesMap.insert(CPWA, ALL);
    addrModesMap.insert(CPWX, ALL);
    addrModesMap.insert(LDBA, ALL);
    addrModesMap.insert(LDBX, ALL);
    addrModesMap.insert(LDWA, ALL);
    addrModesMap.insert(LDWX, ALL);
    addrModesMap.insert(ORA, ALL);
    addrModesMap.insert(ORX, ALL);
    addrModesMap.insert(STBA, D | N | S | SF | X | SX | SFX);
    addrModesMap.insert(STBX, D | N | S | SF | X | SX | SFX);
    addrModesMap.insert(STWA, D | N | S | SF | X | SX | SFX);
    addrModesMap.insert(STWX, D | N | S | SF | X | SX | SFX);
    addrModesMap.insert(SUBA, ALL);
    addrModesMap.insert(SUBX, ALL);
    addrModesMap.insert(SUBSP, ALL);

    // Nonunary trap instructions
    addrModesMap.insert(NOP, defaultMnemon0AddrModes);
    addrModesMap.insert(DECI, defaultMnemon1AddrModes);
    addrModesMap.insert(DECO, defaultMnemon2AddrModes);
    addrModesMap.insert(HEXO, defaultMnemon3AddrModes);
    addrModesMap.insert(STRO, defaultMnemon4AddrModes);
}

// The symbol table
QMap<QString, int> Pep::symbolTable;
QMap<QString, bool> Pep::adjustSymbolValueForBurn;

// The trace tag tables
QMap<QString, Enu::ESymbolFormat> Pep::symbolFormat;
QMap<QString, int> Pep::symbolFormatMultiplier;
QMap<QString, QStringList> Pep::globalStructSymbols;

QMap<int, QStringList> Pep::symbolTraceList; // Key is memory address
QStringList Pep::blockSymbols;
QStringList Pep::equateSymbols;

// Map from instruction memory address to assembler listing line
QMap<int, int> *Pep::memAddrssToAssemblerListing;
QMap<int, Qt::CheckState> *Pep::listingRowChecked;

QMap<int, int> Pep::memAddrssToAssemblerListingProg;
QMap<int, Qt::CheckState> Pep::listingRowCheckedProg;

QMap<int, int> Pep::memAddrssToAssemblerListingOS;
QMap<int, Qt::CheckState> Pep::listingRowCheckedOS;

// Decoder tables
QVector<Enu::EMnemonic> Pep::decodeMnemonic(256);
QVector<Enu::EAddrMode> Pep::decodeAddrMode(256);
void initDecoderTableAHelper(EMnemonic val,int startIdx)
{
    Pep::decodeMnemonic[startIdx] = val; Pep::decodeAddrMode[startIdx] = I;
    Pep::decodeMnemonic[startIdx + 1] = val; Pep::decodeAddrMode[startIdx + 1] = X;
}

void initDecoderTableAAAHelper(EMnemonic val,int startIdx)
{
        Pep::decodeMnemonic[startIdx + 0] = val; Pep::decodeAddrMode[startIdx + 0] = I;
        Pep::decodeMnemonic[startIdx + 1] = val; Pep::decodeAddrMode[startIdx + 1] = D;
        Pep::decodeMnemonic[startIdx + 2] = val; Pep::decodeAddrMode[startIdx + 2] = N;
        Pep::decodeMnemonic[startIdx + 3] = val; Pep::decodeAddrMode[startIdx + 3] = S;
        Pep::decodeMnemonic[startIdx + 4] = val; Pep::decodeAddrMode[startIdx + 4] = SF;
        Pep::decodeMnemonic[startIdx + 5] = val; Pep::decodeAddrMode[startIdx + 5] = X;
        Pep::decodeMnemonic[startIdx + 6] = val; Pep::decodeAddrMode[startIdx + 6] = SX;
        Pep::decodeMnemonic[startIdx + 7] = val; Pep::decodeAddrMode[startIdx + 7] = SFX;
}

void initDecoderTableHelperTrap(EMnemonic val,int startIdx,int distance){
    for(int it=0;it<distance;it++)
    {
        // Note that the trap instructions are all unary at the machine level
        Pep::decodeMnemonic[startIdx + it] = val; Pep::decodeAddrMode[startIdx + it] = NONE;
    }
}

void Pep::initDecoderTables()
{
    decodeMnemonic[0] = STOP; decodeAddrMode[0] = NONE;
    decodeMnemonic[1] = RET; decodeAddrMode[1] = NONE;
    decodeMnemonic[2] = RETTR; decodeAddrMode[2] = NONE;
    decodeMnemonic[3] = MOVSPA; decodeAddrMode[3] = NONE;
    decodeMnemonic[4] = MOVFLGA; decodeAddrMode[4] = NONE;
    decodeMnemonic[5] = MOVAFLG; decodeAddrMode[5] = NONE;

    decodeMnemonic[6] = NOTA; decodeAddrMode[6] = NONE;
    decodeMnemonic[7] = NOTX; decodeAddrMode[7] = NONE;
    decodeMnemonic[8] = NEGA; decodeAddrMode[8] = NONE;
    decodeMnemonic[9] = NEGX; decodeAddrMode[9] = NONE;
    decodeMnemonic[10] = ASLA; decodeAddrMode[10] = NONE;
    decodeMnemonic[11] = ASLX; decodeAddrMode[11] = NONE;
    decodeMnemonic[12] = ASRA; decodeAddrMode[12] = NONE;
    decodeMnemonic[13] = ASRX; decodeAddrMode[13] = NONE;
    decodeMnemonic[14] = ROLA; decodeAddrMode[14] = NONE;
    decodeMnemonic[15] = ROLX; decodeAddrMode[15] = NONE;
    decodeMnemonic[16] = RORA; decodeAddrMode[16] = NONE;
    decodeMnemonic[17] = RORX; decodeAddrMode[17] = NONE;

    initDecoderTableAHelper(BR, 18);
    initDecoderTableAHelper(BRLE, 20);
    initDecoderTableAHelper(BRLT, 22);
    initDecoderTableAHelper(BREQ, 24);
    initDecoderTableAHelper(BRNE, 26);
    initDecoderTableAHelper(BRGE, 28);
    initDecoderTableAHelper(BRGT, 30);
    initDecoderTableAHelper(BRV, 32);
    initDecoderTableAHelper(BRC, 34);
    initDecoderTableAHelper(CALL, 36);

    initDecoderTableHelperTrap(NOP0, 38, 1);
    initDecoderTableHelperTrap(NOP1, 39, 1);
    initDecoderTableHelperTrap(NOP, 40, 8);
    initDecoderTableHelperTrap(DECI, 48, 8);
    initDecoderTableHelperTrap(DECO, 56, 8);
    initDecoderTableHelperTrap(HEXO, 64, 8);
    initDecoderTableHelperTrap(STRO, 72, 8);

    initDecoderTableAAAHelper(ADDSP, 80);
    initDecoderTableAAAHelper(SUBSP, 88);
    initDecoderTableAAAHelper(ADDA, 96);
    initDecoderTableAAAHelper(ADDX, 104);
    initDecoderTableAAAHelper(SUBA, 112);
    initDecoderTableAAAHelper(SUBX, 120);
    initDecoderTableAAAHelper(ANDA, 128);
    initDecoderTableAAAHelper(ANDX, 136);
    initDecoderTableAAAHelper(ORA, 144);
    initDecoderTableAAAHelper(ORX, 152);
    initDecoderTableAAAHelper(CPWA, 160);
    initDecoderTableAAAHelper(CPWX, 168);
    initDecoderTableAAAHelper(CPBA, 176);
    initDecoderTableAAAHelper(CPBX, 184);
    initDecoderTableAAAHelper(LDWA, 192);
    initDecoderTableAAAHelper(LDWX, 200);
    initDecoderTableAAAHelper(LDBA, 208);
    initDecoderTableAAAHelper(LDBX, 216);
    initDecoderTableAAAHelper(STWA, 224);
    initDecoderTableAAAHelper(STWX, 232);
    initDecoderTableAAAHelper(STBA, 240);
    initDecoderTableAAAHelper(STBX, 248);
}

// .BURN and the ROM state
int Pep::byteCount;
int Pep::burnCount;
int Pep::dotBurnArgument;
int Pep::romStartAddress;

// Memory trace state
bool Pep::traceTagWarning;
