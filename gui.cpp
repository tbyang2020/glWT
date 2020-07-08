


#include <Windows.h>
#include "res/resource.h"

#include <assert.h>


#include <commctrl.h>  // LIB: comctl32.lib
// #pragma comment(lib, "comctl32.lib")

#include <RichEdit.h>

#include <stdio.h>  // sprintf  

#include <io.h>    // access 


#define _GUI_CPP_

#include "gui.h"
#include "Control.h"

#include "qTrace.h"


#include "tesseractmain.h"


bool GetFileName(LPTSTR szFile, bool bSave = FALSE, LPCTSTR pFilter = 0L, HWND hWnd = 0L);    // InterfaceMDI.cpp 


// IDD_DIALOG2,   识别选项子窗口 

INT_PTR CALLBACK DialogProc_UI(HWND hdlg, UINT uMsg, WPARAM wParam, LPARAM lParam) {


switch (uMsg) {

case WM_INITDIALOG: {




}break;  //SetClassLong  // HINSTANCE hinst=GetModuleHandle(0); 


case WM_DESTROY:
        // MessageBox(0,"WM_DESTROY",0,0);
break;



case WM_COMMAND:
        
switch (LOWORD(wParam)) {

        
case IDOK: {
      
char szcmd[256]={0};
SendDlgItemMessage(hdlg,IDC_EDIT1,WM_GETTEXT,254,(long)szcmd); 
// MessageBox(0,szcmd,"main32",0);

int ic=0; char*p=szcmd;
while(*p){ ic++; p++; }



// SetCurrentDirectory("D:/Tbyang20/文字识别/Worksite/测试图片");  // 注意!CurrentDirectory 会因为浏览文件而实时改变!





#if CMD_RECO |0

 main32(szcmd);

 #else


int ise=SendDlgItemMessage(hdlg,IDC_COMBO1,CB_GETCURSEL,0,0); 
p=(char*)SendDlgItemMessage(hdlg, IDC_COMBO1,CB_GETITEMDATA,ise,0);

ise=SendDlgItemMessage(hdlg, IDC_COMBO2,CB_GETCURSEL,0,0);
char*q=(char*)SendDlgItemMessage(hdlg, IDC_COMBO2,CB_GETITEMDATA,ise,0);

main32s(p,q);


#endif  // CMD_RECO







// MessageBox(0,"不要关闭!",0,0);
            
//EndDialog(hdlg,GetDlgItemInt(hdlg,IDC_COMBO1,0,0));    //EndDialog(hdlg, 1);
return true;
}break;

        
case IDCANCEL:
EndDialog(hdlg, 0);
// DestroyWindow(hdlg);  
// SendMessage(GetParent(hdlg),WM_APP+0x01,0,0);
break;




case IDC_BUTTON1:
case IDC_BUTTON2:
case IDC_BUTTON3:
case IDC_BUTTON4:
//case PN_IMAGE_BUTTON:
//case PN_DATA_BUTTON: 
{   // HIWORD(wParam) 是 BN_CLICKED,  不是 BM_CLICK
    char szFile[256]={0};
    bool bok = GetFileName(szFile);
    if (!bok) break;

int icm= IDC_COMBO1;
if(LOWORD(wParam) == IDC_BUTTON2) icm= IDC_COMBO2;
if(LOWORD(wParam) == IDC_BUTTON3) icm= IDC_COMBO3;
if(LOWORD(wParam) == IDC_BUTTON4) icm= IDC_COMBO4;




char*p = strrchr(szFile, '\\'); 
if(!p) p=strrchr(szFile, '/');
if (p) p++; else p = szFile;

char*data=new char[256];  strcpy(data,szFile); 

int ise = SendDlgItemMessage(hdlg, icm, CB_ADDSTRING, 0, (long)p);
SendDlgItemMessage(hdlg, icm, CB_SETCURSEL, ise, 0);
SendDlgItemMessage(hdlg, icm, CB_SETITEMDATA, ise, (long)data);  // 不可以总使用地址 szFile 
     
HWND hToolTip =  FindWindowEx( GetParent(hdlg), 0, TOOLTIPS_CLASS, 0);  // GetDlgItem(GetParent(hdlg), IDC_PNTOOLTIP);  //
// assert(hToolTip);

UpdateTip(hdlg, icm,hToolTip, szFile); 


MakeCmd(hdlg);

}break;







default:break;
}//switch(wmId)

break; // WM_COMMAND

    


default:break;  // switch (uMsg)
}
    
return FALSE;  // message  not processed 
}



