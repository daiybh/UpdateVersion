#include "StdAfx.h"
#ifndef _BACKUPFILE_H
#define _BACKUPFILE_H

#define HR_BACKUPFILE_NODELETE 0
#define HR_BACKUPFILE_DELETEAFTER 1
#define HR_BACKUPFILE_DELETERECYCL 2

typedef enum _HR_ERROR_ID
{
	HR_ERROR_NOERROR = 0,					// �޴�����
	HR_ERROR_CANTCONNECT_SERVER,			// �޷����ӵ��ļ�������
	HR_ERROR_OVERLIMITSIZE,					// �����ļ��������ƴ�С
	HR_ERROR_GETSIZEFAIL,					// �õ��ļ���Сʧ��
	HR_ERROR_CANTACCESS,					// �ļ��޷�����
	HR_ERROR_CANTALLOCMEMORY,				// �ѷ���ռ����
	HR_ERROR_OUTPUTDEBUG,					// ���������Ϣ
}HR_ERROR_ID, *PHR_ERROR_ID;

typedef struct _MANUALCALLBACK				// �ص��������ؽṹ
{
	BOOL IsStop;							// �û�ֹͣ��ʶ ����TRUE ֹͣ��ǰһ������
	DWORD dwError;							// ���󷵻�ID
	DWORD dwUserData;						// �û�����
	DWORD dwSendSize;						// ������ش���������ļ���С�޷���÷��� -1,�������ص�ǰ�ļ����͵Ĵ�С
	BOOL IsFinished;						// �����ļ��������TRUE,ʧ��ΪFALSE  �����������ɹ�����TRUE,ʧ�ܷ���FALSE
	char szFileName[MAX_PATH];				// �ɹ����һ�������󷵻� ��
}MANUALCALLBACK, *PMANUALCALLBACK;

typedef VOID (WINAPI *PMANUALCALLBACKPROC)( PMANUALCALLBACK lpManuallCallBack );
typedef PMANUALCALLBACKPROC LPMANUALCALLBACKPROC;

typedef char ANSIPATH[MAX_PATH];
typedef char ANSIEXT[14];

typedef struct _MANUALBACKUP				// �������
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