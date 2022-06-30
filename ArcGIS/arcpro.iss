; Script generated by the Inno Setup Script Wizard.
; SEE THE DOCUMENTATION FOR DETAILS ON CREATING INNO SETUP SCRIPT FILES!

#define MyAppName "KEA for ArcPro"
#define MyAppVersion "1.4.15"
#define MyAppPublisher "Landcare Research NZ"
#define MyAppURL "http://kealib.org/"
#define MyOutputFilename "setup_kea_arcpro_" + GetDateTimeString('yyyymmdd', '', '')

[Setup]
; NOTE: The value of AppId uniquely identifies this application.
; Do not use the same AppId value in installers for other applications.
; (To generate a new GUID, click Tools | Generate GUID inside the IDE.)
AppId={{B3B6896E-9EA0-4F8A-BC5F-EFBAF4EDBFB6}}
AppName={#MyAppName}
AppVersion={#MyAppVersion}
;AppVerName={#MyAppName} {#MyAppVersion}
AppPublisher={#MyAppPublisher}
AppPublisherURL={#MyAppURL}
AppSupportURL={#MyAppURL}
AppUpdatesURL={#MyAppURL}
LicenseFile=C:\dev\kealib\LICENSE.txt
OutputDir=C:\dev\keainstall
OutputBaseFilename={#MyOutputFilename}
Compression=lzma
SolidCompression=yes
DefaultDirName={code:GetArcProDir}
UsePreviousAppDir=no
AppendDefaultDirName=no
DirExistsWarning=no

[Languages]
Name: "english"; MessagesFile: "compiler:Default.isl"

[Files]
; The support libs (hdf5 etc) are duplicated between releases as some will be done with the same compiler
; but I'm hopeful that the compression will work this out.
;Source: "C:\dev\arckea\dist\arcpro14\x64\lib\*.dll"; DestDir: "{app}\bin"; Check: ArcVersion('1.4'); Flags: ignoreversion
;Source: "C:\dev\arckea\dist\arcpro14\x64\lib\gdalplugins\gdal_KEA.dll"; DestDir: "{app}\bin\gdalplugins"; Check: ArcVersion('1.4'); Flags: ignoreversion

;Source: "C:\dev\arckea\dist\arcpro20\x64\lib\*.dll"; DestDir: "{app}\bin"; Check: ArcVersion('2.0'); Flags: ignoreversion
;Source: "C:\dev\arckea\dist\arcpro20\x64\lib\gdalplugins\gdal_KEA.dll"; DestDir: "{app}\bin\gdalplugins"; Check: ArcVersion('2.0'); Flags: ignoreversion

;Source: "C:\dev\arckea\dist\arc106_arcpro21\x64\lib\*.dll"; DestDir: "{app}\bin"; Check: ArcVersion('2.1'); Flags: ignoreversion
;Source: "C:\dev\arckea\dist\arc106_arcpro21\x64\lib\gdalplugins\gdal_KEA.dll"; DestDir: "{app}\bin\gdalplugins"; Check: ArcVersion('2.1'); Flags: ignoreversion

Source: "C:\dev\arckea\dist\arc1061_arcpro22\x64\lib\*.dll"; DestDir: "{app}\bin"; Check: ArcVersion('2.2'); Flags: ignoreversion
Source: "C:\dev\arckea\dist\arc1061_arcpro22\x64\lib\gdalplugins\gdal_KEA.dll"; DestDir: "{app}\bin\gdalplugins"; Check: ArcVersion('2.2'); Flags: ignoreversion

Source: "C:\dev\arckea\dist\arc1071_arcpro24\x64\lib\*.dll"; DestDir: "{app}\bin"; Check: ArcVersion('2.4'); Flags: ignoreversion
Source: "C:\dev\arckea\dist\arc1071_arcpro24\x64\lib\gdalplugins\gdal_KEA.dll"; DestDir: "{app}\bin\gdalplugins"; Check: ArcVersion('2.4'); Flags: ignoreversion

Source: "C:\dev\arckea\dist\arc108_arcpro25\x64\lib\*.dll"; DestDir: "{app}\bin"; Check: ArcVersion('2.5'); Flags: ignoreversion
Source: "C:\dev\arckea\dist\arc108_arcpro25\x64\lib\gdalplugins\gdal_KEA.dll"; DestDir: "{app}\bin\gdalplugins"; Check: ArcVersion('2.5'); Flags: ignoreversion

Source: "C:\dev\arckea\dist\arc1081_arcpro26\x64\lib\*.dll"; DestDir: "{app}\bin"; Check: ArcVersion('2.6'); Flags: ignoreversion
Source: "C:\dev\arckea\dist\arc1081_arcpro26\x64\lib\gdalplugins\gdal_KEA.dll"; DestDir: "{app}\bin\gdalplugins"; Check: ArcVersion('2.6'); Flags: ignoreversion

Source: "C:\dev\arckea\dist\arcpro27\x64\lib\*.dll"; DestDir: "{app}\bin"; Check: ArcVersion('2.7'); Flags: ignoreversion
Source: "C:\dev\arckea\dist\arcpro27\x64\lib\gdalplugins\gdal_KEA.dll"; DestDir: "{app}\bin\gdalplugins"; Check: ArcVersion('2.7'); Flags: ignoreversion

Source: "C:\dev\arckea\dist\arcpro28\x64\lib\*.dll"; DestDir: "{app}\bin"; Check: ArcVersion('2.8'); Flags: ignoreversion
Source: "C:\dev\arckea\dist\arcpro28\x64\lib\gdalplugins\gdal_KEA.dll"; DestDir: "{app}\bin\gdalplugins"; Check: ArcVersion('2.8'); Flags: ignoreversion

[code]
const
  // this is where ArcPro seems to put the install information
  ArcSubKey = 'SOFTWARE\ESRI\ArcGISPro';
var
  // these global vars are set by InitializeSetup() and checked by ArcVersion() and GetArcProDir()
  ArcVersionClass : string; // string from GetArcVersionClass()
  ArcRealVersion : string; // the contents of the "RealVersion" key
  ArcInstallDir : string; // the directory in which Arc is installed

// CompareVersion() and UpdateRasterFormats (for updating RasterFormats.dat) live here
#include "arccommon.isi"

// For calling from Check: above
function ArcVersion(versionClass : string): boolean;
begin
  Result := versionClass = ArcVersionClass;
end;

// forward declarations
function GetArcVersionClass(realVersion: string): string; forward;

// get the dir of ArcPro and fill in global variables
function InitializeSetup(): Boolean;
var
  ErrorDisplayed : Boolean;
begin
  ErrorDisplayed := False;
  Result := False;
  // init global vars
  ArcInstallDir := '';
  ArcRealVersion := '0.0';
  ArcVersionClass := '';

  // need to be 64 bit registry
  if RegQueryStringValue(HKLM64, ArcSubKey, 'InstallDir', ArcInstallDir) then
  begin
    if RegQueryStringValue(HKLM64, ArcSubKey, 'RealVersion', ArcRealVersion) then
    begin
       ArcVersionClass := GetArcVersionClass(ArcRealVersion);
       if ArcVersionClass = '' then
       begin
         MsgBox('Unsupported ArcPro Version ' + ArcRealVersion, mbCriticalError, MB_OK);
         ErrorDisplayed := True;
       end
       else
         Result := True
    end;
  end;

  if not Result and not ErrorDisplayed then
    MsgBox('Failed to read ArcPro Install info. ArcPro may not be installed on this PC', mbCriticalError, MB_OK)
end;

procedure InitializeWizard;
var
  InstallDirPage: TWizardPage;
begin
  // get the Dir page and customize the text
  InstallDirPage := PageFromID(wpSelectDir);

  InstallDirPage.Description := FmtMessage('ArcPro Version %1 has been found in the following location.'  + #13#10 + 'The support files for this version will be installed.', [ArcVersionClass]);
end; 

function GetArcProDir(Value: string): string;
begin
  // return global var set in InitializeSetup()
  Result := ArcInstallDir;
end;

// convert from the arc version string to one of the version classes we recognise
function GetArcVersionClass(realVersion: string): string;
begin
  //if (CompareVersion(realVersion, '1.4') <> -1) and (CompareVersion(realVersion, '2.0') = -1) then
  //  Result := '1.4'
  //else if (CompareVersion(realVersion, '2.0') <> -1) and (CompareVersion(realVersion, '2.1') = -1) then
  //  Result := '2.0'
  //else if (CompareVersion(realVersion, '2.1') <> -1) and (CompareVersion(realVersion, '2.2') = -1) then
  //  Result := '2.1'
  if (CompareVersion(realVersion, '2.2') <> -1) and (CompareVersion(realVersion, '2.3') = -1) then
    Result := '2.2'
  else if (CompareVersion(realVersion, '2.4') <> -1) and (CompareVersion(realVersion, '2.5') = -1) then
    Result := '2.4'
  else if (CompareVersion(realVersion, '2.5') <> -1) and (CompareVersion(realVersion, '2.6') = -1) then
    Result := '2.5'
  else if (CompareVersion(realVersion, '2.6') <> -1) and (CompareVersion(realVersion, '2.7') = -1) then
    Result := '2.6'
  else if (CompareVersion(realVersion, '2.7') <> -1) and (CompareVersion(realVersion, '2.8') = -1) then
    Result := '2.7'
  else if (CompareVersion(realVersion, '2.8') <> -1) and (CompareVersion(realVersion, '2.9') = -1) then
    Result := '2.8'
  else
    Result := ''
end;

// add the 'kea' extension to 'activationExtensions' and 'relevantExtensions'
// for Rasters in ObjectFactoryInfoCache.json
procedure UpdateInfoCache(cachePath: string);
var
   lines : TArrayOfString;
   I : Integer;
   J : Integer;
   bInObject : Boolean;
   bIsRaster : Boolean;
   bFileUpdated : Boolean;
   trimmedLine : string;
   currLine : string;
begin
  bFileUpdated := False

  // Read the whole thing in
  bInObject := False;
  bIsRaster := False;
  if LoadStringsFromFile(cachePath, lines) then
  begin
    for I := 0 to GetArrayLength(lines)-1 do
    begin
      trimmedLine := Trim(lines[I]);
      if trimmedLine = '{' then
      begin
        bInObject := True;
        bIsRaster := False;
      end
      else if trimmedLine = '},' then
      begin
        bInObject := False;
        bIsRaster := False;
      end;

      if (Pos('name', trimmedLine) <> 0) and (Pos('Raster', trimmedLine) <> 0) then
      begin
        bIsRaster := True
      end;

      if bInObject and bIsRaster and ((Pos('activationExtensions', trimmedLine) <> 0) or (Pos('relevantExtensions', trimmedLine) <> 0)) and (Pos('kea', trimmedLine) = 0) then
      begin
        // on the original line, go backwards until we get to the last extension
        currLine := lines[I];
        for J := Length(currLine) downto 0 do
        begin
          if (currLine[J] <> ' ') and (currLine[J] <> ',') and (currLine[J] <> '"') then
          begin
            break;
          end;
        end;

        // insert 'kea' after the last one
        Insert('|kea', currLine, J+1);
        //Log('Updated line is ' + currLine);
        lines[I] := currLine;
        bFileUpdated := True;
      end;
    end;
  end;

  if bFileUpdated then
  begin
    //Log('File updated');
    // first backup existing file
    FileCopy(cachePath, cachePath + '.bak', False);
    // save updated lines
    SaveStringsToFile(cachePath, lines, False);
  end;
end;

// run after the files have been copied. Updates the RasterFormats.dat
procedure CurStepChanged(CurStep: TSetupStep);
var
  rfpath : string;
  cachePath : string;
begin
  if CurStep = ssPostInstall then
  begin
    rfpath := ExpandConstant('{app}\bin\RasterFormats.dat');
    UpdateRasterFormats(rfpath);
    
    // ObjectFactoryInfoCache.json
    cachePath := ExpandConstant('{app}\Resources\SearchResources\Schema\ObjectFactoryInfoCache.json');
    if FileExists(cachePath) then
    begin
      UpdateInfoCache(cachePath);
    end;
  end;
end;