HWND AddPanel(HWND hWnd) {

HWND hdlg_op = 0L;
if (!hdlg_op) {
hdlg_op = CreateDialog(0, (LPCSTR)IDD_DIALOG2, hWnd, DialogProc_UI);
if (!hdlg_op) return 0L;
// SetParent(hdlg_op,hWnd);  // -- Style: Child
}


    // https://zhidao.baidu.com/question/419852323.html
    // FindWindowEx, 类名 #32770
    //HWND  th=FindWindowEx(hWnd,0,"#32770","IDD_DIALOG2"); 
    //HWND  th=GetDlgItem(hWnd,IDD_DIALOG2); 
    //qTrace("hdlg_op=%X, th=%X", hdlg_op, th);


 int T = 0, L = 0;

    RECT rc; GetWindowRect(hdlg_op, &rc);

    int W = rc.right - rc.left, H = rc.bottom - rc.top;

MoveWindow(hdlg_op, L, T, W -2*L, H - 2*T, true);


#if INIT_PANEL|1

// "命令行附加选项\r\n例如\r\n--help\r\n--v\r\n-l chi_tra_vert"  

    // strcpy(szt,"D:/Tbyang20/文字识别/Worksite/测试图片/num.bmp");
    SendDlgItemMessage(hdlg_op, IDC_COMBO1, CB_ADDSTRING, 0, (long)"num.bmp");
    SendDlgItemMessage(hdlg_op, IDC_COMBO1, CB_SETCURSEL, 0, 0);
    SendDlgItemMessage(hdlg_op, IDC_COMBO1, CB_SETITEMDATA, 0, (long)"D:/Tbyang/文字识别/Worksite/测试图片/num.bmp");

    SendDlgItemMessage(hdlg_op, IDC_COMBO2, CB_ADDSTRING, 0, (long)"result.txt");
    SendDlgItemMessage(hdlg_op, IDC_COMBO2, CB_SETCURSEL, 0, 0);
    SendDlgItemMessage(hdlg_op, IDC_COMBO2, CB_SETITEMDATA, 0, (long)"D:/Tbyang/文字识别/Worksite/测试图片/result.txt");

    SendDlgItemMessage(hdlg_op, IDC_COMBO3, CB_ADDSTRING, 0, (long)"tessdata");
    SendDlgItemMessage(hdlg_op, IDC_COMBO3, CB_SETCURSEL, 0, 0);
    SendDlgItemMessage(hdlg_op, IDC_COMBO3, CB_SETITEMDATA, 0, (long)"D:/Tbyang/文字识别/Worksite/tessdata3.02/tessdata");

     



    HWND hCtrl = CreateControl(hWnd, TOOLTIPS_CLASS, IDC_PNTOOLTIP);  // IDC_PNTOOLTIP无效! 

    AddTool(hdlg_op, IDC_COMBO1, hCtrl, "图片文件");  // UpdateTip(hdlg_op, IDC_COMBO1, hCtrl, "图片文件......");
    AddTool(hdlg_op, IDC_COMBO2, hCtrl, "保存识别结果的文件名");
    AddTool(hdlg_op, IDC_COMBO3, hCtrl, "D:/Tbyang/文字识别/Worksite/tessdata3.02/tessdata");

    //HWND hToolTip = FindWindowEx(hWnd, 0, TOOLTIPS_CLASS, 0);  // GetDlgItem(GetParent(hdlg), IDC_PNTOOLTIP);  //
    // assert(hToolTip);

    UpdateTip(hdlg_op, IDC_COMBO1, hCtrl, "D:/Tbyang/文字识别/Worksite/测试图片/num.bmp");
    UpdateTip(hdlg_op, IDC_COMBO2, hCtrl, "D:/Tbyang/文字识别/Worksite/测试图片/result.txt");



#if OCR_OPTIONS |0 

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

#endif  // OCR_OPTIONS

    
char szt[64] = { 0 };
int i=0;
for(i=0; i<14; i++){
  sprintf(szt,"%d",i);
  SendDlgItemMessage(hdlg_op, IDC_COMBO5, CB_ADDSTRING, 0, (long)szt);  //    // 有些style设置无效:
  if(i<=3) SendDlgItemMessage(hdlg_op, IDC_COMBO6, CB_ADDSTRING, 0, (long)szt);  //    // 有些style设置无效:
}
SendDlgItemMessage(hdlg_op, IDC_COMBO5, CB_SETCURSEL, 0, 0);
SendDlgItemMessage(hdlg_op, IDC_COMBO6, CB_SETCURSEL, 0, 0);

 


    HWND hList = GetDlgItem(hdlg_op, IDC_LIST1);   // assert(hList);



    // VS_EX_CHECKBOXES 
    // | LVS_EX_FULLROWSELECT
    // | LVS_EX_GRIDLINES 
    // | LVS_EX_TRACKSELECT 
    // | LVS_EX_ONECLICKACTIVATE;
    SendMessage(hList, LVM_SETEXTENDEDLISTVIEWSTYLE, 0, LVS_EX_FULLROWSELECT | LVS_EX_CHECKBOXES);  //    // 有些style设置无效:



#if FOR_REFERENCE|0

    LVCOLUMNW vc;
    vc.mask = LVCF_WIDTH | LVCF_TEXT; // | LVCF_SUBITEM;  
    //vc.fmt=LVCFMT_CENTER; vc.cchTextMax=256;  
    //vc.iSubItem = 0;   // 添加序号
    vc.cx = 40;    // 200 
    vc.pszText = L"选项";
    int ic = 0;
    //ListView_InsertColumn(hList, 0, &vc); //向list control插入lvc
    SendMessageW(hList, LVM_INSERTCOLUMNW, ic++, (LPARAM)&vc);

    GetWindowRect(hList, &rc);
    vc.pszText = L"命令行"; vc.cx = rc.right - rc.left - 40 - 20 - 8;    // 200 
    SendMessageW(hList, LVM_INSERTCOLUMNW, ic++, (LPARAM)&vc);
    // AddListViewColumns(hList, 3, L"选择|命令行");

    //vc.pszText = L"说明"; vc.cx= 40;    // 200 
    //SendMessageW(hList, LVM_INSERTCOLUMNW, ic, (LPARAM)&vc);

#endif  // FOR_REFERENCE

    AddListViewColumns(hList, L"选项|命令行    |说明            ");





#if FOR_REFERENCE|0

    LVITEMW vi;
    vi.mask = LVIF_TEXT; vi.cchTextMax = MAX_PATH;

    vi.iItem = 0;

    vi.pszText = L"";
    vi.iSubItem = 0;
    SendMessageW(hList, LVM_INSERTITEMW, 0, (LPARAM)&vi);
    // SendMessageW(hList, LVM_SETITEMW, 0, (LPARAM)&vi);

    vi.pszText = L" -lang...1";
    vi.iSubItem = 1;
    SendMessageW(hList, LVM_SETITEMW, 0, (LPARAM)&vi);


    vi.iItem = 1;

    vi.pszText = L"";
    vi.iSubItem = 0;
    SendMessageW(hList, LVM_INSERTITEMW, 0, (LPARAM)&vi);

    vi.pszText = L" -lang...2";
    vi.iSubItem = 1;
    SendMessageW(hList, LVM_SETITEMW, 0, (LPARAM)&vi);

#endif  // FOR_REFERENCE



#if FROM_TESSERACTMAIN |0 

    void PrintUsage(const char* program) {
        printf(
            "Usage:\n"
            "  %s --help | --help-psm | --help-oem | --version\n"
            "  %s --list-langs [--tessdata-dir PATH]\n"
            "  %s --print-parameters [options...] [configfile...]\n"
            "  %s imagename|stdin outputbase|stdout [options...] [configfile...]\n",
            program, program, program, program);
    }

#endif  // FROM_TESSERACTMAIN



    AddListViewItems(hList, L"|--help| "
        ">|--help-psm| "
        ">|--help-oem| "
        ">|--version| "
        ">|--list-langs |[--tessdata-dir PATH] "
        ">|--print-parameters |[options...] [configfile...]"
        ">|imagename/stdin outputbase/stdout |[options...] [configfile...]");



#endif  //  INIT_PANEL







    MakeCmd(hdlg_op);
    return hdlg_op;
}










