// PasswordDialog.cpp

#include "StdAfx.h"

#include "PasswordDialog.h"

#ifdef Z7_LANG
#include "LangUtils.h"
#endif

#ifndef Z7_SFX
#include "../Common/ZipRegistry.h"
#endif

#ifdef Z7_LANG
static const UInt32 kLangIDs[] =
{
  IDT_PASSWORD_ENTER,
  IDX_PASSWORD_SHOW
};
#endif

void CPasswordDialog::ReadControls()
{
  _passwordCombo.GetText(Password);
  ShowPassword = IsButtonCheckedBool(IDX_PASSWORD_SHOW);
}

void CPasswordDialog::SetTextSpec()
{
  HWND editWindow = ::GetWindow(_passwordCombo, GW_CHILD);
  if (editWindow)
    ::SendMessage(editWindow, EM_SETPASSWORDCHAR, ShowPassword ? 0: TEXT('*'), 0);
  _passwordCombo.SetText(Password);
}

bool CPasswordDialog::OnInit()
{
  #ifdef Z7_LANG
  LangSetWindowText(*this, IDD_PASSWORD);
  LangSetDlgItems(*this, kLangIDs, Z7_ARRAY_SIZE(kLangIDs));
  #endif
  _passwordCombo.Attach(GetItem(IDC_PASSWORD_HISTORY));
  CheckButton(IDX_PASSWORD_SHOW, ShowPassword);
  SetTextSpec();

  #ifndef Z7_SFX
  UStringVector passwords;
  NExtract::Load_PasswordHistory(passwords);
  for (unsigned i = 0; i < passwords.Size(); i++)
  {
    _passwordCombo.AddString(passwords[i]);
  }
  #endif

  return CModalDialog::OnInit();
}

bool CPasswordDialog::OnMessage(UINT message, WPARAM wParam, LPARAM lParam)
{
  switch (message)
  {
    case WM_MEASUREITEM:
      if (wParam == IDC_PASSWORD_HISTORY)
      {
        OnMeasureItem((LPMEASUREITEMSTRUCT)lParam);
        return true;
      }
      break;
    case WM_DRAWITEM:
      if (wParam == IDC_PASSWORD_HISTORY)
      {
        OnDrawItem((LPDRAWITEMSTRUCT)lParam);
        return true;
      }
      break;
    case WM_CTLCOLORLISTBOX:
    {
      HWND hWnd = (HWND)lParam;
      if (_listboxHwnd != hWnd)
      {
        _listboxHwnd = hWnd;
        ::SetWindowLongPtr(_listboxHwnd, GWLP_USERDATA, (LONG_PTR)this);
        _oldListBoxProc = (WNDPROC)::SetWindowLongPtr(_listboxHwnd, GWLP_WNDPROC, (LONG_PTR)ListBoxSubclassProc);
      }
      break;
    }
    case WM_DESTROY:
    {
      if (_listboxHwnd && _oldListBoxProc)
      {
        ::SetWindowLongPtr(_listboxHwnd, GWLP_WNDPROC, (LONG_PTR)_oldListBoxProc);
        _listboxHwnd = NULL;
        _oldListBoxProc = NULL;
      }
      break;
    }
  }
  return CModalDialog::OnMessage(message, wParam, lParam);
}

void CPasswordDialog::OnMeasureItem(LPMEASUREITEMSTRUCT mi)
{
  HDC hdc = ::GetDC(_window);
  HFONT hFont = (HFONT)::SendMessage(_window, WM_GETFONT, 0, 0);
  HFONT hOldFont = (HFONT)::SelectObject(hdc, hFont);
  TEXTMETRIC tm;
  ::GetTextMetrics(hdc, &tm);
  mi->itemHeight = tm.tmHeight + 4;
  ::SelectObject(hdc, hOldFont);
  ::ReleaseDC(_window, hdc);
}

