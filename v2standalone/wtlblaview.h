// VIRUZPANEL.h : interface of the CVIRUZPANEL class
//
/////////////////////////////////////////////////////////////////////////////

#if !defined(AFX_VIRUZPANEL_H__213CE356_CC77_11D4_9322_00A0CC20D58C__INCLUDED_)
#define AFX_VIRUZPANEL_H__213CE356_CC77_11D4_9322_00A0CC20D58C__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#include "multibutton.h"
#include "../sounddef.h"
#include "../types.h"
#include "../synth.h"
#include "midi.h"
#include <math.h>

#define  VPE_SETPOINTER  0xF0
#define  VPE_EDIT        0xF1
#define  VPE_GOTOPATCH   0xF2

/*******************************************************************************\
*
*	Linkes Patchedit-Panel: Parameteransicht
*
\*******************************************************************************/


class CViruZPanel : public CScrollWindowImpl<CViruZPanel>
{
public:
	DECLARE_WND_CLASS_EX("fr_ViruZPanel", CS_HREDRAW|CS_VREDRAW, COLOR_BTNFACE)

	CStatic *topics;
	CStatic *labels;
	CWindow **parm;
	
	BOOL PreTranslateMessage(MSG* pMsg)
	{
		pMsg;
		return FALSE;
	}

	BEGIN_MSG_MAP(CViruZPanel)
		MESSAGE_HANDLER(WM_CREATE, OnCreate)
		MESSAGE_HANDLER(WM_DESTROY, OnDestroy)
		MESSAGE_HANDLER(WM_COMMAND, OnCommand)
		MESSAGE_HANDLER(WM_HSCROLL, OnHScroll)
		MESSAGE_HANDLER(WM_CTLCOLORSTATIC, OnCtlColorStatic)
		MESSAGE_HANDLER(TBM_SETPOS, OnSetPos)
		CHAIN_MSG_MAP(CScrollWindowImpl<CViruZPanel>)
	END_MSG_MAP()

	CViruZPanel() : CScrollWindowImpl<CViruZPanel>()
	{
		parm=new CWindow *[v2nparms];
		for (int i=0; i<v2nparms; i++)
			parm[i]=0;
		labels=new CStatic [v2nparms];
		topics=new CStatic [v2ntopics];
	}

	~CViruZPanel()
	{
		delete labels;
		delete topics;
		delete parm;
	}

	void DoPaint(CDCHandle /*dc*/)
	{
	}


	LRESULT OnDestroy(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& bHandled)
	{
		bHandled=0;
		return 0;
	}

	LRESULT OnCreate(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
	{
		HFONT fnt=(HFONT)GetStockObject(DEFAULT_GUI_FONT);

		int ypos=2;
		int xpos=0;
		int tn=0;
		for (int i=0; i<v2nparms; i++)
		{

			if (tn<v2ntopics && v2topics2[tn]==i)
			{
				if (i) ypos+=20;
				if (xpos) ypos+=30;
        CRect rect(7,ypos+5,420,ypos+25);
				topics[tn].Create(m_hWnd,rect, v2topics[tn].name, 
																			 WS_CHILD|WS_VISIBLE, WS_EX_STATICEDGE);
				ypos+=31;
				xpos=0;
				tn++;
			}

			V2PARAM p=v2parms[i];
			if (p.ctltype!=VCTL_SKIP)
			{
        CRect rect(xpos+7,ypos+5,xpos+55,ypos+20);
				labels[i].Create(m_hWnd,rect, p.name, WS_CHILD|WS_VISIBLE);
				labels[i].SetFont(fnt);
			}

			CMultiButton *mb;
			CTrackBarCtrl *tb;
      CRect rect;
			switch (p.ctltype)
			{
			case VCTL_MB:
				mb=new CMultiButton();
        rect=CRect(xpos+60,ypos+2,xpos+285,ypos+22);
				mb->Create(m_hWnd, rect, p.ctlstr);
				parm[i]=mb;
				if (!xpos && p.ctlstr[0]=='!') {xpos=210; }
				else { xpos=0; ypos+=30; }

				break;
			case VCTL_SLIDER:
				tb=new CTrackBarCtrl();
        rect=CRect(xpos+50,ypos,xpos+200,ypos+25);
				tb->Create(m_hWnd, rect, "",WS_CHILD|WS_VISIBLE|TBS_HORZ|TBS_BOTTOM|TBS_AUTOTICKS|TBS_TOOLTIPS);
				tb->SetTicFreq((p.max+1)>>3);
				tb->SetRange(-p.offset,p.max-p.offset,TRUE);
				parm[i]=tb;
				if (!xpos) {xpos=210; }
				else { xpos=0; ypos+=30; }
				break;
			case VCTL_SKIP:
			  parm[i]=new CWindow();
				parm[i]->m_hWnd=0;
				break;
			}
		}

		SetScrollSize(430,ypos+40,TRUE);
		return 0;
	}

	LRESULT OnCommand(UINT /*uMsg*/, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
	  bHandled=0;
		for (int i=0; i<v2nparms; i++) if ((LPARAM)parm[i]->m_hWnd==lParam)
		{
			int pos=(int)wParam+v2parms[i].offset;
			::SendMessage(GetParent(),WM_COMMAND,(i<<8)|pos,(LPARAM)m_hWnd);
			bHandled=1;
		}
		return 0;
	}

	LRESULT OnHScroll(UINT /*uMsg*/, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
		bHandled=0;
		for (int i=0; i<v2nparms; i++) if ((LPARAM)parm[i]->m_hWnd==lParam)
		{
			int pos=((CTrackBarCtrl *)(parm[i]))->GetPos()+v2parms[i].offset;
			::SendMessage(GetParent(),WM_COMMAND,(i<<8)|pos,(LPARAM)m_hWnd);
			bHandled=1;
		}
		return 0;
	}

	LRESULT OnSetPos(UINT /*uMsg*/, WPARAM wParam, LPARAM lParam, BOOL& /*bHandled*/)
	{
		int n=(int)wParam;
		int v=(int)lParam;
		if (n<0 || n>=v2nparms || v<0 || v>127)
			return 0;
		::SendMessage(parm[n]->m_hWnd, TBM_SETPOS, 1, v-v2parms[n].offset);
		return 0;
	}

	LRESULT OnCtlColorStatic(UINT /*uMsg*/, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
		HDC dc((HDC)wParam);
		for (int i=0; i<v2ntopics; i++) if (lParam==(LPARAM)(topics[i].m_hWnd))
		{
			SetTextColor(dc,0xfff0f0f0);
			SetBkMode(dc,TRANSPARENT);
			return (LRESULT)(GetSysColorBrush(COLOR_3DSHADOW));
		}
		SetTextColor(dc,0xff000000);
		SetBkMode(dc,TRANSPARENT);
		return (LRESULT)(GetSysColorBrush(COLOR_BTNFACE));
	}

};

/*******************************************************************************\
*
*	Rechtes Patchedit-Panel: Modulationsansicht
*
\*******************************************************************************/

class CViruZModPanel : public CScrollWindowImpl<CViruZModPanel>
{
public:
	DECLARE_WND_CLASS_EX("fr_ViruZModPanel", CS_HREDRAW|CS_VREDRAW, COLOR_BTNFACE)

