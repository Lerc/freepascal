{
   This file is part of the Free Pascal run time library.
   (c) 2000-2003 by Marco van de Voort
   member of the Free Pascal development team.

   See the file COPYING.FPC, included in this distribution,
   for details about the copyright.

   Signalhandler for FreeBSD/i386

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY;without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
}

CONST FPU_ALL=$7F;

function getfpustate(const Sigcontext:sigcontextRec):longint; {inline;}
begin
  getfpustate:=0;
end;

procedure SignalToRunerror(signo: cint); public name '_FPC_DEFAULTSIGHANDLER'; cdecl;
var
  res : word;
begin
    res:=0;
    if signo = SIGFPE then
     begin
        res := 200;
     end
    else
    if (signo = SIGILL) or (signo = SIGBUS) or (signo = SIGSEGV) then
      begin
        res := 216;
      end
    else if (signo = SIGINT) then
      begin
        res:=217;
      end
    else if (signo = SIGKILL) then
      begin
        res:=233
      end;
  { give runtime error at the position where the signal was raised }
  if res<>0 then
   begin
     HandleError(res);
   end;
end;



