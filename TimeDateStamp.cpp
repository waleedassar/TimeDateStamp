#include "stdafx.h"
#include "windows.h"
#include "time.h"
#include "stdio.h"



#define MAX_RESOURCE_TYPE_STRING_LENGTH 100
#define MAX_RESOURCE_LANG_STRING_LENGTH 50

struct WSTRINGW
{
	unsigned short Length;
	wchar_t Buffer[2]; 
};


struct CV_HEADER
{
  unsigned long Signature;
  unsigned long Offset;
};

typedef struct _CV_INFO_PDB20
{
  CV_HEADER CvHeader;
  unsigned long TimeDateStamp;
  unsigned long Age;
  char PdbFileName[MAX_PATH];
}CV_INFO_PDB20;


#define INVALID_SET_FILE_POINTER 0xFFFFFFFF

typedef BOOL(__stdcall *lpIsWow64Process)(HANDLE,BOOL*);
typedef BOOL(__stdcall *lpWow64DisableWow64FsRedirection)(BOOL*);

lpIsWow64Process IsWow64Process;

lpWow64DisableWow64FsRedirection Wow64DisableWow64FsRedirection;

bool ResolveIsWow64Process()
{
	HMODULE hKern32 = GetModuleHandle("kernel32.dll");
	if(!hKern32) return false;
	IsWow64Process = (lpIsWow64Process)GetProcAddress(hKern32,"IsWow64Process");
	if(! IsWow64Process) return false;
	return true;
}

bool ResolveWow64DisableWow64FsRedirection()
{
	HMODULE hKern32 = GetModuleHandle("kernel32.dll");
	if(!hKern32) return false;
	Wow64DisableWow64FsRedirection = (lpWow64DisableWow64FsRedirection)GetProcAddress(hKern32,"Wow64DisableWow64FsRedirection");
	if(!Wow64DisableWow64FsRedirection) return false;
	return true;
}

unsigned char* DecorateTime(unsigned char* pTimeStr)
{
	if(!pTimeStr) return 0;
	unsigned long length = strlen((char*)pTimeStr);
	if(length==0) return 0;
	if( ( pTimeStr[length-1] == 0x0D ) || ( pTimeStr[length-1]==0x0A ) ) pTimeStr[length-1]=0;
	return pTimeStr;
}


//Input is a string
char* PrintTimeDateStamp(char* argStr)
{
	    unsigned long Length = strlen(argStr);
	    if(Length > 10)
		{
		   printf("Invalid input\r\n");
		   return 0;
		}
		unsigned long Value = strtoul(argStr,0,0x10);
		time_t TimeX = (time_t) Value;
		
		tm* pGMT = gmtime(&TimeX);//remove this if incompatible

		
		//char* pTime = (char*)DecorateTime((unsigned char*)ctime(&TimeX));
		char* pTime = (char*)DecorateTime((unsigned char*)asctime(pGMT));
		if(!pTime)
		{
			pTime = "(Invalid or Spoofed TimeDateStamp)";
			printf("%s %X\r\n",pTime,Value);
		}
		else   printf("%s \r\n",pTime);
		return pTime;
}

char* PrintTimeDateStamp(unsigned long vTimeDateStamp)
{
	    if(!vTimeDateStamp)
		{
			printf("TimeDateStamp: Empty\r\n");
			return 0;
		}

		time_t TimeX = (time_t) vTimeDateStamp;
		tm* pGMT = gmtime(&TimeX); //remove this if incompatible

		//char* pTime = (char*)DecorateTime((unsigned char*)ctime(&TimeX));
		char* pTime = (char*)DecorateTime((unsigned char*)asctime(pGMT));
		
		if(!pTime)
		{
			pTime = "(Invalid or Spoofed TimeDateStamp)";
			printf("%s %X\r\n",pTime,vTimeDateStamp);
		}
		else	printf("%s\r\n",pTime);
		return pTime;
}