// ......................................................


int CreatePanel(HWND hWnd) {

    int B = 4;

    RECT rc; GetClientRect(hWnd, &rc);

    rc.right /= 3;

    int W = rc.right, H = rc.bottom;
    W -= 2 * B;
    H -= 2 * B;

    RECT re = rc;

    re.left = W * 2 / 3 + 2 * B;
    re.right = W;
    re.top = 2 * B;
    re.bottom = 2 * B + 24;

    
HWND hCtrl = CreateControl(hWnd, "Button", IDC_PNIMAGE_BUTTON, &re);
    SendMessage(hCtrl, WM_SETTEXT, 0, (long)"图形文件");

    re.left = 2 * B;
    re.right = W * 2 / 3;
    re.top = 2 * B;
    re.bottom = H / 2;

    hCtrl = CreateControl(hWnd, "ComboBox", IDC_PNIMAGE_COMBO, &re);
    SendMessage(hCtrl, CB_ADDSTRING, 0, (long)"tess_test.bmp");
    SendMessage(hCtrl, CB_SETCURSEL, 0, 0);


    re.left = W * 2 / 3 + 2 * B;
    re.right = W;
    re.top = 4 * B + 24;
    re.bottom = re.top + 24;

    hCtrl = CreateControl(hWnd, "Button", IDC_PNDATA_BUTTON, &re);
    SendMessage(hCtrl, WM_SETTEXT, 0, (long)"识别结果");


    re.left = 2 * B;
    re.right = W * 2 / 3;
    re.top = 4 * B + 24;
    re.bottom = H / 2;

    hCtrl = CreateControl(hWnd, "ComboBox", IDC_PNDATA_COMBO, &re);
    SendMessage(hCtrl, WM_SETTEXT, 0, (long)"tess_test");


    re.left = B;
    re.top = (4 * B + 24) * 2;
    re.right = W;
    re.bottom = (4 * B + 24) * 2 + (H - (4 * B + 24) * 2) / 3;

    hCtrl = CreateControl(hWnd, "Edit", IDC_PNCMDINPUT_EDIT, &re);

    SendMessage(hCtrl, WM_SETTEXT, 0, (long)"命令行附加选项\r\n例如\r\n--help\r\n--v\r\n-l chi_tra_vert");
    // assert( INVALID_HANDLE_VALUE !=GetStdHandle(STD_ERROR_HANDLE)); 



    re.left = B;
    re.top = (4 * B + 24) * 2 + (H - (4 * B + 24) * 2) / 3 + B;
    re.right = W;
    re.bottom = H - 24 * 2;

    hCtrl = CreateControl(hWnd, "Edit", IDC_PNCMD_EDIT, &re);

    SendMessage(hCtrl, WM_SETTEXT, 0, (long)"命令行预览:\r\n");
    // assert( INVALID_HANDLE_VALUE !=GetStdHandle(STD_ERROR_HANDLE)); 


    hCtrl = CreateControl(hWnd, TOOLTIPS_CLASS);
    AddTool(hWnd, IDC_PNCMDINPUT_EDIT, hCtrl, "命令行附加选项\r\n例如\r\n--help\r\n--v\r\n-l chi_tra_vert");
    AddTool(hWnd, IDC_PNCMD_EDIT, hCtrl, "命令行预览");






    UpdateCmd(hWnd);

    return 6;
}