	char *ptr;
	HFONT fnt;
	CButton addbutton;
	CStatic statline;

	CButton				sources[255];
	CTrackBarCtrl values[255];
	CButton       dests[255];
	CButton       erasers[255];
	CStatic       arrows[255];

	CMenu         srcmenu, destmenu;
	int           mselno;
		

	char v2dests[256][256];

	int  v2ndests;

	BEGIN_MSG_MAP(CViruZModPanel)
		MESSAGE_HANDLER(WM_CREATE, OnCreate)
		MESSAGE_HANDLER(WM_DESTROY, OnDestroy)
		MESSAGE_HANDLER(WM_COMMAND, OnCommand)
		MESSAGE_HANDLER(WM_HSCROLL, OnHScroll)
		MESSAGE_HANDLER(VPE_SETPOINTER, OnSetPointer)
		CHAIN_MSG_MAP(CScrollWindowImpl<CViruZModPanel>)
	END_MSG_MAP()

	CViruZModPanel() : CScrollWindowImpl<CViruZModPanel>()
	{
		CString s;
		destmenu.CreatePopupMenu();
		v2ndests=0;
		int lasttn=0;
		int tn=0;
		for (int i=0; i<v2nparms; i++)
		{
			if (tn<(v2ntopics-1) && v2topics2[tn+1]==i)
				tn++;
			s=v2topics[tn].name;
			s+=" ";
			s+=v2parms[i].name;
			v2dests[i][255]=0;
			strcpy_s(v2dests[i],s);
			if (v2parms[i].isdest)
			{
				if (lasttn!=tn)
					destmenu.AppendMenu(MF_SEPARATOR);
				destmenu.AppendMenu(MF_STRING,i+256,s);
				lasttn=tn;
			}
		}

		srcmenu.CreatePopupMenu();
		for (int i=0; i<v2nsources; i++)
		{
			if (i==1 || i==8)
				srcmenu.AppendMenu(MF_SEPARATOR);
			srcmenu.AppendMenu(MF_STRING,i+512,v2sources[i]);
		}

	}

	~CViruZModPanel()
	{
	}

	void DoPaint(CDCHandle /*dc*/)
	{
	}


	LRESULT OnDestroy(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& bHandled)
	{
		bHandled=0;
		return 0;
	}

	
	void UpdateDisplay(int nr=-1)
	{
		int n=(int)ptr[0];
    int i;

		for (i=((nr==-1)?0:nr); i<((nr==-1)?n:nr+1); i++)
		{
			char *p2=ptr+1+3*i;
			sources[i].ShowWindow(1);
			arrows[i].ShowWindow(1);
			dests[i].ShowWindow(1);
			values[i].ShowWindow(1);
			erasers[i].ShowWindow(1);

			sources[i].SetWindowText(v2sources[p2[0]]);
			dests[i].SetWindowText(v2dests[p2[2]]);
			values[i].SetPos(p2[1]-64);
		}
		for (;i<((nr==-1)?255:nr+1); i++)
		{
			sources[i].ShowWindow(0);
			arrows[i].ShowWindow(0);
			dests[i].ShowWindow(0);
			values[i].ShowWindow(0);
			erasers[i].ShowWindow(0);
		}

		CString sl;
		sl.Format("%d of 255 modulations used",n);
		statline.SetWindowText(sl);

		SetScrollSize(230, 70*n+90, TRUE);
	}


