/*!
@author 	winterma
@par		Email: mahuaguan@126.com
@par		Copyright (c) 2014
@date		2014-7-8
@brief		Incredibuild�����������ƽ����
*/

#include <fstream>
#include <stdio.h>
#include <fcntl.h>
#include <io.h>
#include <time.h>
#include <sstream>
#include <afxdisp.h>
#define MAX_PATH 1000
#define RETARTID 129
#define DELAY 5000
using namespace std;


SERVICE_STATUS ServiceStatus;
SERVICE_STATUS_HANDLE hStatus;


string GetIncrediBuildLicKey(DATE date);
void writeToLog(string);
void setKeyValue(string keyValue);
int main()
{
	COleDateTime curTime = COleDateTime::GetCurrentTime();

	/* �Ӻ�ʱ����� */
	COleDateTimeSpan interval;

	/*���Ե�ʱ���Ӻ������Ϊ��ǰ������һ�룬�պò��ᳬ��30��,ע�����Ϊ0�����*/
	/*
	int days = destTime.GetSecond() / 2;
	if (days == 0)
	{
		days = 1;
	}*/

	/* �����汾���Ӻ�20�죬ע������ʮ��Ļ�ÿ�α���ᵯ�������ڽ����ľ��� */
	interval.SetDateTimeSpan( 30, 0, 0, 0);
		
	//if (curTime.GetHour() == 16 && curTime.GetMinute() == 30 && curTime.GetSecond() == 0)
	{
		COleDateTime destTime;
		destTime = curTime + interval;

		COleDateTime dateTime(destTime.GetYear(), destTime.GetMonth(), destTime.GetDay(), destTime.GetHour(), destTime.GetMinute(), destTime.GetSecond());
		DATE Date = (DATE)dateTime;
		string dateStr = GetIncrediBuildLicKey(Date);
		writeToLog("Set value: " + dateStr);
		LPCTSTR data_Set = "Wow6432Node\\Interface\\{C14AEE56-F9E9-41D8-ABF4-9E96BD7D32F9}\\ProxyStubClsid32";
		writeToLog(data_Set);
		setKeyValue(dateStr);
	}
	return 0;
}



string GetIncrediBuildLicKey(DATE date)
{
	/*
	IncrediBuild �������ƺ��㷨
	ע�⣺Incredibuild�жϹ����ǰ��հ�װ����������ʱ�����ľ���ֵ�����м���ģ�������Ҫ���ϸ��°�װʱ������֤������
	��ͬ�汾�������ʱ���ע������λ���ǲ�ͬ��:
	2.40: HKCR\Interface\{E9B0227F-437C-4F7A-86D9-2676B83F359F}\ProxyStubClsid32 = {M1-M2-M3-T1-T2}
	3.20: HKCR\Interface\{B7348B5D-B65D-4BF5-AF63-A3135249ACA7}\ProxyStubClsid32 = {M1-M2-M3-T1-T2}
	3.31: HKCR\Interface\{6F8793A7-3226-4B51-98C1-476A0D1A7AFC}\ProxyStubClsid32
	3.40: HKCR\Interface\{48C6296A-4F4C-4238-A9E1-60E9A544A863}\ProxyStubClsid32 
	�����ʱ Date ��ֵ�� 39598.999988425923 (0x37BA E7FFDF55E340)
	T1 = 37BA
	T2 = E7FFDF55E340
	M1 = 37 * BA * E7 * FF = 23EAEB06
	M2 = DF * 55 = 4A0B
	M3 = E3 * 40 = 38C0
	*/
	BYTE * pByte = (BYTE *)&date;

	BYTE * T1 = (BYTE *) (pByte);
	BYTE * T2 = (BYTE *) (pByte + 2);

	DWORD dwM1 = (DWORD) *pByte;
	dwM1 *= (DWORD) *(pByte + 1);
	dwM1 *= (DWORD) *(pByte + 2);
	dwM1 *= (DWORD) *(pByte + 3);

	WORD wM2 = (WORD) *(pByte + 4);
	wM2 *= (WORD) *(pByte + 5);

	WORD wM3 = (WORD) *(pByte + 6);
	wM3 *= (WORD) *(pByte + 7);

	char ch[1000];
	/************************************************************************/
	/* {%08X-%04X-%04X-%02X%02X-%02X%02X%02X%02X%02X%02X}  ע����ֵ�ĸ�ʽ   */
	/************************************************************************/
	sprintf_s(ch,"{%08X-%04X-%04X-%02X%02X-%02X%02X%02X%02X%02X%02X}", dwM1, wM2, wM3, *T1, *(T1+1), *T2, *(T2+1), *(T2+2), *(T2+3), *(T2+4), *(T2+5));
	string tempStr(ch);
	return tempStr;
}