#define RT_MANIFEST 24
char* GetBasicResourceType(unsigned long Type)
{
	char* pType = 0;
	if(Type == 1) pType = "RT_CURSOR";   //1
	else if(Type == 2) pType = "RT_BITMAP"; //2
	else if(Type == 3) pType = "RT_ICON"; //3
	else if(Type == 4) pType = "RT_MENU"; //4
	else if(Type == 5) pType = "RT_DIALOG"; //5
	else if(Type == 6) pType = "RT_STRING"; //6
	else if(Type == 7) pType = "RT_FONTDIR"; //7
	else if(Type == 8) pType = "RT_FONT"; //8
	else if(Type == 9) pType = "RT_ACCELERATOR"; //9
	else if(Type == 10) pType = "RT_RCDATA"; //10
	else if(Type == 11) pType = "RT_MESSAGETABLE"; //11
	else if(Type == 12) pType = "RT_GROUP_CURSOR"; //12

	else if(Type == 14) pType = "RT_GROUP_ICON"; //14

	else if(Type == 16) pType = "RT_VERSION"; //16
	else if(Type == 17) pType="RT_DLGINCLUDE"; //17

	else if(Type == 19) pType="RT_PLUGPLAY"; //19
	else if(Type == 20) pType = "RT_VXD"; //20
	else if(Type == 21) pType = "RT_ANICURSOR"; //21
	else if(Type == 22) pType = "RT_ANIICON"; //22
	else if(Type == 23) pType = "RT_HTML"; //23
	else if(Type == 24) pType = "RT_MANIFEST"; //24
	else pType = 0;
	return pType;
}

char* GetTimeDateStamp_s(unsigned long vTimeDateStamp)
{
	    if(!vTimeDateStamp)
		{
			printf("TimeDateStamp: Empty\r\n");
			return 0;
		}

		time_t TimeX = (time_t) vTimeDateStamp;

		tm* pGMT = gmtime(&TimeX);//remove this if incompatible

		//char* pTime = (char*)DecorateTime((unsigned char*)ctime(&TimeX));
		char* pTime = (char*)DecorateTime((unsigned char*)asctime(pGMT));

		if(!pTime)
		{
			pTime = "(Invalid or Spoofed TimeDateStamp)";
		}
		return pTime;
}

bool IsValidHexValue(unsigned char* pHex)
{
	if(!pHex) return false;
	unsigned long Length = strlen((char*)pHex);
	if(!Length) return false;
	if(Length > 10) return false;

	bool x_exists = false;
	if(pHex[1]=='x' || pHex[1]=='X') x_exists = true;
	if(x_exists)  { if(pHex[0]!='0') { return false; } }

	unsigned char* pHexx= pHex;
	if(x_exists)	pHexx = pHex+2;
	Length = strlen((char*)pHexx);
	for(unsigned long i=0;i<Length;i++)
	{
		if( pHexx[i] != '0' &&
			pHexx[i] != '1' &&
			pHexx[i] != '2' &&
			pHexx[i] != '3' &&
			pHexx[i] != '4' &&
			pHexx[i] != '5' &&
			pHexx[i] != '6' &&
			pHexx[i] != '7' &&
			pHexx[i] != '8' &&
			pHexx[i] != '9' &&
			pHexx[i] != 'A' &&
			pHexx[i] != 'B' &&
			pHexx[i] != 'C' &&
			pHexx[i] != 'D' &&
			pHexx[i] != 'E' &&
			pHexx[i] != 'F' &&
			pHexx[i] != 'a' &&
			pHexx[i] != 'b' &&
			pHexx[i] != 'c' &&
			pHexx[i] != 'd' &&
			pHexx[i] != 'e' &&
			pHexx[i] != 'f' ) return false;

	}
	return true;
}


void CloseHandles(HANDLE hMapping,HANDLE hFile)
{
	if(hMapping) CloseHandle(hMapping);
	if(hFile != INVALID_HANDLE_VALUE)  CloseHandle(hFile);
}

void UnmapInputFileAndCloseHandles(void* pMap,HANDLE hMapping,HANDLE hFile)
{
	if(pMap) UnmapViewOfFile(pMap);
	CloseHandles(hMapping,hFile);
}

