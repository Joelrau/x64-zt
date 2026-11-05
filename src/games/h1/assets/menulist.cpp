#include <std_include.hpp>
#include "menulist.hpp"
#include <stdexcept>

namespace zonetool::h1
{
	/* windowDef_t->dynamicFlags */
#define WINDOWDYNAMIC_MOUSE_OVER_ITEM   0x00000001
#define WINDOWDYNAMIC_HASFOCUS          0x00000002
#define WINDOWDYNAMIC_VISIBLE           0x00000004
// 0x8
#define WINDOWDYNAMIC_FADEOUT           0x00000010
#define WINDOWDYNAMIC_FADEIN            0x00000020
#define WINDOWDYNAMIC_MOUSE_OVER_TEXT   0x00000040
#define WINDOWDYNAMIC_CLOSING           0x00000080
#define WINDOWDYNAMIC_SCROLL_UPARROW    0x00000100
#define WINDOWDYNAMIC_SCROLL_DOWNARROW  0x00000200
#define WINDOWDYNAMIC_SCROLL_THUMB      0x00000400
#define WINDOWDYNAMIC_SCROLL_PAGEUP     0x00000800
#define WINDOWDYNAMIC_SCROLL_PAGEDOWN   0x00001000
// 0x2000
// 0x4000 loading/forcepaint
#define WINDOWDYNAMIC_BACKCOLOR         0x00008000
#define WINDOWDYNAMIC_FORECOLOR         0x00010000

/* windowDef_t->staticFlags */
#define WINDOWSTATIC_DECORATION					0x00100000
#define WINDOWSTATIC_HORIZONTALSCROLL			0x00200000
#define WINDOWSTATIC_SCREENSPACE				0x00400000
#define WINDOWSTATIC_AUTOWRAPPED				0x00800000
#define WINDOWSTATIC_POPUP						0x01000000
#define WINDOWSTATIC_OUTOFBOUNDSCLICK			0x02000000
#define WINDOWSTATIC_LEGACYSPLITSCREENSCALE		0x04000000
#define WINDOWSTATIC_HIDDENDURINGFLASH			0x10000000
#define WINDOWSTATIC_HIDDENDURINGSCOPE			0x20000000
#define WINDOWSTATIC_HIDDENDURINGUI				0x40000000
#define WINDOWSTATIC_TEXTONLYFOCUS				0x80000000

#define ITEM_TYPE_TEXT				0		// simple text
#define ITEM_TYPE_BUTTON			1		// button, basically text with a border
#define ITEM_TYPE_RADIOBUTTON		2		// toggle button, may be grouped
#define ITEM_TYPE_CHECKBOX			3		// check box
#define ITEM_TYPE_EDITFIELD 		4		// editable text, associated with a dvar
#define ITEM_TYPE_COMBO 			5		// drop down list
#define ITEM_TYPE_LISTBOX			6		// scrollable list
#define ITEM_TYPE_MODEL 			7		// model
#define ITEM_TYPE_OWNERDRAW 		8		// owner draw, name specs what it is
#define ITEM_TYPE_NUMERICFIELD		9		// editable text, associated with a dvar
#define ITEM_TYPE_SLIDER			10		// mouse speed, volume, etc.
#define ITEM_TYPE_YESNO 			11		// yes no dvar setting
#define ITEM_TYPE_MULTI 			12		// multiple list setting, enumerated
#define ITEM_TYPE_DVARENUM 			13		// multiple list setting, enumerated from a dvar
#define ITEM_TYPE_BIND				14		// bind
#define ITEM_TYPE_MENUMODEL 		15		// special menu model
#define ITEM_TYPE_VALIDFILEFIELD	16		// text must be valid for use in a dos filename
#define ITEM_TYPE_DECIMALFIELD		17		// editable text, associated with a dvar, which allows decimal input
#define ITEM_TYPE_UPREDITFIELD		18		// editable text, associated with a dvar
#define ITEM_TYPE_GAME_MESSAGE_WINDOW 19	// game message window
#define ITEM_TYPE_NEWSTICKER		20		// horizontal scrollbox
#define ITEM_TYPE_TEXTSCROLL		21		// vertical scrollbox
#define ITEM_TYPE_EMAILFIELD		22
#define ITEM_TYPE_PASSWORDFIELD		23

	/* expressionEntry->type */
#define OPERATOR	0
#define OPERAND		1

//script flags
#define SCFL_NOERRORS				0x0001
#define SCFL_NOWARNINGS				0x0002
#define SCFL_NOSTRINGWHITESPACES	0x0004
#define SCFL_NOSTRINGESCAPECHARS	0x0008
#define SCFL_PRIMITIVE				0x0010
#define SCFL_NOBINARYNUMBERS		0x0020
#define SCFL_NONUMBERVALUES		0x0040

//token types
#define TT_STRING				1			// string
#define TT_LITERAL				2			// literal
#define TT_NUMBER				3			// number
#define TT_NAME					4			// name
#define TT_PUNCTUATION			5			// punctuation

//string sub type
//---------------
//		the length of the string
//literal sub type
//----------------
//		the ASCII code of the literal
//number sub type
//---------------
#define TT_DECIMAL				0x0008		// decimal number
#define TT_HEX					0x0100		// hexadecimal number
#define TT_OCTAL				0x0200		// octal number
#define TT_BINARY				0x0400		// binary number
#define TT_FLOAT				0x0800		// floating point number
#define TT_INTEGER				0x1000		// integer number
#define TT_LONG					0x2000		// long number
#define TT_UNSIGNED				0x4000		// unsigned number
//punctuation sub type
//--------------------
#define P_RSHIFT_ASSIGN			1
#define P_LSHIFT_ASSIGN			2
#define P_PARMS					3
#define P_PRECOMPMERGE			4

#define P_LOGIC_AND				5
#define P_LOGIC_OR				6
#define P_LOGIC_GEQ				7
#define P_LOGIC_LEQ				8
#define P_LOGIC_EQ				9
#define P_LOGIC_UNEQ			10

#define P_MUL_ASSIGN			11
#define P_DIV_ASSIGN			12
#define P_MOD_ASSIGN			13
#define P_ADD_ASSIGN			14
#define P_SUB_ASSIGN			15
#define P_INC					16
#define P_DEC					17

#define P_BIN_AND_ASSIGN		18
#define P_BIN_OR_ASSIGN			19
#define P_BIN_XOR_ASSIGN		20
#define P_RSHIFT				21
#define P_LSHIFT				22

#define P_POINTERREF			23
#define P_CPP1					24
#define P_CPP2					25
#define P_MUL					26
#define P_DIV					27
#define P_MOD					28
#define P_ADD					29
#define P_SUB					30
#define P_ASSIGN				31

#define P_BIN_AND				32
#define P_BIN_OR				33
#define P_BIN_XOR				34
#define P_BIN_NOT				35

#define P_LOGIC_NOT				36
#define P_LOGIC_GREATER			37
#define P_LOGIC_LESS			38

#define P_REF					39
#define P_COMMA					40
#define P_SEMICOLON				41
#define P_COLON					42
#define P_QUESTIONMARK			43

#define P_PARENTHESESOPEN		44
#define P_PARENTHESESCLOSE		45
#define P_BRACEOPEN				46
#define P_BRACECLOSE			47
#define P_SQBRACKETOPEN			48
#define P_SQBRACKETCLOSE		49
#define P_BACKSLASH				50

#define P_PRECOMP				51
#define P_DOLLAR				52
//name sub type
//-------------
//		the length of the name

#define BUILTIN_LINE			1
#define BUILTIN_FILE			2
#define BUILTIN_DATE			3
#define BUILTIN_TIME			4

#define MAX_TOKEN 1024 // DO NOT CHANGE !
#define MAX_MENUDEFS_PER_MENULIST 512
#define MAX_ITEMDEFS_PER_MENUDEF 512
#define MAX_TOKENS_PER_STATEMENT 512
#define MAX_EVENT_HANDLERS_PER_EVENT 200
#define MAX_VALUES 64
#define MAX_OPERATORS 64

	static const char* g_expOperatorNames[]
	{
		"NOOP",
		")",
		"*",
		"/",
		"%",
		"+",
		"-",
		"!",
		"<",
		"<=",
		">",
		">=",
		"==",
		"!=",
		"&&",
		"||",
		"(",
		",",
		"&",
		"|",
		"~",
		"<<",
		">>",
		"dvarint(static)",
		"dvarbool(static)",
		"dvarfloat(static)",
		"dvarstring(static)",
		"int",
		"string",
		"float",
		"sin",
		"cos",
		"min",
		"max",
		"milliseconds",
		"localclientuimilliseconds",
		"dvarint",
		"dvarbool",
		"dvarfloat",
		"dvarstring",
		"ui_active",
		"flashbanged",
		"usingvehicle",
		"missilecam",
		"scoped",
		"scopedthermal",
		"scoreboard_visible",
		"inkillcam",
		"inkillcamnpc",
		"player",
		"getperk",
		"selecting_location",
		"selecting_direction",
		"team",
		"otherteam",
		"marinesfield",
		"opforfield",
		"menuisopen",
		"writingdata",
		"inlobby",
		"ingamelobby",
		"inprivateparty",
		"privatepartyhost",
		"privatepartyhostinlobby",
		"aloneinparty",
		"adsjavelin",
		"weaplockblink",
		"weapattacktop",
		"weapattackdirect",
		"weaplocking",
		"weaplocked",
		"weaplocktooclose",
		"weaplockscreenposx",
		"weaplockscreenposy",
		"secondsastime",
		"tablelookup",
		"tablelookupbyrow",
		"tablegetrownum",
		"locstring",
		"localvarint",
		"localvarbool",
		"localvarfloat",
		"localvarstring",
		"timeleft",
		"secondsascountdown",
		"gamemsgwndactive",
		"gametypename",
		"gametype",
		"gametypedescription",
		"scoreatrank",
		"spectatingclient",
		"spectatingfree",
		"keybinding",
		"actionslotusable",
		"hudfade",
		"maxrecommendedplayers",
		"acceptinginvite",
		"isintermission",
		"gamehost",
		"partyismissingmappack",
		"partymissingmappackerror",
		"anynewmappacks",
		"amiselected",
		"partystatusstring",
		"attachedcontrollercount",
		"issplitscreenonlinepossible",
		"splitscreenplayercount",
		"getplayerdata",
		"getplayerdatasplitscreen",
		"getmatchrulesdata",
		"getsavedmatchrulesmetadata",
		"levelforexperiencemp",
		"levelforexperienceso",
		"isitemunlocked",
		"isitemunlockedsplitscreen",
		"iscardiconunlocked",
		"iscardtitleunlocked",
		"iscardiconnew",
		"iscardtitlenew",
		"iscardiconunlockedsplitscreen",
		"iscardtitleunlockedsplitscreen",
		"iscardiconnewsplitscreen",
		"iscardtitlenewsplitscreen",
		"isprofileitemunlocked",
		"isprofileitemunlockedsplitscreen",
		"debugprint",
		"getplayerdataanybooltrue",
		"getprofileanybooltrue",
		"weaponname",
		"isreloading",
		"savegameavailable",
		"unlockeditemcount",
		"unlockeditemcountsplitscreen",
		"unlockeditem",
		"unlockeditemsplitscreen",
		"radarisjammed",
		"radarjamintensity",
		"radarisenabled",
		"isempjammed",
		"playerads",
		"weaponheatactive",
		"weaponheatvalue",
		"weaponheatoverheated",
		"getsplashtext",
		"getsplashdescription",
		"getsplashmaterial",
		"splashhasicon",
		"splashrownum",
		"getfocuseditemname",
		"getfocuseditemx",
		"getfocuseditemy",
		"getfocuseditemwidth",
		"getfocuseditemheight",
		"getitemx",
		"getitemy",
		"getitemwidth",
		"getitemheight",
		"playlist",
		"scoreboardexternalmutenotice",
		"getmapname",
		"getmapimage",
		"getmapcustom",
		"getmigrationstatus",
		"getplayercardinfo",
		"isofflineprofileselected",
		"coopplayer",
		"iscoop",
		"getpartystatus",
		"getsearchparams",
		"gettimeplayed",
		"isselectedplayerfriend",
		"getcharbyindex",
		"getprofiledata",
		"getprofiledatasplitscreen",
		"isprofilesignedin",
		"getwaitpopupstatus",
		"getnattype",
		"getlocalizednattype",
		"getadjustedsafeareahorizontal",
		"getadjustedsafeareavertical",
		"connectioninfo",
		"offlineprofilecansave",
		"userwithoutofflineprofilewarning",
		"allsplitscreenprofilescansave",
		"allsplitscreenprofilesaresignedin",
		"dowehavemappack",
		"mayinviteplayertoparty",
		"getpatchnotes",
		"getgameinfos",
		"coopready",
		"votecast",
		"votepassed",
		"getmapvotemapimage",
		"getmapvotemapname",
		"mapvotegametypename",
		"isfriendinvitable",
		"isfriendjoinable",
		"getsortedchallengeindex",
		"getsortedchallengename",
		"getsortedchallengecount",
		"getfilterchallengecount",
		"getfilterchallengelockedcount",
		"getfilterchallengecompletecount",
		"issortedchallengetiered",
		"getchallengefiltercachecount",
		"getchallengefiltercachecompletecount",
		"iscoopsearching",
		"coopshowpublictype",
		"coopdisplayablegroupnum",
		"coophasrequiredonlinefiles",
		"getTextWidth",
		"getTextHeight",
		"isdeveloper",
		"isusingairburst",
		"getairburstmeters",
		"getcrosshairtracemeters",
		"getfacebookstatustext",
		"isfacebookloggedin",
		"isfacebookchecking",
		"isfacebookallowed",
		"getprivatepartystatus",
		"includedinmaprotation",
		"select",
		"isdemoplaying",
		"getusergrouptext",
		"getusergroupcommoninteresttotal",
		"isdemofollowcamera",
		"isdemofreecamera",
		"isdemocapturingscreenshot",
		"ispartyhostwaitingonmembers",
		"ispopuppartymemberaway",
		"isselectedpartymemberaway",
		"gettime",
		"gameendtime",
		"hasfocus",
		"menuhasfocus",
		"getdemosegmentcount",
		"getdemosegmentinformation",
		"isclipmodified",
		"isusingmatchrulesdata",
		"isguest",
		"getfacebookhelptext",
		"iseliteclanallowed",
		"isentitlementsallowed",
		"isusergroupsallowed",
		"iswaitingforonlineservices",
		"getTextWidthModCase",
		"getSaveScreenTitle",
		"getSaveScreenDescription",
		"getOnlineVaultSelectedItemData",
		"isOnlineVaultRestricted",
		"isContentServerTaskInProgress",
		"getContentServerTaskProgress",
		"getRecentGamesSelectedItemData",
		"gametypenameAbbreviated",
		"mapvotegametypenameAbbreviated",
		"isusersignedintolive",
		"usercanplayonline",
		"getFeederData",
		"partyclientsuptodate",
		"truncateTextWithEllipsis",
		"uistarted",
		"canRenderClip",
		"getpreviewmapcustom",
		"getdlcmapsavailablecount",
		"isusersignedin",
		"isUsingIntermissionTimer",
		"isUsingCustomMapRotation",
		"menuistopmost",
		"facebook_isplatfromfriend",
		"eliteclan_isplatfromfriend",
		"eliteclan_isme",
		"eliteclan_isleader",
		"isusersignedinforvault",
		"getusingmatchrulesdata",
		"canuseraccessonlinevault",
		"friend_getgamertag",
		"recentplayer_getgamertag",
		"liveparty_getgamertag",
		"facebook_getgamertag",
		"eliteclan_getgamertag",
		"liveparty_isme",
		"liveparty_islocal",
		"doubleclickwasrightclick",
		"isdemocliprecording",
		"getIndexFromString",
		"getStringWithoutIndex",
		"eliteclan_getname",
		"eliteclan_gethelp",
		"eliteclan_getmotd",
		"eliteclan_ismember",
		"eliteclan_isemblem_ok",
		"facebook_friends_show_next",
		"facebook_friends_show_prev",
		"getOnlineVaultFriendGamerTag",
		"getObjectiveListHeight",
		"isClientDemoEnabled",
		"isusersignedintodemonware",
		"customClassIsRestricted",
		"weaponIsRestricted",
		"anysplitscreenprofilesaresignedin",
		"isguestsplitscreen",
		"isitemunlockedbyclient",
		"isanyusersignedintolive",
		"splitscreenactivegamepadcount",
		"showFriendPlayercard",
		"getFriendPlayercardPresence",
		"showRecentPlayerGroupIcon",
		"getwrappedtextheight",
		"canClientSave",
		"getgameinvitescount",
		"issplitscreengamerliveenabled",
		"so_coopShowCommonGroupIcon",
		"stripColorsFromString",
		"DEPRECATED",
		"ischallengeperiodic",
		"getchallengedata",
		"iseliteapppresent",
		"eliteclan_selectedisme",
		"enoughStorageSpaceForClientDemo",
		"isusersignedinforcommerce",
		"getfacebookmenutext",
		"getfacebookisposting",
		"meetplayer_isplatformfriend",
		"isselectedplayerguest",
		"getsplitscreencontrollerclientnum",
		"isClientDemoEnabledSplitScreen",
		"ItemCanTakeFocus",
		"getTimeSinceLastDoubleClick",
		"isServerListRefreshing",
		"isRecipeNameValid",
		"recipeExists",
		"getfacebookoptionshelptext",
		"dowehaveallavailablemappacks",
		"isThereNewEliteItems",
		"isPayingSubscriber",
		"localuser_ismissingmappack",
		"localuser_missingmappackerror",
		"getFirstSpecOpsDLCMap",
		"localuser_missingmapname",
		"showStoreNew",
		"isUsingBots",
		"isBotsAllowed",
		"isColorBlind",
		"botsConnectType",
		"botsDifficulty",
		"spectatingthird",
		"getplayername",
		"squad_showreport",
		"getrankedplayerdatasplitscreen",
		"getprivateplayerdatasplitscreen",
		"getcoopplayerdatasplitscreen",
		"getcommonplayerdatasplitscreen",
		"getomnvar"
	};

	const char* g_commandList[] =
	{
		"fadein",
		"fadeout",
		"show",
		"hide",
		"showMenu",
		"hideMenu",
		"setcolor",
		"open",
		"close",
		"forceClose",
		"escape",
		"closeForAllPlayers",
		"ingameopen",
		"ingameclose",
		"setbackground",
		"setitemcolor",
		"focusfirst",
		"setfocus",
		"setfocusbydvar",
		"setdvar",
		"exec",
		"execfirstclient",
		"execnow",
		"execkeypress",
		"execOnDvarStringValue",
		"execOnDvarIntValue",
		"execOnDvarFloatValue",
		"execNowOnDvarStringValue",
		"execNowOnDvarIntValue",
		"execNowOnDvarFloatValue",
		"play",
		"scriptmenuresponse",
		"scriptMenuRespondOnDvarStringValue",
		"scriptMenuRespondOnDvarIntValue",
		"scriptMenuRespondOnDvarFloatValue",
		"resetStatsConfirm",
		"resetStatsCancel",
		"setGameMode",
		"deleteEliteCacFile",
		"integrateEliteCacFile",
		"loadMatchRulesDataFromPlayer",
		"saveMatchRulesDataToPlayer",
		"loadMatchRulesDataFromHistory",
		"loadMatchRulesDataDefaults",
		"setUsingMatchRulesData",
		"includeInMapRotation",
		"usecustommaprotation",
		"useintermissiontimer",
		"stopintermissiontimer",
		"cyclebotsconnecttype",
		"usebots",
		"cyclebotsdifficulty",
		"setLocalVarBool",
		"setLocalVarInt",
		"setLocalVarFloat",
		"setLocalVarString",
		"feederTop",
		"feederBottom",
		"feederPageUp",
		"feederPageDown",
		"showGamerCard",
		"showCoopGamerCard",
		"showSplitscreenGamerCard",
		"reportPlayerOffensive",
		"reportPlayerExploiting",
		"reportPlayerCheating",
		"reportPlayerBoosting",
		"reportPlayerBoosting",
		"setCardIcon",
		"setCardTitle",
		"setCardIconNew",
		"setCardTitleNew",
		"setCardIconSplitScreen",
		"setCardTitleSplitScreen",
		"setCardIconNewSplitScreen",
		"setCardTitleNewSplitScreen",
		"purchasePrestigeTitle",
		"setProfileItemNew",
		"setProfileItemNewSplitScreen",
		"openforgametype",
		"closeforgametype",
		"storePopupXuid",
		"kickPlayer",
		"getKickPlayerQuestion",
		"partyUpdateMissingMapPackDvar",
		"getHostMigrateQuestion",
		"makehost",
		"togglePlayerMute",
		"muteAll",
		"muteAllButParty",
		"unmuteAll",
		"resolveError",
		"runCompletionResolve",
		"clearCompletionResolve",
		"execWithResolve",
		"lerp",
		"playMenuVideo",
		"setBackgroundVideo",
		"partyAbortWaiting",
		"clearEntitlementNew",
		"anticheat_bancheck",
		"luiLeaveMenu",
		"luiRestoreMenu",
		"luiRestoreMenuExclusively"
	};

	struct ItemFloatExpressionEntry
	{
		int target;
		const char* s1;
		const char* s2;
	};

	ItemFloatExpressionEntry g_itemFloatExpressions[19] =
	{
		{ 0, "rect", "x" },
		{ 1, "rect", "y" },
		{ 2, "rect", "w" },
		{ 3, "rect", "h" },
		{ 4, "forecolor", "r" },
		{ 5, "forecolor", "g" },
		{ 6, "forecolor", "b" },
		{ 7, "forecolor", "rgb" },
		{ 8, "forecolor", "a" },
		{ 9, "glowcolor", "r" },
		{ 10, "glowcolor", "g" },
		{ 11, "glowcolor", "b" },
		{ 12, "glowcolor", "rgb" },
		{ 13, "glowcolor", "a" },
		{ 14, "backcolor", "r" },
		{ 15, "backcolor", "g" },
		{ 16, "backcolor", "b" },
		{ 17, "backcolor", "rgb" },
		{ 18, "backcolor", "a" }
	};

	enum parseSkip_t : std::int32_t
	{
		SKIP_NO = 0x0,
		SKIP_YES = 0x1,
		SKIP_ALL_ELIFS = 0x2,
	};

	//punctuation
	struct punctuation_s
	{
		const char* p;					//punctuation character(s)
		int n;							//punctuation indication
		punctuation_s* next;			//next punctuation
	};

	//token
	struct token_s
	{
		char string[MAX_TOKEN];			//available token
		int type;						//last read token type
		std::uint64_t subtype;			//last read token sub type
		unsigned int intvalue;			//integer value
		float floatvalue;				//floating point value
		char* whitespace_p;				//start of white space before token
		char* endwhitespace_p;			//start of white space before token
		int line;						//line the token was on
		int linescrossed;				//lines crossed in white space
		token_s* next;					//next token in chain
	};

	//script file
	struct script_s
	{
		char filename[64];				//file name of the script
		char* buffer;					//buffer containing the script
		char* script_p;					//current pointer in the script
		char* end_p;					//pointer to the end of the script
		char* lastscript_p;				//script pointer before reading token
		char* whitespace_p;				//begin of the white space
		char* endwhitespace_p;			//end of the white space
		std::size_t length;					//length of the script in bytes
		int line;						//current line in script
		int lastline;					//line before reading token
		int tokenavailable;				//set by UnreadLastToken
		int flags;						//several script flags
		punctuation_s* punctuations;	//the punctuations used in the script
		punctuation_s** punctuationtable;
		token_s token;					//available token
		script_s* next;					//next script in a chain
	};

	//macro definitions
	struct define_s
	{
		char* name;						//define name
		int flags;						//define flags
		int builtin;					// > 0 if builtin define
		int numparms;					//number of define parameters
		token_s* parms;					//define parameters
		token_s* tokens;				//macro tokens (possibly containing parm tokens)
		define_s* next;					//next defined macro in a list
		define_s* hashnext;				//next define in the hash chain
	};

	//indents
	//used for conditional compilation directives:
	//#if, #else, #elif, #ifdef, #ifndef
	struct indent_s
	{
		int type;						//indent type
		parseSkip_t skip;				//true if skipping current indent
		script_s* script;				//script the indent was in
		indent_s* next;					//next indent on the indent stack
	};

	//source file
	struct source_s
	{
		char filename[64];				//file name of the script
		char includepath[64];			//path to include files
		punctuation_s* punctuations;	//punctuations to use
		script_s* scriptstack;			//stack with scripts of the source
		token_s* tokens;				//tokens to read first
		define_s* defines;				//list with macro definitions
		define_s** definehash;			//hash chain with defines
		indent_s* indentstack;			//stack with indents
		int skip;						// > 0 if skipping conditional code
		token_s token;					//last read token
	};

	struct pc_token_s
	{
		int type;
		int subtype;
		int intvalue;
		float floatvalue;
		char string[1024];
	};

	//directive name with parse function
	struct directive_s
	{
		const char* name;
		int(*func)(source_s*);
	};

	punctuation_s default_punctuations[53] =
	{
		{">>=", 0x1, 0},
		{"<<=", 0x2, 0},
		{"...", 0x3, 0},
		{"##", 0x4, 0},
		{"&&", 0x5, 0},
		{"||", 0x6, 0},
		{">=", 0x7, 0},
		{"<=", 0x8, 0},
		{"==", 0x9, 0},
		{"!=", 0xA, 0},
		{"*=", 0xB, 0},
		{"/=", 0xC, 0},
		{"%=", 0xD, 0},
		{"+=", 0xE, 0},
		{"-=", 0xF, 0},
		{"++", 0x10, 0},
		{"--", 0x11, 0},
		{"&=", 0x12, 0},
		{"|=", 0x13, 0},
		{"^=", 0x14, 0},
		{">>", 0x15, 0},
		{"<<", 0x16, 0},
		{"->", 0x17, 0},
		{"::", 0x18, 0},
		{".*", 0x19, 0},
		{"*", 0x1A, 0},
		{"/", 0x1B, 0},
		{"%", 0x1C, 0},
		{"+", 0x1D, 0},
		{"-", 0x1E, 0},
		{"=", 0x1F, 0},
		{"&", 0x20, 0},
		{"|", 0x21, 0},
		{"^", 0x22, 0},
		{"~", 0x23, 0},
		{"!", 0x24, 0},
		{">", 0x25, 0},
		{"<", 0x26, 0},
		{".", 0x27, 0},
		{",", 0x28, 0},
		{";", 0x29, 0},
		{":", 0x2A, 0},
		{"?", 0x2B, 0},
		{"(", 0x2C, 0},
		{")", 0x2D, 0},
		{"{", 0x2E, 0},
		{"}", 0x2F, 0},
		{"[", 0x30, 0},
		{"]", 0x31, 0},
		{"\\", 0x32, 0},
		{"#", 0x33, 0},
		{"$", 0x34, 0},
		{0, 0, 0}
	};

	struct operator_s
	{
		int op;
		int priority;
		int parentheses;
		struct operator_s* prev, * next;
	};

	struct value_s
	{
		unsigned int intvalue;
		float floatvalue;
		int parentheses;
		struct value_s* prev, * next;
	};

	struct parse_menudef_func
	{
		const char* keyword;
		bool(*func)(menuDef_t*/*, int*/);
	};

	struct parse_itemdef_func
	{
		const char* keyword;
		bool(*func)(itemDef_t*/*, int*/);
	};

	std::vector<parse_menudef_func> p_md_funcs;
	std::vector<parse_itemdef_func> p_id_funcs;

	//list with global defines added to every source loaded
	define_s* globaldefines;

	source_s* sourceFile;
	int numtokens;

	MenuList* menuList;

	class menu_memory
	{
	private:
		std::vector<std::uint8_t> buffer_;
		std::size_t mem_pos_;
		std::recursive_mutex mutex_;

		void allocate_memory(std::size_t size)
		{
			try
			{
				buffer_.reserve(size);
			}
			catch (std::bad_alloc const&)
			{
				ZONETOOL_FATAL("MenuFile could not allocate memory.");
			}
		}

	public:
		menu_memory(std::size_t size)
		{
			this->allocate_memory(size);
			this->mem_pos_ = 0;
			this->buffer_.clear();
		}

