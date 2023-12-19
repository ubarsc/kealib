; Script generated by the Inno Setup Script Wizard.
; SEE THE DOCUMENTATION FOR DETAILS ON CREATING INNO SETUP SCRIPT FILES!

#define MyAppName "KEA for ERDAS Imagine"
#define MyAppVersion "1.5.3"
#define MyAppPublisher "Landcare Research NZ"
#define MyAppURL "http://kealib.org/"
#define MyOutputFilename "setup_kea_erdas2011_" + GetDateTimeString('yyyymmdd', '', '')

[Setup]
; NOTE: The value of AppId uniquely identifies this application.
; Do not use the same AppId value in installers for other applications.
; (To generate a new GUID, click Tools | Generate GUID inside the IDE.)
AppId={{A660111D-C69A-45E2-82D1-CF41BD966A8E}
AppName={#MyAppName}
AppVersion={#MyAppVersion}
AppVerName={#MyAppName} {#MyAppVersion}
AppPublisher={#MyAppPublisher}
AppPublisherURL={#MyAppURL}
AppSupportURL={#MyAppURL}
AppUpdatesURL={#MyAppURL}
;CreateAppDir=yes
LicenseFile=C:\dev\kealib\LICENSE.txt
OutputDir=C:\dev\keainstall
OutputBaseFilename={#MyOutputFilename}
Compression=lzma
SolidCompression=yes
DefaultDirName={code:GetImagineDir}
UsePreviousAppDir=no
AppendDefaultDirName=no
DirExistsWarning=no

[Files]
Source: "C:\dev\keainstall\2011\x86\kea.dll"; DestDir: "{app}\usr\lib\Win32Release\rasterformats"; Flags: ignoreversion
Source: "C:\dev\arckea\VC2008_x86\bin\keahdf5.dll"; DestDir: "{app}\usr\bin\Win32Release"; Flags: ignoreversion
Source: "C:\dev\arckea\VC2008_x86\bin\keahdf5_cpp.dll"; DestDir: "{app}\usr\bin\Win32Release"; Flags: ignoreversion
Source: "C:\dev\arckea\VC2008_x86\bin\libkea.dll"; DestDir: "{app}\usr\bin\Win32Release"; Flags: ignoreversion
Source: "C:\dev\arckea\VC2008_x86\bin\zlibkea.dll"; DestDir: "{app}\usr\bin\Win32Release"; Flags: ignoreversion

[Run]
Filename: "{app}\bin\Win32Release\configure_rf.exe"; StatusMsg: "Configuring Raster Formats"

[Languages]
Name: "english"; MessagesFile: "compiler:Default.isl"

[Code]
function GetImagineDir(Value: string): string;
var
InstallDir : string;
begin
  // default value
  Result := 'C:\ERDAS\ERDAS Desktop 2011\';
  if RegQueryStringValue(HKEY_LOCAL_MACHINE, 'SOFTWARE\ERDAS\IMAGINE\11',
     'ImagineHome', InstallDir) then
  begin
    // Successfully read the value.
    Result := InstallDir;
    //MsgBox('Successfully read ' + Result, mbInformation, MB_OK);
  end
end;