void CPasswordDialog::OnDrawItem(LPDRAWITEMSTRUCT di)
{
  if (di->itemID == (UINT)-1)
    return;

  UString s;
  _passwordCombo.GetLBText(di->itemID, s);

  COLORREF oldBkColor, oldTextColor;
  if (di->itemState & ODS_SELECTED)
  {
    oldBkColor = ::SetBkColor(di->hDC, ::GetSysColor(COLOR_HIGHLIGHT));
    oldTextColor = ::SetTextColor(di->hDC, ::GetSysColor(COLOR_HIGHLIGHTTEXT));
  }
  else
  {
    oldBkColor = ::SetBkColor(di->hDC, ::GetSysColor(COLOR_WINDOW));
    oldTextColor = ::SetTextColor(di->hDC, ::GetSysColor(COLOR_WINDOWTEXT));
  }

  ::ExtTextOutW(di->hDC, di->rcItem.left + 2, di->rcItem.top + 2, ETO_OPAQUE, &di->rcItem, s, s.Len(), NULL);

  // Draw 'x' button
  int xSize = di->rcItem.bottom - di->rcItem.top - 4;
  RECT xRect = { di->rcItem.right - xSize - 2, di->rcItem.top + 2, di->rcItem.right - 2, di->rcItem.bottom - 2 };
  
  if (di->itemState & ODS_SELECTED)
    ::SetTextColor(di->hDC, ::GetSysColor(COLOR_HIGHLIGHTTEXT));
  else
    ::SetTextColor(di->hDC, RGB(200, 0, 0));

  ::DrawText(di->hDC, TEXT("x"), 1, &xRect, DT_CENTER | DT_VCENTER | DT_SINGLELINE);

  ::SetBkColor(di->hDC, oldBkColor);
  ::SetTextColor(di->hDC, oldTextColor);

  if (di->itemState & ODS_FOCUS)
    ::DrawFocusRect(di->hDC, &di->rcItem);
}

LRESULT CALLBACK CPasswordDialog::ListBoxSubclassProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
  CPasswordDialog *p = (CPasswordDialog *)::GetWindowLongPtr(hWnd, GWLP_USERDATA);
  if (uMsg == WM_LBUTTONDOWN)
  {
    int x = (short)LOWORD(lParam);
    int y = (short)HIWORD(lParam);
    
    // Check if 'x' was clicked
    LRESULT res = ::SendMessage(hWnd, LB_ITEMFROMPOINT, 0, lParam);
    if (HIWORD(res) == 0)
    {
      int itemIndex = LOWORD(res);
      RECT itemRect;
      ::SendMessage(hWnd, LB_GETITEMRECT, itemIndex, (LPARAM)&itemRect);
      int xSize = itemRect.bottom - itemRect.top - 4;
      if (x >= itemRect.right - xSize - 10) // Give a bit more space for clicking
      {
        p->OnListBoxClick(x, y);
        return 0; // Don't let the listbox handle the click
      }
    }
  }
  return ::CallWindowProc(p->_oldListBoxProc, hWnd, uMsg, wParam, lParam);
}

void CPasswordDialog::OnListBoxClick(int x, int y)
{
  LPARAM lParam = MAKELPARAM(x, y);
  LRESULT res = ::SendMessage(_listboxHwnd, LB_ITEMFROMPOINT, 0, lParam);
  if (HIWORD(res) == 0)
  {
    int itemIndex = LOWORD(res);
    UString s;
    _passwordCombo.GetLBText(itemIndex, s);
    #ifndef Z7_SFX
    NExtract::Delete_PasswordHistory(s);
    #endif
    _passwordCombo.SendMsg(CB_DELETESTRING, (WPARAM)itemIndex, 0);
    
    // If the list is empty, close the dropdown
    if (_passwordCombo.GetCount() == 0)
      _passwordCombo.ShowDropDown(false);
  }
}

bool CPasswordDialog::OnButtonClicked(unsigned buttonID, HWND buttonHWND)
{
  if (buttonID == IDX_PASSWORD_SHOW)
  {
    ReadControls();
    SetTextSpec();
    return true;
  }
  return CDialog::OnButtonClicked(buttonID, buttonHWND);
}

void CPasswordDialog::OnOK()
{
  ReadControls();

  #ifndef Z7_SFX
  if (!Password.IsEmpty())
  {
    NExtract::Save_PasswordHistory(Password);
  }
  #endif

  CModalDialog::OnOK();
}