	LRESULT OnCreate(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
	{
    CRect rect;
		fnt=(HFONT)GetStockObject(DEFAULT_GUI_FONT);
	
    rect=CRect(5,5,65,30);
		addbutton.Create(m_hWnd,rect,"Add",WS_VISIBLE|WS_CHILD);
		addbutton.SetFont(fnt);

    rect=CRect(80,10,220,30);
		statline.Create(m_hWnd,rect,"0 of 255 modulations used",WS_VISIBLE|WS_CHILD|SS_RIGHT);
		statline.SetFont(fnt);

		int ypos=50;
		for (int i=0; i<255; i++)
		{
      rect=CRect(5,ypos,80,ypos+22);
			sources[i].Create(m_hWnd,rect,v2sources[0],WS_CHILD,WS_EX_STATICEDGE);
			sources[i].SetFont(fnt);
      rect=CRect(90, ypos + 3, 105, ypos + 24);
			arrows[i].Create(m_hWnd,rect,"-->",WS_CHILD);
			arrows[i].SetFont(fnt);
      rect=CRect(110,ypos,220,ypos+22);
			dests[i].Create(m_hWnd,rect,v2dests[0],WS_CHILD,WS_EX_STATICEDGE);
			dests[i].SetFont(fnt);
      rect=CRect(5,ypos+25,155,ypos+50);
      values[i].Create(m_hWnd,rect, "",WS_CHILD|TBS_HORZ|TBS_BOTTOM|TBS_AUTOTICKS|TBS_TOOLTIPS);
			values[i].SetTicFreq(8);
			values[i].SetRange(-64,63);
      rect=CRect(160,ypos+27,220,ypos+45);
			erasers[i].Create(m_hWnd,rect,"Delete",WS_CHILD);
			erasers[i].SetFont(fnt);
			ypos+=70;
		}

		SetScrollSize(230,ypos+40,TRUE);
		SetScrollPos(0,0);
		return 0;
	}


	LRESULT OnCommand(UINT /*uMsg*/, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
		bHandled=0;
		HWND src=(HWND)lParam;
		int i;

		// Menucommand
		if (!lParam)
		{
			if (wParam>=0x100 && wParam<0x200) // dest select
			{
				ptr[3*mselno+1+2]=(char)(wParam-0x100);
				::SendMessage(GetParent(),VPE_EDIT,0,0);
				UpdateDisplay(mselno);
			}
			if (wParam>=0x200 && wParam<0x300) // dest select
			{
				ptr[3*mselno+1]=(char)(wParam-0x200);
				::SendMessage(GetParent(),VPE_EDIT,0,0);
				UpdateDisplay(mselno);
			}
			bHandled=1;
			return 0;
		}

		// Add-Button
		if (wParam==BN_CLICKED && src==addbutton)
		{
			int n=ptr[0];
			if (n<255)
			{
				ptr[3*n+1]=0;
				ptr[3*n+2]=64;
				ptr[3*n+3]=0;
				ptr[0]++;
				::SendMessage(GetParent(),VPE_EDIT,0,0);
				UpdateDisplay(n);
			}
			bHandled=1;
			return 0;
		}
		
		// Voice-Buttons
		CPoint p;
		GetCursorPos(&p);
		for (i=0; i<255; i++)
		{
			if (wParam==BN_CLICKED && src==sources[i])
			{
				srcmenu.TrackPopupMenu(TPM_CENTERALIGN|TPM_LEFTBUTTON,p.x,p.y-100,m_hWnd);
				mselno=i;
				bHandled=1;
				return 0;
			}	
			if (wParam==BN_CLICKED && src==dests[i])
			{
				destmenu.TrackPopupMenu(TPM_CENTERALIGN|TPM_LEFTBUTTON,p.x,p.y,m_hWnd);
				mselno=i;
				bHandled=1;
				return 0;
			}
			if (wParam==BN_CLICKED && src==erasers[i])
			{
				if (MessageBox("Really delete this modulation?","Farbrausch V.II",MB_ICONQUESTION|MB_YESNO)==IDYES)
				{
					for (int n=i; n<(ptr[0]-1); n++)
					{
						register char *p2=ptr+3*n+1;
						p2[0]=p2[3];
						p2[1]=p2[4];
						p2[2]=p2[5];
					}
					ptr[0]--;
					UpdateDisplay();
					::SendMessage(GetParent(),VPE_EDIT,0,0);
				}
			}

		}
		return 0;
	}

	LRESULT OnHScroll(UINT /*uMsg*/, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
		bHandled=0;
		for (int i=0; i<255; i++) if ((HWND)lParam==values[i])
		{
			ptr[1+3*i+1]=values[i].GetPos()+64;
			::SendMessage(GetParent(),VPE_EDIT,0,0);
			bHandled=1;
		}
		return 0;
	}

	LRESULT OnSetPointer(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled) 
	{
		ptr=(char *)lParam;
		UpdateDisplay();
		return 0;
	}

};


/*******************************************************************************\
*
*	Patchedit-Panel
*
\*******************************************************************************/

class CViruZPatchEdit : public CSplitterWindowImpl<CViruZPatchEdit>
{
public:
	DECLARE_WND_CLASS_EX(_T("fr_ViruZPatchEdit"), CS_DBLCLKS, COLOR_WINDOW)

	CViruZPanel leftpane;
	CViruZModPanel rightpane;

	char *ptr;

	BOOL PreTranslateMessage(MSG* pMsg)
	{
		pMsg;
		return FALSE;
	}

	BEGIN_MSG_MAP(CViruZPatchEdit)
		MESSAGE_HANDLER(WM_CREATE, OnCreate)
		MESSAGE_HANDLER(WM_DESTROY, OnDestroy)
		MESSAGE_HANDLER(VPE_SETPOINTER, OnSetPointer)
		MESSAGE_HANDLER(WM_COMMAND, OnCommand)
		MESSAGE_HANDLER(VPE_EDIT, OnEdit)
		CHAIN_MSG_MAP(CSplitterWindowImpl<CViruZPatchEdit>)
	END_MSG_MAP()



	LRESULT OnDestroy(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& bHandled)
	{
		bHandled=0;
		return 0;
	}

	LRESULT OnCreate(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled) 
	{
		CSplitterWindowImpl<CViruZPatchEdit>::OnCreate (uMsg, wParam, lParam, bHandled);
		
		ptr=0;

    CRect rect(0,0,430,400);
		leftpane.Create(m_hWnd, rect, "", WS_VISIBLE|WS_CHILD);
		rightpane.Create(m_hWnd, rect, "", WS_VISIBLE|WS_CHILD);

		SetSplitterPanes(leftpane,rightpane);
		SetSplitterExtendedStyle(SPLIT_NONINTERACTIVE);
		SetSplitterRect(CRect(0,0,900,0));
		SetSplitterPos(-1);

		return 0;
	}

	LRESULT OnSetPointer(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled) 
	{
		ptr=(char *)lParam;
		if (ptr)
			for (int i=0; i<v2nparms; i++)
				::SendMessage(leftpane.m_hWnd, TBM_SETPOS, i, ptr[i]);

		::SendMessage(rightpane.m_hWnd, VPE_SETPOINTER, wParam, (LPARAM)(ptr+v2nparms));
			
		return 0;
	}

	LRESULT OnCommand(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled) 
	{
		bHandled=0;
		if (lParam==(LPARAM)(leftpane.m_hWnd))
		{
			if (ptr)
				ptr[(wParam>>8)&0xff]=wParam&0xff;
			::SendMessage(GetParent(),VPE_EDIT,0,0);
			bHandled=1;
		}
		else if (lParam==(LPARAM)(rightpane.m_hWnd))
		{
		}
		return 0;
	}

	LRESULT OnEdit(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled) 
	{
		::SendMessage(GetParent(), VPE_EDIT, wParam, lParam);
		return 0;
	}
};


/*******************************************************************************\
*
*	Globals-Panel
*
\*******************************************************************************/


class CViruZGlobalsPanel : public CScrollWindowImpl<CViruZGlobalsPanel>
{
public:
	DECLARE_WND_CLASS_EX("fr_ViruZGlobalsPanel", CS_HREDRAW|CS_VREDRAW, COLOR_BTNFACE)

	CStatic *topics;
	CStatic *labels;
	CWindow **parm;

	char *ptr;
	
	BOOL PreTranslateMessage(MSG* pMsg)
	{
		pMsg;
		return FALSE;
	}

	BEGIN_MSG_MAP(CViruZGlobalsPanel)
		MESSAGE_HANDLER(WM_CREATE, OnCreate)
		MESSAGE_HANDLER(WM_DESTROY, OnDestroy)
		MESSAGE_HANDLER(WM_COMMAND, OnCommand)
		MESSAGE_HANDLER(WM_HSCROLL, OnHScroll)
		MESSAGE_HANDLER(WM_CTLCOLORSTATIC, OnCtlColorStatic)
		MESSAGE_HANDLER(VPE_SETPOINTER, OnSetPointer)
		CHAIN_MSG_MAP(CScrollWindowImpl<CViruZGlobalsPanel>)
	END_MSG_MAP()

