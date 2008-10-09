#include <windows.h>
#include "resource.h"

extern "C" {
	#include "lua.h"
	#include "lauxlib.h"
	#include "lualib.h"
}

//------------------------------------------------------------------------------
struct Rect : public RECT {
	Rect() {
		left = top = right = bottom = 0;
	}
	Rect(int Left, int Top, int Right, int Bottom) {
		left   = Left;
		top    = Top;
		right  = Right;
		bottom = Bottom;
	}
	Rect(const Rect &rc) {
		left   = rc.left;
		top    = rc.top;
		right  = rc.right;
		bottom = rc.bottom;
	}
	Rect& operator = (const Rect &rc) {
		if (this != &rc) {
			left   = rc.left;
			top    = rc.top;
			right  = rc.right;
			bottom = rc.bottom;
		}
		return *this;
	}
	Rect& operator = (const Rect *rc) {
		if (this != rc) {
			left   = rc->left;
			top    = rc->top;
			right  = rc->right;
			bottom = rc->bottom;
		}
		return *this;
	}
	int width() const
		{ return right - left; }
	int height() const
		{ return bottom - top; }
	void GetWindowRect(HWND hwnd) {
			::GetWindowRect(hwnd, this);
		}
	void GetWindowRect(int ctrlID, HWND parent) {
			::GetWindowRect(::GetDlgItem(parent, ctrlID), this);
		}
	void GetClientRect(HWND hwnd) {
			::GetClientRect(hwnd, this);
		}
	void MapPoints(HWND hwnd) {
			::MapWindowPoints(0, hwnd, static_cast<POINT*>(static_cast<void*>(this)), 2);
		}
	void MoveWindow(HWND hwnd, int left, int top, bool repaint=false) {
			::MoveWindow(hwnd, left, top, width(), height(), repaint? 1:0);
		}
	void MoveWindow(int ctrlID, HWND parent, int left, int top, bool repaint=false) {
			::MoveWindow(GetDlgItem(parent, ctrlID), left, top, width(), height(), repaint? 1:0);
		}
	void AdjustWindowRect(HWND hwnd, bool hasMenu=false) {
			::AdjustWindowRect(this, GetWindowLongPtr(hwnd, GWL_STYLE) & ~WS_OVERLAPPED, hasMenu? 1:0);
		}
};

//------------------------------------------------------------------------------


int GetClientWidth(HWND hwnd)
{
	Rect rc;
	::GetClientRect(hwnd, &rc);
	return rc.width();
}

int GetClientWidth(int ctrlID, HWND parent)
{
	Rect rc;
	::GetClientRect(::GetDlgItem(parent, ctrlID), &rc);
	return rc.width();
}

int GetWindowWidth(HWND hwnd)
{
	Rect rc;
	::GetWindowRect(hwnd, &rc);
	return rc.width();
}

int GetWindowWidth(int ctrlID, HWND parent)
{
	Rect rc;
	::GetWindowRect(::GetDlgItem(parent, ctrlID), &rc);
	return rc.width();
}

int GetWindowHeight(int ctrlID, HWND parent)
{
	Rect rc;
	::GetWindowRect(::GetDlgItem(parent, ctrlID), &rc);
	return rc.height();
}

//------------------------------------------------------------------------------
class InputBox {
	typedef struct {
		InputBox *self;
		bool isFirst;
		Rect rc;
	} DlgData;

	enum { MAX_SHORT_STRING=64, MAX_MIDDLE_STRING=256, MAX_LONG_STRING=1024 };

public:
	InputBox(const char *Caption, const char *Prompt, const char *Value,
			int CharMinCount, const char *onkey, lua_State* L);
	~InputBox();
	int  ShowModal();
	const char *Text() const;

private:

	InputBox();

	int  PrepareTextOut(HWND hdlg);
	BOOL OutText(HDC hdc);
	void PrepareEdit(HWND hdlg);
	void Layout(HWND hdlg);
	void AdjustWidth(int ctrlID, HWND hdlg, int width);
	void AdjustWidth(int ctrlID, HWND hdlg);
	void AdjustDlg(HWND hdlg);
	void MoveY(int ctrlID, HWND hdlg, int dy);
	void CenterButtons(HWND hdlg);
	void CalcDlgMinWidth(HWND hdlg);
	bool OnKeyHandler(char ch);
	static BOOL CALLBACK EditHandler(HWND, UINT, WPARAM, LPARAM);
	static BOOL CALLBACK DlgHandler(HWND, UINT, WPARAM, LPARAM);

private:

