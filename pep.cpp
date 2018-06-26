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
const bool Pep::defaultMnemon0i = true;
const bool Pep::defaultMnemon0d = false;
const bool Pep::defaultMnemon0n = false;
const bool Pep::defaultMnemon0s = false;
const bool Pep::defaultMnemon0sf = false;
const bool Pep::defaultMnemon0x = false;
const bool Pep::defaultMnemon0sx = false;
const bool Pep::defaultMnemon0sfx = false;
const QString Pep::defaultNonUnaryMnemonic1 = "DECI";
const bool Pep::defaultMnemon1i = false;
const bool Pep::defaultMnemon1d = true;
const bool Pep::defaultMnemon1n = true;
const bool Pep::defaultMnemon1s = true;
const bool Pep::defaultMnemon1sf = true;
const bool Pep::defaultMnemon1x = true;
const bool Pep::defaultMnemon1sx = true;
const bool Pep::defaultMnemon1sfx = true;
const QString Pep::defaultNonUnaryMnemonic2 = "DECO";
const bool Pep::defaultMnemon2i = true;
const bool Pep::defaultMnemon2d = true;
const bool Pep::defaultMnemon2n = true;
const bool Pep::defaultMnemon2s = true;
const bool Pep::defaultMnemon2sf = true;
const bool Pep::defaultMnemon2x = true;
const bool Pep::defaultMnemon2sx = true;
const bool Pep::defaultMnemon2sfx = true;
const QString Pep::defaultNonUnaryMnemonic3 = "HEXO";
const bool Pep::defaultMnemon3i = true;
const bool Pep::defaultMnemon3d = true;
const bool Pep::defaultMnemon3n = true;
const bool Pep::defaultMnemon3s = true;
const bool Pep::defaultMnemon3sf = true;
const bool Pep::defaultMnemon3x = true;
const bool Pep::defaultMnemon3sx = true;
const bool Pep::defaultMnemon3sfx = true;
const QString Pep::defaultNonUnaryMnemonic4 = "STRO";
const bool Pep::defaultMnemon4i = false;
const bool Pep::defaultMnemon4d = true;
const bool Pep::defaultMnemon4n = true;
const bool Pep::defaultMnemon4s = true;
const bool Pep::defaultMnemon4sf = true;
const bool Pep::defaultMnemon4x = true;
const bool Pep::defaultMnemon4sx = false;
const bool Pep::defaultMnemon4sfx = false;

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
    enumToMnemonMap.insert(ADDA, "ADDA"); mnemonToEnumMap.insert("ADDA", ADDA);
    enumToMnemonMap.insert(ADDX, "ADDX"); mnemonToEnumMap.insert("ADDX", ADDX);
    enumToMnemonMap.insert(ADDSP, "ADDSP"); mnemonToEnumMap.insert("ADDSP", ADDSP);
    enumToMnemonMap.insert(ANDA, "ANDA"); mnemonToEnumMap.insert("ANDA", ANDA);
    enumToMnemonMap.insert(ANDX, "ANDX"); mnemonToEnumMap.insert("ANDX", ANDX);
    enumToMnemonMap.insert(ASLA, "ASLA"); mnemonToEnumMap.insert("ASLA", ASLA);
    enumToMnemonMap.insert(ASLX, "ASLX"); mnemonToEnumMap.insert("ASLX", ASLX);
    enumToMnemonMap.insert(ASRA, "ASRA"); mnemonToEnumMap.insert("ASRA", ASRA);
    enumToMnemonMap.insert(ASRX, "ASRX"); mnemonToEnumMap.insert("ASRX", ASRX);
    enumToMnemonMap.insert(BR, "BR"); mnemonToEnumMap.insert("BR", BR);
    enumToMnemonMap.insert(BRC, "BRC"); mnemonToEnumMap.insert("BRC", BRC);
    enumToMnemonMap.insert(BREQ, "BREQ"); mnemonToEnumMap.insert("BREQ", BREQ);
    enumToMnemonMap.insert(BRGE, "BRGE"); mnemonToEnumMap.insert("BRGE", BRGE);
    enumToMnemonMap.insert(BRGT, "BRGT"); mnemonToEnumMap.insert("BRGT", BRGT);
    enumToMnemonMap.insert(BRLE, "BRLE"); mnemonToEnumMap.insert("BRLE", BRLE);
    enumToMnemonMap.insert(BRLT, "BRLT"); mnemonToEnumMap.insert("BRLT", BRLT);
    enumToMnemonMap.insert(BRNE, "BRNE"); mnemonToEnumMap.insert("BRNE", BRNE);
    enumToMnemonMap.insert(BRV, "BRV"); mnemonToEnumMap.insert("BRV", BRV);
    enumToMnemonMap.insert(CALL, "CALL"); mnemonToEnumMap.insert("CALL", CALL);
    enumToMnemonMap.insert(CPBA, "CPBA"); mnemonToEnumMap.insert("CPBA", CPBA);
    enumToMnemonMap.insert(CPBX, "CPBX"); mnemonToEnumMap.insert("CPBX", CPBX);
    enumToMnemonMap.insert(CPWA, "CPWA"); mnemonToEnumMap.insert("CPWA", CPWA);
    enumToMnemonMap.insert(CPWX, "CPWX"); mnemonToEnumMap.insert("CPWX", CPWX);
    enumToMnemonMap.insert(DECI, defaultNonUnaryMnemonic1); mnemonToEnumMap.insert(defaultNonUnaryMnemonic1, DECI);
    enumToMnemonMap.insert(DECO, defaultNonUnaryMnemonic2); mnemonToEnumMap.insert(defaultNonUnaryMnemonic2, DECO);
    enumToMnemonMap.insert(HEXO, defaultNonUnaryMnemonic3); mnemonToEnumMap.insert(defaultNonUnaryMnemonic3, HEXO);
    enumToMnemonMap.insert(LDBA, "LDBA"); mnemonToEnumMap.insert("LDBA", LDBA);
    enumToMnemonMap.insert(LDWA, "LDWA"); mnemonToEnumMap.insert("LDWA", LDWA);
    enumToMnemonMap.insert(LDBX, "LDBX"); mnemonToEnumMap.insert("LDBX", LDBX);
    enumToMnemonMap.insert(LDWX, "LDWX"); mnemonToEnumMap.insert("LDWX", LDWX);
    enumToMnemonMap.insert(MOVAFLG, "MOVAFLG"); mnemonToEnumMap.insert("MOVAFLG", MOVAFLG);
    enumToMnemonMap.insert(MOVFLGA, "MOVFLGA"); mnemonToEnumMap.insert("MOVFLGA", MOVFLGA);
    enumToMnemonMap.insert(MOVSPA, "MOVSPA"); mnemonToEnumMap.insert("MOVSPA", MOVSPA);
    enumToMnemonMap.insert(NEGA, "NEGA"); mnemonToEnumMap.insert("NEGA", NEGA);
    enumToMnemonMap.insert(NEGX, "NEGX"); mnemonToEnumMap.insert("NEGX", NEGX);
    enumToMnemonMap.insert(NOP, defaultNonUnaryMnemonic0); mnemonToEnumMap.insert(defaultNonUnaryMnemonic0, NOP);
    enumToMnemonMap.insert(NOP0, defaultUnaryMnemonic0); mnemonToEnumMap.insert(defaultUnaryMnemonic0, NOP0);
    enumToMnemonMap.insert(NOP1, defaultUnaryMnemonic1); mnemonToEnumMap.insert(defaultUnaryMnemonic1, NOP1);
    enumToMnemonMap.insert(NOTA, "NOTA"); mnemonToEnumMap.insert("NOTA", NOTA);
    enumToMnemonMap.insert(NOTX, "NOTX"); mnemonToEnumMap.insert("NOTX", NOTX);
    enumToMnemonMap.insert(ORA, "ORA"); mnemonToEnumMap.insert("ORA", ORA);
    enumToMnemonMap.insert(ORX, "ORX"); mnemonToEnumMap.insert("ORX", ORX);
    enumToMnemonMap.insert(RET, "RET"); mnemonToEnumMap.insert("RET", RET);
    enumToMnemonMap.insert(RETTR, "RETTR"); mnemonToEnumMap.insert("RETTR", RETTR);
    enumToMnemonMap.insert(ROLA, "ROLA"); mnemonToEnumMap.insert("ROLA", ROLA);
    enumToMnemonMap.insert(ROLX, "ROLX"); mnemonToEnumMap.insert("ROLX", ROLX);
    enumToMnemonMap.insert(RORA, "RORA"); mnemonToEnumMap.insert("RORA", RORA);
    enumToMnemonMap.insert(RORX, "RORX"); mnemonToEnumMap.insert("RORX", RORX);
    enumToMnemonMap.insert(STBA, "STBA"); mnemonToEnumMap.insert("STBA", STBA);
    enumToMnemonMap.insert(STBX, "STBX"); mnemonToEnumMap.insert("STBX", STBX);
    enumToMnemonMap.insert(STWA, "STWA"); mnemonToEnumMap.insert("STWA", STWA);
    enumToMnemonMap.insert(STWX, "STWX"); mnemonToEnumMap.insert("STWX", STWX);
    enumToMnemonMap.insert(STOP, "STOP"); mnemonToEnumMap.insert("STOP", STOP);
    enumToMnemonMap.insert(STRO, defaultNonUnaryMnemonic4); mnemonToEnumMap.insert(defaultNonUnaryMnemonic4, STRO);
    enumToMnemonMap.insert(SUBA, "SUBA"); mnemonToEnumMap.insert("SUBA", SUBA);
    enumToMnemonMap.insert(SUBX, "SUBX"); mnemonToEnumMap.insert("SUBX", SUBX);
    enumToMnemonMap.insert(SUBSP, "SUBSP"); mnemonToEnumMap.insert("SUBSP", SUBSP);
}