		menu_memory()
		{
			// 64 MB should be more than enough
			this->allocate_memory(1024 * 1024 * 64);
			this->mem_pos_ = 0;
			this->buffer_.clear();
		}

		~menu_memory()
		{
			this->buffer_.clear();
			this->buffer_.shrink_to_fit();
		}

		template <typename T>
		T* allocate(std::size_t count)
		{
			std::lock_guard<std::recursive_mutex> g(this->mutex_);
			return this->manual_allocate<T>(sizeof(T), count);
		}

		template <typename T>
		T* allocate()
		{
			std::lock_guard<std::recursive_mutex> g(this->mutex_);
			return this->allocate<T>(1);
		}

		template <typename T>
		T* manual_allocate(std::size_t size, std::size_t count = 1)
		{
			std::lock_guard<std::recursive_mutex> g(this->mutex_);

			std::uint64_t alloc_size = size * count;
			if (alloc_size + mem_pos_ > buffer_.capacity())
			{
				ZONETOOL_FATAL("MenuFile buffer out of memory.");
			}

			// alloc pointer and zero it out
			auto* pointer = buffer_.data() + mem_pos_;
			memset(pointer, 0, alloc_size);
			mem_pos_ += alloc_size;

			// return pointer
			return reinterpret_cast<T*>(pointer);
		}
	};
	menu_memory* mmem;
	zone_memory* zmem;

	//read a token from the source
	int PC_ReadToken(source_s* source, token_s* token);
	//returns true when the token is available
	int PC_CheckTokenString(source_s* source, const char* string);
	//unread the last token read from the script
	void PC_UnreadLastToken(source_s* source);
	//unread the given token
	void PC_UnreadToken(source_s* source, token_s* token);
	//read a token only if on the same line, lines are concatenated with a slash
	int PC_ReadLine(source_s* source, token_s* token, bool expandDefines);
	//returns true if there was a white space in front of the token
	int PC_WhiteSpaceBeforeToken(token_s* token);
	//load a source file
	source_s* LoadSourceFile(const char* filename);
	//print a source error
	void SourceError(source_s* source, const char* str, ...);
	//print a source warning
	void SourceWarning(source_s* source, const char* str, ...);

	void ScriptError(script_s* script, const char* str, ...)
	{
		char text[1024];
		va_list ap;

		if (script->flags & SCFL_NOERRORS) return;

		va_start(ap, str);
		vsprintf(text, str, ap);
		va_end(ap);
		ZONETOOL_ERROR("Error: file %s, line %d: %s", script->filename, script->line, text);
	}

	void ScriptWarning(script_s* script, const char* str, ...)
	{
		char text[1024];
		va_list ap;

		if (script->flags & SCFL_NOWARNINGS) return;

		va_start(ap, str);
		vsprintf(text, str, ap);
		va_end(ap);
		ZONETOOL_WARNING("Warning: file %s, line %d: %s", script->filename, script->line, text);
	}

	void PrintSourceStack(const script_s* scriptstack)
	{
		script_s* scriptstacka;

		for (scriptstacka = scriptstack->next; scriptstacka; scriptstacka = scriptstacka->next)
			ZONETOOL_WARNING("  From file %s, line %d", scriptstacka->filename, scriptstacka->line);
	}

	void SourceError(source_s* source, const char* str, ...)
	{
		char text[1024];
		va_list ap;

		va_start(ap, str);
		vsprintf(text, str, ap);
		va_end(ap);
		ZONETOOL_ERROR("Error: file %s, line %d: %s", source->scriptstack->filename, source->scriptstack->line, text);
		PrintSourceStack(source->scriptstack);
	}

	void SourceWarning(source_s* source, const char* str, ...)
	{
		char text[1024];
		va_list ap;

		va_start(ap, str);
		vsprintf(text, str, ap);
		va_end(ap);
		ZONETOOL_WARNING("Warning: file %s, line %d: %s", source->scriptstack->filename, source->scriptstack->line, text);
		PrintSourceStack(source->scriptstack);
	}

	int PC_SourceFileAndLine(char* filename, int* line)
	{
		if (!sourceFile)
			return 0;

		strcpy(filename, sourceFile->filename);
		if (sourceFile->scriptstack)
			*line = sourceFile->scriptstack->line;
		else
			*line = 0;
		return 1;
	}

	void PC_SourceError(const char* format, ...)
	{
		int line;
		char filename[128];
		va_list argptr;
		char string[4096];

		va_start(argptr, format);
		vsprintf(string, format, argptr);
		va_end(argptr);

		filename[0] = '\0';
		line = 0;
		PC_SourceFileAndLine(filename, &line);
		ZONETOOL_ERROR("Menu load error: %s, line %d: %s", filename, line, string);
	}

	int PS_ReadWhiteSpace(script_s* script)
	{
		while (1)
		{
			while (*script->script_p <= ' ')
			{
				if (!*script->script_p)
					return 0;
				if (*script->script_p == '\n')
					++script->line;
				++script->script_p;
			}
			if (*script->script_p != '/')
				break;
			if (script->script_p[1] == '/')
			{
				++script->script_p;
				do
				{
					if (!*++script->script_p)
						return 0;
				} while (*script->script_p != '\n');
				++script->line;
				if (!*++script->script_p)
					return 0;
			}
			else
			{
				if (script->script_p[1] != '*')
					return 1;
				++script->script_p;
				do
				{
					if (!*++script->script_p)
						return 0;
					if (*script->script_p == '\n')
						++script->line;
				} while (*script->script_p != '*' || script->script_p[1] != '/');
				if (!*++script->script_p)
					return 0;
				if (!*++script->script_p)
					return 0;
			}
		}
		return 1;
	}

	int PC_WhiteSpaceBeforeToken(token_s* token)
	{
		return token->endwhitespace_p - token->whitespace_p > 0;
	}

	void PC_ClearTokenWhiteSpace(token_s* token)
	{
		token->whitespace_p = 0;
		token->endwhitespace_p = 0;
		token->linescrossed = 0;
	}

	int PS_ReadEscapeCharacter(script_s* script, char* cha)
	{
		int c, val, i;

		//step over the leading '\\'
		script->script_p++;
		//determine the escape character
		switch (*script->script_p)
		{
		case '\\': c = '\\'; break;
		case 'n': c = '\n'; break;
		case 'r': c = '\r'; break;
		case 't': c = '\t'; break;
		case 'v': c = '\v'; break;
		case 'b': c = '\b'; break;
		case 'f': c = '\f'; break;
		case 'a': c = '\a'; break;
		case '\'': c = '\''; break;
		case '\"': c = '\"'; break;
		case '\?': c = '\?'; break;
		case 'x':
		{
			script->script_p++;
			for (i = 0, val = 0; ; i++, script->script_p++)
			{
				c = *script->script_p;
				if (c >= '0' && c <= '9') c = c - '0';
				else if (c >= 'A' && c <= 'Z') c = c - 'A' + 10;
				else if (c >= 'a' && c <= 'z') c = c - 'a' + 10;
				else break;
				val = (val << 4) + c;
			}
			script->script_p--;
			if (val > 0xFF)
			{
				ScriptWarning(script, "too large value in escape character");
				val = 0xFF;
			}
			c = val;
			break;
		}
		default: //NOTE: decimal ASCII code, NOT octal
		{
			if (*script->script_p < '0' || *script->script_p > '9') ScriptError(script, "unknown escape char");
			for (i = 0, val = 0; ; i++, script->script_p++)
			{
				c = *script->script_p;
				if (c >= '0' && c <= '9') c = c - '0';
				else break;
				val = val * 10 + c;
			}
			script->script_p--;
			if (val > 0xFF)
			{
				ScriptWarning(script, "too large value in escape character");
				val = 0xFF;
			}
			c = val;
			break;
		}
		}
		//step over the escape character or the last digit of the number
		script->script_p++;
		//store the escape character
		*cha = (char)c;
		//successfully read escape character
		return 1;
	}

	int PS_ReadString(script_s* script, token_s* token, char quote)
	{
		int len, tmpline;
		char* tmpscript_p;

		if (quote == '\"') token->type = TT_STRING;
		else token->type = TT_LITERAL;

		len = 0;
		//leading quote
		token->string[len++] = *script->script_p++;

		while (1)
		{
			//minus 2 because trailing double quote and zero have to be appended
			if (len >= MAX_TOKEN - 2)
			{
				ScriptError(script, "string longer than MAX_TOKEN = %d", MAX_TOKEN);
				return 0;
			}
			//if there is an escape character and
			//if escape characters inside a string are allowed
			if (*script->script_p == '\\' && !(script->flags & SCFL_NOSTRINGESCAPECHARS))
			{
				if (!PS_ReadEscapeCharacter(script, &token->string[len]))
				{
					token->string[len] = 0;
					return 0;
				}
				len++;
			}
			//if a trailing quote
			else if (*script->script_p == quote)
			{
				//step over the double quote
				script->script_p++;
				//if white spaces in a string are not allowed
				if (script->flags & SCFL_NOSTRINGWHITESPACES) break;

				tmpscript_p = script->script_p;
				tmpline = script->line;
				//read unusefull stuff between possible two following strings
				if (!PS_ReadWhiteSpace(script))
				{
					script->script_p = tmpscript_p;
					script->line = tmpline;
					break;
				}
				//if there's no leading double qoute
				if (*script->script_p != quote)
				{
					script->script_p = tmpscript_p;
					script->line = tmpline;
					break;
				}
				//step over the new leading double quote
				script->script_p++;
			}
			else
			{
				if (*script->script_p == '\0')
				{
					token->string[len] = 0;
					ScriptError(script, "missing trailing quote");
					return 0;
				}
				if (*script->script_p == '\n')
				{
					token->string[len] = 0;
					ScriptError(script, "newline inside string %s", token->string);
					return 0;
				}
				token->string[len++] = *script->script_p++;
			}
		}
		//trailing quote
		token->string[len++] = quote;
		//end string with a zero
		token->string[len] = '\0';
		//the sub type is the length of the string
		token->subtype = len;
		return 1;
	}

	int PS_ReadPrimitive(script_s* script, token_s* token)
	{
		int len;

		len = 0;
		while (*script->script_p > ' ' && *script->script_p != ';')
		{
			if (len >= MAX_TOKEN)
			{
				ScriptError(script, "primitive token longer than MAX_TOKEN = %d", MAX_TOKEN);
				return 0;
			}
			token->string[len++] = *script->script_p++;
		}
		token->string[len] = 0;
		//copy the token into the script structure
		memcpy(&script->token, token, sizeof(token_s));
		//primitive reading successfull
		return 1;
	}

	int PS_ReadPunctuation(script_s* script, token_s* token)
	{
		std::size_t len;
		const char* p;
		punctuation_s* punc;

		for (punc = script->punctuationtable[(unsigned int)*script->script_p]; punc; punc = punc->next)
		{
			p = punc->p;
			len = strlen(p);
			//if the script contains at least as much characters as the punctuation
			if (script->script_p + len <= script->end_p)
			{
				//if the script contains the punctuation
				if (!strncmp(script->script_p, p, len))
				{
					strncpy(token->string, p, MAX_TOKEN);
					script->script_p += len;
					token->type = TT_PUNCTUATION;
					//sub type is the number of the punctuation
					token->subtype = punc->n;
					return 1;
				}
			}
		}
		return 0;
	}

	int PS_ReadName(script_s* script, token_s* token)
	{
		int len = 0;
		char c;

		token->type = TT_NAME;
		do
		{
			token->string[len++] = *script->script_p++;
			if (len >= MAX_TOKEN)
			{
				ScriptError(script, "name longer than MAX_TOKEN = %d", MAX_TOKEN);
				return 0;
			}
			c = *script->script_p;
		} while ((c >= 'a' && c <= 'z') ||
			(c >= 'A' && c <= 'Z') ||
			(c >= '0' && c <= '9') ||
			c == '_');
		token->string[len] = '\0';
		//the sub type is the length of the name
		token->subtype = len;
		return 1;
	}

	void NumberValue(char* string, int subtype, unsigned int* intvalue,
		float* floatvalue)
	{
		unsigned int dotfound = 0;

		*intvalue = 0;
		*floatvalue = 0;
		//floating point number
		if (subtype & TT_FLOAT)
		{
			while (*string)
			{
				if (*string == '.')
				{
					if (dotfound) return;
					dotfound = 10;
					string++;
				}
				if (dotfound)
				{
					*floatvalue = *floatvalue + (float)(*string - '0') /
						(float)dotfound;
					dotfound *= 10;
				}
				else
				{
					*floatvalue = *floatvalue * 10.0f + (float)(*string - '0');
				}
				string++;
			}
			*intvalue = (unsigned int)*floatvalue;
		}
		else if (subtype & TT_DECIMAL)
		{
			while (*string) *intvalue = *intvalue * 10 + (*string++ - '0');
			*floatvalue = (float)*intvalue;
		}
		else if (subtype & TT_HEX)
		{
			//step over the leading 0x or 0X
			string += 2;
			while (*string)
			{
				*intvalue <<= 4;
				if (*string >= 'a' && *string <= 'f') *intvalue += *string - 'a' + 10;
				else if (*string >= 'A' && *string <= 'F') *intvalue += *string - 'A' + 10;
				else *intvalue += *string - '0';
				string++;
			}
			*floatvalue = (float)*intvalue;
		}
		else if (subtype & TT_OCTAL)
		{
			//step over the first zero
			string += 1;
			while (*string) *intvalue = (*intvalue << 3) + (*string++ - '0');
			*floatvalue = (float)*intvalue;
		}
		else if (subtype & TT_BINARY)
		{
			//step over the leading 0b or 0B
			string += 2;
			while (*string) *intvalue = (*intvalue << 1) + (*string++ - '0');
			*floatvalue = (float)*intvalue;
		}
	}

	int PS_ReadNumber(script_s* script, token_s* token)
	{
		int len = 0, i;
		int octal, dot;
		char c;

		token->type = TT_NUMBER;
		//check for a hexadecimal number
		if (*script->script_p == '0' &&
			(*(script->script_p + 1) == 'x' ||
				*(script->script_p + 1) == 'X'))
		{
			token->string[len++] = *script->script_p++;
			token->string[len++] = *script->script_p++;
			c = *script->script_p;
			//hexadecimal
			while ((c >= '0' && c <= '9') ||
				(c >= 'a' && c <= 'f') ||
				(c >= 'A' && c <= 'A'))
			{
				token->string[len++] = *script->script_p++;
				if (len >= MAX_TOKEN)
				{
					ScriptError(script, "hexadecimal number longer than MAX_TOKEN = %d", MAX_TOKEN);
					return 0;
				}
				c = *script->script_p;
			}
			token->subtype |= TT_HEX;
		}
		//check for a binary number
		else if (*script->script_p == '0' &&
			(*(script->script_p + 1) == 'b' ||
				*(script->script_p + 1) == 'B'))
		{
			token->string[len++] = *script->script_p++;
			token->string[len++] = *script->script_p++;
			c = *script->script_p;
			//binary
			while (c == '0' || c == '1')
			{
				token->string[len++] = *script->script_p++;
				if (len >= MAX_TOKEN)
				{
					ScriptError(script, "binary number longer than MAX_TOKEN = %d", MAX_TOKEN);
					return 0;
				}
				c = *script->script_p;
			}
			token->subtype |= TT_BINARY;
		}
		else //decimal or octal integer or floating point number
		{
			octal = 0;
			dot = 0;
			if (*script->script_p == '0') octal = 1;
			while (1)
			{
				c = *script->script_p;
				if (c == '.') dot = 1;
				else if (c == '8' || c == '9') octal = 0;
				else if (c < '0' || c > '9') break;
				token->string[len++] = *script->script_p++;
				if (len >= MAX_TOKEN - 1)
				{
					ScriptError(script, "number longer than MAX_TOKEN = %d", MAX_TOKEN);
					return 0;
				}
			}
			if (octal) token->subtype |= TT_OCTAL;
			else token->subtype |= TT_DECIMAL;
			if (dot) token->subtype |= TT_FLOAT;
		}
		for (i = 0; i < 2; i++)
		{
			c = *script->script_p;
			//check for a LONG number
			if ((c == 'l' || c == 'L') // bk001204 - brackets 
				&& !(token->subtype & TT_LONG))
			{
				script->script_p++;
				token->subtype |= TT_LONG;
			}
			//check for an UNSIGNED number
			else if ((c == 'u' || c == 'U') // bk001204 - brackets 
				&& !(token->subtype & (TT_UNSIGNED | TT_FLOAT)))
			{
				script->script_p++;
				token->subtype |= TT_UNSIGNED;
			}
		}
		token->string[len] = '\0';
		NumberValue(token->string, token->subtype & 0xFFFFFFFF, &token->intvalue, &token->floatvalue);
		if (!(token->subtype & TT_FLOAT)) token->subtype |= TT_INTEGER;
		return 1;
	}

	int PS_ReadToken(script_s* script, token_s* token)
	{
		if (script->tokenavailable)
		{
			script->tokenavailable = 0;
			memcpy(token, &script->token, sizeof(token_s));
			return 1;
		}
		script->lastscript_p = script->script_p;
		script->lastline = script->line;
		memset(token, 0, sizeof(token_s));
		script->whitespace_p = script->script_p;
		token->whitespace_p = script->script_p;
		if (!PS_ReadWhiteSpace(script))
			return 0;
		script->endwhitespace_p = script->script_p;
		token->endwhitespace_p = script->script_p;
		token->line = script->line;
		token->linescrossed = script->line - script->lastline;
		if (*script->script_p == '"')
		{
			if (!PS_ReadString(script, token, '"'))
				return 0;
		}
		else if (*script->script_p != '#' || script->script_p[1] != '"')
		{
			if (*script->script_p == '\'')
			{
				if (!PS_ReadString(script, token, '\''))
					return 0;
			}
			else if ((*script->script_p < '0' || *script->script_p > '9')
				&& (*script->script_p != '.' || script->script_p[1] < '0' || script->script_p[1] > '9'))
			{
				if (script->flags & 0x10)
					return PS_ReadPrimitive(script, token);
				if ((*script->script_p < 'a' || *script->script_p > 'z')
					&& (*script->script_p < 'A' || *script->script_p > 'Z')
					&& *script->script_p != '_')
				{
					if (!PS_ReadPunctuation(script, token))
					{
						ScriptError(script, "can't read token");
						return 0;
					}
				}
				else if (!PS_ReadName(script, token))
				{
					return 0;
				}
			}
			else if (!PS_ReadNumber(script, token))
			{
				return 0;
			}
		}
		else if (!PS_ReadString(script, token, '#'))
		{
			return 0;
		}
		memcpy(&script->token, token, sizeof(script->token));
		return 1;
	}

	bool EndOfScript(script_s* script)
	{
		return script->script_p >= script->end_p;
	}

	void PC_PopIndent(source_s* source, int* type, parseSkip_t* skip)
	{
		indent_s* indent;

		*type = 0;
		*skip = SKIP_NO;

		indent = source->indentstack;
		if (!indent) return;

		//must be an indent from the current script
		if (source->indentstack->script != source->scriptstack) return;

		*type = indent->type;
		*skip = indent->skip;
		source->indentstack = source->indentstack->next;
		source->skip -= indent->skip;
		//FreeMemory(indent);
	}

	void PC_PushIndent(source_s* source, int type, parseSkip_t skip)
	{
		indent_s* indent;

		indent = mmem->allocate<indent_s>();
		indent->type = type;
		indent->script = source->scriptstack;
		indent->skip = skip;
		source->skip += indent->skip;
		indent->next = source->indentstack;
		source->indentstack = indent;
	}

	void PC_FreeToken(token_s* token)
	{
		//FreeMemory(token);
		--numtokens;
	}

	int PC_ReadSourceToken(source_s* source, token_s* token)
	{
		token_s* t;
		script_s* script;
		parseSkip_t skip;
		int type;

		//if there's no token already available
		while (!source->tokens)
		{
			//if there's a token to read from the script
			if (PS_ReadToken(source->scriptstack, token)) return 1;
			//if at the end of the script
			if (EndOfScript(source->scriptstack))
			{
				//remove all indents of the script
				while (source->indentstack &&
					source->indentstack->script == source->scriptstack)
				{
					SourceWarning(source, "missing #endif");
					PC_PopIndent(source, &type, &skip);
				}
			}
			//if this was the initial script
			if (!source->scriptstack->next) return 0;
			//remove the script and return to the last one
			script = source->scriptstack;
			source->scriptstack = source->scriptstack->next;
			//FreeScript(script);
		}
		//copy the already available token
		memcpy_s(token, sizeof(token_s), source->tokens, sizeof(token_s));
		//free the read token
		t = source->tokens;
		source->tokens = source->tokens->next;
		PC_FreeToken(t);
		return 1;
	}

	token_s* PC_CopyToken(token_s* token)
	{
		token_s* copy;

		copy = mmem->allocate<token_s>();
		if (copy)
		{
			memcpy(copy, token, sizeof(token_s));
			copy->next = 0;
			++numtokens;
		}
		else
		{
			throw std::runtime_error("Out of memory.");
			//copy = 0;
		}
		return copy;
	}

	int PC_UnreadSourceToken(source_s* source, token_s* token)
	{
		token_s* copy;

		copy = PC_CopyToken(token);
		copy->next = source->tokens;
		source->tokens = copy;
		return 1;
	}

	void PC_UnreadToken(source_s* source, token_s* token)
	{
		PC_UnreadSourceToken(source, token);
	}

	void PC_UnreadLastToken(source_s* source)
	{
		PC_UnreadSourceToken(source, &source->token);
	}

	void PC_UnreadLastTokenHandle()
	{
		if (sourceFile)
			PC_UnreadLastToken(sourceFile);
	}

	void UnreadSignToken(source_s* source)
	{
		token_s token;

		token.line = source->scriptstack->line;
		token.whitespace_p = source->scriptstack->script_p;
		token.endwhitespace_p = source->scriptstack->script_p;
		token.linescrossed = 0;
		strcpy(token.string, "-");
		token.type = TT_PUNCTUATION;
		token.subtype = P_SUB;
		PC_UnreadSourceToken(source, &token);
	}

	int PC_NameHash(char* name)
	{
		int hash;
		int i;

		hash = 0;
		for (i = 0; name[i]; ++i)
			hash += (i + 119) * name[i];
		return ((unsigned short)(hash >> 20) ^ (unsigned short)(hash ^ (hash >> 10))) & 0x3FF;
	}

	define_s* PC_FindHashedDefine(define_s** definehash, char* name)
	{
		define_s* d;
		int hash;

		hash = PC_NameHash(name);
		for (d = definehash[hash]; d; d = d->hashnext)
		{
			if (!strcmp(d->name, name)) return d;
		}
		return 0;
	}

	int PC_ExpandBuiltinDefine(source_s* source, token_s* deftoken, define_s* define, token_s** firsttoken, token_s** lasttoken)
	{
		token_s* token;
		time_t t;
		char* curtime;

		token = PC_CopyToken(deftoken);
		switch (define->builtin)
		{
		case BUILTIN_LINE:
			sprintf(token->string, "%d", deftoken->line);
			token->intvalue = deftoken->line;
			token->floatvalue = (float)deftoken->line;
			token->type = TT_NUMBER;
			token->subtype = TT_DECIMAL | TT_INTEGER;
			*firsttoken = token;
			*lasttoken = token;
			break;
		case BUILTIN_FILE:
			strcpy(token->string, source->scriptstack->filename);
			token->type = TT_NAME;
			token->subtype = strlen(token->string);
			*firsttoken = token;
			*lasttoken = token;
			break;
		case BUILTIN_DATE:
			t = time(0);
			curtime = ctime(&t);
			strcpy(token->string, "\"");
			strncat(token->string, curtime + 4, 7);
			strncat(token->string + 7, curtime + 20, 4);
			strcat(token->string, "\"");
			free(curtime);
			token->type = TT_NAME;
			token->subtype = strlen(token->string);
			*firsttoken = token;
			*lasttoken = token;
			break;
		case BUILTIN_TIME:
			t = time(0);
			curtime = ctime(&t);
			strcpy(token->string, "\"");
			strncat(token->string, curtime + 11, 8);
			strcat(token->string, "\"");
			free(curtime);
			token->type = TT_NAME;
			token->subtype = strlen(token->string);
			*firsttoken = token;
			*lasttoken = token;
			break;
		default:
			*firsttoken = 0;
			*lasttoken = 0;
			break;
		}
		return 1;
	}

	int PC_ReadDefineParms(source_s* source, define_s* define, token_s** parms, int maxparms)
	{
		token_s token;
		int i;

		if (!PC_ReadSourceToken(source, &token))
		{
			SourceError(source, "define %s missing parms", define->name);
			return 0;
		}
		if (define->numparms > maxparms)
		{
			SourceError(source, "define with more than %d parameters", maxparms);
			return 0;
		}
		for (i = 0; i < define->numparms; ++i)
			parms[i] = 0;
		if (strcmp(token.string, "("))
		{
			PC_UnreadSourceToken(source, &token);
			SourceError(source, "define %s missing parms", define->name);
			return 0;
		}

		int done = 0;
		int numparms = 0;
		int indent = 0;
		while (!done)
		{
			if (numparms >= maxparms)
			{
				SourceError(source, "define %s with too many parms", define->name);
				return 0;
			}
			if (numparms >= define->numparms)
			{
				SourceWarning(source, "define %s has too many parms", define->name);
				return 0;
			}
			parms[numparms] = 0;
			int lastcomma = 1;
			token_s* last = 0;
			while (1)
			{
				if (!PC_ReadSourceToken(source, &token))
				{
					SourceError(source, "define %s incomplete", define->name);
					return 0;
				}
				if (!strcmp(token.string, ",") && !indent)
				{
					if (lastcomma)
						SourceWarning(source, "too many comma's");
					break;
				}
				lastcomma = 0;
				if (!strcmp(token.string, "("))
				{
					++indent;
				}
				else if (!strcmp(token.string, ")"))
				{
					if (!indent)
					{
						if (!parms[define->numparms - 1])
							SourceWarning(source, "too few define parms to %s", define->name);
						done = 1;
						break;
					}
					--indent;
				}
				if (numparms < define->numparms)
				{
					token_s* t = PC_CopyToken(&token);
					t->next = 0;
					if (last)
						last->next = t;
					else
						parms[numparms] = t;
					last = t;
				}
			}
			++numparms;
		}
		return 1;
	}

	int PC_FindDefineParm(define_s* define, char* name)
	{
		token_s* p;
		int i;

		i = 0;
		for (p = define->parms; p; p = p->next)
		{
			if (!strcmp(p->string, name))
				return i;
			i++;
		}
		return -1;
	}

	int PC_StringizeTokens(token_s* tokens, token_s* token)
	{
		token_s* t;

		token->type = TT_STRING;
		token->whitespace_p = 0;
		token->endwhitespace_p = 0;
		token->string[0] = '\0';
		strcat(token->string, "\"");
		for (t = tokens; t; t = t->next)
		{
			strncat(token->string, t->string, MAX_TOKEN - strlen(token->string));
		}
		strncat(token->string, "\"", MAX_TOKEN - strlen(token->string));
		return 1;
	}

	int PC_MergeTokens(token_s* t1, token_s* t2)
	{
		if (t1->type == TT_NAME && (t2->type == TT_NAME || t2->type == TT_NUMBER))
		{
			strcat(t1->string, t2->string);
			return 1;
		}
		if (t1->type == TT_STRING && t2->type == TT_STRING)
		{
			t1->string[strlen(t1->string) - 1] = '\0';
			strcat(t1->string, &t2->string[1]);
			return 1;
		}
		return 0;
	}

