
// EncryptToolsDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "EncryptTools.h"
#include "EncryptToolsDlg.h"
#include "afxdialogex.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CEncryptToolsDlg 对话框




CEncryptToolsDlg::CEncryptToolsDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CEncryptToolsDlg::IDD, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CEncryptToolsDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_EDIT, m_path);
}

BEGIN_MESSAGE_MAP(CEncryptToolsDlg, CDialogEx)
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_WM_DROPFILES()
	ON_BN_CLICKED(IDC_ENCRYPT, &CEncryptToolsDlg::OnBnClickedEncrypt)
	ON_BN_CLICKED(IDC_DECRYPT, &CEncryptToolsDlg::OnBnClickedDecrypt)
	ON_WM_TIMER()
END_MESSAGE_MAP()


// CEncryptToolsDlg 消息处理程序

BOOL CEncryptToolsDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// 设置此对话框的图标。当应用程序主窗口不是对话框时，框架将自动
	//  执行此操作
	SetIcon(m_hIcon, TRUE);			// 设置大图标
	SetIcon(m_hIcon, FALSE);		// 设置小图标

	// TODO: 在此添加额外的初始化代码

	return TRUE;  // 除非将焦点设置到控件，否则返回 TRUE
}

// 如果向对话框添加最小化按钮，则需要下面的代码
//  来绘制该图标。对于使用文档/视图模型的 MFC 应用程序，
//  这将由框架自动完成。

void CEncryptToolsDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // 用于绘制的设备上下文

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// 使图标在工作区矩形中居中
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// 绘制图标
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

//当用户拖动最小化窗口时系统调用此函数取得光标
//显示。
HCURSOR CEncryptToolsDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}



void CEncryptToolsDlg::OnDropFiles(HDROP hDropInfo)
{
	// TODO: Add your message handler code here and/or call default
	UINT count;          
	TCHAR filePath[200];            
	count = DragQueryFile(hDropInfo, 0xFFFFFFFF, NULL, 0);   
	m_path.SetWindowText(L"");
	m_vecPaths.clear();
	if(count)           
	{
		CString paths;
		for(UINT i=0; i<count; i++)                    
		{
			int pathLen = DragQueryFile(hDropInfo, i, filePath, sizeof(filePath));
			m_path.GetWindowText(paths);
			paths += filePath;
			paths += "\r\n";
			m_path.SetWindowText(paths);
			m_vecPaths.push_back(filePath);
		}
	}
	DragFinish(hDropInfo); 
	CDialogEx::OnDropFiles(hDropInfo);
}


void CEncryptToolsDlg::OnBnClickedEncrypt()
{
	// TODO: Add your control notification handler code here
	m_vecFilePaths.clear();
	KillTimer(1);
	KillTimer(2);
	for (INT i = 0; i < m_vecPaths.size();i++)
	{
		if (CheckPathValid(m_vecPaths[i]))
		{
			EncryptOrDecryptFile(m_vecPaths[i],true);
			//AfxMessageBox(L"加密完成");
		}
	}
	if (m_vecFilePaths.size() == 0)
	{
		AfxMessageBox(L"不存在可加密文件");
	}
	else
	{
		ShowStatusTip(true);
		SetTimer(1,1,NULL);
	}
}


void CEncryptToolsDlg::OnBnClickedDecrypt()
{
	// TODO: Add your control notification handler code here
	m_vecFilePaths.clear();
	KillTimer(1);
	KillTimer(2);
	for (INT i = 0; i < m_vecPaths.size();i++)
	{
		if (CheckPathValid(m_vecPaths[i]))
		{
			EncryptOrDecryptFile(m_vecPaths[i],false);
			//AfxMessageBox(L"解密完成");
		}	
	}
	if (m_vecFilePaths.size() == 0)
	{
		AfxMessageBox(L"不存在可解密文件");
	}
	else
	{
		ShowStatusTip(true);
		SetTimer(2,1,NULL);
	}
}
void CEncryptToolsDlg::ShowStatusTip(bool bShow)
{
	CStatic *stc = (CStatic*)GetDlgItem(IDC_TIPS);
	stc->ShowWindow(bShow);
	stc = (CStatic*)GetDlgItem(IDC_NUM);
	stc->ShowWindow(bShow);
}

// 检查目录或文件是否合法
bool CEncryptToolsDlg::CheckPathValid(CString path)
{
	DWORD dwAttr = GetFileAttributes(path);  //得到文件属性 

	if (dwAttr == 0xFFFFFFFF)    // 文件或目录不存在
		return false;
	return true;
}