unsigned char* MapInputFile(char* pFileName,HANDLE* phFile,HANDLE* phMapping)
{
	HANDLE hFile=CreateFile(pFileName,GENERIC_READ,FILE_SHARE_READ,0,OPEN_EXISTING,0,0);
	if(hFile == INVALID_HANDLE_VALUE)
	{
		//printf("Can't Open Input File Error: %#x\r\n",GetLastError());
		return 0;
	}
	else { if(phFile) { *phFile = hFile; } }

	unsigned long FileSizeHigh = 0;
	unsigned long InputFileSize = GetFileSize(hFile,&FileSizeHigh);
	if(FileSizeHigh)
	{
		//printf("Input File Size Is Too Huge Try Splitting It\r\n");
		CloseHandle(hFile);
		return 0;
	}
	else
	{
		if(InputFileSize == 0)
		{
			//printf("Input File Is Empty\r\n");
			CloseHandle(hFile);
			return 0;
		}
	}
	HANDLE hMapping=CreateFileMapping(hFile,0,PAGE_READONLY|SEC_IMAGE,0,0,0);
	if(hMapping == 0)
	{
		//printf("Can't Create A Memory Mapping Object For Input File Error: %#x\r\n",GetLastError());
		CloseHandle(hFile);
		return 0;
	}
	else { if(phMapping) { *phMapping = hMapping; } }

	unsigned char* pMapX = (unsigned char*)MapViewOfFile(hMapping,FILE_MAP_READ,0,0,0);
	if(pMapX == 0)
	{
		//printf("Can't Map Input File Into Memory: %#x\r\n",GetLastError());
		CloseHandles(hMapping,hFile);
		return 0;
	}
	return pMapX;
}

void PrintDebugType(unsigned long Type)
{
	if(Type==0)         printf("%s","UNKNOWN");
    else if(Type==1)    printf("%s","COFF");
    else if(Type==2)    printf("%s","CODEVIEW");
    else if(Type==3)    printf("%s","FPO");
    else if(Type==4)    printf("%s","MISC");
    else if(Type==5)    printf("%s","EXCEPTION");
    else if(Type==6)    printf("%s","FIXUP");
    else if(Type==7)    printf("%s","OMAP_TO_SRC");
    else if(Type==8)    printf("%s","OMAP_FROM_SRC");
    else if(Type==9)    printf("%s","BORLAND");
	else                printf("%s","RESERVED");
	return;
}

//For printing resource directories
void PrintWithLevel(unsigned long Level,unsigned char* pString)
{
	unsigned char Buffer[102]={0};
	Buffer[0]='>';

	for(unsigned long i=0;Level<100,i<=Level;i++)
	{
		Buffer[i+1]='>';
	}

	printf("%s %s\r\n",Buffer,pString);

	return;
}

