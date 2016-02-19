; CLW file contains information for the MFC ClassWizard

[General Info]
Version=1
ODLFile=UpdateVersion.odl
ClassCount=3
Class1=CUpdateVersionApp
LastClass=CZipProj
NewFileInclude2=#include "UpdateVersion.h"
ResourceCount=2
NewFileInclude1=#include "stdafx.h"
LastTemplate=CDialog
Class2=COptionDlg
Resource1=IDD_OPTION
Class3=CZipProj
Resource2=IDD_ZIPPRJFOLDER

[CLS:CUpdateVersionApp]
Type=0
HeaderFile=UpdateVersion.h
ImplementationFile=UpdateVersion.cpp
Filter=N
LastObject=CUpdateVersionApp

[DLG:IDD_OPTION]
Type=1
Class=COptionDlg
ControlCount=25
Control1=IDC_STATIC,button,1342178055
Control2=IDC_CKPROJECTPATH,button,1342308361
Control3=IDC_CKROOTPATH,button,1342177289
Control4=IDC_CHECKSVN,button,1342242819
Control5=IDC_STATIC,static,1342308352
Control6=IDC_STATIC,static,1342308352
Control7=IDC_STATIC,static,1342308352
Control8=IDC_STATIC,static,1342308352
Control9=IDC_VERMAJOR,edit,1350631552
Control10=IDC_VERMINOR,edit,1350631552
Control11=IDC_VERREVISION,edit,1350631552
Control12=IDC_VERBUILD,edit,1350631552
Control13=IDC_LOCKVERMAJOR,button,1342242819
Control14=IDC_LOCKVERREVISION,button,1342242819
Control15=IDC_LOCKVERMINOR,button,1342242819
Control16=IDC_LOCKVERBUILD,button,1342242819
Control17=IDC_CHECKSELFUPDATA,button,1342242819
Control18=IDC_STATIC,button,1342177287
Control19=IDC_STATIC,button,1342178055
Control20=IDC_PDBSERVER,SysIPAddress32,1342242816
Control21=IDC_AUTOCOMMIT,button,1342242819
Control22=IDC_STATIC,button,1342177287
Control23=IDC_NEEDCOMMITLIST,listbox,1352728843
Control24=IDC_STATIC,button,1342178055
Control25=IDC_SETGLOBLE,button,1342242819

[CLS:COptionDlg]
Type=0
HeaderFile=OptionDlg.h
ImplementationFile=OptionDlg.cpp
BaseClass=CDialog
Filter=D
LastObject=COptionDlg
VirtualFilter=dWC

[DLG:IDD_ZIPPRJFOLDER]
Type=1
Class=CZipProj
ControlCount=13
Control1=IDOK,button,1342242817
Control2=IDCANCEL,button,1342242816
Control3=IDC_STATIC,button,1342177287
Control4=IDC_RAD_WORKSPACE,button,1342308361
Control5=IDC_RAD_FOLDER,button,1342177289
Control6=IDC_EDIT_SRC_PATH,edit,1342242944
Control7=IDC_STATIC,static,1342308352
Control8=IDC_BTN_SELECT_SRC,button,1342242816
Control9=IDC_STATIC,static,1342308352
Control10=IDC_EDIT_FILETYPE,edit,1342244992
Control11=IDC_STATIC,static,1342308352
Control12=IDC_EDIT_ZIPOUTFILE,edit,1342242944
Control13=IDC_BTN_OUTFILE,button,1342242816

[CLS:CZipProj]
Type=0
HeaderFile=ZipProj.h
ImplementationFile=ZipProj.cpp
BaseClass=CDialog
Filter=D
LastObject=1081

