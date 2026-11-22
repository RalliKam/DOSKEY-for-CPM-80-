{ Copyright (c) 2026 DOSKEY contributors }
{ SPDX-License-Identifier: MIT }
program MkDoskey;
{ Turbo Pascal 3.0, CP/M-80. Erzeugt DOSKEY.RSX fuer GENCOM.
  Nur das durch Bild() beschriebene Abbild wird installiert.
  Alle Pascal-Variablen und die Laufzeitbibliothek sind transient. }
const
  tag=$ED; mark=$00; ref=$01;  { ED 00/01 sind im Z80-Code unbenutzt. }
  mHook=1; mNative=2; mBuffer=3; mNewRow=4; mGotWidth=5; mKeyLoop=6;
  mDecode=7; mShiftR=8; mInsertEnd=9; mEscape=10; mSeq=11;
  mSkipSeq=12; mLeftKey=13; mRightKey=14; mBack=15; mDelKey=16;
  mShiftL=17; mDelDone=18; mOlder=19; mAgeUp=20; mNewer=21;
  mRecall=22; mAccept=23; mCompare=24; mStore=25; mDone=26;
  mSlot=27; mSlotLoop=28; mSave=29; mCopyA=30; mCursor=31; mDraw=32;
  mPrint=33; mTail=34; mBlank=35; mAtNewEnd=36; mRewind=37;
  mGoLeft=38; mBsLoop=39; mBreakKey=40; mKey=41; mPutA=42;
  mNextRS=43; mOldSP=44; mStackEnd=45; mDmaPB=46; mCountPtr=47;
  mTextPtr=48; mWidthPB=49; mWidth=50; mMaxLen=51; mAge=52;
  mScreenPos=53; mScreenLen=54; mUsed=55; mDraft=56; mDraftPos=57;
  mRing=58; mHead=59;
  LastMark=59;
  Capacity=2048;               { Obergrenze nur im Erzeuger. }
type
  Sector=array[0..127] of byte;
var
  Data: array[0..2047] of byte;
  Bits: array[0..255] of byte;
  Address: array[1..LastMark] of integer;
  First, Limit, Pass, Pc, I: integer;
  OutFile: file;
  Buf: Sector;
  Fill: integer;

procedure Fail;
begin
  writeln('Fehler im RSX-Abbild.');
  halt
end;

