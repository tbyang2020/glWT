








#define  _CONTROL_CPP_   // 2020��  

#include "Control.h"


#include <commctrl.h>  // LIB: comctl32.lib
// #pragma comment(lib, "comctl32.lib")
#include <RichEdit.h>

#include <assert.h>

#include "qTrace.h"


#define eInfo(a)  { DWORD er=GetLastError(); LPTSTR psz;  \
	FormatMessage(FORMAT_MESSAGE_FROM_SYSTEM|FORMAT_MESSAGE_ALLOCATE_BUFFER,0,er,0,(LPTSTR)&psz,0,0);  \
	MessageBox(0,psz,a,0); LocalFree(psz); }    // static 


// ����һ�пؼ� 
// ����  TOOLTIPS_CLASS ʱ������ id=0,  ����"��Ч�Ĳ˵����".ԭ��δ֪! 

HWND CreateControl(HWND hWnd, LPCTSTR lpClassName, const int id, RECT* rp, int sty) {  // ="Edit"  =1  =0  =0L  

// INITCOMMONCONTROLSEX initCrl; initCrl.dwSize=sizeof(INITCOMMONCONTROLSEX); // initCrl.dwICC=ICC_TAB_CLASSES ... initCrl.dwICC=ICC_TREEVIEW_CLASSES ...  
// InitCommonControlsEx(&initCrl);  


    HINSTANCE hInst = GetModuleHandle(0L);

    RECT re = { 10,10,30,20 };

    if (rp) re = *rp;
    else {
        GetClientRect(hWnd, &re);
        re.right /= 3;
    }

    int iX = re.left, iY = re.top;
    int iH = re.right - re.left, iW = re.bottom - re.top;


    DWORD dwExtStyle = WS_EX_CLIENTEDGE, dwStyle = WS_CHILD | WS_VISIBLE; // | WS_BORDER | WS_SIZEBOX;


    if (sty != 0) dwStyle = sty;
    else {

#if INIT_OPTIONS |1   // ��ͬ�ؼ�����ĳ�ʼ��  

 
        if (strnicmp(lpClassName, TOOLTIPS_CLASS, 6) == 0) {  //  "TOOLTIPS_CLASSA"
            //id = 0;   // ����"��Ч�Ĳ˵����"???   NOTIFY PARENT  --ԭ���� style����|�� !
            dwExtStyle = WS_EX_TOPMOST;
            dwStyle |= TTS_NOPREFIX | TTS_ALWAYSTIP | TTS_BALLOON;
        }  // TOOLTIPS_CLASSA 




        if (strnicmp(lpClassName, "Button", 6) == 0) {  //  "SysTreeView32"
            dwStyle &= ~(WS_BORDER | WS_SIZEBOX);
            dwStyle |= BS_CENTER | BS_VCENTER;  // |BS_FLAT;  BS_NOTIFY ����˫����Ϣ   
        }


        if (strnicmp(lpClassName, "Edit", 4) == 0) {
            dwStyle |= ES_MULTILINE | ES_WANTRETURN | ES_AUTOVSCROLL | ES_NOHIDESEL;
        }


        if (strnicmp(lpClassName, "ComboBox", 8) == 0) {
            dwStyle &= ~(WS_BORDER | WS_SIZEBOX);
            dwStyle |= WS_VSCROLL | CBS_DROPDOWN | CBS_NOINTEGRALHEIGHT | CBS_AUTOHSCROLL;
        }




        if (strnicmp(lpClassName, "Static", 6) == 0) {  //  "Static"
            dwStyle &= ~(WS_BORDER | WS_SIZEBOX);
            dwStyle |= SS_LEFT | SS_WORDELLIPSIS;
        }


        if (strnicmp(lpClassName, WC_LISTVIEW, strlen(WC_LISTVIEW)) == 0) {  // SysListView32   

            DWORD dwExStyle = WS_EX_LEFT | WS_EX_LTRREADING
                | WS_EX_RIGHTSCROLLBAR
                //|WS_EX_NOPARENTNOTIFY
                | WS_EX_CLIENTEDGE
                | LVS_EX_CHECKBOXES         //Ϊʲô����ʾchecks?
                | LVS_EX_FULLROWSELECT;     //Ϊʲô��ѡ������?

            dwExStyle |= LVS_EX_INFOTIP;

            dwStyle |= WS_VSCROLL | WS_HSCROLL | WS_TABSTOP | LVS_REPORT | LVS_SHOWSELALWAYS | LVS_EDITLABELS;   //The parent window must process the LVN_ENDLABELEDIT notification message. 
            dwStyle |= LVS_SINGLESEL;

        }  // WC_LISTVIEW


        if (strnicmp(lpClassName, WC_TREEVIEW, strlen(WC_TREEVIEW)) == 0) {  //  "SysTreeView32"

            dwStyle |= WS_CLIPSIBLINGS | WS_CLIPCHILDREN //|WS_DLGFRAME |TVS_CHECKBOXES | TVS_SINGLEEXPAND
                | TVS_SHOWSELALWAYS | TVS_EDITLABELS
                | TVS_HASBUTTONS | TVS_HASLINES | TVS_LINESATROOT;
        }





        if (strnicmp(lpClassName, RICHEDIT_CLASS, strlen(RICHEDIT_CLASS)) == 0) {  //  "RichEdit20A"


        // ����RichEdit���ڵĶ�̬���ӿ�; GetModuleHandle �����ж��Ƿ��Ѿ�����. �������һ��Ҳû��ϵ. 
            if (NULL == LoadLibrary("RichEd20.Dll")) { MessageBox(0, "RichEd20.Dll", "Error", 0); return 0L; } //eInfo("RichEd20.Dll");

            dwStyle |= WS_DLGFRAME | WS_CLIPSIBLINGS | WS_CLIPCHILDREN | WS_VSCROLL | WS_HSCROLL | ES_MULTILINE | ES_NOHIDESEL | ES_AUTOHSCROLL | ES_AUTOVSCROLL | ES_SELFIME | ES_WANTRETURN;

        } //  "RichEdit20A"


        if (strnicmp(lpClassName, WC_TABCONTROL, strlen(WC_TABCONTROL)) == 0) {  //  "SysTabControl32"

            dwStyle |= WS_CLIPSIBLINGS //| WS_SIZEBOX |TCS_BOTTOM |TCS_BUTTONS
                | TCS_TABS
                | TCS_MULTILINE | TCS_RIGHTJUSTIFY  // |TCS_RAGGEDRIGHT
                | TCS_FIXEDWIDTH  // | TCS_FORCEICONLEFT|TCS_FORCELABELLEFT
                | TCS_HOTTRACK
                | TCS_FOCUSNEVER   // Ok!
                | TCS_MULTISELECT  // | TCS_SCROLLOPPOSITE
                | TCS_TOOLTIPS     // | TCS_VERTICAL;
                | TCS_OWNERDRAWFIXED
                ;

        } //  "SysTabControl32"



#endif  // INIT_OPTIONS

    }  // if...else 



    HWND hControl = CreateWindowEx(dwExtStyle, lpClassName, lpClassName, dwStyle, iX, iY, iH, iW, hWnd, (HMENU)id, hInst, 0L);
    if (!hControl)eInfo("CreateWindowEx");          // qTrace("id=%d...", GetDlgCtrlID(hControl));   
    
// --ԭ���� style����|�� !                 
 // SetWindowLong(hControl, GWL_ID, 0x101);      
// int idt = GetWindowLong(hControl, GWL_ID);  qTrace("id=%d ", idt);
  // Top-Level Window��ָParent��Owner��ΪNULL�Ĵ���,�Ƚϳ����Ĵ�������FrameWnd, Dialog, Tooltip.  
    
   



#if RESET_OPTIONS |1  // ����ؼ�����ĳ�ʼ��   



    if (strnicmp(lpClassName, TOOLTIPS_CLASS, 6) == 0) {  //  "TOOLTIPS_CLASSA"

        TOOLINFO ti = { 0 }; ti.cbSize = sizeof(TOOLINFO); //ti.hinst=hinst;
        ti.uFlags = TTF_SUBCLASS | TTF_TRANSPARENT | TTF_IDISHWND;  // TTF_ABSOLUTE|TTF_TRACK| 
        ti.hwnd = hWnd;

        ti.uId = (UINT)GetDlgItem(hWnd, 0x100);   // 0x101 = ID_CLIENT, ���� 
        ti.lpszText = "MDICLIENT_AREA";   // GetClientRect(hOwner,& ti.rect); //no effect when TTF_IDISHWND;  // ti.rect.left=ti.rect.top=ti.rect.bottom=ti.rect.right=0; 
        if (!SendMessage(hControl, TTM_ADDTOOL, 0, (LPARAM)&ti)) { MessageBox(0, "TTM_ADDTOOLW", 0, 0); }

        ti.uId = (UINT)FindWindowEx(hWnd, 0, "#32770", "IDD_DIALOG2");  // assert(ti.uId);   
        ti.lpszText = "ʶ��ѡ��";

        //qTrace("IDD_DIALOG2=%d", ti.uId);

        if (!SendMessage(hControl, TTM_ADDTOOL, 0, (LPARAM)&ti)) { MessageBox(0, "TTM_ADDTOOLW", 0, 0); }
        //if (!SendMessage(hControl, TTM_SETMAXTIPWIDTH, 0, 120)) { MessageBox(0, "TTM_SETMAXTIPWIDTH", 0, 0); }    // in pixels  



    }  // TOOLTIPS_CLASSA 





    if (strnicmp(lpClassName, WC_LISTVIEW, strlen(WC_LISTVIEW)) == 0) {  // SysListView32   

        dwExtStyle = LVS_EX_FULLROWSELECT
            //|LVS_EX_GRIDLINES 
            //|LVS_EX_TRACKSELECT 
            | LVS_EX_ONECLICKACTIVATE;
        //dwExStyle|=LVS_EX_CHECKBOXES;
        SendMessage(hControl, LVM_SETEXTENDEDLISTVIEWSTYLE, 0, dwExtStyle);    //��Щstyle������Ч:


        // LVCOLUMNW lvc; lvc.mask =  LVCF_WIDTH | LVCF_TEXT; lvc.fmt=LVCFMT_CENTER; lvc.cchTextMax=256;  lvc.cx=80; 
        // LVITEMW vi; vi.mask=LVIF_TEXT; vi.iItem=0; vi.iSubItem=0; vi.cchTextMax=MAX_PATH;
        // SendMessageW(hListView, LVM_INSERTCOLUMNW, ic, (LPARAM)&lvc);

    }  // SysListView32



    if (strnicmp(lpClassName, RICHEDIT_CLASS, strlen(RICHEDIT_CLASS)) == 0) {  //  "RichEdit20A"


    // SendMessage(hControl, EM_SETTEXTMODE, TM_PLAINTEXT, 0);
        SendMessage(hControl, EM_EXLIMITTEXT, 0, -1);  // ����RichEditΪ�޳�������, ����Ϊ64Kb���ı�����.
        SendMessage(hControl, EM_SETMARGINS, EC_RIGHTMARGIN | EC_LEFTMARGIN, 20);  // ����RichEdit���ڵı༭����, ������� 20 ���ؿ��������ʾ�к�. 


        CHARFORMATW cf;  // ����RichEdit���������ز���
        cf.cbSize = sizeof(cf); cf.dwEffects = 0;
        cf.dwMask = CFM_FACE | CFM_SIZE | CFM_COLOR; //|CFM_CHARSET| CFM_BOLD
        wcscpy(cf.szFaceName, L"Traditional Arabic");
        cf.yHeight = 12 * 20; cf.crTextColor = RGB(100, 200, 0);  //�����ַ��߶�,��ɫ��
        SendMessage(hControl, EM_SETCHARFORMAT, SCF_SELECTION, (long)&cf);//SCF_ALL

        // SendMessage(hControl, EM_SETWORDWRAPMODE,WBF_CUSTOM,0);  // WBF_WORDWRAP,WBF_WORDBREAK,
        // SendMessage(hControl, EM_SETZOOM,32,42);  // SCF_ALL

        // �����µĴ��ڵ�ַ, lpEditProcΪRichEdit �����ô��ڴ����ַ. 
        // lpEditProc =(WNDPROC)SetWindowLong( hWinEdit, GWL_WNDPROC, (long)&SubEditProc );
        // SetWindowText(hControl, TEXT(" ����һ���򵥵Ĳ��� \n ���ǵ�2��.\n ���ǵ�3�� ") );          
    } //  "RichEdit20A"



    if (strnicmp(lpClassName, WC_TABCONTROL, strlen(WC_TABCONTROL)) == 0) {  //  "SysTabControl32"

        TCITEMW tie; tie.mask = TCIF_TEXT | TCIF_IMAGE; tie.iImage = -1; tie.cchTextMax = 16;
        tie.pszText = L"Tab 1";  SendMessage(hControl, TCM_INSERTITEMW, 0, (LPARAM)&tie);  // TabCtrl_InsertItemW(hControl,0, &tie) ; //TAB_WORDS=1  
        tie.pszText = L"Tab 2";  SendMessage(hControl, TCM_INSERTITEMW, 1, (LPARAM)&tie);     // TAB_DICTS=2
        tie.pszText = L"Tab 3"; SendMessageW(hControl, TCM_INSERTITEMW, 2, (LPARAM)&tie);    // TAB_DICTS=2
        tie.pszText = L"Tab 4"; SendMessageW(hControl, TCM_INSERTITEMW, 3, (LPARAM)&tie);    // TAB_DICTS=2
        SendMessage(hControl, TCM_SETCURSEL, 0, 0);

        // ����������: ������ɫ
        // DWORD dwr=SetClassLong(hControl,GCL_HBRBACKGROUND,(long)CreateSolidBrush(RGB(0,255,100)));  \

    } //  "SysTabControl32"








#endif  // RESET_OPTIONS


    return hControl;
}