	DlgData data;

	char editText[MAX_MIDDLE_STRING];  // �������� ������������� �����
	char caption[MAX_SHORT_STRING];    // ��������� ���� �������
	char prompt[MAX_LONG_STRING];      // ������������� ������� ��� ����� �����
	char onKey[MAX_SHORT_STRING];

	int marginX;       // ���. ������ �� ���� ����
	int marginY;       // ����. ������ �� ���� ����
	int spacing;       // ����. �������� ����� ����� ����� � ��������
	int btnSpacing;    // �����. �������� ����� ��������
	int stcDy;
	int charMinCount;  // ������ ���� ����� � ����������� ��������
	int minWidth;      // ���. ������ ���� (� ��������)
	HICON smallIcon;
	HICON bigIcon;
	lua_State *luaState;
};

//------------------------------------------------------------------------------
// ��������� �����. ������ ������ �������� � �������� �� ���-�� ���� ��������
//------------------------------------------------------------------------------
int CalcAverWidth(HWND hwnd, int charCount)
{
	// ������ = (����������� ������ �������) * (���. ���-�� ��������)
	TEXTMETRIC mtr;
	HDC hdc = GetDC(hwnd);
	SelectObject(hdc, GetStockObject(DEFAULT_GUI_FONT));
	GetTextMetrics(hdc, &mtr);
	ReleaseDC(hwnd, hdc);
	return mtr.tmAveCharWidth*charCount;
}

//------------------------------------------------------------------------------
// ��������� ����������� �� ���. ������ ����
//------------------------------------------------------------------------------
void InputBox::CalcDlgMinWidth(HWND hdlg)
{
	int stc = GetWindowWidth(IDC_PROMPTTEXT, hdlg);
	int edc = GetWindowWidth(IDC_EDITTEXT, hdlg);
	int btn = GetWindowWidth(IDOK, hdlg);
	
	minWidth = CalcAverWidth(GetDlgItem(hdlg, IDC_EDITTEXT), charMinCount);
	minWidth = max(minWidth, max(stc, edc));
	minWidth = max(minWidth, 2*btn + btnSpacing);

	Rect rc;
	rc.GetClientRect(hdlg);
	rc.right = minWidth + 2*marginX;
	rc.AdjustWindowRect(hdlg);
	minWidth = rc.width();
}

//------------------------------------------------------------------------------
// �������� ������ �������� �� ��������
//------------------------------------------------------------------------------
void InputBox::AdjustWidth(int ctrlID, HWND hdlg, int width)
{
	HWND hctrl = GetDlgItem(hdlg, ctrlID);
	Rect rc;
	rc.GetWindowRect(hctrl);
	rc.MapPoints(hdlg);
	SetWindowPos(hctrl, 0, 0, 0, width, rc.height(), SWP_NOMOVE|SWP_NOZORDER);
}

//------------------------------------------------------------------------------
// ��������� ������ �������� ��� ������ ���� �������
//------------------------------------------------------------------------------
void InputBox::AdjustWidth(int ctrlID, HWND hdlg)
{
	Rect rc, rcCtrl;
	rc.GetClientRect(hdlg);
	HWND hctrl = GetDlgItem(hdlg, ctrlID);
	rcCtrl.GetWindowRect(hctrl);
	rcCtrl.MapPoints(hdlg);
	SetWindowPos(hctrl, 0, marginX, rcCtrl.top,
		rc.width() - 2*marginX, rcCtrl.height(), SWP_NOZORDER);
}

//------------------------------------------------------------------------------
// ��������� ������� ���� ������� � ��� ��������� ���� � �����
//------------------------------------------------------------------------------
void InputBox::AdjustDlg(HWND hdlg)
{
	Rect rcStc, rcEdc, rcOk;
	rcStc.GetWindowRect(IDC_PROMPTTEXT, hdlg);
	rcEdc.GetWindowRect(IDC_EDITTEXT, hdlg);
	rcOk.GetWindowRect(IDOK, hdlg);
	rcOk.MapPoints(hdlg);

	// ��������� ���. ������ ���� �������
	CalcDlgMinWidth(hdlg);

	// ����������� ������� ���� �������
	Rect rc(0, 0, minWidth, rcOk.bottom + marginY);
	rc.AdjustWindowRect(hdlg);
	SetWindowPos(hdlg, 0, 0, 0, rc.width(), rc.height(), SWP_FRAMECHANGED|SWP_NOMOVE);
	minWidth = GetWindowWidth(hdlg);

	// ��������� ������ ��������� ��� ������ ����
	AdjustWidth(IDC_PROMPTTEXT, hdlg);
	AdjustWidth(IDC_EDITTEXT, hdlg);
}