procedure Locate;
label 1;
begin
  { * bezeichnet die Adresse des gerade emittierten Inline-Operanden.
    First zeigt auf den folgenden Pascal-GOTO. Er ueberspringt saemtliche
    Datenbytes beim Aufruf dieser Prozedur. Limit zeigt hinter die Vorlage. }
  inline($21/*+5/       { LD HL,*+5 - HL = Adresse hinter LD (First),HL. }
         $22/First);   { LD (First),HL - 22h, dann Variablenadresse L/H. }
  goto 1;
  {$I DOSKEY.INC}
1:
  inline($21/*-1/       { LD HL,*-1 - Adresse des Opcodes 21h selbst. }
         $22/Limit)    { LD (Limit),HL - Ende der Vorlage exklusiv. }
end;

procedure Define(N: byte);
begin
  if (N=0) or (N>LastMark) then Fail;
  if Pass=1 then begin
    if Address[N]<>-1 then Fail;
    Address[N]:=Pc
  end else if Address[N]<>Pc then Fail
end;

procedure Emit(V: byte);
begin
  if Pc>=Capacity then Fail;
  Data[Pc]:=V;
  Pc:=Pc+1
end;

procedure WordRef(N: byte);
var W: integer;
begin
  { Eine PRL-Datei ist auf 0100h bezogen. Nur das hohe Adressbyte
    aendert sich beim Verschieben auf eine andere 256-Byte-Seite.
    Bit 7 beschreibt das erste Byte jeder Achtergruppe. }
  if (N=0) or (N>LastMark) then Fail;
  W:=0;
  if Pass=2 then begin
    if Address[N]<0 then Fail;
    W:=$0100+Address[N]
  end;
  Emit(lo(W));
  Bits[Pc div 8]:=Bits[Pc div 8] or ($80 shr (Pc mod 8));
  Emit(hi(W))
end;

procedure Zeros(N: integer);
begin
  while N>0 do begin Emit(0); N:=N-1 end
end;

procedure Bild;
const RsxName: string[8]='DOSKEY  ';
var P,J: integer;
begin
  Pc:=0;
  Zeros(6);              { 00h Seriennummer, vom LOADER eingesetzt. }
  Emit($C3); WordRef(mHook); { 06h JP Hook, offizieller BDOS-Einsprung. }
  Define(mNextRS); Emit($C3); Zeros(2); { 09h JP NEXT, Ziel vom LOADER. }
  Zeros(2);              { 0Ch PREV, Zeiger auf das vorige Kettenglied. }
  Emit(0);               { 0Eh REMOVE=0, ueberlebt einen Warmstart. }
  Emit(0);               { 0Fh NONBANK=0, beide CP/M-3-Varianten. }
  for J:=1 to 8 do Emit(ord(RsxName[J]));  { 10h Name, genau acht Bytes. }
  Zeros(3);              { 18h LOADER-Kennung und zwei reservierte Bytes. }

  P:=First;
  while P<>Limit do begin
    { Nur die beiden reservierten Bytefolgen sind Metadaten.
      Andere ED-Befehle, z.B. ED B0 = LDIR, werden original uebernommen. }
    if (mem[P]=tag) and (mem[P+1] in [mark,ref]) then begin
      if mem[P+1]=mark then Define(mem[P+2]) else WordRef(mem[P+2]);
      P:=P+3
    end else begin Emit(mem[P]); P:=P+1 end
  end;

  Define(mDmaPB); Emit($3C); Zeros(3);   { SCB-Adresse der aktuellen DMA. }
  Define(mWidthPB); Emit($1A); Zeros(3); { SCB-Breite und Spalte lesen. }
  Define(mOldSP); Zeros(2);       { Aufruferstack waehrend des Hooks. }
  Define(mCountPtr); Zeros(2);    { Zeiger auf Ergebnislaenge. }
  Define(mTextPtr); Zeros(2);     { Zeiger auf Eingabetext im Aufrufer. }
  Define(mMaxLen); Emit(0);       { Vom Aufrufer zugelassene Zeichen. }
  Define(mWidth); Emit(0);        { Sichtbare Stellen hinter dem Prompt. }
  Define(mScreenPos); Emit(0);    { Physischer Cursor, relativ zum Prompt. }
  Define(mScreenLen); Emit(0);    { Zahl der momentan sichtbaren Zeichen. }
  Define(mAge); Emit(0);          { Null = Entwurf, eins = juengster Befehl. }
  Define(mHead); Emit(0);         { Index des naechsten Schreibplatzes. }
  Define(mUsed); Emit(0);         { Gueltige Ringplaetze, maximal acht. }
  Define(mDraftPos); Emit(0);     { Entwurfscursor vor dem ersten Hoch. }
  Define(mDraft); Zeros(256);     { Laenge + bis zu 255 Entwurfszeichen. }
  Define(mRing); Zeros(8*81);     { Achtmal Laenge + 80 ASCII-Zeichen. }
  Zeros(64); Define(mStackEnd)    { Eigener abwaerts wachsender Z80-Stack. }
end;

procedure PutByte(V: byte);
begin
  { CP/M schreibt binaer in Records zu 128 Bytes. }
  Buf[Fill]:=V;
  Fill:=Fill+1;
  if Fill=128 then begin BlockWrite(OutFile,Buf,1); Fill:=0 end
end;

begin
  Locate;
  { Der native Compiler erzeugt fuer den langen GOTO ein JP nn.
    Ziel und Grenzen pruefen, bevor die Vorlage dekodiert wird. }
  if mem[First]<>$C3 then Fail;
  if (mem[First+1]<>lo(Limit)) or (mem[First+2]<>hi(Limit)) then Fail;
  First:=First+3;
  for I:=1 to LastMark do Address[I]:=-1;
  FillChar(Bits,SizeOf(Bits),0);
  for Pass:=1 to 2 do Bild;
  Assign(OutFile,'DOSKEY.RSX'); Rewrite(OutFile);
  Fill:=0;
  PutByte(0); PutByte(lo(Pc)); PutByte(hi(Pc));
  for I:=3 to 255 do PutByte(0);         { Rest des PRL-Kopfs. }
  for I:=0 to Pc-1 do PutByte(Data[I]);  { Prefix, Editor, Daten, Stack. }
  for I:=0 to (Pc+7) div 8-1 do PutByte(Bits[I]);
  while Fill<>0 do PutByte(0);           { Nur den letzten Record auffuellen. }
  Close(OutFile);
  writeln('DOSKEY.RSX: ',Pc,' Bytes resident, ',(Pc+255) div 256,' Seiten.')
end.