	int PC_ExpandDefine(source_s* source, token_s* deftoken, define_s* define, token_s** firsttoken, token_s** lasttoken)
	{
		int parmnum;
		int parmnuma;
		token_s* dt;
		token_s* t;
		token_s* ta;
		token_s* tb;
		token_s* nextpt;
		token_s* parms[128];
		token_s* t2;
		token_s* pt;
		token_s token;
		int i;
		token_s* last;
		token_s* first;

		if (define->builtin)
			return PC_ExpandBuiltinDefine(source, deftoken, define, firsttoken, lasttoken);
		if (define->numparms && !PC_ReadDefineParms(source, define, parms, 128))
			return 0;
		first = 0;
		last = 0;
		for (dt = define->tokens; dt; dt = dt->next)
		{
			parmnum = -1;
			if (dt->type == TT_NAME)
				parmnum = PC_FindDefineParm(define, dt->string);
			if (parmnum < 0)
			{
				if (dt->string[0] != '#' || dt->string[1])
				{
					ta = PC_CopyToken(dt);
				}
				else
				{
					if (dt->next)
						parmnuma = PC_FindDefineParm(define, dt->next->string);
					else
						parmnuma = -1;
					if (parmnuma < 0)
					{
						SourceWarning(source, "stringizing operator without define parameter");
						continue;
					}
					dt = dt->next;
					if (!PC_StringizeTokens(parms[parmnuma], &token))
					{
						SourceError(source, "can't stringize tokens");
						return 0;
					}
					ta = PC_CopyToken(&token);
				}
				ta->next = 0;
				if (last)
					last->next = ta;
				else
					first = ta;
				last = ta;
			}
			else
			{
				for (pt = parms[parmnum]; pt; pt = pt->next)
				{
					t = PC_CopyToken(pt);
					t->next = 0;
					if (last)
						last->next = t;
					else
						first = t;
					last = t;
				}
			}
		}
		tb = first;
		while (tb)
		{
			if (tb->next && tb->next->string[0] == '#' && tb->next->string[1] == '#' && (t2 = tb->next->next) != 0)
			{
				if (!PC_MergeTokens(tb, t2))
				{
					SourceError(source, "can't merge %s with %s", tb->string, t2->string);
					return 0;
				}
				PC_FreeToken(tb->next);
				tb->next = t2->next;
				if (t2 == last)
					last = tb;
				PC_FreeToken(t2);
			}
			else
			{
				tb = tb->next;
			}
		}
		*firsttoken = first;
		*lasttoken = last;
		for (i = 0; i < define->numparms; ++i)
		{
			for (pt = parms[i]; pt; pt = nextpt)
			{
				nextpt = pt->next;
				PC_FreeToken(pt);
			}
		}
		return 1;
	}

	int PC_ExpandDefineIntoSource(source_s* source, token_s* deftoken, define_s* define)
	{
		token_s* firsttoken, * lasttoken;

		if (!PC_ExpandDefine(source, deftoken, define, &firsttoken, &lasttoken))
			return 1;

		if (firsttoken && lasttoken)
		{
			lasttoken->next = source->tokens;
			source->tokens = firsttoken;
			return 1;
		}
		return 0;
	}

	void StripDoubleQuotes(char* string)
	{
		if (*string == '\"')
		{
			strcpy(string, string + 1);
		}
		if (string[strlen(string) - 1] == '\"')
		{
			string[strlen(string) - 1] = '\0';
		}
	}

	int PC_OperatorPriority(int op)
	{
		switch (op)
		{
		case P_MUL: return 15;
		case P_DIV: return 15;
		case P_MOD: return 15;
		case P_ADD: return 14;
		case P_SUB: return 14;

		case P_LOGIC_AND: return 7;
		case P_LOGIC_OR: return 6;
		case P_LOGIC_GEQ: return 12;
		case P_LOGIC_LEQ: return 12;
		case P_LOGIC_EQ: return 11;
		case P_LOGIC_UNEQ: return 11;

		case P_LOGIC_NOT: return 16;
		case P_LOGIC_GREATER: return 12;
		case P_LOGIC_LESS: return 12;

		case P_RSHIFT: return 13;
		case P_LSHIFT: return 13;

		case P_BIN_AND: return 10;
		case P_BIN_OR: return 8;
		case P_BIN_XOR: return 9;
		case P_BIN_NOT: return 16;

		case P_COLON: return 5;
		case P_QUESTIONMARK: return 5;
		}
		return 0;
	}

	int PC_EvaluateTokens(source_s* source, token_s* tokens, int* intvalue, float* floatvalue, int integer)
	{
		operator_s operator_heap[MAX_OPERATORS];
		value_s value_heap[MAX_VALUES];
		operator_s* firstoperator = nullptr;
		operator_s* lastoperator = nullptr;
		value_s* firstvalue = nullptr;
		value_s* lastvalue = nullptr;
		int numoperators = 0;
		int numvalues = 0;
		int lastwasvalue = 0;
		int negativevalue = 0;
		int parentheses = 0;
		int brace = 0;
		int error = 0;

		if (intvalue)
			*intvalue = 0;
		if (floatvalue)
			*floatvalue = 0.0f;

		for (; tokens && !error; tokens = tokens->next)
		{
			switch (tokens->type)
			{
			case TT_NUMBER:
				if (lastwasvalue)
				{
					SourceError(source, "syntax error in #if/#elif");
					error = 1;
					break;
				}
				if (numvalues >= MAX_VALUES)
				{
					SourceError(source, "out of value space");
					error = 1;
					break;
				}
				{
					value_s* value = &value_heap[numvalues++];
					if (negativevalue)
					{
						value->intvalue = -(int)tokens->intvalue;
						value->floatvalue = -tokens->floatvalue;
					}
					else
					{
						value->intvalue = tokens->intvalue;
						value->floatvalue = tokens->floatvalue;
					}
					value->parentheses = parentheses;
					value->next = nullptr;
					value->prev = lastvalue;
					if (lastvalue)
						lastvalue->next = value;
					else
						firstvalue = value;
					lastvalue = value;
				}
				lastwasvalue = 1;
				negativevalue = 0;
				break;

			case TT_NAME:
				if (lastwasvalue || negativevalue)
				{
					SourceError(source, "syntax error in #if/#elif");
					error = 1;
					break;
				}
				if (strcmp(tokens->string, "defined"))
				{
					SourceError(source, "undefined name %s in #if/#elif", tokens->string);
					error = 1;
					break;
				}
				tokens = tokens->next;
				if (!strcmp(tokens->string, "("))
				{
					brace = 1;
					tokens = tokens->next;
				}
				if (tokens && tokens->type == TT_NAME)
				{
					if (numvalues >= MAX_VALUES)
					{
						SourceError(source, "out of value space");
						error = 1;
						break;
					}
					value_s* value = &value_heap[numvalues++];
					if (PC_FindHashedDefine(source->definehash, tokens->string))
					{
						value->intvalue = 1;
						value->floatvalue = 1.0f;
					}
					else
					{
						value->intvalue = 0;
						value->floatvalue = 0.0f;
					}
					value->parentheses = parentheses;
					value->next = nullptr;
					value->prev = lastvalue;
					if (lastvalue)
						lastvalue->next = value;
					else
						firstvalue = value;
					lastvalue = value;
					if (!brace || ((tokens = tokens->next) != nullptr && !strcmp(tokens->string, ")")))
					{
						brace = 0;
						lastwasvalue = 1;
					}
					else
					{
						SourceError(source, "defined without ) in #if/#elif");
						error = 1;
					}
				}
				else
				{
					if (tokens)
						SourceError(source, "defined without name in #if/#elif; got %s", tokens->string);
					else
						SourceError(source, "defined without name in #if/#elif; got %s", "end-of-file");
					error = 1;
				}
				break;

			case TT_PUNCTUATION:
				if (negativevalue)
				{
					SourceError(source, "misplaced minus sign in #if/#elif");
					error = 1;
				}
				else if (tokens->subtype == P_PARENTHESESOPEN)
				{
					++parentheses;
				}
				else if (tokens->subtype == P_PARENTHESESCLOSE)
				{
					if (--parentheses < 0)
					{
						SourceError(source, "too many ) in #if/#elsif");
						error = 1;
					}
				}
				else if (!integer
					&& (tokens->subtype == P_BIN_NOT
						|| tokens->subtype == P_MOD
						|| tokens->subtype == P_RSHIFT
						|| tokens->subtype == P_LSHIFT
						|| tokens->subtype == P_BIN_AND
						|| tokens->subtype == P_BIN_OR
						|| tokens->subtype == P_BIN_XOR))
				{
					SourceError(source, "illigal operator %s on floating point operands", tokens->string);
					error = 1;
				}
				else
				{
					switch (tokens->subtype)
					{
					case P_LOGIC_AND:
					case P_LOGIC_OR:
					case P_LOGIC_GEQ:
					case P_LOGIC_LEQ:
					case P_LOGIC_EQ:
					case P_LOGIC_UNEQ:
					case P_RSHIFT:
					case P_LSHIFT:
					case P_MUL:
					case P_DIV:
					case P_MOD:
					case P_ADD:
					case P_BIN_AND:
					case P_BIN_OR:
					case P_BIN_XOR:
					case P_LOGIC_GREATER:
					case P_LOGIC_LESS:
					case P_COLON:
					case P_QUESTIONMARK:
						if (!lastwasvalue)
						{
							SourceError(source, "operator %s after operator in #if/#elif", tokens->string);
							error = 1;
						}
						break;
					case P_INC:
					case P_DEC:
						SourceError(source, "++ or -- used in #if/#elif");
						break;
					case P_SUB:
						if (!lastwasvalue)
							negativevalue = 1;
						break;
					case P_BIN_NOT:
					case P_LOGIC_NOT:
						if (lastwasvalue)
						{
							SourceError(source, "! or ~ after value in #if/#elif");
							error = 1;
						}
						break;
					default:
						SourceError(source, "invalid operator %s in #if/#elif", tokens->string);
						error = 1;
						break;
					}
					if (!error && !negativevalue)
					{
						if (numoperators >= MAX_OPERATORS)
						{
							SourceError(source, "out of operator space");
							error = 1;
						}
						else
						{
							operator_s* op = &operator_heap[numoperators++];
							op->op = tokens->subtype & 0xFFFFFFFF;
							op->priority = PC_OperatorPriority(tokens->subtype & 0xFFFFFFFF);
							op->parentheses = parentheses;
							op->next = nullptr;
							op->prev = lastoperator;
							if (lastoperator)
								lastoperator->next = op;
							else
								firstoperator = op;
							lastoperator = op;
							lastwasvalue = 0;
						}
					}
				}
				break;

			default:
				SourceError(source, "unknown %s in #if/#elif", tokens->string);
				error = 1;
				break;
			}
		}

		if (!error)
		{
			if (!lastwasvalue)
			{
				SourceError(source, "trailing operator in #if/#elif");
				error = 1;
			}
			else if (parentheses)
			{
				SourceError(source, "too many ( in #if/#elif");
				error = 1;
			}
		}

		int gotquestmarkvalue = 0;
		int questmarkintvalue = 0;
		float questmarkfloatvalue = 0.0f;

		while (!error && firstoperator)
		{
			value_s* v = firstvalue;
			operator_s* o;
			for (o = firstoperator;
				o->next
				&& o->parentheses <= o->next->parentheses
				&& (o->parentheses != o->next->parentheses || o->priority < o->next->priority);
				o = o->next)
			{
				if (o->op != P_LOGIC_NOT && o->op != P_BIN_NOT)
					v = v->next;
				if (!v)
				{
					SourceError(source, "mising values in #if/#elif");
					error = 1;
					break;
				}
			}
			if (error)
				break;

			value_s* v1 = v;
			value_s* v2 = v->next;
			switch (o->op)
			{
			case P_LOGIC_AND:
				v1->intvalue = v1->intvalue && v2->intvalue;
				v1->floatvalue = (v1->floatvalue != 0.0 && v2->floatvalue != 0.0);
				break;
			case P_LOGIC_OR:
				v1->intvalue = v1->intvalue || v2->intvalue;
				v1->floatvalue = (v1->floatvalue != 0.0 || v2->floatvalue != 0.0);
				break;
			case P_LOGIC_GEQ:
				v1->intvalue = v1->intvalue >= v2->intvalue;
				v1->floatvalue = (v1->floatvalue >= v2->floatvalue);
				break;
			case P_LOGIC_LEQ:
				v1->intvalue = v1->intvalue <= v2->intvalue;
				v1->floatvalue = (v2->floatvalue >= v1->floatvalue);
				break;
			case P_LOGIC_EQ:
				v1->intvalue = v1->intvalue == v2->intvalue;
				v1->floatvalue = (v1->floatvalue == v2->floatvalue);
				break;
			case P_LOGIC_UNEQ:
				v1->intvalue = v1->intvalue != v2->intvalue;
				v1->floatvalue = (v1->floatvalue != v2->floatvalue);
				break;
			case P_RSHIFT:
				v1->intvalue >>= v2->intvalue;
				break;
			case P_LSHIFT:
				v1->intvalue <<= v2->intvalue;
				break;
			case P_MUL:
				v1->intvalue *= v2->intvalue;
				v1->floatvalue = v1->floatvalue * v2->floatvalue;
				break;
			case P_DIV:
				if (!v2->intvalue || v2->floatvalue == 0.0)
				{
					SourceError(source, "divide by zero in #if/#elif");
					error = 1;
					break;
				}
				v1->intvalue /= v2->intvalue;
				v1->floatvalue = v1->floatvalue / v2->floatvalue;
				break;
			case P_MOD:
				if (!v2->intvalue)
				{
					SourceError(source, "divide by zero in #if/#elif");
					error = 1;
					break;
				}
				v1->intvalue %= v2->intvalue;
				break;
			case P_ADD:
				v1->intvalue += v2->intvalue;
				v1->floatvalue = v1->floatvalue + v2->floatvalue;
				break;
			case P_SUB:
				v1->intvalue -= v2->intvalue;
				v1->floatvalue = v1->floatvalue - v2->floatvalue;
				break;
			case P_BIN_AND:
				v1->intvalue &= v2->intvalue;
				break;
			case P_BIN_OR:
				v1->intvalue |= v2->intvalue;
				break;
			case P_BIN_XOR:
				v1->intvalue ^= v2->intvalue;
				break;
			case P_BIN_NOT:
				v1->intvalue = ~v1->intvalue;
				break;
			case P_LOGIC_NOT:
				v1->intvalue = v1->intvalue == 0;
				v1->floatvalue = (float)(v1->floatvalue == 0.0);
				break;
			case P_LOGIC_GREATER:
				v1->intvalue = v1->intvalue > v2->intvalue;
				v1->floatvalue = (float)(v1->floatvalue > v2->floatvalue);
				break;
			case P_LOGIC_LESS:
				v1->intvalue = v1->intvalue < v2->intvalue;
				v1->floatvalue = (float)(v2->floatvalue > v1->floatvalue);
				break;
			case P_COLON:
				if (!gotquestmarkvalue)
				{
					SourceError(source, ": without ? in #if/#elif");
					error = 1;
					break;
				}
				if (integer)
				{
					if (!questmarkintvalue)
						v1->intvalue = v2->intvalue;
				}
				else if (questmarkfloatvalue == 0.0)
				{
					v1->floatvalue = v2->floatvalue;
				}
				gotquestmarkvalue = 0;
				break;
			case P_QUESTIONMARK:
				if (gotquestmarkvalue)
				{
					SourceError(source, "? after ? in #if/#elif");
					error = 1;
					break;
				}
				questmarkintvalue = v1->intvalue;
				questmarkfloatvalue = v1->floatvalue;
				gotquestmarkvalue = 1;
				break;
			default:
				break;
			}
			if (error)
				break;

			if (o->op != P_LOGIC_NOT && o->op != P_BIN_NOT)
			{
				if (o->op != P_QUESTIONMARK)
					v = v->next;
				if (v->prev)
					v->prev->next = v->next;
				else
					firstvalue = v->next;
				if (v->next)
					v->next->prev = v->prev;
				else
					lastvalue = v->prev;
			}
			if (o->prev)
				o->prev->next = o->next;
			else
				firstoperator = o->next;
			if (o->next)
				o->next->prev = o->prev;
		}

		if (firstvalue)
		{
			if (intvalue)
				*intvalue = firstvalue->intvalue;
			if (floatvalue)
				*floatvalue = firstvalue->floatvalue;
		}
		if (!error)
			return 1;
		if (intvalue)
			*intvalue = 0;
		if (floatvalue)
			*floatvalue = 0.0f;
		return 0;
	}

	int PC_DollarEvaluate(source_s* source, int* intvalue, float* floatvalue, int integer)
	{
		token_s* t;
		token_s* ta;
		token_s* tb;
		token_s* tc;
		define_s* define;
		int defined;
		token_s* nexttoken;
		int indent;
		token_s token;
		token_s* firsttoken;
		token_s* lasttoken;

		defined = 0;
		if (intvalue)
			*intvalue = 0;
		if (floatvalue)
			*floatvalue = 0.0f;
		if (!PC_ReadSourceToken(source, &token))
		{
			SourceError(source, "no leading ( after $evalint/$evalfloat");
			return 0;
		}
		if (!PC_ReadSourceToken(source, &token))
		{
			SourceError(source, "nothing to evaluate");
			return 0;
		}
		indent = 1;
		firsttoken = 0;
		lasttoken = 0;
		do
		{
			if (token.type == TT_NAME)
			{
				if (defined)
				{
					defined = 0;
					t = PC_CopyToken(&token);
					t->next = 0;
					if (lasttoken)
						lasttoken->next = t;
					else
						firsttoken = t;
					lasttoken = t;
				}
				else if (!strcmp(token.string, "defined"))
				{
					defined = 1;
					ta = PC_CopyToken(&token);
					ta->next = 0;
					if (lasttoken)
						lasttoken->next = ta;
					else
						firsttoken = ta;
					lasttoken = ta;
				}
				else
				{
					define = PC_FindHashedDefine(source->definehash, token.string);
					if (!define)
					{
						SourceError(source, "can't evaluate %s, not defined", token.string);
						return 0;
					}
					if (!PC_ExpandDefineIntoSource(source, &token, define))
						return 0;
				}
				continue;
			}
			if (token.type != TT_NUMBER && token.type != TT_PUNCTUATION)
			{
				SourceError(source, "can't evaluate %s", token.string);
				return 0;
			}
			if (token.string[0] == '(')
			{
				++indent;
			}
			else if (token.string[0] == ')')
			{
				--indent;
			}
			if (indent <= 0)
				break;
			tb = PC_CopyToken(&token);
			tb->next = 0;
			if (lasttoken)
				lasttoken->next = tb;
			else
				firsttoken = tb;
			lasttoken = tb;
		} while (PC_ReadSourceToken(source, &token));
		if (!PC_EvaluateTokens(source, firsttoken, intvalue, floatvalue, integer))
			return 0;
		for (tc = firsttoken; tc; tc = nexttoken)
		{
			nexttoken = tc->next;
			PC_FreeToken(tc);
		}
		return 1;
	}

	int PC_DollarDirective_evalint(source_s* source)
	{
		int value;
		token_s token;

		if (!PC_DollarEvaluate(source, &value, 0, 1))
			return 0;
		token.line = source->scriptstack->line;
		token.whitespace_p = source->scriptstack->script_p;
		token.endwhitespace_p = source->scriptstack->script_p;
		token.linescrossed = 0;
		sprintf(token.string, "%d", abs(value));
		token.type = TT_NUMBER;
		token.subtype = TT_INTEGER | TT_LONG | TT_DECIMAL;
		token.intvalue = value;
		token.floatvalue = (float)value;
		PC_UnreadSourceToken(source, &token);
		if (value < 0) UnreadSignToken(source);
		return 1;
	}

	int PC_DollarDirective_evalfloat(source_s* source)
	{
		float value;
		token_s token;

		if (!PC_DollarEvaluate(source, NULL, &value, 0))
			return 0;
		token.line = source->scriptstack->line;
		token.whitespace_p = source->scriptstack->script_p;
		token.endwhitespace_p = source->scriptstack->script_p;
		token.linescrossed = 0;
		sprintf(token.string, "%1.2f", fabs(value));
		token.type = TT_NUMBER;
		token.subtype = TT_FLOAT | TT_LONG | TT_DECIMAL;
		token.intvalue = (unsigned int)value;
		token.floatvalue = (float)value;
		PC_UnreadSourceToken(source, &token);
		if (value < 0) UnreadSignToken(source);
		return 1;
	}

	directive_s dollardirectives[] =
	{
		{"evalint", PC_DollarDirective_evalint},
		{"evalfloat", PC_DollarDirective_evalfloat},
		{0, 0}
	};

	int PC_ReadDollarDirective(source_s* source)
	{
		int result;
		token_s token;
		int i;

		if (PC_ReadSourceToken(source, &token))
		{
			if (token.linescrossed <= 0)
			{
				if (token.type == TT_NAME)
				{
					for (i = 0; dollardirectives[i].name; ++i)
					{
						if (!strcmp(dollardirectives[i].name, token.string))
							return dollardirectives[i].func(source);
					}
				}
				PC_UnreadSourceToken(source, &token);
				SourceError(source, "unknown precompiler directive %s", &token);
				result = 0;
			}
			else
			{
				PC_UnreadSourceToken(source, &token);
				SourceError(source, "found $ at end of line");
				result = 0;
			}
		}
		else
		{
			SourceError(source, "found $ without name");
			result = 0;
		}
		return result;
	}

	int PC_ReadLine(source_s* source, token_s* token, bool expandDefines)
	{
		int crossline;
		define_s* define;

		for (crossline = 0; ; crossline = 1)
		{
			while (1)
			{
				if (!PC_ReadSourceToken(source, token))
					return 0;
				if (token->linescrossed > crossline)
				{
					PC_UnreadSourceToken(source, token);
					return 0;
				}
				if (token->type != TT_NAME)
					break;
				if (!expandDefines)
					break;
				define = PC_FindHashedDefine(source->definehash, token->string);
				if (!define)
					break;
				if (!PC_ExpandDefineIntoSource(source, token, define))
					return 0;
			}
			if (strcmp(token->string, "\\"))
				break;
		}
		return 1;
	}

	int PC_ReadLineHandle(pc_token_s* pc_token)
	{
		token_s token;
		int ret;

		if (!sourceFile)
			return 0;
		ret = PC_ReadLine(sourceFile, &token, 1);
		strcpy(pc_token->string, token.string);
		pc_token->type = token.type;
		pc_token->subtype = token.subtype & 0xFFFFFFFF;
		pc_token->intvalue = token.intvalue;
		pc_token->floatvalue = token.floatvalue;
		if (pc_token->type == TT_STRING)
			StripDoubleQuotes(pc_token->string);
		return ret;
	}

	void PC_ConvertPath(char* path)
	{
		char* ptr;

		//remove double path seperators
		for (ptr = path; *ptr;)
		{
			if ((*ptr == '\\' || *ptr == '/') &&
				(*(ptr + 1) == '\\' || *(ptr + 1) == '/'))
			{
				strcpy(ptr, ptr + 1);
			} //end if
			else
			{
				ptr++;
			}
		}
		//set OS dependent path seperators
		for (ptr = path; *ptr;)
		{
			if (*ptr == '/' || *ptr == '\\')
				*ptr = '\\'; // PATHSEPERATOR_CHAR
			ptr++;
		}
	}

	void PC_PushScript(source_s* source, script_s* script)
	{
		script_s* s;

		for (s = source->scriptstack; s; s = s->next)
		{
			if (!_stricmp(s->filename, script->filename))
			{
				SourceError(source, "%s recursively included", script->filename);
				return;
			}
		}
		//push the script on the script stack
		script->next = source->scriptstack;
		source->scriptstack = script;
	}

	void PC_FreeDefine(define_s* define)
	{
		token_s* t, * next;

		//free the define parameters
		for (t = define->parms; t; t = next)
		{
			next = t->next;
			PC_FreeToken(t);
		}
		//free the define tokens
		for (t = define->tokens; t; t = next)
		{
			next = t->next;
			PC_FreeToken(t);
		}
		//free the define
		//FreeMemory(define);
	}

	define_s* PC_CopyDefine(source_s* source, define_s* define)
	{
		define_s* newdefine;
		token_s* token, * newtoken, * lasttoken;

		newdefine = mmem->manual_allocate<define_s>(strlen(define->name) + sizeof(define_s) + 1);
		//copy the define name
		newdefine->name = (char*)newdefine + sizeof(define_s);
		strcpy(newdefine->name, define->name);
		newdefine->flags = define->flags;
		newdefine->builtin = define->builtin;
		newdefine->numparms = define->numparms;
		//the define is not linked
		newdefine->next = 0;
		newdefine->hashnext = 0;
		//copy the define tokens
		newdefine->tokens = 0;
		for (lasttoken = NULL, token = define->tokens; token; token = token->next)
		{
			newtoken = PC_CopyToken(token);
			newtoken->next = 0;
			if (lasttoken) lasttoken->next = newtoken;
			else newdefine->tokens = newtoken;
			lasttoken = newtoken;
		}
		//copy the define parameters
		newdefine->parms = 0;
		for (lasttoken = NULL, token = define->parms; token; token = token->next)
		{
			newtoken = PC_CopyToken(token);
			newtoken->next = 0;
			if (lasttoken) lasttoken->next = newtoken;
			else newdefine->parms = newtoken;
			lasttoken = newtoken;
		}
		return newdefine;
	}

	void PC_AddDefineToHash(define_s* define, define_s** definehash)
	{
		int hash;

		hash = PC_NameHash(define->name);
		define->hashnext = definehash[hash];
		definehash[hash] = define;
	}

	void PC_AddGlobalDefinesToSource(source_s* source)
	{
		define_s* newdefine;
		define_s* define;

		for (define = globaldefines; define; define = define->next)
		{
			newdefine = PC_CopyDefine(source, define);
			PC_AddDefineToHash(newdefine, source->definehash);
		}
	}

	int Com_Compress(char* data_p)
	{
		char* in = data_p;
		char* out = data_p;
		int size = 0;

		if (in)
		{
			char c;
			while ((c = *in) != 0)
			{
				if (c == '/' && in[1] == '/')
				{
					// strip a // comment to the end of the line
					while (*in && *in != '\n')
						++in;
				}
				else if (c == '/' && in[1] == '*')
				{
					// strip a /* */ comment, but keep its newlines so line numbers stay correct
					while (*in && (*in != '*' || in[1] != '/'))
					{
						if (*in == '\n')
						{
							*out++ = '\n';
							++size;
						}
						++in;
					}
					if (*in)
						in += 2;
				}
				else
				{
					*out++ = c;
					++size;
					++in;
				}
			}
			*out = 0;
		}
		return size;
	}

	void PS_CreatePunctuationTable(script_s* script, punctuation_s* punctuations)
	{
		punctuation_s* lastp;
		int i;
		punctuation_s* newp;
		punctuation_s* p;

		if (!script->punctuationtable)
			script->punctuationtable = mmem->allocate<punctuation_s*>(256);
		for (i = 0; punctuations[i].p; ++i)
		{
			newp = &punctuations[i];
			lastp = 0;
			for (p = script->punctuationtable[*punctuations[i].p]; p; p = p->next)
			{
				if (strlen(p->p) < strlen(newp->p))
				{
					newp->next = p;
					if (lastp)
						lastp->next = newp;
					else
						script->punctuationtable[*newp->p] = newp;
					break;
				}
				lastp = p;
			}
			if (!p)
			{
				newp->next = 0;
				if (lastp)
					lastp->next = newp;
				else
					script->punctuationtable[*newp->p] = newp;
			}
		}
	}

	void SetScriptPunctuations(script_s* script)
	{
		PS_CreatePunctuationTable(script, default_punctuations);
		script->punctuations = default_punctuations;
	}

	script_s* LoadScriptFile(const char* filename)
	{
		script_s* script;
		FILE* fp;
		char pathname[64];
		std::size_t length;

		sprintf_s(pathname, 64, "%s", filename);
		auto file = filesystem::file(pathname);
		file.open("rb");
		fp = file.get_fp();
		length = file.size();
		if (!fp) return 0;
		script = mmem->manual_allocate<script_s>(length + sizeof(script_s) + 1);
		strcpy(script->filename, filename);
		script->buffer = (char*)script + sizeof(script_s);
		script->buffer[length] = 0;
		script->length = length;
		script->script_p = script->buffer;
		script->lastscript_p = script->buffer;
		script->end_p = &script->buffer[length];
		script->tokenavailable = 0;
		script->line = 1;
		script->lastline = 1;
		SetScriptPunctuations(script);
		memcpy(script->buffer, file.read_bytes(length).data(), length);
		file.close();
		script->length = Com_Compress(script->buffer);
		return script;
	}

	source_s* LoadSourceFile(const char* filename)
	{
		source_s* source;
		script_s* script;

		script = LoadScriptFile(filename);
		if (!script) return 0;
		script->next = 0;
		source = mmem->allocate<source_s>();
		//memset(source, 0, sizeof(source_s));
		strncpy(source->filename, filename, 0x40u);
		source->scriptstack = script;
		source->tokens = 0;
		source->defines = 0;
		source->indentstack = 0;
		source->skip = 0;
		source->definehash = mmem->allocate<define_s*>(1024);
		PC_AddGlobalDefinesToSource(source);
		return source;
	}