void Parse_Resources_TimeDateStamp(_IMAGE_RESOURCE_DIRECTORY* pResource,unsigned long Offset,_IMAGE_RESOURCE_DIRECTORY_ENTRY* pParent,unsigned long StartingLevel,unsigned long NumberOfLevels)
{
	if(!pResource) return;
	if(StartingLevel >= NumberOfLevels) return;

	unsigned long TimeDateStamp = 0;
	unsigned char Buffer[300]={0};


	if(pResource)
	{
		_IMAGE_RESOURCE_DIRECTORY* pResourceX = (_IMAGE_RESOURCE_DIRECTORY*)(((unsigned char*)pResource)+Offset);
		TimeDateStamp = pResourceX->TimeDateStamp;

		if(!StartingLevel)
		{
			sprintf((char*)Buffer,"TimeDateStamp from root _IMAGE_RESOURCE_DIRECTORY is %s\r\n",GetTimeDateStamp_s(TimeDateStamp));
		    PrintWithLevel(StartingLevel,Buffer);
		}
		else if(StartingLevel==1)
		{
			if(pParent->NameIsString)
			{
				WSTRINGW* pResourceType = (WSTRINGW*)(((unsigned char*)(pResource))+pParent->NameOffset);
				unsigned long Length = pResourceType->Length;
				unsigned long RealLength = wcslen(pResourceType->Buffer);

				if(Length > RealLength) Length = RealLength;
				if(Length > MAX_RESOURCE_TYPE_STRING_LENGTH) Length=MAX_RESOURCE_TYPE_STRING_LENGTH;

				char cBuffer[MAX_RESOURCE_TYPE_STRING_LENGTH+1]={0};
				WideCharToMultiByte(0,0,pResourceType->Buffer,-1,cBuffer,Length,0,0);

				sprintf((char*)Buffer,"TimeDateStamp from _IMAGE_RESOURCE_DIRECTORY of type %s is %s\r\n",cBuffer,GetTimeDateStamp_s(TimeDateStamp));
		        PrintWithLevel(StartingLevel,Buffer);
			}
			else
			{
				char* pType = GetBasicResourceType(pParent->Name);
				if(pType)				sprintf((char*)Buffer,"TimeDateStamp from _IMAGE_RESOURCE_DIRECTORY of type %s is %s\r\n",pType,GetTimeDateStamp_s(TimeDateStamp));
				else                    sprintf((char*)Buffer,"TimeDateStamp from _IMAGE_RESOURCE_DIRECTORY of type %#x is %s\r\n",pParent->Name,GetTimeDateStamp_s(TimeDateStamp));
		        PrintWithLevel(StartingLevel,Buffer);
			}
		}
		else if(StartingLevel==2)
		{
			if(pParent->NameIsString)
			{
				WSTRINGW* pResourceLang = (WSTRINGW*)(((unsigned char*)(pResource))+pParent->NameOffset);
				unsigned long Length = pResourceLang->Length;
				unsigned long RealLength = wcslen(pResourceLang->Buffer);

				if(Length > RealLength) Length = RealLength;
				if(Length > MAX_RESOURCE_LANG_STRING_LENGTH) Length=MAX_RESOURCE_LANG_STRING_LENGTH;

				char cBuffer[MAX_RESOURCE_LANG_STRING_LENGTH+1]={0};
				WideCharToMultiByte(0,0,pResourceLang->Buffer,-1,cBuffer,Length,0,0);

				sprintf((char*)Buffer,"TimeDateStamp from _IMAGE_RESOURCE_DIRECTORY of identifier %s is %s\r\n",cBuffer,GetTimeDateStamp_s(TimeDateStamp));
		        PrintWithLevel(StartingLevel,Buffer);
			}
			else
			{
				 sprintf((char*)Buffer,"TimeDateStamp from _IMAGE_RESOURCE_DIRECTORY of identifier %#x is %s\r\n",pParent->Name,GetTimeDateStamp_s(TimeDateStamp));
		         PrintWithLevel(StartingLevel,Buffer);
			}

		}
		//---------------------------------------------------------------------
		unsigned long NumberOfEntries = pResourceX->NumberOfNamedEntries+pResourceX->NumberOfIdEntries;
		_IMAGE_RESOURCE_DIRECTORY_ENTRY* pResourceEntryX = (_IMAGE_RESOURCE_DIRECTORY_ENTRY*)(((unsigned char*)pResourceX)+sizeof(_IMAGE_RESOURCE_DIRECTORY));
		for(unsigned long i=0;i<NumberOfEntries;i++)
		{
			if(pResourceEntryX[i].DataIsDirectory)
			{
				Parse_Resources_TimeDateStamp(pResource,pResourceEntryX[i].OffsetToDirectory,&pResourceEntryX[i],StartingLevel+1,NumberOfLevels);
			}
		}
	}
}

void main(int argc, char* argv[])

