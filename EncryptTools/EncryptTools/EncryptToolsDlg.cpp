
// EncryptToolsDlg.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "EncryptTools.h"
#include "EncryptToolsDlg.h"
#include "afxdialogex.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CEncryptToolsDlg �Ի���




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


// CEncryptToolsDlg ��Ϣ�������

BOOL CEncryptToolsDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// ���ô˶Ի����ͼ�ꡣ��Ӧ�ó��������ڲ��ǶԻ���ʱ����ܽ��Զ�
	//  ִ�д˲���
	SetIcon(m_hIcon, TRUE);			// ���ô�ͼ��
	SetIcon(m_hIcon, FALSE);		// ����Сͼ��

	// TODO: �ڴ���Ӷ���ĳ�ʼ������

	return TRUE;  // ���ǽ��������õ��ؼ������򷵻� TRUE
}

// �����Ի��������С����ť������Ҫ����Ĵ���
//  �����Ƹ�ͼ�ꡣ����ʹ���ĵ�/��ͼģ�͵� MFC Ӧ�ó���
//  �⽫�ɿ���Զ���ɡ�

void CEncryptToolsDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // ���ڻ��Ƶ��豸������

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// ʹͼ���ڹ����������о���
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// ����ͼ��
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

//���û��϶���С������ʱϵͳ���ô˺���ȡ�ù��
//��ʾ��
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
			//AfxMessageBox(L"�������");
		}
	}
	if (m_vecFilePaths.size() == 0)
	{
		AfxMessageBox(L"�����ڿɼ����ļ�");
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
			//AfxMessageBox(L"�������");
		}	
	}
	if (m_vecFilePaths.size() == 0)
	{
		AfxMessageBox(L"�����ڿɽ����ļ�");
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

// ���Ŀ¼���ļ��Ƿ�Ϸ�
bool CEncryptToolsDlg::CheckPathValid(CString path)
{
	DWORD dwAttr = GetFileAttributes(path);  //�õ��ļ����� 

	if (dwAttr == 0xFFFFFFFF)    // �ļ���Ŀ¼������
		return false;
	return true;
}


// �ӽ����ļ���Ŀ¼
void CEncryptToolsDlg::EncryptOrDecryptFile(CString _path,bool bEncrypt)
{
	//�ж�_path��Ŀ¼�����ļ����ļ�ֱ�Ӽӽ��ܣ��������Ŀ¼
	DWORD dwAttr = GetFileAttributes(_path);  //�õ��ļ����� 
	if(dwAttr == FILE_ATTRIBUTE_DIRECTORY)
	{
		CFileFind fileFinder;
		CString filePath = _path + _T("//*.*");
		BOOL bFinished = fileFinder.FindFile(filePath);
		while(bFinished)  //ÿ��ѭ����Ӧһ�����Ŀ¼
		{
			bFinished = fileFinder.FindNextFile();
			if(fileFinder.IsDirectory() && !fileFinder.IsDots())  //����Ŀ¼��ݹ���ô˷���
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
	if(!bEncrypt && fileExt == _T(".syg"))  //����
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
		fileExt == _T(".bak") || fileExt == _T(".lua"))) //����
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
	if(!bEncrypt && fileExt == _T(".syg"))  //����
		m_vecFilePaths.push_back(_filepath);
	else if(bEncrypt && (fileExt == _T(".conf") || fileExt == _T(".png") || fileExt == _T(".jpg") || 
		fileExt == _T(".bak") || fileExt == _T(".lua"))) //����
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
		AfxMessageBox(L"�������");
		KillTimer(nIDEvent);
	}

	CDialogEx::OnTimer(nIDEvent);
}
