{
    Copyright (c) 2000-2002 by Florian Klaempfl

    This unit contains basic functions for unicode support in the
    compiler, this unit is mainly necessary to bootstrap widestring
    support ...

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program; if not, write to the Free Software
    Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.

 ****************************************************************************
}
unit widestr;

{$i fpcdefs.inc}

  interface

    uses
       {$ifdef VER2_2}ccharset{$else VER2_2}charset{$endif VER2_2},globtype;


    type
       tcompilerwidechar = word;
       tcompilerwidecharptr = ^tcompilerwidechar;
       pcompilerwidechar = ^tcompilerwidechar;

       pcompilerwidestring = ^_tcompilerwidestring;
       _tcompilerwidestring = record
          data : pcompilerwidechar;
          maxlen,len : SizeInt;
       end;

    procedure initwidestring(out r : pcompilerwidestring);
    procedure donewidestring(var r : pcompilerwidestring);
    procedure setlengthwidestring(r : pcompilerwidestring;l : SizeInt);
    function getlengthwidestring(r : pcompilerwidestring) : SizeInt;
    procedure concatwidestringchar(r : pcompilerwidestring;c : tcompilerwidechar);
    procedure concatwidestrings(s1,s2 : pcompilerwidestring);
    function comparewidestrings(s1,s2 : pcompilerwidestring) : SizeInt;
    procedure copywidestring(s,d : pcompilerwidestring);
    function asciichar2unicode(c : char) : tcompilerwidechar;
    function unicode2asciichar(c : tcompilerwidechar) : char;
    procedure ascii2unicode(p : pchar;l : SizeInt;r : pcompilerwidestring);
    procedure unicode2ascii(r : pcompilerwidestring;p : pchar);
    function hasnonasciichars(const p: pcompilerwidestring): boolean;
    function getcharwidestring(r : pcompilerwidestring;l : SizeInt) : tcompilerwidechar;
    function cpavailable(const s : string) : boolean;

  implementation

    uses
      cp8859_1,cp850,cp437,
      { cyrillic code pages }
      cp1251,cp866,cp8859_5,
      globals,cutils;


    procedure initwidestring(out r : pcompilerwidestring);

      begin
         new(r);
         r^.data:=nil;
         r^.len:=0;
         r^.maxlen:=0;
      end;

    procedure donewidestring(var r : pcompilerwidestring);

      begin
         if assigned(r^.data) then
           freemem(r^.data);
         dispose(r);
         r:=nil;
      end;

    function getcharwidestring(r : pcompilerwidestring;l : SizeInt) : tcompilerwidechar;

      begin
         getcharwidestring:=r^.data[l];
      end;

    function getlengthwidestring(r : pcompilerwidestring) : SizeInt;

      begin
         getlengthwidestring:=r^.len;
      end;

    procedure setlengthwidestring(r : pcompilerwidestring;l : SizeInt);

      begin
         if r^.maxlen>=l then
           exit;
         if assigned(r^.data) then
           reallocmem(r^.data,sizeof(tcompilerwidechar)*l)
         else
           getmem(r^.data,sizeof(tcompilerwidechar)*l);
         r^.maxlen:=l;
      end;

    procedure concatwidestringchar(r : pcompilerwidestring;c : tcompilerwidechar);

      begin
         if r^.len>=r^.maxlen then
           setlengthwidestring(r,r^.len+16);
         r^.data[r^.len]:=c;
         inc(r^.len);
      end;

    procedure concatwidestrings(s1,s2 : pcompilerwidestring);
      begin
         setlengthwidestring(s1,s1^.len+s2^.len);
         move(s2^.data^,s1^.data[s1^.len],s2^.len*sizeof(tcompilerwidechar));
         inc(s1^.len,s2^.len);
      end;

    procedure copywidestring(s,d : pcompilerwidestring);

      begin
         setlengthwidestring(d,s^.len);
         d^.len:=s^.len;
         move(s^.data^,d^.data^,s^.len*sizeof(tcompilerwidechar));
      end;

    function comparewidestrings(s1,s2 : pcompilerwidestring) : SizeInt;
      var
         maxi,temp : SizeInt;
      begin
         if pointer(s1)=pointer(s2) then
           begin
              comparewidestrings:=0;
              exit;
           end;
         maxi:=s1^.len;
         temp:=s2^.len;
         if maxi>temp then
           maxi:=Temp;
         temp:=compareword(s1^.data^,s2^.data^,maxi);
         if temp=0 then
           temp:=s1^.len-s2^.len;
         comparewidestrings:=temp;
      end;

    function asciichar2unicode(c : char) : tcompilerwidechar;
      var
         m : punicodemap;
      begin
         if (current_settings.sourcecodepage <> 'utf8') then
           begin
             m:=getmap(current_settings.sourcecodepage);
             asciichar2unicode:=getunicode(c,m);
           end
         else
           result:=tcompilerwidechar(c);
      end;

    function unicode2asciichar(c : tcompilerwidechar) : char;
      begin
        if word(c)<128 then
          unicode2asciichar:=char(word(c))
         else
          unicode2asciichar:='?';
      end;

    procedure ascii2unicode(p : pchar;l : SizeInt;r : pcompilerwidestring);
      var
         source : pchar;
         dest   : tcompilerwidecharptr;
         i      : SizeInt;
         m      : punicodemap;
      begin
         m:=getmap(current_settings.sourcecodepage);
         setlengthwidestring(r,l);
         source:=p;
         r^.len:=l;
         dest:=tcompilerwidecharptr(r^.data);
         if (current_settings.sourcecodepage <> 'utf8') then
           begin
             for i:=1 to l do
                begin
                  dest^:=getunicode(source^,m);
                  inc(dest);
                  inc(source);
                end;
           end
         else
           begin
             for i:=1 to l do
                begin
                  dest^:=tcompilerwidechar(source^);
                  inc(dest);
                  inc(source);
                end;
           end;
      end;

    procedure unicode2ascii(r : pcompilerwidestring;p:pchar);
(*
      var
         m : punicodemap;
         i : longint;

      begin
         m:=getmap(current_settings.sourcecodepage);
         { should be a very good estimation :) }
         setlengthwidestring(r,length(s));
         // !!!! MBCS
         for i:=1 to length(s) do
           begin
           end;
      end;
*)
      var
        source : tcompilerwidecharptr;
        dest   : pchar;
        i      : longint;
      begin
        { This routine must work the same as the
          the routine in the RTL to have the same compile time (for constant strings)
          and runtime conversion (for variables) }
        source:=tcompilerwidecharptr(r^.data);
        dest:=p;
        for i:=1 to r^.len do
         begin
           if word(source^)<128 then
            dest^:=char(word(source^))
           else
            dest^:='?';
           inc(dest);
           inc(source);
         end;
      end;


    function hasnonasciichars(const p: pcompilerwidestring): boolean;
      var
        source : tcompilerwidecharptr;
        i      : longint;
      begin
        source:=tcompilerwidecharptr(p^.data);
        result:=true;
        for i:=1 to p^.len do
          begin
            if word(source^)>=128 then
              exit;
            inc(source);
          end;
        result:=false;
      end;


    function cpavailable(const s : string) : boolean;
      begin
          cpavailable:=mappingavailable(lower(s));
      end;

end.
