#include "BackupFile.h"

#define TCP_PORT 7780
#define WM_BACKFILE WM_USER + 101
#define MAX_SEND_FILE_SIZE 64 * 1024

typedef struct tagCommandFtp
{

	int CommandID;

	DWORD DataSize;
	char cIpAddress[128];
	char cRunType[10];
	char cNotice[128];
	char cNoticeType[100];
	char cFileName[128];
}COMMANDFTP,*PCOMMANDFTP;

typedef struct _FileBackUp
{
	char szName[128];
	char szSendTime[128];
	char szReason[512];
	int  bIsFile;
}FileBackUp, *PFileBackUp; 

typedef struct _BACKUPFileInfo
{
	char szFilePath[MAX_PATH];
	DWORD FileSize;
}BACKUPFILEINFO, *PBACKUPFILEINFO;

typedef struct _ISFILE
{
	BOOL bIsFile;
	BOOL bError;
	BOOL bNeedCoverPath;
	DWORD dwDeleteAfter;
	DWORD dwUserData;
	DWORD dwMaxFileSize;
	MANUALCALLBACK ManulCallBack;
	LPMANUALCALLBACKPROC pfnCallBackProc;
	ANSIPATH *pFileName;
	UINT uFileConut;
	char szExtName[130];
	UINT uExtCount;
	CString strFileName;
	CString strPathSendToServer;
	CString strServerIP;
	CString strReason;
	_ISFILE()
	{
		dwDeleteAfter = 0;
		bIsFile = 0;
		bNeedCoverPath = 0;
		bError = 0;
		dwUserData = 0;
		dwMaxFileSize = 0;
		pfnCallBackProc = 0;
		pFileName = 0;
		uFileConut = 0;
		ZeroMemory(szExtName,sizeof szExtName);
		ZeroMemory(&ManulCallBack, sizeof ManulCallBack);
		uExtCount = 0;
	};
}ISFILE,*PISFILE;

BOOL BackFileSendAndClose(CString strFileName, HANDLE hFile, CString strFileNameSendToServer, PISFILE pIsFile);
HANDLE BackFileOpenFile(CString strFileName, PISFILE pIsFile);
BOOL SendDataToServer(PVOID pData, DWORD dwSize);
BOOL BackFileInit();