// Maps to characterize each instruction
QMap<Enu::EMnemonic, int> Pep::opCodeMap;
QMap<Enu::EMnemonic, bool> Pep::isUnaryMap;
QMap<Enu::EMnemonic, bool> Pep::addrModeRequiredMap;
QMap<Enu::EMnemonic, bool> Pep::isTrapMap;

void Pep::initMnemonicMaps()
{
    opCodeMap.insert(ADDA, 96); isUnaryMap.insert(ADDA, false); addrModeRequiredMap.insert(ADDA, true); isTrapMap.insert(ADDA, false);
    opCodeMap.insert(ADDX, 104); isUnaryMap.insert(ADDX, false); addrModeRequiredMap.insert(ADDX, true); isTrapMap.insert(ADDX, false);
    opCodeMap.insert(ADDSP, 80); isUnaryMap.insert(ADDSP, false); addrModeRequiredMap.insert(ADDSP, true); isTrapMap.insert(ADDSP, false);
    opCodeMap.insert(ANDA, 128); isUnaryMap.insert(ANDA, false); addrModeRequiredMap.insert(ANDA, true); isTrapMap.insert(ANDA, false);
    opCodeMap.insert(ANDX, 136); isUnaryMap.insert(ANDX, false); addrModeRequiredMap.insert(ANDX, true); isTrapMap.insert(ANDX, false);
    opCodeMap.insert(ASLA, 10); isUnaryMap.insert(ASLA, true); addrModeRequiredMap.insert(ASLA, true); isTrapMap.insert(ASLA, false);
    opCodeMap.insert(ASLX, 11); isUnaryMap.insert(ASLX, true); addrModeRequiredMap.insert(ASLX, true); isTrapMap.insert(ASLX, false);
    opCodeMap.insert(ASRA, 12); isUnaryMap.insert(ASRA, true); addrModeRequiredMap.insert(ASRA, true); isTrapMap.insert(ASRA, false);
    opCodeMap.insert(ASRX, 13); isUnaryMap.insert(ASRX, true); addrModeRequiredMap.insert(ASRX, true); isTrapMap.insert(ASRX, false);

    opCodeMap.insert(BR, 18); isUnaryMap.insert(BR, false); addrModeRequiredMap.insert(BR, false); isTrapMap.insert(BR, false);
    opCodeMap.insert(BRC, 34); isUnaryMap.insert(BRC, false); addrModeRequiredMap.insert(BRC, false); isTrapMap.insert(BRC, false);
    opCodeMap.insert(BREQ, 24); isUnaryMap.insert(BREQ, false); addrModeRequiredMap.insert(BREQ, false); isTrapMap.insert(BREQ, false);
    opCodeMap.insert(BRGE, 28); isUnaryMap.insert(BRGE, false); addrModeRequiredMap.insert(BRGE, false); isTrapMap.insert(BRGE, false);
    opCodeMap.insert(BRGT, 30); isUnaryMap.insert(BRGT, false); addrModeRequiredMap.insert(BRGT, false); isTrapMap.insert(BRGT, false);
    opCodeMap.insert(BRLE, 20); isUnaryMap.insert(BRLE, false); addrModeRequiredMap.insert(BRLE, false); isTrapMap.insert(BRLE, false);
    opCodeMap.insert(BRLT, 22); isUnaryMap.insert(BRLT, false); addrModeRequiredMap.insert(BRLT, false); isTrapMap.insert(BRLT, false);
    opCodeMap.insert(BRNE, 26); isUnaryMap.insert(BRNE, false); addrModeRequiredMap.insert(BRNE, false); isTrapMap.insert(BRNE, false);
    opCodeMap.insert(BRV, 32); isUnaryMap.insert(BRV, false); addrModeRequiredMap.insert(BRV, false); isTrapMap.insert(BRV, false);

    opCodeMap.insert(CALL, 36); isUnaryMap.insert(CALL, false); addrModeRequiredMap.insert(CALL, false); isTrapMap.insert(CALL, false);
    opCodeMap.insert(CPBA, 176); isUnaryMap.insert(CPBA, false); addrModeRequiredMap.insert(CPBA, true); isTrapMap.insert(CPBA, false);
    opCodeMap.insert(CPBX, 184); isUnaryMap.insert(CPBX, false); addrModeRequiredMap.insert(CPBX, true); isTrapMap.insert(CPBX, false);
    opCodeMap.insert(CPWA, 160); isUnaryMap.insert(CPWA, false); addrModeRequiredMap.insert(CPWA, true); isTrapMap.insert(CPWA, false);
    opCodeMap.insert(CPWX, 168); isUnaryMap.insert(CPWX, false); addrModeRequiredMap.insert(CPWX, true); isTrapMap.insert(CPWX, false);

    opCodeMap.insert(DECI, 48); isUnaryMap.insert(DECI, false); addrModeRequiredMap.insert(DECI, true); isTrapMap.insert(DECI, true);
    opCodeMap.insert(DECO, 56); isUnaryMap.insert(DECO, false); addrModeRequiredMap.insert(DECO, true); isTrapMap.insert(DECO, true);

    opCodeMap.insert(HEXO, 64); isUnaryMap.insert(HEXO, false); addrModeRequiredMap.insert(HEXO, true); isTrapMap.insert(HEXO, true);

    opCodeMap.insert(LDBA, 208); isUnaryMap.insert(LDBA, false); addrModeRequiredMap.insert(LDBA, true); isTrapMap.insert(LDBA, false);
    opCodeMap.insert(LDBX, 216); isUnaryMap.insert(LDBX, false); addrModeRequiredMap.insert(LDBX, true); isTrapMap.insert(LDBX, false);
    opCodeMap.insert(LDWA, 192); isUnaryMap.insert(LDWA, false); addrModeRequiredMap.insert(LDWA, true); isTrapMap.insert(LDWA, false);
    opCodeMap.insert(LDWX, 200); isUnaryMap.insert(LDWX, false); addrModeRequiredMap.insert(LDWX, true); isTrapMap.insert(LDWX, false);

    opCodeMap.insert(MOVAFLG, 5); isUnaryMap.insert(MOVAFLG, true); addrModeRequiredMap.insert(MOVAFLG, true); isTrapMap.insert(MOVAFLG, false);
    opCodeMap.insert(MOVFLGA, 4); isUnaryMap.insert(MOVFLGA, true); addrModeRequiredMap.insert(MOVFLGA, true); isTrapMap.insert(MOVFLGA, false);
    opCodeMap.insert(MOVSPA, 3); isUnaryMap.insert(MOVSPA, true); addrModeRequiredMap.insert(MOVSPA, true); isTrapMap.insert(MOVSPA, false);

    opCodeMap.insert(NEGA, 8); isUnaryMap.insert(NEGA, true); addrModeRequiredMap.insert(NEGA, true); isTrapMap.insert(NEGA, false);
    opCodeMap.insert(NEGX, 9); isUnaryMap.insert(NEGX, true); addrModeRequiredMap.insert(NEGX, true); isTrapMap.insert(NEGX, false);
    opCodeMap.insert(NOP, 40); isUnaryMap.insert(NOP, false); addrModeRequiredMap.insert(NOP, true); isTrapMap.insert(NOP, true);
    opCodeMap.insert(NOP0, 38); isUnaryMap.insert(NOP0, true); addrModeRequiredMap.insert(NOP0, true); isTrapMap.insert(NOP0, true);
    opCodeMap.insert(NOP1, 39); isUnaryMap.insert(NOP1, true); addrModeRequiredMap.insert(NOP1, true); isTrapMap.insert(NOP1, true);
    opCodeMap.insert(NOTA, 6); isUnaryMap.insert(NOTA, true); addrModeRequiredMap.insert(NOTA, true); isTrapMap.insert(NOTA, false);
    opCodeMap.insert(NOTX, 7); isUnaryMap.insert(NOTX, true); addrModeRequiredMap.insert(NOTX, true); isTrapMap.insert(NOTX, false);

    opCodeMap.insert(ORA, 144); isUnaryMap.insert(ORA, false); addrModeRequiredMap.insert(ORA, true); isTrapMap.insert(ORA, false);
    opCodeMap.insert(ORX, 152); isUnaryMap.insert(ORX, false); addrModeRequiredMap.insert(ORX, true); isTrapMap.insert(ORX, false);

    opCodeMap.insert(RET, 1); isUnaryMap.insert(RET, true); addrModeRequiredMap.insert(RET, true); isTrapMap.insert(RET, false);
    opCodeMap.insert(RETTR, 2); isUnaryMap.insert(RETTR, true); addrModeRequiredMap.insert(RETTR, true); isTrapMap.insert(RETTR, false);
    opCodeMap.insert(ROLA, 14); isUnaryMap.insert(ROLA, true); addrModeRequiredMap.insert(ROLA, true); isTrapMap.insert(ROLA, false);
    opCodeMap.insert(ROLX, 15); isUnaryMap.insert(ROLX, true); addrModeRequiredMap.insert(ROLX, true); isTrapMap.insert(ROLX, false);
    opCodeMap.insert(RORA, 16); isUnaryMap.insert(RORA, true); addrModeRequiredMap.insert(RORA, true); isTrapMap.insert(RORA, false);
    opCodeMap.insert(RORX, 17); isUnaryMap.insert(RORX, true); addrModeRequiredMap.insert(RORX, true); isTrapMap.insert(RORX, false);

    opCodeMap.insert(STBA, 240); isUnaryMap.insert(STBA, false); addrModeRequiredMap.insert(STBA, true); isTrapMap.insert(STBA, false);
    opCodeMap.insert(STBX, 248); isUnaryMap.insert(STBX, false); addrModeRequiredMap.insert(STBX, true); isTrapMap.insert(STBX, false);
    opCodeMap.insert(STWA, 224); isUnaryMap.insert(STWA, false); addrModeRequiredMap.insert(STWA, true); isTrapMap.insert(STWA, false);
    opCodeMap.insert(STWX, 232); isUnaryMap.insert(STWX, false); addrModeRequiredMap.insert(STWX, true); isTrapMap.insert(STWX, false);
    opCodeMap.insert(STOP, 0); isUnaryMap.insert(STOP, true); addrModeRequiredMap.insert(STOP, true); isTrapMap.insert(STOP, false);
    opCodeMap.insert(STRO, 72); isUnaryMap.insert(STRO, false); addrModeRequiredMap.insert(STRO, true); isTrapMap.insert(STRO, true);
    opCodeMap.insert(SUBA, 112); isUnaryMap.insert(SUBA, false); addrModeRequiredMap.insert(SUBA, true); isTrapMap.insert(SUBA, false);
    opCodeMap.insert(SUBX, 120); isUnaryMap.insert(SUBX, false); addrModeRequiredMap.insert(SUBX, true); isTrapMap.insert(SUBX, false);
    opCodeMap.insert(SUBSP, 88); isUnaryMap.insert(SUBSP, false); addrModeRequiredMap.insert(SUBSP, true); isTrapMap.insert(SUBSP, false);
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
    int addrMode;
    addrMode = 0;
    if (defaultMnemon0i) addrMode |= I;
    if (defaultMnemon0d) addrMode |= D;
    if (defaultMnemon0n) addrMode |= N;
    if (defaultMnemon0s) addrMode |= S;
    if (defaultMnemon0sf) addrMode |= SF;
    if (defaultMnemon0x) addrMode |= X;
    if (defaultMnemon0sx) addrMode |= SX;
    if (defaultMnemon0sfx) addrMode |= SFX;
    addrModesMap.insert(NOP, addrMode);
    addrMode = 0;
    if (defaultMnemon1i) addrMode |= I;
    if (defaultMnemon1d) addrMode |= D;
    if (defaultMnemon1n) addrMode |= N;
    if (defaultMnemon1s) addrMode |= S;
    if (defaultMnemon1sf) addrMode |= SF;
    if (defaultMnemon1x) addrMode |= X;
    if (defaultMnemon1sx) addrMode |= SX;
    if (defaultMnemon1sfx) addrMode |= SFX;
    addrModesMap.insert(DECI, addrMode);
    addrMode = 0;
    if (defaultMnemon2i) addrMode |= I;
    if (defaultMnemon2d) addrMode |= D;
    if (defaultMnemon2n) addrMode |= N;
    if (defaultMnemon2s) addrMode |= S;
    if (defaultMnemon2sf) addrMode |= SF;
    if (defaultMnemon2x) addrMode |= X;
    if (defaultMnemon2sx) addrMode |= SX;
    if (defaultMnemon2sfx) addrMode |= SFX;
    addrModesMap.insert(DECO, addrMode);
    addrMode = 0;
    if (defaultMnemon3i) addrMode |= I;
    if (defaultMnemon3d) addrMode |= D;
    if (defaultMnemon3n) addrMode |= N;
    if (defaultMnemon3s) addrMode |= S;
    if (defaultMnemon3sf) addrMode |= SF;
    if (defaultMnemon3x) addrMode |= X;
    if (defaultMnemon3sx) addrMode |= SX;
    if (defaultMnemon3sfx) addrMode |= SFX;
    addrModesMap.insert(HEXO, addrMode);
    addrMode = 0;
    if (defaultMnemon3i) addrMode |= I;
    if (defaultMnemon3d) addrMode |= D;
    if (defaultMnemon3n) addrMode |= N;
    if (defaultMnemon3s) addrMode |= S;
    if (defaultMnemon3sf) addrMode |= SF;
    if (defaultMnemon3x) addrMode |= X;
    if (defaultMnemon3sx) addrMode |= SX;
    if (defaultMnemon3sfx) addrMode |= SFX;
    addrModesMap.insert(STRO, addrMode);
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
void decoderTableAHelper(EMnemonic val,int startIdx)
{
    Pep::decodeMnemonic[startIdx] = val; Pep::decodeAddrMode[startIdx] = I;
    Pep::decodeMnemonic[startIdx + 1] = val; Pep::decodeAddrMode[startIdx + 1] = X;
}
void decoderTableAAAHelper(EMnemonic val,int startIdx)
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
void decoderTableHelperTrap(EMnemonic val,int startIdx,int distance){
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

    decoderTableAHelper(BR,18);
    decoderTableAHelper(BRLE,20);
    decoderTableAHelper(BRLT,22);
    decoderTableAHelper(BREQ,24);
    decoderTableAHelper(BRNE,26);
    decoderTableAHelper(BRGE,28);
    decoderTableAHelper(BRGT,30);
    decoderTableAHelper(BRV,32);
    decoderTableAHelper(BRC,34);
    decoderTableAHelper(CALL,36);

    decoderTableHelperTrap(NOP0,38,1);
    decoderTableHelperTrap(NOP1,39,1);
    decoderTableHelperTrap(NOP,40,8);
    decoderTableHelperTrap(DECI,48,8);
    decoderTableHelperTrap(DECO,56,8);
    decoderTableHelperTrap(HEXO,64,8);
    decoderTableHelperTrap(STRO,72,8);

    decoderTableAAAHelper(ADDSP,80);
    decoderTableAAAHelper(SUBSP,88);
    decoderTableAAAHelper(ADDA,96);
    decoderTableAAAHelper(ADDX,104);
    decoderTableAAAHelper(SUBA,112);
    decoderTableAAAHelper(SUBX,120);
    decoderTableAAAHelper(ANDA,128);
    decoderTableAAAHelper(ANDX,136);
    decoderTableAAAHelper(ORA,144);
    decoderTableAAAHelper(ORX,152);
    decoderTableAAAHelper(CPWA,160);
    decoderTableAAAHelper(CPWX,168);
    decoderTableAAAHelper(CPBA,176);
    decoderTableAAAHelper(CPBX,184);
    decoderTableAAAHelper(LDWA,192);
    decoderTableAAAHelper(LDWX,200);
    decoderTableAAAHelper(LDBA,208);
    decoderTableAAAHelper(LDBX,216);
    decoderTableAAAHelper(STWA,224);
    decoderTableAAAHelper(STWX,232);
    decoderTableAAAHelper(STBA,240);
    decoderTableAAAHelper(STBX,248);
}

// .BURN and the ROM state
int Pep::byteCount;
int Pep::burnCount;
int Pep::dotBurnArgument;
int Pep::romStartAddress;

// Memory trace state
bool Pep::traceTagWarning;
