// PasswordDialog.h

#ifndef ZIP7_INC_PASSWORD_DIALOG_H
#define ZIP7_INC_PASSWORD_DIALOG_H

#include "../../../Windows/Control/Dialog.h"
#include "../../../Windows/Control/ComboBox.h"

#include "PasswordDialogRes.h"

class CPasswordDialog: public NWindows::NControl::CModalDialog
{
  NWindows::NControl::CComboBox _passwordCombo;
  HWND _listboxHwnd;
  WNDPROC _oldListBoxProc;

  virtual void OnOK() Z7_override;
  virtual bool OnInit() Z7_override;
  virtual bool OnButtonClicked(unsigned buttonID, HWND buttonHWND) Z7_override;
  virtual bool OnMessage(UINT message, WPARAM wParam, LPARAM lParam) Z7_override;

  void SetTextSpec();
  void ReadControls();

  static LRESULT CALLBACK ListBoxSubclassProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
  void OnDrawItem(LPDRAWITEMSTRUCT di);
  void OnMeasureItem(LPMEASUREITEMSTRUCT mi);
  void OnListBoxClick(int x, int y);

public:
  UString Password;
  bool ShowPassword;
  
  CPasswordDialog(): _listboxHwnd(NULL), _oldListBoxProc(NULL), ShowPassword(false) {}
  INT_PTR Create(HWND parentWindow = NULL) { return CModalDialog::Create(IDD_PASSWORD, parentWindow); }
};

#endif