//------------------------------------------------------------------------------
// ������� �������� �� ������
//------------------------------------------------------------------------------
void InputBox::MoveY(int ctrlID, HWND hdlg, int y)
{
	Rect rc;
	rc.GetWindowRect(ctrlID, hdlg);
	rc.MapPoints(hdlg);
	rc.MoveWindow(ctrlID, hdlg, rc.left, y, true);
}

//------------------------------------------------------------------------------
// ���������� ������ �� �����������, ������ �� ������� �������
//------------------------------------------------------------------------------
void InputBox::CenterButtons(HWND hdlg)
{
	// ������ Ok ��� ��������� ������ ���������� �����.
	// ������� � ��������� �� ��������� ������� �� ������� ��� ������ Ok

	HWND hOk = GetDlgItem(hdlg, IDOK);
	HWND hCancel = GetDlgItem(hdlg, IDCANCEL);
	Rect rcOk;
	rcOk.GetWindowRect(hOk);
	rcOk.MapPoints(hdlg);

	// ��������� ��������� ��������
	int okX = (GetClientWidth(hdlg) - 2*rcOk.width() - btnSpacing)/2;
	int cancelX = okX + rcOk.width() + btnSpacing;

	// ���������� ��� � ��������
	rcOk.MoveWindow(hOk, okX, rcOk.top, true);
	rcOk.MoveWindow(hCancel, cancelX, rcOk.top, true);
}

//------------------------------------------------------------------------------
// ������� static ��� ������ �������������� ������.
//------------------------------------------------------------------------------
int InputBox::PrepareTextOut(HWND hdlg)
{
	if (!prompt || ! *prompt)
		return 0;

	HWND hctrl = GetDlgItem(hdlg, IDC_PROMPTTEXT);

	SIZE maxSize = { 0 };
	HDC hdc = GetDC(hctrl);
	SelectObject(hdc, GetStockObject(DEFAULT_GUI_FONT));
	GetTextExtentPoint32 (hdc, prompt, lstrlen(prompt), &maxSize);
	maxSize.cx = 0;

	SIZE size = { 0 };
	int lnCount = 1;
	const char *prev = prompt;
	const char *p = prompt;

	for (; *p; ++p) {
		if (*p == '\r' || *p == '\n') {
			GetTextExtentPoint32 (hdc, prev, p-prev, &size);
			maxSize.cx = max(size.cx, maxSize.cx);
			prev = p + 1;
			lnCount++;
		}
	}

	if (prev != p) {
		GetTextExtentPoint32 (hdc, prev, p-prev, &size);
		maxSize.cx = max(size.cx, maxSize.cx);
	}

	ReleaseDC(hdlg, hdc);

	// �������� ������ ������� � ������������� � 1.1.
	int ext = maxSize.cy/10;
	stcDy = maxSize.cy + ext;
	maxSize.cy = maxSize.cy*lnCount + (lnCount-1)*ext;

	// ������������� ������� �������
	SetWindowPos(hctrl, 0, 0, 0, maxSize.cx, maxSize.cy, SWP_NOMOVE);

	// ������ ������ �������� ���� �������������� ��������
	return maxSize.cy + 4*ext;
}

//------------------------------------------------------------------------------
// ������� ����� �������
//------------------------------------------------------------------------------
BOOL InputBox::OutText(HDC hdc)
{
	SelectObject(hdc, GetStockObject(DEFAULT_GUI_FONT));
	SelectObject(hdc, reinterpret_cast<HBRUSH>(COLOR_BTNFACE+1));
	SetBkMode(hdc, TRANSPARENT);
	const char *prev = prompt;
	const char *p = prompt;
	int y = 0;

	for (; *p; ++p) {
		if (*p == '\r' || *p == '\n') {
			TextOut(hdc, 0, y, prev, p-prev);
			prev = p + 1;
			y += stcDy;
		}
	}
	if (prev != p)
		TextOut(hdc, 0, y, prev, p-prev);

	return reinterpret_cast<BOOL>(GetStockObject(NULL_BRUSH));
}

