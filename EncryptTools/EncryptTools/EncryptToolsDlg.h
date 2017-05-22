
// EncryptToolsDlg.h : ͷ�ļ�
//

#pragma once
#include "afxwin.h"
#include <vector>
const static BYTE gXOrTable[] = {
	0x69,0x33,0x22,0x69,0x33,0x22,0x69,0x33,0x22,0x69,0x33,0x22,0x69,0x33,0x22,0x69,0x33,0x22
};
// CEncryptToolsDlg �Ի���
class CEncryptToolsDlg : public CDialogEx
{
// ����
public:
	CEncryptToolsDlg(CWnd* pParent = NULL);	// ��׼���캯��

// �Ի�������
	enum { IDD = IDD_ENCRYPTTOOLS_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV ֧��


// ʵ��
protected:
	HICON m_hIcon;

	// ���ɵ���Ϣӳ�亯��
	virtual BOOL OnInitDialog();
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnDropFiles(HDROP hDropInfo);
	CEdit m_path;
	afx_msg void OnBnClickedEncrypt();
	afx_msg void OnBnClickedDecrypt();
	// ���Ŀ¼���ļ��Ƿ�Ϸ�
	bool CheckPathValid(CString path);
	// �ӽ����ļ���Ŀ¼
	void EncryptOrDecryptFile(CString _path,bool bEncrypt);
	bool DoEncrypt(CString _path,bool bEncrypt);

private:
	std::vector<CString> m_vecPaths;
	std::vector<CString> m_vecFilePaths;
public:
	void ParseAllFilePaths(CString _filepath, bool bEncrypt);
	void ShowStatusTip(bool bShow);
	afx_msg void OnTimer(UINT_PTR nIDEvent);
};
