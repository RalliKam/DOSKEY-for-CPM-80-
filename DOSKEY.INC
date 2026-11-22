{ Copyright (c) 2026 DOSKEY contributors }
{ SPDX-License-Identifier: MIT }
{ DOSKEY.INC - Z80-Befehle des residenten Editors.
  tag/mark/Name setzt eine Marke, tag/ref/Name liefert ein Adresswort.
  Der Erzeuger entfernt die Marken und setzt die PRL-Relokationsbits.
  Zahlen hinter einem Opcode sind seine unmittelbaren Operanden. }
inline(
  tag/mark/mHook/             { Sprungmarke Hook. }
  $F5/                   { PUSH AF - A und Flags des Aufrufers sichern. }
  $79/                   { LD A,C - C ist die BDOS-Funktionsnummer. }
  $FE/$0A/               { CP 0Ah - Nur gepufferte Zeileneingabe abfangen. }
  $28/$04/               { JR Z,$+6 - Vier Bytes (POP AF und JP NEXT) ueberspringen. }
  $F1/                   { POP AF - Alle Eingangsregister original an NEXT geben. }
  $C3/tag/ref/mNextRS/   { JP NEXT - Direkter Sprung, kein weiterer Stackrahmen. }
  $F1/                   { POP AF - Vergleichswert vom Aufruferstack entfernen. }
  $ED/$73/tag/ref/mOldSP/ { LD (OldSP),SP - Rueckkehradresse auf dem Aufruferstack belassen. }
  $31/tag/ref/mStackEnd/ { LD SP,StackEnd - BDOS und Editor verwenden den eigenen Stack. }
  $7A/                   { LD A,D - Oberes Byte des Pufferzeigers laden. }
  $B3/                   { OR E - DE=0 bezeichnet den vorbelegten DMA-Puffer. }
  $C2/tag/ref/mBuffer/   { JP NZ,Buffer - Normalfall mit expliziter Adresse. }
  $11/tag/ref/mDmaPB/    { LD DE,DmaPB - SCB-Abfrage der aktuellen DMA-Adresse. }
  $0E/$31/               { LD C,49 - BDOS Get/Set SCB. }
  $CD/tag/ref/mNextRS/   { CALL NEXT - Liefert DMA-Adresse in HL. }
  $E5/                   { PUSH HL - Pufferanfang fuer den Leerfall aufheben. }
  $23/                   { INC HL - Laengenbyte ueberspringen ... }
  $23/                   { INC HL - ... HL zeigt nun auf das erste Vorbelegungszeichen. }
  $7E/                   { LD A,(HL) - Null bedeutet leere Vorbelegung. }
  $D1/                   { POP DE - DE zeigt wieder auf den Pufferkopf. }
  $B7/                   { OR A - Enthielt der Puffer bereits Zeichen? }
  $CA/tag/ref/mBuffer/   { JP Z,Buffer - Leeren DMA-Puffer mit History bearbeiten. }
  $11/$00/$00/           { LD DE,0 - Initialisierten DMA-Puffer dem CP/M-Editor geben. }
  tag/mark/mNative/             { Sprungmarke Native. }
  $ED/$7B/tag/ref/mOldSP/ { LD SP,(OldSP) - Urspruenglichen Stack fuer die Weitergabe herstellen. }
  $0E/$0A/               { LD C,10 - Originale Funktion wieder einsetzen. }
  $C3/tag/ref/mNextRS/   { JP NEXT - Betriebssystem bearbeitet den Sonderfall. }
  tag/mark/mBuffer/             { Sprungmarke Buffer. }
  $1A/                   { LD A,(DE) - Maximallaenge aus Byte 0. }
  $B7/                   { OR A - Sonderwert 0 behandelt das native BDOS. }
  $CA/tag/ref/mNative/   { JP Z,Native - DE bleibt der explizite Pufferzeiger. }
  $32/tag/ref/mMaxLen/   { LD (MaxLen),A - Grenze des Aufrufers, maximal 255. }
  $13/                   { INC DE - DE zeigt auf das Ergebnis-Laengenbyte. }
  $ED/$53/tag/ref/mCountPtr/ { LD (CountPtr),DE - Fuer Ergebnis, Kopieren und Vergleich merken. }
  $13/                   { INC DE - DE zeigt auf das erste Textbyte. }
  $ED/$53/tag/ref/mTextPtr/ { LD (TextPtr),DE - Textanfang zum adressierten Zugriff. }
  $11/tag/ref/mWidthPB/  { LD DE,WidthPB - SCB 1Ah/1Bh liefert Breite minus eins und Spalte. }
  $0E/$31/               { LD C,49 - System Control Block lesen. }
  $CD/tag/ref/mNextRS/   { CALL NEXT - L = letzte Spalte, H = aktuelle Spalte. }
  $7D/                   { LD A,L - Letzte physische Spalte. }
  $94/                   { SUB H - Freie Stellen bis vor den rechten Rand. }
  $DA/tag/ref/mNewRow/   { JP C,NewRow - Bei ungueltiger Spalte auf neuer Zeile beginnen. }
  $C2/tag/ref/mGotWidth/ { JP NZ,GotWidth - Rechts vom Prompt steht Platz zur Verfuegung. }
  tag/mark/mNewRow/             { Sprungmarke NewRow. }
  $E5/                   { PUSH HL - Konsolenbreite ueber die Ausgabe retten. }
  $3E/$0D/               { LD A,13 - Zum Zeilenanfang. }
  $CD/tag/ref/mPutA/     { CALL PutA - Wagenruecklauf ausgeben. }
  $3E/$0A/               { LD A,10 - Auf die naechste Bildschirmzeile. }
  $CD/tag/ref/mPutA/     { CALL PutA - Zeilenvorschub ausgeben. }
  $E1/                   { POP HL - L enthaelt wieder die letzte Spalte. }
  $7D/                   { LD A,L - Ganze freie Zeile nutzen. }
  $B7/                   { OR A - Nullbreite bei fehlerhaftem SCB vermeiden. }
  $20/$01/               { JR NZ,$+3 - Positive Breite uebernehmen. }
  $3C/                   { INC A - Mindestbreite 1. }
  tag/mark/mGotWidth/             { Sprungmarke GotWidth. }
  $32/tag/ref/mWidth/    { LD (Width),A - Anzeigefenster endet vor dem automatischen Umbruch. }
  $AF/                   { XOR A - Zustand eines neuen Aufrufs initialisieren. }
  $32/tag/ref/mAge/      { LD (Age),A - Abstand null steht fuer den Entwurf. }
  $32/tag/ref/mScreenPos/ { LD (ScreenPos),A - Sichtbare Cursorposition relativ zum Prompt. }
  $32/tag/ref/mScreenLen/ { LD (ScreenLen),A - Bisher gezeichnete sichtbare Zeichen. }
  $01/$00/$00/           { LD BC,0 - B = Textlaenge, C = Cursorindex, beide null. }
  tag/mark/mKeyLoop/             { Sprungmarke KeyLoop. }
  $CD/tag/ref/mKey/      { CALL Key - Ein Eingabebyte ohne Echo nach A. }
  tag/mark/mDecode/             { Sprungmarke Decode. }
  $FE/$0D/               { CP 13 - Return beendet die Zeile. }
  $CA/tag/ref/mAccept/   { JP Z,Accept - Ergebnis und History aktualisieren. }
  $FE/$0A/               { CP 10 - Auch LF ist bei BDOS 10 ein Zeilenabschluss. }
  $CA/tag/ref/mAccept/   { JP Z,Accept - Gleicher Rueckgabepfad wie bei CR. }
  $FE/$1B/               { CP 27 - Anfang einer VT100-Tastenfolge? }
  $CA/tag/ref/mEscape/   { JP Z,Escape - ESC [ und ESC O auswerten. }
  $FE/$10/               { CP 16 - Ctrl-P als terminalunabhaengiges Zurueckblaettern. }
  $CA/tag/ref/mOlder/    { JP Z,Older - Einen Eintrag aelter. }
  $FE/$0E/               { CP 14 - Ctrl-N als terminalunabhaengiges Vorblaettern. }
  $CA/tag/ref/mNewer/    { JP Z,Newer - Einen Eintrag neuer. }
  $FE/$08/               { CP 8 - Backspace links vom Cursor. }
  $CA/tag/ref/mBack/     { JP Z,Back - Linkes Zeichen loeschen. }
  $FE/$7F/               { CP 127 - ASCII RUBOUT vieler serieller Tastaturen. }
  $CA/tag/ref/mBack/     { JP Z,Back - Wie Backspace behandeln. }
  $FE/$03/               { CP 3 - CP/M-Warmstart am Zeilenanfang. }
  $CA/tag/ref/mBreakKey/ { JP Z,BreakKey - Konsolenmodus beruecksichtigen. }
  $FE/$20/               { CP 32 - Weitere Steuerzeichen ausfiltern. }
  $DA/tag/ref/mKeyLoop/  { JP C,KeyLoop - Keine Steuerbytes im Verlauf speichern. }
  $FE/$7F/               { CP 127 - Der Editor verwendet druckbares 7-Bit-ASCII. }
  $D2/tag/ref/mKeyLoop/  { JP NC,KeyLoop - Andere Terminalcodes ignorieren. }
  $57/                   { LD D,A - Einzufuegendes Zeichen parken. }
  $3A/tag/ref/mMaxLen/   { LD A,(MaxLen) - Pufferkapazitaet des Aufrufers. }
  $B8/                   { CP B - Zeile schon voll? }
  $CA/tag/ref/mKeyLoop/  { JP Z,KeyLoop - Weiterhin CR und Loeschen erlauben. }
  $CD/tag/ref/mCursor/   { CALL Cursor - HL = Textanfang + Cursor, D bleibt erhalten. }
  $5A/                   { LD E,D - E traegt das jeweils rechts einzusetzende Zeichen. }
  $78/                   { LD A,B - Gesamte alte Laenge. }
  $91/                   { SUB C - Anzahl zu verschiebender Zeichen. }
  $CA/tag/ref/mInsertEnd/ { JP Z,InsertEnd - Am Ende direkt anhaengen. }
  $57/                   { LD D,A - D zaehlt den zu verschiebenden Rest. }
  tag/mark/mShiftR/             { Sprungmarke ShiftR. }
  $7E/                   { LD A,(HL) - Bisheriges Zeichen holen, bevor es ueberschrieben wird. }
  $73/                   { LD (HL),E - Mitgetragenes Zeichen einsetzen. }
  $5F/                   { LD E,A - Verdraengtes Zeichen fuer die naechste Stelle retten. }
  $23/                   { INC HL - Eine Position weiter. }
  $15/                   { DEC D - Ein Restzeichen weniger. }
  $C2/tag/ref/mShiftR/   { JP NZ,ShiftR - Bis zum alten Ende wiederholen. }
  tag/mark/mInsertEnd/             { Sprungmarke InsertEnd. }
  $73/                   { LD (HL),E - Letztes Zeichen erreicht den freien Platz. }
  $04/                   { INC B - Laenge um eins erhoehen. }
  $0C/                   { INC C - Cursor hinter das eingefuegte Zeichen stellen. }
  $79/                   { LD A,C - Cursor mit neuer Laenge vergleichen. }
  $B8/                   { CP B - Nur einfaches Anhaengen laesst sich direkt ausgeben. }
  $C2/tag/ref/mDraw/     { JP NZ,Draw - Zeilenmitte neu zeichnen. }
  $3A/tag/ref/mScreenPos/ { LD A,(ScreenPos) - Sichtbare Cursorposition. }
  $21/tag/ref/mWidth/    { LD HL,Width - HL adressiert die Fensterbreite. }
  $BE/                   { CP (HL) - Muss das Fenster nach links rollen? }
  $D2/tag/ref/mDraw/     { JP NC,Draw - Am rechten Rand neu zeichnen. }
  $3C/                   { INC A - Cursor wandert eine Spalte weiter. }
  $32/tag/ref/mScreenPos/ { LD (ScreenPos),A - Neue sichtbare Cursorposition merken. }
  $32/tag/ref/mScreenLen/ { LD (ScreenLen),A - Beim Anhaengen liegt der Cursor am Ende. }
  $7B/                   { LD A,E - Angehaengtes Zeichen fuer die Konsole. }
  $CD/tag/ref/mPutA/     { CALL PutA - Genau ein Byte ueber die serielle Leitung. }
  $C3/tag/ref/mKeyLoop/  { JP KeyLoop - Naechste Taste erwarten. }
  tag/mark/mEscape/             { Sprungmarke Escape. }
  $CD/tag/ref/mKey/      { CALL Key - Zweites Byte einer Escape-Folge. }
  $FE/$5B/               { CP '[' - Normaler VT100-Cursormodus. }
  $CA/tag/ref/mSeq/      { JP Z,Seq - CSI-Folge auswerten. }
  $FE/$4F/               { CP 'O' - VT100-Applikationsmodus. }
  $C2/tag/ref/mDecode/   { JP NZ,Decode - Einzelnes ESC verwerfen, Folgetaste normal behandeln. }
  tag/mark/mSeq/             { Sprungmarke Seq. }
  $CD/tag/ref/mKey/      { CALL Key - Endbyte oder erster Parameter. }
  $FE/$41/               { CP 'A' - Pfeil hoch. }
  $CA/tag/ref/mOlder/    { JP Z,Older - Aelteren Eintrag holen. }
  $FE/$42/               { CP 'B' - Pfeil runter. }
  $CA/tag/ref/mNewer/    { JP Z,Newer - Neueren Eintrag holen. }
  $FE/$43/               { CP 'C' - Pfeil rechts. }
  $CA/tag/ref/mRightKey/ { JP Z,RightKey - Cursor nach rechts bewegen. }
  $FE/$44/               { CP 'D' - Pfeil links. }
  $CA/tag/ref/mLeftKey/  { JP Z,LeftKey - Cursor nach links bewegen. }
  $FE/$33/               { CP '3' - CSI 3 ~ ist die Entf-Taste. }
  $C2/tag/ref/mSkipSeq/  { JP NZ,SkipSeq - Andere Folge bis zum Endbyte konsumieren. }
  $CD/tag/ref/mKey/      { CALL Key - Abschliessende Tilde pruefen. }
  $FE/$7E/               { CP '~' - Vollstaendige Entf-Sequenz? }
  $CA/tag/ref/mDelKey/   { JP Z,DelKey - Zeichen unter dem Cursor loeschen. }
  tag/mark/mSkipSeq/             { Sprungmarke SkipSeq. }
  $FE/$20/               { CP 32 - CR, LF oder ESC beendet eine unvollstaendige Folge. }
  $DA/tag/ref/mDecode/   { JP C,Decode - Steuertaste normal behandeln. }
  $FE/$40/               { CP 64 - ANSI-Endbytes liegen ab 40h. }
  $D2/tag/ref/mKeyLoop/  { JP NC,KeyLoop - Unbekannte abgeschlossene Folge verwerfen. }
  $CD/tag/ref/mKey/      { CALL Key - Parameter- oder Zwischenbyte uebergehen. }
  $C3/tag/ref/mSkipSeq/  { JP SkipSeq - Rest derselben Folge lesen. }
  tag/mark/mLeftKey/             { Sprungmarke LeftKey. }
  $79/                   { LD A,C - Cursorindex testen. }
  $B7/                   { OR A - Bereits vor dem ersten Zeichen? }
  $CA/tag/ref/mKeyLoop/  { JP Z,KeyLoop - Linke Grenze einhalten. }
  $0D/                   { DEC C - Cursor ein Zeichen nach links. }
  $C3/tag/ref/mDraw/     { JP Draw - Anzeige anpassen. }
  tag/mark/mRightKey/             { Sprungmarke RightKey. }
  $79/                   { LD A,C - Cursorindex holen. }
  $B8/                   { CP B - Schon am Textende? }
  $CA/tag/ref/mKeyLoop/  { JP Z,KeyLoop - Rechte Grenze einhalten. }
  $0C/                   { INC C - Cursor ein Zeichen nach rechts. }
  $C3/tag/ref/mDraw/     { JP Draw - Anzeige anpassen. }
  tag/mark/mBack/             { Sprungmarke Back. }
  $79/                   { LD A,C - Position vor dem Loeschen testen. }
  $B7/                   { OR A - Am linken Rand existiert kein Vorgaenger. }
  $CA/tag/ref/mKeyLoop/  { JP Z,KeyLoop - Leeres Backspace ignorieren. }
  $0D/                   { DEC C - Zu loeschendes Zeichen links auswaehlen. }
  tag/mark/mDelKey/             { Sprungmarke DelKey. }
  $78/                   { LD A,B - Gesamte Laenge holen. }
  $91/                   { SUB C - Zahl der Zeichen ab Cursor. }
  $CA/tag/ref/mKeyLoop/  { JP Z,KeyLoop - Entf am Ende ist wirkungslos. }
  $3D/                   { DEC A - Restlaenge hinter dem zu loeschenden Zeichen. }
  $CA/tag/ref/mDelDone/  { JP Z,DelDone - Letztes Zeichen braucht keine Verschiebung. }
  $57/                   { LD D,A - D zaehlt die verbleibenden Zeichen. }
  $CD/tag/ref/mCursor/   { CALL Cursor - HL zeigt auf das zu loeschende Zeichen. }
  tag/mark/mShiftL/             { Sprungmarke ShiftL. }
  $23/                   { INC HL - Nachfolger adressieren. }
  $7E/                   { LD A,(HL) - Nachfolger holen. }
  $2B/                   { DEC HL - Zur Luecke zurueckgehen. }
  $77/                   { LD (HL),A - Luecke mit dem Nachfolger schliessen. }
  $23/                   { INC HL - Neue Luecke rechts. }
  $15/                   { DEC D - Ein Zeichen weniger zu verschieben. }
  $C2/tag/ref/mShiftL/   { JP NZ,ShiftL - Rest nach links ruecken. }
  tag/mark/mDelDone/             { Sprungmarke DelDone. }
  $05/                   { DEC B - Neue Zeile ist ein Zeichen kuerzer. }
  $C3/tag/ref/mDraw/     { JP Draw - Altes Endzeichen auch am Terminal entfernen. }
  tag/mark/mOlder/             { Sprungmarke Older. }
  $3A/tag/ref/mAge/      { LD A,(Age) - Abstand vom noch offenen Entwurf. }
  $21/tag/ref/mUsed/     { LD HL,Used - Anzahl gueltiger Ringplaetze. }
  $BE/                   { CP (HL) - Schon am aeltesten Eintrag? }
  $CA/tag/ref/mKeyLoop/  { JP Z,KeyLoop - Dort stehen bleiben. }
  $B7/                   { OR A - Zum ersten Mal den Entwurf verlassen? }
  $C2/tag/ref/mAgeUp/    { JP NZ,AgeUp - Entwurf wurde bereits gesichert. }
  $11/tag/ref/mDraft/    { LD DE,Draft - Ablage fuer Laenge und Entwurfstext. }
  $CD/tag/ref/mSave/     { CALL Save - Kopiert exakt B Zeichen. }
  $79/                   { LD A,C - Auch die Cursorposition des Entwurfs sichern. }
  $32/tag/ref/mDraftPos/ { LD (DraftPos),A - Beim Zurueckblaettern wiederherstellen. }
  $AF/                   { XOR A - Ausgangsalter null. }
  tag/mark/mAgeUp/             { Sprungmarke AgeUp. }
  $3C/                   { INC A - Um einen Eintrag altern. }
  $C3/tag/ref/mRecall/   { JP Recall - Ringplatz laden. }
  tag/mark/mNewer/             { Sprungmarke Newer. }
  $3A/tag/ref/mAge/      { LD A,(Age) - Aktuellen Abstand holen. }
  $B7/                   { OR A - Bereits beim Entwurf? }
  $CA/tag/ref/mKeyLoop/  { JP Z,KeyLoop - Nichts zu tun. }
  $3D/                   { DEC A - Einen Schritt zum Entwurf. }
  tag/mark/mRecall/             { Sprungmarke Recall. }
  $32/tag/ref/mAge/      { LD (Age),A - Neue Auswahl merken. }
  $B7/                   { OR A - Bei null den Entwurf laden. }
  $21/tag/ref/mDraft/    { LD HL,Draft - HL zeigt zunaechst auf den Entwurf. }
  $C4/tag/ref/mSlot/     { CALL NZ,Slot - Sonst HL = Ringplatz zum Alter A. }
  $3A/tag/ref/mMaxLen/   { LD A,(MaxLen) - Grenze des aktuellen Aufrufers. }
  $BE/                   { CP (HL) - Gespeicherter Text laenger als der Zielpuffer? }
  $38/$01/               { JR C,$+3 - Dann hoechstens MaxLen Zeichen kopieren. }
  $7E/                   { LD A,(HL) - Sonst die gespeicherte Laenge verwenden. }
  $47/                   { LD B,A - Neue Textlaenge. }
  $4F/                   { LD C,A - Cursor zunaechst ans Zeilenende. }
  $23/                   { INC HL - Quelle hinter dem gespeicherten Laengenbyte. }
  $ED/$5B/tag/ref/mTextPtr/ { LD DE,(TextPtr) - Ziel ist der Aufruferpuffer. }
  $CD/tag/ref/mCopyA/    { CALL CopyA - Genau A Zeichen uebertragen. }
  $3A/tag/ref/mAge/      { LD A,(Age) - Wurde der Entwurf geladen? }
  $B7/                   { OR A - Null steht fuer den Entwurf. }
  $C2/tag/ref/mDraw/     { JP NZ,Draw - History-Eintraege mit Cursor am Ende anzeigen. }
  $3A/tag/ref/mDraftPos/ { LD A,(DraftPos) - Alte Cursorposition des Entwurfs. }
  $4F/                   { LD C,A - Cursor zuruecksetzen. }
  $C3/tag/ref/mDraw/     { JP Draw - Auswahl darstellen. }
  tag/mark/mAccept/             { Sprungmarke Accept. }
  $2A/tag/ref/mCountPtr/ { LD HL,(CountPtr) - Ergebnis-Laengenbyte des Aufrufers. }
  $70/                   { LD (HL),B - Anzahl ohne abschliessendes CR/LF zurueckgeben. }
  $3E/$0D/               { LD A,13 - Der CP/M-Zeileneditor echot CR. }
  $CD/tag/ref/mPutA/     { CALL PutA - LF uebernimmt der Aufrufer. }
  $78/                   { LD A,B - Eingabelaenge beurteilen. }
  $B7/                   { OR A - Leere Eingabe bleibt ausserhalb des Rings. }
  $CA/tag/ref/mDone/     { JP Z,Done - Kein Eintrag. }
  $FE/$51/               { CP 81 - Ein Ringplatz fasst 80 Zeichen. }
  $D2/tag/ref/mDone/     { JP NC,Done - Lange Eingaben voll liefern, nie verstuemmelt merken. }
  $3A/tag/ref/mUsed/     { LD A,(Used) - Gibt es bereits einen Vorgaenger? }
  $B7/                   { OR A - Ersten Eintrag sofort ablegen. }
  $CA/tag/ref/mStore/    { JP Z,Store - Kein Vergleich noetig. }
  $3E/$01/               { LD A,1 - Alter eins ist der zuletzt gespeicherte Befehl. }
  $CD/tag/ref/mSlot/     { CALL Slot - HL zeigt auf dessen Laenge. }
  $78/                   { LD A,B - Aktuelle Laenge. }
  $BE/                   { CP (HL) - Unterschiedliche Laenge schliesst Gleichheit aus. }
  $C2/tag/ref/mStore/    { JP NZ,Store - Neuen Eintrag schreiben. }
  $23/                   { INC HL - Erstes Zeichen des Vorgaengers. }
  $ED/$5B/tag/ref/mTextPtr/ { LD DE,(TextPtr) - DE zeigt auf die aktuelle Zeile. }
  $48/                   { LD C,B - Vergleichszaehler, Cursor wird nicht mehr gebraucht. }
  tag/mark/mCompare/             { Sprungmarke Compare. }
  $1A/                   { LD A,(DE) - Ein Zeichen der Eingabe holen. }
  $BE/                   { CP (HL) - Bytegenau mit dem Vorgaenger vergleichen. }
  $C2/tag/ref/mStore/    { JP NZ,Store - Erster Unterschied reicht aus. }
  $13/                   { INC DE - Beide Zeiger vorruecken. }
  $23/                   { INC HL - Naechstes History-Zeichen. }
  $0D/                   { DEC C - Vergleichslaenge vermindern. }
  $C2/tag/ref/mCompare/  { JP NZ,Compare - Rest vergleichen. }
  $C3/tag/ref/mDone/     { JP Done - Vollstaendig gleich, Ring bleibt unveraendert. }
  tag/mark/mStore/             { Sprungmarke Store. }
  $AF/                   { XOR A - Alter null adressiert den naechsten Schreibplatz. }
  $CD/tag/ref/mSlot/     { CALL Slot - HL zeigt auf diesen Ringplatz. }
  $EB/                   { EX DE,HL - DE wird das Ziel fuer Save. }
  $CD/tag/ref/mSave/     { CALL Save - Laenge und Inhalt ablegen. }
  $3A/tag/ref/mHead/     { LD A,(Head) - Bisheriger Schreibindex. }
  $3C/                   { INC A - Einen Platz weiter. }
  $E6/$07/               { AND 7 - Modulo acht umbrechen. }
  $32/tag/ref/mHead/     { LD (Head),A - Naechsten Schreibplatz merken. }
  $21/tag/ref/mUsed/     { LD HL,Used - Anzahl belegter Plaetze. }
  $7E/                   { LD A,(HL) - Bisherige Belegung. }
  $FE/$08/               { CP 8 - Ist der Ring voll? }
  $30/$01/               { JR NC,$+3 - Bei acht bleibt die Belegung konstant. }
  $34/                   { INC (HL) - Einen neu belegten Platz zaehlen. }
  tag/mark/mDone/             { Sprungmarke Done. }
  $ED/$7B/tag/ref/mOldSP/ { LD SP,(OldSP) - Aufruferstack wieder einsetzen. }
  $C9/                   { RET - Funktion 10 liefert ihr Ergebnis im Puffer. }
  tag/mark/mSlot/             { Sprungmarke Slot. }
  $57/                   { LD D,A - D = Alter, null bezeichnet Schreibplatz. }
  $3A/tag/ref/mHead/     { LD A,(Head) - Index des naechsten Schreibplatzes. }
  $92/                   { SUB D - Alter vom Schreibindex abziehen. }
  $E6/$07/               { AND 7 - Auch negative Differenzen modulo acht abbilden. }
  $21/tag/ref/mRing/     { LD HL,Ring - Basisadresse aller acht Plaetze. }
  $C8/                   { RET Z - Slot null liegt direkt an der Basis. }
  $11/$51/$00/           { LD DE,81 - Ein Laengenbyte und achtzig Textbytes pro Platz. }
  tag/mark/mSlotLoop/             { Sprungmarke SlotLoop. }
  $19/                   { ADD HL,DE - Einen Platz weiterruecken. }
  $3D/                   { DEC A - Anzahl verbleibender Schritte. }
  $C2/tag/ref/mSlotLoop/ { JP NZ,SlotLoop - Hoechstens sieben Schritte. }
  $C9/                   { RET - BC blieb als Editorzustand erhalten. }
  tag/mark/mSave/             { Sprungmarke Save. }
  $78/                   { LD A,B - Anzahl der Textbytes. }
  $12/                   { LD (DE),A - Erstes Zielbyte ist die Laenge. }
  $13/                   { INC DE - Ziel fuer das erste Textbyte. }
  $2A/tag/ref/mTextPtr/  { LD HL,(TextPtr) - Quelle ist die bearbeitete Zeile. }
  tag/mark/mCopyA/             { Sprungmarke CopyA. }
  $B7/                   { OR A - A Zeichen von HL nach DE. }
  $C8/                   { RET Z - LDIR mit BC=0 wuerde 65536 Bytes kopieren. }
  $C5/                   { PUSH BC - Laenge und Cursor sichern. }
  $4F/                   { LD C,A - Anzahl in das untere Zaehlerbyte. }
  $06/$00/               { LD B,0 - Oberes Zaehlerbyte null. }
  $ED/$B0/               { LDIR - BC Zeichen kopieren und beide Zeiger vorruecken. }
  $C1/                   { POP BC - Editorzustand wiederherstellen. }
  $C9/                   { RET - Kopie beendet. }
  tag/mark/mCursor/             { Sprungmarke Cursor. }
  $2A/tag/ref/mTextPtr/  { LD HL,(TextPtr) - Textbasis laden. }
  $D5/                   { PUSH DE - Aufruferwerte in D und E schuetzen. }
  $59/                   { LD E,C - Cursorindex als Offset. }
  $16/$00/               { LD D,0 - Offset auf 16 Bit erweitern. }
  $19/                   { ADD HL,DE - HL zeigt auf die Cursorstelle. }
  $D1/                   { POP DE - Urspruengliches DE. }
  $C9/                   { RET - BC unveraendert. }
  tag/mark/mDraw/             { Sprungmarke Draw. }
  $C5/                   { PUSH BC - Logische Laenge und Cursor waehrend des Zeichnens aufheben. }
  $3A/tag/ref/mScreenPos/ { LD A,(ScreenPos) - Aktuelle sichtbare Cursorposition. }
  $CD/tag/ref/mGoLeft/   { CALL GoLeft - Zum Anfang des Eingabefelds, direkt hinter den Prompt. }
  $3A/tag/ref/mWidth/    { LD A,(Width) - Maximal sichtbare Zeichen. }
  $B9/                   { CP C - Passt die logische Cursorposition ins erste Fenster? }
  $30/$01/               { JR NC,$+3 - Passt der Cursor ins erste Fenster, C beibehalten. }
  $4F/                   { LD C,A - Sichtbaren Cursor auf die Fensterbreite beschraenken. }
  $79/                   { LD A,C - Sichtbaren Cursorindex sichern. }
  $32/tag/ref/mScreenPos/ { LD (ScreenPos),A - Position nach dem Neuzeichnen. }
  $C1/                   { POP BC - Logische Cursorposition erneut laden. }
  $C5/                   { PUSH BC - Am Ende des Zeichnens wiederherstellen. }
  $3A/tag/ref/mScreenPos/ { LD A,(ScreenPos) - Sichtbare Cursorposition. }
  $57/                   { LD D,A - Subtrahend fuer den Fensteroffset. }
  $79/                   { LD A,C - Logischer Cursor. }
  $92/                   { SUB D - Fensterbeginn = logischer minus sichtbarer Cursor. }
  $5F/                   { LD E,A - Offset des ersten sichtbaren Zeichens. }
  $16/$00/               { LD D,0 - 16-Bit-Offset DE. }
  $78/                   { LD A,B - Logische Textlaenge. }
  $93/                   { SUB E - Ab dem Fensterbeginn verfuegbare Zeichen. }
  $21/tag/ref/mWidth/    { LD HL,Width - HL zeigt zum Vergleich auf die Fensterbreite. }
  $BE/                   { CP (HL) - Muss die Ausgabe rechts abgeschnitten werden? }
  $38/$01/               { JR C,$+3 - Verfuegbare Laenge passt bereits ins Fenster. }
  $7E/                   { LD A,(HL) - Sonst auf Fensterbreite kuerzen. }
  $4F/                   { LD C,A - Neue sichtbare Textlaenge merken. }
  $2A/tag/ref/mTextPtr/  { LD HL,(TextPtr) - Textbasis fuer die Ausgabe. }
  $19/                   { ADD HL,DE - Auf erstes sichtbares Zeichen stellen. }
  $B7/                   { OR A - Leeres Fenster? }
  $CA/tag/ref/mTail/     { JP Z,Tail - Direkt alte Zeichen entfernen. }
  $47/                   { LD B,A - Ausgabezaehler. }
  tag/mark/mPrint/             { Sprungmarke Print. }
  $7E/                   { LD A,(HL) - Sichtbares ASCII-Zeichen holen. }
  $CD/tag/ref/mPutA/     { CALL PutA - Zeichen ausgeben, BC/DE/HL bleiben erhalten. }
  $23/                   { INC HL - Zum naechsten Zeichen. }
  $10/$F9/               { DJNZ Print - Sieben Bytes zum Anfang der Druckschleife zurueck. }
  tag/mark/mTail/             { Sprungmarke Tail. }
  $3A/tag/ref/mScreenLen/ { LD A,(ScreenLen) - Alte sichtbare Laenge. }
  $91/                   { SUB C - Eventuell noch sichtbarer alter Ueberhang. }
  $DA/tag/ref/mAtNewEnd/ { JP C,AtNewEnd - Bei gewachsenem Text keine Leerzeichen schreiben. }
  $CA/tag/ref/mAtNewEnd/ { JP Z,AtNewEnd - Gleiche Laenge braucht ebenfalls kein Auffuellen. }
  $47/                   { LD B,A - Zahl zu loeschender alter Zeichen. }
  tag/mark/mBlank/             { Sprungmarke Blank. }
  $3E/$20/               { LD A,32 - Leerzeichen loescht ein altes Zeichen. }
  $CD/tag/ref/mPutA/     { CALL PutA - Leerstelle ausgeben. }
  $10/$F9/               { DJNZ Blank - Bis zum Ende der alten Darstellung. }
  $3A/tag/ref/mScreenLen/ { LD A,(ScreenLen) - Bildschirmcursor nach dem Auffuellen. }
  $C3/tag/ref/mRewind/   { JP Rewind - Zur neuen Cursorposition zurueck. }
  tag/mark/mAtNewEnd/             { Sprungmarke AtNewEnd. }
  $79/                   { LD A,C - Bei gewachsenem Text steht der Cursor am neuen Ende. }
  tag/mark/mRewind/             { Sprungmarke Rewind. }
  $57/                   { LD D,A - Physische Endposition merken. }
  $79/                   { LD A,C - Neue sichtbare Textlaenge. }
  $32/tag/ref/mScreenLen/ { LD (ScreenLen),A - Fuer das naechste Neuzeichnen sichern. }
  $3A/tag/ref/mScreenPos/ { LD A,(ScreenPos) - Gewuenschte Cursorposition. }
  $5F/                   { LD E,A - Zielspalte relativ zum Prompt. }
  $7A/                   { LD A,D - Momentane physische Spalte. }
  $93/                   { SUB E - Erforderliche Anzahl BS. }
  $CD/tag/ref/mGoLeft/   { CALL GoLeft - Cursor an die Bearbeitungsstelle stellen. }
  $C1/                   { POP BC - Logische Textlaenge und Cursor zurueckholen. }
  $C3/tag/ref/mKeyLoop/  { JP KeyLoop - Mit aktualisierter Anzeige weiterlesen. }
  tag/mark/mGoLeft/             { Sprungmarke GoLeft. }
  $B7/                   { OR A - A ist die Anzahl der Backspaces. }
  $C8/                   { RET Z - Bei null sofort fertig. }
  $47/                   { LD B,A - Schleifenzaehler (B wird verbraucht). }
  tag/mark/mBsLoop/             { Sprungmarke BsLoop. }
  $3E/$08/               { LD A,8 - Ein Backspace bewegt den Cursor eine Spalte links. }
  $CD/tag/ref/mPutA/     { CALL PutA - Das Fenster bleibt auf derselben Bildschirmzeile. }
  $10/$F9/               { DJNZ BsLoop - Bis zum Anfang bzw. zur Zielspalte. }
  $C9/                   { RET - Zielspalte erreicht. }
  tag/mark/mBreakKey/             { Sprungmarke BreakKey. }
  $78/                   { LD A,B - Ist die Zeile leer? }
  $B7/                   { OR A - Ctrl-C nur am Zeilenanfang als Abbruch behandeln. }
  $C2/tag/ref/mKeyLoop/  { JP NZ,KeyLoop - Auf nichtleerer Zeile ignorieren. }
  $C5/                   { PUSH BC - Bei deaktiviertem Warmstart weiter editieren. }
  $11/$FF/$FF/           { LD DE,0FFFFh - Aktuellen Konsolenmodus abfragen. }
  $0E/$6D/               { LD C,109 - BDOS Get/Set Console Mode. }
  $CD/tag/ref/mNextRS/   { CALL NEXT - Modus im Registerpaar HL. }
  $CB/$5D/               { BIT 3,L - Gesetztes Bit 3 sperrt den Warmstart. }
  $C1/                   { POP BC - Editorzustand retten, Flags bleiben erhalten. }
  $C2/tag/ref/mKeyLoop/  { JP NZ,KeyLoop - Bei Sperre auf weitere Eingabe warten. }
  $ED/$7B/tag/ref/mOldSP/ { LD SP,(OldSP) - Aufruferstack fuer CP/M wieder einsetzen. }
  $0E/$00/               { LD C,0 - BDOS System Reset. }
  $C3/tag/ref/mNextRS/   { JP NEXT - Warmstart, History bleibt resident. }
  tag/mark/mKey/             { Sprungmarke Key. }
  $C5/                   { PUSH BC - Logische Laenge und Cursor ueber den BDOS-Aufruf retten. }
  $0E/$06/               { LD C,6 - Direct Console I/O ohne Echo. }
  $1E/$FD/               { LD E,0FDh - Unter CP/M 3 blockierend ein Zeichen lesen. }
  $CD/tag/ref/mNextRS/   { CALL NEXT - A enthaelt das gelesene Byte. }
  $C1/                   { POP BC - Editorzustand wiederherstellen. }
  $C9/                   { RET - Eingabe in A liefern. }
  tag/mark/mPutA/             { Sprungmarke PutA. }
  $C5/                   { PUSH BC - Textlaenge und Cursor bzw. Schleifenzaehler schuetzen. }
  $D5/                   { PUSH DE - Zeiger und Zwischenwerte schuetzen. }
  $E5/                   { PUSH HL - Textzeiger schuetzen. }
  $5F/                   { LD E,A - E = auszugebendes Zeichen. }
  $0E/$02/               { LD C,2 - Console Output, BDOS fuehrt den Spaltenzaehler mit. }
  $CD/tag/ref/mNextRS/   { CALL NEXT - Eigene Aufrufe durch den NEXT-Vektor der RSX. }
  $E1/                   { POP HL - Textzeiger zurueckholen. }
  $D1/                   { POP DE - Zwischenwerte zurueckholen. }
  $C1/                   { POP BC - Editorzustand zurueckholen. }
  $C9                   { RET - Zur aufrufenden Editorfunktion. }
);
