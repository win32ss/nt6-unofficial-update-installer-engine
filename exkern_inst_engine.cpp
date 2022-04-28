// exkern_inst_engine.cpp : Defines the entry point for the console application.
//

#include "installer.h"

using namespace std;

/*
 A perfect combination of C and C++ features are used in this installer engine designed for replacing system files on x64, x86 and IA-64 Windows NT 6.x.

 To work with the win32 API, I use char array based strings, but combined with the convenience of vector containers.
*/

vector<CHAR*> FileExistingNative;
vector<CHAR*> FileNewNative;
#ifndef _X86_
vector<CHAR*> FileExistingWOW64;
vector<CHAR*> FileNewWOW64;
#endif

CHAR curdir [MAX_PATH];
CHAR windirsys [MAX_PATH];
#ifndef _X86_
CHAR windirsyswow [MAX_PATH];
#endif

void SetupSystemDirs(CHAR* windir)
{
	if(windir == NULL)
    GetWindowsDirectoryA(windirsys, MAX_PATH);
	else
	strcpy_s(windirsys, MAX_PATH, windir);

#ifndef _X86_
    strcpy_s(windirsyswow, MAX_PATH, windirsys);
	strcat_s(windirsyswow, MAX_PATH, "\\SysWOW64\0");
#endif

	strcat_s(windirsys, MAX_PATH, "\\System32\0");	

}