int UpdateCmd(HWND hWnd) {


    const char* Tess4 = "d:/tbyangw10/Tesseract/Tesseract-OCR";
    // path     ../../tesseract-ocr;%path%
    // tesseract 回耶辨真.bmp  回耶辨真  -l chi_tra_vert

    char szcmd[1024], szt[256];
    GetModuleFileName(0, szt, 256);  // GetCurrentDirectory(256, szt);
    int argc = 1;
    char*p = szcmd;

    int ip = sprintf(p, "%s", szt);                                           // argv[0]  当前程序. 也可以指定最新版本 Tesseract 4.0


    int ie = SendDlgItemMessage(hWnd, IDC_PNIMAGE_COMBO, CB_GETCURSEL, 0, 0);
    SendDlgItemMessage(hWnd, IDC_PNIMAGE_COMBO, CB_GETLBTEXT, ie, (long)szt);   // WM_GETTEXT 也可以 


    //\\if( access(szt, 0)==0 ) {  
    ip += sprintf(p + ip, "\r\n%s", szt);                                     // argv[1]  图片文件
    argc++;

    SendDlgItemMessage(hWnd, IDC_PNDATA_COMBO, WM_GETTEXT, 256, (long)szt);     //  没有图片文件, outputbas 参数就没有意义

    //\\if( access(szt, 0)==0 )
    { ip += sprintf(p + ip, "\r\n%s", szt); argc++; }  // argv[2]  识别结果 

    //\\}  // if  




    SendDlgItemMessage(hWnd, IDC_PNCMDINPUT_EDIT, WM_GETTEXT, 256, (long)szt);
    char*q = strchr(szt, ':');
    if (q) q++; else q = szt;

    char*r = strchr(q, '-'); char*t = 0;

    while (r) {

        t = r + 1;
        if (*t == '-') t++;

        t = strchr(t, '-');


        if (t) *t = 0;
        ip += sprintf(p + ip, "\r\n%s", r);               // argv[3]  其他选项  ......
        if (t) *t = '-';

        if (!t) break;

        r = t;
    }  // while (r) 



    SendDlgItemMessage(hWnd, IDC_PNCMD_EDIT, WM_SETTEXT, 256, (long)szcmd);

    return argc;
}








