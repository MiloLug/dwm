//keys
#include "XF86keysym.h"

#define DE_CONTROL "/usr/local/de-scripts/control"
#define DE_WIDGETS "/usr/local/de-scripts/widgets"

/* appearance */
static const unsigned int borderpx  = 1;        /* border pixel of windows */
static const unsigned int snap      = 32;       /* snap pixel */
static const int showbar            = 0;        /* 0 means no bar */
static const int topbar             = 0;        /* 0 means bottom bar */
static const char *fonts[]          = { "monospace:size=8" };
static const char dmenufont[]       = "monospace:size=8";
static const char col_gray1[]       = "#111111";
static const char col_gray2[]       = "#444444";
static const char col_gray3[]       = "#bbbbbb";
static const char col_gray4[]       = "#eeeeee";
static const char col_cyan[]        = "#241017";
static const char col_black[]       = "#000000";
static const char *colors[][3]      = {
	/*               fg         bg         border   */
	[SchemeNorm] = { col_gray3, col_gray1, col_black },
	[SchemeSel]  = { col_gray4, col_gray1,  col_cyan  },
};

/* tagging */
static const char *tags[] = { "1", "2", "3", "4", "5", "6", "7", "8", "9" };

static const Rule rules[] = {
	/* xprop(1):
	 *	WM_CLASS(STRING) = instance, class
	 *	WM_NAME(STRING) = title
	 */
	/* class      instance    title       tags mask     isfloating   monitor */
	{ "Gimp",     NULL,       NULL,       0,            1,           -1 },
	// { "Firefox",  NULL,       NULL,       1 << 8,       0,           -1 },
};

/* layout(s) */
static const float mfact     = 0.55; /* factor of master area size [0.05..0.95] */
static const int nmaster     = 1;    /* number of clients in master area */
static const int resizehints = 1;    /* 1 means respect size hints in tiled resizals */
static const int lockfullscreen = 1; /* 1 will force focus on the fullscreen window */

static const Layout layouts[] = {
	/* symbol     arrange function */
	{ "[T]",      tile },    /* first entry is default */
	{ "[F]",      NULL },    /* no layout function means floating behavior */
	{ "[M]",      monocle },
};

/* key definitions */
#define MODKEY Mod1Mask
#define TAGKEYS(KEY,TAG) \
	{ MODKEY,                       KEY,      view,           {.ui = 1 << TAG} }, \
	{ ControlMask|ShiftMask,        KEY,      toggleview,     {.ui = 1 << TAG} }, \
	{ MODKEY|ControlMask,           KEY,      tag,            {.ui = 1 << TAG} }, \
	{ MODKEY|ControlMask|ShiftMask, KEY,      toggletag,      {.ui = 1 << TAG} },

/* helper for spawning shell commands in the pre dwm-5.0 fashion */
#define SHCMD(cmd) { .v = (const char*[]){ "/bin/sh", "-c", cmd, NULL } }

/* commands */
static char dmenumon[2] = "0"; /* component of dmenucmd, manipulated in spawn() */
static const char *dmenucmd[] = { "dmenu_run", "-m", dmenumon, "-fn", dmenufont, "-nb", col_gray1, "-nf", col_gray3, "-sb", col_cyan, "-sf", col_gray4, NULL };
static const char *termcmd[]  = { "kitty", NULL };
static const char *clipmenu[] = { "clipmenu", NULL };

// volume
// Default
static const char *up_volD[]   = { "/bin/sh", "-c",
	DE_WIDGETS "/run-widget volume-indicator 3 & "
	DE_CONTROL "/sound change_value 5", NULL
};
static const char *down_volD[] = { "/bin/sh", "-c", 
	DE_WIDGETS "/run-widget volume-indicator 3 & "
	DE_CONTROL "/sound change_value -5", NULL
};
static const char *mute_volD[] = { "/bin/sh", "-c",
	DE_WIDGETS "/run-widget volume-indicator 3 & "
	DE_CONTROL "/sound toggle_muted",NULL
};

