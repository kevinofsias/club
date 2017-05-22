
// EncryptToolsDlg.h : 头文件
//

#pragma once
#include "afxwin.h"
#include <vector>
const static BYTE gXOrTable[] = {
	0x69,0x33,0x22,0x69,0x33,0x22,0x69,0x33,0x22,0x69,0x33,0x22,0x69,0x33,0x22,0x69,0x33,0x22
};
// CEncryptToolsDlg 对话框
class CEncryptToolsDlg : public CDialogEx
{
// 构造
public:
	CEncryptToolsDlg(CWnd* pParent = NULL);	// 标准构造函数

// 对话框数据
	enum { IDD = IDD_ENCRYPTTOOLS_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV 支持


// 实现
protected:
	HICON m_hIcon;

	// 生成的消息映射函数
	virtual BOOL OnInitDialog();
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnDropFiles(HDROP hDropInfo);
	CEdit m_path;
	afx_msg void OnBnClickedEncrypt();
	afx_msg void OnBnClickedDecrypt();
	// 检查目录或文件是否合法
	bool CheckPathValid(CString path);
	// 加解密文件或目录
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