	CViruZGlobalsPanel() : CScrollWindowImpl<CViruZGlobalsPanel>()
	{
		parm=new CWindow *[v2ngparms];
		for (int i=0; i<v2ngparms; i++)
			parm[i]=0;
		labels=new CStatic [v2ngparms];
		topics=new CStatic [v2ngtopics];
	}

	~CViruZGlobalsPanel()
	{
		delete labels;
		delete topics;
		delete parm;
	}

	void DoPaint(CDCHandle /*dc*/)
	{
	}


	LRESULT OnDestroy(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& bHandled)
	{
		bHandled=0;
		return 0;
	}

	LRESULT OnCreate(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
	{
    CRect rect;
		HFONT fnt=(HFONT)GetStockObject(DEFAULT_GUI_FONT);

		int ypos=2;
		int xpos=0;
		int tn=0;
		for (int i=0; i<v2ngparms; i++)
		{

			if (tn<v2ngtopics && v2gtopics2[tn]==i)
			{
				if (i) ypos+=20;
				if (xpos) ypos+=30;
        rect=CRect(7,ypos+5,420,ypos+25);
				topics[tn].Create(m_hWnd,rect, v2gtopics[tn].name, 
																			 WS_CHILD|WS_VISIBLE, WS_EX_STATICEDGE);
				ypos+=31;
				xpos=0;
				tn++;
			}

			V2PARAM p=v2gparms[i];
      rect=CRect(xpos+7,ypos+5,xpos+55,ypos+20);
			labels[i].Create(m_hWnd,rect, p.name, WS_CHILD|WS_VISIBLE);
			labels[i].SetFont(fnt);

			CMultiButton *mb;
			CTrackBarCtrl *tb;
			switch (p.ctltype)
			{
			case VCTL_MB:
				mb=new CMultiButton();
        rect=CRect(xpos+60,ypos+2,xpos+285,ypos+22);
				mb->Create(m_hWnd, rect, p.ctlstr);
				parm[i]=mb;
				if (!xpos && p.max<3) {xpos=210; }
				else { xpos=0; ypos+=30; }

				break;
			case VCTL_SLIDER:
				tb=new CTrackBarCtrl();
        rect=CRect(xpos+50,ypos,xpos+200,ypos+25);
				tb->Create(m_hWnd, rect, "",WS_CHILD|WS_VISIBLE|TBS_HORZ|TBS_BOTTOM|TBS_AUTOTICKS|TBS_TOOLTIPS);
				tb->SetTicFreq((p.max+1)>>3);
				tb->SetRange(-p.offset,p.max-p.offset,TRUE);
				parm[i]=tb;
				if (!xpos) {xpos=210; }
				else { xpos=0; ypos+=30; }
				break;
			}
		}

		SetScrollSize(430,ypos+40,TRUE);
		return 0;
	}

	LRESULT OnCommand(UINT /*uMsg*/, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
	  bHandled=0;
		for (int i=0; i<v2ngparms; i++) if (parm[i] && (LPARAM)parm[i]->m_hWnd==lParam)
		{
			int pos=(int)wParam+v2gparms[i].offset;
			ptr[i]=pos;
      synthSetGlobals(v2instance,ptr);
			bHandled=1;
		}
		return 0;
	}

	LRESULT OnHScroll(UINT /*uMsg*/, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
		bHandled=0;
		for (int i=0; i<v2ngparms; i++) if (parm[i] && (LPARAM)parm[i]->m_hWnd==lParam)
		{
			int pos=((CTrackBarCtrl *)(parm[i]))->GetPos()+v2gparms[i].offset;
			ptr[i]=pos;
      synthSetGlobals(v2instance,ptr);
			bHandled=1;
		}
		return 0;
	}

	LRESULT OnCtlColorStatic(UINT /*uMsg*/, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
		HDC dc((HDC)wParam);
		for (int i=0; i<v2ngtopics; i++) if (lParam==(LPARAM)(topics[i].m_hWnd))
		{
			SetTextColor(dc,0xfff0f0f0);
			SetBkMode(dc,TRANSPARENT);
			return (LRESULT)(GetSysColorBrush(COLOR_3DSHADOW));
		}
		SetTextColor(dc,0xff000000);
		SetBkMode(dc,TRANSPARENT);
		return (LRESULT)(GetSysColorBrush(COLOR_BTNFACE));
	}

	LRESULT OnSetPointer(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled) 
	{
		ptr=(char *)wParam;
    synthSetGlobals(v2instance,ptr);
		if (ptr)
			for (int i=0; i<v2ngparms; i++)
        if (parm[i])
				  parm[i]->SendMessage(TBM_SETPOS, i, ptr[i]-v2gparms[i].offset);
		return 0;
	}

};

/*******************************************************************************\
*
*	Sprachsynth-Panel
*
\*******************************************************************************/

#ifdef RONAN

class CViruZLisaPanel : public CScrollWindowImpl<CViruZLisaPanel>
{
public:
	DECLARE_WND_CLASS_EX("fr_ViruZLisaPanel", CS_HREDRAW|CS_VREDRAW, COLOR_BTNFACE)

	CStatic xl1, xl2;
	CEdit engl_in;
	CEdit phon_out;

	CStatic labels[64];
	CEdit   texts[64];

	BOOL PreTranslateMessage(MSG* pMsg)
	{
		pMsg;
		return FALSE;
	}

	BEGIN_MSG_MAP(CViruZLisaPanel)
		MESSAGE_HANDLER(WM_CREATE, OnCreate)
		MESSAGE_HANDLER(WM_DESTROY, OnDestroy)
		MESSAGE_HANDLER(WM_COMMAND, OnCommand)
		MESSAGE_HANDLER(WM_CTLCOLORSTATIC, OnCtlColorStatic)
		MESSAGE_HANDLER(VPE_SETPOINTER, OnSetPointer)
		CHAIN_MSG_MAP(CScrollWindowImpl<CViruZLisaPanel>)
	END_MSG_MAP()

	CViruZLisaPanel() : CScrollWindowImpl<CViruZLisaPanel>()
	{
	}

	~CViruZLisaPanel()
	{
	}

	void DoPaint(CDCHandle /*dc*/)
	{
	}