void TakeoverSystemFiles()
/*
  Icacls calls wait for the takeown call to end before executing, due to the possibility of
  a race condition where the icacls call finishes before the takeown call, causing the former to fail.
*/
{
	CHAR TakeownBase [MAX_PATH];
	CHAR IcaclsBase [MAX_PATH];
	CHAR Bcd [MAX_PATH];
	CHAR* IcaclsTemp;
	CHAR* Username;
	CHAR* TakeownTemp;
	DWORD UsernameBuffer = 0;
	DWORD ExitCode;
	STARTUPINFOA sua;
	PROCESS_INFORMATION ProcInfo;

	GetStartupInfoA(&sua);

	GetUserNameA(Username, &UsernameBuffer);
	Username = (CHAR*)malloc(UsernameBuffer);
	GetUserNameA(Username, &UsernameBuffer);

	strcpy_s(TakeownBase, MAX_PATH, windirsys);
	strcpy_s(IcaclsBase, MAX_PATH, windirsys);
	strcat_s(TakeownBase, MAX_PATH, "\\takeown.exe /F ");
	strcat_s(IcaclsBase, MAX_PATH, "\\icacls.exe ");

	for (size_t i = 0; i < FileExistingNative.size(); i++)
	{
		
	TakeownTemp = (CHAR*) malloc(sizeof(CHAR)*MAX_PATH);
	strcpy_s(TakeownTemp, MAX_PATH, TakeownBase);

	strcat_s(TakeownTemp, MAX_PATH, windirsys);
    strcat_s(TakeownTemp, MAX_PATH, "\\");
	strcat_s(TakeownTemp, MAX_PATH, FileExistingNative.at(i));

	IcaclsTemp = (CHAR*) malloc(sizeof(CHAR)*MAX_PATH);

	strcpy_s(IcaclsTemp, MAX_PATH, IcaclsBase);
	strcat_s(IcaclsTemp, MAX_PATH, windirsys);
	strcat_s(IcaclsTemp, MAX_PATH, "\\");
	strcat_s(IcaclsTemp, MAX_PATH, FileExistingNative.at(i));
	strcat_s(IcaclsTemp, MAX_PATH, " /grant ");
	strcat_s(IcaclsTemp, MAX_PATH, Username);
	strcat_s(IcaclsTemp, MAX_PATH, ":F");


	if(!CreateProcessA(NULL, TakeownTemp, NULL, NULL, FALSE, CREATE_PRESERVE_CODE_AUTHZ_LEVEL | CREATE_NO_WINDOW, NULL, NULL,
		&sua, &ProcInfo))
	{
		printf("\nFailed to create process with the following parameters: %s.\nSetup will now exit.\nPress any key to continue.", TakeownTemp);
		getchar();
		free(TakeownTemp);
		free(IcaclsTemp);
		exit(-1);
	}

	while(true)
	{
		GetExitCodeProcess(ProcInfo.hProcess, &ExitCode);
		if(ExitCode != STILL_ACTIVE)
			break;
	}

	CloseHandle(ProcInfo.hProcess);
    CloseHandle(ProcInfo.hThread);

 
	if(!CreateProcessA(NULL, IcaclsTemp, NULL, NULL, FALSE, CREATE_PRESERVE_CODE_AUTHZ_LEVEL | CREATE_NO_WINDOW, NULL, NULL,
		&sua, &ProcInfo))
	{
		printf("\nFailed to create process with the following parameters: %s.\nSetup will now exit.\nPress any key to continue.", IcaclsTemp);
		getchar();
		free(TakeownTemp);
		free(IcaclsTemp);
		exit(-1);
	}

	while(true)
	{
		GetExitCodeProcess(ProcInfo.hProcess, &ExitCode);
		if(ExitCode != STILL_ACTIVE)
			break;
	}

	CloseHandle(ProcInfo.hProcess);
    CloseHandle(ProcInfo.hThread);


	free(TakeownTemp);
	free(IcaclsTemp);

	}
#ifndef _X86_
	for (size_t i = 0; i < FileExistingWOW64.size(); i++)
	{
	TakeownTemp = (CHAR*) malloc(sizeof(CHAR)*MAX_PATH);
	strcpy_s(TakeownTemp, MAX_PATH, TakeownBase);

	strcat_s(TakeownTemp, MAX_PATH, windirsyswow);
    strcat_s(TakeownTemp, MAX_PATH, "\\");
	strcat_s(TakeownTemp, MAX_PATH, FileExistingWOW64.at(i));

	IcaclsTemp = (CHAR*) malloc(sizeof(CHAR)*MAX_PATH);

	strcpy_s(IcaclsTemp, MAX_PATH, IcaclsBase);
	strcat_s(IcaclsTemp, MAX_PATH, windirsyswow);
	strcat_s(IcaclsTemp, MAX_PATH, "\\");
	strcat_s(IcaclsTemp, MAX_PATH, FileExistingWOW64.at(i));
	strcat_s(IcaclsTemp, MAX_PATH, " /grant ");
	strcat_s(IcaclsTemp, MAX_PATH, Username);
	strcat_s(IcaclsTemp, MAX_PATH, ":F");

	if(!CreateProcessA(NULL, TakeownTemp, NULL, NULL, FALSE, CREATE_PRESERVE_CODE_AUTHZ_LEVEL | CREATE_NO_WINDOW, NULL, NULL,
		&sua, &ProcInfo))
	{
		printf("\nFailed to create process with the following parameters: %s.\nSetup will now exit.\nPress any key to continue.", TakeownTemp);
		getchar();
		free(TakeownTemp);
		free(IcaclsTemp);
		exit(-1);
	}

	while(true)
	{
		GetExitCodeProcess(ProcInfo.hProcess, &ExitCode);
		if(ExitCode != STILL_ACTIVE)
			break;
	}

	CloseHandle(ProcInfo.hProcess);
    CloseHandle(ProcInfo.hThread);


	if(!CreateProcessA(NULL, IcaclsTemp, NULL, NULL, FALSE, CREATE_PRESERVE_CODE_AUTHZ_LEVEL | CREATE_NO_WINDOW, NULL, NULL,
		&sua, &ProcInfo))
	{
		printf("\nFailed to create process with the following parameters: %s.\nSetup will now exit.\nPress any key to continue.", IcaclsTemp);
		getchar();
		free(TakeownTemp);
		free(IcaclsTemp);
		exit(-1);
	}

	while(true)
	{
		GetExitCodeProcess(ProcInfo.hProcess, &ExitCode);
		if(ExitCode != STILL_ACTIVE)
			break;
	}

	CloseHandle(ProcInfo.hProcess);
    CloseHandle(ProcInfo.hThread);

	free(TakeownTemp);
	free(IcaclsTemp);
	}
#endif
	free(Username);

	strcpy_s(Bcd, MAX_PATH, windirsys);
// This is done for the benefit of the Vista extended kernel.
// Perhaps in future, a list of "jobs" can be included, with variables representing paths that may be
// part of the command line arguments.
	strcat_s(Bcd, MAX_PATH, "\\bcdedit.exe /set {current} nointegritychecks yes");

	if(!CreateProcessA(NULL, Bcd, NULL, NULL, FALSE, CREATE_PRESERVE_CODE_AUTHZ_LEVEL | CREATE_NO_WINDOW, NULL,
		NULL, &sua, &ProcInfo))
	{
		printf("\nFailed to create process with the following parameters: %s.\nSetup will now exit.\nPress any key to continue.", Bcd);
		getchar();
		exit(-1);
	}

	CloseHandle(ProcInfo.hProcess);
    CloseHandle(ProcInfo.hThread);

	
}

