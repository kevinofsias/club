
// EncryptTools.h : PROJECT_NAME Ӧ�ó������ͷ�ļ�
//

#pragma once

#ifndef __AFXWIN_H__
	#error "�ڰ������ļ�֮ǰ������stdafx.h�������� PCH �ļ�"
#endif

#include "resource.h"		// ������


// CEncryptToolsApp:
// �йش����ʵ�֣������ EncryptTools.cpp
//

class CEncryptToolsApp : public CWinApp
{
public:
	CEncryptToolsApp();

// ��д
public:
	virtual BOOL InitInstance();

// ʵ��

	DECLARE_MESSAGE_MAP()
};

extern CEncryptToolsApp theApp;