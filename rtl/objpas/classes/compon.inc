{
    This file is part of the Free Component Library (FCL)
    Copyright (c) 1999-2000 by the Free Pascal development team

    See the file COPYING.FPC, included in this distribution,
    for details about the copyright.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.

 **********************************************************************}

{****************************************************************************}
{*                             TComponent                                   *}
{****************************************************************************}

Function  TComponent.GetComponent(AIndex: Integer): TComponent;

begin
  If assigned(FComponents) then
    Result:=TComponent(FComponents.Items[Aindex])
  else
    Result:=Nil;
end;

function TComponent.IsImplementorOf (const Intf:IInterface):boolean;
var ref : IInterfaceComponentReference;
begin
 result:=assigned(intf) and supports(intf,IInterfaceComponentReference,ref) and (ref.getcomponent=self);
end;

procedure TComponent.ReferenceInterface(const intf:IInterface;op:TOperation);
var ref : IInterfaceComponentReference;
begin
 if assigned(intf) and supports(intf,IInterfaceComponentReference,ref) then
    ref.getcomponent.notification(self,op);
end;

Function  TComponent.GetComponentCount: Integer;

begin
  If assigned(FComponents) then
    Result:=FComponents.Count
  else
    result:=0
end;


Function  TComponent.GetComponentIndex: Integer;

begin
  If Assigned(FOwner) and Assigned(FOwner.FComponents) then
    Result:=FOwner.FComponents.IndexOf(Self)
  else
    Result:=-1;
end;


Procedure TComponent.Insert(AComponent: TComponent);

begin
  If not assigned(FComponents) then
    FComponents:=TList.Create;
  FComponents.Add(AComponent);
  AComponent.FOwner:=Self;
end;


Procedure TComponent.ReadLeft(Reader: TReader);

begin
  LongRec(FDesignInfo).Lo:=Reader.ReadInteger;
end;


Procedure TComponent.ReadTop(Reader: TReader);

begin
  LongRec(FDesignInfo).Hi:=Reader.ReadInteger;
end;


Procedure TComponent.Remove(AComponent: TComponent);

begin
  AComponent.FOwner:=Nil;
  If assigned(FComponents) then
    begin
    FComponents.Remove(AComponent);
    IF FComponents.Count=0 then
      begin
      FComponents.Free;
      FComponents:=Nil;
      end;
    end;
end;


Procedure TComponent.RemoveNotification(AComponent: TComponent);

begin
  if FFreeNotifies<>nil then
    begin
    FFreeNotifies.Remove(AComponent);
    if FFreeNotifies.Count=0 then
      begin
      FFreeNotifies.Free;
      FFreeNotifies:=nil;
      FComponentState := FComponentState - [csFreeNotification];
      end;
    end;
end;


Procedure TComponent.SetComponentIndex(Value: Integer);

Var Temp,Count : longint;

begin
  If Not assigned(FOwner) then exit;
  Temp:=getcomponentindex;
  If temp<0 then exit;
  If value<0 then value:=0;
  Count:=Fowner.FComponents.Count;
  If Value>=Count then value:=count-1;
  If Value<>Temp then
    begin
    FOWner.FComponents.Delete(Temp);
    FOwner.FComponents.Insert(Value,Self);
    end;
end;


Procedure TComponent.SetReference(Enable: Boolean);

var
  Field: ^TComponent;
begin
  if Assigned(Owner) then
  begin
    Field := Owner.FieldAddress(Name);
    if Assigned(Field) then
      if Enable then
        Field^ := Self
      else
        Field^ := nil;
  end;
end;


Procedure TComponent.WriteLeft(Writer: TWriter);

begin
  Writer.WriteInteger(LongRec(FDesignInfo).Lo);
end;


Procedure TComponent.WriteTop(Writer: TWriter);

begin
  Writer.WriteInteger(LongRec(FDesignInfo).Hi);
end;


Procedure TComponent.ChangeName(const NewName: TComponentName);

begin
  FName:=NewName;
end;


Procedure TComponent.DefineProperties(Filer: TFiler);

Var Ancestor : TComponent;
    Temp : longint;

begin
  Temp:=0;
  Ancestor:=TComponent(Filer.Ancestor);
  If Assigned(Ancestor) then Temp:=Ancestor.FDesignInfo;
  Filer.Defineproperty('left',@readleft,@writeleft,
                       (longrec(FDesignInfo).Lo<>Longrec(temp).Lo));
  Filer.Defineproperty('top',@readtop,@writetop,
                       (longrec(FDesignInfo).Hi<>Longrec(temp).Hi));
end;


Procedure TComponent.GetChildren(Proc: TGetChildProc; Root: TComponent);

begin
  // Does nothing.
end;


