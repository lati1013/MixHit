#ifndef _FILEOPERATIONS_H_
#define _FILEOPERATIONS_H_

#include "FS.h"
#include "SPIFFS.h"

void listDir(fs::FS &fs, const char * dirname, uint8_t levels, String *pTxTFiles);	// Listet alle Verzeichnisse auf und gibt diese in pTxTFiles (mit ';'-Trennzeichen zurueck.
void readFile(fs::FS &fs, const char * path, String* pContent);						// Liest den Inhalt der angegebenen Datei
void writeFile(fs::FS &fs, const char * path, const char * message);				// Schreibt Text in die angegebenen Datei
void appendFile(fs::FS &fs, const char * path, const char * message);				// Fuegt den Text an die angegebene Datei hinzu.
void renameFile(fs::FS &fs, const char * path1, const char * path2);				// Benennt die angegebene Datei um
void deleteFile(fs::FS &fs, const char * path);										// Loescht die angegebene Datei
void testFileIO(fs::FS &fs, const char * path);										// ???

void listDir(fs::FS &fs, String dirname, uint8_t levels, String *pTxTFiles);		// Listet alle Verzeichnisse auf und gibt diese in pTxTFiles (mit ';'-Trennzeichen zurueck.
void readFile(fs::FS &fs, String path, String* pContent);							// Liest den Inhalt der angegebenen Datei
void writeFile(fs::FS &fs, String path, String message);							// Schreibt Text in die angegebenen Datei
void appendFile(fs::FS &fs, String path, String message);							// Fuegt den Text an die angegebene Datei hinzu.
void renameFile(fs::FS &fs, String path1, String path2);							// Benennt die angegebene Datei um
void deleteFile(fs::FS &fs, String path);											// Loescht die angegebene Datei
void testFileIO(fs::FS &fs, String path);											// ???

#endif