//------------------------------------------------------------------------------
// ������� edit ��� ����� ������.
//------------------------------------------------------------------------------
void InputBox::PrepareEdit(HWND hdlg)
{
	HWND hctrl = GetDlgItem(hdlg, IDC_EDITTEXT);

	HDC hdc = GetDC(hctrl);
	SelectObject(hdc, GetStockObject(DEFAULT_GUI_FONT));

	SIZE size = { 0 };
	Rect rc;
	rc.GetClientRect(hctrl);
	rc.right = 0;
		
	int len = lstrlen(editText);
	if (len) {
		GetTextExtentPoint32 (hdc, editText, len, &size);
		if (rc.right < size.cx)
			rc.right = size.cx;
	}
	
	TEXTMETRIC tm = { 0 };
	GetTextMetrics(hdc, &tm);
	if (rc.height() < tm.tmHeight)
		rc.bottom = rc.top + tm.tmHeight;

	rc.AdjustWindowRect(hctrl);
	SetWindowPos(hctrl, 0, 0, 0, rc.width(), rc.height(), SWP_NOZORDER|SWP_NOMOVE);

	ReleaseDC(hdlg, hdc);
}

//------------------------------------------------------------------------------
// ��������� �������� �������
//------------------------------------------------------------------------------
void InputBox::Layout(HWND hdlg)
{
	// ������� ��������� �������
	SetWindowText(hdlg, caption);

	// ���������� �������
	int editY =  marginY + PrepareTextOut(hdlg);

	// ���������� ���� �����
	PrepareEdit(hdlg);

	// ������������� �������� �� ���������
	MoveY(IDC_PROMPTTEXT, hdlg, marginY);
	MoveY(IDC_EDITTEXT, hdlg, editY);
	editY += GetWindowHeight(IDC_EDITTEXT, hdlg) + spacing;
	MoveY(IDOK, hdlg, editY);
	MoveY(IDCANCEL, hdlg, editY);

	// ������������� �������� ��������
	AdjustDlg(hdlg);

	// ���������� ������
	CenterButtons(hdlg);
}

//------------------------------------------------------------------------------
// ������� ����������������� ����������� �������� �������
//------------------------------------------------------------------------------
bool InputBox::OnKeyHandler(char ch)
{
	if (*onKey) {
		char str[2] = { ch, '\0' };
		lua_getfield(luaState, LUA_GLOBALSINDEX, onKey);
		lua_pushstring(luaState, str);
		lua_pcall(luaState, 1, 1, 0);
		bool res = lua_toboolean(luaState, -1) != 0;
		lua_pop(luaState, 1);
		return res;
	}
	return true;
}

//------------------------------------------------------------------------------
typedef LRESULT (CALLBACK * WNDPROC)(HWND, UINT, WPARAM, LPARAM);
WNDPROC originalEditHandler = 0;

//------------------------------------------------------------------------------
// ���������� ��������� ���� �����
//------------------------------------------------------------------------------
BOOL CALLBACK InputBox::EditHandler(HWND hctrl, UINT msg, WPARAM wParam, LPARAM lParam)
{
	InputBox *self = reinterpret_cast<InputBox*>(GetWindowLongPtr(hctrl, GWLP_USERDATA));

	switch (msg) {
		case WM_CHAR:
			if (!self->OnKeyHandler(static_cast<char>(wParam)))
				return 0;
	}
	return CallWindowProc(originalEditHandler, hctrl, msg, wParam, lParam);
}