	script_s* LoadScriptMemory(const char* ptr, std::size_t length, const char* name)
	{
		script_s* script;

		script = mmem->manual_allocate<script_s>(length + sizeof(script_s) + 1);
		strcpy(script->filename, name);
		script->buffer = (char*)script + sizeof(script_s);
		script->buffer[length] = 0;
		script->length = length;
		script->script_p = script->buffer;
		script->lastscript_p = script->buffer;
		script->end_p = &script->buffer[length];
		script->tokenavailable = 0;
		script->line = 1;
		script->lastline = 1;
		SetScriptPunctuations(script);
		memcpy((unsigned char*)script->buffer, (unsigned char*)ptr, length);
		return script;
	}

	int PC_Evaluate(source_s* source, int* intvalue, float* floatvalue, int integer)
	{
		int result;
		token_s* t;
		token_s* ta;
		token_s* tb;
		token_s* tc;
		define_s* define;
		token_s* nexttoken;
		int defined;
		token_s token;
		token_s* firsttoken;
		token_s* lasttoken;

		defined = 0;
		if (intvalue)
			*intvalue = 0;
		if (floatvalue)
			*floatvalue = 0.0f;
		if (PC_ReadLine(source, &token, 1))
		{
			firsttoken = 0;
			lasttoken = 0;
			do
			{
				if (token.type == TT_NAME)
				{
					if (defined)
					{
						defined = 0;
						t = PC_CopyToken(&token);
						t->next = 0;
						if (lasttoken)
							lasttoken->next = t;
						else
							firsttoken = t;
						lasttoken = t;
					}
					else if (!strcmp(token.string, "defined"))
					{
						defined = 1;
						ta = PC_CopyToken(&token);
						ta->next = 0;
						if (lasttoken)
							lasttoken->next = ta;
						else
							firsttoken = ta;
						lasttoken = ta;
					}
					else
					{
						define = PC_FindHashedDefine(source->definehash, token.string);
						if (!define)
						{
							SourceError(source, "can't evaluate %s, not defined", token.string);
							return 0;
						}
						if (!PC_ExpandDefineIntoSource(source, &token, define))
							return 0;
					}
				}
				else
				{
					if (token.type != TT_NUMBER && token.type != TT_PUNCTUATION)
					{
						SourceError(source, "can't evaluate %s", token.string);
						return 0;
					}
					tb = PC_CopyToken(&token);
					tb->next = 0;
					if (lasttoken)
						lasttoken->next = tb;
					else
						firsttoken = tb;
					lasttoken = tb;
				}
			} while (PC_ReadLine(source, &token, defined == 0));
			if (PC_EvaluateTokens(source, firsttoken, intvalue, floatvalue, integer))
			{
				for (tc = firsttoken; tc; tc = nexttoken)
				{
					nexttoken = tc->next;
					PC_FreeToken(tc);
				}
				result = 1;
			}
			else
			{
				result = 0;
			}
		}
		else
		{
			SourceError(source, "no value after #if/#elif");
			result = 0;
		}
		return result;
	}

	int PC_Directive_if(source_s* source)
	{
		int value;

		if (!PC_Evaluate(source, &value, 0, 1))
			return 0;
		PC_PushIndent(source, 1, (parseSkip_t)(value == 0));
		return 1;
	}

	int PC_Directive_if_def(source_s* source, int type)
	{
		int result;
		define_s* d;
		token_s token;

		if (PC_ReadLine(source, &token, 0))
		{
			if (token.type == TT_NAME)
			{
				d = PC_FindHashedDefine(source->definehash, token.string);
				PC_PushIndent(source, type, (parseSkip_t)((type == 8) == (d == 0)));
				result = 1;
			}
			else
			{
				PC_UnreadSourceToken(source, &token);
				SourceError(source, "expected name after #ifdef, found %s", token.string);
				result = 0;
			}
		}
		else
		{
			SourceError(source, "#ifdef without name");
			result = 0;
		}
		return result;
	}

	int PC_Directive_ifdef(source_s* source)
	{
		return PC_Directive_if_def(source, 8);
	}

	int PC_Directive_ifndef(source_s* source)
	{
		return PC_Directive_if_def(source, 16);
	}

	int PC_Directive_elif(source_s* source)
	{
		int result;
		parseSkip_t skip;
		int type;
		int value;

		PC_PopIndent(source, &type, &skip);
		if (type && type != 2)
		{
			if (PC_Evaluate(source, &value, 0, 1))
			{
				if (skip == SKIP_YES)
					skip = value == 0 ? SKIP_YES : SKIP_NO;
				else
					skip = SKIP_ALL_ELIFS;
				PC_PushIndent(source, 4, skip);
				result = 1;
			}
			else
			{
				result = 0;
			}
		}
		else
		{
			SourceError(source, "misplaced #elif");
			result = 0;
		}
		return result;
	}

	int PC_Directive_else(source_s* source)
	{
		int result;
		parseSkip_t skip;
		int type;

		PC_PopIndent(source, &type, &skip);
		if (type)
		{
			if (type == 2)
			{
				SourceError(source, "#else after #else");
				result = 0;
			}
			else
			{
				PC_PushIndent(source, 2, (parseSkip_t)(skip != SKIP_YES));
				result = 1;
			}
		}
		else
		{
			SourceError(source, "misplaced #else");
			result = 0;
		}
		return result;
	}

	int PC_Directive_endif(source_s* source)
	{
		parseSkip_t skip;
		int type;

		PC_PopIndent(source, &type, &skip);
		if (type)
			return 1;
		SourceError(source, "misplaced #endif");
		return 0;
	}

	int PC_Directive_undef(source_s* source)
	{
		int result;
		define_s* lastdefine;
		int hash;
		define_s* define;
		token_s token;

		if (source->skip > 0)
			return 1;
		if (PC_ReadLine(source, &token, 0))
		{
			if (token.type == TT_NAME)
			{
				hash = PC_NameHash(token.string);
				lastdefine = 0;
				for (define = source->definehash[hash]; define; define = define->hashnext)
				{
					if (!strcmp(define->name, token.string))
					{
						if ((define->flags & 1) != 0)
						{
							SourceWarning(source, "can't undef %s", token.string);
						}
						else
						{
							if (lastdefine)
								lastdefine->hashnext = define->hashnext;
							else
								source->definehash[hash] = define->hashnext;
							PC_FreeDefine(define);
						}
						break;
					}
					lastdefine = define;
				}
				result = 1;
			}
			else
			{
				PC_UnreadSourceToken(source, &token);
				SourceError(source, "expected name, found %s", token.string);
				result = 0;
			}
		}
		else
		{
			SourceError(source, "undef without name");
			result = 0;
		}
		return result;
	}

	int PC_Directive_include(source_s* source)
	{
		script_s* script = nullptr;
		token_s token;
		char path[64] = {};

		if (source->skip > 0)
		{
			return 1;
		}

		if (!PC_ReadSourceToken(source, &token) || token.linescrossed > 0)
		{
			SourceError(source, "#include without file name");
			return 0;
		}

		if (token.type == TT_STRING)
		{
			StripDoubleQuotes(token.string);
			PC_ConvertPath(token.string);

			script = LoadScriptFile(token.string);
			strncpy(path, token.string, sizeof(path) - 1);

			if (!script)
			{
				snprintf(path, sizeof(path), "%s%s", source->includepath, token.string);
				script = LoadScriptFile(path);
			}
		}
		else if (token.type == TT_PUNCTUATION && token.string[0] == '<')
		{
			std::size_t len = snprintf(path, sizeof(path), "%s", source->includepath);
			const std::size_t base_len = len;

			while (PC_ReadSourceToken(source, &token))
			{
				if (token.linescrossed > 0)
				{
					PC_UnreadSourceToken(source, &token);
					break;
				}

				if (token.type == TT_PUNCTUATION && token.string[0] == '>')
				{
					break;
				}

				std::size_t token_len = strlen(token.string);

				if (len + token_len >= sizeof(path))
				{
					SourceError(source, "#include path too long");
					return 0;
				}

				memcpy(path + len, token.string, token_len);
				len += token_len;
				path[len] = '\0';
			}

			if (token.type != TT_PUNCTUATION || token.string[0] != '>')
			{
				SourceWarning(source, "#include missing trailing >");
			}

			if (len == base_len)
			{
				SourceError(source, "#include without file name between < >");
				return 0;
			}

			PC_ConvertPath(path);
			script = LoadScriptFile(path);
		}
		else
		{
			SourceError(source, "#include without file name");
			return 0;
		}

		if (!script)
		{
			SourceError(source, "file %s not found", path);
			return 0;
		}

		PC_PushScript(source, script);
		return 1;
	}

	int PC_Directive_define(source_s* source)
	{
		token_s token;

		if (source->skip > 0)
			return 1;
		if (!PC_ReadLine(source, &token, 0))
		{
			SourceError(source, "#define without name");
			return 0;
		}
		if (token.type != TT_NAME)
		{
			PC_UnreadSourceToken(source, &token);
			SourceError(source, "expected name after #define, found %s", token.string);
			return 0;
		}
		define_s* define = PC_FindHashedDefine(source->definehash, token.string);
		if (define)
		{
			if ((define->flags & 1) != 0)
			{
				SourceError(source, "can't redefine %s", token.string);
				return 0;
			}
			SourceWarning(source, "redefinition of %s", token.string);
			PC_UnreadSourceToken(source, &token);
			if (!PC_Directive_undef(source))
				return 0;
			PC_FindHashedDefine(source->definehash, token.string);
		}
		define_s* newdefine = mmem->manual_allocate<define_s>(strlen(token.string) + 1 + sizeof(define_s));
		newdefine->flags = 0;
		newdefine->builtin = 0;
		newdefine->numparms = 0;
		newdefine->parms = 0;
		newdefine->tokens = 0;
		newdefine->next = 0;
		newdefine->hashnext = 0;
		newdefine->name = (char*)&newdefine[1];
		strcpy(newdefine->name, token.string);
		PC_AddDefineToHash(newdefine, source->definehash);
		if (!PC_ReadLine(source, &token, 0))
			return 1;

		if (!PC_WhiteSpaceBeforeToken(&token) && !strcmp(token.string, "("))
		{
			token_s* lastparm = 0;
			if (!PC_CheckTokenString(source, ")"))
			{
				while (1)
				{
					if (!PC_ReadLine(source, &token, 0))
					{
						SourceError(source, "expected define parameter");
						return 0;
					}
					if (token.type != TT_NAME)
					{
						SourceError(source, "invalid define parameter");
						return 0;
					}
					if (PC_FindDefineParm(newdefine, token.string) >= 0)
					{
						SourceError(source, "two of the same define parameters");
						return 0;
					}
					token_s* t = PC_CopyToken(&token);
					PC_ClearTokenWhiteSpace(t);
					t->next = 0;
					if (lastparm)
						lastparm->next = t;
					else
						newdefine->parms = t;
					lastparm = t;
					++newdefine->numparms;
					if (!PC_ReadLine(source, &token, 0))
					{
						SourceError(source, "define parameters not terminated");
						return 0;
					}
					if (!strcmp(token.string, ")"))
						break;
					if (strcmp(token.string, ","))
					{
						SourceError(source, "define not terminated");
						return 0;
					}
				}
			}
			if (!PC_ReadLine(source, &token, 0))
				return 1;
		}

		token_s* last = 0;
		do
		{
			token_s* ta = PC_CopyToken(&token);
			if (ta->type == TT_NAME && !strcmp(ta->string, newdefine->name))
			{
				SourceError(source, "recursive define (removed recursion)");
			}
			else
			{
				PC_ClearTokenWhiteSpace(ta);
				ta->next = 0;
				if (last)
					last->next = ta;
				else
					newdefine->tokens = ta;
				last = ta;
			}
		} while (PC_ReadLine(source, &token, 0));
		if (!last || (strcmp(newdefine->tokens->string, "##") && strcmp(last->string, "##")))
			return 1;
		SourceError(source, "define with misplaced ##");
		return 0;
	}

	int PC_Directive_line(source_s* source)
	{
		SourceError(source, "#line directive not supported");
		return 0;
	}

	int PC_Directive_error(source_s* source)
	{
		token_s token;

		token.string[0] = 0;
		PC_ReadSourceToken(source, &token);
		if (source->skip)
			return 1;
		SourceError(source, "#error directive: %s", token.string);
		return 0;
	}

	int PC_Directive_pragma(source_s* source)
	{
		token_s token;

		SourceWarning(source, "#pragma directive not supported");
		while (PC_ReadLine(source, &token, 0));
		return 1;
	}

	int PC_Directive_eval(source_s* source)
	{
		token_s token;
		int value;

		if (!PC_Evaluate(source, &value, 0, 1))
			return 0;
		token.line = source->scriptstack->line;
		token.whitespace_p = source->scriptstack->script_p;
		token.endwhitespace_p = source->scriptstack->script_p;
		token.linescrossed = 0;
		sprintf(token.string, "%d", abs(value));
		token.type = TT_NUMBER;
		token.subtype = TT_INTEGER | TT_LONG | TT_DECIMAL;
		PC_UnreadSourceToken(source, &token);
		if (value < 0)
			UnreadSignToken(source);
		return 1;
	}

	int PC_Directive_evalfloat(source_s* source)
	{
		token_s token;
		float value;

		if (!PC_Evaluate(source, 0, &value, 0))
			return 0;
		token.line = source->scriptstack->line;
		token.whitespace_p = source->scriptstack->script_p;
		token.endwhitespace_p = source->scriptstack->script_p;
		token.linescrossed = 0;
		sprintf(token.string, "%1.2f", fabs((float)value));
		token.type = TT_NUMBER;
		token.subtype = TT_FLOAT | TT_LONG | TT_DECIMAL;
		PC_UnreadSourceToken(source, &token);
		if (value < 0.0f)
			UnreadSignToken(source);
		return 1;
	}

	directive_s directives[] =
	{
		{"if", PC_Directive_if},
		{"ifdef", PC_Directive_ifdef},
		{"ifndef", PC_Directive_ifndef},
		{"elif", PC_Directive_elif},
		{"else", PC_Directive_else},
		{"endif", PC_Directive_endif},
		{"include", PC_Directive_include},
		{"define", PC_Directive_define},
		{"undef", PC_Directive_undef},
		{"line", PC_Directive_line},
		{"error", PC_Directive_error},
		{"pragma", PC_Directive_pragma},
		{"eval", PC_Directive_eval},
		{"evalfloat", PC_Directive_evalfloat},
		{0, 0}
	};

	define_s* PC_DefineFromString(const char* string)
	{
		source_s src;
		token_s* t;
		define_s* def;
		script_s* script;
		int i;
		int res;

		script = LoadScriptMemory(string, strlen(string), "*extern");
		memset((unsigned char*)&src, 0, sizeof(src));
		strncpy(src.filename, "*extern", 0x40u);
		src.scriptstack = script;
		src.definehash = mmem->allocate<define_s*>(1024);
		res = PC_Directive_define(&src);
		for (t = src.tokens; t; t = src.tokens)
		{
			src.tokens = src.tokens->next;
			PC_FreeToken(t);
		}
		def = 0;
		for (i = 0; i < 1024; ++i)
		{
			if (src.definehash[i])
			{
				def = src.definehash[i];
				break;
			}
		}
		//FreeMemory(src.definehash);
		//FreeScript(script);
		if (res > 0)
			return def;
		if (src.defines)
			PC_FreeDefine(def);
		return 0;
	}

	int PC_AddDefine(source_s* source, const char* string)
	{
		define_s* define;

		define = PC_DefineFromString(string);
		if (!define)
			return 0;
		PC_AddDefineToHash(define, source->definehash);
		return 1;
	}

	int PC_LoadSourceHandle(const char* filename, const char** builtinDefines)
	{
		source_s* source;
		int defineIter;

		source = LoadSourceFile(filename);
		if (!source)
			return 0;
		if (builtinDefines)
		{
			for (defineIter = 0; builtinDefines[defineIter]; ++defineIter)
				PC_AddDefine(source, builtinDefines[defineIter]);
		}
		sourceFile = source;
		return 1;
	}

	int PC_ReadDirective(source_s* source)
	{
		int result;
		token_s token;
		int i;

		if (PC_ReadSourceToken(source, &token))
		{
			if (token.linescrossed <= 0)
			{
				if (token.type == TT_NAME)
				{
					for (i = 0; directives[i].name; ++i)
					{
						if (!strcmp(directives[i].name, token.string))
							return directives[i].func(source);
					}
				}
				SourceError(source, "unknown precompiler directive %s", &token);
				result = 0;
			}
			else
			{
				PC_UnreadSourceToken(source, &token);
				SourceError(source, "found # at end of line");
				result = 0;
			}
		}
		else
		{
			SourceError(source, "found # without name");
			result = 0;
		}
		return result;
	}

	int PC_ReadToken(source_s* source, token_s* token)
	{
		token_s newtoken;
		define_s* define;

		while (1)
		{
			do
			{
				while (1)
				{
					while (1)
					{
						if (!PC_ReadSourceToken(source, token))
							return 0;
						if (token->type != TT_PUNCTUATION || token->string[0] != '#' || token->string[1])
							break;
						if (!PC_ReadDirective(source))
							return 0;
					}
					if (token->type != TT_PUNCTUATION || token->string[0] != '$')
						break;
					if (!PC_ReadDollarDirective(source))
						return 0;
				}
			} while (source->skip);
			if (token->type == TT_STRING && PC_ReadToken(source, &newtoken))
			{
				if (newtoken.type == TT_STRING)
				{
					token->string[strlen(token->string) - 1] = 0;
					if (strlen(token->string) + &newtoken.string[strlen(&newtoken.string[1]) + 2] - &newtoken.string[2] + 1 >= 0x400)
					{
						SourceError(source, "string longer than MAX_TOKEN %d", 1024);
						return 0;
					}
					memcpy(
						&token->string[strlen(token->string)],
						&newtoken.string[1],
						&newtoken.string[strlen(&newtoken.string[1]) + 2] - &newtoken.string[1]);
				}
				else
				{
					PC_UnreadToken(source, &newtoken);
				}
			}
			if (token->type != 4)
				break;
			define = PC_FindHashedDefine(source->definehash, token->string);
			if (!define)
				break;
			if (!PC_ExpandDefineIntoSource(source, token, define))
				return 0;
		}
		memcpy(&source->token, token, sizeof(source->token));
		return 1;
	}

	int PC_CheckTokenString(source_s* source, const char* string)
	{
		token_s tok;

		if (!PC_ReadToken(source, &tok))
			return 0;
		if (!strcmp(tok.string, string))
			return 1;
		PC_UnreadSourceToken(source, &tok);
		return 0;
	}

	int PC_ReadTokenHandle(pc_token_s* pc_token)
	{
		token_s token;
		int ret;

		if (!sourceFile)
			return 0;
		ret = PC_ReadToken(sourceFile, &token);
		memcpy(pc_token->string, token.string, 0x400);
		pc_token->type = token.type;
		pc_token->subtype = token.subtype & 0xFFFFFFFF;
		pc_token->intvalue = token.intvalue;
		pc_token->floatvalue = token.floatvalue;
		if (pc_token->type == TT_STRING)
			StripDoubleQuotes(pc_token->string);
		return ret;
	}

	operationEnum Expression_GetOp(const char* token)
	{
		for (int opNum = 0; opNum < sizeof(g_expOperatorNames) / sizeof(const char*); opNum++)
		{
			if (!_stricmp(token, g_expOperatorNames[opNum]))
			{
				return operationEnum(opNum);
			}
		}

		return OP_NOOP;
	}

	bool Expression_OpIsFunction(operationEnum op)
	{
		return op >= OP_FIRSTFUNCTIONCALL;
	}

	std::string GetOperandAsString(Operand operand)
	{
		switch (operand.dataType)
		{
		case VAL_STRING:
			return utils::string::va("%s", operand.internals.stringVal.string);
		case VAL_INT:
			return utils::string::va("%i", operand.internals.intVal);
		case VAL_FLOAT:
			return utils::string::va("%f", operand.internals.floatVal);
		}
		return "";
	}

	bool Expression_Parse(Statement_s* statement, int maxTokens)
	{
		pc_token_s token;
		operationEnum op;
		operationEnum lastOp = OP_NOOP;
		Operand lastOperand{};
		int lastType = 2;
		int numOpenLeftParens = 0;

		while (PC_ReadTokenHandle(&token))
		{
			if (statement->numEntries == maxTokens)
			{
				PC_SourceError("Need to increment MAX_TOKENS_PER_STATEMENT - this statement has more than %i tokens", maxTokens);
				return false;
			}
			expressionEntry* newExpression = &statement->entries[statement->numEntries];

			int tokenType = token.type;
			if (token.string[0] == ';')
				break;

			op = (tokenType == TT_STRING) ? OP_NOOP : Expression_GetOp(token.string);

			bool isOperand = false;
			if (op == OP_NOOP)
			{
				switch (tokenType)
				{
				case TT_NUMBER:
					newExpression->type = 1;
					if (token.floatvalue == (float)token.intvalue)
					{
						newExpression->data.operand.dataType = VAL_INT;
						newExpression->data.operand.internals.intVal = token.intvalue;
					}
					else
					{
						newExpression->data.operand.dataType = VAL_FLOAT;
						newExpression->data.operand.internals.floatVal = token.floatvalue;
					}
					lastType = 1;
					lastOperand = newExpression->data.operand;
					isOperand = true;
					break;
				case TT_STRING:
				case TT_NAME:
					newExpression->type = 1;
					newExpression->data.operand.dataType = VAL_STRING;
					newExpression->data.operand.internals.stringVal.string = zmem->duplicate_string(token.string);
					lastType = 1;
					lastOperand = newExpression->data.operand;
					isOperand = true;
					break;
				default:
					PC_SourceError("Expression Error: Unknown token '%s'", token.string);
					break;
				}
			}

			if (isOperand)
			{
				++statement->numEntries;
				continue;
			}

			if (op == OP_LEFTPAREN)
			{
				++numOpenLeftParens;
				if (lastType != 2)
				{
					if (lastType)
					{
						PC_SourceError("Expression Error: %s(...", GetOperandAsString(lastOperand).data());
						return false;
					}
					if (Expression_OpIsFunction(lastOp))
					{
						lastOp = OP_LEFTPAREN;
						continue;
					}
				}
			}
			else if (op == OP_RIGHTPAREN)
			{
				if (--numOpenLeftParens < 0)
				{
					PC_SourceError("Expression Error: Found a right parenthesis that doesn't match any left parenthesis");
					return false;
				}
				if (!numOpenLeftParens)
					break;
			}

			newExpression->type = 0;
			newExpression->data.op = op;
			lastType = 0;
			lastOp = op;
			++statement->numEntries;
		}

		return true;
	}

	bool Expression_Read(Statement_s** statement)
	{
		int maxTokens = MAX_TOKENS_PER_STATEMENT;

		*statement = zmem->allocate<Statement_s>();
		(*statement)->entries = zmem->allocate<expressionEntry>(maxTokens);

		if (!Expression_Parse(*statement, maxTokens))
			return false;
		return true;
	}

	bool PC_String_Parse(const char** out)
	{
		pc_token_s token;

		if (!PC_ReadTokenHandle(&token))
			return false;
		*out = zmem->duplicate_string(token.string);
		return true;
	}

	enum EvalValueType : std::int32_t
	{
		EVAL_VALUE_FLOAT = 0x0,
		EVAL_VALUE_DOUBLE = 0x0,
		EVAL_VALUE_INT = 0x1,
		EVAL_VALUE_STRING = 0x2,
	};

	struct EvalValue
	{
		EvalValueType type;
		union
		{
			float f;
			double d;
			int i;
			char* s;
		} u;
	};

	enum EvalOperatorType : std::int32_t
	{
		EVAL_OP_LPAREN = 0x0,
		EVAL_OP_RPAREN = 0x1,
		EVAL_OP_COLON = 0x2,
		EVAL_OP_QUESTION = 0x3,
		EVAL_OP_PLUS = 0x4,
		EVAL_OP_MINUS = 0x5,
		EVAL_OP_UNARY_PLUS = 0x6,
		EVAL_OP_UNARY_MINUS = 0x7,
		EVAL_OP_MULTIPLY = 0x8,
		EVAL_OP_DIVIDE = 0x9,
		EVAL_OP_MODULUS = 0xA,
		EVAL_OP_LSHIFT = 0xB,
		EVAL_OP_RSHIFT = 0xC,
		EVAL_OP_BITWISE_NOT = 0xD,
		EVAL_OP_BITWISE_AND = 0xE,
		EVAL_OP_BITWISE_OR = 0xF,
		EVAL_OP_BITWISE_XOR = 0x10,
		EVAL_OP_LOGICAL_NOT = 0x11,
		EVAL_OP_LOGICAL_AND = 0x12,
		EVAL_OP_LOGICAL_OR = 0x13,
		EVAL_OP_EQUALS = 0x14,
		EVAL_OP_NOT_EQUAL = 0x15,
		EVAL_OP_LESS = 0x16,
		EVAL_OP_LESS_EQUAL = 0x17,
		EVAL_OP_GREATER = 0x18,
		EVAL_OP_GREATER_EQUAL = 0x19,
		EVAL_OP_COUNT = 0x1A,
	};

	struct Eval
	{
		EvalOperatorType opStack[1024];
		EvalValue valStack[1024];
		int opStackPos;
		int valStackPos;
		int parenCount;
		bool pushedOp;
	};

	bool Eval_IsUnaryOp(Eval* eval)
	{
		return !eval->valStackPos || eval->pushedOp;
	}

	bool Eval_CanPushValue(Eval* eval)
	{
		if (eval->valStackPos == 1024)
			throw std::runtime_error("evaluation stack overflow - expression is too complex");

		return Eval_IsUnaryOp(eval);
	}

	bool Eval_PushNumber(Eval* eval, float value)
	{
		if (!Eval_CanPushValue(eval))
			return false;

		eval->valStack[eval->valStackPos].type = EVAL_VALUE_FLOAT;
		eval->valStack[eval->valStackPos++].u.f = value;
		eval->pushedOp = false;

		return true;
	}

	bool Eval_PushInteger(Eval* eval, int value)
	{
		if (!Eval_CanPushValue(eval))
			return false;

		eval->valStack[eval->valStackPos].type = EVAL_VALUE_INT;
		eval->valStack[eval->valStackPos++].u.i = value;
		eval->pushedOp = false;

		return true;
	}

	bool Eval_OperatorForToken(const char* text, EvalOperatorType* op)
	{
		if (!text || !op)
			__debugbreak();

		switch (*text)
		{
		case '!':
			*op = (text[1] == '=') ? EVAL_OP_NOT_EQUAL : EVAL_OP_LOGICAL_NOT;
			return true;

		case '%':
			*op = EVAL_OP_MODULUS;
			return true;

		case '&':
			*op = (text[1] == '&') ? EVAL_OP_LOGICAL_AND : EVAL_OP_BITWISE_AND;
			return true;

		case '(':
			*op = EVAL_OP_LPAREN;
			return true;

		case ')':
			*op = EVAL_OP_RPAREN;
			return true;

		case '*':
			*op = EVAL_OP_MULTIPLY;
			return true;

		case '+':
			*op = EVAL_OP_PLUS;
			return true;

		case '-':
			*op = EVAL_OP_MINUS;
			return true;

		case '/':
			*op = EVAL_OP_DIVIDE;
			return true;

		case ':':
			*op = EVAL_OP_COLON;
			return true;

		case '<':
			if (text[1] == '<')
				*op = EVAL_OP_LSHIFT;
			else
				*op = (text[1] == '=') ? EVAL_OP_LESS_EQUAL : EVAL_OP_LESS;
			return true;

		case '=':
			if (text[1] != '=')
				return false;
			*op = EVAL_OP_EQUALS;
			return true;

		case '>':
			if (text[1] == '>')
				*op = EVAL_OP_RSHIFT;
			else
				*op = (text[1] == '=') ? EVAL_OP_GREATER_EQUAL : EVAL_OP_GREATER;
			return true;

		case '?':
			*op = EVAL_OP_QUESTION;
			return true;

		case '^':
			*op = EVAL_OP_BITWISE_XOR;
			return true;

		case '|':
			*op = (text[1] == '|') ? EVAL_OP_LOGICAL_OR : EVAL_OP_BITWISE_OR;
			return true;

		case '~':
			*op = EVAL_OP_BITWISE_NOT;
			return true;

		default:
			return false;
		}
	}