#if WC_LISTVIEW_MGR|1  


// CreateLV(  ) 






// ����� AddListViewColumns ,  Adding List View Columns in report view. szHeadings: '|' separated headings.

int AddListViewColumns(HWND hListView,  const wchar_t* szHeadings) {  //=1//=NULL

if(szHeadings==NULL) return 0;  // ��������һ�� 




    LVCOLUMNW lvc;
    lvc.mask = LVCF_WIDTH | LVCF_TEXT; // |     // | LVCF_SUBITEM   //|LVCF_FMT   // format, width, text, and subitem   
    // lvc.fmt=LVCFMT_CENTER
    lvc.pszText = new wchar_t[MAX_PATH];  lvc.pszText[0] = 0;
    lvc.cchTextMax = 256; // lvc.cx = 80;  // lvc.iSubItem=0;  

   
    const wchar_t*p = szHeadings; const wchar_t*q = NULL;//�����븳ֵNULL!//=wcschr(p,'|');


int ic=0; 

while(p){
q = wcschr(p, '|');
if(!q) q=wcsrchr(p,0);  
if(!q) break;

memcpy(lvc.pszText, p, (q-p)*2);  lvc.pszText[ q-p ] =0;  
lvc.cx = 20+10*wcslen(lvc.pszText);  // ÿ���ַ�ռ�ݴ�Լ 10 ������
SendMessageW(hListView, LVM_INSERTCOLUMNW, ic, (LPARAM)&lvc);


if(*q==0) break;
p=q+1;  ic++; 
}  // while(p) 


delete[] lvc.pszText;
return ic;
}




