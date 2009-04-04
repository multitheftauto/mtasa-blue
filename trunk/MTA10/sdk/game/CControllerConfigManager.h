/*****************************************************************************
*
*  PROJECT:		Multi Theft Auto v1.0
*  LICENSE:		See LICENSE in the top level directory
*  FILE:		sdk/game/CControllerConfigManager.h
*  PURPOSE:		Controller configuration manager interface
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#ifndef __CGAME_CONTROLLER_CONFIG_MANAGER
#define __CGAME_CONTROLLER_CONFIG_MANAGER

// FROM VC, unlikely that they were changed though
#define GTA_KEY_NO_MOUSE_BUTTON	0	
#define GTA_KEY_LMOUSE			1
#define GTA_KEY_MMOUSE			2
#define GTA_KEY_RMOUSE			3
#define GTA_KEY_MSCROLLUP		4
#define GTA_KEY_MSCROLLDOWN		5
#define GTA_KEY_MXB1			6
#define GTA_KEY_MXB2			7
#define GTA_KEY_SPACE           32
#define GTA_KEY_COMMA           44
#define GTA_KEY_MINUS           45	    /* - on main keyboard */
#define GTA_KEY_PERIOD          46    /* . on main keyboard */
#define GTA_KEY_SLASH           47    /* / on main keyboard */
#define GTA_KEY_0               48
#define GTA_KEY_1               49
#define GTA_KEY_2               50
#define GTA_KEY_3               51
#define GTA_KEY_4               52
#define GTA_KEY_5               53
#define GTA_KEY_6               54
#define GTA_KEY_7               55
#define GTA_KEY_8               56
#define GTA_KEY_9               57
#define GTA_KEY_EQUALS          61
#define GTA_KEY_A               65
#define GTA_KEY_B               66
#define GTA_KEY_C               67
#define GTA_KEY_D               68
#define GTA_KEY_E               69
#define GTA_KEY_F               70
#define GTA_KEY_G               71
#define GTA_KEY_H               72
#define GTA_KEY_I               73
#define GTA_KEY_J               74
#define GTA_KEY_K               75
#define GTA_KEY_L               76
#define GTA_KEY_M               77
#define GTA_KEY_N               78
#define GTA_KEY_O               79
#define GTA_KEY_P               80
#define GTA_KEY_Q               81
#define GTA_KEY_R               82
#define GTA_KEY_S               83
#define GTA_KEY_T               84
#define GTA_KEY_U               85
#define GTA_KEY_V               86
#define GTA_KEY_W               87
#define GTA_KEY_X               88
#define GTA_KEY_Y               89
#define GTA_KEY_Z               90
#define GTA_KEY_HASH            35
#define GTA_KEY_SEMICOLON       59
#define GTA_KEY_APOSTROPHE      39
#define GTA_KEY_LBRACKET        91
#define GTA_KEY_BACKSLASH       92
#define GTA_KEY_RBRACKET        93
#define GTA_KEY_GRAVE           96    /* accent grave */
#define GTA_KEY_F1              1001
#define GTA_KEY_F2              1002
#define GTA_KEY_F3              1003
#define GTA_KEY_F4              1004
#define GTA_KEY_F5              1005
#define GTA_KEY_F6              1006
#define GTA_KEY_F7              1007
#define GTA_KEY_F8              1009
#define GTA_KEY_F9              1009
#define GTA_KEY_F10             1010
#define GTA_KEY_F11             1011
#define GTA_KEY_F12             1012
#define GTA_KEY_INSERT          1013    /* Insert on arrow keypad */
#define GTA_KEY_DELETE          1014    /* Delete on arrow keypad */
#define GTA_KEY_HOME            1015    /* Home on arrow keypad */
#define GTA_KEY_END             1016    /* End on arrow keypad */
#define GTA_KEY_PGUP            1017    /* PgUp on arrow keypad */
#define GTA_KEY_PGDN            1018    /* PgDn on arrow keypad */
#define GTA_KEY_UP              1019    /* UpArrow on arrow keypad */
#define GTA_KEY_DOWN            1020    /* DownArrow on arrow keypad */
#define GTA_KEY_LEFT            1021    /* LeftArrow on arrow keypad */
#define GTA_KEY_RIGHT           1022    /* RightArrow on arrow keypad */
#define GTA_KEY_DIVIDE          1023    /* / on numeric keypad */
#define GTA_KEY_MULTIPLY        1024    /* * on numeric keypad */
#define GTA_KEY_ADD             1025    /* + on numeric keypad */
#define GTA_KEY_SUBTRACT        1026    /* - on numeric keypad */
#define GTA_KEY_DECIMAL         1027    /* . on numeric keypad */
#define GTA_KEY_NUMPAD1         1028
#define GTA_KEY_NUMPAD2         1029
#define GTA_KEY_NUMPAD3         1030
#define GTA_KEY_NUMPAD4         1031
#define GTA_KEY_NUMPAD5         1032
#define GTA_KEY_NUMLOCK         1033
#define GTA_KEY_NUMPAD6         1034
#define GTA_KEY_NUMPAD7         1035
#define GTA_KEY_NUMPAD8         1036
#define GTA_KEY_NUMPAD9         1037
#define GTA_KEY_NUMPAD0         1038
#define GTA_KEY_NUMPADENTER     1039    /* Enter on numeric keypad */
#define GTA_KEY_SCROLL          1040    /* Scroll Lock */
#define GTA_KEY_PAUSE           1041    /* Pause */
#define GTA_KEY_BACK            1042    /* backspace */
#define GTA_KEY_TAB             1043
#define GTA_KEY_CAPSLOCK        1044    /* Caps Lock */
#define GTA_KEY_RETURN          1045    /* Enter on main keyboard */
#define GTA_KEY_LSHIFT          1046
#define GTA_KEY_RSHIFT          1047
#define GTA_KEY_LCONTROL        1049	
#define GTA_KEY_RCONTROL        1050
#define GTA_KEY_LMENU           1051    /* left Alt */
#define GTA_KEY_RMENU           1052    /* right Alt */
#define GTA_KEY_LWIN            1053    /* Left Windows key */
#define GTA_KEY_RWIN            1054    /* Right Windows key */
#define NO_KEY_DEFINED			1056

