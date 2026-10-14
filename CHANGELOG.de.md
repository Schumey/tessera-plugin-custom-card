# Änderungen

## 0.5.0 - 2026-10-14
- Große Zahlen auf großen Tiles: Wo sie passen und alle Werte Zahlen sind, bekommen die Werte die großen Ziffern
  der Klimakarte von Tessera. Ein Wert mit einem Wort darin behält seine bisherige Schrift.
- Grün, Gelb, Orange, Rot und Blau sind Tesseras eigene Farben nach Bedeutung (gut, mäßig, schlecht, Akzent), hell und
  dunkel auf der Karte lesbar, wie Tessera einen Wert einfärbt.
- Wo auf einer kleinen Kachel nichts passt, stehen die Werte zu zweit oder einzeln untereinander, statt in einer Zeile
  abgeschnitten zu werden.
- Ein Text neben anderen Teilen auf einer flachen Kachel bleibt ganz, wo die Teile stattdessen untereinander stehen
  können; Labels gehen weiterhin vor.
- Braucht Tessera 0.4.88 oder neuer: eine ältere bietet weiter 0.4.3 an.

## 0.4.3 - 2026-10-12
- Bis zu acht Teile auf einer Karte statt sechs: zwei volle Reihen mit vier Werten. Jede Kachel der Karte kostet den
  Bildschirm dafür etwas mehr Speicher.
- Textzeilen und Balken über die ganze Breite werden auf einer niedrigen Kachel mit den Werten kleiner, statt unten
  herauszulaufen; wo selbst die kleinste Größe zu groß ist, zeigt die Karte die Zeilen, die ganz passen.

## 0.4.2 - 2026-10-12
- Ein Bild auf der Plugins-Seite.

## 0.4.1 - 2026-10-11
- Ein farbiges Icon wird im hellen Modus dunkler, wo es sich sonst nicht von der Karte abhebt: ein gelbes wird auf Weiß ocker.

## 0.4.0 - 2026-10-10
- Ein Tippen auf das Tile öffnet die Teile groß auf einer Karte über der Seite, mit ihren Labels.
- Braucht ein Tessera vom 10.10.2026 oder neuer: ein älteres zeigt die Karte unter ihrer Titelzeile leer.

## 0.3.3 - 2026-10-09
- Auf der Plugins-Seite unter dem Thema Zuhause zu finden.
- Braucht ein Tessera mit Plugin-API 0.7: erst die App aktualisieren.

## 0.3.2 - 2026-10-09
- Ein Label ohne Icon und ohne Farbe bleibt immer, so sagt jede Zahl, was sie ist.
- Ein Icon oder Punkt vor einer Zahl hat etwas mehr Abstand zu ihr.

## 0.3.1 - 2026-10-09
- Der Inspector des Tiles bietet nur noch Sensoren an, die eine Karte beschreiben (mit einem Attribut `elements`).

## 0.3.0 - 2026-10-09
- Erste Version: Werte, Balken und Textzeilen, die ein Template-Sensor beschreibt, nebeneinander auf einem breiten
  Tile und gestapelt auf jedem anderen, in den Schriften und Farben des Bildschirms.
