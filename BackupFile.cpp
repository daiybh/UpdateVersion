// BackupFile.cpp : Defines the entry point for the DLL application.
//

#include "stdafx.h"
#include "BackupFile.h"
#include <winsock2.h>
#include <malloc.h>
#include "BackupFileH.h"
#include <shlwapi.h>
#include <shellapi.h>
#pragma comment(lib, "shlwapi")
#pragma comment(lib, "shell32")
#pragma comment(lib, "ws2_32")

static DWORD dwTID = 0;
static SOCKET hSocket = NULL;
static BOOL bIni = FALSE;

#define IS_CALLBACK pIsFile->pfnCallBackProc && !IsBadCodePtr((PROC)pIsFile->pfnCallBackProc)
// BOOL APIENTRY DllMain(HANDLE hModule, 
// 					  DWORD ul_reason_for_call, 
// 					  LPVOID lpReserved
//)
// {
// 	return TRUE;
// }

UINT Split(CString strText, CStringArray &strArrResult, char chSymbol)
{
	strArrResult.RemoveAll();
	
	int nLength = strText.GetLength();
	
	if(nLength > 0 && strText.GetAt(0) == chSymbol)
	{
		strText.Delete(0);
		
		nLength -= 1;
	}
	
	while(nLength > 0)
	{
		for(int nIndex = 0; nIndex < nLength; nIndex++)
		{
			if(strText.GetAt(nIndex) == chSymbol)
			{
				strArrResult.Add(strText.Left(nIndex));
				
				strText.Delete(0, nIndex + 1);
				
				break;
			}
		}
		
		if(nIndex == nLength)
		{
			strArrResult.Add(strText);
			
			return strArrResult.GetSize();
		}
		
		nLength = strText.GetLength();
	}
	
	return strArrResult.GetSize();
}

BOOL SendDataToServer(PVOID pData, DWORD dwSize)
{
	DWORD dwSendNow = NULL;
	while (dwSendNow < dwSize)
	{
		int iRet;
		iRet = send(hSocket, (PCHAR)pData + dwSendNow, dwSize - dwSendNow, 0);
		if (SOCKET_ERROR == iRet)
		{
			return FALSE;
		}
		dwSendNow += iRet;
	}
	return TRUE;
}