void MoveBackupSystemFiles(BOOL Restore)
/*
  Intended usage is to move paged/in-memory files so new ones can be copied, or to restore the old versions of files
  online after the installer has been run earlier in the session, or to restore backupped files offline.

  That is why you can use the "restore back-up" feature even if the system does not meet the 
  requirements for installation.
*/
{
	CHAR* BackupFile;
	CHAR* BackupFileBak;
	BOOL Success;



	for(size_t i = 0; i < FileExistingNative.size(); i++)
	{
		BackupFile = (CHAR*) malloc(sizeof(CHAR)*MAX_PATH);

		BackupFileBak = (CHAR*) malloc(sizeof(CHAR)*MAX_PATH);

		strcpy_s(BackupFile, MAX_PATH, windirsys);

		strcat_s(BackupFile, MAX_PATH, "\\");

		strcat_s(BackupFile, MAX_PATH, FileExistingNative.at(i));

		strcpy_s(BackupFileBak, MAX_PATH, BackupFile);

		strcat_s(BackupFileBak, MAX_PATH, ".bak");

		if(Restore)
		{
        Success = MoveFileExA(BackupFileBak, BackupFile, MOVEFILE_COPY_ALLOWED | MOVEFILE_REPLACE_EXISTING);
		if(!Success)
			printf("\nERROR: Unable to restore %s to %s", BackupFileBak, BackupFile);
		}
		else
		{
		Success = MoveFileExA(BackupFile, BackupFileBak, MOVEFILE_COPY_ALLOWED | MOVEFILE_REPLACE_EXISTING);
		if(!Success)
			printf("\nERROR: Unable to backup %s to %s", BackupFile, BackupFileBak);
		}
		
		free(BackupFile);

        free(BackupFileBak);
	}
#ifndef _X86_
	for(size_t i = 0; i < FileExistingWOW64.size(); i++)
	{
		BackupFile = (CHAR*) malloc(sizeof(CHAR)*MAX_PATH);

		BackupFileBak = (CHAR*) malloc(sizeof(CHAR)*MAX_PATH);

		strcpy_s(BackupFile, MAX_PATH, windirsyswow);

		strcat_s(BackupFile, MAX_PATH, "\\");

		strcat_s(BackupFile, MAX_PATH, FileExistingWOW64.at(i));

		strcpy_s(BackupFileBak, MAX_PATH, BackupFile);

		strcat_s(BackupFileBak, MAX_PATH, ".bak");

		if(Restore)
		{
        Success = MoveFileExA(BackupFileBak, BackupFile, MOVEFILE_COPY_ALLOWED | MOVEFILE_REPLACE_EXISTING);
		if(!Success)
			printf("\nERROR: Unable to restore %s to %s", BackupFileBak, BackupFile);
		}
		else
		{
		Success = MoveFileExA(BackupFile, BackupFileBak, MOVEFILE_COPY_ALLOWED | MOVEFILE_REPLACE_EXISTING);
		if(!Success)
			printf("\nERROR: Unable to backup %s to %s", BackupFile, BackupFileBak);
		}
		free(BackupFile);

        free(BackupFileBak);
	}
#endif
	   for(size_t i = 0; i < FileNewNative.size(); i++)
	{
		BackupFile = (CHAR*) malloc(sizeof(CHAR)*MAX_PATH);

		BackupFileBak = (CHAR*) malloc(sizeof(CHAR)*MAX_PATH);

		strcpy_s(BackupFile, MAX_PATH, windirsys);

		strcat_s(BackupFile, MAX_PATH, "\\");

		strcat_s(BackupFile, MAX_PATH, FileNewNative.at(i));

		strcpy_s(BackupFileBak, MAX_PATH, BackupFile);

		strcat_s(BackupFileBak, MAX_PATH, ".bak");

		if(Restore)
		{
        Success = MoveFileExA(BackupFileBak, BackupFile, MOVEFILE_COPY_ALLOWED | MOVEFILE_REPLACE_EXISTING);
		if(!Success)
			printf("\nERROR: Unable to restore %s to %s", BackupFileBak, BackupFile);
		}
		else
		{
		Success = MoveFileExA(BackupFile, BackupFileBak, MOVEFILE_COPY_ALLOWED | MOVEFILE_REPLACE_EXISTING);
		if(!Success)
			printf("\nERROR: Unable to backup %s to %s", BackupFile, BackupFileBak);
		}

		free(BackupFile);

        free(BackupFileBak);
	}
#ifndef _X86_
	for(size_t i = 0; i < FileNewWOW64.size(); i++)
	{
		BackupFile = (CHAR*) malloc(sizeof(CHAR)*MAX_PATH);

		BackupFileBak = (CHAR*) malloc(sizeof(CHAR)*MAX_PATH);

		strcpy_s(BackupFile, MAX_PATH, windirsyswow);

		strcat_s(BackupFile, MAX_PATH, "\\");

		strcat_s(BackupFile, MAX_PATH, FileNewWOW64.at(i));

		strcpy_s(BackupFileBak, MAX_PATH, BackupFile);

		strcat_s(BackupFileBak, MAX_PATH, ".bak");

		if(Restore)
		{
        Success = MoveFileExA(BackupFileBak, BackupFile, MOVEFILE_COPY_ALLOWED | MOVEFILE_REPLACE_EXISTING);
		if(!Success)
			printf("\nERROR: Unable to restore %s to %s", BackupFileBak, BackupFile);
		}
		else
		{
		Success = MoveFileExA(BackupFile, BackupFileBak, MOVEFILE_COPY_ALLOWED | MOVEFILE_REPLACE_EXISTING);
		if(!Success)
			printf("\nERROR: Unable to backup %s to %s", BackupFile, BackupFileBak);
		}

		free(BackupFile);

        free(BackupFileBak);
	}
#endif	
}

