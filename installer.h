// stdafx.h : include file for standard system include files,
// or project specific include files that are used frequently, but
// are changed infrequently
//

#pragma once

#include <iostream>
#include <tchar.h>
#include <windows.h>
#include <winternl.h>
#include <shlobj.h>
#include <vector>


typedef struct
{
	CHAR** FileList;
	int length;
} FileList_Struct, *pFileList_Struct;


// TODO: reference additional headers your program requires here
