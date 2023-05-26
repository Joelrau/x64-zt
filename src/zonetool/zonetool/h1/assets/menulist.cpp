#include <std_include.hpp>
#include "menulist.hpp"

// trash code, ignore all warnings
#pragma warning( push )
#pragma warning( disable : 4309)
#pragma warning( disable : 4146)
#pragma warning( disable : 4244)
#pragma warning( disable : 4267)
#pragma warning( disable : 4311)
#pragma warning( disable : 4302)
#pragma warning( disable : 4267)
#pragma warning( disable : 4312)
#pragma warning( disable : 4834)
#pragma warning( disable : 4101)
#pragma warning( disable : 4702)
#pragma warning( disable : 4701)
#pragma warning( disable : 4700)

namespace zonetool::h1
{
	/* windowDef_t->dynamicFlags */
// 0x1
#define WINDOWDYNAMIC_HASFOCUS		0x00000002
#define WINDOWDYNAMIC_VISIBLE		0x00000004
#define WINDOWDYNAMIC_FADEOUT		0x00000010
#define WINDOWDYNAMIC_FADEIN		0x00000020
// 0x40
// 0x80
#define WINDOWDYNAMIC_CLOSED		0x00000800
// 0x2000
#define WINDOWDYNAMIC_BACKCOLOR		0x00008000
#define WINDOWDYNAMIC_FORECOLOR		0x00010000

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
		"inprivatepartyhost",
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
		"isprofileitemnew",
		"isprofileitemnewsplitscreen",
		"debugprint",
		"getplayerdataanybooltrue",
		"getprofileanybooltrue",
		"weaponclassnew",
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
		"getclientmatchdata",
		"getclientmatchdatadef",
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
		"getPastTitleRank",
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
		"getPastTitlePrestige",
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
		"commerce_getstatus",
		"isManifestDownloaded",
		"areAllItemsUnlocked",
		"commerce_getstatuscond",
		"doWeHaveMissingOwnedContent"
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
		"setPlayerData",
		"setPlayerDataSplitScreen",
		"resetStatsConfirm",
		"resetStatsCancel",
		"setGameMode",
		"deleteEliteCacFile",
		"integrateEliteCacFile",
		"setMatchRulesData",
		"loadMatchRulesDataFromPlayer",
		"saveMatchRulesDataToPlayer",
		"loadMatchRulesDataFromHistory",
		"loadMatchRulesDataDefaults",
		"setUsingMatchRulesData",
		"saveMatchRulesDedicatedServer",
		"loadMatchRulesDedicatedServer",
		"onlineVaultEditMetadata",
		"includeInMapRotation",
		"usecustommaprotation",
		"useintermissiontimer",
		"stopintermissiontimer",
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
		"resolveError",
		"runCompletionResolve",
		"clearCompletionResolve",
		"execWithResolve",
		"lerp",
		"playMenuVideo",
		"setBackgroundVideo",
		"partyAbortWaiting",
		"clearEntitlementNew",
		"setPastTitleRank",
		"setPastTitlePrestige",
		"anticheat_bancheck"
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

	enum parseSkip_t : __int32
	{
		SKIP_NO = 0x0,
		SKIP_YES = 0x1,
		SKIP_ALL_ELIFS = 0x2,
	};

	//punctuation
	struct punctuation_s
	{
		const char* p;						//punctuation character(s)
		int n;							//punctuation indication
		punctuation_s* next;			//next punctuation
	};

	//token
	struct token_s
	{
		char string[MAX_TOKEN];			//available token
		int type;						//last read token type
		int subtype;					//last read token sub type
		unsigned int intvalue;			//integer value
		long double floatvalue;			//floating point value
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
		int length;						//length of the script in bytes
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
		int(__cdecl* func)(source_s*);
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
		{"", 0, 0}
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
		long double floatvalue;
		int parentheses;
		struct value_s* prev, * next;
	};

	struct parse_menudef_func
	{
		const char* keyword;
		int(__cdecl* func)(menuDef_t*/*, int*/);
	};

	struct parse_itemdef_func
	{
		const char* keyword;
		int(__cdecl* func)(itemDef_t*/*, int*/);
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
				ZONETOOL_FATAL("MenuFile could not allocate memory...");
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
				ZONETOOL_FATAL("MenuFile buffer out of memory...");
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

	int PC_SourceFileAndLine(/*int handle,*/ char* filename, int* line)
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