	void Eval_PrepareBinaryOpSameTypes(Eval* eval)
	{
		if (eval->valStackPos < 2)
			throw std::runtime_error("missing operand (for example, 'a + ' or ' / b')");

		EvalValue* lhs = &eval->valStack[eval->valStackPos - 2];
		EvalValue* rhs = &eval->valStack[eval->valStackPos - 1];
		if (lhs->type == EVAL_VALUE_STRING || rhs->type == EVAL_VALUE_STRING)
			throw std::runtime_error("operation not valid on strings");

		if (lhs->type != rhs->type)
		{
			if (lhs->type == EVAL_VALUE_INT)
			{
				lhs->u.f = (float)lhs->u.i;
				lhs->type = EVAL_VALUE_FLOAT;
			}
			else
			{
				rhs->u.f = (float)rhs->u.i;
				rhs->type = EVAL_VALUE_FLOAT;
			}
		}
	}

	void Eval_PrepareBinaryOpBoolean(Eval* eval)
	{
		if (eval->valStackPos < 2)
			throw std::runtime_error("missing operand (for example, 'a + ' or ' / b')");

		EvalValue* lhs = &eval->valStack[eval->valStackPos - 2];
		EvalValue* rhs = &eval->valStack[eval->valStackPos - 1];
		if (lhs->type == EVAL_VALUE_STRING || rhs->type == EVAL_VALUE_STRING)
			throw std::runtime_error("operation not valid on strings");

		if (lhs->type == EVAL_VALUE_FLOAT)
		{
			lhs->u.i = lhs->u.f != 0.0f;
			lhs->type = EVAL_VALUE_INT;
		}
		else
		{
			lhs->u.i = lhs->u.i != 0;
		}
		if (rhs->type == EVAL_VALUE_FLOAT)
		{
			rhs->u.i = rhs->u.f != 0.0f;
			rhs->type = EVAL_VALUE_INT;
		}
		else
		{
			rhs->u.i = rhs->u.i != 0;
		}
	}

	void Eval_PrepareBinaryOpIntegers(Eval* eval)
	{
		if (eval->valStackPos < 2)
			throw std::runtime_error("missing operand (for example, 'a + ' or ' / b')");

		EvalValue* lhs = &eval->valStack[eval->valStackPos - 2];
		EvalValue* rhs = &eval->valStack[eval->valStackPos - 1];
		if (lhs->type == EVAL_VALUE_STRING || rhs->type == EVAL_VALUE_STRING)
			throw std::runtime_error("operation not valid on strings");

		if (lhs->type == EVAL_VALUE_FLOAT)
		{
			lhs->u.i = (signed int)lhs->u.f;
			lhs->type = EVAL_VALUE_INT;
		}
		if (rhs->type == EVAL_VALUE_FLOAT)
		{
			rhs->u.i = (signed int)rhs->u.f;
			rhs->type = EVAL_VALUE_INT;
		}
	}

	bool Eval_EvaluationStep(Eval* eval)
	{
		if (!eval->opStackPos)
			return false;

		if (eval->opStack[--eval->opStackPos] == EVAL_OP_LPAREN)
			return true;
		if (eval->opStack[eval->opStackPos] == EVAL_OP_QUESTION)
			throw std::runtime_error("found '?' with no following ':' in expression of type 'a ? b : c'");
		if (!eval->valStackPos)
			throw std::runtime_error("missing operand (for example, 'a + ' or ' / b')");

		EvalOperatorType op = eval->opStack[eval->opStackPos];
		EvalValue* rhs = &eval->valStack[eval->valStackPos - 1];
		EvalValue* lhs = &eval->valStack[eval->valStackPos - 2];

		switch (op)
		{
		case EVAL_OP_COLON:
		{
			if (eval->valStackPos < 3)
				throw std::runtime_error("missing operand (for example, 'a + ' or ' / b')");

			EvalValue* condition = &eval->valStack[eval->valStackPos - 3];
			bool conditionTrue;
			if (condition->type == EVAL_VALUE_FLOAT)
			{
				conditionTrue = condition->u.f != 0.0f;
			}
			else
			{
				if (condition->type != EVAL_VALUE_INT)
					throw std::runtime_error("can only switch on numbers");
				conditionTrue = condition->u.i != 0;
			}

			EvalValue* selected = conditionTrue ? lhs : rhs;
			if (lhs->type == EVAL_VALUE_STRING && rhs->type == EVAL_VALUE_STRING)
				free((conditionTrue ? rhs : lhs)->u.s);
			else
				Eval_PrepareBinaryOpSameTypes(eval);

			*condition = *selected;
			eval->valStackPos -= 2;
			--eval->opStackPos;
			return true;
		}
		case EVAL_OP_QUESTION:
			__debugbreak();
			return false;
		case EVAL_OP_PLUS:
			if (lhs->type != EVAL_VALUE_STRING || rhs->type != EVAL_VALUE_STRING)
			{
				Eval_PrepareBinaryOpSameTypes(eval);
				if (lhs->type == EVAL_VALUE_INT)
					lhs->u.i += rhs->u.i;
				else
					lhs->u.f += rhs->u.f;
			}
			else
			{
				char* concatenated = (char*)malloc(strlen(lhs->u.s) + strlen(rhs->u.s) + 1);
				strcpy(concatenated, lhs->u.s);
				strcat(concatenated, rhs->u.s);
				free(lhs->u.s);
				free(rhs->u.s);
				lhs->u.s = concatenated;
			}
			--eval->valStackPos;
			break;
		case EVAL_OP_MINUS:
			Eval_PrepareBinaryOpSameTypes(eval);
			if (lhs->type == EVAL_VALUE_INT)
				lhs->u.i -= rhs->u.i;
			else
				lhs->u.f -= rhs->u.f;
			--eval->valStackPos;
			break;
		case EVAL_OP_UNARY_PLUS:
			break;
		case EVAL_OP_UNARY_MINUS:
			if (rhs->type == EVAL_VALUE_INT)
			{
				rhs->u.i = -rhs->u.i;
			}
			else
			{
				if (rhs->type == EVAL_VALUE_STRING)
					throw std::runtime_error("cannot negate strings");
				rhs->u.f = -rhs->u.f;
			}
			break;
		case EVAL_OP_MULTIPLY:
			Eval_PrepareBinaryOpSameTypes(eval);
			if (lhs->type == EVAL_VALUE_INT)
				lhs->u.i *= rhs->u.i;
			else
				lhs->u.f *= rhs->u.f;
			--eval->valStackPos;
			break;
		case EVAL_OP_DIVIDE:
			Eval_PrepareBinaryOpSameTypes(eval);
			if (lhs->type == EVAL_VALUE_INT)
			{
				if (!rhs->u.i)
					throw std::runtime_error("divide by zero");
				lhs->u.i /= rhs->u.i;
			}
			else
			{
				if (rhs->u.f == 0.0f)
					throw std::runtime_error("divide by zero");
				lhs->u.f /= rhs->u.f;
			}
			--eval->valStackPos;
			break;
		case EVAL_OP_MODULUS:
			Eval_PrepareBinaryOpSameTypes(eval);
			if (lhs->type == EVAL_VALUE_INT)
			{
				if (!rhs->u.i)
					throw std::runtime_error("divide by zero");
				lhs->u.i %= rhs->u.i;
			}
			else
			{
				if (rhs->u.f == 0.0)
					throw std::runtime_error("divide by zero");
				lhs->u.f -= rhs->u.f * floor(lhs->u.f / rhs->u.f);
			}
			--eval->valStackPos;
			break;
		case EVAL_OP_LSHIFT:
			Eval_PrepareBinaryOpSameTypes(eval);
			if (lhs->type == EVAL_VALUE_INT)
				lhs->u.i <<= rhs->u.i;
			else
				lhs->u.f = lhs->u.f * pow(2.0f, rhs->u.f);
			--eval->valStackPos;
			break;
		case EVAL_OP_RSHIFT:
			Eval_PrepareBinaryOpSameTypes(eval);
			if (lhs->type == EVAL_VALUE_INT)
				lhs->u.i >>= rhs->u.i;
			else
				lhs->u.f = lhs->u.f * pow(2.0f, -rhs->u.f);
			--eval->valStackPos;
			break;
		case EVAL_OP_BITWISE_NOT:
			if (rhs->type == EVAL_VALUE_FLOAT)
			{
				rhs->u.i = (signed int)rhs->u.f;
				rhs->type = EVAL_VALUE_INT;
			}
			else if (rhs->type == EVAL_VALUE_STRING)
			{
				throw std::runtime_error("cannot bitwise invert strings");
			}
			rhs->u.i = ~rhs->u.i;
			break;
		case EVAL_OP_BITWISE_AND:
			Eval_PrepareBinaryOpIntegers(eval);
			lhs->u.i &= rhs->u.i;
			--eval->valStackPos;
			break;
		case EVAL_OP_BITWISE_OR:
			Eval_PrepareBinaryOpIntegers(eval);
			lhs->u.i |= rhs->u.i;
			--eval->valStackPos;
			break;
		case EVAL_OP_BITWISE_XOR:
			Eval_PrepareBinaryOpIntegers(eval);
			lhs->u.i ^= rhs->u.i;
			--eval->valStackPos;
			break;
		case EVAL_OP_LOGICAL_NOT:
			if (rhs->type == EVAL_VALUE_FLOAT)
			{
				rhs->u.i = rhs->u.f == 0.0;
			}
			else
			{
				if (rhs->type != EVAL_VALUE_INT)
					throw std::runtime_error("cannot logical invert strings");
				rhs->u.i = rhs->u.i == 0;
			}
			rhs->type = EVAL_VALUE_INT;
			break;
		case EVAL_OP_LOGICAL_AND:
			Eval_PrepareBinaryOpBoolean(eval);
			lhs->u.i &= rhs->u.i;
			--eval->valStackPos;
			break;
		case EVAL_OP_LOGICAL_OR:
			Eval_PrepareBinaryOpBoolean(eval);
			lhs->u.i |= rhs->u.i;
			--eval->valStackPos;
			break;
		case EVAL_OP_EQUALS:
			if (lhs->type != EVAL_VALUE_STRING || rhs->type != EVAL_VALUE_STRING)
			{
				Eval_PrepareBinaryOpSameTypes(eval);
				if (lhs->type == EVAL_VALUE_INT)
				{
					lhs->u.i = lhs->u.i == rhs->u.i;
				}
				else
				{
					lhs->u.i = lhs->u.f == rhs->u.f;
					lhs->type = EVAL_VALUE_INT;
				}
			}
			else
			{
				bool equal = _stricmp(lhs->u.s, rhs->u.s) == 0;
				free(lhs->u.s);
				free(rhs->u.s);
				lhs->type = EVAL_VALUE_INT;
				lhs->u.i = equal;
			}
			--eval->valStackPos;
			break;
		case EVAL_OP_NOT_EQUAL:
			if (lhs->type != EVAL_VALUE_STRING || rhs->type != EVAL_VALUE_STRING)
			{
				Eval_PrepareBinaryOpSameTypes(eval);
				if (lhs->type == EVAL_VALUE_INT)
				{
					lhs->u.i = lhs->u.i != rhs->u.i;
				}
				else
				{
					lhs->u.i = lhs->u.f != rhs->u.f;
					lhs->type = EVAL_VALUE_INT;
				}
			}
			else
			{
				bool equal = _stricmp(lhs->u.s, rhs->u.s) == 0;
				free(lhs->u.s);
				free(rhs->u.s);
				lhs->type = EVAL_VALUE_INT;
				lhs->u.i = equal == 0;
			}
			--eval->valStackPos;
			break;
		case EVAL_OP_LESS:
			Eval_PrepareBinaryOpSameTypes(eval);
			if (lhs->type == EVAL_VALUE_INT)
			{
				lhs->u.i = lhs->u.i < rhs->u.i;
			}
			else
			{
				lhs->u.i = rhs->u.f > lhs->u.f;
				lhs->type = EVAL_VALUE_INT;
			}
			--eval->valStackPos;
			break;
		case EVAL_OP_LESS_EQUAL:
			Eval_PrepareBinaryOpSameTypes(eval);
			if (lhs->type == EVAL_VALUE_INT)
			{
				lhs->u.i = lhs->u.i <= rhs->u.i;
			}
			else
			{
				lhs->u.i = rhs->u.f >= lhs->u.f;
				lhs->type = EVAL_VALUE_INT;
			}
			--eval->valStackPos;
			break;
		case EVAL_OP_GREATER:
			Eval_PrepareBinaryOpSameTypes(eval);
			if (lhs->type == EVAL_VALUE_INT)
			{
				lhs->u.i = lhs->u.i > rhs->u.i;
			}
			else
			{
				lhs->u.i = lhs->u.f > rhs->u.f;
				lhs->type = EVAL_VALUE_INT;
			}
			--eval->valStackPos;
			break;
		case EVAL_OP_GREATER_EQUAL:
			Eval_PrepareBinaryOpSameTypes(eval);
			if (lhs->type == EVAL_VALUE_INT)
			{
				lhs->u.i = lhs->u.i >= rhs->u.i;
			}
			else
			{
				lhs->u.i = lhs->u.f >= rhs->u.f;
				lhs->type = EVAL_VALUE_INT;
			}
			--eval->valStackPos;
			break;
		default:
			break;
		}
		return true;
	}

	char s_precedence[26] =
	{
		0x63,
		0x00,
		0x03,
		0x02,
		0x0B,
		0x0B,
		0x0D,
		0x0D,
		0x0C,
		0x0C,
		0x0C,
		0x0A,
		0x0A,
		0x0D,
		0x07,
		0x05,
		0x06,
		0x0D,
		0x02,
		0x01,
		0x08,
		0x08,
		0x09,
		0x09,
		0x09,
		0x09
	};

	char s_rightToLeft[26] =
	{
		0x00,
		0x00,
		0x01,
		0x01,
		0x00,
		0x00,
		0x01,
		0x01,
		0x00,
		0x00,
		0x00,
		0x00,
		0x00,
		0x01,
		0x00,
		0x00,
		0x00,
		0x01,
		0x00,
		0x00,
		0x00,
		0x00,
		0x00,
		0x00,
		0x00,
		0x00
	};

	int s_consumedOperandCount[26] =
	{
		1,
		-1,
		2,
		0,
		1,
		1,
		0,
		0,
		1,
		1,
		1,
		1,
		1,
		0,
		1,
		1,
		1,
		0,
		1,
		1,
		1,
		1,
		1,
		1,
		1,
		1
	};

	bool Eval_PushOperator(Eval* eval, EvalOperatorType op)
	{
		if (s_precedence[op] < 0)
			return false;
		if (op == EVAL_OP_RPAREN && !eval->parenCount)
			return false;
		if (op == EVAL_OP_LPAREN)
		{
			if (eval->valStackPos && !eval->pushedOp)
				return false;
			++eval->parenCount;
		}
		if (op == EVAL_OP_PLUS)
		{
			if (Eval_IsUnaryOp(eval))
				op = EVAL_OP_UNARY_PLUS;
		}
		else if (op == EVAL_OP_MINUS && Eval_IsUnaryOp(eval))
		{
			op = EVAL_OP_UNARY_MINUS;
		}
		if ((op < EVAL_OP_LPAREN || op >= EVAL_OP_COUNT))
		{
			__debugbreak();
		}
		char precedence = s_precedence[op];
		while (eval->opStackPos > 0)
		{
			EvalOperatorType top = eval->opStack[eval->opStackPos - 1];
			bool higherPrecedence = s_precedence[top] > precedence;
			bool leftToRight = s_precedence[top] == precedence && !s_rightToLeft[top];
			if (!higherPrecedence && !leftToRight)
				break;
			if (top == EVAL_OP_LPAREN)
			{
				if (op == EVAL_OP_RPAREN)
				{
					--eval->parenCount;
					--eval->opStackPos;
					eval->pushedOp = 0;
					return true;
				}
				break;
			}
			if (!Eval_EvaluationStep(eval))
				return false;
		}
		if (op != EVAL_OP_COLON || eval->opStackPos && eval->opStack[eval->opStackPos - 1] == EVAL_OP_QUESTION)
		{
			if (eval->opStackPos == 1024)
				throw std::runtime_error("evaluation stack overflow - expression is too complex");
			eval->opStack[eval->opStackPos++] = op;
			eval->pushedOp = 1;
			return true;
		}
		if (eval->parenCount)
			throw std::runtime_error("found ':' without preceding '?' in expression of type 'a ? b : c'");
		return false;
	}

	bool Eval_AnyMissingOperands(const Eval* eval)
	{
		int opIndex;
		int requiredOperandCount;

		requiredOperandCount = 1;
		for (opIndex = 0; opIndex < eval->opStackPos; ++opIndex)
			requiredOperandCount += s_consumedOperandCount[eval->opStack[opIndex]];
		return requiredOperandCount != eval->valStackPos;
	}

	EvalValue* Eval_Solve(EvalValue* result, Eval* eval)
	{
		if (!(eval->parenCount == 0))
			throw std::runtime_error("missing ')'");
		while (Eval_EvaluationStep(eval));
		if (eval->opStackPos)
		{
			__debugbreak();
		}
		if (eval->valStackPos > 1)
			throw std::runtime_error("extra operand (for example, 'a b +')");
		*result = eval->valStack[0];
		return result;
	}

	bool PC_Float_Expression_Parse(float* f)
	{
		Eval eval;
		eval.opStackPos = 0;
		eval.valStackPos = 0;
		eval.parenCount = 0;
		eval.pushedOp = 0;

		int openParens = 0;
		EvalOperatorType op;
		pc_token_s pc_token;
		while (1)
		{
			if (!PC_ReadTokenHandle(&pc_token))
				return false;
			if (pc_token.type == TT_NUMBER)
			{
				if (!Eval_PushNumber(&eval, pc_token.floatvalue))
				{
					PC_SourceError("error evaluating expression");
					return false;
				}
				continue;
			}
			if (!Eval_OperatorForToken(pc_token.string, &op))
			{
				PC_SourceError("expected operator but found %s", pc_token.string);
				return false;
			}
			if (op == EVAL_OP_RPAREN)
			{
				if (!openParens)
					break;
				--openParens;
			}
			else if (op == EVAL_OP_LPAREN && ++openParens > 16)
			{
				PC_SourceError("too much recursive macro expansion");
				return false;
			}
			Eval_PushOperator(&eval, op);
		}
		if (Eval_AnyMissingOperands(&eval))
		{
			PC_SourceError("error evaluating expression");
			return false;
		}
		EvalValue result;
		Eval_Solve(&result, &eval);
		if (result.type != EVAL_VALUE_FLOAT)
		{
			__debugbreak();
		}
		*f = result.u.f;
		return true;
	}

	bool PC_Float_Parse(float* f)
	{
		pc_token_s token;
		int negative;

		if (!PC_ReadTokenHandle(&token))
			return false;
		if (token.string[0] == '(')
			return PC_Float_Expression_Parse(f);
		negative = 0;
		if (token.string[0] == '-')
		{
			if (!PC_ReadTokenHandle(&token))
				return false;
			negative = 1;
		}
		if (token.type == TT_NUMBER)
		{
			if (negative)
				*f = -token.floatvalue;
			else
				*f = token.floatvalue;
			return true;
		}
		else
		{
			PC_SourceError("expected float but found %s", token.string);
			return false;
		}
	}

	bool PC_Int_Expression_Parse(int* i)
	{
		Eval eval;
		eval.opStackPos = 0;
		eval.valStackPos = 0;
		eval.parenCount = 0;
		eval.pushedOp = 0;

		int openParens = 0;
		EvalOperatorType op;
		pc_token_s pc_token;
		while (1)
		{
			if (!PC_ReadTokenHandle(&pc_token))
				return false;
			if (pc_token.type == TT_NUMBER)
			{
				if (!Eval_PushInteger(&eval, pc_token.intvalue))
				{
					PC_SourceError("error evaluating expression");
					return false;
				}
				continue;
			}
			if (!Eval_OperatorForToken(pc_token.string, &op))
			{
				PC_SourceError("expected operator but found %s", pc_token.string);
				return false;
			}
			if (op == EVAL_OP_RPAREN)
			{
				if (!openParens)
					break;
				--openParens;
			}
			else if (op == EVAL_OP_LPAREN && ++openParens > 16)
			{
				PC_SourceError("too much recursive macro expansion");
				return false;
			}
			Eval_PushOperator(&eval, op);
		}
		if (Eval_AnyMissingOperands(&eval))
		{
			PC_SourceError("error evaluating expression");
			return false;
		}
		EvalValue result;
		Eval_Solve(&result, &eval);
		if (result.type != EVAL_VALUE_INT)
		{
			__debugbreak();
		}
		*i = result.u.i;
		return true;
	}

	bool PC_Int_Parse(int* i)
	{
		pc_token_s token;
		int negative;

		if (!PC_ReadTokenHandle(&token))
			return false;
		if (token.string[0] == '(')
			return PC_Int_Expression_Parse(i);
		negative = 0;
		if (token.string[0] == '-')
		{
			if (!PC_ReadTokenHandle(&token))
				return false;
			negative = 1;
		}
		if (token.type == TT_NUMBER)
		{
			*i = token.intvalue;
			if (negative)
				*i = -*i;
			return true;
		}
		else
		{
			PC_SourceError("expected integer but found %s", token.string);
			return false;
		}
	}

	bool PC_Int_ParseLine(int* i)
	{
		pc_token_s token;
		int negative;

		if (!PC_ReadLineHandle(&token))
			return false;
		negative = 0;
		if (token.string[0] == '-')
		{
			if (!PC_ReadLineHandle(&token))
				return false;
			negative = 1;
		}
		if (token.type == TT_NUMBER)
		{
			*i = token.intvalue;
			if (negative)
				*i = -*i;
			return true;
		}
		else
		{
			PC_SourceError("expected integer but found %s", token.string);
			return false;
		}
	}

	bool PC_Byte_Parse(unsigned char* b)
	{
		int i;

		if (!PC_Int_Parse(&i))
			return false;
		if (i >= 0 && i <= 255)
		{
			*b = (unsigned char)i;
			return true;
		}
		else
		{
			PC_SourceError("expect int value between 0 and 255, but got '%d'", i);
			return false;
		}
	}

	bool PC_Char_Parse(char* out)
	{
		pc_token_s token;

		if (!PC_ReadTokenHandle(&token))
			return false;
		*out = token.string[0];
		return true;
	}

	bool PC_Color_Parse(vec4_t* c)
	{
		float f;
		int i;

		for (i = 0; i < 4; ++i)
		{
			if (!PC_Float_Parse(&f))
				return false;
			(*c)[i] = f;
		}
		return true;
	}

	bool PC_Flag_ParseOptional(int* i)
	{
		pc_token_s token;

		if (!PC_ReadTokenHandle(&token))
			return false;
		if (token.type == TT_NUMBER)
		{
			*i = token.intvalue;
			return true;
		}
		else
		{
			PC_UnreadLastTokenHandle();
		}
		return false;
	}

	bool PC_Rect_Parse(rectDef_s* r)
	{
		int horzAlign;
		int vertAlign;

		if (!PC_Float_Parse(&r->x)
			|| !PC_Float_Parse(&r->y)
			|| !PC_Float_Parse(&r->w)
			|| !PC_Float_Parse(&r->h))
		{
			return false;
		}

		r->horzAlign = 0;
		r->vertAlign = 0;
		if (PC_Flag_ParseOptional(&horzAlign))
		{
			r->horzAlign = (unsigned char)horzAlign;
			if (PC_Flag_ParseOptional(&vertAlign))
			{
				r->vertAlign = (unsigned char)vertAlign;
			}
		}

		return true;
	}

	bool PC_Script_Parse(const char** out)
	{
		char dst[0x1400];
		pc_token_s pc_token;

		memset(dst, 0, sizeof(dst));
		if (!PC_ReadTokenHandle(&pc_token))
			return false;
		if (_stricmp(pc_token.string, "{"))
			return false;
		do
		{
			if (!PC_ReadTokenHandle(&pc_token))
				return false;
			if (!_stricmp(pc_token.string, "}"))
			{
				*out = zmem->duplicate_string(dst);
				return true;
			}
			if (strlen(pc_token.string) + &dst[strlen(dst) + 1] - &dst[1] > sizeof(dst))
				break;
			if (pc_token.type <= 0 || (pc_token.type > 2 && pc_token.type != 6))
			{
				snprintf(dst + strlen(dst), sizeof(dst), "%s", pc_token.string);
				if (pc_token.type != TT_PUNCTUATION || pc_token.subtype != P_SUB)
					snprintf(dst + strlen(dst), sizeof(dst), "%s", " ");
			}
			else if (pc_token.type <= 2)
			{
				snprintf(dst + strlen(dst), sizeof(dst), "\"%s\" ", pc_token.string);
			}
		} while ((unsigned int)(&dst[strlen(dst) + 1] - &dst[1] + 1) <= sizeof(dst));
		PC_SourceError("action block too long that starts with: %s", dst);
		return false;
	}

	bool PC_EventScript_Parse(MenuEventHandlerSet** eventHandlerSet)
	{
		pc_token_s token;
		MenuEventHandler* eventHandler;
		EventType eventType;
		char dst[0x1000] = { 0 };

		*eventHandlerSet = zmem->allocate<MenuEventHandlerSet>();
		(*eventHandlerSet)->eventHandlers = zmem->allocate<MenuEventHandler*>(MAX_EVENT_HANDLERS_PER_EVENT);

		if (!PC_ReadTokenHandle(&token))
			return false;
		if (token.string[0] != '{')
		{
			PC_SourceError("Expected '{'.");
			return false;
		}

		while (1)
		{
			if (!PC_ReadTokenHandle(&token))
			{
				PC_SourceError("Unexpected end of file.");
				return false;
			}

			if ((*eventHandlerSet)->eventHandlerCount == MAX_EVENT_HANDLERS_PER_EVENT)
			{
				PC_SourceError("Exceeded MAX_EVENT_HANDLERS_PER_EVENT(%d)", MAX_EVENT_HANDLERS_PER_EVENT);
				return false;
			}

			// end of event script
			if (token.string[0] == '}')
				break;

			eventType = EVENT_UNCONDITIONAL;
			if (!_stricmp(token.string, "if"))
				eventType = EVENT_IF;
			else if (!_stricmp(token.string, "else"))
				eventType = EVENT_ELSE;
			else if (!_stricmp(token.string, "setLocalVarBool"))
				eventType = EVENT_SET_LOCAL_VAR_BOOL;
			else if (!_stricmp(token.string, "setLocalVarInt"))
				eventType = EVENT_SET_LOCAL_VAR_INT;
			else if (!_stricmp(token.string, "setLocalVarFloat"))
				eventType = EVENT_SET_LOCAL_VAR_FLOAT;
			else if (!_stricmp(token.string, "setLocalVarString"))
				eventType = EVENT_SET_LOCAL_VAR_STRING;

			if (eventType == EVENT_IF)
			{
				eventHandler = zmem->allocate<MenuEventHandler>();
				eventHandler->eventType = eventType;
				eventHandler->eventData.conditionalScript = zmem->allocate<ConditionalScript>();
				if (!Expression_Read(&eventHandler->eventData.conditionalScript->eventExpression))
				{
					PC_SourceError("Could not read expression.");
					return false;
				}
				if (!PC_EventScript_Parse(&eventHandler->eventData.conditionalScript->eventHandlerSet))
				{
					PC_SourceError("Could not read event handler set.");
					return false;
				}
				(*eventHandlerSet)->eventHandlers[(*eventHandlerSet)->eventHandlerCount++] = eventHandler;
				continue;
			}
			else if (eventType == EVENT_ELSE)
			{
				if (!(*eventHandlerSet)->eventHandlerCount ||
					(*eventHandlerSet)->eventHandlers[(*eventHandlerSet)->eventHandlerCount - 1]->eventType != EVENT_IF)
				{
					PC_SourceError("Misplaced 'else'.");
					return false;
				}
				eventHandler = zmem->allocate<MenuEventHandler>();
				eventHandler->eventType = eventType;
				if (!PC_EventScript_Parse(&eventHandler->eventData.elseScript))
				{
					PC_SourceError("Could not read event handler set.");
					return false;
				}
				(*eventHandlerSet)->eventHandlers[(*eventHandlerSet)->eventHandlerCount++] = eventHandler;
				continue;
			}
			else if (eventType == EVENT_SET_LOCAL_VAR_BOOL ||
				eventType == EVENT_SET_LOCAL_VAR_INT ||
				eventType == EVENT_SET_LOCAL_VAR_FLOAT ||
				eventType == EVENT_SET_LOCAL_VAR_STRING)
			{
				if (!PC_ReadTokenHandle(&token))
				{
					PC_SourceError("Expected local var name.");
					return false;
				}
				eventHandler = zmem->allocate<MenuEventHandler>();
				eventHandler->eventType = eventType;
				eventHandler->eventData.setLocalVarData = zmem->allocate<SetLocalVarData>();
				eventHandler->eventData.setLocalVarData->localVarName = zmem->duplicate_string(token.string);
				if (!Expression_Read(&eventHandler->eventData.setLocalVarData->expression))
				{
					PC_SourceError("Could not read expression.");
					return false;
				}
				(*eventHandlerSet)->eventHandlers[(*eventHandlerSet)->eventHandlerCount++] = eventHandler;
				continue;
			}
			else
			{
				memset(dst, 0, sizeof(dst));
				PC_UnreadLastTokenHandle();

				if (!PC_ReadTokenHandle(&token))
					return false;
				if (!_stricmp(token.string, ";"))
				{
					continue;
				}
				PC_UnreadLastTokenHandle();
				do
				{
					if (!PC_ReadTokenHandle(&token))
						return false;
					if (!_stricmp(token.string, "if"))
					{
						PC_UnreadLastTokenHandle();
						break;
					}
					else if (!_stricmp(token.string, "}"))
					{
						PC_UnreadLastTokenHandle();
						break;
					}
					else if (!_stricmp(token.string, "{"))
					{
						PC_SourceError("Unexpected '{' in an event handler.");
						return false;
					}
					if (strlen(token.string) + strlen(dst) + 1 >= sizeof(dst))
					{
						PC_SourceError("action block too long that starts with: %s", dst);
						return false;
					}
					snprintf(dst + strlen(dst), sizeof(dst), "\"%s\" ", token.string);
				} while (strlen(dst) + 1 < sizeof(dst));

				eventHandler = zmem->allocate<MenuEventHandler>();
				eventHandler->eventType = eventType;
				eventHandler->eventData.unconditionalScript = zmem->duplicate_string(dst);

				(*eventHandlerSet)->eventHandlers[(*eventHandlerSet)->eventHandlerCount++] = eventHandler;
				continue;
			}
		}

		return true;
	}