{
	if(argc<2)
	{
		printf("Usage:\r\nTimeDateStamp.exe 0xCCAACCAA\r\nTimeDateStamp.exe CCAACCAA\r\nTimeDateStamp.exe -f c:\\file.exe\r\n");
		return;
	}

	bool bHex = true;
	if(strcmpi(argv[1],"-f")==0) bHex = false;

	if(bHex)
	{
		if(!IsValidHexValue((unsigned char*)(argv[1])))
		{
			printf("Invalid Hex Input\r\n");
			return;
		}
		PrintTimeDateStamp(argv[1]);
	}
	else
	{
		//--------------- FS Redirector Stuff ----------------------------
		if(ResolveIsWow64Process())
		{
			BOOL bResult = FALSE;
			BOOL B = IsWow64Process(GetCurrentProcess(),& bResult);
			if(B && bResult)
			{
				if(ResolveWow64DisableWow64FsRedirection())
				{
					BOOL bOld;
					Wow64DisableWow64FsRedirection(&bOld);
				}
			}
		}
		//---------------- Start Reading from Input File -----------------
		HANDLE hFile = 0;
		HANDLE hMapping = 0;
		unsigned char* pMap = MapInputFile(argv[2],&hFile,&hMapping);
		if(!pMap)
		{
			printf("Can't read TimeDateStamp from input file. Error: %x\r\n",GetLastError());
			return;
		}

		if(GetFileSize(hFile,0)==0)
		{
			UnmapInputFileAndCloseHandles(pMap,hMapping,hFile);
			return;
		}

		IMAGE_DOS_HEADER DosHdr={0};
		IMAGE_NT_HEADERS NtHdr={0};



		if( ((IMAGE_DOS_HEADER*)pMap)->e_magic != 0x5A4D)
		{
			printf("Error: Not a PE\r\n");
			UnmapInputFileAndCloseHandles(pMap,hMapping,hFile);
			return;
		}

		if( ((IMAGE_DOS_HEADER*)pMap)->e_lfanew > 0x10000000)
		{
			printf("Error: Not a valid PE\r\n");
			UnmapInputFileAndCloseHandles(pMap,hMapping,hFile);
			return;
		}
		//--------------------- TimeDateStamp From IMAGE_FILE_HEADER----------------------------
		IMAGE_NT_HEADERS* pNtHdrs = (IMAGE_NT_HEADERS*)(pMap+ ((IMAGE_DOS_HEADER*)pMap)->e_lfanew );
		printf(">> TimeDateStamp from _IMAGE_FILE_HEADER        ---> ");
		PrintTimeDateStamp(pNtHdrs->FileHeader.TimeDateStamp);
		//----------------------------------------------------------
		unsigned long size_OptionalHeader = pNtHdrs->FileHeader.SizeOfOptionalHeader;
		unsigned long NumberOfSections = pNtHdrs->FileHeader.NumberOfSections;
		IMAGE_SECTION_HEADER* pSects = (IMAGE_SECTION_HEADER*) (  (  (unsigned char*)pNtHdrs  ) + size_OptionalHeader);

		unsigned long rva_Export =0;
		unsigned long rva_Resource = 0;
		unsigned long rva_Debug = 0;
		unsigned long sz_Debug = 0;
		unsigned long rva_LoadConfig = 0;

		if( 0 < (pNtHdrs->OptionalHeader.NumberOfRvaAndSizes) )    rva_Export = pNtHdrs->OptionalHeader.DataDirectory[0].VirtualAddress;
		if( 2 < (pNtHdrs->OptionalHeader.NumberOfRvaAndSizes) )    rva_Resource = pNtHdrs->OptionalHeader.DataDirectory[2].VirtualAddress;
		if( 6 < (pNtHdrs->OptionalHeader.NumberOfRvaAndSizes) ) 
		{
			rva_Debug = pNtHdrs->OptionalHeader.DataDirectory[6].VirtualAddress;
			sz_Debug =  pNtHdrs->OptionalHeader.DataDirectory[6].Size;
		}
		if( 10 < (pNtHdrs->OptionalHeader.NumberOfRvaAndSizes) )   rva_LoadConfig = pNtHdrs->OptionalHeader.DataDirectory[10].VirtualAddress;
		//------------- Trying With Export Directory ------------------------------
		printf(">> TimeDateStamp from _IMAGE_EXPORT_DIRECTORY   ---> ");
		if(rva_Export)
		{
			IMAGE_EXPORT_DIRECTORY* pExport = (IMAGE_EXPORT_DIRECTORY*) (pMap + rva_Export);
			PrintTimeDateStamp(pExport->TimeDateStamp);
		}
		else printf("Empty\r\n");
		//-------------- Trying With Resource Directory ----------------------------
		if(rva_Resource) //sz_Resource is not consulted by PE Loader
		{
			//Number of levels is 3 for typical linkers. For non-typical linkers, we should ignore next levels since they won't have TimeDateStamp.
			unsigned long NumberOfLevels = 3; //Default is ResourceType + Resource Id + Resource Language
			Parse_Resources_TimeDateStamp((_IMAGE_RESOURCE_DIRECTORY*)(pMap+rva_Resource),0,0,0,NumberOfLevels);
		}
		//-------------- Trying With Debug Directory -------------------------------
		printf(">> TimeDateStamp from _IMAGE_DEBUG_DIRECTORY    ---> \r\n");
		unsigned long Number_Debug_Entries = 0;
		if(rva_Debug && sz_Debug)
		{
			Number_Debug_Entries = sz_Debug / sizeof(IMAGE_DEBUG_DIRECTORY);
			if(Number_Debug_Entries < 1)
			{
				printf("Empty\r\n");
			}
			else
			{
				IMAGE_DEBUG_DIRECTORY* pDebug = (IMAGE_DEBUG_DIRECTORY*) (pMap+rva_Debug);
				for(unsigned long i=0;i<Number_Debug_Entries;i++)
				{
					printf(">>> TimeDateStamp from Entry  %x is:             ---> ",i+1);
					PrintTimeDateStamp((&pDebug[i])->TimeDateStamp);
					
					if( ((&pDebug[i])->Type) == 2)
					{
						//RSDS ---> Only RSDS + GUID + Age + FileName  ===> Useless
						//NB10 ---> NB10 + ZERO + TimeDateStamp + Age + FileName ===> Of Interest
						bool NB10_Found = false;
						unsigned char NB10Buffer[0xC]={0};
						if( (&pDebug[i])->PointerToRawData ) //Raw
						{
							if( SetFilePointer(hFile,(&pDebug[i])->PointerToRawData,0,FILE_BEGIN) != INVALID_SET_FILE_POINTER )
							{
								unsigned long BytesReadd = 0;
								if(ReadFile(hFile,NB10Buffer,0xC,&BytesReadd,0))
								{
									if(BytesReadd == 0xC)
									{
										if(strncmp((char*)NB10Buffer,"NB10",4) == 0)
										{
											unsigned long TimeDateStampNB10 = *(unsigned long*)(&NB10Buffer[8]);
											printf(">>>> TimeDateStamp from PDB2.0 Debug Format     ---> ");
											if(!TimeDateStampNB10)		printf("Empty\r\n");
											else 
											{
												PrintTimeDateStamp(TimeDateStampNB10);
												NB10_Found = true;
											}
										}
									}
								}
							}
						}

						if(NB10_Found == false)
						{
							if( (&pDebug[i])->AddressOfRawData)  //RVA
							{
								memset(NB10Buffer,0,0xC);
								memcpy(NB10Buffer, pMap+((&pDebug[i])->AddressOfRawData),0xC);
								if(strncmp((char*)NB10Buffer,"NB10",4) == 0)
								{
											unsigned long TimeDateStampNB10 = *(unsigned long*)(&NB10Buffer[8]);
											printf(">>>> TimeDateStamp from PDB2.0 Debug Format     ---> ");
											if(!TimeDateStampNB10)		printf("Empty\r\n");
											else 
											{
												PrintTimeDateStamp(TimeDateStampNB10);
											}
								}
							}
						}
					}
					
				}
			}
		}
		//-------------- Trying With "Load Config" Directory-------------------------
		printf("TimeDateStamp from _IMAGE_LOAD_CONFIG_DIRECTORY ---> ");
		if(rva_LoadConfig)
		{
			IMAGE_LOAD_CONFIG_DIRECTORY* pLoadConfig = (IMAGE_LOAD_CONFIG_DIRECTORY*) (pMap + rva_LoadConfig);
			PrintTimeDateStamp(pLoadConfig->TimeDateStamp);
		}
		else printf("Empty\r\n");
	    //---------------------------------------------------------------------------
		UnmapInputFileAndCloseHandles(pMap,hMapping,hFile);
	}
	return;
}