Function  TComponent.GetChildOwner: TComponent;

begin
 Result:=Nil;
end;


Function  TComponent.GetChildParent: TComponent;

begin
  Result:=Self;
end;


Function  TComponent.GetNamePath: string;

begin
  Result:=FName;
end;


Function  TComponent.GetOwner: TPersistent;

begin
  Result:=FOwner;
end;


Procedure TComponent.Loaded;

begin
  FComponentState := FComponentState - [csLoading];
end;

Procedure TComponent.Loading;

begin
  FComponentState := FComponentState + [csLoading];
end;


Procedure TComponent.Notification(AComponent: TComponent;
  Operation: TOperation);

//Var Runner : Longint;

begin
  If (Operation=opRemove) and Assigned(FFreeNotifies) then
    begin
    FFreeNotifies.Remove(AComponent);
    If FFreeNotifies.Count=0 then
      begin
      FFreeNotifies.Free;
      FFreenotifies:=Nil;
      FComponentState := FComponentState - [csFreeNotification];
      end;
    end;
{ disabled this code: do we really have to notify the child components also? Each observer
  should register itself to it's subject! }
  {If assigned(FComponents) then
    For Runner:=0 To FComponents.Count-1 do
      TComponent(FComponents.Items[Runner]).Notification(AComponent,Operation);}
end;


procedure TComponent.PaletteCreated;
  begin
  end;


Procedure TComponent.ReadState(Reader: TReader);

begin
  Reader.ReadData(Self);
end;


Procedure TComponent.SetAncestor(Value: Boolean);

Var Runner : Longint;

begin
  If Value then
    FComponentState := FComponentState + [csAncestor]
  else
    FComponentState := FComponentState - [csAncestor];
  if Assigned(FComponents) then
    For Runner:=0 To FComponents.Count-1 do
      TComponent(FComponents.Items[Runner]).SetAncestor(Value);
end;


Procedure TComponent.SetDesigning(Value: Boolean; SetChildren : Boolean = True);

Var Runner : Longint;

begin
  If Value then
    FComponentState := FComponentState + [csDesigning]
  else
    FComponentState := FComponentState - [csDesigning];
  if Assigned(FComponents) and SetChildren then
    For Runner:=0 To FComponents.Count - 1 do
      TComponent(FComponents.items[Runner]).SetDesigning(Value);
end;

Procedure TComponent.SetDesignInstance(Value: Boolean);

begin
  If Value then
    FComponentState := FComponentState + [csDesignInstance]
  else
    FComponentState := FComponentState - [csDesignInstance];
end;

Procedure TComponent.SetInline(Value: Boolean);

begin
  If Value then
    FComponentState := FComponentState + [csInline]
  else
    FComponentState := FComponentState - [csInline];
end;


Procedure TComponent.SetName(const NewName: TComponentName);

begin
  If FName=NewName then exit;
  If (NewName<>'') and not IsValidIdent(NewName) then
    Raise EComponentError.CreateFmt(SInvalidName,[NewName]);
  If Assigned(FOwner) Then
    FOwner.ValidateRename(Self,FName,NewName)
  else
    ValidateRename(Nil,FName,NewName);
  SetReference(False);
  ChangeName(NewName);
  Setreference(True);
end;


Procedure TComponent.SetChildOrder(Child: TComponent; Order: Integer);

begin
  // does nothing
end;


Procedure TComponent.SetParentComponent(Value: TComponent);

begin
  // Does nothing
end;


Procedure TComponent.Updating;

begin
  FComponentState := FComponentState + [csUpdating];
end;


Procedure TComponent.Updated;

begin
  FComponentState := FComponentState - [csUpdating];
end;


class Procedure TComponent.UpdateRegistry(Register: Boolean; const ClassID, ProgID: string);

begin
  // For compatibility only.
end;


Procedure TComponent.ValidateRename(AComponent: TComponent;
  const CurName, NewName: string);

begin
//!! This contradicts the Delphi manual.
  If (AComponent<>Nil) and (CompareText(CurName,NewName)<>0) and (AComponent.Owner = Self) and
     (FindComponent(NewName)<>Nil) then
      raise EComponentError.Createfmt(SDuplicateName,[newname]);
  If (csDesigning in FComponentState) and (FOwner<>Nil) then
    FOwner.ValidateRename(AComponent,Curname,Newname);
end;


Procedure TComponent.ValidateContainer(AComponent: TComponent);

begin
  AComponent.ValidateInsert(Self);
end;


Procedure TComponent.ValidateInsert(AComponent: TComponent);

begin
  // Does nothing.
end;


Procedure TComponent.WriteState(Writer: TWriter);

begin
  Writer.WriteComponentData(Self);
end;


Constructor TComponent.Create(AOwner: TComponent);

begin
  FComponentStyle:=[csInheritable];
  If Assigned(AOwner) then AOwner.InsertComponent(Self);
end;


Destructor TComponent.Destroy;

Var
  I : Integer;
  L : TList;
begin
  Destroying;
  If Assigned(FFreeNotifies) then
    begin
    L := FFreeNotifies;
    FFreenotifies:=Nil;
    for I := L.Count - 1 downto 0 do
      TComponent(L.Items[I]).Notification(self,opRemove);
    L.Free;
    end;
  DestroyComponents;
  If FOwner<>Nil Then FOwner.RemoveComponent(Self);
  inherited destroy;
end;


Procedure TComponent.DestroyComponents;

Var acomponent: TComponent;

begin
  While assigned(FComponents) do
    begin
    aComponent:=TComponent(FComponents.Last);
    Remove(aComponent);
    Acomponent.Destroy;
    end;
end;


Procedure TComponent.Destroying;

Var Runner : longint;

begin
  If csDestroying in FComponentstate Then Exit;
  FComponentState := FComponentState + [csDestroying];
  If Assigned(FComponents) then
    for Runner:=0 to FComponents.Count-1 do
      TComponent(FComponents.Items[Runner]).Destroying;
end;


function TComponent.ExecuteAction(Action: TBasicAction): Boolean;
begin
  if Action.HandlesTarget(Self) then
   begin
     Action.ExecuteTarget(Self);
     Result := True;
   end
  else
   Result := False;
end;


Function  TComponent.FindComponent(const AName: string): TComponent;

Var I : longint;

begin
  Result:=Nil;
  If (AName='') or Not assigned(FComponents) then exit;
  For i:=0 to FComponents.Count-1 do
    if (CompareText(TComponent(FComponents[I]).Name,AName)=0) then
      begin
      Result:=TComponent(FComponents.Items[I]);
      exit;
      end;
end;


Procedure TComponent.FreeNotification(AComponent: TComponent);

begin
  If (Owner<>Nil) and (AComponent=Owner) then exit;
  if csDestroying in ComponentState then
    AComponent.Notification(Self,opRemove)
  else
    begin
    If not Assigned(FFreeNotifies) then
      begin
        FFreeNotifies:=TList.Create;
        FComponentState := FComponentState + [csFreeNotification];
      end;
    If FFreeNotifies.IndexOf(AComponent)=-1 then
      begin
      FFreeNotifies.Add(AComponent);
      AComponent.FreeNotification (self);
      end;
    end;
end;


procedure TComponent.RemoveFreeNotification(AComponent: TComponent);
begin
  RemoveNotification(AComponent);
  AComponent.RemoveNotification (self);
end;


Procedure TComponent.FreeOnRelease;

begin
  // Delphi compatibility only at the moment.
end;


Function  TComponent.GetParentComponent: TComponent;

begin
  Result:=Nil;
end;


Function  TComponent.HasParent: Boolean;

begin
  Result:=False;
end;


Procedure TComponent.InsertComponent(AComponent: TComponent);

begin
  AComponent.ValidateContainer(Self);
  ValidateRename(AComponent,'',AComponent.FName);
  Insert(AComponent);
  AComponent.SetReference(True);
  If csDesigning in FComponentState then
    AComponent.SetDesigning(true);
  Notification(AComponent,opInsert);
end;


Procedure TComponent.RemoveComponent(AComponent: TComponent);

begin
  Notification(AComponent,opRemove);
  AComponent.SetReference(False);
  Remove(AComponent);
  Acomponent.Setdesigning(False);
  ValidateRename(AComponent,AComponent.FName,'');
end;


Function  TComponent.SafeCallException(ExceptObject: TObject;
  ExceptAddr: Pointer): Integer;

begin
  SafeCallException:=0;
end;

procedure TComponent.SetSubComponent(ASubComponent: Boolean);
begin
  if ASubComponent then
    FComponentStyle := FComponentStyle + [csSubComponent]
  else
    FComponentStyle := FComponentStyle - [csSubComponent];
end;


function TComponent.UpdateAction(Action: TBasicAction): Boolean;
begin
  if Action.HandlesTarget(Self) then
    begin
      Action.UpdateTarget(Self);
      Result := True;
    end
  else
    Result := False;
end;

function TComponent.QueryInterface(const IID: TGUID; out Obj): HResult;stdcall;
begin
  if GetInterface(IID, Obj) then
    result:=S_OK
  else
    result:=E_NOINTERFACE;
end;

function TComponent._AddRef: Integer;stdcall;
begin
  result:=-1;
end;

function TComponent._Release: Integer;stdcall;
begin
  result:=-1;
end;

function TComponent.iicrGetComponent: TComponent;

begin
  result:=self;
end;