void setKeyValue(string keyValue)
{
	HKEY hKey;

	/************************************************************************/
	/* data_set��5.0�汾��¼���ΰ�װʱ���ֵ����·����ÿ���汾��λ�ö���һ����Ҫ���� */
	/* ��ֵ��λ��Ҳ���ѣ�ʹ��һ���� Process Monitor��΢���Ʒ���ϲ��Ǿ�Ʒ���������� */
	/* ���õ�^_^�����������Incredibuild�ڰ�װ�����и��ĵ�ע����ֵ������         */
	/* ProxyStubClsid32�����ֱȽ϶����һ�������								*/
	/************************************************************************/
	LPCTSTR data_Set = "Wow6432Node\\Interface\\{C14AEE56-F9E9-41D8-ABF4-9E96BD7D32F9}\\ProxyStubClsid32";
	if (::RegOpenKeyEx(HKEY_CLASSES_ROOT, data_Set, 0, KEY_ALL_ACCESS, &hKey) != ERROR_SUCCESS)
	{
		writeToLog("Open Key Error");
		MessageBox(NULL,"����ʧ�ܣ��������Ҽ�����Ա����","��ʾ",0);
		return;
	}

	TCHAR res[MAX_PATH];//=new BYTE[MAX_PATH];
	DWORD type_1 = REG_SZ;
	DWORD cbData_1 = MAX_PATH;
	/*   ��ȡ����
	if (RegQueryValueEx(hKey, L"", NULL, &type_1, (LPBYTE)res, &cbData_1) != ERROR_SUCCESS)
	{
		cout << "read error" << endl;
		return;
	}
	*/

	LPCTSTR data = keyValue.c_str();


	/* ��ʹ�޸ĳɹ�Ҳ�Ƿ��ش����޷��жϣ����������о��� */
	::RegSetValueEx(hKey, "", 0, REG_SZ, (LPBYTE)data, 100) != ERROR_SUCCESS;

	if (RegQueryValueEx(hKey, "", NULL, &type_1, (LPBYTE)res, &cbData_1) != ERROR_SUCCESS)
	{
		writeToLog("Read value error");
		MessageBox(NULL,"����ʧ�ܣ��������Ҽ�����Ա����","��ʾ",0);
		return;
	}
	
	string tempStr(res);
	writeToLog("Set value: " + tempStr);

	::RegCloseKey(hKey);
	MessageBox(NULL,"���ڳɹ�","��ʾ",0);
	return;
}



void writeToLog(string msg)
{
	/*��ʽ�治дlog*/
	time_t tick;
	struct tm tm;
	char s[100];
	tick = time(NULL);
	tm = *localtime(&tick);
	strftime(s, sizeof(s), "%Y-%m-%d %H:%M:%S", &tm);
	ofstream outfile("D:\\incredibuild_crack_log.txt", ios::app);
	outfile << "[" << s << "]: " << msg.c_str() << endl;
	outfile.close();
	return;
}

string itos(int i)
{
	stringstream s;
	s << i;
	return s.str();
}