	editFieldDef_s* Item_GetEditFieldDef(itemDef_t* item)
	{
		editFieldDef_s* result;

		switch (item->dataType)
		{
		case ITEM_TYPE_TEXT:
		case ITEM_TYPE_EDITFIELD:
		case ITEM_TYPE_NUMERICFIELD:
		case ITEM_TYPE_SLIDER:
		case ITEM_TYPE_YESNO:
		case ITEM_TYPE_BIND:
		case ITEM_TYPE_VALIDFILEFIELD:
		case ITEM_TYPE_DECIMALFIELD:
		case ITEM_TYPE_UPREDITFIELD:
		case ITEM_TYPE_EMAILFIELD:
		case ITEM_TYPE_PASSWORDFIELD:
			result = item->typeData.editField;
			break;
		default:
			ZONETOOL_ERROR(
				"Menu Error: Expecting type: ITEM_TYPE_EDITFIELD, ITEM_TYPE_NUMERICFIELD, ITEM_TYPE_DECIMALFIELD, ITEM_TYPE_VALID"
				"FILEFIELD, ITEM_TYPE_UPREDITFIELD, ITEM_TYPE_YESNO, ITEM_TYPE_BIND, ITEM_TYPE_SLIDER, ITEM_TYPE_EMAILFIELD, ITEM"
				"_TYPE_PASSWORDFIELD, or ITEM_TYPE_TEXT");
			result = 0;
			break;
		}
		return result;
	}

	listBoxDef_s* Item_GetListBoxDef(itemDef_t* item)
	{
		if (item->dataType == ITEM_TYPE_LISTBOX)
			return item->typeData.listBox;
		ZONETOOL_ERROR("Menu Error: Expecting type: ITEM_TYPE_LISTBOX");
		return 0;
	}

	multiDef_s* Item_GetMultiDef(itemDef_t* item)
	{
		if (item->dataType == ITEM_TYPE_MULTI)
			return item->typeData.multi;
		ZONETOOL_ERROR("Menu Error: Expecting type: ITEM_TYPE_MULTI");
		return 0;
	}

	const char* Item_GetEnumDvarName(itemDef_t* item)
	{
		if (item->dataType == ITEM_TYPE_DVARENUM)
			return item->typeData.enumDvarName;
		ZONETOOL_ERROR("Menu Error: Expecting type: ITEM_TYPE_DVARENUM");
		return 0;
	}

	newsTickerDef_s* Item_GetNewsTickerDef(itemDef_t* item)
	{
		if (item->dataType == ITEM_TYPE_NEWSTICKER)
			return item->typeData.ticker;
		ZONETOOL_ERROR("Menu Error: Expecting type: ITEM_TYPE_NEWSTICKER");
		return 0;
	}

	void Item_ValidateTypeData(itemDef_t* item)
	{
		editFieldDef_s* editDef;

		if (item->typeData.data)
		{
			if (item->dataType != item->type)
				PC_SourceError(

					"Attempting to change type from %d to %d.\nMove the type definition higher up in the itemDef.",
					item->dataType,
					item->type);
		}
		else
		{
			item->dataType = item->type;
			switch (item->type)
			{
			case ITEM_TYPE_LISTBOX:
				item->typeData.listBox = zmem->allocate<listBoxDef_s>();
				break;
			case ITEM_TYPE_EDITFIELD:
			case ITEM_TYPE_NUMERICFIELD:
			case ITEM_TYPE_VALIDFILEFIELD:
			case ITEM_TYPE_UPREDITFIELD:
			case ITEM_TYPE_YESNO:
			case ITEM_TYPE_BIND:
			case ITEM_TYPE_SLIDER:
			case ITEM_TYPE_TEXT:
			case ITEM_TYPE_DECIMALFIELD:
			case ITEM_TYPE_EMAILFIELD:
			case ITEM_TYPE_PASSWORDFIELD:
				item->typeData.editField = zmem->allocate<editFieldDef_s>();
				if (item->type == ITEM_TYPE_EDITFIELD
					|| item->type == ITEM_TYPE_VALIDFILEFIELD
					|| item->type == ITEM_TYPE_NUMERICFIELD
					|| item->type == ITEM_TYPE_UPREDITFIELD
					|| item->type == ITEM_TYPE_DECIMALFIELD
					|| item->type == ITEM_TYPE_EMAILFIELD
					|| item->type == ITEM_TYPE_PASSWORDFIELD)
				{
					editDef = Item_GetEditFieldDef(item);
					if (!editDef->maxPaintChars)
						editDef->maxPaintChars = 256;
				}
				break;
			case ITEM_TYPE_MULTI:
				item->typeData.multi = zmem->allocate<multiDef_s>();
				break;
			case ITEM_TYPE_NEWSTICKER:
				item->typeData.ticker = zmem->allocate<newsTickerDef_s>();
				break;
			case ITEM_TYPE_TEXTSCROLL:
				item->typeData.scroll = zmem->allocate<textScrollDef_s>();
				break;
			}
		}
	}

	bool Item_IsEditFieldDef(itemDef_t* item)
	{
		bool result;

		switch (item->dataType)
		{
		case ITEM_TYPE_TEXT:
		case ITEM_TYPE_EDITFIELD:
		case ITEM_TYPE_NUMERICFIELD:
		case ITEM_TYPE_SLIDER:
		case ITEM_TYPE_YESNO:
		case ITEM_TYPE_BIND:
		case ITEM_TYPE_VALIDFILEFIELD:
		case ITEM_TYPE_DECIMALFIELD:
		case ITEM_TYPE_UPREDITFIELD:
		case ITEM_TYPE_EMAILFIELD:
		case ITEM_TYPE_PASSWORDFIELD:
			result = true;
			break;
		default:
			result = false;
			break;
		}
		return result;
	}

	void Window_AddDynamicFlags(windowDef_t* w, const int flags)
	{
		w->dynamicFlags[0] |= flags;
	}

	int Window_GetDynamicFlags(const windowDef_t* w)
	{
		return w->dynamicFlags[0];
	}

	void Window_SetDynamicFlags(windowDef_t* w, const int flags)
	{
		w->dynamicFlags[0] = flags;
	}

	int Window_GetStaticFlags(const windowDef_t* w)
	{
		return w->staticFlags;
	}

	void Window_SetStaticFlags(windowDef_t* w, const int flags)
	{
		w->staticFlags = flags;
	}

	bool SetItemStaticFlag(menuDef_t* menu, int flag)
	{
		if (!menu)
			__debugbreak();
		Window_SetStaticFlags(&menu->window, flag | menu->window.staticFlags);
		return true;
	}

	bool MenuParse_name(menuDef_t* menu)
	{
		if (!PC_String_Parse(&menu->window.name))
			return false;
		_strlwr((char*)menu->window.name);
		return true;
	}

	bool MenuParse_fullscreen(menuDef_t* menu)
	{
		return PC_Int_Parse(&menu->data->fullScreen);
	}

	bool MenuParse_screenSpace(menuDef_t* menu)
	{
		SetItemStaticFlag(menu, WINDOWSTATIC_SCREENSPACE);
		return true;
	}

	bool MenuParse_decoration(menuDef_t* menu)
	{
		SetItemStaticFlag(menu, WINDOWSTATIC_DECORATION);
		return true;
	}

	bool MenuParse_rect(menuDef_t* menu)
	{
		return PC_Rect_Parse(&menu->window.rect);
	}

	bool MenuParse_rect720(menuDef_t* menu)
	{
		// TODO:
		return false;
	}

	bool MenuParse_pos(menuDef_t* menu)
	{
		// TODO:
		return false;
	}

	bool MenuParse_pos720(menuDef_t* menu)
	{
		// TODO:
		return false;
	}

	bool MenuParse_group(menuDef_t* menu)
	{
		if (!PC_String_Parse(&menu->window.group))
			return false;
		_strlwr((char*)menu->window.group);
		return true;
	}

	bool MenuParse_style(menuDef_t* menu)
	{
		return PC_Int_Parse(&menu->window.style);
	}

	bool MenuParse_visible(menuDef_t* menu)
	{
		pc_token_s token;

		if (!PC_ReadTokenHandle(&token))
			return false;
		if (!_stricmp(token.string, "when") || !_stricmp(token.string, "if"))
		{
			Window_AddDynamicFlags(&menu->window, WINDOWDYNAMIC_VISIBLE);
			return Expression_Read(&menu->data->visibleExp);
		}
		if (atoi(token.string))
		{
			Window_AddDynamicFlags(&menu->window, WINDOWDYNAMIC_VISIBLE);
		}
		return true;
	}

	bool MenuParse_onOpen(menuDef_t* menu)
	{
		return PC_EventScript_Parse(&menu->data->onOpen);
	}

	bool MenuParse_onClose(menuDef_t* menu)
	{
		return PC_EventScript_Parse(&menu->data->onClose);
	}

	bool MenuParse_onCloseRequest(menuDef_t* menu)
	{
		return PC_EventScript_Parse(&menu->data->onCloseRequest);
	}

	bool MenuParse_onESC(menuDef_t* menu)
	{
		return PC_EventScript_Parse(&menu->data->onESC);
	}

	bool MenuParse_border(menuDef_t* menu)
	{
		return PC_Int_Parse(&menu->window.border);
	}

	bool MenuParse_borderSize(menuDef_t* menu)
	{
		return PC_Float_Parse(&menu->window.borderSize);
	}

	bool MenuParse_backcolor(menuDef_t* menu)
	{
		return PC_Color_Parse(&menu->window.backColor);
	}

	bool MenuParse_forecolor(menuDef_t* menu)
	{
		if (!PC_Color_Parse(&menu->window.foreColor))
			return false;
		Window_AddDynamicFlags(&menu->window, WINDOWDYNAMIC_FORECOLOR);
		return true;
	}

	bool MenuParse_bordercolor(menuDef_t* menu)
	{
		return PC_Color_Parse(&menu->window.borderColor);
	}

	bool MenuParse_focuscolor(menuDef_t* menu)
	{
		return PC_Color_Parse(&menu->data->focusColor);
	}

	bool MenuParse_outlinecolor(menuDef_t* menu)
	{
		return PC_Color_Parse(&menu->window.outlineColor);
	}

	bool MenuParse_background(menuDef_t* menu)
	{
		const char* name;

		if (!PC_String_Parse(&name))
			return false;
		menu->window.background = zmem->manual_allocate<Material>(sizeof(char*));
		menu->window.background->name = name;
		_strlwr((char*)menu->window.background->name);
		return true;
	}

	bool MenuParse_ownerDraw(menuDef_t* menu)
	{
		return PC_Int_Parse(&menu->window.ownerDraw);
	}

	bool MenuParse_ownerDrawFlags(menuDef_t* menu)
	{
		return PC_Int_Parse(&menu->window.ownerDrawFlags);
	}

	bool MenuParse_outOfBounds(menuDef_t* menu)
	{
		Window_SetStaticFlags(&menu->window, WINDOWSTATIC_OUTOFBOUNDSCLICK | menu->window.staticFlags);
		return true;
	}

	bool MenuParse_soundLoop(menuDef_t* menu)
	{
		return PC_String_Parse(&menu->data->soundName);
	}

	bool MenuParse_execExp(menuDef_t* menu)
	{
		const char* expressionType;
		pc_token_s token;

		if (!PC_String_Parse(&expressionType))
			return false;

		if (!_stricmp(expressionType, "visible"))
		{
			if (!PC_ReadTokenHandle(&token))
			{
				PC_SourceError("ERROR: line ended early after \"visible\"");
				return false;
			}
			if (_stricmp(token.string, "when"))
			{
				PC_SourceError("ERROR: Expected 'when' after \"visible\" but found \"%s\"", token.string);
				return false;
			}
			Window_AddDynamicFlags(&menu->window, WINDOWDYNAMIC_VISIBLE);
			if (Expression_Read(&menu->data->visibleExp))
				return true;
		}
		else if (!_stricmp(expressionType, "rect"))
		{
			if (!PC_ReadTokenHandle(&token))
			{
				PC_SourceError("ERROR: line ended early after \"exp rect\"");
				return false;
			}
			if (!_stricmp(token.string, "X"))
			{
				return (Expression_Read(&menu->data->rectXExp));
			}
			else if (!_stricmp(token.string, "Y"))
			{
				return (Expression_Read(&menu->data->rectYExp));
			}
			else if (!_stricmp(token.string, "W"))
			{
				return (Expression_Read(&menu->data->rectWExp));
			}
			else if (!_stricmp(token.string, "H"))
			{
				return (Expression_Read(&menu->data->rectHExp));
			}
			PC_SourceError("ERROR: Expected 'X', 'Y', 'W' or 'H' after \"exp rect\" but found \"%s\"", token.string);
			return false;
		}
		else if (!_stricmp(expressionType, "openSound"))
		{
			if (Expression_Read(&menu->data->openSoundExp))
				return true;
		}
		else if (!_stricmp(expressionType, "closeSound"))
		{
			if (Expression_Read(&menu->data->closeSoundExp))
				return true;
		}
		else if (!_stricmp(expressionType, "soundLoop"))
		{
			if (Expression_Read(&menu->data->soundLoopExp))
				return true;
		}
		else
		{
			PC_SourceError("ERROR: Unknown exp type '%s'", token.string);
		}
		return false;
	}

	bool MenuParse_popup(menuDef_t* menu)
	{
		Window_SetStaticFlags(&menu->window, WINDOWSTATIC_POPUP | menu->window.staticFlags);
		return true;
	}

	bool MenuParse_fadeClamp(menuDef_t* menu)
	{
		return PC_Float_Parse(&menu->data->fadeClamp);
	}

	bool MenuParse_fadeCycle(menuDef_t* menu)
	{
		return PC_Int_Parse(&menu->data->fadeCycle);
	}

	bool MenuParse_fadeAmount(menuDef_t* menu)
	{
		return PC_Float_Parse(&menu->data->fadeAmount);
	}

	bool MenuParse_fadeInAmount(menuDef_t* menu)
	{
		return PC_Float_Parse(&menu->data->fadeInAmount);
	}

	bool MenuParse_execKey(menuDef_t* menu)
	{
		char key;
		MenuEventHandlerSet* action;
		ItemKeyHandler* handler;

		if (!PC_Char_Parse(&key))
			return false;
		if (!PC_EventScript_Parse(&action))
			return false;
		handler = zmem->allocate<ItemKeyHandler>();
		handler->key = key;
		handler->next = menu->data->onKey;
		handler->action = action;
		menu->data->onKey = handler;
		return true;
	}

	bool MenuParse_execKeyInt(menuDef_t* menu)
	{
		int key;
		MenuEventHandlerSet* action;
		ItemKeyHandler* handler;

		if (!PC_Int_Parse(&key))
			return false;
		if (!PC_EventScript_Parse(&action))
			return false;
		handler = zmem->allocate<ItemKeyHandler>();
		handler->key = key;
		handler->next = menu->data->onKey;
		handler->action = action;
		menu->data->onKey = handler;
		return true;
	}

	bool MenuParse_blurWorld(menuDef_t* menu)
	{
		if (!PC_Float_Parse(&menu->data->blurRadius))
			return false;
		if (menu->data->blurRadius >= 0.0)
			return true;
		PC_SourceError("blur must be >= 0; %g is invalid", menu->data->blurRadius);
		return false;
	}

	bool MenuParse_legacySplitScreenScale(menuDef_t* menu)
	{
		return SetItemStaticFlag(menu, WINDOWSTATIC_LEGACYSPLITSCREENSCALE);
	}

	bool MenuParse_hiddenDuringScope(menuDef_t* menu)
	{
		return SetItemStaticFlag(menu, WINDOWSTATIC_HIDDENDURINGSCOPE);
	}

	bool MenuParse_hiddenDuringFlashbang(menuDef_t* menu)
	{
		return SetItemStaticFlag(menu, WINDOWSTATIC_HIDDENDURINGFLASH);
	}

	bool MenuParse_hiddenDuringUI(menuDef_t* menu)
	{
		return SetItemStaticFlag(menu, WINDOWSTATIC_HIDDENDURINGUI);
	}

	bool MenuParse_allowedBinding(menuDef_t* menu)
	{
		if (menu->data->allowedBinding)
			PC_SourceError("Only one 'allowedBinding' is supported");
		return PC_String_Parse(&menu->data->allowedBinding);
	}

	bool MenuParse_textOnlyFocus(menuDef_t* menu)
	{
		return SetItemStaticFlag(menu, WINDOWSTATIC_TEXTONLYFOCUS);
	}

	void Window_Init(windowDef_t* window)
	{

	}

	void Item_Init(itemDef_t* item)
	{
		Window_Init(&item->window);
	}

	void Item_InitControls(itemDef_t* item)
	{
		listBoxDef_s* listPtr;

		if (item && item->type == ITEM_TYPE_LISTBOX)
		{
			item->cursorPos[0] = 0;
			listPtr = Item_GetListBoxDef(item);
			if (listPtr)
			{
				listPtr->startPos[0] = 0;
				listPtr->endPos[0] = 0;
			}
		}
	}

	bool ItemParse_IsValidTextAlignment(int textAlignMode)
	{
		bool result;

		if (textAlignMode >= 0 && textAlignMode <= 15)
			result = (textAlignMode & 3) != 3;
		else
			result = false;
		return result;
	}

	bool ItemParse_name(itemDef_t* item)
	{
		return PC_String_Parse(&item->window.name);
	}

	bool ItemParse_text(itemDef_t* item)
	{
		return PC_String_Parse(&item->text);
	}

	char* UI_FileText(const char* fileName)
	{
		char* result;
		static char buffer[0x1000];
		std::size_t len;
		FILE* fp;

		memset(buffer, 0, sizeof(buffer));

		auto file = filesystem::file(fileName);
		file.open("rb");
		fp = file.get_fp();
		len = file.size();
		if (!fp)
			return 0;
		if (len < sizeof(buffer))
		{
			memcpy(buffer, file.read_bytes(len).data(), len);
			buffer[len] = 0;
			file.close();
			result = buffer;
		}
		else
		{
			file.close();
			ZONETOOL_ERROR("Menu file %s is larger than the %zi byte buffer used to parse menu files", fileName, sizeof(buffer));
			result = 0;
		}
		return result;
	}

	bool ItemParse_textfile(itemDef_t* item)
	{
		const char* text;
		pc_token_s token;

		if (!PC_ReadTokenHandle(&token))
			return false;
		text = UI_FileText(token.string);
		if (!text)
			return false;
		item->text = zmem->duplicate_string(text);
		return true;
	}

	bool ItemParse_textsavegame(itemDef_t* item)
	{
		item->text = "savegameinfo";
		item->itemFlags |= 1u;
		return true;
	}

	bool ItemParse_textcinematicsubtitle(itemDef_t* item)
	{
		item->text = "cinematicsubtitle";
		item->itemFlags |= 2u;
		return true;
	}

	bool ItemParse_group(itemDef_t* item)
	{
		return PC_String_Parse(&item->window.group);
	}

	bool ItemParse_rect(itemDef_t* item)
	{
		return PC_Rect_Parse(&item->window.rectClient);
	}

	bool ItemParse_rect720(itemDef_t* item)
	{
		// TODO:
		return false;
	}

	bool ItemParse_pos(itemDef_t* item)
	{
		// TODO:
		return false;
	}

	bool ItemParse_pos720(itemDef_t* item)
	{
		// TODO:
		return false;
	}

	bool ItemParse_origin(itemDef_t* item)
	{
		float x;
		float y;

		if (!PC_Float_Parse(&x) || !PC_Float_Parse(&y))
			return false;
		item->window.rectClient.x += x;
		item->window.rectClient.y += y;
		return true;
	}

	bool ItemParse_style(itemDef_t* item)
	{
		return PC_Int_Parse(&item->window.style);
	}

	bool ItemParse_decoration(itemDef_t* item)
	{
		Window_SetStaticFlags(&item->window, item->window.staticFlags | WINDOWSTATIC_DECORATION);
		return true;
	}

	bool ItemParse_notselectable(itemDef_t* item)
	{
		listBoxDef_s* listPtr;

		Item_ValidateTypeData(item);
		listPtr = Item_GetListBoxDef(item);
		if (!listPtr)
			return false;
		if (item->type == ITEM_TYPE_LISTBOX)
			listPtr->notselectable = 1;
		return true;
	}

	bool ItemParse_usePaging(itemDef_t* item)
	{
		listBoxDef_s* listPtr;

		Item_ValidateTypeData(item);
		listPtr = Item_GetListBoxDef(item);
		if (!listPtr)
			return false;
		if (item->type == ITEM_TYPE_LISTBOX)
			listPtr->usePaging = 1;
		return true;
	}

	bool ItemParse_autowrapped(itemDef_t* item)
	{
		Window_SetStaticFlags(&item->window, item->window.staticFlags | WINDOWSTATIC_AUTOWRAPPED);
		return true;
	}

	bool ItemParse_horizontalscroll(itemDef_t* item)
	{
		Window_SetStaticFlags(&item->window, item->window.staticFlags | WINDOWSTATIC_HORIZONTALSCROLL);
		return true;
	}

	bool ItemParse_type(itemDef_t* item)
	{
		if (!PC_Int_Parse(&item->type))
			return false;
		Item_ValidateTypeData(item);
		return true;
	}

	bool ItemParse_elementwidth(itemDef_t* item)
	{
		listBoxDef_s* listPtr;

		Item_ValidateTypeData(item);
		listPtr = Item_GetListBoxDef(item);
		if (listPtr)
			return PC_Float_Parse(&listPtr->elementWidth);
		return false;
	}

	bool ItemParse_elementheight(itemDef_t* item)
	{
		listBoxDef_s* listPtr;

		Item_ValidateTypeData(item);
		listPtr = Item_GetListBoxDef(item);
		if (listPtr)
			return PC_Float_Parse(&listPtr->elementHeight);
		return false;
	}

	bool ItemParse_special(itemDef_t* item)
	{
		return PC_Float_Parse(&item->special);
	}

	bool ItemParse_elementtype(itemDef_t* item)
	{
		listBoxDef_s* listPtr;

		Item_ValidateTypeData(item);
		if (item->typeData.data && (listPtr = Item_GetListBoxDef(item)) != 0)
			return PC_Int_Parse(&listPtr->elementStyle);
		return false;
	}

	bool ItemParse_columns(itemDef_t* item)
	{
		int pos;
		int width;
		int align;
		int maxChars;
		listBoxDef_s* listPtr;
		int num;
		int i;

		Item_ValidateTypeData(item);
		if (!item->typeData.listBox)
			return false;
		listPtr = Item_GetListBoxDef(item);
		if (!listPtr)
			return false;
		if (!PC_Int_Parse(&num))
			return false;
		if (num > 16)
			num = 16;
		listPtr->numColumns = num;
		for (i = 0; i < num; ++i)
		{
			if (!PC_Int_Parse(&pos) || !PC_Int_Parse(&width) || !PC_Int_Parse(&maxChars))
				return false;
			listPtr->columnInfo[i].xpos = pos;
			listPtr->columnInfo[i].width = width;
			listPtr->columnInfo[i].height = (int)listPtr->elementHeight;
			listPtr->columnInfo[i].maxChars = maxChars;
			if (PC_Int_ParseLine(&align))
				listPtr->columnInfo[i].alignment = align;
			else
				listPtr->columnInfo[i].alignment = 0;
		}
		return true;
	}

	bool ItemParse_border(itemDef_t* item)
	{
		return PC_Int_Parse(&item->window.border);
	}

	bool ItemParse_bordersize(itemDef_t* item)
	{
		return PC_Float_Parse(&item->window.borderSize);
	}

	bool ItemParse_visible(itemDef_t* item)
	{
		pc_token_s token;

		if (!PC_ReadTokenHandle(&token))
			return false;
		if (!_stricmp(token.string, "when") || !_stricmp(token.string, "if"))
		{
			Window_AddDynamicFlags(&item->window, WINDOWDYNAMIC_VISIBLE);
			return Expression_Read(&item->visibleExp);
		}
		if (atoi(token.string))
		{
			Window_AddDynamicFlags(&item->window, WINDOWDYNAMIC_VISIBLE);
		}
		return true;
	}

	bool ItemParse_disabled(itemDef_t* item)
	{
		pc_token_s token;

		if (!PC_ReadTokenHandle(&token))
			return false;
		if (!_stricmp(token.string, "when") || !_stricmp(token.string, "if"))
		{
			if (Expression_Read(&item->disabledExp))
				return true;
		}
		return false;
	}

	bool ItemParse_ownerdraw(itemDef_t* item)
	{
		bool result;

		result = PC_Int_Parse(&item->window.ownerDraw);
		if (result)
		{
			item->type = 8;
			result = true;
		}
		return result;
	}

	bool ItemParse_align(itemDef_t* item)
	{
		return PC_Int_Parse(&item->alignment);
	}

	bool ItemParse_textalign(itemDef_t* item)
	{
		if (!PC_Int_Parse(&item->textAlignMode))
			return false;
		if (ItemParse_IsValidTextAlignment(item->textAlignMode))
			return true;
		PC_SourceError("expected ITEM_ALIGN_* value");
		return false;
	}

	bool ItemParse_textalignx(itemDef_t* item)
	{
		return PC_Float_Parse(&item->textalignx);
	}

	bool ItemParse_textaligny(itemDef_t* item)
	{
		return PC_Float_Parse(&item->textaligny);
	}

	bool ItemParse_textscale(itemDef_t* item)
	{
		return PC_Float_Parse(&item->textscale);
	}

	bool ItemParse_textstyle(itemDef_t* item)
	{
		return PC_Int_Parse(&item->textStyle);
	}

	bool ItemParse_textfont(itemDef_t* item)
	{
		return PC_Int_Parse(&item->fontEnum);
	}