	LRESULT OnDestroy(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& bHandled)
	{
		bHandled=0;
		return 0;
	}

	LRESULT OnCreate(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
	{
    CRect rect;
		HFONT fnt=(HFONT)GetStockObject(DEFAULT_GUI_FONT);

		sInt ypos=0;
		sInt xpos=0;

    rect=CRect(xpos+9,ypos+8,xpos+55,ypos+27);
		xl1.Create(m_hWnd,rect,"Translate:",WS_CHILD|WS_VISIBLE);
		xl1.SetFont(fnt);

    rect=CRect(xpos+60,ypos+5,xpos+265,ypos+27);
	  engl_in.Create(m_hWnd,rect,"english",WS_CHILD|WS_VISIBLE|ES_AUTOHSCROLL|SS_CENTERIMAGE|WS_TABSTOP,WS_EX_CLIENTEDGE|WS_EX_CONTROLPARENT);
		engl_in.SetFont(fnt);
		engl_in.SetLimitText(255);

    rect=CRect(xpos+270,ypos+8,xpos+288,ypos+27);
		xl2.Create(m_hWnd,rect,"=>",WS_CHILD|WS_VISIBLE);
		xl2.SetFont(fnt);

    rect=CRect(xpos+289,ypos+5,xpos+655,ypos+27);
	  phon_out.Create(m_hWnd,rect,"phonemes",WS_CHILD|WS_VISIBLE|ES_AUTOHSCROLL|ES_READONLY|SS_CENTERIMAGE,WS_EX_STATICEDGE);
		phon_out.SetFont(fnt);
		phon_out.SetLimitText(255);

		ypos+=40;

		for (sInt i=0; i<64; i++)
		{
			CString s;
			s.Format("Text #%02d",i);
      rect=CRect(xpos+7,ypos+8,xpos+107,ypos+25);
			labels[i].Create(m_hWnd,rect,s,WS_CHILD|WS_VISIBLE);
			labels[i].SetFont(fnt);

      rect=CRect(xpos+60,ypos+5,xpos+655,ypos+25);
			texts[i].Create(m_hWnd,rect,"",WS_CHILD|WS_VISIBLE|ES_AUTOHSCROLL|WS_TABSTOP,WS_EX_CLIENTEDGE|WS_EX_CONTROLPARENT);
			texts[i].SetFont(fnt);
			texts[i].LimitText(255);

			ypos+=25;
		}

		SetScrollSize(650,ypos+40,TRUE);

		SendMessage(VPE_SETPOINTER,0,0);

		return 0;
	}

	LRESULT OnCommand(UINT /*uMsg*/, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
	  bHandled=0;
		switch (HIWORD(wParam))
		{
		case EN_CHANGE:
			if (lParam==(LPARAM)(engl_in.m_hWnd))
			{
				/*char in[256];
				char out[256];
				engl_in.GetWindowText(in,255);
				e2p_initio(in,out);
				e2p_main();
				phon_out.SetWindowText(out);*/
			}
			for (sInt i=0; i<64; i++) if (lParam==(LPARAM)(texts[i].m_hWnd))
			{
				char txt[256];
				ZeroMemory(txt,256);
				texts[i].GetWindowText(txt,255);
				memcpy(speech[i],txt,256);
			}
			break;
		}
		return 0;
	}

	LRESULT OnCtlColorStatic(UINT /*uMsg*/, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
		HDC dc((HDC)wParam);
		SetTextColor(dc,0xff000000);
		SetBkMode(dc,TRANSPARENT);
		return (LRESULT)(GetSysColorBrush(COLOR_BTNFACE));
	}

	LRESULT OnSetPointer(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled) 
	{
		for (sInt i=0; i<64; i++)
			texts[i].SetWindowText(speech[i]);
		/*
		ptr=(char *)lParam;
		synthSetGlobals(v2instance,ptr);
		if (ptr)
			for (int i=0; i<v2ngparms; i++)
				parm[i]->SendMessage(TBM_SETPOS, i, ptr[i]-v2gparms[i].offset);
				*/
		return 0;
	}

};

#endif


/*******************************************************************************\
*
*	Status
*
\*******************************************************************************/

class CViruZStatusPanel : public CWindowImpl<CViruZStatusPanel>
{
public:
	DECLARE_WND_CLASS_EX("fr_ViruZPanel", CS_HREDRAW|CS_VREDRAW, COLOR_BTNFACE)

	CStatic frlogo, v2logo, thlogo;
	HFONT sysfnt, v2fnt;

	CComboBox			patchsel, devsel;
	CButton       editbutton, comparebutton, storebutton, panicbutton;
	CStatic       patchno, chtitle, chs[16], pntitle, pnos[16], potitle, poly[16], potitle2, pototal, cpumeter;

	CProgressBarCtrl  vleft, vright;

	int clipl, clipr;

	int gpo[17], gpg[16];

	int editmode;
	int comparemode;
	int np;
	int oldcpu;

	BOOL PreTranslateMessage(MSG* pMsg)
	{
		pMsg;
		return FALSE;
	}

	BEGIN_MSG_MAP(CViruZPanel)
		MESSAGE_HANDLER(WM_CREATE, OnCreate)
		MESSAGE_HANDLER(WM_DESTROY, OnDestroy)
		MESSAGE_HANDLER(WM_COMMAND, OnCommand)
		MESSAGE_HANDLER(WM_HSCROLL, OnHScroll)
		MESSAGE_HANDLER(WM_CTLCOLORSTATIC, OnCtlColorStatic)
		MESSAGE_HANDLER(WM_TIMER, OnTimer)
		MESSAGE_HANDLER(VPE_GOTOPATCH, OnGotoPatch)
		MESSAGE_HANDLER(VPE_EDIT, OnEdit)
	END_MSG_MAP()


	LRESULT OnDestroy(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& bHandled)
	{
		DeleteObject(v2fnt);
		return 0;
	}


	void UpdateDisplay(int updcb=1)
	{
		if (updcb)
		{
			patchsel.ResetContent();
			patchsel.LimitText(32);
			for (int i=0; i<128; i++)
				patchsel.AddString(patchnames[i]);
			patchsel.SetCurSel(np);
		}

		if (!editmode) comparemode=0;
		editbutton.SetCheck(editmode);
		comparebutton.SetCheck(comparemode);
		comparebutton.EnableWindow(editmode);
		storebutton.EnableWindow(editmode);
		CString s;
		s.Format("%03d",np);
		patchno.SetWindowText(s);
	}