void CopyFiles()
{
	CHAR* NewFilePath;
	CHAR* SourcePath;

	for (size_t i = 0; i < FileExistingNative.size(); i++)
	{
		NewFilePath = (CHAR*) malloc(sizeof(CHAR)*MAX_PATH);
		SourcePath = (CHAR*) malloc(sizeof(CHAR)*MAX_PATH);

		strcpy_s(NewFilePath, MAX_PATH, windirsys);
		strcat_s(NewFilePath, MAX_PATH, "\\");
		strcat_s(NewFilePath, MAX_PATH, FileExistingNative.at(i));
		strcpy_s(SourcePath, MAX_PATH, curdir);
		strcat_s(SourcePath, MAX_PATH, "\\");
		strcat_s(SourcePath, MAX_PATH, FileExistingNative.at(i));

		if(!CopyFileA(SourcePath, NewFilePath, FALSE))
		{
			free(NewFilePath);
			free(SourcePath);
			printf("\n File %s is missing from setup folder or could not be copied. Rolling back changes.", 
				FileExistingNative.at(i));
			MoveBackupSystemFiles(TRUE);
			return;
		}
        free(SourcePath);
		free(NewFilePath);
	}
#ifndef _X86_
	for (size_t i = 0; i < FileExistingWOW64.size(); i++)
	{
		NewFilePath = (CHAR*) malloc(sizeof(CHAR)*MAX_PATH);
        SourcePath = (CHAR*) malloc(sizeof(CHAR)*MAX_PATH);

		strcpy_s(NewFilePath, MAX_PATH, windirsyswow);
		strcat_s(NewFilePath, MAX_PATH, "\\");
		strcat_s(NewFilePath, MAX_PATH, FileExistingWOW64.at(i));
		strcpy_s(SourcePath, MAX_PATH, curdir);
		strcat_s(SourcePath, MAX_PATH, "\\");
		strcpy_s(SourcePath, MAX_PATH, FileExistingWOW64.at(i));
		strcat_s(SourcePath, MAX_PATH, ".wow64");

		if(!CopyFileA(SourcePath, NewFilePath, FALSE))
		{
			free(NewFilePath);
			printf("\n File %s is missing from setup folder or could not be copied. Rolling back changes.",
				    SourcePath);
			free(SourcePath);
			MoveBackupSystemFiles(TRUE);
			return;
		}

		free(NewFilePath);
		free(SourcePath);
	}
#endif
	for (size_t i = 0; i < FileNewNative.size(); i++)
	{
		NewFilePath = (CHAR*) malloc(sizeof(CHAR)*MAX_PATH);
		SourcePath = (CHAR*) malloc(sizeof(CHAR)*MAX_PATH);

		strcpy_s(NewFilePath, MAX_PATH, windirsys);
		strcat_s(NewFilePath, MAX_PATH, "\\");
		strcat_s(NewFilePath, MAX_PATH, FileNewNative.at(i));
		strcpy_s(SourcePath, MAX_PATH, curdir);
		strcat_s(SourcePath, MAX_PATH, "\\");
		strcat_s(SourcePath, MAX_PATH, FileNewNative.at(i));

		if(!CopyFileA(SourcePath, NewFilePath, FALSE))
		{
			free(NewFilePath);
			printf("\n File %s is missing from setup folder or could not be copied. Rolling back changes.",
				    FileNewNative.at(i));
			free(SourcePath);
			MoveBackupSystemFiles(TRUE);
			return;
		}

		free(NewFilePath);
		free(SourcePath);
	}
#ifndef _X86_
	for (size_t i = 0; i < FileNewWOW64.size(); i++)
	{
		NewFilePath = (CHAR*) malloc(sizeof(CHAR)*MAX_PATH);
        SourcePath = (CHAR*) malloc(sizeof(CHAR)*MAX_PATH);

		strcpy_s(NewFilePath, MAX_PATH, windirsyswow);
		strcat_s(NewFilePath, MAX_PATH, "\\");
		strcat_s(NewFilePath, MAX_PATH, FileNewWOW64.at(i));
		strcpy_s(SourcePath, MAX_PATH, curdir);
		strcat_s(SourcePath, MAX_PATH, "\\");
		strcpy_s(SourcePath, MAX_PATH, FileNewWOW64.at(i));
		strcat_s(SourcePath, MAX_PATH, ".wow64");

		if(!CopyFileA(SourcePath, NewFilePath, FALSE))
		{
			free(NewFilePath);
			printf("\n File %s is missing from setup folder or could not be copied. Rolling back changes.",
				    SourcePath);
			free(SourcePath);
			MoveBackupSystemFiles(TRUE);
			return;
		}

		free(NewFilePath);
		free(SourcePath);
	}
#endif
}