//AddListViewItem, ����б��и��� .  SendMessageW(hListView,LVM_GETITEMCOUNT,0,0)
// szItem: '|' separated row items, rows are distinguished by '>' 

int AddListViewItems(HWND hListView, const wchar_t* szItem) {   //=NULL
    if (szItem == NULL) return 0;
    int ic = wcslen(szItem);
    if (ic == 0) return 0;  //|| ic>MAX_PATH*100

    LVITEMW vi; vi.mask = LVIF_TEXT;  // | LVIF_IMAGE | LVIF_PARAM | LVIF_STATE;  
//    vi.iItem = 0; vi.iSubItem = 0; 
vi.cchTextMax = MAX_PATH;

    wchar_t*szText = new wchar_t[ic + 20]; wcscpy(szText, szItem); 
    wchar_t*p = szText, *q = NULL, *r = NULL;


int ir = 0;  ic = 0;
    
do {  //  д�� 

q = wcschr(p, '>');    // ��һ�б�־ 
 if (q)*q = 0;

        ic = 0;
        do {  // д��֮�� 
            r = wcschr(p, '|');       // ��һ�б�־ 
            if (r)*r = 0;

            vi.pszText = p;
            vi.iItem = ir;  vi.iSubItem = ic;
            if (ic == 0)SendMessageW(hListView, LVM_INSERTITEMW, 0, (LPARAM)&vi);
            else SendMessageW(hListView, LVM_SETITEMW, 0, (LPARAM)&vi);
            ic++;

            if (r) { *r = '|'; p = r + 1; }  // ��һ��     
        } while (r);

        
if (q) { *q == '>'; p = q + 1; }  // ��һ�� 
ir++;

} while (q);

    

delete[] szText;
return ic;
}