	void SetEditBuffer(int next)
	{
		patchoffsets[np]=4*128+v2soundsize*np;
		np=next;
		if (!editmode)
			for (int i=0; i<v2soundsize; i++)
				editmem[i]=soundmem[128*4+v2soundsize*np+i];
		::SendMessage(GetParent(),VPE_SETPOINTER,(WPARAM)globals,(LPARAM)editmem);
		patchoffsets[np]=(long)(editmem-soundmem);
		UpdateDisplay(0);
	}


	LRESULT OnCreate(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
	{
    CRect rect;
		sysfnt=(HFONT)GetStockObject(DEFAULT_GUI_FONT);

		v2fnt=CreateFont(48,0,0,0,FW_BOLD,0,0,0,DEFAULT_CHARSET,OUT_TT_PRECIS,
			CLIP_DEFAULT_PRECIS,PROOF_QUALITY,VARIABLE_PITCH|FF_DONTCARE,"Impact");

    rect = CRect(45, 0, 170, 50);
		v2logo.Create(m_hWnd, rect, "ViruZ II", WS_VISIBLE|WS_CHILD);
		v2logo.SetFont(v2fnt);

    rect = CRect(45, 82, 170, 96);
		cpumeter.Create(m_hWnd, rect, "CPU: ???%", WS_VISIBLE|WS_CHILD);
		cpumeter.SetFont(sysfnt);
		oldcpu=-1;

    rect = CRect(180, 75, 230, 96);
		comparebutton.Create(m_hWnd, rect, "Compare", WS_VISIBLE|WS_CHILD|BS_AUTOCHECKBOX|BS_PUSHLIKE);
		comparebutton.SetFont(sysfnt);
    rect = CRect(235, 75, 285, 96);
		editbutton.Create(m_hWnd, rect, "Edit", WS_VISIBLE|WS_CHILD|BS_CHECKBOX|BS_PUSHLIKE);
		editbutton.SetFont(sysfnt);
    rect = CRect(475, 75, 525, 96);
		storebutton.Create(m_hWnd, rect, "Store", WS_VISIBLE|WS_CHILD);
		storebutton.SetFont(sysfnt);

    rect = CRect(5, 5, 18, 95);
		vleft.Create(m_hWnd, rect, "left", WS_VISIBLE|WS_CHILD|PBS_SMOOTH|PBS_VERTICAL, WS_EX_STATICEDGE);
		vleft.SetRange(0,90);
		vleft.SetPos(0);

    rect = CRect(20, 5, 33, 95);
		vright.Create(m_hWnd, rect, "right", WS_VISIBLE|WS_CHILD|PBS_SMOOTH|PBS_VERTICAL, WS_EX_STATICEDGE);
		vright.SetRange(0,90);
		vright.SetPos(0);

    rect = CRect(290, 75, 315, 96);
		patchno.Create(m_hWnd, rect, "000", WS_VISIBLE|WS_CHILD|SS_CENTER|SS_CENTERIMAGE, WS_EX_STATICEDGE);
		patchno.SetFont(sysfnt);

    rect = CRect(320, 75, 470, 296);
		patchsel.Create(m_hWnd, rect, "Bla", WS_VISIBLE|WS_CHILD|CBS_DROPDOWN|WS_VSCROLL);
		patchsel.SetFont(sysfnt);

    rect = CRect(180, 5, 230, 20);
		chtitle.Create(m_hWnd, rect, "Channel #", WS_VISIBLE|WS_CHILD|SS_CENTERIMAGE);
		chtitle.SetFont(sysfnt);
    rect = CRect(180, 25, 230, 40);
		pntitle.Create(m_hWnd, rect, "Patch #", WS_VISIBLE|WS_CHILD|SS_CENTERIMAGE);
		pntitle.SetFont(sysfnt);
    rect = CRect(180, 45, 230, 60);
		potitle.Create(m_hWnd, rect, "Polyphony", WS_VISIBLE|WS_CHILD|SS_CENTERIMAGE);
		potitle.SetFont(sysfnt);
		for (int i=0; i<16; i++)
		{
			CString s;
			s.Format("%02d",i+1);
      rect = CRect(235 + 25 * i, 5, 257 + 25 * i, 20);
			chs[i].Create(m_hWnd, rect, s, WS_VISIBLE|WS_CHILD|SS_CENTERIMAGE|SS_CENTER);
			chs[i].SetFont(sysfnt);
      rect = CRect(235 + 25 * i, 25, 257 + 25 * i, 40);
			pnos[i].Create(m_hWnd, rect, "---", WS_VISIBLE|WS_CHILD|SS_CENTERIMAGE|SS_CENTER, WS_EX_STATICEDGE);
			pnos[i].SetFont(sysfnt);
      rect = CRect(235 + 25 * i, 45, 257 + 25 * i, 60);
			poly[i].Create(m_hWnd, rect, "?", WS_VISIBLE|WS_CHILD|SS_CENTERIMAGE|SS_CENTER, WS_EX_STATICEDGE);
			poly[i].SetFont(sysfnt);
		}
    rect = CRect(635, 45, 645, 60);
		potitle2.Create(m_hWnd, rect, "->", WS_VISIBLE|WS_CHILD|SS_CENTERIMAGE);
		potitle2.SetFont(sysfnt);
    rect = CRect(650, 45, 672, 60);
		pototal.Create(m_hWnd, rect, "0", WS_VISIBLE|WS_CHILD|SS_CENTERIMAGE|SS_CENTER, WS_EX_STATICEDGE);
		pototal.SetFont(sysfnt);
    rect = CRect(645, 5, 695, 30);
		panicbutton.Create(m_hWnd, rect, "PANIC", WS_VISIBLE|WS_CHILD, WS_EX_CLIENTEDGE);
		panicbutton.SetFont(sysfnt);

    rect = CRect(530, 75, 695, 196);
		devsel.Create(m_hWnd, rect, "", WS_VISIBLE|WS_CHILD|CBS_DROPDOWNLIST|WS_VSCROLL);
		devsel.SetFont(sysfnt);
		devsel.ResetContent();
		devsel.AddString("MIDI IN disabled");
		for (int i=0; i<msnumdevs; i++)
			devsel.AddString(msdevnames[i]);
		devsel.SetCurSel(msGetDevice()+1);

		clipl=clipr=0;
		for (int i=0; i<16; i++)
			gpo[i]=gpg[i]=-1;
		gpo[16]=-1;

		::SetTimer(m_hWnd,303,20,0);

		np=editmode=comparemode=0;
		UpdateDisplay();
		UpdateWindow();
		return 0;
	}

	LRESULT OnCommand(UINT /*uMsg*/, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
		CString s;
		s.Format("got cmd %d from %d\n",wParam,lParam);
		//tputDebugString(s);

		HWND src=(HWND)lParam;

		int next;
		char buf[256];
		if (src==patchsel)
		{
			switch (wParam>>16)
			{
			case CBN_SELENDOK:  // Patchwexel
				next=patchsel.GetCurSel();
			  SetEditBuffer(next);
				UpdateDisplay(0);
				return 0;
			case CBN_EDITCHANGE: // Patchumbenennung
				OutputDebugString("got EDITCHANGE\n");
				buf[255]=0;
				::SendMessage(patchsel,WM_GETTEXT,255,(LPARAM)buf);
				s.Format("-> got %s\n",buf);
				OutputDebugString(s);
				strcpy_s(patchnames[np],buf);
				patchsel.InsertString(np,buf);
				patchsel.DeleteString(np+1);
				return 0;
			}
		}
		else if (src==editbutton && wParam==BN_CLICKED)
		{
			if (!editmode)
				SendMessage(VPE_EDIT,0,0);
			else
				if (MessageBox("Discard changes?","Farbrausch V.II",MB_ICONQUESTION|MB_YESNO)==IDYES)
				{
					editmode=0;
					SetEditBuffer(np);
				}
			UpdateDisplay(0);
		}
		else if (src==comparebutton && wParam==BN_CLICKED)
		{
			if (comparebutton.GetCheck())
			{
				comparemode=1;
				patchoffsets[np]=4*128+v2soundsize*np;
			}
			else
			{
				comparemode=0;
				patchoffsets[np]=(long)(editmem-soundmem);
			}
			UpdateDisplay(0);
		}
		else if (src==storebutton && wParam==BN_CLICKED)
		{
			CString s;
			s.Format("Overwrite '%s'?",patchnames[np]);
			if (MessageBox(s,"Farbrausch V.II",MB_ICONQUESTION|MB_YESNO)==IDYES)
			{
				editmode=0;
				if (!editmode)
					for (int i=0; i<v2soundsize; i++)
						soundmem[128*4+v2soundsize*np+i]=editmem[i];
			}
			UpdateDisplay(0);
		}
		else if (src==devsel)
		{
			switch (wParam>>16)
			{
			case CBN_SELENDOK:  // Patchwexel
				msSetDevice(devsel.GetCurSel()-1);
				return 0;
			}
		}
		else if (src==panicbutton && wParam==BN_CLICKED)
		{
			msStopAudio();
			HWND ich=m_hWnd, er;
			while (er=::GetParent(ich)) ich=er;
#ifdef RONAN
			msStartAudio(ich,soundmem,globals,(const char **)speechptrs);
#else
			msStartAudio(ich,soundmem,globals);
#endif
		}

		return 0;
	}

	LRESULT OnEdit(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled) 
	{
		if (!editmode)
		{
			editmode=1;
			UpdateDisplay();
		}
		return 0;
	}

	LRESULT OnHScroll(UINT /*uMsg*/, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
		bHandled=0;
		return 0;
	}

	LRESULT OnTimer(UINT /*uMsg*/, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
		int po[17], pg[16];
		CString s;
		synthGetPoly(v2instance,po);
		synthGetPgm(v2instance,pg);
		for (int i=0; i<16; i++)
		{
			if (po[i]!=gpo[i])
			{
				s.Format("%d",po[i]);
				poly[i].SetWindowText(s);
				gpo[i]=po[i];
			}
			if (pg[i]!=gpg[i])
			{
				s.Format("%03d",pg[i]);
				pnos[i].SetWindowText(s);
				gpg[i]=pg[i];
			}
		}
		if (po[16]!=gpo[16])
		{
			s.Format("%d",po[16]);
			pototal.SetWindowText(s);
			gpo[16]=po[16];
		}

		int cpu=msGetCPUUsage();
    if (cpu!=oldcpu)
		{
			s.Format("CPU: %3d%%",cpu);
			cpumeter.SetWindowText(s);
			oldcpu=cpu;
		}

		float l,r;
		int cl, cr;
		msGetLD(&l,&r,&cl,&cr);
		if (l) l=(float)(18.0f*(log(l)/log(2.0f))); else l=-100;
		if (r) r=(float)(18.0f*(log(r)/log(2.0f))); else l=-100;
		if (cl) 
		{
			clipl=5;
			vleft.SetBarColor(0xff4080ff);
		}
		if (cr) {
			clipr=5;
			vright.SetBarColor(0xff4080ff);
		}

		vleft.SetPos(90+(int)(l));
		vright.SetPos(90+(int)(r));

		if (clipl)
		{
			clipl--;
			if (!clipl) vleft.SetBarColor(CLR_DEFAULT);
		}
		if (clipr) 
		{
			clipr--;
			if (!clipr) vright.SetBarColor(CLR_DEFAULT);
		}

		return 0;
	}

	LRESULT OnCtlColorStatic(UINT /*uMsg*/, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
		bHandled=0;
		return 0;
	}

	LRESULT OnGotoPatch(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled) 
	{
		np=(int)wParam;
		SetEditBuffer(np);
		UpdateDisplay(1);
		return 0;
	}

};



/*******************************************************************************\
*
*	Unterpanel
*
\*******************************************************************************/


class CViruZWorkspace;
typedef CWindowImpl<CViruZWorkspace> vwhorzsplitter;

class CViruZWorkspace : public vwhorzsplitter
{
public:
	DECLARE_WND_CLASS_EX(_T("fr_ViruZWorkspace"), CS_DBLCLKS, COLOR_BTNFACE)