// 加解密文件或目录
void CEncryptToolsDlg::EncryptOrDecryptFile(CString _path,bool bEncrypt)
{
	//判断_path是目录还是文件，文件直接加解密，否则遍历目录
	DWORD dwAttr = GetFileAttributes(_path);  //得到文件属性 
	if(dwAttr == FILE_ATTRIBUTE_DIRECTORY)
	{
		CFileFind fileFinder;
		CString filePath = _path + _T("//*.*");
		BOOL bFinished = fileFinder.FindFile(filePath);
		while(bFinished)  //每次循环对应一个类别目录
		{
			bFinished = fileFinder.FindNextFile();
			if(fileFinder.IsDirectory() && !fileFinder.IsDots())  //若是目录则递归调用此方法
			{
				EncryptOrDecryptFile(fileFinder.GetFilePath(),bEncrypt);
			}
			else
			{
				CString fileName = fileFinder.GetFilePath();
				
				ParseAllFilePaths(fileName,bEncrypt);
			}
		}

		fileFinder.Close();
	}
	else
		ParseAllFilePaths(_path,bEncrypt);
}


bool CEncryptToolsDlg::DoEncrypt(CString fileName,bool bEncrypt)
{
	int dotPos=fileName.ReverseFind('.');
	CString fileExt=fileName.Right(fileName.GetLength()-dotPos);
	if(!bEncrypt && fileExt == _T(".syg"))  //解密
	{
		CFile file;
		BOOL bSuc = file.Open(fileName,CFile::typeBinary|CFile::modeRead);
		if(bSuc)
		{
			DWORD dwSize = file.GetLength();
			BYTE* pData = new BYTE[dwSize+1];
			file.Read(pData, dwSize);
			file.Close();
			pData[dwSize] = '\0';
			for (int i = 0; i < dwSize;i++)
			{
				pData[i] ^= gXOrTable[i%18];
			}

			bSuc = file.Open(fileName.Left(fileName.GetLength()-4),CFile::typeBinary|CFile::modeCreate|CFile::modeWrite);
			if(bSuc)
				file.Write(pData,dwSize);
			file.Close();
			delete [] pData;
			pData = NULL;
			SetFileAttributes(fileName,GetFileAttributes(fileName) & ~FILE_ATTRIBUTE_READONLY);
			DeleteFile(fileName);
		}
	}
	else if(bEncrypt && (fileExt == _T(".conf") || fileExt == _T(".png") || fileExt == _T(".jpg") || 
		fileExt == _T(".bak") || fileExt == _T(".lua"))) //加密
	{
		CFile file;
		BOOL bSuc = file.Open(fileName,CFile::typeBinary|CFile::modeRead);
		if(bSuc)
		{
			DWORD dwSize = file.GetLength();
			BYTE* pData = new BYTE[dwSize+1];
			file.Read(pData, dwSize);
			file.Close();
			pData[dwSize] = '\0';
			for (int i = 0; i < dwSize;i++)
			{
				pData[i] ^= gXOrTable[i%18];
			}

			bSuc = file.Open(fileName+".syg",CFile::typeBinary|CFile::modeCreate|CFile::modeWrite);
			if(bSuc)
				file.Write(pData,dwSize);
			file.Close();
			delete [] pData;
			pData = NULL;
			SetFileAttributes(fileName,GetFileAttributes(fileName) & ~FILE_ATTRIBUTE_READONLY);
			DeleteFile(fileName);
		}
	}
	
	return true;
}


void CEncryptToolsDlg::ParseAllFilePaths(CString _filepath, bool bEncrypt)
{
	int dotPos=_filepath.ReverseFind('.');
	CString fileExt=_filepath.Right(_filepath.GetLength()-dotPos);
	if(!bEncrypt && fileExt == _T(".syg"))  //解密
		m_vecFilePaths.push_back(_filepath);
	else if(bEncrypt && (fileExt == _T(".conf") || fileExt == _T(".png") || fileExt == _T(".jpg") || 
		fileExt == _T(".bak") || fileExt == _T(".lua"))) //加密
		m_vecFilePaths.push_back(_filepath);
}


void CEncryptToolsDlg::OnTimer(UINT_PTR nIDEvent)
{
	// TODO: Add your message handler code here and/or call default

	if(m_vecFilePaths.size()>0)
	{
		int num = min(m_vecFilePaths.size(),20);
		while(num > 0)
		{
			CString tips;
			tips.Format(L"%d",m_vecFilePaths.size());

			SetDlgItemText(IDC_NUM,tips);

			DoEncrypt(m_vecFilePaths[m_vecFilePaths.size()-1],nIDEvent == 1?true:false);
			m_vecFilePaths.pop_back();
			num--;
		}
		
	}
	if(m_vecFilePaths.size()==0&&((CStatic*)GetDlgItem(IDC_TIPS))->IsWindowVisible())
	{
		ShowStatusTip(false);
		AfxMessageBox(L"处理完成");
		KillTimer(nIDEvent);
	}

	CDialogEx::OnTimer(nIDEvent);
}