	void PC_SourceError(/*int handle,*/ const char* format, ...)
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
		PC_SourceFileAndLine(/*handle,*/ filename, &line);
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
		char c, val, i;

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
		*cha = c;
		//succesfully read escape character
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
		int len;
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
		long double* floatvalue)
	{
		unsigned long int dotfound = 0;

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
					*floatvalue = *floatvalue + (long double)(*string - '0') /
						(long double)dotfound;
					dotfound *= 10;
				}
				else
				{
					*floatvalue = *floatvalue * 10.0 + (long double)(*string - '0');
				}
				string++;
			}
			*intvalue = (unsigned long)*floatvalue;
		}
		else if (subtype & TT_DECIMAL)
		{
			while (*string) *intvalue = *intvalue * 10 + (*string++ - '0');
			*floatvalue = *intvalue;
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
			*floatvalue = *intvalue;
		}
		else if (subtype & TT_OCTAL)
		{
			//step over the first zero
			string += 1;
			while (*string) *intvalue = (*intvalue << 3) + (*string++ - '0');
			*floatvalue = *intvalue;
		}
		else if (subtype & TT_BINARY)
		{
			//step over the leading 0b or 0B
			string += 2;
			while (*string) *intvalue = (*intvalue << 1) + (*string++ - '0');
			*floatvalue = *intvalue;
		}
	}

	int PS_ReadNumber(script_s* script, token_s* token)
	{
		int len = 0, i;
		int octal, dot;
		char c;
		//	unsigned long int intvalue = 0;
		//	long double floatvalue = 0;

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
		NumberValue(token->string, token->subtype, &token->intvalue, &token->floatvalue);
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
			ZONETOOL_FATAL("EXE_ERR_OUT_OF_MEMORY");
			copy = 0;
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

	void PC_UnreadLastTokenHandle(/*int handle*/)
	{
		//if (handle >= 1 && handle < 64)
		//{
		//	if (sourceFiles[handle])
		//		PC_UnreadLastToken(sourceFiles[handle]);
		//}
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
		return ((unsigned __int16)(hash >> 20) ^ (unsigned __int16)(hash ^ (hash >> 10))) & 0x3FF;
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
		__int64 t;	// time_t t;
		char* curtime;

		token = PC_CopyToken(deftoken);
		switch (define->builtin)
		{
		case BUILTIN_LINE:
			sprintf(token->string, "%d", deftoken->line);
			token->intvalue = deftoken->line;
			token->floatvalue = deftoken->line;
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

	int __cdecl PC_ReadDefineParms(source_s* source, define_s* define, token_s** parms, int maxparms)
	{
		token_s* t; // [esp+50h] [ebp-454h]
		int done; // [esp+54h] [ebp-450h]
		int lastcomma; // [esp+58h] [ebp-44Ch]
		int indent; // [esp+5Ch] [ebp-448h]
		int numparms; // [esp+60h] [ebp-444h]
		token_s token; // [esp+64h] [ebp-440h] BYREF
		int i; // [esp+49Ch] [ebp-8h]
		token_s* last; // [esp+4A0h] [ebp-4h]

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
		done = 0;
		numparms = 0;
		indent = 0;
	LABEL_11:
		if (!done)
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
			lastcomma = 1;
			last = 0;
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
					goto LABEL_38;
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
					LABEL_38:
						++numparms;
						goto LABEL_11;
					}
					--indent;
				}
				if (numparms < define->numparms)
				{
					t = PC_CopyToken(&token);
					t->next = 0;
					if (last)
						last->next = t;
					else
						parms[numparms] = t;
					last = t;
				}
			}
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

	int __cdecl PC_ExpandDefine(source_s* source, token_s* deftoken, define_s* define, token_s** firsttoken, token_s** lasttoken)
	{
		int parmnum; // [esp+0h] [ebp-65Ch]
		int parmnuma; // [esp+0h] [ebp-65Ch]
		token_s* dt; // [esp+4h] [ebp-658h]
		token_s* t; // [esp+8h] [ebp-654h]
		token_s* ta; // [esp+8h] [ebp-654h]
		token_s* tb; // [esp+8h] [ebp-654h]
		token_s* nextpt; // [esp+10h] [ebp-64Ch]
		token_s* parms[128]; // [esp+14h] [ebp-648h] BYREF
		token_s* t2; // [esp+214h] [ebp-448h]
		token_s* pt; // [esp+218h] [ebp-444h]
		token_s token; // [esp+21Ch] [ebp-440h] BYREF
		int i; // [esp+650h] [ebp-Ch]
		token_s* last; // [esp+654h] [ebp-8h]
		token_s* first; // [esp+658h] [ebp-4h]

		if (define->builtin)
			return PC_ExpandBuiltinDefine(source, deftoken, define, firsttoken, lasttoken);
		if (define->numparms && !PC_ReadDefineParms(source, define, parms, 128))
			return 0;
		first = 0;
		last = 0;
		for (dt = define->tokens; dt; dt = dt->next)
		{
			parmnum = -1;
			if (dt->type == 4)
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

	int __cdecl PC_EvaluateTokens(source_s* source, token_s* tokens, int* intvalue, long double* floatvalue, int integer)
	{
		BOOL v6; // [esp+18h] [ebp-DC0h]
		int v7; // [esp+1Ch] [ebp-DBCh]
		BOOL v8; // [esp+20h] [ebp-DB8h]
		int v9; // [esp+24h] [ebp-DB4h]
		int v10; // [esp+74h] [ebp-D64h]
		operator_s* o; // [esp+78h] [ebp-D60h]
		operator_s* oa; // [esp+78h] [ebp-D60h]
		operator_s* ob; // [esp+78h] [ebp-D60h]
		double questmarkfloatvalue; // [esp+80h] [ebp-D58h]
		operator_s* lastoperator; // [esp+8Ch] [ebp-D4Ch]
		value_s value_heap[64]; // [esp+90h] [ebp-D48h] BYREF
		operator_s* firstoperator; // [esp+894h] [ebp-544h]
		int lastwasvalue; // [esp+898h] [ebp-540h]
		value_s* v2; // [esp+89Ch] [ebp-53Ch]
		int error; // [esp+8A0h] [ebp-538h]
		int negativevalue; // [esp+8A4h] [ebp-534h]
		operator_s operator_heap[64]; // [esp+8A8h] [ebp-530h] BYREF
		value_s* firstvalue; // [esp+DACh] [ebp-2Ch]
		value_s* v1; // [esp+DB0h] [ebp-28h]
		int numvalues; // [esp+DB4h] [ebp-24h]
		int parentheses; // [esp+DB8h] [ebp-20h]
		int numoperators; // [esp+DBCh] [ebp-1Ch]
		value_s* v; // [esp+DC0h] [ebp-18h]
		value_s* lastvalue; // [esp+DC4h] [ebp-14h]
		int lastoperatortype; // [esp+DC8h] [ebp-10h]
		int brace; // [esp+DCCh] [ebp-Ch]
		int questmarkintvalue; // [esp+DD0h] [ebp-8h]
		int gotquestmarkvalue; // [esp+DD4h] [ebp-4h]

		brace = 0;
		parentheses = 0;
		error = 0;
		lastwasvalue = 0;
		negativevalue = 0;
		questmarkintvalue = 0;
		gotquestmarkvalue = 0;
		lastoperatortype = 0;
		numoperators = 0;
		numvalues = 0;
		lastoperator = 0;
		firstoperator = 0;
		lastvalue = 0;
		firstvalue = 0;
		if (intvalue)
			*intvalue = 0;
		if (floatvalue)
			*floatvalue = 0.0f;
		while (tokens)
		{
			v10 = tokens->type;
			switch (v10)
			{
			case 3:
				if (lastwasvalue)
					goto LABEL_37;
				if (numvalues >= 64)
				{
				LABEL_24:
					SourceError(source, "out of value space");
					error = 1;
					break;
				}
				v = &value_heap[numvalues++];
				if (negativevalue)
				{
					v->intvalue = -tokens->intvalue;
					v->floatvalue = -tokens->floatvalue;
				}
				else
				{
					v->intvalue = tokens->intvalue;
					v->floatvalue = tokens->floatvalue;
				}
				v->parentheses = parentheses;
				v->next = 0;
				v->prev = lastvalue;
				if (lastvalue)
					lastvalue->next = v;
				else
					firstvalue = v;
				lastvalue = v;
				lastwasvalue = 1;
				negativevalue = 0;
				break;
			case 4:
				if (lastwasvalue || negativevalue)
				{
				LABEL_37:
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
				if (tokens && tokens->type == 4)
				{
					if (numvalues >= 64)
						goto LABEL_24;
					v = &value_heap[numvalues++];
					if (PC_FindHashedDefine(source->definehash, tokens->string))
					{
						v->intvalue = 1;
						v->floatvalue = 1.0f;
					}
					else
					{
						v->intvalue = 0;
						v->floatvalue = 0.0f;
					}
					v->parentheses = parentheses;
					v->next = 0;
					v->prev = lastvalue;
					if (lastvalue)
						lastvalue->next = v;
					else
						firstvalue = v;
					lastvalue = v;
					if (!brace || (tokens = tokens->next) != 0 && !strcmp(tokens->string, ")"))
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
			case 5:
				if (negativevalue)
				{
					SourceError(source, "misplaced minus sign in #if/#elif");
					error = 1;
				}
				else if (tokens->subtype == 44)
				{
					++parentheses;
				}
				else if (tokens->subtype == 45)
				{
					if (--parentheses < 0)
					{
						SourceError(source, "too many ) in #if/#elsif");
						error = 1;
					}
				}
				else if (!integer
					&& (tokens->subtype == 35
						|| tokens->subtype == 28
						|| tokens->subtype == 21
						|| tokens->subtype == 22
						|| tokens->subtype == 32
						|| tokens->subtype == 33
						|| tokens->subtype == 34))
				{
					SourceError(source, "illigal operator %s on floating point operands", tokens->string);
					error = 1;
				}
				else
				{
					switch (tokens->subtype)
					{
					case 5:
					case 6:
					case 7:
					case 8:
					case 9:
					case 0xA:
					case 0x15:
					case 0x16:
					case 0x1A:
					case 0x1B:
					case 0x1C:
					case 0x1D:
					case 0x20:
					case 0x21:
					case 0x22:
					case 0x25:
					case 0x26:
					case 0x2A:
					case 0x2B:
						goto $LN82;
					case 0x10:
					case 0x11:
						SourceError(source, "++ or -- used in #if/#elif");
						break;
					case 0x1E:
						if (lastwasvalue)
						{
						$LN82:
							if (!lastwasvalue)
							{
								SourceError(source, "operator %s after operator in #if/#elif", tokens->string);
								error = 1;
							}
						}
						else
						{
							negativevalue = 1;
						}
						break;
					case 0x23:
					case 0x24:
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
						if (numoperators < 64)
						{
							o = &operator_heap[numoperators++];
							o->op = tokens->subtype;
							o->priority = PC_OperatorPriority(tokens->subtype);
							o->parentheses = parentheses;
							o->next = 0;
							o->prev = lastoperator;
							if (lastoperator)
								lastoperator->next = o;
							else
								firstoperator = o;
							lastoperator = o;
							lastwasvalue = 0;
						}
						else
						{
							SourceError(source, "out of operator space");
							error = 1;
						}
					}
				}
				break;
			default:
				SourceError(source, "unknown %s in #if/#elif", tokens->string);
				error = 1;
				break;
			}
			if (error)
				break;
			tokens = tokens->next;
		}
		if (!error)
		{
			if (lastwasvalue)
			{
				if (parentheses)
				{
					SourceError(source, "too many ( in #if/#elif");
					error = 1;
				}
			}
			else
			{
				SourceError(source, "trailing operator in #if/#elif");
				error = 1;
			}
		}
		gotquestmarkvalue = 0;
		questmarkintvalue = 0;
		questmarkfloatvalue = 0.0f;
		while (!error && firstoperator)
		{
			v = firstvalue;
			for (oa = firstoperator;
				oa->next
				&& oa->parentheses <= oa->next->parentheses
				&& (oa->parentheses != oa->next->parentheses || oa->priority < oa->next->priority);
				oa = oa->next)
			{
				if (oa->op != 36 && oa->op != 35)
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
			v1 = v;
			v2 = v->next;
			switch (oa->op)
			{
			case 5:
				v9 = v1->intvalue && v2->intvalue;
				v1->intvalue = v9;
				v8 = v1->floatvalue != 0.0 && v2->floatvalue != 0.0;
				v1->floatvalue = (double)v8;
				break;
			case 6:
				v7 = v1->intvalue || v2->intvalue;
				v1->intvalue = v7;
				v6 = v1->floatvalue != 0.0 || v2->floatvalue != 0.0;
				v1->floatvalue = (double)v6;
				break;
			case 7:
				v1->intvalue = v1->intvalue >= v2->intvalue;
				v1->floatvalue = (double)(v1->floatvalue >= v2->floatvalue);
				break;
			case 8:
				v1->intvalue = v1->intvalue <= v2->intvalue;
				v1->floatvalue = (double)(v2->floatvalue >= v1->floatvalue);
				break;
			case 9:
				v1->intvalue = v1->intvalue == v2->intvalue;
				v1->floatvalue = (double)(v1->floatvalue == v2->floatvalue);
				break;
			case 0xA:
				v1->intvalue = v1->intvalue != v2->intvalue;
				v1->floatvalue = (double)(v1->floatvalue != v2->floatvalue);
				break;
			case 0x15:
				v1->intvalue >>= v2->intvalue;
				break;
			case 0x16:
				v1->intvalue <<= v2->intvalue;
				break;
			case 0x1A:
				v1->intvalue *= v2->intvalue;
				v1->floatvalue = v1->floatvalue * v2->floatvalue;
				break;
			case 0x1B:
				if (!v2->intvalue || v2->floatvalue == 0.0)
					goto LABEL_113;
				v1->intvalue /= v2->intvalue;
				v1->floatvalue = v1->floatvalue / v2->floatvalue;
				break;
			case 0x1C:
				if (v2->intvalue)
				{
					v1->intvalue %= v2->intvalue;
				}
				else
				{
				LABEL_113:
					SourceError(source, "divide by zero in #if/#elif");
					error = 1;
				}
				break;
			case 0x1D:
				v1->intvalue += v2->intvalue;
				v1->floatvalue = v1->floatvalue + v2->floatvalue;
				break;
			case 0x1E:
				v1->intvalue -= v2->intvalue;
				v1->floatvalue = v1->floatvalue - v2->floatvalue;
				break;
			case 0x20:
				v1->intvalue &= v2->intvalue;
				break;
			case 0x21:
				v1->intvalue |= v2->intvalue;
				break;
			case 0x22:
				v1->intvalue ^= v2->intvalue;
				break;
			case 0x23:
				v1->intvalue = ~v1->intvalue;
				break;
			case 0x24:
				v1->intvalue = v1->intvalue == 0;
				v1->floatvalue = (double)(v1->floatvalue == 0.0);
				break;
			case 0x25:
				v1->intvalue = v1->intvalue > v2->intvalue;
				v1->floatvalue = (double)(v1->floatvalue > v2->floatvalue);
				break;
			case 0x26:
				v1->intvalue = v1->intvalue < v2->intvalue;
				v1->floatvalue = (double)(v2->floatvalue > v1->floatvalue);
				break;
			case 0x2A:
				if (gotquestmarkvalue)
				{
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
				}
				else
				{
					SourceError(source, ": without ? in #if/#elif");
					error = 1;
				}
				break;
			case 0x2B:
				if (gotquestmarkvalue)
				{
					SourceError(source, "? after ? in #if/#elif");
					error = 1;
				}
				else
				{
					questmarkintvalue = v1->intvalue;
					questmarkfloatvalue = v1->floatvalue;
					gotquestmarkvalue = 1;
				}
				break;
			default:
				break;
			}
			if (error)
				break;
			lastoperatortype = oa->op;
			if (oa->op != 36 && oa->op != 35)
			{
				if (oa->op != 43)
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
			if (oa->prev)
				oa->prev->next = oa->next;
			else
				firstoperator = oa->next;
			if (oa->next)
				oa->next->prev = oa->prev;
		}
		if (firstvalue)
		{
			if (intvalue)
				*intvalue = firstvalue->intvalue;
			if (floatvalue)
				*floatvalue = firstvalue->floatvalue;
		}
		for (ob = firstoperator; ob; ob = ob->next)
			;
		for (v = firstvalue; v; v = lastvalue)
			lastvalue = v->next;
		if (!error)
			return 1;
		if (intvalue)
			*intvalue = 0;
		if (floatvalue)
			*floatvalue = 0.0f;
		return 0;
	}

	int __cdecl PC_DollarEvaluate(source_s* source, int* intvalue, long double* floatvalue, int integer)
	{
		token_s* t; // [esp+14h] [ebp-454h]
		token_s* ta; // [esp+14h] [ebp-454h]
		token_s* tb; // [esp+14h] [ebp-454h]
		token_s* tc; // [esp+14h] [ebp-454h]
		define_s* define; // [esp+18h] [ebp-450h]
		int defined; // [esp+1Ch] [ebp-44Ch]
		token_s* nexttoken; // [esp+20h] [ebp-448h]
		int indent; // [esp+24h] [ebp-444h]
		token_s token; // [esp+28h] [ebp-440h] BYREF
		token_s* firsttoken; // [esp+460h] [ebp-8h]
		token_s* lasttoken; // [esp+464h] [ebp-4h]

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
			if (token.type == 4)
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
			if (token.type != 3 && token.type != 5)
			{
				SourceError(source, "can't evaluate %s", token.string);
				return 0;
			}
			if (token.string[0] == 40)
			{
				++indent;
			}
			else if (token.string[0] == 41)
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
		token.floatvalue = value;
		PC_UnreadSourceToken(source, &token);
		if (value < 0) UnreadSignToken(source);
		return 1;
	}

	int PC_DollarDirective_evalfloat(source_s* source)
	{
		long double value;
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
		token.intvalue = (unsigned long)value;
		token.floatvalue = value;
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

	int __cdecl PC_ReadDollarDirective(source_s* source)
	{
		int result;
		token_s token;
		int i;

		if (PC_ReadSourceToken(source, &token))
		{
			if (token.linescrossed <= 0)
			{
				if (token.type == 4)
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

	int __cdecl PC_ReadLine(source_s* source, token_s* token, bool expandDefines)
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
				if (token->type != 4)
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

	int __cdecl PC_ReadLineHandle(/*int handle,*/ pc_token_s* pc_token)
	{
		char v4; // [esp+3h] [ebp-449h]
		char* v5; // [esp+8h] [ebp-444h]
		token_s* v6; // [esp+Ch] [ebp-440h]
		int ret; // [esp+10h] [ebp-43Ch]
		token_s token; // [esp+14h] [ebp-438h] BYREF

		//if (handle < 1 || handle >= 64)
		//	return 0;
		//if (!sourceFiles[handle])
		//	return 0;
		if (!sourceFile)
			return 0;
		ret = PC_ReadLine(sourceFile, &token, 1); //PC_ReadLine(sourceFiles[handle], &token, 1);
		v6 = &token;
		v5 = pc_token->string;
		do
		{
			v4 = v6->string[0];
			*v5 = v6->string[0];
			v6 = (token_s*)((char*)v6 + 1);
			++v5;
		} while (v4);
		pc_token->type = token.type;
		pc_token->subtype = token.subtype;
		pc_token->intvalue = token.intvalue;
		pc_token->floatvalue = token.floatvalue;
		if (pc_token->type == 1)
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

	int __cdecl Com_Compress(char* data_p)
	{
		char c; // [esp+7h] [ebp-Dh]
		char* datai; // [esp+8h] [ebp-Ch]
		int size; // [esp+Ch] [ebp-8h]
		char* datao; // [esp+10h] [ebp-4h]

		size = 0;
		datao = data_p;
		datai = data_p;
		if (data_p)
		{
			while (1)
			{
				c = *datai;
				if (!*datai)
					break;
				if (c != 13 && c != 10)
				{
					if (c != 47 || datai[1] != 47)
					{
						if (c != 47 || datai[1] != 42)
						{
							*datao++ = c;
							++size;
							++datai;
						}
						else
						{
							while (*datai && (*datai != 42 || datai[1] != 47))
							{
								if (*datai == 10)
								{
									*datao++ = 10;
									++size;
								}
								++datai;
							}
							if (*datai)
								datai += 2;
						}
					}
					else
					{
						while (*datai && *datai != 10)
							++datai;
					}
				}
				else
				{
					*datao++ = c;
					++size;
					++datai;
				}
			}
		}
		*datao = 0;
		return size;
	}

	void __cdecl PS_CreatePunctuationTable(script_s* script, punctuation_s* punctuations)
	{
		punctuation_s* lastp;
		int i;
		punctuation_s* newp;
		punctuation_s* p;

		if (!script->punctuationtable)
			script->punctuationtable = mmem->allocate<punctuation_s*>(256);
		//memset((unsigned __int8*)script->punctuationtable, 0, 0x400u);
		for (i = 0; punctuations[i].p; ++i)
		{
			newp = &punctuations[i];
			lastp = 0;
			for (p = script->punctuationtable[*punctuations[i].p]; p; p = p->next)
			{
				newp->p;
				p->p;
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
		int length;

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

	script_s* __cdecl LoadScriptMemory(const char* ptr, int length, const char* name)
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
		memcpy((unsigned __int8*)script->buffer, (unsigned __int8*)ptr, length);
		return script;
	}

	int __cdecl PC_Evaluate(source_s* source, int* intvalue, long double* floatvalue, int integer)
	{
		int result; // eax
		token_s* t; // [esp+14h] [ebp-450h]
		token_s* ta; // [esp+14h] [ebp-450h]
		token_s* tb; // [esp+14h] [ebp-450h]
		token_s* tc; // [esp+14h] [ebp-450h]
		define_s* define; // [esp+18h] [ebp-44Ch]
		token_s* nexttoken; // [esp+1Ch] [ebp-448h]
		int defined; // [esp+20h] [ebp-444h]
		token_s token; // [esp+24h] [ebp-440h] BYREF
		token_s* firsttoken; // [esp+45Ch] [ebp-8h]
		token_s* lasttoken; // [esp+460h] [ebp-4h]

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
				if (token.type == 4)
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
					if (token.type != 3 && token.type != 5)
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

	int __cdecl PC_Directive_if(source_s* source)
	{
		int value; // [esp+4h] [ebp-4h] BYREF

		if (!PC_Evaluate(source, &value, 0, 1))
			return 0;
		PC_PushIndent(source, 1, (parseSkip_t)(value == 0));
		return 1;
	}

	int __cdecl PC_Directive_if_def(source_s* source, int type)
	{
		int result; // eax
		define_s* d; // [esp+0h] [ebp-440h]
		token_s token; // [esp+8h] [ebp-438h] BYREF

		if (PC_ReadLine(source, &token, 0))
		{
			if (token.type == 4)
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

	int __cdecl PC_Directive_ifdef(source_s* source)
	{
		return PC_Directive_if_def(source, 8);
	}

	int __cdecl PC_Directive_ifndef(source_s* source)
	{
		return PC_Directive_if_def(source, 16);
	}

	int __cdecl PC_Directive_elif(source_s* source)
	{
		int result; // eax
		parseSkip_t skip; // [esp+0h] [ebp-Ch] BYREF
		int type; // [esp+4h] [ebp-8h] BYREF
		int value; // [esp+8h] [ebp-4h] BYREF

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

	int __cdecl PC_Directive_else(source_s* source)
	{
		int result; // eax
		parseSkip_t skip; // [esp+0h] [ebp-8h] BYREF
		int type; // [esp+4h] [ebp-4h] BYREF

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

	int __cdecl PC_Directive_endif(source_s* source)
	{
		parseSkip_t skip; // [esp+0h] [ebp-8h] BYREF
		int type; // [esp+4h] [ebp-4h] BYREF

		PC_PopIndent(source, &type, &skip);
		if (type)
			return 1;
		SourceError(source, "misplaced #endif");
		return 0;
	}

	int __cdecl PC_Directive_undef(source_s* source)
	{
		int result; // eax
		define_s* lastdefine; // [esp+14h] [ebp-444h]
		int hash; // [esp+18h] [ebp-440h]
		define_s* define; // [esp+1Ch] [ebp-43Ch]
		token_s token; // [esp+20h] [ebp-438h] BYREF

		if (source->skip > 0)
			return 1;
		if (PC_ReadLine(source, &token, 0))
		{
			if (token.type == 4)
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
		script_s* script;
		token_s token;
		char path[64];

		if (source->skip > 0)
			return 1;
		if (!PC_ReadSourceToken(source, &token))
		{
			SourceError(source, "#include without file name");
			return 0;
		}
		if (token.linescrossed > 0)
		{
			SourceError(source, "#include without file name");
			return 0;
		}
		if (token.type == TT_STRING)
		{
			StripDoubleQuotes(token.string);
			PC_ConvertPath(token.string);
			script = LoadScriptFile(token.string);
			if (!script)
			{
				strcpy(path, source->includepath);
				strcat(path, token.string);
				script = LoadScriptFile(path);
			}
		}
		else if (token.type == TT_PUNCTUATION && *token.string == '<')
		{
			strcpy(path, source->includepath);
			while (PC_ReadSourceToken(source, &token))
			{
				if (token.linescrossed > 0)
				{
					PC_UnreadSourceToken(source, &token);
					break;
				}
				if (token.type == TT_PUNCTUATION && *token.string == '>') break;
				strncat(path, token.string, 64);
			}
			if (*token.string != '>')
			{
				SourceWarning(source, "#include missing trailing >");
			}
			if (!strlen(path))
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

	int __cdecl PC_Directive_define(source_s* source)
	{
		char v2; // [esp+7Bh] [ebp-45Dh]
		char* v3; // [esp+80h] [ebp-458h]
		token_s* v4; // [esp+84h] [ebp-454h]
		token_s* t; // [esp+98h] [ebp-440h]
		token_s* ta; // [esp+98h] [ebp-440h]
		define_s* define; // [esp+9Ch] [ebp-43Ch]
		define_s* definea; // [esp+9Ch] [ebp-43Ch]
		token_s token; // [esp+A0h] [ebp-438h] BYREF
		token_s* last; // [esp+4D4h] [ebp-4h]

		if (source->skip > 0)
			return 1;
		if (!PC_ReadLine(source, &token, 0))
		{
			SourceError(source, "#define without name");
			return 0;
		}
		if (token.type != 4)
		{
			PC_UnreadSourceToken(source, &token);
			SourceError(source, "expected name after #define, found %s", token.string);
			return 0;
		}
		define = PC_FindHashedDefine(source->definehash, token.string);
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
		definea = mmem->manual_allocate<define_s>(strlen(token.string) + 1 + sizeof(define_s)); //(define_s*)GetMemory(&token.string[strlen(token.string) + 1] - &token.string[1] + 33);
		definea->name = 0;
		definea->flags = 0;
		definea->builtin = 0;
		definea->numparms = 0;
		definea->parms = 0;
		definea->tokens = 0;
		definea->next = 0;
		definea->hashnext = 0;
		definea->name = (char*)&definea[1];
		v4 = &token;
		v3 = definea->name;
		do
		{
			v2 = v4->string[0];
			*v3 = v4->string[0];
			v4 = (token_s*)((char*)v4 + 1);
			++v3;
		} while (v2);
		PC_AddDefineToHash(definea, source->definehash);
		if (!PC_ReadLine(source, &token, 0))
			return 1;
		if (PC_WhiteSpaceBeforeToken(&token) || strcmp(token.string, "("))
		{
		LABEL_37:
			last = 0;
			do
			{
				ta = PC_CopyToken(&token);
				if (ta->type == 4 && !strcmp(ta->string, definea->name))
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
						definea->tokens = ta;
					last = ta;
				}
			} while (PC_ReadLine(source, &token, 0));
			if (!last || strcmp(definea->tokens->string, "##") && strcmp(last->string, "##"))
				return 1;
			SourceError(source, "define with misplaced ##");
			return 0;
		}
		last = 0;
		if (PC_CheckTokenString(source, ")"))
		{
		LABEL_35:
			if (!PC_ReadLine(source, &token, 0))
				return 1;
			goto LABEL_37;
		}
		do
		{
			if (!PC_ReadLine(source, &token, 0))
			{
				SourceError(source, "expected define parameter");
				return 0;
			}
			if (token.type != 4)
			{
				SourceError(source, "invalid define parameter");
				return 0;
			}
			if (PC_FindDefineParm(definea, token.string) >= 0)
			{
				SourceError(source, "two of the same define parameters");
				return 0;
			}
			t = PC_CopyToken(&token);
			PC_ClearTokenWhiteSpace(t);
			t->next = 0;
			if (last)
				last->next = t;
			else
				definea->parms = t;
			last = t;
			++definea->numparms;
			if (!PC_ReadLine(source, &token, 0))
			{
				SourceError(source, "define parameters not terminated");
				return 0;
			}
			if (!strcmp(token.string, ")"))
				goto LABEL_35;
		} while (!strcmp(token.string, ","));
		SourceError(source, "define not terminated");
		return 0;
	}

	int __cdecl PC_Directive_line(source_s* source)
	{
		SourceError(source, "#line directive not supported");
		return 0;
	}

	int __cdecl PC_Directive_error(source_s* source)
	{
		token_s token; // [esp+0h] [ebp-438h] BYREF

		token.string[0] = 0;
		PC_ReadSourceToken(source, &token);
		if (source->skip)
			return 1;
		SourceError(source, "#error directive: %s", token.string);
		return 0;
	}

	int __cdecl PC_Directive_pragma(source_s* source)
	{
		token_s token; // [esp+0h] [ebp-438h] BYREF

		SourceWarning(source, "#pragma directive not supported");
		while (PC_ReadLine(source, &token, 0))
			;
		return 1;
	}

	int __cdecl PC_Directive_eval(source_s* source)
	{
		token_s token; // [esp+0h] [ebp-438h] BYREF
		int value; // [esp+434h] [ebp-4h] BYREF

		if (!PC_Evaluate(source, &value, 0, 1))
			return 0;
		token.line = source->scriptstack->line;
		token.whitespace_p = source->scriptstack->script_p;
		token.endwhitespace_p = source->scriptstack->script_p;
		token.linescrossed = 0;
		sprintf(token.string, "%d", abs(value));
		token.type = 3;
		token.subtype = 12296;
		PC_UnreadSourceToken(source, &token);
		if (value < 0)
			UnreadSignToken(source);
		return 1;
	}

	int __cdecl PC_Directive_evalfloat(source_s* source)
	{
		float v2; // xmm0_4
		token_s token; // [esp+Ch] [ebp-440h] BYREF
		long double value; // [esp+444h] [ebp-8h] BYREF

		if (!PC_Evaluate(source, 0, &value, 0))
			return 0;
		token.line = source->scriptstack->line;
		token.whitespace_p = source->scriptstack->script_p;
		token.endwhitespace_p = source->scriptstack->script_p;
		token.linescrossed = 0;
		v2 = value;
		sprintf(token.string, "%1.2f", fabs(v2));
		token.type = 3;
		token.subtype = 10248;
		PC_UnreadSourceToken(source, &token);
		if (value < 0.0)
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

	define_s* __cdecl PC_DefineFromString(const char* string)
	{
		source_s src; // [esp+10h] [ebp-4E8h] BYREF
		token_s* t; // [esp+4E4h] [ebp-14h]
		define_s* def; // [esp+4E8h] [ebp-10h]
		script_s* script; // [esp+4ECh] [ebp-Ch]
		int i; // [esp+4F0h] [ebp-8h]
		int res; // [esp+4F4h] [ebp-4h]

		script = LoadScriptMemory(string, strlen(string), "*extern");
		memset((unsigned __int8*)&src, 0, sizeof(src));
		strncpy(src.filename, "*extern", 0x40u);
		src.scriptstack = script;
		src.definehash = mmem->allocate<define_s*>(1024);//(define_s**)GetClearedMemory(0x1000u);
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

	int __cdecl PC_AddDefine(source_s* source, const char* string)
	{
		define_s* define; // [esp+0h] [ebp-4h]

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
		//int i;

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

	int __cdecl PC_ReadDirective(source_s* source)
	{
		int result;
		token_s token;
		int i;

		if (PC_ReadSourceToken(source, &token))
		{
			if (token.linescrossed <= 0)
			{
				if (token.type == 4)
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

	int __cdecl PC_ReadToken(source_s* source, token_s* token)
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
						if (token->type != 5 || token->string[0] != '#' || token->string[1])
							break;
						if (!PC_ReadDirective(source))
							return 0;
					}
					if (token->type != 5 || token->string[0] != '$')
						break;
					if (!PC_ReadDollarDirective(source))
						return 0;
				}
			} while (source->skip);
			if (token->type == 1 && PC_ReadToken(source, &newtoken))
			{
				if (newtoken.type == 1)
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

	int __cdecl PC_CheckTokenString(source_s* source, const char* string)
	{
		token_s tok; // [esp+14h] [ebp-438h] BYREF

		if (!PC_ReadToken(source, &tok))
			return 0;
		if (!strcmp(tok.string, string))
			return 1;
		PC_UnreadSourceToken(source, &tok);
		return 0;
	}

	int __cdecl PC_ReadTokenHandle(/*int handle,*/ pc_token_s* pc_token)
	{
		token_s token;
		int ret;

		//if (handle < 1 || handle >= 64)
		//	return 0;
		//if (!sourceFiles[handle])
		//	return 0;
		if (!sourceFile)
			return 0;
		ret = PC_ReadToken(sourceFile, &token); // PC_ReadToken(sourceFiles[handle], &token);
		memcpy(pc_token->string, token.string, 0x400);
		pc_token->type = token.type;
		pc_token->subtype = token.subtype;
		pc_token->intvalue = token.intvalue;
		pc_token->floatvalue = token.floatvalue;
		if (pc_token->type == 1)
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

	bool __cdecl Expression_OpIsFunction(operationEnum op)
	{
		return op >= OP_FIRSTFUNCTIONCALL;
	}

	std::string __cdecl GetOperandAsString(Operand operand)
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

	bool Expression_Parse(/*int handle,*/ Statement_s* statement, int maxTokens)
	{
		pc_token_s token;
		int numOpenLeftParens;
		int tokenType;
		int lastType;
		operationEnum op;
		operationEnum lastOp;
		Operand lastOperand;
		expressionEntry* newExpression;

		lastOp = OP_NOOP;
		lastType = 2;

		numOpenLeftParens = 0;
		while (PC_ReadTokenHandle(/*handle,*/ &token))
		{
			if (statement->numEntries == maxTokens)
			{
				PC_SourceError(/*handle,*/
					"Need to increment MAX_TOKENS_PER_STATEMENT - this statement has more than %i tokens", maxTokens);
				return 0;
			}
			newExpression = &statement->entries[statement->numEntries];

			tokenType = token.type;
			if (token.string[0] == ';')
				break;
			if (tokenType == TT_STRING)
			{
				op = OP_NOOP;
			}
			else
			{
				op = Expression_GetOp(token.string);
			}
			if (op == OP_NOOP)
			{
				switch (tokenType)
				{
				case TT_NUMBER:
					if (token.floatvalue == (float)(token.intvalue))
					{
						newExpression->type = 1;
						newExpression->data.operand.dataType = VAL_INT;
						newExpression->data.operand.internals.intVal = token.intvalue;
					}
					else
					{
						newExpression->type = 1;
						newExpression->data.operand.dataType = VAL_FLOAT;
						newExpression->data.operand.internals.floatVal = token.floatvalue;
					}
					goto LABEL_39;
				case TT_STRING:
				case TT_NAME:
					newExpression->type = 1;
					newExpression->data.operand.dataType = VAL_STRING;
					newExpression->data.operand.internals.stringVal.string = zmem->duplicate_string(token.string);

				LABEL_39:
					lastType = 1;
					lastOperand = newExpression->data.operand;
					goto LABEL_40;

				default:
					PC_SourceError(
						/*handle,*/
						"Expression Error: Unknown token '%s'",
						token.string);
				}
			}
			if (op == OP_LEFTPAREN)
			{
				++numOpenLeftParens;
				if (lastType == 2)
					goto LABEL_31;
				if (lastType)
				{
					PC_SourceError(/*handle,*/ "Expression Error: %s(...", GetOperandAsString(lastOperand).data());
					return 0;
				}
				if (!Expression_OpIsFunction(lastOp))
					goto LABEL_31;
				lastOp = OP_LEFTPAREN;
			}
			else
			{
				if (op == OP_RIGHTPAREN)
				{
					if (--numOpenLeftParens < 0)
					{
						PC_SourceError(
							/*handle,*/
							"Expression Error: Found a right parenthesis that doesn't match any left parenthesis");
						return 0;
					}
					if (!numOpenLeftParens)
						break;
				}
			LABEL_31:
				newExpression->type = 0;
				newExpression->data.op = op;
				lastType = 0;
				lastOp = op;
			LABEL_40:
				if (newExpression)
				{
					++statement->numEntries;
				}
			}
		}

		return 1;
	}

	int Expression_Read(/*int handle,*/ Statement_s** statement)
	{
		int maxTokens = MAX_TOKENS_PER_STATEMENT;

		*statement = zmem->allocate<Statement_s>();
		(*statement)->entries = zmem->allocate<expressionEntry>(maxTokens);

		if (!Expression_Parse(/*handle,*/ *statement, maxTokens))
			return 0;
		return 1;
	}

	int PC_String_Parse(/*int handle,*/ const char** out)
	{
		pc_token_s token;

		if (!PC_ReadTokenHandle(/*handle,*/ &token))
			return 0;
		*out = zmem->duplicate_string(token.string);
		return 1;
	}

	enum EvalValueType : __int32
	{
		EVAL_VALUE_DOUBLE = 0x0,
		EVAL_VALUE_INT = 0x1,
		EVAL_VALUE_STRING = 0x2,
	};

	struct EvalValue
	{
		EvalValueType type;
		union
		{
			long double d;
			int i;
			char* s;
		} u;
	};

	enum EvalOperatorType : __int32
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

	bool __cdecl Eval_CanPushValue(const Eval* eval)
	{
		const char* pExceptionObject;

		if (!eval->valStackPos)
			return 1;
		if (eval->valStackPos == 1024)
		{
			pExceptionObject = "evaluation stack overflow - expression is too complex";
			throw std::exception(pExceptionObject);
		}
		return eval->pushedOp;
	}

	char __cdecl Eval_PushNumber(Eval* eval, long double value)
	{
		if (!Eval_CanPushValue(eval))
			return 0;
		eval->valStack[eval->valStackPos].type = EVAL_VALUE_DOUBLE;
		eval->valStack[eval->valStackPos++].u.d = value;
		eval->pushedOp = 0;
		return 1;
	}

	char __cdecl Eval_PushInteger(Eval* eval, int value)
	{
		if (!Eval_CanPushValue(eval))
			return 0;
		eval->valStack[eval->valStackPos].type = EVAL_VALUE_INT;
		eval->valStack[eval->valStackPos++].u.i = value;
		eval->pushedOp = 0;
		return 1;
	}

	char __cdecl Eval_OperatorForToken(const char* text, EvalOperatorType* op)
	{
		char result; // al

		if (!text || !op)
			__debugbreak();
		switch (*text)
		{
		case '!':
			if (text[1] == '=')
				*op = EVAL_OP_NOT_EQUAL;
			else
				*op = EVAL_OP_LOGICAL_NOT;
			result = 1;
			break;
		case '%':
			*op = EVAL_OP_MODULUS;
			result = 1;
			break;
		case '&':
			if (text[1] == '&')
				*op = EVAL_OP_LOGICAL_AND;
			else
				*op = EVAL_OP_BITWISE_AND;
			result = 1;
			break;
		case '(':
			*op = EVAL_OP_LPAREN;
			result = 1;
			break;
		case ')':
			*op = EVAL_OP_RPAREN;
			result = 1;
			break;
		case '*':
			*op = EVAL_OP_MULTIPLY;
			result = 1;
			break;
		case '+':
			*op = EVAL_OP_PLUS;
			result = 1;
			break;
		case '-':
			*op = EVAL_OP_MINUS;
			result = 1;
			break;
		case '/':
			*op = EVAL_OP_DIVIDE;
			result = 1;
			break;
		case ':':
			*op = EVAL_OP_COLON;
			result = 1;
			break;
		case '<':
			if (text[1] == '<')
			{
				*op = EVAL_OP_LSHIFT;
				result = 1;
			}
			else
			{
				if (text[1] == '=')
					*op = EVAL_OP_LESS_EQUAL;
				else
					*op = EVAL_OP_LESS;
				result = 1;
			}
			break;
		case '=':
			if (text[1] != '=')
				goto LABEL_46;
			*op = EVAL_OP_EQUALS;
			result = 1;
			break;
		case '>':
			if (text[1] == '>')
			{
				*op = EVAL_OP_RSHIFT;
				result = 1;
			}
			else
			{
				if (text[1] == '=')
					*op = EVAL_OP_GREATER_EQUAL;
				else
					*op = EVAL_OP_GREATER;
				result = 1;
			}
			break;
		case '?':
			*op = EVAL_OP_QUESTION;
			result = 1;
			break;
		case '^':
			*op = EVAL_OP_BITWISE_XOR;
			result = 1;
			break;
		case '|':
			if (text[1] == '|')
				*op = EVAL_OP_LOGICAL_OR;
			else
				*op = EVAL_OP_BITWISE_OR;
			result = 1;
			break;
		case '~':
			*op = EVAL_OP_BITWISE_NOT;
			result = 1;
			break;
		default:
		LABEL_46:
			result = 0;
			break;
		}
		return result;
	}

	bool __cdecl Eval_IsUnaryOp(const Eval* eval)
	{
		bool result; // al

		if (eval->valStackPos)
			result = eval->pushedOp;
		else
			result = 1;
		return result;
	}

	void __cdecl Eval_PrepareBinaryOpSameTypes(Eval* eval)
	{
		int pos; // eax
		const char* pExceptionObject; // [esp+8h] [ebp-4h] BYREF

		if (eval->valStackPos < 2)
		{
			pExceptionObject = "missing operand (for example, 'a + ' or ' / b')";
			throw std::exception(pExceptionObject);
		}
		if (eval->opStack[4 * eval->valStackPos + 1016] == EVAL_OP_COLON
			|| eval->opStack[4 * eval->valStackPos + 1020] == EVAL_OP_COLON)
		{
			pExceptionObject = "operation not valid on strings";
			throw std::exception(pExceptionObject);
		}
		if (eval->opStack[4 * eval->valStackPos + 1016] != eval->opStack[4 * eval->valStackPos + 1020])
		{
			if (eval->opStack[4 * eval->valStackPos + 1016] == EVAL_OP_RPAREN)
			{
				*(double*)&eval->opStack[4 * eval->valStackPos + 1018] = (double)(int)eval->opStack[4 * eval->valStackPos + 1018];
				pos = eval->valStackPos - 2;
			}
			else
			{
				*(double*)&eval->opStack[4 * eval->valStackPos + 1022] = (double)(int)eval->opStack[4 * eval->valStackPos + 1022];
				pos = eval->valStackPos - 1;
			}
			eval->valStack[pos].type = EVAL_VALUE_DOUBLE;
		}
	}

	void __cdecl Eval_PrepareBinaryOpBoolean(Eval* eval)
	{
		const char* pExceptionObject; // [esp+Ch] [ebp-4h] BYREF

		if (eval->valStackPos < 2)
		{
			pExceptionObject = "missing operand (for example, 'a + ' or ' / b')";
			throw std::exception(pExceptionObject);
		}
		if (eval->valStack[eval->valStackPos - 2].type == 2 || eval->valStack[eval->valStackPos - 1].type == 2)
		{
			pExceptionObject = "operation not valid on strings";
			throw std::exception(pExceptionObject);
		}
		if (eval->valStack[eval->valStackPos - 2].type)
		{
			eval->valStack[eval->valStackPos - 2].u.i = eval->valStack[eval->valStackPos - 2].u.i != 0;
		}
		else
		{
			eval->valStack[eval->valStackPos - 2].u.i = eval->valStack[eval->valStackPos - 2].u.d != 0.0;
			eval->valStack[eval->valStackPos - 2].type = EVAL_VALUE_INT;
		}
		if (eval->valStack[eval->valStackPos - 1].type)
		{
			eval->valStack[eval->valStackPos - 1].u.i = eval->valStack[eval->valStackPos - 1].u.i != 0;
		}
		else
		{
			eval->valStack[eval->valStackPos - 1].u.i = eval->valStack[eval->valStackPos - 1].u.d != 0.0;
			eval->valStack[eval->valStackPos - 1].type = EVAL_VALUE_INT;
		}
	}

	void __cdecl Eval_PrepareBinaryOpIntegers(Eval* eval)
	{
		const char* pExceptionObject; // [esp+4h] [ebp-4h]

		if (eval->valStackPos < 2)
		{
			pExceptionObject = "missing operand (for example, 'a + ' or ' / b')";
			throw std::exception(pExceptionObject);
		}
		if (eval->valStack[eval->valStackPos - 2].type == 2 || eval->valStack[eval->valStackPos - 1].type == 2)
		{
			pExceptionObject = "operation not valid on strings";
			throw std::exception(pExceptionObject);
		}
		if (eval->valStack[eval->valStackPos - 2].type == EVAL_VALUE_DOUBLE)
		{
			eval->valStack[eval->valStackPos - 2].u.i = (signed int)eval->valStack[eval->valStackPos - 2].u.d;
			eval->valStack[eval->valStackPos - 2].type = EVAL_VALUE_INT;
		}
		if (eval->valStack[eval->valStackPos - 1].type == EVAL_VALUE_DOUBLE)
		{
			eval->valStack[eval->valStackPos - 1].u.i = (signed int)eval->valStack[eval->valStackPos - 1].u.d;
			eval->valStack[eval->valStackPos - 1].type = EVAL_VALUE_INT;
		}
	}

	char Eval_EvaluationStep(Eval* eval)
	{
		const char* pExceptionObject;
		int v4; // esi
		unsigned __int64 v5; // xmm1
		int v6; // esi
		long double v7; // xmm1_8
		EvalValue* v8; // edx
		EvalValue* v9; // ecx
		long double v10; // [esp+8h] [ebp-BCh]
		long double v11; // [esp+10h] [ebp-B4h]
		int v13; // [esp+20h] [ebp-A4h]
		int v14; // [esp+24h] [ebp-A0h]
		int i; // [esp+2Ch] [ebp-98h]
		int v17; // [esp+30h] [ebp-94h]
		int v18; // [esp+34h] [ebp-90h]
		int v19; // [esp+38h] [ebp-8Ch]
		int v20; // [esp+3Ch] [ebp-88h]
		bool v21; // [esp+43h] [ebp-81h]
		int v22; // [esp+44h] [ebp-80h]
		bool v23; // [esp+4Bh] [ebp-79h]
		double v25; // [esp+50h] [ebp-74h] OVERLAPPED
		unsigned __int8* v28; // [esp+68h] [ebp-5Ch]
		unsigned int v29; // [esp+6Ch] [ebp-58h]
		char* s; // [esp+74h] [ebp-50h]
		unsigned int v31; // [esp+78h] [ebp-4Ch]
		int v32; // [esp+7Ch] [ebp-48h]
		unsigned int v33; // [esp+80h] [ebp-44h]
		unsigned int v34; // [esp+84h] [ebp-40h]
		unsigned int v35; // [esp+88h] [ebp-3Ch]
		int length[2]; // [esp+8Ch] [ebp-38h]
		unsigned int v37; // [esp+94h] [ebp-30h]
		const char* v38; // [esp+98h] [ebp-2Ch]
		const char* v39; // [esp+9Ch] [ebp-28h]
		const char* v40; // [esp+A0h] [ebp-24h]
		int v41; // [esp+A4h] [ebp-20h]
		const char* v42; // [esp+A8h] [ebp-1Ch]
		int v43; // [esp+ACh] [ebp-18h]
		const char* v44; // [esp+B0h] [ebp-14h]
		const char* v45; // [esp+B4h] [ebp-10h]

		if (!eval->opStackPos)
			return 0;
		if (eval->opStack[--eval->opStackPos] == EVAL_OP_LPAREN)
			return 1;
		if (eval->opStack[eval->opStackPos] == 3)
		{
			pExceptionObject = "found '?' with no following ':' in expression of type 'a ? b : c'";
			throw std::exception(pExceptionObject);
		}
		if (!eval->valStackPos)
		{
			pExceptionObject = "missing operand (for example, 'a + ' or ' / b')";
			throw std::exception(pExceptionObject);
		}
		v43 = eval->opStack[eval->opStackPos] - 2;
		switch (v43)
		{
		case 0:
			if (eval->valStackPos < 3)
			{
				pExceptionObject = "missing operand (for example, 'a + ' or ' / b')";
				throw std::exception(pExceptionObject);
			}
			if (eval->valStack[eval->valStackPos - 3].type)
			{
				if (eval->valStack[eval->valStackPos - 3].type != 1)
				{
					pExceptionObject = "can only switch on numbers";
					throw std::exception(pExceptionObject);
				}
				v13 = -(eval->valStack[eval->valStackPos - 3].u.i != 0) - 1;
			}
			else
			{
				if (eval->valStack[eval->valStackPos - 3].u.d == 0.0)
					v14 = -1;
				else
					v14 = -2;
				v13 = v14;
			}
			if (eval->valStack[eval->valStackPos - 2].type != 2 || eval->valStack[eval->valStackPos - 1].type != 2)
				Eval_PrepareBinaryOpSameTypes(eval);
			else
				free(eval->valStack[1 - v13 + eval->valStackPos].u.s);
			v8 = &eval->valStack[v13 + eval->valStackPos];
			v9 = &eval->valStack[eval->valStackPos - 3];
			v9->type = v8->type;
			*(&v9->type + 1) = *(&v8->type + 1);
			v9->u.i = v8->u.i;
			*(&v9->u.s + 1) = *(&v8->u.s + 1);
			eval->valStackPos -= 2;
			--eval->opStackPos;
			return 1;
		case 1:
			__debugbreak();
			return 0;
		case 2:
			if (eval->valStackPos < 2
				|| eval->valStack[eval->valStackPos - 2].type != 2
				|| eval->valStack[eval->valStackPos - 1].type != 2)
			{
				Eval_PrepareBinaryOpSameTypes(eval);
				if (eval->valStack[eval->valStackPos - 2].type)
					eval->valStack[eval->valStackPos - 2].u.i += eval->valStack[eval->valStackPos - 1].u.i;
				else
					eval->valStack[eval->valStackPos - 2].u.d = eval->valStack[eval->valStackPos - 2].u.d
					+ eval->valStack[eval->valStackPos - 1].u.d;
			}
			else
			{
				v38 = eval->valStack[eval->valStackPos - 2].u.s;
				length[1] = (int)(v38 + 1);
				v37 = (unsigned int)&v38[strlen(v38) + 1];
				v35 = v37 - (char)(v38 + 1);
				v33 = v37 - (char)(v38 + 1);
				v32 = eval->valStack[eval->valStackPos - 1].u.i;
				s = (char*)(v32 + 1);
				v31 = v32 + strlen((const char*)v32) + 1;
				v29 = v31 - (v32 + 1);
				v34 = v31 - (v32 + 1);
				v28 = (unsigned __int8*)malloc(v33 + v31 - v32);
				memcpy(v28, (unsigned __int8*)eval->valStack[eval->valStackPos - 2].u.i, v33);
				memcpy(&v28[v33], (unsigned __int8*)eval->valStack[eval->valStackPos - 1].u.i, v31 - v32);
				free(eval->valStack[eval->valStackPos - 2].u.s);
				free(eval->valStack[eval->valStackPos - 1].u.s);
				eval->valStack[eval->valStackPos - 2].u.i = (int)v28;
			}
			--eval->valStackPos;
			break;
		case 3:
			Eval_PrepareBinaryOpSameTypes(eval);
			if (eval->valStack[eval->valStackPos - 2].type)
				eval->valStack[eval->valStackPos - 2].u.i -= eval->valStack[eval->valStackPos - 1].u.i;
			else
				eval->valStack[eval->valStackPos - 2].u.d = eval->valStack[eval->valStackPos - 2].u.d
				- eval->valStack[eval->valStackPos - 1].u.d;
			--eval->valStackPos;
			break;
		case 4:
			return 1;
		case 5:
			if (eval->valStack[eval->valStackPos - 1].type == 1)
			{
				eval->valStack[eval->valStackPos - 1].u.i = -eval->valStack[eval->valStackPos - 1].u.i;
			}
			else
			{
				if (eval->valStack[eval->valStackPos - 1].type)
				{
					pExceptionObject = "cannot negate strings";
					throw std::exception(pExceptionObject);
				}
				eval->valStack[eval->valStackPos - 1].u.d = -eval->valStack[eval->valStackPos - 1].u.d;
			}
			break;
		case 6:
			Eval_PrepareBinaryOpSameTypes(eval);
			if (eval->valStack[eval->valStackPos - 2].type)
				eval->valStack[eval->valStackPos - 2].u.i *= eval->valStack[eval->valStackPos - 1].u.i;
			else
				eval->valStack[eval->valStackPos - 2].u.d = eval->valStack[eval->valStackPos - 2].u.d
				* eval->valStack[eval->valStackPos - 1].u.d;
			--eval->valStackPos;
			break;
		case 7:
			Eval_PrepareBinaryOpSameTypes(eval);
			if (eval->valStack[eval->valStackPos - 2].type)
			{
				if (!eval->valStack[eval->valStackPos - 1].u.i)
				{
					pExceptionObject = "divide by zero";
					throw std::exception(pExceptionObject);
				}
				eval->valStack[eval->valStackPos - 2].u.i /= eval->valStack[eval->valStackPos - 1].u.i;
			}
			else
			{
				if (eval->valStack[eval->valStackPos - 1].u.d == 0.0)
				{
					pExceptionObject = "divide by zero";
					throw std::exception(pExceptionObject);
				}
				eval->valStack[eval->valStackPos - 2].u.d = eval->valStack[eval->valStackPos - 2].u.d
					/ eval->valStack[eval->valStackPos - 1].u.d;
			}
			--eval->valStackPos;
			break;
		case 8:
			Eval_PrepareBinaryOpSameTypes(eval);
			if (eval->valStack[eval->valStackPos - 2].type)
			{
				if (!eval->valStack[eval->valStackPos - 1].u.i)
				{
					pExceptionObject = "divide by zero";
					throw std::exception(pExceptionObject);
				}
				eval->valStack[eval->valStackPos - 2].u.i %= eval->valStack[eval->valStackPos - 1].u.i;
			}
			else
			{
				if (eval->valStack[eval->valStackPos - 1].u.d == 0.0)
				{
					pExceptionObject = "divide by zero";
					throw std::exception(pExceptionObject);
				}
				v25 = floor(eval->valStack[eval->valStackPos - 2].u.d / eval->valStack[eval->valStackPos - 1].u.d);
				eval->valStack[eval->valStackPos - 2].u.d = eval->valStack[eval->valStackPos - 2].u.d
					- eval->valStack[eval->valStackPos - 1].u.d * v25;
			}
			--eval->valStackPos;
			break;
		case 9:
			Eval_PrepareBinaryOpSameTypes(eval);
			if (eval->valStack[eval->valStackPos - 2].type)
			{
				eval->valStack[eval->valStackPos - 2].u.i <<= eval->valStack[eval->valStackPos - 1].u.i;
			}
			else
			{
				v4 = eval->valStackPos - 2;
				v5 = *(unsigned __int64*)&eval->valStack[eval->valStackPos - 1].u.d;
				pow(v10, v11);
				eval->valStack[eval->valStackPos - 2].u.d = 2.0 * eval->valStack[v4].u.d;
			}
			--eval->valStackPos;
			break;
		case 10:
			Eval_PrepareBinaryOpSameTypes(eval);
			if (eval->valStack[eval->valStackPos - 2].type)
			{
				eval->valStack[eval->valStackPos - 2].u.i >>= eval->valStack[eval->valStackPos - 1].u.i;
			}
			else
			{
				v6 = eval->valStackPos - 2;
				v7 = -eval->valStack[eval->valStackPos - 1].u.d;
				pow(v10, v11);
				eval->valStack[eval->valStackPos - 2].u.d = 2.0 * eval->valStack[v6].u.d;
			}
			--eval->valStackPos;
			break;
		case 11:
			if (eval->valStack[eval->valStackPos - 1].type)
			{
				if (eval->valStack[eval->valStackPos - 2].type == 2)
				{
					pExceptionObject = "cannot bitwise invert strings";
					throw std::exception(pExceptionObject);
				}
			}
			else
			{
				eval->valStack[eval->valStackPos - 1].u.i = (signed int)eval->valStack[eval->valStackPos - 1].u.d;
				eval->valStack[eval->valStackPos - 1].type = EVAL_VALUE_INT;
			}
			eval->valStack[eval->valStackPos - 1].u.i = ~eval->valStack[eval->valStackPos - 1].u.i;
			break;
		case 12:
			Eval_PrepareBinaryOpIntegers(eval);
			eval->valStack[eval->valStackPos - 2].u.i &= eval->valStack[eval->valStackPos - 1].u.i;
			--eval->valStackPos;
			break;
		case 13:
			Eval_PrepareBinaryOpIntegers(eval);
			eval->valStack[eval->valStackPos - 2].u.i |= eval->valStack[eval->valStackPos - 1].u.i;
			--eval->valStackPos;
			break;
		case 14:
			Eval_PrepareBinaryOpIntegers(eval);
			eval->valStack[eval->valStackPos - 2].u.i ^= eval->valStack[eval->valStackPos - 1].u.i;
			--eval->valStackPos;
			break;
		case 15:
			if (eval->valStack[eval->valStackPos - 1].type)
			{
				if (eval->valStack[eval->valStackPos - 1].type != 1)
				{
					pExceptionObject = "cannot logical invert strings";
					throw std::exception(pExceptionObject);;
				}
				eval->valStack[eval->valStackPos - 1].u.i = eval->valStack[eval->valStackPos - 1].u.i == 0;
			}
			else
			{
				v41 = eval->valStack[eval->valStackPos - 1].u.d == 0.0;
				eval->valStack[eval->valStackPos - 1].u.i = v41;
			}
			eval->valStack[eval->valStackPos - 1].type = EVAL_VALUE_INT;
			break;
		case 16:
			Eval_PrepareBinaryOpBoolean(eval);
			eval->valStack[eval->valStackPos - 2].u.i &= eval->valStack[eval->valStackPos - 1].u.i;
			--eval->valStackPos;
			break;
		case 17:
			Eval_PrepareBinaryOpBoolean(eval);
			eval->valStack[eval->valStackPos - 2].u.i |= eval->valStack[eval->valStackPos - 1].u.i;
			--eval->valStackPos;
			break;
		case 18:
			if (eval->valStackPos < 2
				|| eval->valStack[eval->valStackPos - 2].type != 2
				|| eval->valStack[eval->valStackPos - 1].type != 2)
			{
				Eval_PrepareBinaryOpSameTypes(eval);
				if (eval->valStack[eval->valStackPos - 2].type)
				{
					eval->valStack[eval->valStackPos - 2].u.i = eval->valStack[eval->valStackPos - 2].u.i == eval->valStack[eval->valStackPos - 1].u.i;
				}
				else
				{
					v22 = eval->valStack[eval->valStackPos - 2].u.d == eval->valStack[eval->valStackPos - 1].u.d;
					eval->valStack[eval->valStackPos - 2].u.i = v22;
					eval->valStack[eval->valStackPos - 2].type = EVAL_VALUE_INT;
				}
			}
			else
			{
				v23 = _stricmp(eval->valStack[eval->valStackPos - 2].u.s, eval->valStack[eval->valStackPos - 1].u.s) == 0;
				free(eval->valStack[eval->valStackPos - 2].u.s);
				free(eval->valStack[eval->valStackPos - 1].u.s);
				eval->valStack[eval->valStackPos - 2].type = EVAL_VALUE_INT;
				eval->valStack[eval->valStackPos - 2].u.i = v23;
			}
			--eval->valStackPos;
			break;
		case 19:
			if (eval->valStackPos < 2
				|| eval->valStack[eval->valStackPos - 2].type != 2
				|| eval->valStack[eval->valStackPos - 1].type != 2)
			{
				Eval_PrepareBinaryOpSameTypes(eval);
				if (eval->valStack[eval->valStackPos - 2].type)
				{
					eval->valStack[eval->valStackPos - 2].u.i = eval->valStack[eval->valStackPos - 2].u.i != eval->valStack[eval->valStackPos - 1].u.i;
				}
				else
				{
					v20 = eval->valStack[eval->valStackPos - 2].u.d != eval->valStack[eval->valStackPos - 1].u.d;
					eval->valStack[eval->valStackPos - 2].u.i = v20;
					eval->valStack[eval->valStackPos - 2].type = EVAL_VALUE_INT;
				}
			}
			else
			{
				v21 = _stricmp(eval->valStack[eval->valStackPos - 2].u.s, eval->valStack[eval->valStackPos - 1].u.s) == 0;
				free(eval->valStack[eval->valStackPos - 2].u.s);
				free(eval->valStack[eval->valStackPos - 1].u.s);
				eval->valStack[eval->valStackPos - 2].type = EVAL_VALUE_INT;
				eval->valStack[eval->valStackPos - 2].u.i = v21 == 0;
			}
			--eval->valStackPos;
			break;
		case 20:
			Eval_PrepareBinaryOpSameTypes(eval);
			if (eval->valStack[eval->valStackPos - 2].type)
			{
				eval->valStack[eval->valStackPos - 2].u.i = eval->valStack[eval->valStackPos - 2].u.i < eval->valStack[eval->valStackPos - 1].u.i;
			}
			else
			{
				v19 = eval->valStack[eval->valStackPos - 1].u.d > eval->valStack[eval->valStackPos - 2].u.d;
				eval->valStack[eval->valStackPos - 2].u.i = v19;
				eval->valStack[eval->valStackPos - 2].type = EVAL_VALUE_INT;
			}
			--eval->valStackPos;
			break;
		case 21:
			Eval_PrepareBinaryOpSameTypes(eval);
			if (eval->valStack[eval->valStackPos - 2].type)
			{
				eval->valStack[eval->valStackPos - 2].u.i = eval->valStack[eval->valStackPos - 2].u.i <= eval->valStack[eval->valStackPos - 1].u.i;
			}
			else
			{
				v18 = eval->valStack[eval->valStackPos - 1].u.d >= eval->valStack[eval->valStackPos - 2].u.d;
				eval->valStack[eval->valStackPos - 2].u.i = v18;
				eval->valStack[eval->valStackPos - 2].type = EVAL_VALUE_INT;
			}
			--eval->valStackPos;
			break;
		case 22:
			Eval_PrepareBinaryOpSameTypes(eval);
			if (eval->valStack[eval->valStackPos - 2].type)
			{
				eval->valStack[eval->valStackPos - 2].u.i = eval->valStack[eval->valStackPos - 2].u.i > eval->valStack[eval->valStackPos - 1].u.i;
			}
			else
			{
				v17 = eval->valStack[eval->valStackPos - 2].u.d > eval->valStack[eval->valStackPos - 1].u.d;
				eval->valStack[eval->valStackPos - 2].u.i = v17;
				eval->valStack[eval->valStackPos - 2].type = EVAL_VALUE_INT;
			}
			--eval->valStackPos;
			break;
		case 23:
			Eval_PrepareBinaryOpSameTypes(eval);
			if (eval->valStack[eval->valStackPos - 2].type)
			{
				eval->valStack[eval->valStackPos - 2].u.i = eval->valStack[eval->valStackPos - 2].u.i >= eval->valStack[eval->valStackPos - 1].u.i;
			}
			else
			{
				i = eval->valStack[eval->valStackPos - 2].u.d >= eval->valStack[eval->valStackPos - 1].u.d;
				eval->valStack[eval->valStackPos - 2].u.i = i;
				eval->valStack[eval->valStackPos - 2].type = EVAL_VALUE_INT;
			}
			--eval->valStackPos;
			break;
		}
		return 1;
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

	bool __cdecl Eval_PushOperator(Eval* eval, EvalOperatorType op)
	{
		bool result; // al
		bool v8; // [esp+0h] [ebp-10h]
		const char* pExceptionObject; // [esp+8h] [ebp-8h] BYREF
		bool leftToRight; // [esp+Dh] [ebp-3h]
		char precedence; // [esp+Eh] [ebp-2h]
		bool higherPrecedence; // [esp+Fh] [ebp-1h]

		if (s_precedence[op] < 0)
			return 0;
		if (op == EVAL_OP_RPAREN && !eval->parenCount)
			return 0;
		if (op == EVAL_OP_LPAREN)
		{
			if (eval->valStackPos && !eval->pushedOp)
				return 0;
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
		precedence = s_precedence[op];
		while (eval->opStackPos > 0)
		{
			higherPrecedence = s_precedence[eval->opStack[eval->opStackPos - 1]] > precedence;
			v8 = s_precedence[eval->opStack[eval->opStackPos - 1]] == precedence
				&& !s_rightToLeft[eval->opStack[eval->opStackPos - 1]];
			leftToRight = v8;
			if (!higherPrecedence && !leftToRight)
				break;
			if (eval->opStack[eval->opStackPos - 1] == EVAL_OP_LPAREN)
			{
				if (op == EVAL_OP_RPAREN)
				{
					--eval->parenCount;
					--eval->opStackPos;
					eval->pushedOp = 0;
					return 1;
				}
				break;
			}
			if (!Eval_EvaluationStep(eval))
				return 0;
		}
		if (op != EVAL_OP_COLON || eval->opStackPos && eval->opStack[eval->opStackPos - 1] == EVAL_OP_QUESTION)
		{
			if (eval->opStackPos == 1024)
			{
				pExceptionObject = "evaluation stack overflow - expression is too complex";
				throw std::exception(pExceptionObject);
			}
			eval->opStack[eval->opStackPos++] = op;
			eval->pushedOp = 1;
			result = 1;
		}
		else
		{
			if (eval->parenCount)
			{
				pExceptionObject = "found ':' without preceding '?' in expression of type 'a ? b : c'";
				throw std::exception(pExceptionObject);
			}
			result = 0;
		}
		return result;
	}

	bool __cdecl Eval_AnyMissingOperands(const Eval* eval)
	{
		int opIndex; // [esp+0h] [ebp-8h]
		int requiredOperandCount; // [esp+4h] [ebp-4h]

		requiredOperandCount = 1;
		for (opIndex = 0; opIndex < eval->opStackPos; ++opIndex)
			requiredOperandCount += s_consumedOperandCount[eval->opStack[opIndex]];
		return requiredOperandCount != eval->valStackPos;
	}

	EvalValue* __cdecl Eval_Solve(EvalValue* result, Eval* eval)
	{
		const char* pExceptionObject;

		if (!(eval->parenCount == 0))
		{
			pExceptionObject = "missing ')'";
			throw std::exception(pExceptionObject);
		}
		while (Eval_EvaluationStep(eval));
		if (eval->opStackPos)
		{
			__debugbreak();
		}
		if (eval->valStackPos > 1)
		{
			pExceptionObject = "extra operand (for example, 'a b +')";
			throw std::exception(pExceptionObject);
		}
		*result = eval->valStack[0];
		return result;
	}

	bool __cdecl PC_Float_Expression_Parse(/*int handle,*/ float* f)
	{
		EvalValue result; // [esp+8h] [ebp-5468h] BYREF
		EvalOperatorType op; // [esp+2Ch] [ebp-5444h] BYREF
		Eval eval; // [esp+40h] [ebp-5430h] BYREF
		int v14; // [esp+5054h] [ebp-41Ch]
		pc_token_s pc_token; // [esp+5058h] [ebp-418h] BYREF

		eval.opStackPos = 0;
		eval.valStackPos = 0;
		eval.parenCount = 0;
		eval.pushedOp = 0;
		v14 = 0;
		while (1)
		{
			while (1)
			{
				if (!PC_ReadTokenHandle(/*handle,*/ &pc_token))
					return 0;
				if (pc_token.type != 3)
					break;
				if (!Eval_PushNumber(&eval, pc_token.floatvalue))
					goto LABEL_6;
			}
			if (!Eval_OperatorForToken(pc_token.string, &op))
			{
				PC_SourceError(/*handle,*/ "expected operator but found %s", pc_token.string);
				return 0;
			}
			if (op == EVAL_OP_RPAREN)
				break;
			if (op == EVAL_OP_LPAREN && ++v14 > 16)
			{
				PC_SourceError(/*handle,*/ "too much recursive macro expansion");
				return 0;
			}
		LABEL_16:
			Eval_PushOperator(&eval, op);
		}
		if (v14)
		{
			--v14;
			goto LABEL_16;
		}
		if (Eval_AnyMissingOperands(&eval))
		{
		LABEL_6:
			PC_SourceError(/*handle,*/ "error evaluating expression");
			return 0;
		}
		Eval_Solve(&result, &eval);
		if (result.type != EVAL_VALUE_DOUBLE)
		{
			__debugbreak();
		}
		*f = result.u.d;
		return 1;
	}

	bool PC_Float_Parse(/*int handle,*/ float* f)
	{
		pc_token_s token;
		int negative;

		if (!PC_ReadTokenHandle(/*handle,*/ &token))
			return 0;
		if (token.string[0] == '(')
			return PC_Float_Expression_Parse(/*handle,*/ f);
		negative = 0;
		if (token.string[0] == '-')
		{
			if (!PC_ReadTokenHandle(/*handle,*/ &token))
				return 0;
			negative = 1;
		}
		if (token.type == TT_NUMBER)
		{
			if (negative)
				*f = -token.floatvalue;
			else
				*f = token.floatvalue;
			return 1;
		}
		else
		{
			PC_SourceError(/*handle,*/ "expected float but found %s", token.string);
			return 0;
		}
		return 0;
	}

	bool __cdecl PC_Int_Expression_Parse(/*int handle,*/ int* i)
	{
		EvalValue result; // [esp+0h] [ebp-5468h] BYREF
		EvalOperatorType op; // [esp+24h] [ebp-5444h] BYREF
		Eval eval; // [esp+38h] [ebp-5430h] BYREF
		int v13; // [esp+504Ch] [ebp-41Ch]
		pc_token_s pc_token; // [esp+5050h] [ebp-418h] BYREF

		eval.opStackPos = 0;
		eval.valStackPos = 0;
		eval.parenCount = 0;
		eval.pushedOp = 0;
		v13 = 0;
		while (1)
		{
			while (1)
			{
				if (!PC_ReadTokenHandle(/*handle,*/ &pc_token))
					return 0;
				if (pc_token.type != 3)
					break;
				if (!Eval_PushInteger(&eval, pc_token.intvalue))
					goto LABEL_6;
			}
			if (!Eval_OperatorForToken(pc_token.string, &op))
			{
				PC_SourceError(/*handle,*/ "expected operator but found %s", pc_token.string);
				return 0;
			}
			if (op == EVAL_OP_RPAREN)
				break;
			if (op == EVAL_OP_LPAREN && ++v13 > 16)
			{
				PC_SourceError(/*handle,*/ "too much recursive macro expansion");
				return 0;
			}
		LABEL_16:
			Eval_PushOperator(&eval, op);
		}
		if (v13)
		{
			--v13;
			goto LABEL_16;
		}
		if (Eval_AnyMissingOperands(&eval))
		{
		LABEL_6:
			PC_SourceError(/*handle,*/ "error evaluating expression");
			return 0;
		}
		Eval_Solve(&result, &eval);
		if (result.type != EVAL_VALUE_INT)
		{
			__debugbreak();
		}
		*i = result.u.i;
		return 1;
	}

	bool PC_Int_Parse(/*int handle,*/ int* i)
	{
		pc_token_s token;
		int negative;

		if (!PC_ReadTokenHandle(/*handle,*/ &token))
			return 0;
		if (token.string[0] == '(')
			return PC_Int_Expression_Parse(/*handle,*/ i);
		negative = 0;
		if (token.string[0] == '-')
		{
			if (!PC_ReadTokenHandle(/*handle,*/ &token))
				return 0;
			negative = 1;
		}
		if (token.type == TT_NUMBER)
		{
			*i = token.intvalue;
			if (negative)
				*i = -*i;
			return 1;
		}
		else
		{
			PC_SourceError(/*handle,*/ "expected integer but found %s", token.string);
			return 0;
		}
		return 0;
	}

	bool PC_Int_ParseLine(/*int handle,*/ int* i)
	{
		pc_token_s token;
		int negative;

		if (!PC_ReadLineHandle(/*handle,*/ &token))
			return 0;
		negative = 0;
		if (token.string[0] == '-')
		{
			if (!PC_ReadLineHandle(/*handle,*/ &token))
				return 0;
			negative = 1;
		}
		if (token.type == TT_NUMBER)
		{
			*i = token.intvalue;
			if (negative)
				*i = -*i;
			return 1;
		}
		else
		{
			PC_SourceError(/*handle,*/ "expected integer but found %s", token.string);
			return 0;
		}
		return 0;
	}

	bool PC_Byte_Parse(/*int handle,*/ unsigned __int8* b)
	{
		int i;

		if (!PC_Int_Parse(/*handle,*/ &i))
			return 0;
		if (i >= 0 && i <= 255)
		{
			*b = i;
			return 1;
		}
		else
		{
			PC_SourceError(/*handle,*/ "expect int value between 0 and 255, but got '%d'", i);
			return 0;
		}
		return 1;
	}

	bool PC_Char_Parse(/*int handle,*/ char* out)
	{
		pc_token_s token;

		if (!PC_ReadTokenHandle(/*handle,*/ &token))
			return 0;
		*out = token.string[0];
		return 1;
	}

	bool PC_Color_Parse(/*int handle,*/ vec4_t* c)
	{
		float f;
		int i;

		for (i = 0; i < 4; ++i)
		{
			if (!PC_Float_Parse(/*handle,*/ &f))
				return 0;
			(*c)[i] = f;
		}
		return 1;
	}

	bool PC_Flag_ParseOptional(/*int handle,*/ int* i)
	{
		pc_token_s token;

		if (!PC_ReadTokenHandle(/*handle,*/ &token))
			return 0;
		if (token.type == TT_NUMBER)
		{
			*i = token.intvalue;
			return 1;
		}
		else
		{
			PC_UnreadLastTokenHandle(/*handle*/);
		}
		return 0;
	}

	bool PC_Rect_Parse(/*int handle,*/ rectDef_s* r)
	{
		int horzAlign;
		int vertAlign;

		if (!PC_Float_Parse(/*handle,*/ &r->x)
			|| !PC_Float_Parse(/*handle,*/ &r->y)
			|| !PC_Float_Parse(/*handle,*/ &r->w)
			|| !PC_Float_Parse(/*handle,*/ &r->h))
		{
			return 0;
		}

		r->horzAlign = 0;
		r->vertAlign = 0;
		if (PC_Flag_ParseOptional(/*handle,*/ &horzAlign))
		{
			r->horzAlign = horzAlign;
			if (PC_Flag_ParseOptional(/*handle,*/ &vertAlign))
			{
				r->vertAlign = vertAlign;
			}
		}

		return 1;
	}

	bool __cdecl PC_Script_Parse(/*int handle,*/ const char** out) // not tested
	{
		char dst[0x1400];
		pc_token_s pc_token;

		memset(dst, 0, sizeof(dst));
		if (!PC_ReadTokenHandle(/*handle,*/ &pc_token))
			return 0;
		if (_stricmp(pc_token.string, "{"))
			return 0;
		do
		{
			if (!PC_ReadTokenHandle(/*handle,*/ &pc_token))
				return 0;
			if (!_stricmp(pc_token.string, "}"))
			{
				*out = zmem->duplicate_string(dst);
				return 1;
			}
			if (strlen(pc_token.string) + &dst[strlen(dst) + 1] - &dst[1] > sizeof(dst))
				break;
			if (pc_token.type <= 0)
				goto LABEL_16;
			if (pc_token.type <= 2)
			{
				snprintf(dst + strlen(dst), sizeof(dst), "\"%s\" ", pc_token.string);
			}
			else if (pc_token.type != 6)
			{
			LABEL_16:
				snprintf(dst + strlen(dst), sizeof(dst), "%s", pc_token.string);
				if (pc_token.type != 5 || pc_token.subtype != 30)
					snprintf(dst + strlen(dst), sizeof(dst), "%s", " ");
				continue;
			}
		} while ((unsigned int)(&dst[strlen(dst) + 1] - &dst[1] + 1) <= sizeof(dst));
		PC_SourceError(/*handle,*/ "action block too long that starts with: %s", dst);
		return 0;
	}

	bool __cdecl PC_EventScript_Parse(/*int handle,*/ MenuEventHandlerSet** eventHandlerSet)
	{
		pc_token_s token;
		MenuEventHandler* eventHandler;
		EventType eventType;
		char dst[0x1000] = { 0 };

		*eventHandlerSet = zmem->allocate<MenuEventHandlerSet>();
		(*eventHandlerSet)->eventHandlers = zmem->allocate<MenuEventHandler*>(MAX_EVENT_HANDLERS_PER_EVENT);

		if (!PC_ReadTokenHandle(/*handle,*/ &token))
			return 0;
		if (token.string[0] != '{')
		{
			PC_SourceError(/*handle,*/ "Expected '{'.");
			return 0;
		}

		while (1)
		{
			if (!PC_ReadTokenHandle(/*handle,*/ &token))
			{
				PC_SourceError(/*handle,*/ "Unexpected end of file.");
				return 0;
			}

			if ((*eventHandlerSet)->eventHandlerCount == MAX_EVENT_HANDLERS_PER_EVENT)
			{
				PC_SourceError(/*handle,*/ "Exceeded MAX_EVENT_HANDLERS_PER_EVENT(%d)", MAX_EVENT_HANDLERS_PER_EVENT);
				return 0;
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
				if (!Expression_Read(/*handle,*/ &eventHandler->eventData.conditionalScript->eventExpression))
				{
					PC_SourceError(/*handle,*/ "Could not read expression.");
					return 0;
				}
				if (!PC_EventScript_Parse(/*handle,*/ &eventHandler->eventData.conditionalScript->eventHandlerSet))
				{
					PC_SourceError(/*handle,*/ "Could not read event handler set.");
					return 0;
				}
				(*eventHandlerSet)->eventHandlers[(*eventHandlerSet)->eventHandlerCount++] = eventHandler;
				continue;
			}
			else if (eventType == EVENT_ELSE)
			{
				if (!(*eventHandlerSet)->eventHandlerCount ||
					(*eventHandlerSet)->eventHandlers[(*eventHandlerSet)->eventHandlerCount - 1]->eventType != EVENT_IF)
				{
					PC_SourceError(/*handle,*/ "Misplaced 'else'.");
					return 0;
				}
				eventHandler = zmem->allocate<MenuEventHandler>();
				eventHandler->eventType = eventType;
				if (!PC_EventScript_Parse(/*handle,*/ &eventHandler->eventData.elseScript))
				{
					PC_SourceError(/*handle,*/ "Could not read event handler set.");
					return 0;
				}
				(*eventHandlerSet)->eventHandlers[(*eventHandlerSet)->eventHandlerCount++] = eventHandler;
				continue;
			}
			else if (eventType == EVENT_SET_LOCAL_VAR_BOOL ||
				eventType == EVENT_SET_LOCAL_VAR_INT ||
				eventType == EVENT_SET_LOCAL_VAR_FLOAT ||
				eventType == EVENT_SET_LOCAL_VAR_STRING)
			{
				if (!PC_ReadTokenHandle(/*handle,*/ &token))
				{
					PC_SourceError(/*handle,*/ "Expected local var name.");
					return 0;
				}
				eventHandler = zmem->allocate<MenuEventHandler>();
				eventHandler->eventType = eventType;
				eventHandler->eventData.setLocalVarData = zmem->allocate<SetLocalVarData>();
				eventHandler->eventData.setLocalVarData->localVarName = zmem->duplicate_string(token.string);
				if (!Expression_Read(/*handle,*/ &eventHandler->eventData.setLocalVarData->expression))
				{
					PC_SourceError(/*handle,*/ "Could not read expression.");
					return 0;
				}
				(*eventHandlerSet)->eventHandlers[(*eventHandlerSet)->eventHandlerCount++] = eventHandler;
				continue;
			}
			else
			{
				memset(dst, 0, sizeof(dst));
				PC_UnreadLastTokenHandle(/*handle*/);

				if (!PC_ReadTokenHandle(/*handle,*/ &token))
					return 0;
				if (!_stricmp(token.string, ";"))
				{
					continue;
				}
				PC_UnreadLastTokenHandle(/*handle*/);
				do
				{
					if (!PC_ReadTokenHandle(/*handle,*/ &token))
						return 0;
					if (!_stricmp(token.string, "if"))
					{
						PC_UnreadLastTokenHandle(/*handle*/);
						break;
					}
					else if (!_stricmp(token.string, "}"))
					{
						PC_UnreadLastTokenHandle(/*handle*/);
						break;
					}
					else if (!_stricmp(token.string, "{"))
					{
						PC_SourceError(/*/handle,*/ "Unexpected '{' in an event handler.");
						return 0;
					}
					if (strlen(token.string) + strlen(dst) + 1 >= sizeof(dst))
					{
						PC_SourceError(/*handle,*/ "action block too long that starts with: %s", dst);
						return 0;
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

		return 1;
	}

	editFieldDef_s* __cdecl Item_GetEditFieldDef(itemDef_t* item)
	{
		editFieldDef_s* result;

		switch (item->dataType)
		{
		case 0:
		case 4:
		case 9:
		case 10:
		case 11:
		case 14:
		case 16:
		case 17:
		case 18:
		case 22:
		case 23:
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

	listBoxDef_s* __cdecl Item_GetListBoxDef(itemDef_t* item)
	{
		if (item->dataType == 6)
			return item->typeData.listBox;
		ZONETOOL_ERROR("Menu Error: Expecting type: ITEM_TYPE_LISTBOX");
		return 0;
	}

	multiDef_s* __cdecl Item_GetMultiDef(itemDef_t* item)
	{
		if (item->dataType == 12)
			return item->typeData.multi;
		ZONETOOL_ERROR("Menu Error: Expecting type: ITEM_TYPE_MULTI");
		return 0;
	}

	const char* __cdecl Item_GetEnumDvarName(itemDef_t* item)
	{
		if (item->dataType == 13)
			return item->typeData.enumDvarName;
		ZONETOOL_ERROR("Menu Error: Expecting type: ITEM_TYPE_DVARENUM");
		return 0;
	}

	newsTickerDef_s* __cdecl Item_GetNewsTickerDef(itemDef_t* item)
	{
		if (item->dataType == 20)
			return item->typeData.ticker;
		ZONETOOL_ERROR("Menu Error: Expecting type: ITEM_TYPE_NEWSTICKER");
		return 0;
	}

	void __cdecl Item_ValidateTypeData(itemDef_t* item/*, int handle*/)
	{
		editFieldDef_s* editDef;

		if (item->typeData.data)
		{
			if (item->dataType != item->type)
				PC_SourceError(
					/*handle,*/
					"Attempting to change type from %d to %d.\nMove the type definition higher up in the itemDef.",
					item->dataType,
					item->type);
		}
		else
		{
			item->dataType = item->type;
			switch (item->type)
			{
			case 6:
				item->typeData.listBox = zmem->allocate<listBoxDef_s>();
				break;
			case 4:
			case 9:
			case 16:
			case 18:
			case 11:
			case 14:
			case 10:
			case 0:
			case 17:
			case 22:
			case 23:
				item->typeData.editField = zmem->allocate<editFieldDef_s>();
				if (item->type == 4
					|| item->type == 16
					|| item->type == 9
					|| item->type == 18
					|| item->type == 17
					|| item->type == 22
					|| item->type == 23)
				{
					editDef = Item_GetEditFieldDef(item);
					if (!editDef->maxPaintChars)
						editDef->maxPaintChars = 256;
				}
				break;
			case 12:
				item->typeData.multi = zmem->allocate<multiDef_s>();
				break;
			case 20:
				item->typeData.ticker = zmem->allocate<newsTickerDef_s>();
				break;
			case 21:
				item->typeData.scroll = zmem->allocate<textScrollDef_s>();
				break;
			}
		}
	}

	int __cdecl Item_IsEditFieldDef(itemDef_t* item)
	{
		int result;

		switch (item->dataType)
		{
		case 0:
		case 4:
		case 9:
		case 10:
		case 11:
		case 14:
		case 16:
		case 17:
		case 18:
		case 22:
		case 23:
			result = 1;
			break;
		default:
			result = 0;
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

	int SetItemStaticFlag(menuDef_t* menu, /*int handle,*/ int flag)
	{
		if (!menu)
			__debugbreak();
		Window_SetStaticFlags(&menu->window, flag | menu->window.staticFlags);
		return 1;
	}

	int MenuParse_name(menuDef_t* menu/*, int handle*/)
	{
		if (!PC_String_Parse(/*handle,*/ &menu->window.name))
			return 0;
		_strlwr((char*)menu->window.name);
		return 1;
	}

	int MenuParse_fullscreen(menuDef_t* menu/*, int handle*/)
	{
		return PC_Int_Parse(/*handle,*/ &menu->data->fullScreen);
	}

	int MenuParse_screenSpace(menuDef_t* menu/*, int handle*/)
	{
		SetItemStaticFlag(menu, WINDOWSTATIC_SCREENSPACE);
		return 1;
	}

	int  MenuParse_decoration(menuDef_t* menu/*, int handle*/)
	{
		SetItemStaticFlag(menu, WINDOWSTATIC_DECORATION);
		return 1;
	}

	int MenuParse_rect(menuDef_t* menu/*, int handle*/)
	{
		return PC_Rect_Parse(/*handle,*/ &menu->window.rect);
	}

	int __cdecl MenuParse_rect720(menuDef_t* menu/*, int handle*/)
	{
		// TODO:
		return 0;
	}

	int __cdecl MenuParse_pos(menuDef_t* menu/*, int handle*/)
	{
		// TODO:
		return 0;
	}

	int __cdecl MenuParse_pos720(menuDef_t* menu/*, int handle*/)
	{
		// TODO:
		return 0;
	}

	int MenuParse_group(menuDef_t* menu/*, int handle*/)
	{
		if (!PC_String_Parse(/*handle,*/ &menu->window.group))
			return 0;
		_strlwr((char*)menu->window.group);
		return 1;
	}

	int MenuParse_style(menuDef_t* menu/*, int handle*/)
	{
		return PC_Int_Parse(/*handle,*/ &menu->window.style);
	}

	int MenuParse_visible(menuDef_t* menu/*, int handle*/)
	{
		pc_token_s token;

		if (!PC_ReadTokenHandle(/*handle,*/ &token))
			return 0;
		if (!_stricmp(token.string, "when") || !_stricmp(token.string, "if"))
		{
			Window_AddDynamicFlags(&menu->window, WINDOWDYNAMIC_VISIBLE);
			return Expression_Read(/*handle,*/ &menu->data->visibleExp);
		}
		if (atoi(token.string))
		{
			Window_AddDynamicFlags(&menu->window, WINDOWDYNAMIC_VISIBLE);
		}
		return 1;
	}

	int MenuParse_onOpen(menuDef_t* menu/*, int handle*/)
	{
		return PC_EventScript_Parse(/*handle,*/ &menu->data->onOpen);
	}

	int MenuParse_onClose(menuDef_t* menu/*, int handle*/)
	{
		return PC_EventScript_Parse(/*handle,*/ &menu->data->onClose);
	}

	int MenuParse_onCloseRequest(menuDef_t* menu/*, int handle*/)
	{
		return PC_EventScript_Parse(/*handle,*/ &menu->data->onCloseRequest);
	}

	int MenuParse_onESC(menuDef_t* menu/*, int handle*/)
	{
		return PC_EventScript_Parse(/*handle,*/ &menu->data->onESC);
	}

	int MenuParse_border(menuDef_t* menu/*, int handle*/)
	{
		return PC_Int_Parse(/*handle, */&menu->window.border);
	}

	int MenuParse_borderSize(menuDef_t* menu/*, int handle*/)
	{
		return PC_Float_Parse(/*handle,*/ &menu->window.borderSize);
	}

	int MenuParse_backcolor(menuDef_t* menu/*, int handle*/)
	{
		return PC_Color_Parse(/*handle,*/ &menu->window.backColor);
	}

	int MenuParse_forecolor(menuDef_t* menu/*, int handle*/)
	{
		if (!PC_Color_Parse(/*handle,*/ &menu->window.foreColor))
			return 0;
		Window_AddDynamicFlags(&menu->window, WINDOWDYNAMIC_FORECOLOR);
		return 1;
	}

	int MenuParse_bordercolor(menuDef_t* menu/*, int handle*/)
	{
		return PC_Color_Parse(/*handle,*/ &menu->window.borderColor);
	}

	int MenuParse_focuscolor(menuDef_t* menu/*, int handle*/)
	{
		return PC_Color_Parse(/*handle,*/ &menu->data->focusColor);
	}

	int MenuParse_outlinecolor(menuDef_t* menu/*, int handle*/)
	{
		return PC_Color_Parse(/*handle,*/ &menu->window.outlineColor);
	}

	int MenuParse_background(menuDef_t* menu/*, int handle*/)
	{
		const char* name;

		if (!PC_String_Parse(/*handle,*/ &name))
			return 0;
		menu->window.background = zmem->manual_allocate<Material>(sizeof(char*));
		menu->window.background->name = name;
		_strlwr((char*)menu->window.background->name);
		return 1;
	}

	int MenuParse_ownerDraw(menuDef_t* menu/*, int handle*/)
	{
		return PC_Int_Parse(/*handle, */&menu->window.ownerDraw);
	}

	int MenuParse_ownerDrawFlags(menuDef_t* menu/*, int handle*/)
	{
		return PC_Int_Parse(/*handle, */&menu->window.ownerDrawFlags);
	}

	int MenuParse_outOfBounds(menuDef_t* menu/*, int handle*/)
	{
		Window_SetStaticFlags(&menu->window, WINDOWSTATIC_OUTOFBOUNDSCLICK | menu->window.staticFlags);
		return 1;
	}

	int MenuParse_soundLoop(menuDef_t* menu/*, int handle*/)
	{
		return PC_String_Parse(/*handle,*/ &menu->data->soundName);
	}

	int MenuParse_execExp(menuDef_t* menu/*, int handle*/)
	{
		const char* expressionType;
		pc_token_s token;

		if (!PC_String_Parse(/*handle,*/ &expressionType))
			return 0;

		if (!_stricmp(expressionType, "visible"))
		{
			if (!PC_ReadTokenHandle(/*handle,*/ &token))
			{
				PC_SourceError(/*handle,*/ "ERROR: line ended early after \"visible\"");
				return 0;
			}
			if (_stricmp(token.string, "when"))
			{
				PC_SourceError(/*handle,*/ "ERROR: Expected 'when' after \"visible\" but found \"%s\"", token.string);
				return 0;
			}
			Window_AddDynamicFlags(&menu->window, WINDOWDYNAMIC_VISIBLE);
			if (Expression_Read(/*handle,*/ &menu->data->visibleExp))
				return 1;
		}
		else if (!_stricmp(expressionType, "rect"))
		{
			if (!PC_ReadTokenHandle(/*handle,*/ &token))
			{
				PC_SourceError(/*handle,*/ "ERROR: line ended early after \"exp rect\"");
				return 0;
			}
			if (!_stricmp(token.string, "X"))
			{
				return (Expression_Read(/*handle,*/ &menu->data->rectXExp)) != 0;
			}
			else if (!_stricmp(token.string, "Y"))
			{
				return (Expression_Read(/*handle,*/ &menu->data->rectYExp)) != 0;
			}
			else if (!_stricmp(token.string, "W"))
			{
				return (Expression_Read(/*handle,*/ &menu->data->rectWExp)) != 0;
			}
			else if (!_stricmp(token.string, "H"))
			{
				return (Expression_Read(/*handle,*/ &menu->data->rectHExp)) != 0;
			}
			PC_SourceError(/*handle,*/ "ERROR: Expected 'X', 'Y', 'W' or 'H' after \"exp rect\" but found \"%s\"", token.string);
			return 0;
		}
		else if (!_stricmp(expressionType, "openSound"))
		{
			if (Expression_Read(/*handle,*/ &menu->data->openSoundExp))
				return 1;
		}
		else if (!_stricmp(expressionType, "closeSound"))
		{
			if (Expression_Read(/*handle,*/ &menu->data->closeSoundExp))
				return 1;
		}
		else if (!_stricmp(expressionType, "soundLoop"))
		{
			if (Expression_Read(/*handle,*/ &menu->data->soundLoopExp))
				return 1;
		}
		else
		{
			PC_SourceError(/*handle,*/ "ERROR: Unknown exp type '%s'", token.string);
		}
		return 0;
	}

	int MenuParse_popup(menuDef_t* menu/*, int handle*/)
	{
		Window_SetStaticFlags(&menu->window, WINDOWSTATIC_POPUP | menu->window.staticFlags);
		return 1;
	}

	int MenuParse_fadeClamp(menuDef_t* menu/*, int handle*/)
	{
		return PC_Float_Parse(/*handle,*/ &menu->data->fadeClamp);
	}

	int MenuParse_fadeCycle(menuDef_t* menu/*, int handle*/)
	{
		return PC_Int_Parse(/*handle,*/ &menu->data->fadeCycle);
	}

	int MenuParse_fadeAmount(menuDef_t* menu/*, int handle*/)
	{
		return PC_Float_Parse(/*handle,*/ &menu->data->fadeAmount);
	}

	int MenuParse_fadeInAmount(menuDef_t* menu/*, int handle*/)
	{
		return PC_Float_Parse(/*handle,*/ &menu->data->fadeInAmount);
	}

	int MenuParse_execKey(menuDef_t* menu/*, int handle*/)
	{
		char key;
		MenuEventHandlerSet* action;
		ItemKeyHandler* handler;

		if (!PC_Char_Parse(/*handle,*/ &key))
			return 0;
		if (!PC_EventScript_Parse(/*handle,*/ &action))
			return 0;
		handler = zmem->allocate<ItemKeyHandler>();
		handler->key = key;
		handler->next = menu->data->onKey;
		handler->action = action;
		menu->data->onKey = handler;
		return 1;
	}

	int MenuParse_execKeyInt(menuDef_t* menu/*, int handle*/)
	{
		int key;
		MenuEventHandlerSet* action;
		ItemKeyHandler* handler;

		if (!PC_Int_Parse(/*handle,*/ &key))
			return 0;
		if (!PC_EventScript_Parse(/*handle,*/ &action))
			return 0;
		handler = zmem->allocate<ItemKeyHandler>();
		handler->key = key;
		handler->next = menu->data->onKey;
		handler->action = action;
		menu->data->onKey = handler;
		return 1;
	}

	int MenuParse_blurWorld(menuDef_t* menu/*, int handle*/)
	{
		if (!PC_Float_Parse(/*handle,*/ &menu->data->blurRadius))
			return 0;
		if (menu->data->blurRadius >= 0.0)
			return 1;
		PC_SourceError(/*handle,*/ "blur must be >= 0; %g is invalid", menu->data->blurRadius);
		return 0;
	}

	int MenuParse_legacySplitScreenScale(menuDef_t* menu/*, int handle*/)
	{
		return SetItemStaticFlag(menu, /*handle,*/ WINDOWSTATIC_LEGACYSPLITSCREENSCALE);
	}

	int MenuParse_hiddenDuringScope(menuDef_t* menu/*, int handle*/)
	{
		return SetItemStaticFlag(menu, /*handle,*/ WINDOWSTATIC_HIDDENDURINGSCOPE);
	}

	int MenuParse_hiddenDuringFlashbang(menuDef_t* menu/*, int handle*/)
	{
		return SetItemStaticFlag(menu, /*handle,*/ WINDOWSTATIC_HIDDENDURINGFLASH);
	}

	int MenuParse_hiddenDuringUI(menuDef_t* menu/*, int handle*/)
	{
		return SetItemStaticFlag(menu, /*handle,*/ WINDOWSTATIC_HIDDENDURINGUI);
	}

	int MenuParse_allowedBinding(menuDef_t* menu/*, int handle*/)
	{
		if (menu->data->allowedBinding)
			PC_SourceError(/*handle,*/ "Only one 'allowedBinding' is supported");
		return PC_String_Parse(/*handle,*/ &menu->data->allowedBinding);
	}

	int MenuParse_textOnlyFocus(menuDef_t* menu/*, int handle*/)
	{
		return SetItemStaticFlag(menu, /*handle,*/ WINDOWSTATIC_TEXTONLYFOCUS);
	}

	void Window_Init(windowDef_t* window)
	{

	}

	void Item_Init(itemDef_t* item)
	{
		Window_Init(&item->window);
	}

	void __cdecl Item_InitControls(itemDef_t* item)
	{
		listBoxDef_s* listPtr;

		if (item && item->type == 6)
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

	bool __cdecl ItemParse_IsValidTextAlignment(int textAlignMode)
	{
		bool result;

		if (textAlignMode >= 0 && textAlignMode <= 15)
			result = (textAlignMode & 3) != 3;
		else
			result = 0;
		return result;
	}

	int ItemParse_name(itemDef_t* item/*, int handle*/)
	{
		return PC_String_Parse(/*handle,*/ &item->window.name);
	}

	int ItemParse_text(itemDef_t* item/*, int handle*/)
	{
		return PC_String_Parse(/*handle,*/ &item->text);
	}

	char* UI_FileText(const char* fileName)
	{
		char* result;
		char buffer[0x1000];
		unsigned int len;
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

	int ItemParse_textfile(itemDef_t* item/*, int handle*/)
	{
		const char* text;
		pc_token_s token;

		if (!PC_ReadTokenHandle(/*handle,*/ &token))
			return 0;
		text = UI_FileText(token.string);
		if (!text)
			return 0;
		item->text = zmem->duplicate_string(text);
		return 1;
	}

	int __cdecl ItemParse_textsavegame(itemDef_t* item/*, int handle*/)
	{
		item->text = "savegameinfo";
		item->itemFlags |= 1u;
		return 1;
	}

	int __cdecl ItemParse_textcinematicsubtitle(itemDef_t* item/*, int handle*/)
	{
		item->text = "cinematicsubtitle";
		item->itemFlags |= 2u;
		return 1;
	}

	int ItemParse_group(itemDef_t* item/*, int handle*/)
	{
		return PC_String_Parse(/*handle,*/ &item->window.group);
	}

	int ItemParse_rect(itemDef_t* item/*, int handle*/)
	{
		return PC_Rect_Parse(/*handle,*/ &item->window.rectClient);
	}

	int __cdecl ItemParse_rect720(itemDef_t* item/*, int handle*/)
	{
		// TODO:
		return 0;
	}

	int __cdecl ItemParse_pos(itemDef_t* item/*, int handle*/)
	{
		// TODO:
		return 0;
	}

	int __cdecl ItemParse_pos720(itemDef_t* item/*, int handle*/)
	{
		// TODO:
		return 0;
	}

	int ItemParse_origin(itemDef_t* item/*, int handle*/)
	{
		float x;
		float y;

		if (!PC_Float_Parse(/*handle,*/ &x) || !PC_Float_Parse(/*handle,*/ &y))
			return 0;
		item->window.rectClient.x += x;
		item->window.rectClient.y += y;
		return 1;
	}

	int ItemParse_style(itemDef_t* item/*, int handle*/)
	{
		return PC_Int_Parse(/*handle,*/ &item->window.style);
	}

	int ItemParse_decoration(itemDef_t* item/*, int handle*/)
	{
		Window_SetStaticFlags(&item->window, item->window.staticFlags | WINDOWSTATIC_DECORATION);
		return 1;
	}

	int __cdecl ItemParse_notselectable(itemDef_t* item/*, int handle*/)
	{
		listBoxDef_s* listPtr;

		Item_ValidateTypeData(item/*, handle*/);
		listPtr = Item_GetListBoxDef(item);
		if (!listPtr)
			return 0;
		if (item->type == 6)
			listPtr->notselectable = 1;
		return 1;
	}

	int __cdecl ItemParse_usePaging(itemDef_t* item/*, int handle*/)
	{
		listBoxDef_s* listPtr;

		Item_ValidateTypeData(item/*, handle*/);
		listPtr = Item_GetListBoxDef(item);
		if (!listPtr)
			return 0;
		if (item->type == 6)
			listPtr->usePaging = 1;
		return 1;
	}

	int ItemParse_autowrapped(itemDef_t* item/*, int handle*/)
	{
		Window_SetStaticFlags(&item->window, item->window.staticFlags | WINDOWSTATIC_AUTOWRAPPED);
		return 1;
	}

	int __cdecl ItemParse_horizontalscroll(itemDef_t* item/*, int handle*/)
	{
		Window_SetStaticFlags(&item->window, item->window.staticFlags | WINDOWSTATIC_HORIZONTALSCROLL);
		return 1;
	}

	int ItemParse_type(itemDef_t* item/*, int handle*/)
	{
		if (!PC_Int_Parse(/*handle,*/ &item->type))
			return 0;
		Item_ValidateTypeData(item/*, handle*/);
		return 1;
	}

	int ItemParse_elementwidth(itemDef_t* item/*, int handle*/)
	{
		listBoxDef_s* listPtr;

		Item_ValidateTypeData(item/*, handle*/);
		listPtr = Item_GetListBoxDef(item);
		if (listPtr)
			return PC_Float_Parse(/*handle,*/ &listPtr->elementWidth) != 0;
		return 0;
	}

	int ItemParse_elementheight(itemDef_t* item/*, int handle*/)
	{
		listBoxDef_s* listPtr;

		Item_ValidateTypeData(item/*, handle*/);
		listPtr = Item_GetListBoxDef(item);
		if (listPtr)
			return PC_Float_Parse(/*handle,*/ &listPtr->elementHeight) != 0;
		return 0;
	}

	int __cdecl ItemParse_special(itemDef_t* item/*, int handle*/)
	{
		return PC_Float_Parse(/*handle,*/ &item->special) != 0;
	}

	int ItemParse_elementtype(itemDef_t* item/*, int handle*/)
	{
		int result;
		listBoxDef_s* listPtr;

		Item_ValidateTypeData(item/*, handle*/);
		if (item->typeData.data && (listPtr = Item_GetListBoxDef(item)) != 0)
			return PC_Int_Parse(/*handle,*/ &listPtr->elementStyle) != 0;
		return 0;
	}

	int __cdecl ItemParse_columns(itemDef_t* item/*, int handle*/)
	{
		int pos;
		int width;
		int align;
		int maxChars;
		listBoxDef_s* listPtr;
		int num;
		int i;

		Item_ValidateTypeData(item/*, handle*/);
		if (!item->typeData.listBox)
			return 0;
		listPtr = Item_GetListBoxDef(item);
		if (!listPtr)
			return 0;
		if (!PC_Int_Parse(/*handle,*/ &num))
			return 0;
		if (num > 16)
			num = 16;
		listPtr->numColumns = num;
		for (i = 0; i < num; ++i)
		{
			if (!PC_Int_Parse(/*handle,*/ &pos) || !PC_Int_Parse(/*handle,*/ &width) || !PC_Int_Parse(/*handle,*/ &maxChars))
				return 0;
			listPtr->columnInfo[i].xpos = pos;
			listPtr->columnInfo[i].width = width;
			listPtr->columnInfo[i].height = listPtr->elementHeight;
			listPtr->columnInfo[i].maxChars = maxChars;
			if (PC_Int_ParseLine(/*handle,*/ &align))
				listPtr->columnInfo[i].alignment = align;
			else
				listPtr->columnInfo[i].alignment = 0;
		}
		return 1;
	}

	int ItemParse_border(itemDef_t* item/*, int handle*/)
	{
		return PC_Int_Parse(/*handle,*/ &item->window.border) != 0;
	}

	int ItemParse_bordersize(itemDef_t* item/*, int handle*/)
	{
		return PC_Float_Parse(/*handle,*/ &item->window.borderSize) != 0;
	}

	int ItemParse_visible(itemDef_t* item/*, int handle*/)
	{
		pc_token_s token;

		if (!PC_ReadTokenHandle(/*handle,*/ &token))
			return 0;
		if (!_stricmp(token.string, "when") || !_stricmp(token.string, "if"))
		{
			Window_AddDynamicFlags(&item->window, WINDOWDYNAMIC_VISIBLE);
			return Expression_Read(/*handle,*/ &item->visibleExp);
		}
		if (atoi(token.string))
		{
			Window_AddDynamicFlags(&item->window, WINDOWDYNAMIC_VISIBLE);
		}
		return 1;
	}

	int ItemParse_disabled(itemDef_t* item/*, int handle*/)
	{
		pc_token_s token;

		if (!PC_ReadTokenHandle(/*handle,*/ &token))
			return 0;
		if (!_stricmp(token.string, "when") || !_stricmp(token.string, "if"))
		{
			if (Expression_Read(/*handle,*/ &item->disabledExp))
				return 1;
		}
		return 0;
	}

	int __cdecl ItemParse_ownerdraw(itemDef_t* item/*, int handle*/)
	{
		int result;

		result = PC_Int_Parse(/*handle,*/ &item->window.ownerDraw);
		if (result)
		{
			item->type = 8;
			result = 1;
		}
		return result;
	}

	int ItemParse_align(itemDef_t* item/*, int handle*/)
	{
		return PC_Int_Parse(/*handle,*/ &item->alignment) != 0;
	}

	int ItemParse_textalign(itemDef_t* item/*, int handle*/)
	{
		if (!PC_Int_Parse(/*handle,*/ &item->textAlignMode))
			return 0;
		if (ItemParse_IsValidTextAlignment(item->textAlignMode))
			return 1;
		PC_SourceError(/*handle,*/ "expected ITEM_ALIGN_* value");
		return 0;
	}

	int ItemParse_textalignx(itemDef_t* item/*, int handle*/)
	{
		return PC_Float_Parse(/*handle,*/ &item->textalignx) != 0;
	}

	int ItemParse_textaligny(itemDef_t* item/*, int handle*/)
	{
		return PC_Float_Parse(/*handle,*/ &item->textaligny) != 0;
	}

	int ItemParse_textscale(itemDef_t* item/*, int handle*/)
	{
		return PC_Float_Parse(/*handle,*/ &item->textscale) != 0;
	}

	int ItemParse_textstyle(itemDef_t* item/*, int handle*/)
	{
		return PC_Int_Parse(/*handle,*/ &item->textStyle) != 0;
	}

	int ItemParse_textfont(itemDef_t* item/*, int handle*/)
	{
		return PC_Int_Parse(/*handle,*/ &item->fontEnum) != 0;
	}

	int ItemParse_backcolor(itemDef_t* item/*, int handle*/)
	{
		return PC_Color_Parse(/*handle,*/ &item->window.backColor);
	}

	int ItemParse_forecolor(itemDef_t* item/*, int handle*/)
	{
		if (!PC_Color_Parse(/*handle,*/ &item->window.foreColor))
			return 0;
		Window_AddDynamicFlags(&item->window, WINDOWDYNAMIC_FORECOLOR);
		return 1;
	}

	int ItemParse_bordercolor(itemDef_t* item/*, int handle*/)
	{
		return PC_Color_Parse(/*handle,*/ &item->window.borderColor);
	}

	int ItemParse_outlinecolor(itemDef_t* item/*, int handle*/)
	{
		return PC_Color_Parse(/*handle,*/ &item->window.outlineColor);
	}

	int ItemParse_background(itemDef_t* item/*, int handle*/)
	{
		const char* name;

		if (!PC_String_Parse(/*handle,*/ &name))
			return 0;
		item->window.background = zmem->manual_allocate<Material>(sizeof(char*));
		item->window.background->name = name;
		_strlwr((char*)item->window.background->name);
		return 1;
	}

	int ItemParse_onFocus(itemDef_t* item/*, int handle*/)
	{
		return PC_EventScript_Parse(/*handle,*/ &item->onFocus);
	}

	int ItemParse_leaveFocus(itemDef_t* item/*, int handle*/)
	{
		return PC_EventScript_Parse(/*handle,*/ &item->leaveFocus);
	}

	int ItemParse_mouseEnter(itemDef_t* item/*, int handle*/)
	{
		return PC_EventScript_Parse(/*handle,*/ &item->mouseEnter);
	}

	int ItemParse_mouseExit(itemDef_t* item/*, int handle*/)
	{
		return PC_EventScript_Parse(/*handle,*/ &item->mouseExit);
	}

	int ItemParse_mouseEnterText(itemDef_t* item/*, int handle*/)
	{
		return PC_EventScript_Parse(/*handle,*/ &item->mouseEnterText);
	}

	int ItemParse_mouseExitText(itemDef_t* item/*, int handle*/)
	{
		return PC_EventScript_Parse(/*handle,*/ &item->mouseExitText);
	}

	int ItemParse_action(itemDef_t* item/*, int handle*/)
	{
		return PC_EventScript_Parse(/*handle,*/ &item->action);
	}

	int ItemParse_accept(itemDef_t* item/*, int handle*/)
	{
		return PC_EventScript_Parse(/*handle,*/ &item->accept);
	}

	int ItemParse_dvar(itemDef_t* item/*, int handle*/)
	{
		editFieldDef_s* editPtr;

		Item_ValidateTypeData(item/*, handle*/);
		if (!PC_String_Parse(/*handle,*/ &item->dvar))
			return 0;

		if (item->typeData.editField && Item_IsEditFieldDef(item))
		{
			editPtr = Item_GetEditFieldDef(item);

			editPtr->minVal = -1.0f;
			editPtr->maxVal = -1.0f;
			editPtr->stepVal = -1.0f;
		}

		return 1;
	}

	int ItemParse_localvar(itemDef_t* item/*, int handle*/)
	{
		editFieldDef_s* editPtr;

		Item_ValidateTypeData(item/*, handle*/);
		if (!PC_String_Parse(/*handle,*/ &item->localVar))
			return 0;

		if (item->typeData.editField && Item_IsEditFieldDef(item))
		{
			editPtr = Item_GetEditFieldDef(item);

			editPtr->minVal = -1.0f;
			editPtr->maxVal = -1.0f;
			editPtr->stepVal = -1.0f;
		}

		return 1;
	}

	int ItemParse_maxChars(itemDef_t* item/*, int handle*/)
	{
		editFieldDef_s* editPtr;
		int maxChars;

		Item_ValidateTypeData(item/*, handle*/);
		if (!item->typeData.editField)
			return 0;
		if (!PC_Int_Parse(/*handle,*/ &maxChars))
			return 0;
		editPtr = Item_GetEditFieldDef(item);
		if (!editPtr)
			return 0;
		editPtr->maxChars = maxChars;
		return 1;
	}

	int ItemParse_maxCharsGotoNext(itemDef_t* item/*, int handle*/)
	{
		editFieldDef_s* editPtr;

		Item_ValidateTypeData(item/*, handle*/);
		if (!item->typeData.editField)
			return 0;
		editPtr = Item_GetEditFieldDef(item);
		if (!editPtr)
			return 0;
		editPtr->maxCharsGotoNext = 1;
		return 1;
	}

	int ItemParse_maxPaintChars(itemDef_t* item/*, int handle*/)
	{
		editFieldDef_s* editPtr;
		int maxChars;

		Item_ValidateTypeData(item/*, handle*/);
		if (!item->typeData.editField)
			return 0;
		if (!PC_Int_Parse(/*handle,*/ &maxChars))
			return 0;
		editPtr = Item_GetEditFieldDef(item);
		if (!editPtr)
			return 0;
		editPtr->maxPaintChars = maxChars;
		return 1;
	}

	int ItemParse_focusSound(itemDef_t* item/*, int handle*/)
	{
		const char* name;

		if (!PC_String_Parse(/*handle,*/ &name))
			return 0;
		item->focusSound = zmem->manual_allocate<snd_alias_list_t>(sizeof(char*));
		item->focusSound->name = name;
		return 1;
	}

	int ItemParse_dvarFloat(itemDef_t* item/*, int handle*/)
	{
		editFieldDef_s* editPtr; // [esp+0h] [ebp-4h]

		Item_ValidateTypeData(item/*, handle*/);
		if (!item->typeData.editField)
			return 0;
		editPtr = Item_GetEditFieldDef(item);
		if (!editPtr)
			return 0;
		return PC_String_Parse(/*handle,*/ &item->dvar)
			&& PC_Float_Parse(/*handle,*/ &editPtr->stepVal)
			&& PC_Float_Parse(/*handle,*/ &editPtr->minVal)
			&& PC_Float_Parse(/*handle,*/ &editPtr->maxVal);
	}

	int __cdecl ItemParse_dvarStrList(itemDef_t* item/*, int handle*/)
	{
		int pass;
		multiDef_s* multiPtr;
		pc_token_s token;

		Item_ValidateTypeData(item/*, handle*/);
		if (!item->typeData.multi)
			return 0;
		if (item->type != 12)
			return 0;
		multiPtr = Item_GetMultiDef(item);
		multiPtr->count = 0;
		multiPtr->strDef = 1;
		if (!PC_ReadTokenHandle(/*handle,*/ &token))
			return 0;
		if (token.string[0] != '{')
			return 0;
		pass = 0;
		do
		{
			while (1)
			{
				do
				{
					if (!PC_ReadTokenHandle(/*handle,*/ &token))
					{
						PC_SourceError(/*handle,*/ "end of file inside menu item");
						return 0;
					}
					if (token.string[0] == '}')
						return 1;
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
		return 0;
	}

	int __cdecl ItemParse_dvarFloatList(itemDef_t* item/*, int handle*/)
	{
		multiDef_s* multiPtr;
		pc_token_s token;

		Item_ValidateTypeData(item/*, handle*/);
		if (!item->typeData.multi)
			return 0;
		if (item->type != 12)
			return 0;
		multiPtr = Item_GetMultiDef(item);
		multiPtr->count = 0;
		multiPtr->strDef = 0;
		if (!PC_ReadTokenHandle(/*handle,*/ &token))
			return 0;
		if (token.string[0] != '{')
			return 0;
		do
		{
			do
			{
				if (!PC_ReadTokenHandle(/*handle,*/ &token))
				{
					PC_SourceError(/*handle,*/ "end of file inside menu item");
					return 0;
				}
				if (token.string[0] == '}')
					return 1;
			} while (token.string[0] == ',' || token.string[0] == ';');
			multiPtr->dvarList[multiPtr->count] = zmem->duplicate_string(token.string);
			if (!PC_Float_Parse(/*handle,*/ &multiPtr->dvarValue[multiPtr->count]))
				return 0;
			++multiPtr->count;
		} while (multiPtr->count < 32);
		return 0;
	}

	int __cdecl ItemParse_dvarEnumList(itemDef_t* item/*, int handle*/)
	{
		const char* enumDvarPtr;

		Item_ValidateTypeData(item/*, handle*/);
		enumDvarPtr = Item_GetEnumDvarName(item);
		if (item->type != 13)
			return 0;
		if (!enumDvarPtr)
			return PC_String_Parse(/*handle,*/ &enumDvarPtr);
		PC_SourceError(/*handle,*/ "enumDvarList already given");
		return 0;
	}

	int __cdecl ItemParse_ownerdrawFlag(itemDef_t* item/*, int handle*/)
	{
		int flags;

		if (!PC_Int_Parse(/*handle,*/ &flags))
			return 0;
		item->window.ownerDrawFlags |= flags;
		return 1;
	}

	int __cdecl ItemParse_enableDvar(itemDef_t* item/*, int handle*/)
	{
		if (!PC_Script_Parse(/*handle,*/ &item->enableDvar))
			return 0;
		item->dvarFlags |= 1u;
		return 1;
	}

	int __cdecl ItemParse_dvarTest(itemDef_t* item/*, int handle*/)
	{
		return PC_String_Parse(/*handle,*/ &item->dvarTest) != 0;
	}

	int __cdecl ItemParse_disableDvar(itemDef_t* item/*, int handle*/)
	{
		if (!PC_Script_Parse(/*handle,*/ &item->enableDvar))
			return 0;
		item->dvarFlags |= 2u;
		return 1;
	}

	int __cdecl ItemParse_showDvar(itemDef_t* item/*, int handle*/)
	{
		if (!PC_Script_Parse(/*handle,*/ &item->enableDvar))
			return 0;
		item->dvarFlags |= 4u;
		return 1;
	}

	int __cdecl ItemParse_hideDvar(itemDef_t* item/*, int handle*/)
	{
		if (!PC_Script_Parse(/*handle,*/ &item->enableDvar))
			return 0;
		item->dvarFlags |= 8u;
		return 1;
	}

	int __cdecl ItemParse_focusDvar(itemDef_t* item/*, int handle*/)
	{
		if (!PC_Script_Parse(/*handle,*/ &item->enableDvar))
			return 0;
		item->dvarFlags |= 0x10u;
		return 1;
	}

	int ItemParse_doubleClick(itemDef_t* item/*, int handle*/)
	{
		listBoxDef_s* listPtr;

		Item_ValidateTypeData(item/*, handle*/);
		if (!item->typeData.listBox)
			return 0;
		listPtr = Item_GetListBoxDef(item);
		if (!listPtr)
			return 0;
		return PC_EventScript_Parse(/*handle,*/ &listPtr->onDoubleClick);
	}

	int ItemParse_execKey(itemDef_t* item/*, int handle*/)
	{
		char key;
		MenuEventHandlerSet* action;
		ItemKeyHandler* handler;

		if (!PC_Char_Parse(/*handle,*/ &key))
			return 0;
		if (!PC_EventScript_Parse(/*handle,*/ &action))
			return 0;
		handler = zmem->allocate<ItemKeyHandler>();
		handler->key = key;
		handler->next = item->onKey;
		handler->action = action;
		item->onKey = handler;
		return 1;
	}

	int ItemParse_execKeyInt(itemDef_t* item/*, int handle*/)
	{
		int key;
		MenuEventHandlerSet* action;
		ItemKeyHandler* handler;

		if (!PC_Int_Parse(/*handle,*/ &key))
			return 0;
		if (!PC_EventScript_Parse(/*handle,*/ &action))
			return 0;
		handler = zmem->allocate<ItemKeyHandler>();
		handler->key = key;
		handler->next = item->onKey;
		handler->action = action;
		item->onKey = handler;
		return 1;
	}

	int ItemParse_execExp(itemDef_t* item/*, int handle*/)
	{
		const char* expressionType;
		const char* expressionComponent;
		ItemFloatExpressionTarget target;

		target = ITEM_FLOATEXP_TGT__COUNT;

		if (!PC_String_Parse(/*handle,*/ &expressionType))
			return 0;

		if (!_stricmp(expressionType, "visible"))
		{
			if (item->visibleExp)
			{
				PC_SourceError(/*handle,*/ "redefinition of expression for '%s'.", expressionType);
				return 0;
			}
			else
			{
				Window_AddDynamicFlags(&item->window, WINDOWDYNAMIC_VISIBLE);
				return Expression_Read(/**handle,*/ &item->visibleExp);
			}
		}
		else if (!_stricmp(expressionType, "text"))
		{
			if (item->textExp)
			{
				PC_SourceError(/*handle,*/ "redefinition of expression for '%s'.", expressionType);
				return 0;
			}
			else
			{
				return Expression_Read(/*handle,*/ &item->textExp);
			}
		}
		else if (!_stricmp(expressionType, "material"))
		{
			if (item->materialExp)
			{
				PC_SourceError(/*handle,*/ "redefinition of expression for '%s'.", expressionType);
				return 0;
			}
			else
			{
				return Expression_Read(/*handle,*/ &item->materialExp);
			}
		}
		else if (!_stricmp(expressionType, "disabled"))
		{
			if (item->disabledExp)
			{
				PC_SourceError(/*handle,*/ "redefinition of expression for '%s'.", expressionType);
				return 0;
			}
			else
			{
				return Expression_Read(/*handle,*/ &item->disabledExp);
			}
		}
		if (!PC_String_Parse(&expressionComponent))
		{
			PC_SourceError(/*handle,*/ "component required (exp type '%s' is not a known single-component expression slot)",
				expressionType);
			return 0;
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
						PC_SourceError(/*handle,*/ "redefinition of expression for '%s', component '%s.", expressionType, expressionComponent);
						return 0;
					}
					else
					{
						if (!Expression_Read(/*handle,*/ &item->floatExpressions[i].expression))
							return 0;
						if (target == ITEM_FLOATEXP_TGT_FORECOLOR_R ||
							target == ITEM_FLOATEXP_TGT_FORECOLOR_G ||
							target == ITEM_FLOATEXP_TGT_FORECOLOR_B ||
							target == ITEM_FLOATEXP_TGT_FORECOLOR_A)
						{
							Window_AddDynamicFlags(&item->window, WINDOWDYNAMIC_FORECOLOR);
						}
						return 1;
					}
				}
			}
			if (!Expression_Read(/*handle,*/ &item->floatExpressions[item->floatExpressionCount].expression))
				return 0;
			if (target == ITEM_FLOATEXP_TGT_FORECOLOR_R ||
				target == ITEM_FLOATEXP_TGT_FORECOLOR_G ||
				target == ITEM_FLOATEXP_TGT_FORECOLOR_B ||
				target == ITEM_FLOATEXP_TGT_FORECOLOR_A)
			{
				Window_AddDynamicFlags(&item->window, WINDOWDYNAMIC_FORECOLOR);
			}
			item->floatExpressions[item->floatExpressionCount].target = target;
			item->floatExpressionCount++;
			return 1;
		}
		PC_SourceError(/*handle,*/ "unknown exp type and component '%s' '%s'", expressionType, expressionComponent);
		return 0;
	}

	int ItemParse_gameMsgWindowIndex(itemDef_t* item/*, int handle*/)
	{
		return PC_Int_Parse(/*handle,*/ &item->gameMsgWindowIndex);
	}

	int ItemParse_gameMsgWindowMode(itemDef_t* item/*, int handle*/)
	{
		return PC_Int_Parse(/*handle,*/ &item->gameMsgWindowMode);
	}

	int ItemParse_selectBorder(itemDef_t* item/*, int handle*/)
	{
		listBoxDef_s* listPtr;

		Item_ValidateTypeData(item/*, handle*/);
		listPtr = Item_GetListBoxDef(item);
		if (listPtr)
			return PC_Color_Parse(/*handle,*/ &listPtr->selectBorder);
		return 0;
	}

	int ItemParse_disableColor(itemDef_t* item/*, int handle*/)
	{
		return PC_Color_Parse(/*handle,*/ &item->window.disableColor);
	}

	int ItemParse_selectIcon(itemDef_t* item/*, int handle*/)
	{
		listBoxDef_s* listPtr;
		const char* name;

		Item_ValidateTypeData(item/*, handle*/);
		listPtr = Item_GetListBoxDef(item);
		if (!listPtr)
			return 0;
		if (!PC_String_Parse(/*handle,*/ &name))
			return 0;

		listPtr->selectIcon = zmem->manual_allocate<Material>(sizeof(char*));
		listPtr->selectIcon->name = name;
		_strlwr((char*)listPtr->selectIcon->name);
		return 1;
	}

	int ItemParse_spacing(itemDef_t* item/*, int handle*/)
	{
		newsTickerDef_s* tickerPtr;
		int spacing;

		Item_ValidateTypeData(/*handle,*/ item);
		tickerPtr = Item_GetNewsTickerDef(item);
		if (!tickerPtr)
			return 0;
		if (!PC_Int_Parse(/*handle,*/ &spacing) || spacing < 0)
			return 0;
		tickerPtr->spacing = spacing;
		return 1;
	}

	int ItemParse_speed(itemDef_t* item/*, int handle*/)
	{
		newsTickerDef_s* tickerPtr;
		int speed;

		Item_ValidateTypeData(/*handle,*/ item);
		tickerPtr = Item_GetNewsTickerDef(item);
		if (!tickerPtr)
			return 0;
		if (!PC_Int_Parse(/*handle,*/ &speed) || speed < 0)
			return 0;
		tickerPtr->speed = speed;
		return 1;
	}

	int ItemParse_newsfeed(itemDef_t* item/*, int handle*/)
	{
		newsTickerDef_s* tickerPtr;
		int feedId;

		Item_ValidateTypeData(/*handle,*/ item);
		tickerPtr = Item_GetNewsTickerDef(item);
		if (!tickerPtr)
			return 0;
		if (!PC_Int_Parse(/*handle,*/ &feedId))
			return 0;
		tickerPtr->feedId = feedId;
		return 1;
	}

	int ItemParse_glowColor(itemDef_t* item/*, int handle*/)
	{
		Item_ValidateTypeData(item/*, handle*/);
		return PC_Color_Parse(/*handle,*/ &item->glowColor);
	}

	int ItemParse_decodeEffect(itemDef_t* item/*, int handle*/)
	{
		Item_ValidateTypeData(item/*, handle*/);
		if (!PC_Int_Parse(/*handle,*/ &item->fxLetterTime)
			|| !PC_Int_Parse(/*handle,*/ &item->fxDecayStartTime)
			|| !PC_Int_Parse(/*handle,*/ &item->fxDecayDuration))
		{
			return 0;
		}
		item->decayActive = 1;
		return 1;
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

	int Item_Parse(/*int handle,*/ itemDef_t* item)
	{
		parse_itemdef_func* key;
		pc_token_s token;

		if (!PC_ReadTokenHandle(/*handle,*/ &token))
			return 0;
		if (token.string[0] != '{')
			return 0;
		do
		{
			while (1)
			{
				do
				{
					if (!PC_ReadTokenHandle(/*handle,*/ &token))
					{
						PC_SourceError(/*handle,*/ "end of file inside menu item");
						return 0;
					}
					if (token.string[0] == '}')
						return 1;
				} while (token.string[0] == ';');
				key = find_itemdef_func(token.string);
				if (key)
					break;
				PC_SourceError(/*handle,*/ "unknown menu item keyword %s", token.string);
			}
		} while (key->func(item/*, handle*/));
		PC_SourceError(/*handle,*/ "couldn't parse menu item keyword %s", token.string);
		return 0;
	}

	int __cdecl MenuParse_itemDef(menuDef_t* menu/*, int handle*/)
	{
		itemDef_t* item;

		if (menu->itemCount >= MAX_ITEMDEFS_PER_MENUDEF)
		{
			PC_SourceError(/*handle,*/ "too many itemDefs for menu '%s', MAX is %i", menu->window.name, MAX_ITEMDEFS_PER_MENUDEF);
			return 0;
		}
		else
		{
			item = zmem->allocate<itemDef_t>();
			Item_Init(item);
			if (!Item_Parse(/*handle,*/ item))
			{
				//Menu_FreeItemMemory(item);
				return 0;
			}
			//Item_PostParse(item);
			Item_InitControls(item);
			item->parent = menu;
			menu->items[menu->itemCount++] = item;
		}
		return 1;
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

	int Menu_Parse(/*int handle,*/ menuDef_t* menu)
	{
		parse_menudef_func* key;
		pc_token_s token;

		if (!PC_ReadTokenHandle(/*handle,*/ &token))
			return 0;
		if (token.string[0] != '{')
			return 0;
		do
		{
			while (1)
			{
				do
				{
					memset(&token, 0, sizeof(pc_token_s));
					if (!PC_ReadTokenHandle(/*handle,*/ &token))
					{
						PC_SourceError(/*handle,*/ "end of file inside menu");
						return 0;
					}
					if (token.string[0] == '}')
						return 1;
				} while (token.string[0] == ';');
				key = find_menudef_func(token.string);
				if (key)
					break;
				PC_SourceError(/*handle,*/ "unknown menu keyword %s", token.string);
				return 0;
			}
		} while (key->func(menu/*, handle,*/));
		PC_SourceError(/*handle,*/ "couldn't parse menu keyword %s", token.string);
		return 0;
	}

	void Menu_Init(menuDef_t* menu)
	{
		menu->items = zmem->allocate<itemDef_t*>(MAX_ITEMDEFS_PER_MENUDEF);
		menu->data = zmem->allocate<menuData_t>();
		Window_Init(&menu->window);
	}

	bool __cdecl Menu_New(/*int handle,*/)
	{
		bool result;
		menuDef_t* menu;

		menu = zmem->allocate<menuDef_t>();

		Menu_Init(menu);
		if (Menu_Parse(/*handle,*/ menu))
		{
			if (menu->window.name)
			{
				//Menu_PostParse(menu);
				if (menuList->menuCount >= MAX_MENUDEFS_PER_MENULIST)
				{
					PC_SourceError(/*handle,*/ "too many menuDefs in menuList %s, MAX is %i", menuList->name, MAX_MENUDEFS_PER_MENULIST);
					result = 0;
					return result;
				}
				menuList->menus[menuList->menuCount++] = menu;
				result = 1;
			}
			else
			{
				PC_SourceError(/*handle,*/ "menu has no name");
				//Menu_FreeMemory(menu);
				result = 0;
			}
		}
		else
		{
			//Menu_FreeMemory(menu);
			result = 0;
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
			while (PC_ReadTokenHandle(/*handle,*/ &token))
			{
				if (_stricmp(token.string, "}") && _stricmp(token.string, "{"))
				{
					if (_stricmp(token.string, "assetGlobalDef"))
					{
						if (_stricmp(token.string, "menudef"))
						{
							PC_SourceError(
								/*handle,*/
								"Unknown token %s in menu file.  Expected \"menudef\" or \"assetglobaldef\".",
								token.string);
							break;
						}
						else if (!Menu_New(/*handle,*/))
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
			result = 1;
		}
		else
		{
			ZONETOOL_ERROR("Couldn't find menu file '%s'", menuFile);
			result = 0;
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
	{
	}

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

		for (auto i = 0; i < indentCounter && i < 10; i++)
		{
			tabs[i] = '\t';
		}

		tabs[indentCounter] = 0;
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
		for (auto i = 0; i < 102; i++)
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
		// TODO: Some pre-processing may be needed
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
		if (statement)
		{
			const bool clean_statements = false;
			int num_open_parenthesis = 0;

			if (name)
			{
				fprintf(fp, "%s%s", get_tabs(), name);
			}

			if (clean_statements)
			{
				if (statement->entries[0].data.op != OP_LEFTPAREN)
				{
					fputs(" (", fp);
				}
			}

			for (auto i = 0; i < statement->numEntries; i++)
			{
				int type = statement->entries[i].type;
				if (type == OPERATOR)
				{
					auto op = statement->entries[i].data.op;
					if (op < OP_FIRSTFUNCTIONCALL)
					{
						if (statement->entries[i - 1].data.op == OP_RIGHTPAREN && op == OP_LEFTPAREN)
						{
							//++i;
							//continue;
						}

						if (statement->entries[i].data.op == OP_LEFTPAREN)
						{
							++num_open_parenthesis;
						}
						else if (statement->entries[i].data.op == OP_RIGHTPAREN)
						{
							--num_open_parenthesis;
						}

						if (statement->entries[i - 1].data.op == OP_NOT)
						{
							fprintf(fp, "%s", g_expOperatorNames[op]);
						}
						else
						{
							fprintf(fp, " %s", g_expOperatorNames[op]);
						}
					}
					else if (op >= OP_STATICDVARINT && op <= OP_STATICDVARSTRING)
					{
						i++;

						std::string command;
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

						command += "( \"";
						command += statement->supportingData->staticDvarList.staticDvars[statement->entries[i].data.operand.internals.intVal]->dvarName;
						command += "\" )";

						if (statement->entries[i - 2].data.op == OP_NOT)
						{
							fprintf(fp, "%s", command.c_str());
						}
						else
						{
							fprintf(fp, " %s", command.c_str());
						}

						i++;
					}
					else
					{
						if (statement->entries[i - 1].data.op == OP_NOT)
							fprintf(fp, "%s(", g_expOperatorNames[op]);
						else
							fprintf(fp, " %s(", g_expOperatorNames[op]);
						++num_open_parenthesis;
					}
				}
				else if (type == OPERAND)
				{
					switch (statement->entries[i].data.operand.dataType)
					{
					case VAL_INT:
						fprintf(fp, " %i", statement->entries[i].data.operand.internals.intVal);
						break;
					case VAL_FLOAT:
						fprintf(fp, " %g", statement->entries[i].data.operand.internals.floatVal);
						break;
					case VAL_STRING:
						fprintf(fp, " \"%s\"", escape_string(statement->entries[i].data.operand.internals.stringVal.string).c_str());
						break;
					case VAL_FUNCTION:
						emit_statement(nullptr, statement->entries[i].data.operand.internals.function, false, false);
						break;
					}
				}
			}

			if (num_open_parenthesis > 1)
			{
				printf("something is wrong but i fix\n");
			}
			while (num_open_parenthesis > 0)
			{
				fputs(" )", fp);
				num_open_parenthesis -= 1;
			}

			if (clean_statements)
			{
				if (statement->entries[0].data.op != OP_LEFTPAREN)
				{
					fputs(" )", fp);
				}
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
			//const bool formatStatement = true;

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

	void menu_list::dump_menudef(menuDef_t* asset)
	{
		emit_menu_def(asset);
	}

	void menu_list::dump(MenuList* asset)
	{
		const auto path = asset->name;

		auto file = filesystem::file(path);
		file.open("wb");
		fp = file.get_fp();
		if (fp)
		{
			ZONETOOL_INFO("Dumping menu \"%s\"...", asset->name);
			indentCounter = 0;
			push_indent();
			for (int i = 0; i < asset->menuCount; i++)
			{
				dump_menudef(asset->menus[i]);
			}
			pop_indent();
		}

		file.close();
	}
}

#pragma warning( pop ) 