	CTabCtrl tubby;
	HWND		 views[10];
	int      nv;

	CViruZPatchEdit patched;
	CViruZGlobalsPanel globed;
#ifdef RONAN
	CViruZLisaPanel speeched;
#endif

	char *ptr;

	BOOL PreTranslateMessage(MSG* pMsg)
	{
		pMsg;
		return FALSE;
	}

	BEGIN_MSG_MAP(CViruZWorkspace)
		MESSAGE_HANDLER(WM_CREATE, OnCreate)
		MESSAGE_HANDLER(WM_DESTROY, OnDestroy)
		MESSAGE_HANDLER(VPE_SETPOINTER, OnSetPointer)
		MESSAGE_HANDLER(VPE_EDIT, OnEdit)
		MESSAGE_HANDLER(WM_COMMAND, OnCommand)
		MESSAGE_HANDLER(WM_NOTIFY, OnNotify)
		MESSAGE_HANDLER(WM_SIZE, OnCWP)
	END_MSG_MAP()

	LRESULT OnDestroy(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& bHandled)
	{
		bHandled=0;
		return 0;
	}

	LRESULT OnCreate(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled) 
	{
    CRect rect;
		HFONT sysfnt=(HFONT)GetStockObject(DEFAULT_GUI_FONT);
		
		ptr=0;

		CRect wr;
		GetWindowRect(wr);

		int h=wr.Height();
		int w=wr.Width();

    rect = CRect(100, 100, 300, 300);
		patched.Create(m_hWnd, rect, "", WS_CHILD, WS_EX_STATICEDGE);
    rect = CRect(200, 200, 400, 400);
		globed.Create(m_hWnd, rect, "", WS_CHILD, WS_EX_STATICEDGE);
#ifdef RONAN
    rect = CRect(300, 300, 500, 500);
		speeched.Create(m_hWnd, rect, "", WS_CHILD, WS_EX_STATICEDGE);
#endif

    rect = CRect(0, 0, wr.right - 5, wr.bottom - 5);
		tubby.Create(m_hWnd, rect, "", WS_VISIBLE|WS_CHILD|TCS_FLATBUTTONS|TCS_BUTTONS|TCS_HOTTRACK);
		tubby.SetFont(sysfnt);
		
		views[0]=patched;			TCITEM bla0={ TCIF_TEXT, 0, 0, (LPSTR)"Patch Editor", 10, -1, 0};
		tubby.InsertItem(0,&bla0);
		views[1]=globed;			TCITEM bla1={ TCIF_TEXT, 0, 0, (LPSTR)"Globals", 10, -1, 0};
		tubby.InsertItem(1,&bla1);
#ifdef RONAN
		views[2]=speeched;		TCITEM bla2={ TCIF_TEXT, 0, 0, (LPSTR)"Speech", 10, -1, 0};
		tubby.InsertItem(2,&bla2);
		nv=3;
#else
		nv=2;
#endif

		patched.ShowWindow(SW_SHOW);
		return 0;
	}