void InitFileLists()
/*
  Filenames are read from a configuration file named setup.conf. There are two separate categories of files:
  "existing files" and "new files". Existing files are files that are part of the target version of Windows 
  and require file ownership to be given to the administrator. New files are novel to the version of Windows
  and do not require the same file ownership and permission tasks to be performed.

  The (FileExisting/FileNew)Native sections are available on every platform, while only x64 and IA64 installers
  make use of the *WOW64 analogs. All files are assumed to be copied to System32 or SysWOW64, as is presently 
  demanded by the projects that use this installer.
*/
{
	FILE* configfile;
	CHAR Path [MAX_PATH];
	int List = 0;
    int fopenerr = fopen_s(&configfile, "setup.conf", "r");

	if(fopenerr)
	{
		printf("\nUnable to load or locate configuration file. Setup will exit.");
		getchar();
		getchar();
		exit(-1);
	}


	
	while(!feof(configfile))
	{
		
		fscanf_s(configfile, "%s ", Path, MAX_PATH);
		if(strstr(Path, "[FileExistingNative]"))
			List = 1;
		else if(strstr(Path, "[FileNewNative]"))
            List = 2;
#ifndef _X86_
		else if(strstr(Path, "[FileExistingWOW64]"))
			List = 3;
		else if(strstr(Path, "[FileNewWOW64]"))
            List = 4;
#endif
		else
		{
		if(List == 1)
		{
		CHAR* Pos = (CHAR*)malloc(sizeof(CHAR)*MAX_PATH);
		strcpy_s(Pos, MAX_PATH, Path);
		strcat_s(Pos, MAX_PATH, "\0");
		FileExistingNative.push_back(Pos);
		}
		if(List == 2)
		{
		CHAR* Pos = (CHAR*)malloc(sizeof(CHAR)*MAX_PATH);
		strcpy_s(Pos, MAX_PATH, Path);
		FileNewNative.push_back(Pos);
		}
#ifndef _X86_
		if(List == 3)
		{
		CHAR* Pos = (CHAR*)malloc(sizeof(CHAR)*MAX_PATH);
		strcpy_s(Pos, MAX_PATH, Path);
		FileExistingWOW64.push_back(Pos);
		}
		if(List == 4)
		{
		CHAR* Pos = (CHAR*)malloc(sizeof(CHAR)*MAX_PATH);
		strcpy_s(Pos, MAX_PATH, Path);
		FileNewWOW64.push_back(Pos);
		}
#endif
		}
	}

	fclose(configfile);
/* A listing of the contents in each file section can be printed if the DEBUG flag
   is used at compile time.
   It is possible that the same data could be printed to a file using a verbose command line switch, or
   printed to the command line itself also using a verbose command line switch, if this feature is desired
   by users.
*/
#ifdef DEBUG

	cout << "File debug listings: " << endl;

	for(int i = 0; i < FileExistingNative.size(); i++)
		cout << FileExistingNative.at(i) << "\n";
   cout << "File NewX64: " << endl;
	for(int i = 0; i < FileNewNative.size(); i++)
		cout << FileNewNative.at(i) << "\n";
#ifndef _X86_
cout << "File ExistingWOW64: " << endl;
    for(int i = 0; i < FileExistingWOW64.size(); i++)
	cout << FileExistingWOW64.at(i) << "\n";
cout << "File NewWOW64: " << endl;
	for(int i = 0; i < FileNewWOW64.size(); i++)
    cout << FileNewWOW64.at(i) << "\n";
#endif

#endif
}



