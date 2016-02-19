#include "StdAfx.h"
#ifndef _BACKUPFILE_H
#define _BACKUPFILE_H

#define HR_BACKUPFILE_NODELETE 0
#define HR_BACKUPFILE_DELETEAFTER 1
#define HR_BACKUPFILE_DELETERECYCL 2

typedef enum _HR_ERROR_ID
{
	HR_ERROR_NOERROR = 0,					// 无错误发生
	HR_ERROR_CANTCONNECT_SERVER,			// 无法连接到文件服务器
	HR_ERROR_OVERLIMITSIZE,					// 单个文件超过限制大小
	HR_ERROR_GETSIZEFAIL,					// 得到文件大小失败
	HR_ERROR_CANTACCESS,					// 文件无法访问
	HR_ERROR_CANTALLOCMEMORY,				// 堆分配空间错误
	HR_ERROR_OUTPUTDEBUG,					// 输出调试信息
}HR_ERROR_ID, *PHR_ERROR_ID;

typedef struct _MANUALCALLBACK				// 回调函数返回结构
{
	BOOL IsStop;							// 用户停止标识 返回TRUE 停止当前一批任务
	DWORD dwError;							// 错误返回ID
	DWORD dwUserData;						// 用户数据
	DWORD dwSendSize;						// 如果有重大错误发生或文件大小无法获得返回 -1,正常返回当前文件发送的大小
	BOOL IsFinished;						// 单个文件发送完成TRUE,失败为FALSE  如果此批任务成功返回TRUE,失败返回FALSE
	char szFileName[MAX_PATH];				// 成功完成一批操作后返回 空
}MANUALCALLBACK, *PMANUALCALLBACK;

typedef VOID (WINAPI *PMANUALCALLBACKPROC)( PMANUALCALLBACK lpManuallCallBack );
typedef PMANUALCALLBACKPROC LPMANUALCALLBACKPROC;

typedef char ANSIPATH[MAX_PATH];
typedef char ANSIEXT[14];

typedef struct _MANUALBACKUP				// 传入参数
{
	DWORD dwUserData;
	ANSIPATH *pFileName;
	UINT uFileCount;
	char szExtName[130];
	DWORD dwMaxSingleFileSize;
	char szServerIP[20];
	char szReason[512];
	LPMANUALCALLBACKPROC pfnCallBackProc;
}MANUALBACKUP, *PMANUALBACKUP;

BOOL WINAPI ManualBackupToServer(PMANUALBACKUP pManulBackup);
BOOL WINAPI BackupToServer( LPCSTR lpFileName, LPCSTR lpPathSendPath, LPCSTR lpServerIP, LPCSTR lpReason, DWORD dwMaxSingleFileSize, DWORD dwDeleteAfter, LPCSTR pExtName);

#endif