	LRESULT OnSetPointer(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled) 
	{
		for (sInt i=0; i<nv; i++)
			SendMessage(views[i], VPE_SETPOINTER, wParam, lParam);


//		::SendMessage(patched.m_hWnd, VPE_SETPOINTER, wParam, lParam);
//		::SendMessage(globed.m_hWnd, VPE_SETPOINTER, lParam, wParam);
		return 0;
	}

	LRESULT OnCommand(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled) 
	{
		return 0;
	}

	LRESULT OnCWP(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled) 
	{
		CRect wr;
		GetWindowRect(wr);
		tubby.MoveWindow(0,0,wr.Width(),wr.Height());
		CRect cr(0,0,wr.Width(),wr.Height());
		tubby.AdjustRect(0,cr);
		for (int i=0; i<nv; i++)
			::MoveWindow(views[i],cr.left-4,cr.top-2,cr.right+4,cr.bottom-16,1);
		return 0;
	}

	LRESULT OnNotify(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled) 
	{
		NMHDR *nm=(LPNMHDR)lParam;
		if (nm->hwndFrom==tubby && nm->code==TCN_SELCHANGE)
		{
			int n=tubby.GetCurSel();
			ATLTRACE("sel window %d!\n",n);

			for (int i=0; i<nv; i++) if (i!=n)
				::ShowWindow(views[i], SW_HIDE);
			::ShowWindow(views[n], SW_SHOW);
		}
		return 0;
	}

	LRESULT OnEdit(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled) 
	{
		::SendMessage(GetParent(), VPE_EDIT, wParam, lParam);
		return 0;
	}
};


/*******************************************************************************\
*
*	Mainpanel
*
\*******************************************************************************/


class CViruZClientWindow;
typedef CSplitterWindowImpl<CViruZClientWindow/*, false*/> vcwhorzsplitter;

class CViruZClientWindow : public vcwhorzsplitter
{
public:
	DECLARE_WND_CLASS_EX(_T("fr_ViruZClientWin"), CS_DBLCLKS, COLOR_WINDOW)

	CViruZStatusPanel oberpane;
	CViruZWorkspace unterpane;

	char *ptr;

	BOOL PreTranslateMessage(MSG* pMsg)
	{
		pMsg;
		return FALSE;
	}

	BEGIN_MSG_MAP(CViruZClientWindow)
		MESSAGE_HANDLER(WM_CREATE, OnCreate)
		MESSAGE_HANDLER(WM_DESTROY, OnDestroy)
		MESSAGE_HANDLER(VPE_SETPOINTER, OnSetPointer)
		MESSAGE_HANDLER(VPE_EDIT, OnEdit)
		MESSAGE_HANDLER(VPE_GOTOPATCH, OnGotoPatch)
		MESSAGE_HANDLER(WM_COMMAND, OnCommand)
		CHAIN_MSG_MAP(vcwhorzsplitter)
	END_MSG_MAP()



	LRESULT OnDestroy(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& bHandled)
	{
		bHandled=0;
		return 0;
	}

	LRESULT OnCreate(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled) 
	{
		vcwhorzsplitter::OnCreate (uMsg, wParam, lParam, bHandled);
		
		ptr=0;

    CRect rect(0, 0, 430, 400);
		unterpane.Create(m_hWnd, rect, "", WS_VISIBLE|WS_CHILD);
		oberpane.Create(m_hWnd, rect, "", WS_VISIBLE|WS_CHILD);

		SetSplitterPanes(oberpane,unterpane);
		SetSplitterExtendedStyle(SPLIT_NONINTERACTIVE);
		SetSplitterRect(CRect(0,0,0,200));
		SetSplitterPos(-1);

		return 0;
	}

	LRESULT OnSetPointer(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled) 
	{
		::SendMessage(unterpane.m_hWnd, VPE_SETPOINTER, wParam, lParam);
		return 0;
	}

	LRESULT OnCommand(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled) 
	{
		return 0;
	}

	LRESULT OnEdit(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled) 
	{
		::SendMessage(oberpane.m_hWnd, VPE_EDIT, wParam, lParam);
		return 0;
	}

	LRESULT OnGotoPatch(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled) 
	{
		::SendMessage(oberpane.m_hWnd, VPE_GOTOPATCH, wParam, lParam);
		return 0;
	}

};



/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_VIRUZPANEL_H__213CE356_CC77_11D4_9322_00A0CC20D58C__INCLUDED_)
