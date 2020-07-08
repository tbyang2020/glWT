


#include <Windows.h>
#include <stdio.h>

// 打开 cmd 进程, 并向其窗口读写消息  


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
char cmd[1024]= "/k @echo Tesseract 命令行:";      // 不使用"/k "不行

int ip=sprintf(cmd,"/k echo Tesseract 命令行:\r\n"); 
ip+=sprintf(cmd+ip,"set usg=%s\r\n", ocr_usage);     
ip+=sprintf(cmd+ip,"set opt=%s\r\n", ocr_options);     


FILE* fp=fopen("c:/s_tmp.txt","wb");
if(!fp){ MessageBox(0,strerror(errno),"fopen c:/s_tmp.txt",0); return 0; }
fprintf(fp,"%s\r\n", ocr_usage);
fprintf(fp,"%s\r\n", ocr_options);
fclose(fp);



fp=fopen("c:/tmp.bat","wb");
if(!fp){ MessageBox(0,strerror(errno),"fopen c:/...",0); return 0; }

fprintf(fp,"@echo Tesseract 命令行:\r\n");
fprintf(fp,"@type c:\\s_tmp.txt\r\n");

fprintf(fp,"@cd %s\r\n", Tess4);

fclose(fp);
  

sprintf(cmd,"/k @echo off & c:/tmp.bat & @echo on");  // 不使用"/k "不行



// 不指定 dwCreationFlags = CREATE_NEW_CONSOLE, 则使用 main 窗口 

STARTUPINFO si; memset(&si,0,sizeof(si)); si.cb=sizeof(si);  PROCESS_INFORMATION pi;      
////si.dwFlags=STARTF_USESHOWWINDOW;  si.wShowWindow=SW_SHOW;  //// Ignored unless dwFlags specifies STARTF_USESHOWWINDOW. 
CreateProcess(app,cmd,   0L,0L,false,  0, 0L, 0L,&si,&pi);    

// WaitForInputIdle(pi.hProcess,INFINITE);       // 等待,直到子过程接受键盘输入. 试验结果, 也是太早. 
// WaitForSingleObject(pi.hProcess,INFINITE);    // 等待,直到子过程退出!  

CloseHandle(pi.hThread);
CloseHandle(pi.hProcess);

Sleep(1000);
DeleteFile("c:/s_tmp.txt"); \
DeleteFile("c:/tmp.bat"); 
return 1; 
}