// brightness
static const char *up_brightness[] = { "/bin/sh", "-c",
	DE_WIDGETS "/run-widget brightness-indicator 3 & "
	"sudo " DE_CONTROL "/backlight change_value 5", NULL
};
static const char *down_brightness[] = { "/bin/sh", "-c",
	DE_WIDGETS "/run-widget brightness-indicator 3 & "
	"sudo " DE_CONTROL "/backlight change_value -5", NULL
};

// redshift
static const char *up_redshift[] = { "/bin/sh", "-c",
	DE_WIDGETS "/run-widget redshift-indicator 3 & "
	DE_CONTROL "/redshift change_value 5", NULL
};
static const char *down_redshift[] = { "/bin/sh", "-c",
	DE_WIDGETS "/run-widget redshift-indicator 3 & "
	DE_CONTROL "/redshift change_value -5", NULL
};
static const char *toggle_redshift[] = { "/bin/sh", "-c",
	DE_WIDGETS "/run-widget redshift-indicator 3 & "
	DE_CONTROL "/redshift toggle_enabled", NULL
};
static const char *play_pause_cmd[] = { "playerctl", "play-pause", NULL };
static const char *play_next_cmd[] = { "playerctl", "next", NULL };
static const char *play_prev_cmd[] = { "playerctl", "previous", NULL };

// some widgets
static const char *toggle_slstatus[] = { DE_WIDGETS "/switch-widget", "slstatus-panel", NULL };

//screenshot
static const char *full_screenshot[] = { "screenshot",  NULL };
static const char *active_screenshot[] = { "screenshot", "window", NULL };
static const char *select_screenshot[] = { "screenshot", "-i", "select", NULL };

//power
static const char *sh_sleep[] = { "/bin/sh", "-c", "slock & (sleep 0.1; sudo systemctl suspend)", NULL };
static const char *sh_shutdown[] = { "/bin/sh", "-c", "sudo systemctl poweroff", NULL };