//------------------------------------------------------------------------------
// ���������� ��������� ���� �������
//------------------------------------------------------------------------------
BOOL CALLBACK InputBox::DlgHandler(HWND hdlg, UINT msg, WPARAM wParam, LPARAM lParam)
{
	if (msg == WM_INITDIALOG) {

		// ������ ���� �������� ���� � �������� ���� InputBox, ��������� ���
		InputBox *self = reinterpret_cast<InputBox*>(lParam);
		self->data.self = self;
		self->data.isFirst = true;
		SetWindowLongPtr(hdlg, DWLP_USER, reinterpret_cast<LONG_PTR>(&self->data));

		// ������ �������� ���� ����� � ����� ��������� ������������ �������
		HWND hedit = GetDlgItem(hdlg, IDC_EDITTEXT);
		originalEditHandler = reinterpret_cast<WNDPROC>(
			SetWindowLongPtr(hedit, GWLP_WNDPROC, reinterpret_cast<LONG_PTR>(EditHandler))
		);
		SetWindowLongPtr(hedit, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(self));

		WPARAM font = reinterpret_cast<WPARAM>(GetStockObject(DEFAULT_GUI_FONT));
		SendMessage(GetDlgItem(hdlg, IDC_PROMPTTEXT), WM_SETFONT, font, FALSE);
		SendMessage(hedit, WM_SETFONT, font, FALSE);

		// ��������� � ���� �������� �������
		self->Layout(hdlg);

		// �������� ����� � ���� �����
		SetDlgItemText(hdlg, IDC_EDITTEXT, self->editText);

		// ���������� ������ �� ������
		Rect rcDt, rcDlg;
		rcDt.GetWindowRect(GetDesktopWindow());
		rcDlg.GetWindowRect(hdlg);
		rcDlg.MoveWindow(hdlg, rcDt.left + (rcDt.width() - rcDlg.width())/2,
			rcDt.top + (rcDt.height() - rcDlg.height())/2);

		return TRUE;

	} else {

		DlgData  *data = reinterpret_cast<DlgData*>(GetWindowLongPtr(hdlg, DWLP_USER));
		InputBox *self = 0;
		Rect     *strc = 0;

		if (data) {
			self = data->self;
			strc = &data->rc;
		}

		switch (msg) {
			case WM_COMMAND:
			{
				switch (LOWORD(wParam)) {
					case IDOK:
						if (!GetDlgItemText(hdlg, IDC_EDITTEXT, self->editText, MAX_MIDDLE_STRING)) {
							self->editText[0] = '\0';
						}
					case IDCANCEL:
						EndDialog(hdlg, wParam);
						return TRUE;
				}
				break;
			}

			case WM_GETICON:
				// ���������� ����������� ����� ������ ������ �����������
				switch (wParam) {
					case ICON_BIG:
						SetWindowLongPtr(hdlg, DWL_MSGRESULT, reinterpret_cast<LONG_PTR>(self->bigIcon));
						break;
					case ICON_SMALL:
					case 2 /*ICON_SMALL2*/:
						SetWindowLongPtr(hdlg, DWL_MSGRESULT, reinterpret_cast<LONG_PTR>(self->smallIcon));
						break;
					default:
						return FALSE;
				}
				return TRUE;

			case WM_CTLCOLORSTATIC:
			{
				// ������� �������
				HWND hctrl = reinterpret_cast<HWND>(lParam);
				if (hctrl == GetDlgItem(hdlg, IDC_PROMPTTEXT)) {
					// ������ ���������� NULL_BRUSH, ����� ������� �� ���������
					return self->OutText(reinterpret_cast<HDC>(wParam));
				}
				break;
			}

			case WM_SIZING:
			{
				// �������� ������������ � ������������� �������� �������� ������ ����

				Rect *rc = reinterpret_cast<Rect*>(lParam);

				if (!data->isFirst) {
					switch (wParam) {
						case WMSZ_BOTTOM:
						case WMSZ_BOTTOMLEFT:
						case WMSZ_BOTTOMRIGHT:
							if (rc->bottom != strc->bottom)
								rc->bottom = strc->bottom;
							break;
						case WMSZ_TOP:
						case WMSZ_TOPLEFT:
						case WMSZ_TOPRIGHT:
							if (rc->top != strc->top)
								rc->top = strc->top;
							break;
					}

					int newWidth = rc->width();

					switch (wParam) {
						case WMSZ_RIGHT:
						case WMSZ_TOPRIGHT:
						case WMSZ_BOTTOMRIGHT:
						{
							if (newWidth < self->minWidth) {
								newWidth = self->minWidth;
								rc->right = rc->left + self->minWidth;
							}
						}
						case WMSZ_LEFT:
						case WMSZ_TOPLEFT:
						case WMSZ_BOTTOMLEFT:
						{
							if (newWidth < self->minWidth) {
								newWidth = self->minWidth;
								rc->left = rc->right - self->minWidth;
							}
						}
					}

					*strc = *rc;
					return TRUE;
				}
				break;
			}

			case WM_SIZE:
			{
				// ��������� ������� � ������� ��������� ��� ���������

				int dx = strc->width();
				GetWindowRect(hdlg, strc);
				dx = strc->width() - dx;

				if (data->isFirst) {
					data->isFirst = false;
				} else if (wParam == SIZE_RESTORED) {
					self->AdjustWidth(IDC_PROMPTTEXT, hdlg);
					self->AdjustWidth(IDC_EDITTEXT, hdlg);
					self->CenterButtons(hdlg);
				}
				return TRUE;
			}

			case WM_MOVE:
			{
				// �������� ����� ������� ����
				strc->GetWindowRect(hdlg);
				return TRUE;
			}
		}
	}
	return FALSE;
}

