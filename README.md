# MixHit
Cocktail-Mixer HS-Karlsruhe

Dies ist das Github-Repository für den Cocktail-Mixer an der Hochschule unter der Aufsicht von Jürgen Walter.

## Klonen des Projektes:
Das Klonen des Github-Repository wird hier beschrieben: https://help.github.com/articles/cloning-a-repository/
Eine Einleitung zu Git findet sich hier: http://rogerdudler.github.io/git-guide/index.de.html

## Abhängigkeiten herunterladen:
Für das Projekt wird eine Bibliothek benötigt, welche [hier](http://hit-karlsruhe.de/hit-info/info-ws17/MixHit_ESP32_3/0407Quellcode.html "Link zum Quellcode HIT-Karlsruhe") zu finden ist. (Rechts die Bibliothekt runterladen)
Diese Bibliothekt muss unter C:\Users\<user>\Documents\Arduino\libraries entpackt werden. Der Ordner ist nach der Installation der Arduino-Umgebung vorhanden.

## Einrichtung unter Visual Studio 2017
1. Benötigt wird Visual Studio 2017. Dazu wird die [Arduino-Erweiterung](https://marketplace.visualstudio.com/items?itemName=VisualMicro.ArduinoIDEforVisualStudio "Visual Studio Marketplace") benötigt.
2. Nach dem Start von Visual Studio 2017 wird das Projekt "MixHit" ausgewählt.
3. Unter der Menü-Leiste ist eine neue Leiste entstanden. Dies ist die Leiste von der Arduino-Erweiterung.
Bei dieser Leiste sollten folgende Einträge geändert werden:
    * Erster Eintrag: Auf "Arduino 1.6/1.8"
    * Zweiter Eintrag: Find and install Board:
        * Tab "Manage Boards" auswählen
        * Das Paket "esp32" auswählen
        * Warten - das dauert lange und hat keinen Fortschrittsbalken
        * wenn fertig: Visual Studio neu starten
        * Nun im 2. Eintrag "ESP32 Dev Module" auswählen.
4. Unter dem Punkt "Add Library" den Punkt "Download and Install Arduino Library" auswählen und folgende Libraries hinzufügen:
        * ESP32
        * FS
        * SX1509 IO Expander
        * SPIFFS
        * WiFi
        * 
5. Das Projekt lässt sich nun Kompilieren

## Einrichtung unter Arduino IDE
1. Die Arduino IDE lässt sich [hier](https://www.arduino.cc/en/Main/Software "Arduino IDE") herunterladen. Bitte auch installieren.
2. Die Abhängigkeiten (siehe oben) sollten nach der Installation eingebunden werden.
3. Durch Doppelklick auf die MixHit.ino-Datei, sollte sich die Arduino IDE öffnen. Alternativ kann auch die ArduinoIDE gestartet werden und über Datei -> Öffnen zu der Datei navigiert werden.
4. Richtiges Board auswählen:
    * Im Menü: Werkzeuge -> Board "xxx" -> Boardverwalter
    * Nach "esp32" suchen und diese durch anklicken instalieren
    * Unter Werkzeuge -> Board "xxx" das Board ESP32 Dev Module auswählen
5. Über die Schaltfläche "Überprüfen" (unter Menü / Strg+R) kann das Projekt nun kompiliert werden.
