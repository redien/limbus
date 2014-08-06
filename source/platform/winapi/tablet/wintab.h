
/*
 Limbus - A cross-platform multimedia library.
 Written in 2014 by Jesper Oskarsson jesosk@gmail.com

 To the extent possible under law, the author(s) have dedicated all copyright
 and related and neighboring rights to this software to the public domain worldwide.
 This software is distributed without any warranty.

 You should have received a copy of the CC0 Public Domain Dedication along with this software.
 If not, see <http://creativecommons.org/publicdomain/zero/1.0/>.
*/

DECLARE_HANDLE( HCTX );

#define PK_BUTTONS 0x40
#define PK_X 0x80
#define PK_Y 0x100
#define PK_NORMAL_PRESSURE 0x400

#define WT_PACKET 0x7FF0
#define WT_PROXIMITY 0x7FF5

#define WTI_DEFCONTEXT 3

#define CXO_SYSTEM 0x1
#define CXO_MESSAGES 0x4

typedef struct PACKET_
{
	DWORD pkButtons;
	LONG pkX;
	LONG pkY;
	UINT pkNormalPressure;
} PACKET;

#define PACKETDATA (PK_X | PK_Y | PK_BUTTONS | PK_NORMAL_PRESSURE)
#define PACKETMODE PK_BUTTONS

typedef struct LOGCONTEXT_
{
	char lcName[40];
	UINT lcOptions;
	UINT lcStatus;
	UINT lcLocks;
	UINT lcMsgBase;
	UINT lcDevice;
	UINT lcPktRate;
	DWORD lcPktData;
	DWORD lcPktMode;
	DWORD lcMoveMask;
	DWORD lcBtnDnMask;
	DWORD lcBtnUpMask;
	LONG lcInOrgX;
	LONG lcInOrgY;
	LONG lcInOrgZ;
	LONG lcInExtX;
	LONG lcInExtY;
	LONG lcInExtZ;
	LONG lcOutOrgX;
	LONG lcOutOrgY;
	LONG lcOutOrgZ;
	LONG lcOutExtX;
	LONG lcOutExtY;
	LONG lcOutExtZ;
	DWORD lcSensX;
	DWORD lcSensY;
	DWORD lcSensZ;
	BOOL lcSysMode;
	int lcSysOrgX;
	int lcSysOrgY;
	int lcSysExtX;
	int lcSysExtY;
	DWORD lcSysSensX;
	DWORD lcSysSensY;
} LOGCONTEXT;

#define WTInfoOrdinal 20
typedef UINT (WINAPI *WTInfoProc)( UINT, UINT, LPVOID );
#define WTOpenOrdinal 21
typedef HCTX (WINAPI *WTOpenProc)( HWND, LOGCONTEXT*, BOOL );
#define WTCloseOrdinal 22
typedef BOOL (WINAPI *WTCloseProc)( HCTX );
#define WTPacketsGetOrdinal 23
typedef int (WINAPI *WTPacketsGetProc)( HCTX, int, LPVOID );
#define WTPacketOrdinal 24
typedef BOOL (WINAPI *WTPacketProc)( HCTX, UINT, LPVOID );
#define WTEnableOrdinal 40
typedef BOOL (WINAPI *WTEnableProc)( HCTX, BOOL );
#define WTOverlapOrdinal 41
typedef BOOL (WINAPI *WTOverlapProc)( HCTX, BOOL );
#define WTQueueSizeSetOrdinal 85
typedef BOOL (WINAPI *WTQueueSizeSetProc)( HCTX, int );
