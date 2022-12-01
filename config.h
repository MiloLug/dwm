//keys
#include "XF86keysym.h"

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
static const char *upvolD[]   = { "/bin/sh", "-c",
    "if ! pgrep -f ^/tmp/light-status-vol; then light-status-vol & (sleep 3; pkill -f /tmp/light-status-vol) & fi;"
    "pactl set-sink-volume `pactl get-default-sink` +5%", NULL
};
static const char *downvolD[] = { "/bin/sh", "-c", 
    "if ! pgrep -f ^/tmp/light-status-vol; then light-status-vol & (sleep 3; pkill -f /tmp/light-status-vol) & fi;"
    "pactl set-sink-volume `pactl get-default-sink` -5%", NULL
};
static const char *mutevolD[] = { "/bin/sh", "-c",
    "if ! pgrep -f ^/tmp/light-status-vol; then light-status-vol & (sleep 3; pkill -f /tmp/light-status-vol) & fi;"
    "/usr/bin/pamixer -t", NULL
};
// 0 Dev
static const char *upvol0[]   = { "/usr/bin/pactl", "set-sink-volume", "0", "+5%",     NULL };
static const char *downvol0[] = { "/usr/bin/pactl", "set-sink-volume", "0", "-5%",     NULL };
static const char *mutevol0[] = { "/usr/bin/pactl", "set-sink-mute",   "0", "toggle",  NULL };

// brightness
static const char *upbrigh[] = {
    "/bin/sh", "-c",
    "if ! pgrep -f ^/tmp/light-status-brightness; then light-status-brightness & (sleep 3; pkill -f /tmp/light-status-brightness) & fi;"
    "sudo percentage-conf -FM /sys/class/backlight/intel_backlight/max_brightness -Fc /sys/class/backlight/intel_backlight/actual_brightness -Fo /sys/class/backlight/intel_backlight/brightness -i 5",
    NULL
};
static const char *downbrigh[] = {
    "/bin/sh", "-c",
    "if ! pgrep -f ^/tmp/light-status-brightness; then light-status-brightness & (sleep 3; pkill -f /tmp/light-status-brightness) & fi;"
    "sudo percentage-conf -FM /sys/class/backlight/intel_backlight/max_brightness -Fc /sys/class/backlight/intel_backlight/actual_brightness -Fo /sys/class/backlight/intel_backlight/brightness -i -5",
    NULL
};

static const char *up_redshift[] = {
    "/bin/sh", "-c",
    "if ! pgrep -f ^/tmp/light-status-redshift; then light-status-redshift & (sleep 3; pkill -f /tmp/light-status-redshift) & fi;"
    "percentage-conf -m 1000 -M 25000 -Fc ~/dwm.conf.d/redshift-state.conf -Fo ~/dwm.conf.d/redshift-state.conf -i 5;" 
    "redshift -P -O `cat ~/dwm.conf.d/redshift-state.conf`",
    NULL
};
static const char *down_redshift[] = {
    "/bin/sh", "-c",
    "if ! pgrep -f ^/tmp/light-status-redshift; then light-status-redshift & (sleep 3; pkill -f /tmp/light-status-redshift) & fi;"
    "percentage-conf -m 1000 -M 25000 -Fc ~/dwm.conf.d/redshift-state.conf -Fo ~/dwm.conf.d/redshift-state.conf -i -5;" 
    "redshift -P -O `cat ~/dwm.conf.d/redshift-state.conf`",
    NULL
};
static const char *normal_redshift[] = { "/bin/sh", "-c", "cat ~/dwm.conf.d/redshift-state.normal.conf | tee ~/dwm.conf.d/redshift-state.conf | (read a; redshift -PO $a)", NULL };

//screenshot
static const char *fullscreenshot[] = { "screenshot",  NULL };
static const char *activescreenshot[] = { "screenshot", "window", NULL };
static const char *selectscreenshot[] = { "screenshot", "select", NULL };

//power
static const char *sh_sleep[] = { "/bin/sh", "-c", "slock & sudo zzz", NULL };
static const char *sh_shutdown[] = { "/bin/sh", "-c", "sudo shutdown -P now", NULL };

static const char *switchstatuspanel[] = { "/bin/sh", "-c", "if pgrep -f ^/tmp/light-status-slstatus; then pkill -f /tmp/light-status-slstatus; else light-status-slstatus; fi", NULL };

/* KEYS */
static Key keys[] = {
	/* modifier                     key        function        argument */
	// Audio volume controls
	{ 0,              XF86XK_AudioLowerVolume, spawn,          {.v = downvolD } },
	{ 0,              XF86XK_AudioMute,        spawn,          {.v = mutevolD } },
	{ 0,              XF86XK_AudioRaiseVolume, spawn,          {.v = upvolD } },
	
	{ ShiftMask,      XF86XK_AudioLowerVolume, spawn,          {.v = downvol0 } },
	{ ShiftMask,      XF86XK_AudioMute,        spawn,          {.v = mutevol0 } },
	{ ShiftMask,      XF86XK_AudioRaiseVolume, spawn,          {.v = upvol0 } },

	// Brightness controls
	{ 0,               XF86XK_MonBrightnessUp, spawn,          {.v = upbrigh } },
	{ 0,             XF86XK_MonBrightnessDown, spawn,          {.v = downbrigh } },
    
    { MODKEY,          XF86XK_MonBrightnessUp, spawn,          {.v = up_redshift } },
	{ MODKEY,        XF86XK_MonBrightnessDown, spawn,          {.v = down_redshift } },
	{ ControlMask,     XF86XK_MonBrightnessUp, spawn,          {.v = normal_redshift } },

	// Screenshots
	{ 0,                            XK_Print,  spawn,          {.v = fullscreenshot } },
	{ ControlMask,                  XK_Print,  spawn,          {.v = activescreenshot } },
	{ ShiftMask,                    XK_Print,  spawn,          {.v = selectscreenshot } },	

	// Power management
	{ MODKEY,                XF86XK_PowerOff,  spawn,          {.v = sh_sleep } },
	{ MODKEY|ControlMask,    XF86XK_PowerOff,  spawn,          {.v = sh_shutdown } },

	// Other
	{ MODKEY,                       XK_r,      spawn,          {.v = dmenucmd } },
	{ MODKEY,                       XK_grave,  spawn,          {.v = termcmd } },
	{ MODKEY,                       XK_b,      spawn,          {.v = switchstatuspanel } },
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
static Button buttons[] = {
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