	bool ItemParse_backcolor(itemDef_t* item)
	{
		return PC_Color_Parse(&item->window.backColor);
	}

	bool ItemParse_forecolor(itemDef_t* item)
	{
		if (!PC_Color_Parse(&item->window.foreColor))
			return false;
		Window_AddDynamicFlags(&item->window, WINDOWDYNAMIC_FORECOLOR);
		return true;
	}

	bool ItemParse_bordercolor(itemDef_t* item)
	{
		return PC_Color_Parse(&item->window.borderColor);
	}

	bool ItemParse_outlinecolor(itemDef_t* item)
	{
		return PC_Color_Parse(&item->window.outlineColor);
	}

	bool ItemParse_background(itemDef_t* item)
	{
		const char* name;

		if (!PC_String_Parse(&name))
			return false;
		item->window.background = zmem->manual_allocate<Material>(sizeof(char*));
		item->window.background->name = name;
		_strlwr((char*)item->window.background->name);
		return true;
	}

	bool ItemParse_onFocus(itemDef_t* item)
	{
		return PC_EventScript_Parse(&item->onFocus);
	}

	bool ItemParse_leaveFocus(itemDef_t* item)
	{
		return PC_EventScript_Parse(&item->leaveFocus);
	}

	bool ItemParse_mouseEnter(itemDef_t* item)
	{
		return PC_EventScript_Parse(&item->mouseEnter);
	}

	bool ItemParse_mouseExit(itemDef_t* item)
	{
		return PC_EventScript_Parse(&item->mouseExit);
	}

	bool ItemParse_mouseEnterText(itemDef_t* item)
	{
		return PC_EventScript_Parse(&item->mouseEnterText);
	}

	bool ItemParse_mouseExitText(itemDef_t* item)
	{
		return PC_EventScript_Parse(&item->mouseExitText);
	}

	bool ItemParse_action(itemDef_t* item)
	{
		return PC_EventScript_Parse(&item->action);
	}

	bool ItemParse_accept(itemDef_t* item)
	{
		return PC_EventScript_Parse(&item->accept);
	}

	bool ItemParse_dvar(itemDef_t* item)
	{
		editFieldDef_s* editPtr;

		Item_ValidateTypeData(item);
		if (!PC_String_Parse(&item->dvar))
			return false;

		if (item->typeData.editField && Item_IsEditFieldDef(item))
		{
			editPtr = Item_GetEditFieldDef(item);

			editPtr->minVal = -1.0f;
			editPtr->maxVal = -1.0f;
			editPtr->stepVal = -1.0f;
		}

		return true;
	}

	bool ItemParse_localvar(itemDef_t* item)
	{
		editFieldDef_s* editPtr;

		Item_ValidateTypeData(item);
		if (!PC_String_Parse(&item->localVar))
			return false;

		if (item->typeData.editField && Item_IsEditFieldDef(item))
		{
			editPtr = Item_GetEditFieldDef(item);

			editPtr->minVal = -1.0f;
			editPtr->maxVal = -1.0f;
			editPtr->stepVal = -1.0f;
		}

		return true;
	}

	bool ItemParse_maxChars(itemDef_t* item)
	{
		editFieldDef_s* editPtr;
		int maxChars;

		Item_ValidateTypeData(item);
		if (!item->typeData.editField)
			return false;
		if (!PC_Int_Parse(&maxChars))
			return false;
		editPtr = Item_GetEditFieldDef(item);
		if (!editPtr)
			return false;
		editPtr->maxChars = maxChars;
		return true;
	}

	bool ItemParse_maxCharsGotoNext(itemDef_t* item)
	{
		editFieldDef_s* editPtr;

		Item_ValidateTypeData(item);
		if (!item->typeData.editField)
			return false;
		editPtr = Item_GetEditFieldDef(item);
		if (!editPtr)
			return false;
		editPtr->maxCharsGotoNext = 1;
		return true;
	}

	bool ItemParse_maxPaintChars(itemDef_t* item)
	{
		editFieldDef_s* editPtr;
		int maxChars;

		Item_ValidateTypeData(item);
		if (!item->typeData.editField)
			return false;
		if (!PC_Int_Parse(&maxChars))
			return false;
		editPtr = Item_GetEditFieldDef(item);
		if (!editPtr)
			return false;
		editPtr->maxPaintChars = maxChars;
		return true;
	}

	bool ItemParse_focusSound(itemDef_t* item)
	{
		const char* name;

		if (!PC_String_Parse(&name))
			return false;
		item->focusSound = zmem->manual_allocate<snd_alias_list_t>(sizeof(char*));
		item->focusSound->name = name;
		return true;
	}

	bool ItemParse_dvarFloat(itemDef_t* item)
	{
		editFieldDef_s* editPtr;

		Item_ValidateTypeData(item);
		if (!item->typeData.editField)
			return false;
		editPtr = Item_GetEditFieldDef(item);
		if (!editPtr)
			return false;
		return PC_String_Parse(&item->dvar)
			&& PC_Float_Parse(&editPtr->stepVal)
			&& PC_Float_Parse(&editPtr->minVal)
			&& PC_Float_Parse(&editPtr->maxVal);
	}

	bool ItemParse_dvarStrList(itemDef_t* item)
	{
		int pass;
		multiDef_s* multiPtr;
		pc_token_s token;

		Item_ValidateTypeData(item);
		if (!item->typeData.multi)
			return false;
		if (item->type != ITEM_TYPE_MULTI)
			return false;
		multiPtr = Item_GetMultiDef(item);
		multiPtr->count = 0;
		multiPtr->strDef = 1;
		if (!PC_ReadTokenHandle(&token))
			return false;
		if (token.string[0] != '{')
			return false;
		pass = 0;
		do
		{
			while (1)
			{
				do
				{
					if (!PC_ReadTokenHandle(&token))
					{
						PC_SourceError("end of file inside menu item");
						return false;
					}
					if (token.string[0] == '}')
						return true;
				} while (token.string[0] == ',' || token.string[0] == ';');
				if (pass)
					break;
				multiPtr->dvarList[multiPtr->count] = zmem->duplicate_string(token.string);
				pass = 1;
			}
			multiPtr->dvarStr[multiPtr->count] = zmem->duplicate_string(token.string);
			pass = 0;
			++multiPtr->count;
		} while (multiPtr->count < 32);
		return false;
	}

	bool ItemParse_dvarFloatList(itemDef_t* item)
	{
		multiDef_s* multiPtr;
		pc_token_s token;

		Item_ValidateTypeData(item);
		if (!item->typeData.multi)
			return false;
		if (item->type != ITEM_TYPE_MULTI)
			return false;
		multiPtr = Item_GetMultiDef(item);
		multiPtr->count = 0;
		multiPtr->strDef = 0;
		if (!PC_ReadTokenHandle(&token))
			return false;
		if (token.string[0] != '{')
			return false;
		do
		{
			do
			{
				if (!PC_ReadTokenHandle(&token))
				{
					PC_SourceError("end of file inside menu item");
					return false;
				}
				if (token.string[0] == '}')
					return true;
			} while (token.string[0] == ',' || token.string[0] == ';');
			multiPtr->dvarList[multiPtr->count] = zmem->duplicate_string(token.string);
			if (!PC_Float_Parse(&multiPtr->dvarValue[multiPtr->count]))
				return false;
			++multiPtr->count;
		} while (multiPtr->count < 32);
		return false;
	}

	bool ItemParse_dvarEnumList(itemDef_t* item)
	{
		const char* enumDvarPtr;

		Item_ValidateTypeData(item);
		enumDvarPtr = Item_GetEnumDvarName(item);
		if (item->type != ITEM_TYPE_DVARENUM)
			return false;
		if (!enumDvarPtr)
			return PC_String_Parse(&item->typeData.enumDvarName);
		PC_SourceError("enumDvarList already given");
		return false;
	}

	bool ItemParse_ownerdrawFlag(itemDef_t* item)
	{
		int flags;

		if (!PC_Int_Parse(&flags))
			return false;
		item->window.ownerDrawFlags |= flags;
		return true;
	}

	bool ItemParse_enableDvar(itemDef_t* item)
	{
		if (!PC_Script_Parse(&item->enableDvar))
			return false;
		item->dvarFlags |= 1u;
		return true;
	}

	bool ItemParse_dvarTest(itemDef_t* item)
	{
		return PC_String_Parse(&item->dvarTest);
	}

	bool ItemParse_disableDvar(itemDef_t* item)
	{
		if (!PC_Script_Parse(&item->enableDvar))
			return false;
		item->dvarFlags |= 2u;
		return true;
	}

	bool ItemParse_showDvar(itemDef_t* item)
	{
		if (!PC_Script_Parse(&item->enableDvar))
			return false;
		item->dvarFlags |= 4u;
		return true;
	}

	bool ItemParse_hideDvar(itemDef_t* item)
	{
		if (!PC_Script_Parse(&item->enableDvar))
			return false;
		item->dvarFlags |= 8u;
		return true;
	}

	bool ItemParse_focusDvar(itemDef_t* item)
	{
		if (!PC_Script_Parse(&item->enableDvar))
			return false;
		item->dvarFlags |= 0x10u;
		return true;
	}

	bool ItemParse_doubleClick(itemDef_t* item)
	{
		listBoxDef_s* listPtr;

		Item_ValidateTypeData(item);
		if (!item->typeData.listBox)
			return false;
		listPtr = Item_GetListBoxDef(item);
		if (!listPtr)
			return false;
		return PC_EventScript_Parse(&listPtr->onDoubleClick);
	}

	bool ItemParse_execKey(itemDef_t* item)
	{
		char key;
		MenuEventHandlerSet* action;
		ItemKeyHandler* handler;

		if (!PC_Char_Parse(&key))
			return false;
		if (!PC_EventScript_Parse(&action))
			return false;
		handler = zmem->allocate<ItemKeyHandler>();
		handler->key = key;
		handler->next = item->onKey;
		handler->action = action;
		item->onKey = handler;
		return true;
	}

	bool ItemParse_execKeyInt(itemDef_t* item)
	{
		int key;
		MenuEventHandlerSet* action;
		ItemKeyHandler* handler;

		if (!PC_Int_Parse(&key))
			return false;
		if (!PC_EventScript_Parse(&action))
			return false;
		handler = zmem->allocate<ItemKeyHandler>();
		handler->key = key;
		handler->next = item->onKey;
		handler->action = action;
		item->onKey = handler;
		return true;
	}

	bool ItemParse_execExp(itemDef_t* item)
	{
		const char* expressionType;
		const char* expressionComponent;
		ItemFloatExpressionTarget target;

		target = ITEM_FLOATEXP_TGT__COUNT;

		if (!PC_String_Parse(&expressionType))
			return false;

		if (!_stricmp(expressionType, "visible"))
		{
			if (item->visibleExp)
			{
				PC_SourceError("redefinition of expression for '%s'.", expressionType);
				return false;
			}
			else
			{
				Window_AddDynamicFlags(&item->window, WINDOWDYNAMIC_VISIBLE);
				return Expression_Read(&item->visibleExp);
			}
		}
		else if (!_stricmp(expressionType, "text"))
		{
			if (item->textExp)
			{
				PC_SourceError("redefinition of expression for '%s'.", expressionType);
				return false;
			}
			else
			{
				return Expression_Read(&item->textExp);
			}
		}
		else if (!_stricmp(expressionType, "material"))
		{
			if (item->materialExp)
			{
				PC_SourceError("redefinition of expression for '%s'.", expressionType);
				return false;
			}
			else
			{
				return Expression_Read(&item->materialExp);
			}
		}
		else if (!_stricmp(expressionType, "disabled"))
		{
			if (item->disabledExp)
			{
				PC_SourceError("redefinition of expression for '%s'.", expressionType);
				return false;
			}
			else
			{
				return Expression_Read(&item->disabledExp);
			}
		}
		if (!PC_String_Parse(&expressionComponent))
		{
			PC_SourceError("component required (exp type '%s' is not a known single-component expression slot)",
				expressionType);
			return false;
		}
		for (int i = 0; i < ITEM_FLOATEXP_TGT__COUNT; i++)
		{
			if (!_stricmp(expressionType, g_itemFloatExpressions[i].s1) && !_stricmp(expressionComponent, g_itemFloatExpressions[i].s2))
			{
				target = ItemFloatExpressionTarget(i);
			}
		}
		if (target < ITEM_FLOATEXP_TGT__COUNT)
		{
			// allocate that i guess
			if (!item->floatExpressions)
			{
				item->floatExpressions = zmem->allocate<ItemFloatExpression>(ITEM_FLOATEXP_TGT__COUNT);
			}
			for (int i = 0; i < item->floatExpressionCount; i++)
			{
				if (item->floatExpressions[i].target == target)
				{
					if (item->floatExpressions[i].expression)
					{
						PC_SourceError("redefinition of expression for '%s', component '%s.", expressionType, expressionComponent);
						return false;
					}
					else
					{
						if (!Expression_Read(&item->floatExpressions[i].expression))
							return false;
						if (target == ITEM_FLOATEXP_TGT_FORECOLOR_R ||
							target == ITEM_FLOATEXP_TGT_FORECOLOR_G ||
							target == ITEM_FLOATEXP_TGT_FORECOLOR_B ||
							target == ITEM_FLOATEXP_TGT_FORECOLOR_A)
						{
							Window_AddDynamicFlags(&item->window, WINDOWDYNAMIC_FORECOLOR);
						}
						return true;
					}
				}
			}
			if (!Expression_Read(&item->floatExpressions[item->floatExpressionCount].expression))
				return false;
			if (target == ITEM_FLOATEXP_TGT_FORECOLOR_R ||
				target == ITEM_FLOATEXP_TGT_FORECOLOR_G ||
				target == ITEM_FLOATEXP_TGT_FORECOLOR_B ||
				target == ITEM_FLOATEXP_TGT_FORECOLOR_A)
			{
				Window_AddDynamicFlags(&item->window, WINDOWDYNAMIC_FORECOLOR);
			}
			item->floatExpressions[item->floatExpressionCount].target = target;
			item->floatExpressionCount++;
			return true;
		}
		PC_SourceError("unknown exp type and component '%s' '%s'", expressionType, expressionComponent);
		return false;
	}

	bool ItemParse_gameMsgWindowIndex(itemDef_t* item)
	{
		return PC_Int_Parse(&item->gameMsgWindowIndex);
	}

	bool ItemParse_gameMsgWindowMode(itemDef_t* item)
	{
		return PC_Int_Parse(&item->gameMsgWindowMode);
	}

	bool ItemParse_selectBorder(itemDef_t* item)
	{
		listBoxDef_s* listPtr;

		Item_ValidateTypeData(item);
		listPtr = Item_GetListBoxDef(item);
		if (listPtr)
			return PC_Color_Parse(&listPtr->selectBorder);
		return false;
	}

	bool ItemParse_disableColor(itemDef_t* item)
	{
		return PC_Color_Parse(&item->window.disableColor);
	}

	bool ItemParse_selectIcon(itemDef_t* item)
	{
		listBoxDef_s* listPtr;
		const char* name;

		Item_ValidateTypeData(item);
		listPtr = Item_GetListBoxDef(item);
		if (!listPtr)
			return false;
		if (!PC_String_Parse(&name))
			return false;

		listPtr->selectIcon = zmem->manual_allocate<Material>(sizeof(char*));
		listPtr->selectIcon->name = name;
		_strlwr((char*)listPtr->selectIcon->name);
		return true;
	}

	bool ItemParse_spacing(itemDef_t* item)
	{
		newsTickerDef_s* tickerPtr;
		int spacing;

		Item_ValidateTypeData(item);
		tickerPtr = Item_GetNewsTickerDef(item);
		if (!tickerPtr)
			return false;
		if (!PC_Int_Parse(&spacing) || spacing < 0)
			return false;
		tickerPtr->spacing = spacing;
		return true;
	}

	bool ItemParse_speed(itemDef_t* item)
	{
		newsTickerDef_s* tickerPtr;
		int speed;

		Item_ValidateTypeData(item);
		tickerPtr = Item_GetNewsTickerDef(item);
		if (!tickerPtr)
			return false;
		if (!PC_Int_Parse(&speed) || speed < 0)
			return false;
		tickerPtr->speed = speed;
		return true;
	}

	bool ItemParse_newsfeed(itemDef_t* item)
	{
		newsTickerDef_s* tickerPtr;
		int feedId;

		Item_ValidateTypeData(item);
		tickerPtr = Item_GetNewsTickerDef(item);
		if (!tickerPtr)
			return false;
		if (!PC_Int_Parse(&feedId))
			return false;
		tickerPtr->feedId = feedId;
		return true;
	}

	bool ItemParse_glowColor(itemDef_t* item)
	{
		Item_ValidateTypeData(item);
		return PC_Color_Parse(&item->glowColor);
	}

	bool ItemParse_decodeEffect(itemDef_t* item)
	{
		Item_ValidateTypeData(item);
		if (!PC_Int_Parse(&item->fxLetterTime)
			|| !PC_Int_Parse(&item->fxDecayStartTime)
			|| !PC_Int_Parse(&item->fxDecayDuration))
		{
			return false;
		}
		item->decayActive = 1;
		return true;
	}

	parse_itemdef_func* find_itemdef_func(const char* keyword)
	{
		if (p_id_funcs.empty())
		{
			p_id_funcs.push_back({ "name", ItemParse_name });
			p_id_funcs.push_back({ "text", ItemParse_text });
			p_id_funcs.push_back({ "textfile", ItemParse_textfile });
			p_id_funcs.push_back({ "textsavegame", ItemParse_textsavegame });
			p_id_funcs.push_back({ "textcinematicsubtitle", ItemParse_textcinematicsubtitle });
			p_id_funcs.push_back({ "group", ItemParse_group });
			p_id_funcs.push_back({ "rect", ItemParse_rect });
			p_id_funcs.push_back({ "rect480", ItemParse_rect });
			//p_id_funcs.push_back({ "rect720", ItemParse_rect720 });
			//p_id_funcs.push_back({ "pos480", ItemParse_pos });
			//p_id_funcs.push_back({ "pos720", ItemParse_pos720 });
			p_id_funcs.push_back({ "origin", ItemParse_origin });
			p_id_funcs.push_back({ "style", ItemParse_style });
			p_id_funcs.push_back({ "decoration", ItemParse_decoration });
			p_id_funcs.push_back({ "notselectable", ItemParse_notselectable });
			p_id_funcs.push_back({ "usepaging", ItemParse_usePaging });
			p_id_funcs.push_back({ "autowrapped", ItemParse_autowrapped });
			p_id_funcs.push_back({ "horizontalscroll", ItemParse_horizontalscroll });
			p_id_funcs.push_back({ "type", ItemParse_type });
			p_id_funcs.push_back({ "elementwidth", ItemParse_elementwidth });
			p_id_funcs.push_back({ "elementheight", ItemParse_elementheight });
			p_id_funcs.push_back({ "feeder", ItemParse_special });
			p_id_funcs.push_back({ "elementtype", ItemParse_elementtype });
			p_id_funcs.push_back({ "columns", ItemParse_columns });
			p_id_funcs.push_back({ "border", ItemParse_border });
			p_id_funcs.push_back({ "bordersize", ItemParse_bordersize });
			p_id_funcs.push_back({ "visible", ItemParse_visible });
			p_id_funcs.push_back({ "disabled", ItemParse_disabled });
			p_id_funcs.push_back({ "ownerdraw", ItemParse_ownerdraw });
			p_id_funcs.push_back({ "align", ItemParse_align });
			p_id_funcs.push_back({ "textalign", ItemParse_textalign });
			p_id_funcs.push_back({ "textalignx", ItemParse_textalignx });
			p_id_funcs.push_back({ "textaligny", ItemParse_textaligny });
			p_id_funcs.push_back({ "textscale", ItemParse_textscale });
			p_id_funcs.push_back({ "textstyle", ItemParse_textstyle });
			p_id_funcs.push_back({ "textfont", ItemParse_textfont });
			p_id_funcs.push_back({ "backcolor", ItemParse_backcolor });
			p_id_funcs.push_back({ "forecolor", ItemParse_forecolor });
			p_id_funcs.push_back({ "bordercolor", ItemParse_bordercolor });
			p_id_funcs.push_back({ "outlinecolor", ItemParse_outlinecolor });
			p_id_funcs.push_back({ "background", ItemParse_background });
			p_id_funcs.push_back({ "onFocus", ItemParse_onFocus });
			p_id_funcs.push_back({ "leaveFocus", ItemParse_leaveFocus });
			p_id_funcs.push_back({ "mouseEnter", ItemParse_mouseEnter });
			p_id_funcs.push_back({ "mouseExit", ItemParse_mouseExit });
			p_id_funcs.push_back({ "mouseEnterText", ItemParse_mouseEnterText });
			p_id_funcs.push_back({ "mouseExitText", ItemParse_mouseExitText });
			p_id_funcs.push_back({ "action", ItemParse_action });
			p_id_funcs.push_back({ "accept", ItemParse_accept });
			p_id_funcs.push_back({ "special", ItemParse_special });
			p_id_funcs.push_back({ "dvar", ItemParse_dvar });
			p_id_funcs.push_back({ "localvar", ItemParse_localvar });
			p_id_funcs.push_back({ "maxChars", ItemParse_maxChars });
			p_id_funcs.push_back({ "maxCharsGotoNext", ItemParse_maxCharsGotoNext });
			p_id_funcs.push_back({ "maxPaintChars", ItemParse_maxPaintChars });
			p_id_funcs.push_back({ "focusSound", ItemParse_focusSound });
			p_id_funcs.push_back({ "dvarFloat", ItemParse_dvarFloat });
			p_id_funcs.push_back({ "dvarStrList", ItemParse_dvarStrList });
			p_id_funcs.push_back({ "dvarFloatList", ItemParse_dvarFloatList });
			p_id_funcs.push_back({ "dvarEnumList", ItemParse_dvarEnumList });
			p_id_funcs.push_back({ "ownerdrawFlag", ItemParse_ownerdrawFlag });
			p_id_funcs.push_back({ "enableDvar", ItemParse_enableDvar });
			p_id_funcs.push_back({ "dvarTest", ItemParse_dvarTest });
			p_id_funcs.push_back({ "disableDvar", ItemParse_disableDvar });
			p_id_funcs.push_back({ "showDvar", ItemParse_showDvar });
			p_id_funcs.push_back({ "hideDvar", ItemParse_hideDvar });
			p_id_funcs.push_back({ "focusDvar", ItemParse_focusDvar });
			p_id_funcs.push_back({ "doubleclick", ItemParse_doubleClick });
			p_id_funcs.push_back({ "execKey", ItemParse_execKey });
			p_id_funcs.push_back({ "execKeyInt", ItemParse_execKeyInt });
			p_id_funcs.push_back({ "exp", ItemParse_execExp });
			p_id_funcs.push_back({ "gamemsgwindowindex", ItemParse_gameMsgWindowIndex });
			p_id_funcs.push_back({ "gamemsgwindowmode", ItemParse_gameMsgWindowMode });
			p_id_funcs.push_back({ "selectBorder", ItemParse_selectBorder });
			p_id_funcs.push_back({ "disablecolor", ItemParse_disableColor });
			p_id_funcs.push_back({ "selectIcon", ItemParse_selectIcon });
			p_id_funcs.push_back({ "spacing", ItemParse_spacing });
			p_id_funcs.push_back({ "speed", ItemParse_speed });
			p_id_funcs.push_back({ "newsfeed", ItemParse_newsfeed });
			p_id_funcs.push_back({ "glowColor", ItemParse_glowColor });
			p_id_funcs.push_back({ "decodeEffect", ItemParse_decodeEffect });
		}

		for (auto i = 0; i < p_id_funcs.size(); i++)
		{
			if (!_stricmp(keyword, p_id_funcs.at(i).keyword))
			{
				return &p_id_funcs.at(i);
			}
		}
		return nullptr;
	}

	bool Item_Parse(itemDef_t* item)
	{
		parse_itemdef_func* key;
		pc_token_s token;

		if (!PC_ReadTokenHandle(&token))
			return false;
		if (token.string[0] != '{')
			return false;
		do
		{
			while (1)
			{
				do
				{
					if (!PC_ReadTokenHandle(&token))
					{
						PC_SourceError("end of file inside menu item");
						return false;
					}
					if (token.string[0] == '}')
						return true;
				} while (token.string[0] == ';');
				key = find_itemdef_func(token.string);
				if (key)
					break;
				PC_SourceError("unknown menu item keyword %s", token.string);
			}
		} while (key->func(item));
		PC_SourceError("couldn't parse menu item keyword %s", token.string);
		return false;
	}

	bool MenuParse_itemDef(menuDef_t* menu)
	{
		itemDef_t* item;

		if (menu->itemCount >= MAX_ITEMDEFS_PER_MENUDEF)
		{
			PC_SourceError("too many itemDefs for menu '%s', MAX is %i", menu->window.name, MAX_ITEMDEFS_PER_MENUDEF);
			return false;
		}
		else
		{
			item = zmem->allocate<itemDef_t>();
			Item_Init(item);
			if (!Item_Parse(item))
			{
				//Menu_FreeItemMemory(item);
				return false;
			}
			//Item_PostParse(item);
			Item_InitControls(item);
			item->parent = menu;
			menu->items[menu->itemCount++] = item;
		}
		return true;
	}

	parse_menudef_func* find_menudef_func(const char* keyword)
	{
		if (p_md_funcs.empty())
		{
			p_md_funcs.push_back({ "name", MenuParse_name });
			p_md_funcs.push_back({ "fullscreen", MenuParse_fullscreen });
			p_md_funcs.push_back({ "screenSpace", MenuParse_screenSpace });
			p_md_funcs.push_back({ "decoration", MenuParse_decoration });
			p_md_funcs.push_back({ "rect", MenuParse_rect });
			p_md_funcs.push_back({ "rect480", MenuParse_rect });
			//p_md_funcs.push_back({ "rect720", MenuParse_rect720 });
			//p_md_funcs.push_back({ "pos480", MenuParse_pos });
			//p_md_funcs.push_back({ "pos720", MenuParse_pos720 });
			p_md_funcs.push_back({ "group", MenuParse_group });
			p_md_funcs.push_back({ "style", MenuParse_style });
			p_md_funcs.push_back({ "visible", MenuParse_visible });
			p_md_funcs.push_back({ "onOpen", MenuParse_onOpen });
			p_md_funcs.push_back({ "onClose", MenuParse_onClose });
			p_md_funcs.push_back({ "onRequestClose", MenuParse_onCloseRequest });
			p_md_funcs.push_back({ "onESC", MenuParse_onESC });
			p_md_funcs.push_back({ "border", MenuParse_border });
			p_md_funcs.push_back({ "borderSize", MenuParse_borderSize });
			p_md_funcs.push_back({ "backcolor", MenuParse_backcolor });
			p_md_funcs.push_back({ "forecolor", MenuParse_forecolor });
			p_md_funcs.push_back({ "bordercolor", MenuParse_bordercolor });
			p_md_funcs.push_back({ "focuscolor", MenuParse_focuscolor });
			p_md_funcs.push_back({ "outlinecolor", MenuParse_outlinecolor });
			p_md_funcs.push_back({ "background", MenuParse_background });
			p_md_funcs.push_back({ "ownerDraw", MenuParse_ownerDraw });
			p_md_funcs.push_back({ "ownerDrawFlags", MenuParse_ownerDrawFlags });
			p_md_funcs.push_back({ "outOfBoundsClick", MenuParse_outOfBounds });
			p_md_funcs.push_back({ "soundLoop", MenuParse_soundLoop });
			p_md_funcs.push_back({ "itemDef", MenuParse_itemDef });
			p_md_funcs.push_back({ "exp", MenuParse_execExp });
			p_md_funcs.push_back({ "popup", MenuParse_popup });
			p_md_funcs.push_back({ "fadeClamp", MenuParse_fadeClamp });
			p_md_funcs.push_back({ "fadeCycle", MenuParse_fadeCycle });
			p_md_funcs.push_back({ "fadeAmount", MenuParse_fadeAmount });
			p_md_funcs.push_back({ "fadeInAmount", MenuParse_fadeInAmount });
			p_md_funcs.push_back({ "execKey", MenuParse_execKey });
			p_md_funcs.push_back({ "execKeyInt", MenuParse_execKeyInt });
			p_md_funcs.push_back({ "blurWorld", MenuParse_blurWorld });
			p_md_funcs.push_back({ "legacySplitScreenScale", MenuParse_legacySplitScreenScale });
			p_md_funcs.push_back({ "hiddenDuringScope", MenuParse_hiddenDuringScope });
			p_md_funcs.push_back({ "hiddenDuringFlashbang", MenuParse_hiddenDuringFlashbang });
			p_md_funcs.push_back({ "hiddenDuringUI", MenuParse_hiddenDuringUI });
			p_md_funcs.push_back({ "allowedBinding", MenuParse_allowedBinding });
			p_md_funcs.push_back({ "textOnlyFocus", MenuParse_textOnlyFocus });
		}

		for (auto i = 0; i < p_md_funcs.size(); i++)
		{
			if (!_stricmp(keyword, p_md_funcs.at(i).keyword))
			{
				return &p_md_funcs.at(i);
			}
		}
		return nullptr;
	}