int _tmain(int argc, _TCHAR* argv[])
/*
   Name of the product to be installed is gathered from the configuration file, as is the required maximum/minimum
   OS build numbers, if necessary. Build numbers are obtained directly from the PEB to best even the most
   powerful version spoofers, that is until you load the installer via CreateProcess in a suspended state and
   manually modify its PEB before resuming the starting thread.

   To emphasize the installer's ability to restore failed installs in situations where the target operating system
   install is unbootable, other systems that may not be able to install the product can be used to restore another
   system that is capable of using the product.
*/
{
	CHAR ProductName [128];
	CHAR CurrentConfigPath [MAX_PATH];
	CHAR windir [MAX_PATH];
	char s;
	DWORD BuildNumber;
	DWORD MinBuildNumber;
	DWORD MaxBuildNumber;
	CHAR Test [MAX_PATH];
	CHAR winsystemp [MAX_PATH];
	HANDLE Check;
	GetCurrentDirectoryA(MAX_PATH, curdir);
	GetSystemDirectoryA(winsystemp, MAX_PATH);
	strcpy_s(CurrentConfigPath, MAX_PATH, curdir);
	strcat_s(CurrentConfigPath, MAX_PATH, "\\");
	strcat_s(CurrentConfigPath, MAX_PATH, "setup.conf");
    
   #ifndef _X86_
	BuildNumber = (DWORD) NtCurrentTeb()->ProcessEnvironmentBlock->Reserved9[23];
   #else
    BuildNumber = (DWORD) NtCurrentTeb()->ProcessEnvironmentBlock->Reserved9[29];
   #endif
	BuildNumber = BuildNumber << 16;
	BuildNumber = BuildNumber >> 16;
	MinBuildNumber = GetPrivateProfileIntA("Config", "MinBuildNumber", 0, CurrentConfigPath);
	MaxBuildNumber = GetPrivateProfileIntA("Config", "MaxBuildNumber", (1 << 30) - 1 + (1 << 30), CurrentConfigPath);

	GetPrivateProfileStringA("Config", "ProductName", "Product Name Not Defined", ProductName, sizeof(ProductName),
		                     CurrentConfigPath);
   /*
     Conventional methods of detecting a lack of process elevation were not working well here,
	 such as GetPrivilegeName() consistently providing me with an error code related to asynchronous I/O issues.
	 So we have to create a small file in system32 then delete it immediately. If it cannot create that file, then
	 the privileges are insufficient to run the installer.
   */
	
	strcpy_s(Test, MAX_PATH, winsystemp);
	strcat_s(Test, MAX_PATH, "\\");
	strcat_s(Test, MAX_PATH, "test");
	DeleteFileA(Test);
	SetLastError(0);
	Check = CreateFileA(Test, GENERIC_WRITE, 0, NULL, OPEN_ALWAYS, FILE_ATTRIBUTE_SYSTEM, NULL);
    if(GetLastError() == 5)
	{
		printf("\nInsufficient permissions to run %s setup. Please right-click \"run as administrator\" and try again.",
			ProductName);
		getchar();
		getchar();
		return 0;
	}
	else
	{
		CloseHandle(Check);
		DeleteFileA(Test);
	}

   #ifdef _X86_
	BOOL Wow64Process;
	if(IsWow64Process(GetCurrentProcess(), &Wow64Process))
	{
		printf("The %s setup package is intended for an x86 version of Windows.", ProductName);
		printf("\nOffline backup restoration is still possible. To restore a backup, press 'R'.\n");
		scanf_s("%c", &s, 1);
		if(s == 'r' || s == 'R')
			goto Restore;
		return 0;
	}
   #endif


    if(BuildNumber < MinBuildNumber || BuildNumber > MaxBuildNumber)
	{
		printf("An OS build between %d and %d is required to run %s setup.", MinBuildNumber, MaxBuildNumber, ProductName);
		printf("\nOffline backup restoration is still possible. To restore a backup, press 'R'.\n");
		scanf_s("%c", &s, 1);
		if(s == 'r' || s == 'R')
			goto Restore;
		return 0;
	}


	printf("This is %s setup. \nPress 'R' for restore. \nPress all other keys for setup.\n", 
		   ProductName);
	scanf_s("%c", &s, 1);
	
	InitFileLists();
	if(s == 'r' || s == 'R')
	{
        Restore:
		printf("Provide the path of the Windows directory that you want to restore.\n");
		scanf_s("%s", windir, MAX_PATH);
		SetupSystemDirs(windir);
		MoveBackupSystemFiles(TRUE);
		printf("\nRestoration of backed-up files is complete\nPress any key to continue.");
		getchar();
		getchar();
		return 0;
	}
	SetupSystemDirs(NULL);
	TakeoverSystemFiles();
	MoveBackupSystemFiles(FALSE);
	CopyFiles();
	printf("\n%s setup is complete.\nPress any key to continue.", ProductName);
	getchar();
    getchar();

	return 0;
}