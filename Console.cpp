


#include <Windows.h>
#include <stdio.h>

// �� cmd ����, �����䴰�ڶ�д��Ϣ  


#include <output.h>


int CreateConsole(){

const char* Tess4="d:/tbyangw10/Tesseract/Tesseract-OCR"; 


// PrintUsage 
const char* ocr_usage = 
      "Usage:\n"
      "  %s --help | --help-psm | --help-oem | --version\n"
      "  %s --list-langs [--tessdata-dir PATH]\n"
      "  %s --print-parameters [options...] [configfile...]\n"
      "  %s imagename|stdin outputbase|stdout [options...] [configfile...]\n"; 


// PrintHelpMessage
const char* ocr_options =
      "OCR options:\n"
      "  --tessdata-dir PATH   Specify the location of tessdata path.\n"
      "  --user-words PATH     Specify the location of user words file.\n"
      "  --user-patterns PATH  Specify the location of user patterns file.\n"
      "  -l LANG[+LANG]        Specify language(s) used for OCR.\n"
      "  -c VAR=VALUE          Set value for config variables.\n"
      "                        Multiple -c arguments are allowed.\n"
      "  --psm NUM             Specify page segmentation mode.\n"
      "  --oem NUM             Specify OCR Engine mode.\n"
      "NOTE: These options must occur before any configfile.\n";




char app[128]= "c:\\Windows\\System32\\cmd.exe";
char cmd[1024]= "/k @echo Tesseract ������:";      // ��ʹ��"/k "����

int ip=sprintf(cmd,"/k echo Tesseract ������:\r\n"); 
ip+=sprintf(cmd+ip,"set usg=%s\r\n", ocr_usage);     
ip+=sprintf(cmd+ip,"set opt=%s\r\n", ocr_options);     


FILE* fp=fopen("c:/s_tmp.txt","wb");
if(!fp){ MessageBox(0,strerror(errno),"fopen c:/s_tmp.txt",0); return 0; }
fprintf(fp,"%s\r\n", ocr_usage);
fprintf(fp,"%s\r\n", ocr_options);
fclose(fp);



fp=fopen("c:/tmp.bat","wb");
if(!fp){ MessageBox(0,strerror(errno),"fopen c:/...",0); return 0; }

fprintf(fp,"@echo Tesseract ������:\r\n");
fprintf(fp,"@type c:\\s_tmp.txt\r\n");

fprintf(fp,"@cd %s\r\n", Tess4);

fclose(fp);
  

sprintf(cmd,"/k @echo off & c:/tmp.bat & @echo on");  // ��ʹ��"/k "����



// ��ָ�� dwCreationFlags = CREATE_NEW_CONSOLE, ��ʹ�� main ���� 

STARTUPINFO si; memset(&si,0,sizeof(si)); si.cb=sizeof(si);  PROCESS_INFORMATION pi;      
////si.dwFlags=STARTF_USESHOWWINDOW;  si.wShowWindow=SW_SHOW;  //// Ignored unless dwFlags specifies STARTF_USESHOWWINDOW. 
CreateProcess(app,cmd,   0L,0L,false,  0, 0L, 0L,&si,&pi);    

// WaitForInputIdle(pi.hProcess,INFINITE);       // �ȴ�,ֱ���ӹ��̽��ܼ�������. ������, Ҳ��̫��. 
// WaitForSingleObject(pi.hProcess,INFINITE);    // �ȴ�,ֱ���ӹ����˳�!  

CloseHandle(pi.hThread);
CloseHandle(pi.hProcess);

Sleep(1000);
DeleteFile("c:/s_tmp.txt"); \
DeleteFile("c:/tmp.bat"); 
return 1; 
}