int MakeCmd(HWND hdlg) {

const char* Tess4 = "d:/tbyang/文字识别/Tesseract/Tesseract-OCR";   // path     ../../tesseract-ocr;%path%


// tesseract 回耶辨真.bmp  回耶辨真  -l chi_tra_vert

    char szcmd[1024], szt[256];
    GetModuleFileName(0, szt, 256);  // GetCurrentDirectory(256, szt);
    int argc = 1;
    char*p = szcmd;

    // int ip = sprintf(p, "%s", szt);   // argv[0]  当前程序. 也可以指定最新版本 Tesseract 4.0
    int ip = sprintf(p, "tesseract.exe");   // argv[0]  没有使用
    
    int ie = SendDlgItemMessage(hdlg, IDC_COMBO1, CB_GETCURSEL, 0, 0);
    // SendDlgItemMessage(hdlg, IDC_COMBO1, CB_GETLBTEXT, ie, (long)szt);   // WM_GETTEXT 也可以 

   char* pd=(char*)SendDlgItemMessage(hdlg, IDC_COMBO1, CB_GETITEMDATA, ie,0);   // qTrace("[%s]",pd);  // WM_GETTEXT 也可以 

    //\\if( access(szt, 0)==0 ) {  
   if ((int)pd != CB_ERR) { ip += sprintf(p + ip, "\r\n%s", pd); argc++;  }        // argv[1]  图片文件
   
  
    ie=SendDlgItemMessage(hdlg, IDC_COMBO2, CB_GETCURSEL, 0, 0);    
    pd=(char*)SendDlgItemMessage(hdlg, IDC_COMBO2, CB_GETITEMDATA, ie,0);

    //\\if( access(szt, 0)==0 )
    if((int)pd!=CB_ERR){ ip += sprintf(p + ip, "\r\n%s", pd); argc++; }           // argv[2]  识别结果    //  没有图片文件, outputbas 参数就没有意义

    //\\}  // if  



#if MORE_CMD|0

//#define ListView_SetCheckState(hwndLV, i, fCheck) \
  ListView_SetItemState(hwndLV, i, INDEXTOSTATEIMAGEMASK((fCheck)?2:1), LVIS_STATEIMAGEMASK)

//#define ListView_GetItemState(hwndLV, i, mask) \
   (UINT)SNDMSG((hwndLV), LVM_GETITEMSTATE, (WPARAM)(i), (LPARAM)(mask))

//#define ListView_GetCheckState(hwndLV, i) \
   ((((UINT)(SNDMSG((hwndLV), LVM_GETITEMSTATE, (WPARAM)(i), LVIS_STATEIMAGEMASK))) >> 12) -1)


HWND hList=GetDlgItem(hdlg,IDC_LIST1); 
ListView_SetCheckState(hList,0,true);

int ick=0;  
int iL=SendDlgItemMessage(hdlg, IDC_LIST1, LVM_GETITEMCOUNT,0,0);  qTrace("%d",iL);
for(int i=0; i<iL; i++ ){
ick=ListView_GetCheckState(hList,i);
if(ick) { ListView_GetItemText(hList, i, 1, szt, 256);  MessageBox(0,szt,0,0); }
}


#endif  // MORE_CMD


    SendDlgItemMessage(hdlg, IDC_EDIT1, WM_SETTEXT, 0, (long)szcmd);
    
    return argc;
}