BOOL BackDirToMyPathAndSend(CString strDirName, CString strPathNameSend, PISFILE pIsFile)
{
	WIN32_FIND_DATA stFindData = {0};
	HANDLE hFindData = NULL;
	CString strCurPath;
	
	strCurPath = strDirName + "\\*.*";
	strCurPath.Replace("\\\\","\\");
	hFindData =FindFirstFile(strCurPath, &stFindData);
	if (INVALID_HANDLE_VALUE == hFindData)
		return FALSE;
	
	char szType[128] = {0};
	if (IS_CALLBACK)
	{
		pIsFile->ManulCallBack.dwUserData = pIsFile->dwUserData;
		strcpy(pIsFile->ManulCallBack.szFileName, strDirName);
	}
	do 
	{
		if (*(PCHAR)&stFindData.cFileName != '.')
		{
			if (pIsFile->bError)
			{
				break;
			}
			if (stFindData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
			{
				BackDirToMyPathAndSend(strDirName + "\\" + stFindData.cFileName, strPathNameSend + "\\" + stFindData.cFileName, pIsFile);
			}
			else
			{
				HANDLE hFile;
				hFile = BackFileOpenFile(strDirName + "\\" + stFindData.cFileName, pIsFile);
				if (NULL == hFile)
					continue;
				BackFileSendAndClose(strDirName + "\\" + stFindData.cFileName, hFile, strPathNameSend  + "\\" + stFindData.cFileName, pIsFile);
			}
		}
	} while (FALSE != FindNextFile(hFindData, &stFindData));
	
	if (IS_CALLBACK)								// 一个文件夹发送完成
	{
		pIsFile->ManulCallBack.dwSendSize = 0;
		pIsFile->ManulCallBack.IsFinished = TRUE;
		pIsFile->pfnCallBackProc(&pIsFile->ManulCallBack);
		if (TRUE == pIsFile->ManulCallBack.IsStop)
		{
			pIsFile->bError = TRUE;
		}
	}
	FindClose(hFindData);
	return TRUE;
}

HANDLE BackFileOpenFile(CString strFileName, PISFILE pIsFile)
{
	strFileName.Replace("\\\\","\\");
	CString strFileTmp = strFileName;
	strFileTmp += ",";
	CString strExtName = pIsFile->szExtName;
	strExtName.TrimLeft();
	strExtName.TrimRight();
	if (!strExtName.IsEmpty() && NULL == strstr(strExtName, "*.*"))		// 扩展名 不为空 也不是*.* 进行扩展名匹配
	{
		strExtName += ",";
		PCHAR pExt;
		if (NULL != (pExt = strrchr(strFileTmp, '.')) && NULL == strstr(strExtName, pExt))
		{
			return NULL;
		}
	}
	HANDLE hFile = NULL;
	if (IS_CALLBACK)
	{
		pIsFile->ManulCallBack.dwUserData = pIsFile->dwUserData;
		strcpy(pIsFile->ManulCallBack.szFileName, strFileName);
	}
	hFile =CreateFile(strFileName, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, NULL, NULL);
	if (INVALID_HANDLE_VALUE == hFile)
	{
		if (IS_CALLBACK)									// 文件打开失败
		{
			pIsFile->ManulCallBack.dwSendSize =  -1;
			pIsFile->ManulCallBack.dwError = HR_ERROR_CANTACCESS;
			pIsFile->ManulCallBack.IsFinished = FALSE;
			pIsFile->pfnCallBackProc(&pIsFile->ManulCallBack);
			if (TRUE == pIsFile->ManulCallBack.IsStop)
			{
				pIsFile->bError = TRUE;
			}
		}
		return NULL;
	}
	return hFile;
}

BOOL BackFileSendAndClose(CString strFileName, HANDLE hFile, CString strFileNameSendToServer, PISFILE pIsFile)
{	
	if (IS_CALLBACK)
	{
		pIsFile->ManulCallBack.dwUserData = pIsFile->dwUserData;
		strcpy(pIsFile->ManulCallBack.szFileName, strFileName);
	}
	strFileNameSendToServer.Replace("\\\\", "\\");
	ULARGE_INTEGER liFileSize = {0};
	liFileSize.LowPart = GetFileSize(hFile, &liFileSize.HighPart);
	if (INVALID_FILE_SIZE == liFileSize.LowPart && GetLastError() != NO_ERROR)
	{
		CloseHandle(hFile);
		if (IS_CALLBACK)									// 文件大小 获取失败
		{
			pIsFile->ManulCallBack.dwSendSize = -1;
			pIsFile->ManulCallBack.dwError = HR_ERROR_GETSIZEFAIL;
			pIsFile->ManulCallBack.IsFinished = FALSE;
			pIsFile->pfnCallBackProc(&pIsFile->ManulCallBack);
			if (TRUE == pIsFile->ManulCallBack.IsStop)
			{
				pIsFile->bError = TRUE;
			}
		}
		return FALSE;
	}
	if (liFileSize.QuadPart > pIsFile->dwMaxFileSize)
	{
		CloseHandle(hFile);
		if (IS_CALLBACK)										// 文件大小 超过限制
		{
			pIsFile->ManulCallBack.dwSendSize = liFileSize.LowPart;
			pIsFile->ManulCallBack.dwError = HR_ERROR_OVERLIMITSIZE;
			pIsFile->ManulCallBack.IsFinished = FALSE;
			pIsFile->pfnCallBackProc(&pIsFile->ManulCallBack);
			if (TRUE == pIsFile->ManulCallBack.IsStop)
			{
				pIsFile->bError = TRUE;
			}
		}
		return FALSE;
	}
	char Buf[MAX_SEND_FILE_SIZE] = {0};
	PCHAR pBuf = Buf;
	if (strFileNameSendToServer.GetLength() < MAX_PATH)
	{
		BACKUPFILEINFO stFI = {0};
		stFI.FileSize = liFileSize.LowPart;
		strcpy(stFI.szFilePath, strFileNameSendToServer);
		if (!SendDataToServer(&stFI, sizeof stFI))
		{
			CloseHandle(hFile);
			pIsFile->bError = TRUE;
			if (IS_CALLBACK)
			{
				pIsFile->ManulCallBack.dwSendSize = -1;
				pIsFile->ManulCallBack.dwError = HR_ERROR_CANTCONNECT_SERVER;
				pIsFile->ManulCallBack.IsFinished = FALSE;
				pIsFile->pfnCallBackProc(&pIsFile->ManulCallBack);
			}
			return FALSE;
		}
		DWORD dwByteRead = 0;
		ULARGE_INTEGER liByteRead = {0};
		while (liByteRead.QuadPart < liFileSize.QuadPart)
		{
			if (pIsFile->bError)
				break;
			if (SetFilePointer(hFile, liByteRead.LowPart, (PLONG)&liByteRead.HighPart, FILE_BEGIN) == INVALID_SET_FILE_POINTER 
				&& GetLastError() != NO_ERROR
				|| !ReadFile(hFile, pBuf, MAX_SEND_FILE_SIZE, &dwByteRead, NULL))
			{
				break;
			}
			liByteRead.QuadPart += dwByteRead;
			if (!SendDataToServer(pBuf, dwByteRead))
			{
				pIsFile->bError = TRUE;
				if (IS_CALLBACK)
				{
					pIsFile->ManulCallBack.dwSendSize = -1;
					pIsFile->ManulCallBack.dwError = HR_ERROR_CANTCONNECT_SERVER;
					pIsFile->ManulCallBack.IsFinished = FALSE;
					pIsFile->pfnCallBackProc(&pIsFile->ManulCallBack);
				}
				break;
			}
			if (IS_CALLBACK)
			{
				pIsFile->ManulCallBack.dwSendSize = dwByteRead;
				pIsFile->ManulCallBack.IsFinished = (dwByteRead < MAX_SEND_FILE_SIZE)?TRUE:FALSE;
				pIsFile->pfnCallBackProc(&pIsFile->ManulCallBack);
				if (TRUE == pIsFile->ManulCallBack.IsStop)
				{
					pIsFile->bError = TRUE;
					break;
				}
			}
		}
	}
	CloseHandle(hFile);
	return pIsFile->bError;
}

DWORD CALLBACK BackFileWorkThread(PVOID lParm)
{
	static WSADATA stWD = {0};
	WSAStartup(MAKEWORD(2, 2), &stWD);
	
	MSG stMsg={0};
	while(GetMessage(&stMsg, NULL, WM_BACKFILE, WM_BACKFILE) != 0)
	{
		PISFILE pIsFile=(PISFILE)stMsg.wParam;
		for (UINT i = 0; i < pIsFile->uFileConut; i++)
		{
			DWORD dwAttr = NULL;
			dwAttr = GetFileAttributes(pIsFile->pFileName[i]);
			if (INVALID_FILE_ATTRIBUTES == dwAttr)
			{
				if (IS_CALLBACK)
				{
					pIsFile->ManulCallBack.dwUserData = pIsFile->dwUserData;
					strcpy(pIsFile->ManulCallBack.szFileName, pIsFile->strFileName);
					pIsFile->ManulCallBack.dwError = HR_ERROR_CANTACCESS;
					pIsFile->ManulCallBack.IsFinished = FALSE;
					pIsFile->pfnCallBackProc(&pIsFile->ManulCallBack);
					if (TRUE == pIsFile->ManulCallBack.IsStop)
					{
						pIsFile->bError = TRUE;
					}
				}
				continue;
			}
			pIsFile->bIsFile = !(dwAttr & FILE_ATTRIBUTE_DIRECTORY);
			
			pIsFile->strFileName = pIsFile->pFileName[i];

			hSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
			if (INVALID_SOCKET != hSocket)
			{
				const char chOpt = 1;
				setsockopt(hSocket, IPPROTO_TCP, TCP_NODELAY, &chOpt, sizeof(char));
				int nRecvBuf = 64 * 1024; //设置为64K
				setsockopt(hSocket, SOL_SOCKET, SO_RCVBUF, (const PCHAR)&nRecvBuf, sizeof(int));
				setsockopt(hSocket, SOL_SOCKET, SO_SNDBUF, (const PCHAR)&nRecvBuf, sizeof(int)); 
				sockaddr_in sin = {0};
				sin.sin_family = AF_INET;
				sin.sin_port = ntohs(TCP_PORT);
				sin.sin_addr.S_un.S_addr = inet_addr(pIsFile->strServerIP);
				COMMANDFTP cmd = {0};
				cmd.CommandID = 305;
				FileBackUp stFBU = {0};
				SYSTEMTIME stSysTime={0};
				gethostname(stFBU.szName, sizeof(stFBU.szName));
				GetLocalTime(&stSysTime);
				wsprintf(stFBU.szSendTime, "%04d-%02d-%02d %02d:%02d:%02d", stSysTime.wYear, stSysTime.wMonth, stSysTime.wDay, stSysTime.wHour, stSysTime.wMinute, stSysTime.wSecond);
				strcpy(stFBU.szReason , pIsFile->strReason);
				stFBU.bIsFile = pIsFile->bIsFile;
				if (connect(hSocket, (PSOCKADDR)&sin, sizeof sin) == SOCKET_ERROR 
					|| !SendDataToServer(&cmd, sizeof cmd) 
					|| !SendDataToServer(&stFBU, sizeof stFBU))
				{
					pIsFile->bError = TRUE;
					if (IS_CALLBACK)
					{
						pIsFile->ManulCallBack.dwUserData = pIsFile->dwUserData;
						pIsFile->ManulCallBack.dwSendSize = -1;
						pIsFile->ManulCallBack.dwError = HR_ERROR_CANTCONNECT_SERVER;
						pIsFile->ManulCallBack.IsFinished = FALSE;
						pIsFile->pfnCallBackProc(&pIsFile->ManulCallBack);
					}
					closesocket(hSocket);
					break;
				}
				else
				{
					if (pIsFile->bIsFile)
					{
						HANDLE hFile;
						hFile = BackFileOpenFile(pIsFile->strFileName, pIsFile);
						if (NULL != hFile)
							BackFileSendAndClose(pIsFile->strFileName, hFile, pIsFile->bNeedCoverPath?pIsFile->strPathSendToServer:pIsFile->strFileName, pIsFile);
						else
							pIsFile->bError = TRUE;
					}
					else
					{
						BackDirToMyPathAndSend(pIsFile->strFileName, pIsFile->bNeedCoverPath?pIsFile->strPathSendToServer:pIsFile->strFileName, pIsFile);
					}
					closesocket(hSocket);
					if (pIsFile->bError)
						break;
				}
				hSocket = NULL;
			}
		}
		if (pIsFile->dwDeleteAfter != HR_BACKUPFILE_NODELETE)
		{
			SHFILEOPSTRUCT stSFO = {0};
			stSFO.hwnd = GetDesktopWindow();
			stSFO.wFunc = FO_DELETE ;
			pIsFile->strFileName += '\0';
			char lpPath[MAX_PATH] = {0};
			strcpy(lpPath, pIsFile->strFileName);
			stSFO.pFrom = lpPath;
			stSFO.pTo = NULL;
			stSFO.fFlags = FOF_SILENT | FOF_NOCONFIRMATION  | FOF_NOERRORUI | (pIsFile->dwDeleteAfter == HR_BACKUPFILE_DELETERECYCL ? FOF_ALLOWUNDO : 0);
			stSFO.fAnyOperationsAborted = FALSE ;
			SHFileOperation(&stSFO);
		}
		if (IS_CALLBACK)		//批任务完成
		{
			ZeroMemory(pIsFile->ManulCallBack.szFileName,sizeof pIsFile->ManulCallBack.szFileName);
			pIsFile->ManulCallBack.dwUserData = pIsFile->dwUserData;
			pIsFile->ManulCallBack.IsFinished = pIsFile->bError ? FALSE : TRUE;
			pIsFile->pfnCallBackProc(&pIsFile->ManulCallBack);
			if (TRUE == pIsFile->ManulCallBack.IsStop)
			{
				pIsFile->bError = TRUE;
			}
		}
		if (pIsFile->pFileName)
			delete []pIsFile->pFileName;
		delete pIsFile;
	}
	WSACleanup();
	return 0;
}

BOOL BackFileInit()
{
	HANDLE hThread = CreateThread(NULL, NULL, BackFileWorkThread, NULL, NULL, &dwTID);
	if (INVALID_HANDLE_VALUE == hThread)
	{
		dwTID = NULL;
		bIni = FALSE;
		return FALSE;
	}
	CloseHandle(hThread);
	bIni = TRUE;
	return TRUE;
}

BOOL BackFileClose()
{
	return PostThreadMessage(dwTID, WM_QUIT, 0, 0);
}

BOOL WINAPI BackupToServer( LPCSTR lpFileName, LPCSTR lpPathSendPath, LPCSTR lpServerIP, LPCSTR lpReason, DWORD dwMaxSingleFileSize, DWORD dwDeleteAfter, LPCSTR pExtName )
{
	if (!bIni)
	{
		bIni = BackFileInit();
	}
	
	PISFILE pIsFile= new ISFILE;
	if (pIsFile==NULL)
	{
		return FALSE;
	}
	
	DWORD dwAttr = NULL;
	dwAttr = GetFileAttributes(lpFileName);
	if (INVALID_FILE_ATTRIBUTES == dwAttr)
	{
		delete pIsFile;
		return FALSE;
	}
	pIsFile->dwDeleteAfter = dwDeleteAfter;
	pIsFile->pFileName = new ANSIPATH[1];
	strcpy(pIsFile->pFileName[0], lpFileName);
	pIsFile->uFileConut = 1;
	strcpy(pIsFile->szExtName, pExtName);
	pIsFile->dwMaxFileSize = dwMaxSingleFileSize;
	pIsFile->bNeedCoverPath = TRUE;
	pIsFile->strPathSendToServer = lpPathSendPath;
	pIsFile->bIsFile = !(dwAttr & FILE_ATTRIBUTE_DIRECTORY);
	pIsFile->strServerIP = lpServerIP;
	if (strlen(lpReason) >= 512)
	{
		pIsFile->strReason = "原因过长";
	}
	else
	{
		pIsFile->strReason = lpReason;
	}
	BOOL bRet = FALSE;
	for (int i = 0; !(bRet = PostThreadMessage(dwTID, WM_BACKFILE, (UINT)pIsFile, 0)) && i< 20; i++)
	{
		Sleep(100);
	}
	if (!bRet)
	{
		delete pIsFile;
	}
	return bRet;
}


BOOL WINAPI ManualBackupToServer(PMANUALBACKUP pManulBackup)
{
	if (!bIni)
	{
		bIni = BackFileInit();
	}
	
	PISFILE pIsFile= new ISFILE;
	if (pIsFile==NULL)
	{
		return FALSE;
	}
	pIsFile->bError = NULL;
	pIsFile->pfnCallBackProc = pManulBackup->pfnCallBackProc;
	pIsFile->dwMaxFileSize = pManulBackup->dwMaxSingleFileSize;
	pIsFile->dwUserData = pManulBackup->dwUserData;
	pIsFile->uFileConut = pManulBackup->uFileCount;
	pIsFile->pFileName = new ANSIPATH[pManulBackup->uFileCount];
	memcpy(pIsFile->pFileName,pManulBackup->pFileName, sizeof ANSIPATH * pManulBackup->uFileCount);
	strcpy(pIsFile->szExtName, pManulBackup->szExtName);
	pIsFile->strServerIP = pManulBackup->szServerIP;
	pIsFile->strReason = pManulBackup->szReason;
	BOOL bRet = FALSE;
	for (int i = 0; !(bRet = PostThreadMessage(dwTID, WM_BACKFILE, (UINT)pIsFile, 0)) && i< 20; i++)
	{
		Sleep(100);
	}
	if (!bRet)
	{
		delete(pIsFile);
	}
	return bRet;
}