#endif  // WC_LISTVIEW_MGR





#if TOOLTIPS_CLASS_MGR|1




bool AddTool(HWND hWnd, int uID, HWND hTooltip, LPSTR wtip) {
    TOOLINFO ti = { 0 }; ti.cbSize = sizeof(TOOLINFO);  // ti.hinst=hinst;
    ti.uFlags = TTF_SUBCLASS | TTF_IDISHWND;  //| TTF_TRACK|TTF_ABSOLUTE  | TTF_TRANSPARENT;   //  |TTF_CENTERTIP
    ti.hwnd = hWnd;  ti.uId = (UINT)GetDlgItem(hWnd, uID);  // uID; //
    ti.lpszText = wtip;
    if (!SendMessage(hTooltip, TTM_ADDTOOL, 0, (LPARAM)&ti)) { eInfo("TTM_ADDTOOL"); return false; }    // TTM_SETMAXTIPWIDTH 
    return true;
}



//UpdateTip.  ע�� TTN_GETDISPINFO

void UpdateTip(HWND hWnd, UINT uID, HWND hTooltip, LPWSTR newTip){
    TOOLINFOW ti; ti.cbSize = sizeof(TOOLINFOW);  
    ti.hwnd = hWnd;  
    ti.uId = (UINT)GetDlgItem(hWnd, uID);   
    ti.lpszText = newTip;   
    SendMessageW(hTooltip, TTM_UPDATETIPTEXTW, 0, (LPARAM)&ti);
}



void UpdateTip(HWND hWnd, UINT uID, HWND hTooltip, char* newTip) {
    TOOLINFO ti; ti.cbSize = sizeof(TOOLINFO);  // ti.uFlags= TTF_SUBCLASS | TTF_IDISHWND| TTF_TRACK;//|TTF_ABSOLUTE;  //  | TTF_TRANSPARENT;   //  |TTF_CENTERTIP
    ti.hwnd = hWnd;
    ti.uId = (UINT)GetDlgItem(hWnd, uID);
    ti.lpszText = newTip;
    SendMessage(hTooltip, TTM_UPDATETIPTEXT, 0, (LPARAM)&ti);
}


#endif  // TOOLTIPS_CLASS_MGR