/* KEYS */
static const Key keys[] = {
	/* modifier                     key        function        argument */
	// Media controls
	{ 0,              XF86XK_AudioLowerVolume, spawn,          {.v = down_volD } },
	{ 0,              XF86XK_AudioMute,        spawn,          {.v = mute_volD } },
	{ 0,              XF86XK_AudioRaiseVolume, spawn,          {.v = up_volD } },
	{ 0,              XF86XK_AudioPlay,        spawn,          {.v = play_pause_cmd } },
	{ 0,              XF86XK_AudioNext,        spawn,          {.v = play_next_cmd } },
	{ 0,              XF86XK_AudioPrev,        spawn,          {.v = play_prev_cmd } },

	// Brightness controls
	{ 0,               XF86XK_MonBrightnessUp, spawn,          {.v = up_brightness } },
	{ 0,             XF86XK_MonBrightnessDown, spawn,          {.v = down_brightness } },
	
	{ MODKEY,          XF86XK_MonBrightnessUp, spawn,          {.v = up_redshift } },
	{ MODKEY,        XF86XK_MonBrightnessDown, spawn,          {.v = down_redshift } },
	{ ControlMask,     XF86XK_MonBrightnessUp, spawn,          {.v = toggle_redshift } },

	// Screenshots
	{ 0,                            XK_Print,  spawn,          {.v = full_screenshot } },
	{ ControlMask,                  XK_Print,  spawn,          {.v = active_screenshot } },
	{ ShiftMask,                    XK_Print,  spawn,          {.v = select_screenshot } },	

	// Power management
	{ MODKEY,                XF86XK_PowerOff,  spawn,          {.v = sh_sleep } },
	{ MODKEY|ControlMask,    XF86XK_PowerOff,  spawn,          {.v = sh_shutdown } },

	// Other
	{ MODKEY,                       XK_r,      spawn,          {.v = dmenucmd } },
	{ MODKEY,                       XK_grave,  spawn,          {.v = termcmd } },
	{ MODKEY,                       XK_b,      spawn,          {.v = toggle_slstatus } },
	{ MODKEY,                  XK_backslash,   spawn,          {.v = clipmenu } },

	// DWM Controls
	{ MODKEY,                       XK_j,      focusstack,     {.i = +1 } },
	{ MODKEY,                       XK_k,      focusstack,     {.i = -1 } },
	
	{ MODKEY,                       XK_i,      incnmaster,     {.i = +1 } },
	{ MODKEY,                       XK_d,      incnmaster,     {.i = -1 } },

	{ MODKEY,                       XK_h,      setmfact,       {.f = -0.05} },  // + right size
	{ MODKEY,                       XK_l,      setmfact,       {.f = +0.05} },  // + left size

	{ MODKEY|ShiftMask,             XK_h,      setcfact,       {.f = +0.25} },  // - current vertical
	{ MODKEY|ShiftMask,             XK_l,      setcfact,       {.f = -0.25} },  // - current vertical
	{ MODKEY|ShiftMask,             XK_o,      setcfact,       {.f =  0.00} },  // balance vertical

	{ MODKEY,                       XK_Return, zoom,           {0} },  //Switch layout between last tiles
	
	{ MODKEY,                       XK_Tab,    view,           {0} },  //Switch between last desktops

	{ MODKEY|ControlMask,           XK_c,      killclient,     {0} },
	{ MODKEY,                       XK_t,      setlayout,      {.v = &layouts[0]} },
//	{ MODKEY,                       XK_f,      setlayout,      {.v = &layouts[1]} },
	{ MODKEY,                       XK_m,      setlayout,      {.v = &layouts[2]} },
//	{ MODKEY,                       XK_space,  setlayout,      {0} },
//	{ MODKEY|ControlMask,           XK_space,  togglefloating, {0} },
//	{ MODKEY,                       XK_0,      view,           {.ui = ~0 } },
//	{ MODKEY|ControlMask,           XK_0,      tag,            {.ui = ~0 } },
	
	// Monitors switching
	{ MODKEY,                       XK_comma,  focusmon,       {.i = -1 } },
	{ MODKEY,                       XK_period, focusmon,       {.i = +1 } },
	{ MODKEY|ControlMask,           XK_comma,  tagmon,         {.i = -1 } },
	{ MODKEY|ControlMask,           XK_period, tagmon,         {.i = +1 } },
	
	// Tags switching
	TAGKEYS(                        XK_1,                      0)
	TAGKEYS(                        XK_2,                      1)
	TAGKEYS(                        XK_3,                      2)
	TAGKEYS(                        XK_4,                      3)
	TAGKEYS(                        XK_5,                      4)
	TAGKEYS(                        XK_6,                      5)
	TAGKEYS(                        XK_7,                      6)
	TAGKEYS(                        XK_8,                      7)
	TAGKEYS(                        XK_9,                      8)
	
	// Quit
	{ ControlMask|ShiftMask,        XK_q,      quit,           {0} },
};

/* button definitions */
/* click can be ClkTagBar, ClkLtSymbol, ClkStatusText, ClkWinTitle, ClkClientWin, or ClkRootWin */
static const Button buttons[] = {
	/* click                event mask      button          function        argument */
	
	// Window position/size controls
	{ ClkClientWin,  ControlMask|MODKEY,    Button1,        movemouse,      {0} },
	{ ClkClientWin,  ControlMask|MODKEY,    Button2,        togglefloating, {0} },
	{ ClkClientWin,  ControlMask|MODKEY,    Button3,        resizemouse,    {0} },
	
	// Other
	{ ClkLtSymbol,          0,              Button1,        setlayout,      {0} },
	{ ClkLtSymbol,          0,              Button3,        setlayout,      {.v = &layouts[2]} },
	{ ClkWinTitle,          0,              Button2,        zoom,           {0} },
	{ ClkStatusText,        0,              Button2,        spawn,          {.v = termcmd } },
	{ ClkTagBar,            0,              Button1,        view,           {0} },
	{ ClkTagBar,            0,              Button3,        toggleview,     {0} },
	{ ClkTagBar,            MODKEY,         Button1,        tag,            {0} },
	{ ClkTagBar,            MODKEY,         Button3,        toggletag,      {0} },
};
