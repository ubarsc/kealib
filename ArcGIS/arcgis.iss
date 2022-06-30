; Script generated by the Inno Setup Script Wizard.
; SEE THE DOCUMENTATION FOR DETAILS ON CREATING INNO SETUP SCRIPT FILES!

#define MyAppName "KEA for ArcGIS"
#define MyAppVersion "1.4.15"
#define MyAppPublisher "Landcare Research NZ"
#define MyAppURL "http://kealib.org/"
#define MyOutputFilename "setup_kea_arcgis_" + GetDateTimeString('yyyymmdd', '', '')

[Setup]
; NOTE: The value of AppId uniquely identifies this application.
; Do not use the same AppId value in installers for other applications.
; (To generate a new GUID, click Tools | Generate GUID inside the IDE.)
AppId={{F14E4CBA-4760-4F6C-AC19-9FBDF801A96A}
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
DefaultDirName={code:GetArcGISDir}
UsePreviousAppDir=no
AppendDefaultDirName=no
DirExistsWarning=no

[Languages]
Name: "english"; MessagesFile: "compiler:Default.isl"

[Files]
; The support libs (hdf5 etc) are duplicated between releases as some will be done with the same compiler
; but I'm hopeful that the compression will work this out.
;Source: "C:\dev\arckea\dist\arc93\lib\*.dll"; DestDir: "{app}\bin"; Check: ArcVersion('9.3', 0); Flags: ignoreversion
;Source: "C:\dev\arckea\dist\arc93\lib\gdalplugins\gdal_KEA.dll"; DestDir: "{app}\bin\gdalplugins"; Check: ArcVersion('9.3', 0); Flags: ignoreversion

;Source: "C:\dev\arckea\dist\arc100\lib\*.dll"; DestDir: "{app}\bin"; Check: ArcVersion('10.0', 0); Flags: ignoreversion
;Source: "C:\dev\arckea\dist\arc100\lib\gdalplugins\gdal_KEA.dll"; DestDir: "{app}\bin\gdalplugins"; Check: ArcVersion('10.0', 0); Flags: ignoreversion

;Source: "C:\dev\arckea\dist\arc101\lib\*.dll"; DestDir: "{app}\bin"; Check: ArcVersion('10.1', 0); Flags: ignoreversion
;Source: "C:\dev\arckea\dist\arc101\lib\gdalplugins\gdal_KEA.dll"; DestDir: "{app}\bin\gdalplugins"; Check: ArcVersion('10.1', 0); Flags: ignoreversion

Source: "C:\dev\arckea\dist\arc104\x86\lib\*.dll"; DestDir: "{app}\bin"; Check: ArcVersion('10.4', 32); Flags: ignoreversion
Source: "C:\dev\arckea\dist\arc104\x86\lib\gdalplugins\gdal_KEA.dll"; DestDir: "{app}\bin\gdalplugins"; Check: ArcVersion('10.4', 32); Flags: ignoreversion

Source: "C:\dev\arckea\dist\arc104\x64\lib\*.dll"; DestDir: "{app}\bin64"; Check: ArcVersion('10.4', 64); Flags: ignoreversion
Source: "C:\dev\arckea\dist\arc104\x64\lib\gdalplugins\gdal_KEA.dll"; DestDir: "{app}\bin64\gdalplugins"; Check: ArcVersion('10.4', 64); Flags: ignoreversion

Source: "C:\dev\arckea\dist\arc105\x86\lib\*.dll"; DestDir: "{app}\bin"; Check: ArcVersion('10.5', 32); Flags: ignoreversion
Source: "C:\dev\arckea\dist\arc105\x86\lib\gdalplugins\gdal_KEA.dll"; DestDir: "{app}\bin\gdalplugins"; Check: ArcVersion('10.5', 32); Flags: ignoreversion

Source: "C:\dev\arckea\dist\arc105\x64\lib\*.dll"; DestDir: "{app}\bin64"; Check: ArcVersion('10.5', 64); Flags: ignoreversion
Source: "C:\dev\arckea\dist\arc105\x64\lib\gdalplugins\gdal_KEA.dll"; DestDir: "{app}\bin64\gdalplugins"; Check: ArcVersion('10.5', 64); Flags: ignoreversion

Source: "C:\dev\arckea\dist\arc1051\x86\lib\*.dll"; DestDir: "{app}\bin"; Check: ArcVersion('10.5.1', 32); Flags: ignoreversion
Source: "C:\dev\arckea\dist\arc1051\x86\lib\gdalplugins\gdal_KEA.dll"; DestDir: "{app}\bin\gdalplugins"; Check: ArcVersion('10.5.1', 32); Flags: ignoreversion

Source: "C:\dev\arckea\dist\arc1051\x64\lib\*.dll"; DestDir: "{app}\bin64"; Check: ArcVersion('10.5.1', 64); Flags: ignoreversion
Source: "C:\dev\arckea\dist\arc1051\x64\lib\gdalplugins\gdal_KEA.dll"; DestDir: "{app}\bin64\gdalplugins"; Check: ArcVersion('10.5.1', 64); Flags: ignoreversion

Source: "C:\dev\arckea\dist\arc106_arcpro21\x86\lib\*.dll"; DestDir: "{app}\bin"; Check: ArcVersion('10.6', 32); Flags: ignoreversion
Source: "C:\dev\arckea\dist\arc106_arcpro21\x86\lib\gdalplugins\gdal_KEA.dll"; DestDir: "{app}\bin\gdalplugins"; Check: ArcVersion('10.6', 32); Flags: ignoreversion

Source: "C:\dev\arckea\dist\arc106_arcpro21\x64\lib\*.dll"; DestDir: "{app}\bin64"; Check: ArcVersion('10.6', 64); Flags: ignoreversion
Source: "C:\dev\arckea\dist\arc106_arcpro21\x64\lib\gdalplugins\gdal_KEA.dll"; DestDir: "{app}\bin64\gdalplugins"; Check: ArcVersion('10.6', 64); Flags: ignoreversion

Source: "C:\dev\arckea\dist\arc1061_arcpro22\x86\lib\*.dll"; DestDir: "{app}\bin"; Check: ArcVersion('10.6.1', 32); Flags: ignoreversion
Source: "C:\dev\arckea\dist\arc1061_arcpro22\x86\lib\gdalplugins\gdal_KEA.dll"; DestDir: "{app}\bin\gdalplugins"; Check: ArcVersion('10.6.1', 32); Flags: ignoreversion

Source: "C:\dev\arckea\dist\arc1061_arcpro22\x64\lib\*.dll"; DestDir: "{app}\bin64"; Check: ArcVersion('10.6.1', 64); Flags: ignoreversion
Source: "C:\dev\arckea\dist\arc1061_arcpro22\x64\lib\gdalplugins\gdal_KEA.dll"; DestDir: "{app}\bin64\gdalplugins"; Check: ArcVersion('10.6.1', 64); Flags: ignoreversion

Source: "C:\dev\arckea\dist\arc1071_arcpro24\x86\lib\*.dll"; DestDir: "{app}\bin"; Check: ArcVersion('10.7.1', 32); Flags: ignoreversion
Source: "C:\dev\arckea\dist\arc1071_arcpro24\x86\lib\gdalplugins\gdal_KEA.dll"; DestDir: "{app}\bin\gdalplugins"; Check: ArcVersion('10.7.1', 32); Flags: ignoreversion

Source: "C:\dev\arckea\dist\arc1071_arcpro24\x64\lib\*.dll"; DestDir: "{app}\bin64"; Check: ArcVersion('10.7.1', 64); Flags: ignoreversion
Source: "C:\dev\arckea\dist\arc1071_arcpro24\x64\lib\gdalplugins\gdal_KEA.dll"; DestDir: "{app}\bin64\gdalplugins"; Check: ArcVersion('10.7.1', 64); Flags: ignoreversion

Source: "C:\dev\arckea\dist\arc108_arcpro25\x86\lib\*.dll"; DestDir: "{app}\bin"; Check: ArcVersion('10.8', 32); Flags: ignoreversion
Source: "C:\dev\arckea\dist\arc108_arcpro25\x86\lib\gdalplugins\gdal_KEA.dll"; DestDir: "{app}\bin\gdalplugins"; Check: ArcVersion('10.8', 32); Flags: ignoreversion

Source: "C:\dev\arckea\dist\arc108_arcpro25\x64\lib\*.dll"; DestDir: "{app}\bin64"; Check: ArcVersion('10.8', 64); Flags: ignoreversion
Source: "C:\dev\arckea\dist\arc108_arcpro25\x64\lib\gdalplugins\gdal_KEA.dll"; DestDir: "{app}\bin64\gdalplugins"; Check: ArcVersion('10.8', 64); Flags: ignoreversion

Source: "C:\dev\arckea\dist\arc1081_arcpro26\x86\lib\*.dll"; DestDir: "{app}\bin"; Check: ArcVersion('10.8.1', 32); Flags: ignoreversion
Source: "C:\dev\arckea\dist\arc1081_arcpro26\x86\lib\gdalplugins\gdal_KEA.dll"; DestDir: "{app}\bin\gdalplugins"; Check: ArcVersion('10.8.1', 32); Flags: ignoreversion

Source: "C:\dev\arckea\dist\arc1081_arcpro26\x64\lib\*.dll"; DestDir: "{app}\bin64"; Check: ArcVersion('10.8.1', 64); Flags: ignoreversion
Source: "C:\dev\arckea\dist\arc1081_arcpro26\x64\lib\gdalplugins\gdal_KEA.dll"; DestDir: "{app}\bin64\gdalplugins"; Check: ArcVersion('10.8.1', 64); Flags: ignoreversion

Source: "C:\dev\arckea\dist\arc109\x86\lib\*.dll"; DestDir: "{app}\bin"; Check: ArcVersion('10.9', 32); Flags: ignoreversion
Source: "C:\dev\arckea\dist\arc109\x86\lib\gdalplugins\gdal_KEA.dll"; DestDir: "{app}\bin\gdalplugins"; Check: ArcVersion('10.9', 32); Flags: ignoreversion

Source: "C:\dev\arckea\dist\arc109\x64\lib\*.dll"; DestDir: "{app}\bin64"; Check: ArcVersion('10.9', 64); Flags: ignoreversion
Source: "C:\dev\arckea\dist\arc109\x64\lib\gdalplugins\gdal_KEA.dll"; DestDir: "{app}\bin64\gdalplugins"; Check: ArcVersion('10.9', 64); Flags: ignoreversion

[code]
const
  // this is where ArcGIS seems to put the install information
  ArcSubKey = 'SOFTWARE\Wow6432Node\ESRI';
var
  // these global vars are set by InitializeSetup() and checked by ArcVersion() and GetArcGISDir()
  ArcVersionClass : string; // string with version - see GetArcVersionClass()
  ArcRealVersion : string; // the contents of the "RealVersion" key
  HasBin64 : boolean;  // Has a "bin64" subdir (BGProcessing) so 64 bit DLL also needs to be installed
  ArcInstallDir : string; // the directory in which Arc is installed

// CompareVersion() and UpdateRasterFormats (for updating RasterFormats.dat) live here
#include "arccommon.isi"

// For calling from Check: above
// bits parameter is 0 for don't care or 32 or 64
// ideally would have used enums but that doesn't seem to work when calling from Check:
function ArcVersion(versionClass : string; bits : Integer): boolean;
begin
  Result := False;
  if versionClass = ArcVersionClass then
    // matches the version class
    if bits = 0 then
      Result := True // always match
    else if bits = 32 then
      Result := True  // always match as Arc is 32 bit application
    else if bits = 64 then
      Result := HasBin64 // install 64 bit as 'bin64' dir is present
    else
      MsgBox('Unknown number of bits ' + IntToStr(bits), mbInformation, MB_OK)
end;

// forward declarations
function GetArcVersionClass(realVersion: string): string; forward;

// get the dir of ArcGIS and fill in global variables
function InitializeSetup(): Boolean;
var
  ArcNames : TArrayOfString;
  I : Integer;
  S : String;
  bin64dir : String;
  currSubKey : String;
  ErrorDisplayed : Boolean;
begin
  ErrorDisplayed := False;
  Result := False;
  // init global vars
  ArcInstallDir := '';
  ArcRealVersion := '0.0';
  ArcVersionClass := '';
  HasBin64 := False;
  // look at all the subkeys of ArcSubKey and choose the best one
  if RegGetSubkeyNames(HKEY_LOCAL_MACHINE, ArcSubKey, ArcNames) then
  begin
    for I := 0 to GetArrayLength(ArcNames)-1 do
    begin
       // only process keys that start with 'Desktop'. TODO: correct?
      if Pos('Desktop', ArcNames[I]) = 1 then
      begin
        currSubKey := ArcSubKey + '\' + ArcNames[I];
        if RegQueryStringValue(HKEY_LOCAL_MACHINE, currSubKey, 'RealVersion', S) then
        begin
          if CompareVersion(ArcRealVersion, S) = -1 then
          begin
            // it's the most recent version
            // get the install dir
            if RegQueryStringValue(HKEY_LOCAL_MACHINE, currSubKey, 'InstallDir', ArcInstallDir) then
            begin
              ArcRealVersion := S;
              ArcVersionClass := GetArcVersionClass(ArcRealVersion);
              if ArcVersionClass = '' then
              begin
                MsgBox('Unsupported ArcGIS Version ' + ArcRealVersion, mbCriticalError, MB_OK);
                ErrorDisplayed := True;
              end
              else
                // is there a 'bin64' sub dir?
                bin64dir := AddBackslash(ArcInstallDir) + 'bin64';
                HasBin64 := DirExists(bin64dir);
              
                Result := True

            end;
          end;
        end;
      end;
    end;
  end;

  if not Result and not ErrorDisplayed then
    MsgBox('Failed to read ArcGIS Install info. ArcGIS may not be installed on this PC', mbCriticalError, MB_OK)
end;

procedure InitializeWizard;
var
  InstallDirPage: TWizardPage;
  bgpresent: string;
begin
  // get the Dir page and customize the text
  InstallDirPage := PageFromID(wpSelectDir);

  if HasBin64 then
    bgpresent := 'with BGProcessor'
  else
    bgpresent := 'no BGProcessor';

  InstallDirPage.Description := FmtMessage('ArcGIS Version %1 (%2) has been found.'  + #13#10 + 'The support files for this version will be installed in the location below.', [ArcVersionClass, bgpresent]);
end; 

function GetArcGISDir(Value: string): string;
begin
  // return global var set in InitializeSetup()
  Result := ArcInstallDir;
end;

// convert from the arc version string to one of the version classes we recognise
function GetArcVersionClass(realVersion: string): string;
begin
  //if (CompareVersion(realVersion, '9.3') <> -1) and (CompareVersion(realVersion, '9.4') = -1) then
  //  Result := '9.3'
  //else if (CompareVersion(realVersion, '10.0') <> -1) and (CompareVersion(realVersion, '10.1') = -1) then
  //  Result := '10.0'
  //else if (CompareVersion(realVersion, '10.1') <> -1) and (CompareVersion(realVersion, '10.4') = -1) then
  //  Result := '10.1'
  if (CompareVersion(realVersion, '10.4') <> -1) and (CompareVersion(realVersion, '10.5') = -1) then
    Result := '10.4'
  else if (CompareVersion(realVersion, '10.5') <> -1) and (CompareVersion(realVersion, '10.5.1') = -1) then
    Result := '10.5'
  else if (CompareVersion(realVersion, '10.5.1') <> -1) and (CompareVersion(realVersion, '10.6') = -1) then
    Result := '10.5.1'
  else if (CompareVersion(realVersion, '10.6') <> -1) and (CompareVersion(realVersion, '10.6.1') = -1) then
    Result := '10.6'
  else if (CompareVersion(realVersion, '10.6.1') <> -1) and (CompareVersion(realVersion, '10.7') = -1) then
    Result := '10.6.1'
  else if (CompareVersion(realVersion, '10.7.1') <> -1) and (CompareVersion(realVersion, '10.8') = -1) then
    Result := '10.7.1'
  else if (CompareVersion(realVersion, '10.8') <> -1) and (CompareVersion(realVersion, '10.8.1') = -1) then
    Result := '10.8'
  else if (CompareVersion(realVersion, '10.8.1') <> -1) and (CompareVersion(realVersion, '10.8.9') = -1) then
    Result := '10.8.1'
  else if (CompareVersion(realVersion, '10.9') <> -1) and (CompareVersion(realVersion, '10.9.9') = -1) then
    Result := '10.9'
  else
    Result := ''
end;


// run after the files have been copied. Updates the RasterFormats.dat
procedure CurStepChanged(CurStep: TSetupStep);
var
  rfpath : string;
begin
  if CurStep = ssPostInstall then
  begin
    rfpath := ExpandConstant('{app}\bin\RasterFormats.dat');
    UpdateRasterFormats(rfpath);
    
    // bin64 there also?
    rfpath := ExpandConstant('{app}\bin64\RasterFormats.dat');
    if FileExists(rfpath) then
    begin
      UpdateRasterFormats(rfpath);
    end;
  end;
end;