//------------------------------------------------------------------------------
InputBox::InputBox(const char *Caption, const char *Prompt, const char *Value,
	int CharMinCount, const char *onkey, lua_State* L)
	:
	marginX      (10),
	marginY      (10),
	spacing      (15),
	btnSpacing   (10),
	stcDy        (0),
	charMinCount (CharMinCount),
	luaState     (L),
	minWidth     (0)
{
	lstrcpyn(editText, Value, sizeof(editText));
	lstrcpyn(caption, Caption, sizeof(caption));
	lstrcpyn(prompt, Prompt, sizeof(prompt));
	lstrcpyn(onKey, onkey, sizeof(onKey));

	smallIcon = static_cast<HICON>(
		LoadImage(GetModuleHandle(0), "SCITE", IMAGE_ICON,
			GetSystemMetrics(SM_CXSMICON), GetSystemMetrics(SM_CYSMICON),
			LR_DEFAULTCOLOR));
	bigIcon = static_cast<HICON>(
		LoadImage(GetModuleHandle(0), "SCITE", IMAGE_ICON,
			48, 48, LR_DEFAULTCOLOR));
}

//------------------------------------------------------------------------------
InputBox::~InputBox()
{
	DestroyIcon(smallIcon);
	DestroyIcon(bigIcon);
}

//------------------------------------------------------------------------------
const char *InputBox::Text() const
{
	return editText;
}

//------------------------------------------------------------------------------
int InputBox::ShowModal()
{
	int result = DialogBoxParam(GetModuleHandle("shell.dll"), "IBOX_DLG",
		GetDesktopWindow(), DlgHandler, reinterpret_cast<LPARAM>(this));
	if (result == -1) {
		// ������-��, ��� ��������, ��� ��������� �����-�� ������,
		// �� �� ������� ���, ��� �� ��: ����� �� ������ Cancel
		return IDCANCEL;
	}
	return result;
}

//------------------------------------------------------------------------------
// ���������:
//   caption: ����� ��������� ����
//   prompt:  ������� ��� ����� ����� (������ ������ ���������� '\r' ��� '\n')
//   value:   �����, ���������� � ���� �����
//   onkey:   ���������� �������� ��������; �������� ������, ������ �������
//            false, ����� �������� ����� �������, ����� - true
//   width:   ������ ���� ����� � �������� (����������� ��������)
// ���������:
//   �������� ������, ���� ������ Ok, ��� nil, ���� - Cancel
//
// �������� �� ���������:
//   caption = "InputBox"
//   prompt  = "Enter"
//   value   = ""
//   onkey   =  nil
//   width   =  20
//
extern int showinputbox(lua_State* L)
{
	int num = lua_gettop(L);
	const char *caption = luaL_optstring(L, 1, "InputBox");
	const char *prompt = num > 1? luaL_optstring(L, 2, "Enter") : "Enter";
	const char *value  = num > 2? luaL_optstring(L, 3, "") : "";
	const char *onkey  = num > 3? luaL_optstring(L, 4, "") : "";
	int         width  = num > 4? luaL_optint(L, 5, 20) : 20;

	InputBox dlg(caption, prompt, value, width, onkey, L);
	bool res = dlg.ShowModal() == IDOK;
	lua_pushboolean(L, res);

	if (res)
		lua_pushstring(L, dlg.Text());
	else
		lua_pushnil(L);

	return 1;
}