	bool Menu_Parse(menuDef_t* menu)
	{
		parse_menudef_func* key;
		pc_token_s token;

		if (!PC_ReadTokenHandle(&token))
			return false;
		if (token.string[0] != '{')
			return false;
		do
		{
			while (1)
			{
				do
				{
					memset(&token, 0, sizeof(pc_token_s));
					if (!PC_ReadTokenHandle(&token))
					{
						PC_SourceError("end of file inside menu");
						return false;
					}
					if (token.string[0] == '}')
						return true;
				} while (token.string[0] == ';');
				key = find_menudef_func(token.string);
				if (key)
					break;
				PC_SourceError("unknown menu keyword %s", token.string);
				return false;
			}
		} while (key->func(menu));
		PC_SourceError("couldn't parse menu keyword %s", token.string);
		return false;
	}

	void Menu_Init(menuDef_t* menu)
	{
		menu->items = zmem->allocate<itemDef_t*>(MAX_ITEMDEFS_PER_MENUDEF);
		menu->data = zmem->allocate<menuData_t>();
		Window_Init(&menu->window);
	}

	bool Menu_New()
	{
		bool result;
		menuDef_t* menu;

		menu = zmem->allocate<menuDef_t>();

		Menu_Init(menu);
		if (Menu_Parse(menu))
		{
			if (menu->window.name)
			{
				//Menu_PostParse(menu);
				if (menuList->menuCount >= MAX_MENUDEFS_PER_MENULIST)
				{
					PC_SourceError("too many menuDefs in menuList %s, MAX is %i", menuList->name, MAX_MENUDEFS_PER_MENULIST);
					result = false;
					return result;
				}
				menuList->menus[menuList->menuCount++] = menu;
				result = true;
			}
			else
			{
				PC_SourceError("menu has no name");
				//Menu_FreeMemory(menu);
				result = false;
			}
		}
		else
		{
			//Menu_FreeMemory(menu);
			result = false;
		}
		return result;
	}

	bool UI_ParseMenuInternal(const char* menuFile)
	{
		bool result;
		int handle;
		const char* builtinDefines[2];
		pc_token_s token;

		builtinDefines[0] = "PC";
		builtinDefines[1] = 0;

		ZONETOOL_INFO("Parsing menu '%s'...", menuFile);

		handle = PC_LoadSourceHandle(menuFile, builtinDefines);

		if (handle)
		{
			while (PC_ReadTokenHandle(&token))
			{
				if (_stricmp(token.string, "}") && _stricmp(token.string, "{"))
				{
					if (_stricmp(token.string, "assetGlobalDef"))
					{
						if (_stricmp(token.string, "menudef"))
						{
							PC_SourceError(

								"Unknown token %s in menu file.  Expected \"menudef\" or \"assetglobaldef\".",
								token.string);
							break;
						}
						else if (!Menu_New())
						{
							break;
						}
					}
					//else if (!Asset_Parse(handle))
					//{
					//	break;
					//}
				}
			}
			//PC_FreeSourceHandle(handle);
			result = true;
		}
		else
		{
			ZONETOOL_ERROR("Couldn't find menu file '%s'", menuFile);
			result = false;
		}

		return result;
	}

	MenuList* menu_list::parse(const std::string& name, zone_memory* mem)
	{
		MenuList* asset = nullptr;
		menu_memory menu_memory;

		menuList = mem->allocate<MenuList>();
		menuList->name = mem->duplicate_string(name.data());
		menuList->menus = mem->allocate<menuDef_t*>(MAX_MENUDEFS_PER_MENULIST);

		zmem = mem;
		mmem = &menu_memory;

		if (!UI_ParseMenuInternal(name.data()))
		{
			return nullptr;
		}

		asset = menuList;
		return asset;
	}

	void menu_list::init(const std::string& name, zone_memory* mem)
	{
		this->name_ = name;
		this->asset_ = parse(name, mem);

		if (!this->asset_)
		{
			this->asset_ = db_find_x_asset_header_safe(XAssetType(this->type()), this->name().data()).menuList;
		}
	}

	void menu_list::prepare(zone_buffer* buf, zone_memory* mem)
	{}

	void menu_list::load_depending(zone_base* zone)
	{
		auto* data = this->asset_;
		if (data->menus)
		{
			for (int i = 0; i < data->menuCount; i++)
			{
				zone->add_asset_of_type_by_pointer(ASSET_TYPE_MENU, data->menus[i]);
			}
		}
	}

	std::string menu_list::name()
	{
		return this->name_;
	}

	std::int32_t menu_list::type()
	{
		return ASSET_TYPE_MENULIST;
	}

	void menu_list::read(zone_reader& reader, MenuList* asset)
	{
		reader.push_stream(XFILE_BLOCK_VIRTUAL);
		reader.read_string(asset->name);
		if (const auto menus = reader.read_array(asset->menus, 7, asset->menuCount))
		{
			for (auto i = 0; i < asset->menuCount; i++)
			{
				reader.read_asset(ASSET_TYPE_MENU, menus[i]);
			}
		}
		reader.pop_stream();
	}

	void menu_list::write(zone_base* zone, zone_buffer* buf)
	{
		auto data = this->asset_;
		auto dest = buf->write(data);

		buf->push_stream(XFILE_BLOCK_VIRTUAL);

		dest->name = buf->write_str(this->name_);

		if (data->menus)
		{
			buf->align(7);
			auto* destMenus = buf->write(data->menus, data->menuCount);
			for (int i = 0; i < data->menuCount; i++)
			{
				destMenus[i] = reinterpret_cast<menuDef_t*>(zone->get_asset_pointer(ASSET_TYPE_MENU, data->menus[i]->window.name));
			}
		}

		buf->pop_stream();
	}

	int menu_list::indentCounter = 0;
	FILE* menu_list::fp = nullptr;

	const char* menu_list::get_tabs()
	{
		static char tabs[10];
		const int n = std::clamp(indentCounter, 0, 9); // cap at 9 tabs
		memset(tabs, '\t', n);
		tabs[n] = '\0';
		return tabs;
	}

	void menu_list::emit_open_brace()
	{
		fprintf(fp, "%s{\n", get_tabs());
	}

	void menu_list::emit_closing_brace()
	{
		fprintf(fp, "%s}\n", get_tabs());
	}

	void menu_list::push_indent()
	{
		emit_open_brace();
		indentCounter++;
	}

	void menu_list::pop_indent()
	{
		indentCounter--;
		emit_closing_brace();
	}

	void menu_list::replace_all(std::string& str, std::string from, std::string to, bool case_insensitive)
	{
		if (case_insensitive)
		{
			auto replace = [&](std::string from, std::string to) -> bool
			{
				std::string lowered(str);
				std::transform(str.begin(), str.end(), lowered.begin(), ::tolower);

				auto pos = lowered.find(from);
				if (pos == std::string::npos)
				{
					return false;
				}

				str.replace(pos, from.length(), to);
				return true;
			};

			std::transform(from.begin(), from.end(), from.begin(), ::tolower);
			while (replace(from, to));
		}
		else
		{
			std::size_t pos = 0;
			while ((pos = str.find(from, pos)) != std::string::npos)
			{
				str.replace(pos, from.length(), to);
				pos += to.length();
			}
		}
	}

	std::string menu_list::escape_string(const char* value)
	{
		std::string out(value);
		replace_all(out, "\t", "\\t");
		replace_all(out, "\n", "\\n");
		return out;
	}

	std::string menu_list::format_script(const char* value)
	{
		std::string out = escape_string(value);
		const std::string tabs = get_tabs();

		// clean indentation and semi colons
		out += "\n";
		replace_all(out, "\"", "");		// Remove all quotes

		replace_all(out, "\t;", "\t");
		replace_all(out, tabs + "\n", "");
		replace_all(out, " ; \n", ";\n");
		replace_all(out, " \n", ";\n");
		replace_all(out, " ; ", ";\n" + tabs);
		replace_all(out, "; ", ";\n" + tabs);

		// remove quotes from keywords
		replace_all(out, "\"self\"", "self");
		replace_all(out, "\"forecolor\"", "forecolor");
		replace_all(out, "\"backcolor\"", "backcolor");
		replace_all(out, "\"bordercolor\"", "bordercolor");

		// remove quotes from commands
		for (auto i = 0; i < std::size(g_commandList); i++)
		{
			replace_all(out, "\""s + g_commandList[i] + "\""s, g_commandList[i], true);
		}

		return out;
	}

	void menu_list::emit_string(const char* name, const char* value)
	{
		if (value && *value)
		{
			fprintf(fp, "%s%s \"%s\"\n", get_tabs(), name, value);
		}
	}

	void menu_list::emit_bool(const char* name, bool value)
	{
		if (value)
		{
			fprintf(fp, "%s%s\n", get_tabs(), name);
		}
	}

	void menu_list::emit_int(const char* name, int value)
	{
		if (value)
		{
			fprintf(fp, "%s%s %i\n", get_tabs(), name, value);
		}
	}

	void menu_list::emit_float(const char* name, float value)
	{
		if (value)
		{
			fprintf(fp, "%s%s %g\n", get_tabs(), name, value);
		}
	}

	void menu_list::emit_rect(const char* name, rectDef_s& rect)
	{
		fprintf(fp, "%s%s %g %g %g %g %i %i\n", get_tabs(), name, rect.x, rect.y, rect.w, rect.h, rect.horzAlign, rect.vertAlign);
	}

	void menu_list::emit_color(const char* name, vec4_t& color)
	{
		if (color[0] > 0.0f || color[1] > 0.0f || color[2] > 0.0f || color[3] > 0.0f)
		{
			fprintf(fp, "%s%s %g %g %g %g\n", get_tabs(), name, color[0], color[1], color[2], color[3]);
		}
	}

	void menu_list::emit_statement(const char* name, Statement_s* statement, bool semiColon, bool linebreak)
	{
		if (!statement)
		{
			return;
		}

		if (name)
		{
			fprintf(fp, "%s%s", get_tabs(), name);
		}

		// wrap the whole expression in parentheses unless it already starts with one
		const bool wrap_in_parens = statement->entries[0].data.op != OP_LEFTPAREN;
		if (wrap_in_parens)
		{
			fputs(" (", fp);
		}

		int num_open_parenthesis = 0;

		for (auto i = 0; i < statement->numEntries; i++)
		{
			const auto& entry = statement->entries[i];

			// a leading '!' suppresses the space before the following token
			const bool prev_is_not = i > 0 && statement->entries[i - 1].data.op == OP_NOT;

			if (entry.type == OPERATOR)
			{
				const auto op = entry.data.op;
				if (op < OP_FIRSTFUNCTIONCALL)
				{
					if (op == OP_LEFTPAREN)
					{
						++num_open_parenthesis;
					}
					else if (op == OP_RIGHTPAREN)
					{
						--num_open_parenthesis;
					}

					fprintf(fp, prev_is_not ? "%s" : " %s", g_expOperatorNames[op]);
				}
				else if (op >= OP_STATICDVARINT && op <= OP_STATICDVARSTRING)
				{
					const char* command = "";
					switch (op)
					{
					case OP_STATICDVARINT:
						command = "dvarint";
						break;
					case OP_STATICDVARBOOL:
						command = "dvarbool";
						break;
					case OP_STATICDVARFLOAT:
						command = "dvarfloat";
						break;
					case OP_STATICDVARSTRING:
						command = "dvarstring";
						break;
					}

					// the dvar index lives in the operand that follows the op
					const auto dvarIndex = statement->entries[++i].data.operand.internals.intVal;
					const char* dvarName = statement->supportingData->staticDvarList.staticDvars[dvarIndex]->dvarName;

					fprintf(fp, prev_is_not ? "%s( \"%s\" )" : " %s( \"%s\" )", command, dvarName);

					i++;
				}
				else
				{
					fprintf(fp, prev_is_not ? "%s(" : " %s(", g_expOperatorNames[op]);
					++num_open_parenthesis;
				}
			}
			else if (entry.type == OPERAND)
			{
				switch (entry.data.operand.dataType)
				{
				case VAL_INT:
					fprintf(fp, " %i", entry.data.operand.internals.intVal);
					break;
				case VAL_FLOAT:
					fprintf(fp, " %g", entry.data.operand.internals.floatVal);
					break;
				case VAL_STRING:
					fprintf(fp, " \"%s\"", escape_string(entry.data.operand.internals.stringVal.string).c_str());
					break;
				case VAL_FUNCTION:
					emit_statement(nullptr, entry.data.operand.internals.function, false, false);
					break;
				}
			}
		}

		while (num_open_parenthesis > 0)
		{
			fputs(" )", fp);
			--num_open_parenthesis;
		}

		if (wrap_in_parens)
		{
			fputs(" )", fp);
		}

		if (semiColon)
		{
			fputs(";", fp);
		}

		if (linebreak)
		{
			fputs("\n", fp);
		}
	}

	void menu_list::emit_dynamic_flags(int flags)
	{
		emit_bool("visible 1", flags & WINDOWDYNAMIC_VISIBLE);
	}

	void menu_list::emit_static_flags(int flags)
	{
		emit_bool("decoration", flags & WINDOWSTATIC_DECORATION);
		emit_bool("horizontalscroll", flags & WINDOWSTATIC_HORIZONTALSCROLL);
		emit_bool("screenSpace", flags & WINDOWSTATIC_SCREENSPACE);
		emit_bool("autowrapped", flags & WINDOWSTATIC_AUTOWRAPPED);
		emit_bool("popup", flags & WINDOWSTATIC_POPUP);
		emit_bool("outOfBoundsClick", flags & WINDOWSTATIC_OUTOFBOUNDSCLICK);
		emit_bool("legacySplitScreenScale", flags & WINDOWSTATIC_LEGACYSPLITSCREENSCALE);
		emit_bool("hiddenDuringFlashbang", flags & WINDOWSTATIC_HIDDENDURINGFLASH);
		emit_bool("hiddenDuringScope", flags & WINDOWSTATIC_HIDDENDURINGSCOPE);
		emit_bool("hiddenDuringUI", flags & WINDOWSTATIC_HIDDENDURINGUI);
		emit_bool("textOnlyFocus", flags & WINDOWSTATIC_TEXTONLYFOCUS);
	}

	void menu_list::emit_set_local_var_data(SetLocalVarData* data, EventType type)
	{
		if (data)
		{
			std::string cmd;
			switch (type)
			{
			case EVENT_SET_LOCAL_VAR_BOOL:
				cmd = "setLocalVarBool";
				break;
			case EVENT_SET_LOCAL_VAR_INT:
				cmd = "setLocalVarInt";
				break;
			case EVENT_SET_LOCAL_VAR_FLOAT:
				cmd = "setLocalVarFloat";
				break;
			case EVENT_SET_LOCAL_VAR_STRING:
				cmd = "setLocalVarString";
				break;
			}

			fprintf(fp, "%s%s \"%s\"", get_tabs(), cmd.c_str(), data->localVarName);
			emit_statement(nullptr, data->expression, true);
		}
	}

	void menu_list::emit_unconditional_script(const char* script)
	{
		if (!script || !strlen(script) || script[0] == ';')
		{
			return;
		}

		fprintf(fp, "%s%s", get_tabs(), format_script(script).c_str());
	}

	void menu_list::emit_conditional_script(ConditionalScript* script)
	{
		if (script)
		{
			fprintf(fp, "%sif", get_tabs());
			emit_statement(nullptr, script->eventExpression);
			push_indent();
			emit_menu_event_handler_set(nullptr, script->eventHandlerSet);
			pop_indent();
		}
	}

	void menu_list::emit_menu_event_handler_set(const char* name, MenuEventHandlerSet* set)
	{
		if (set)
		{
			if (name)
			{
				fprintf(fp, "%s%s\n", get_tabs(), name);
				push_indent();
			}

			for (auto i = 0; i < set->eventHandlerCount; i++)
			{
				switch (set->eventHandlers[i]->eventType)
				{
				case EVENT_UNCONDITIONAL:
					emit_unconditional_script(set->eventHandlers[i]->eventData.unconditionalScript);
					break;
				case EVENT_IF:
					emit_conditional_script(set->eventHandlers[i]->eventData.conditionalScript);
					break;
				case EVENT_ELSE:
					emit_menu_event_handler_set("else", set->eventHandlers[i]->eventData.elseScript);
					break;
				case EVENT_SET_LOCAL_VAR_BOOL:
				case EVENT_SET_LOCAL_VAR_INT:
				case EVENT_SET_LOCAL_VAR_FLOAT:
				case EVENT_SET_LOCAL_VAR_STRING:
					emit_set_local_var_data(set->eventHandlers[i]->eventData.setLocalVarData, set->eventHandlers[i]->eventType);
					break;
				default:
					break;
				}
			}

			if (name)
			{
				pop_indent();
			}
		}
	}

	void menu_list::emit_item_key_handler(const char* name, ItemKeyHandler* handler)
	{
		if (handler)
		{
			std::string keyName = std::string(name) + " "s + std::to_string(handler->key);
			emit_menu_event_handler_set(keyName.c_str(), handler->action);
			emit_item_key_handler("execKeyInt", handler->next);
		}
	}

	void menu_list::emit_window_def(windowDef_t* window, bool is_item)
	{
		emit_string("name", window->name);
		if (is_item)
		{
			emit_rect("rect", window->rectClient);
		}
		else
		{
			emit_rect("rect", window->rect);
		}
		emit_static_flags(window->staticFlags);
		emit_dynamic_flags(window->dynamicFlags[0]);
		emit_string("group", window->group);
		emit_int("style", window->style);
		emit_int("border", window->border);
		emit_float("borderSize", window->borderSize);
		emit_int("ownerdraw", window->ownerDraw);
		emit_int("ownerdrawFlag", window->ownerDrawFlags);
		emit_color("forecolor", window->foreColor);
		emit_color("backcolor", window->backColor);
		emit_color("bordercolor", window->borderColor);
		emit_color("outlinecolor", window->outlineColor);
		emit_color("disablecolor", window->disableColor);

		if (window->background)
		{
			emit_string("background", window->background->name);
		}
	}

	void menu_list::emit_column_info(columnInfo_s* columns, int count)
	{
		fprintf(fp, "//%snumcol\t\t\txpos\txwidth\ttextlen\t  alignment\n", get_tabs());
		fprintf(fp, "%s%s %i\t\t%i\t\t%i\t\t%i\t\t  %i\n", get_tabs(), "columns", count, columns[0].xpos, columns[0].width, columns[0].maxChars, columns[0].alignment);
		for (int i = 1; i < count; i++)
		{
			fprintf(fp, "%s\t\t\t\t%i\t\t%i\t\t%i\t\t  %i\n", get_tabs(), columns[i].xpos, columns[i].width, columns[i].maxChars, columns[i].alignment);
		}
	}

	void menu_list::emit_list_box(listBoxDef_s* listBox)
	{
		emit_color("selectBorder", listBox->selectBorder);

		if (listBox->selectIcon)
		{
			emit_string("selectIcon", listBox->selectIcon->name);
		}

		emit_float("elementWidth", listBox->elementWidth);
		emit_float("elementHeight", listBox->elementHeight);
		emit_int("elementtype", listBox->elementStyle);
		emit_column_info(listBox->columnInfo, listBox->numColumns);
		emit_bool("noscrollbars", listBox->noScrollBars);
		emit_bool("notselectable", listBox->notselectable);
		emit_bool("usepaging", listBox->usePaging);
		emit_menu_event_handler_set("doubleClick", listBox->onDoubleClick);
	}

	void menu_list::emit_multi_def(multiDef_s* multiDef)
	{
		if (multiDef->strDef)
		{
			fprintf(fp, "%s%s {", get_tabs(), "dvarStrList");
		}
		else
		{
			fprintf(fp, "%s%s {", get_tabs(), "dvarFloatList");
		}

		for (auto i = 0; i < multiDef->count; i++)
		{
			fprintf(fp, " \"%s\"", multiDef->dvarList[i]);
			if (multiDef->strDef)
			{
				fprintf(fp, " \"%s\"", multiDef->dvarStr[i]);
			}
			else
			{
				fprintf(fp, " %g", multiDef->dvarValue[i]);
			}
		}

		fputs(" }\n", fp);
	}

	void menu_list::emit_item_def_data(itemDefData_t* data, int type)
	{
		if (data->data)
		{
			switch (type)
			{
			case ITEM_TYPE_LISTBOX:
				emit_list_box(data->listBox);
				break;
			case ITEM_TYPE_MULTI:
				emit_multi_def(data->multi);
				break;
			case ITEM_TYPE_DVARENUM:
				emit_string("dvarEnumList", data->enumDvarName);
				break;
			case ITEM_TYPE_NEWSTICKER:
				emit_int("newsfeed", data->ticker->feedId);
				emit_int("speed", data->ticker->speed);
				emit_int("spacing", data->ticker->spacing);
				break;
			case ITEM_TYPE_EDITFIELD:
			case ITEM_TYPE_NUMERICFIELD:
			case ITEM_TYPE_VALIDFILEFIELD:
			case ITEM_TYPE_UPREDITFIELD:
			case ITEM_TYPE_YESNO:
			case ITEM_TYPE_BIND:
			case ITEM_TYPE_TEXT:
			case ITEM_TYPE_DECIMALFIELD:
			case ITEM_TYPE_EMAILFIELD:
			case ITEM_TYPE_PASSWORDFIELD:
				emit_int("maxChars", data->editField->maxChars);
				emit_int("maxCharsGotoNext", data->editField->maxCharsGotoNext);
				emit_int("maxPaintChars", data->editField->maxPaintChars);
				break;
			case ITEM_TYPE_TEXTSCROLL:
			default:
				break;
			}
		}
	}

	void menu_list::emit_dvar_flags(int dvarFlags, const char* dvarTest, const char* enableDvar)
	{
		std::string command;
		switch (dvarFlags)
		{
		case 0x01:
			command = "enableDvar";
			break;
		case 0x02:
			command = "disableDvar";
			break;
		case 0x04:
			command = "showDvar";
			break;
		case 0x08:
			command = "hideDvar";
			break;
		case 0x10:
			command = "focusDvar";
			break;
		}

		emit_string("dvarTest", dvarTest);

		if (enableDvar)
		{
			fprintf(fp, "%s%s { %s}\n", get_tabs(), command.c_str(), enableDvar);
		}
	}

	void menu_list::emit_item_float_expressions(ItemFloatExpression* expressions, int count)
	{
		if (expressions)
		{
			for (auto i = 0; i < count; i++)
			{
				std::string name;
				name = "exp ";
				name += g_itemFloatExpressions[expressions[i].target].s1;
				name += " ";
				name += g_itemFloatExpressions[expressions[i].target].s2;
				emit_statement(name.c_str(), expressions[i].expression);
			}
		}
	}

	void menu_list::emit_item_def(itemDef_t* item)
	{
		fputs("\t\titemDef\n", fp);
		push_indent();

		emit_window_def(&item->window, true);
		emit_int("type", item->type);
		emit_int("align", item->alignment);
		emit_int("textfont", item->fontEnum);
		emit_int("textalign", item->textAlignMode);
		emit_float("textalignx", item->textalignx);
		emit_float("textaligny", item->textaligny);
		emit_int("textstyle", item->textStyle);
		emit_float("textscale", item->textscale);
		emit_color("glowColor", item->glowColor);
		if (item->decayActive)
		{
			fprintf(fp, "%sdecodeEffect %d %d %d\n",
				get_tabs(),
				item->fxLetterTime,
				item->fxDecayStartTime,
				item->fxDecayDuration);
		}

		if (item->type == ITEM_TYPE_GAME_MESSAGE_WINDOW)
		{
			emit_int("gamemsgwindowindex", item->gameMsgWindowIndex);
			emit_int("gamemsgwindowmode", item->gameMsgWindowMode);
		}

		emit_string("text", item->text);
		emit_bool("textsavegame", item->itemFlags & 1);
		emit_bool("textcinematicsubtitle", item->itemFlags & 2);
		emit_float("feeder", item->special);
		if (item->focusSound)
		{
			emit_string("focusSound", item->focusSound->name);
		}

		if (item->type != ITEM_TYPE_SLIDER && item->type != ITEM_TYPE_DVARENUM)
		{
			emit_string("dvar", item->dvar);
			emit_item_def_data(&item->typeData, item->type);
		}
		else
		{
			emit_item_def_data(&item->typeData, item->type);
		}

		emit_dvar_flags(item->dvarFlags, item->dvarTest, item->enableDvar);
		emit_item_float_expressions(item->floatExpressions, item->floatExpressionCount);
		emit_statement("visible when", item->visibleExp);
		emit_statement("disabled when", item->disabledExp);
		emit_statement("exp text", item->textExp);
		emit_statement("exp material", item->materialExp);
		emit_item_key_handler("execKeyInt", item->onKey);
		emit_menu_event_handler_set("mouseEnterText", item->mouseEnterText);
		emit_menu_event_handler_set("mouseExitText", item->mouseExitText);
		emit_menu_event_handler_set("mouseEnter", item->mouseEnter);
		emit_menu_event_handler_set("mouseExit", item->mouseExit);
		emit_menu_event_handler_set("action", item->action);
		emit_menu_event_handler_set("accept", item->accept);
		emit_menu_event_handler_set("onFocus", item->onFocus);
		emit_menu_event_handler_set("leaveFocus", item->leaveFocus);

		pop_indent();
	}

	void menu_list::emit_menu_def(menuDef_t* asset)
	{
		fprintf(fp, "%smenuDef\n", get_tabs());
		push_indent();

		emit_window_def(&asset->window, false);
		if (asset->data)
		{
			emit_color("focuscolor", asset->data->focusColor);
			emit_int("fullscreen", asset->data->fullScreen);
			emit_int("fadeCycle", asset->data->fadeCycle);
			emit_float("fadeClamp", asset->data->fadeClamp);
			emit_float("fadeAmount", asset->data->fadeAmount);
			emit_float("fadeInAmount", asset->data->fadeInAmount);
			emit_float("blurWorld", asset->data->blurRadius);
			emit_string("allowedBinding", asset->data->allowedBinding);
			emit_string("soundLoop", asset->data->soundName);
			emit_statement("visible when", asset->data->visibleExp);
			emit_statement("exp rect x", asset->data->rectXExp);
			emit_statement("exp rect y", asset->data->rectYExp);
			emit_statement("exp rect w", asset->data->rectWExp);
			emit_statement("exp rect h", asset->data->rectHExp);
			emit_statement("exp openSound", asset->data->openSoundExp);
			emit_statement("exp closeSound", asset->data->closeSoundExp);
			emit_item_key_handler("execKeyInt", asset->data->onKey);
			emit_menu_event_handler_set("onOpen", asset->data->onOpen);
			emit_menu_event_handler_set("onRequestClose", asset->data->onCloseRequest);
			emit_menu_event_handler_set("onClose", asset->data->onClose);
			emit_menu_event_handler_set("onEsc", asset->data->onESC);
		}

		for (auto i = 0; i < asset->itemCount; i++)
		{
			emit_item_def(asset->items[i]);
		}

		pop_indent();
	}

	void menu_list::dump(MenuList* asset)
	{
		auto file = filesystem::file(asset->name);
		file.open("wb");
		fp = file.get_fp();
		if (fp)
		{
			ZONETOOL_INFO("Dumping menu \"%s\"...", asset->name);
			indentCounter = 0;
			push_indent();
			for (int i = 0; i < asset->menuCount; i++)
			{
				emit_menu_def(asset->menus[i]);
			}
			pop_indent();
		}

		file.close();
	}
}