/*
FROM VC:
#define KEYBOARD				0
#define OPTIONAL_EXTRA_KEY		1
#define MOUSE					2
#define JOY_STICK				3
*/

enum eControllerType 
{
    KEYBOARD = 0,
    OPTIONAL_EXTRA,
    MOUSE,
    JOYSTICK
};

enum eControllerAction
{
	FIRE = 0,
	
	NEXT_WEAPON = 2,
	PREVIOUS_WEAPON,
	FORWARDS,
	BACKWARDS,
	LEFT,
	RIGHT,
	ZOOM_IN,
	ZOOM_OUT,
	ENTER_EXIT,
	CHANGE_CAMERA,
	JUMP,
	SPRINT,
	LOOK_BEHIND,
	CROUCH,
	ACTION,
	WALK,
	VEHICLE_FIRE,
	VEHICLE_SECONDARY_FIRE,
	VEHICLE_LEFT,
	VEHICLE_RIGHT,
	STEER_FORWARDS_DOWN,
	STEER_BACK_UP,
	ACCELERATE,
	BRAKE_REVERSE,
	RADIO_NEXT,
	RADIO_PREVIOUS,
	RADIO_USER_TRACK_SKIP,
	HORN,
	SUB_MISSION,
	HANDBRAKE,
	
	VEHICLE_LOOK_LEFT = 0x22,
	VEHICLE_LOOK_RIGHT,
	VEHICLE_LOOK_BEHIND,
	VEHICLE_MOUSE_LOOK,
	SPECIAL_CONTROL_LEFT,
	SPECIAL_CONTROL_RIGHT,
	SPECIAL_CONTROL_DOWN,
	SPECIAL_CONTROL_UP,

	AIM_WEAPON = 0x2D,

	CONVERSATION_YES = 0x2F,
	CONVERSATION_NO,
	GROUP_CONTROL_FORWARDS,
	GROUP_CONTROL_BACK,
};

class CControllerConfigManager
{
public:
    virtual void                SetControllerKeyAssociatedWithAction ( eControllerAction action, int iKey, eControllerType controllerType )=0;
    virtual int                 GetControllerKeyAssociatedWithAction ( eControllerAction action, eControllerType controllerType )=0;
    virtual unsigned char       GetInputType ( void ) = 0;
    virtual void                SetInputType ( unsigned char ucInputType ) = 0;
    virtual bool                IsMouseInverted ( void ) = 0;
    virtual void                SetMouseInverted ( bool bInverted ) = 0;
    virtual bool                GetFlyWithMouse ( void ) = 0;
    virtual void                SetFlyWithMouse ( bool bFlyWithMouse ) = 0;
    virtual bool                GetSteerWithMouse ( void ) = 0;
    virtual void                SetSteerWithMouse ( bool bFlyWithMouse ) = 0;
};
#endif