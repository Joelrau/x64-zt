#pragma once
#pragma warning(push)
#pragma warning(disable: 4201)

namespace zonetool::iw5
{
	typedef float vec_t;

	typedef vec_t vec2_t[2];

	typedef vec_t vec3_t[3];

	typedef unsigned __int16 scr_string_t;

	enum XAssetType_old : std::int32_t
	{
		physpreset,
		phys_collmap,
		xanim,
		xmodelsurfs,
		xmodel,
		material,
		pixelshader,
		vertexshader,
		vertexdecl,
		techset,
		image,
		sound,
		sndcurve,
		loaded_sound,
		col_map_mp,
		com_map,
		glass_map,
		ai_paths,
		vehicle_tracks,
		map_ents,
		fx_map,
		gfx_map,
		lightdef,
		ui_map,
		// not used
		font,
		menufile,
		menu,
		localize,
		attachment,
		weapon,
		snddriverglobals,
		// not used
		fx,
		impactfx,
		surfacefx,
		aitype,
		// not used
		mptype,
		// not used
		character,
		// not used
		xmodelalias,
		// not used
		rawfile,
		scriptfile,
		stringtable,
		leaderboarddef,
		structureddatadef,
		tracer,
		vehicle,
		addon_map_ents,
		max,

		col_map_sp = col_map_mp,
	};

	struct ScriptFile
	{
		x86::pointer<const char> name;
		int compressedLen;
		unsigned int len;
		int bytecodeLen;
		x86::pointer<unsigned char> buffer;
		x86::pointer<unsigned char> bytecode;
	};

	struct RawFile
	{
		x86::pointer<const char> name;
		unsigned int compressedLen;
		unsigned int len;
		x86::pointer<char> buffer;
	};

	struct PhysPreset;

	struct PhysCollmap;

	struct XAnimParts;

	struct XModelSurfs;

	struct XModel;

	struct Material;

	struct MaterialPixelShader;

	struct MaterialVertexShader;

	struct MaterialVertexDeclaration;

	struct MaterialTechniqueSet;

	struct GfxImage;

	struct snd_alias_list_t;

	struct SndCurve;

	struct LoadedSound;

	struct clipMap_t;

	struct ComWorld;

	struct GlassWorld;

	struct PathData;

	struct VehicleTrack;

	struct MapEnts;

	struct FxWorld;

	struct GfxWorld;

	struct GfxLightDef;

	struct Font_s;

	struct MenuList;

	struct menuDef_t;

	struct WeaponAttachment;

	struct WeaponCompleteDef;

	struct FxEffectDef;

	struct FxImpactTable;

	struct SurfaceFxTable;

	struct RawFile;

	struct ScriptFile;

	struct StringTable;

	struct LeaderboardDef;

	struct StructuredDataDefSet;

	struct TracerDef;

	struct VehicleDef;

	struct AddonMapEnts;

	struct StringTableCell
	{
		x86::pointer<const char> string;
		int hash;
	};

	struct StringTable
	{
		x86::pointer<const char> name;
		int columnCount;
		int rowCount;
		x86::pointer<StringTableCell> values;
	};

	struct MenuEventHandlerSet;

	struct Statement_s;

	enum EventType
	{
		EVENT_UNCONDITIONAL = 0x0,
		EVENT_IF = 0x1,
		EVENT_ELSE = 0x2,
		EVENT_SET_LOCAL_VAR_BOOL = 0x3,
		EVENT_SET_LOCAL_VAR_INT = 0x4,
		EVENT_SET_LOCAL_VAR_FLOAT = 0x5,
		EVENT_SET_LOCAL_VAR_STRING = 0x6,
		EVENT_COUNT = 0x7,
	};

	enum operationEnum
	{
		OP_NOOP,
		OP_RIGHTPAREN,
		OP_MULTIPLY,
		OP_DIVIDE,
		OP_MODULUS,
		OP_ADD,
		OP_SUBTRACT,
		OP_NOT,
		OP_LESSTHAN,
		OP_LESSTHANEQUALTO,
		OP_GREATERTHAN,
		OP_GREATERTHANEQUALTO,
		OP_EQUALS,
		OP_NOTEQUAL,
		OP_AND,
		OP_OR,
		OP_LEFTPAREN,
		OP_COMMA,
		OP_BITWISEAND,
		OP_BITWISEOR,
		OP_BITWISENOT,
		OP_BITSHIFTLEFT,
		OP_BITSHIFTRIGHT,
		OP_FIRSTFUNCTIONCALL = 0x17,
		OP_STATICDVARINT = 0x17,
		OP_STATICDVARBOOL,
		OP_STATICDVARFLOAT,
		OP_STATICDVARSTRING,
		OP_INT,
		OP_STRING,
		OP_FLOAT,
		OP_SIN,
		OP_COS,
		OP_MIN,
		OP_MAX,
		OP_MILLISECONDS,
		OP_LOCALCLIENTUIMILLISECONDS,
		OP_DVARINT,
		OP_DVARBOOL,
		OP_DVARFLOAT,
		OP_DVARSTRING,
		OP_UI_ACTIVE,
		OP_FLASHBANGED,
		OP_USINGVEHICLE,
		OP_MISSILECAM,
		OP_SCOPED,
		OP_SCOPEDTHERMAL,
		OP_SCOREBOARD_VISIBLE,
		OP_INKILLCAM,
		OP_INKILLCAMNPC,
		OP_PLAYER,
		OP_GETPERK,
		OP_SELECTING_LOCATION,
		OP_SELECTING_DIRECTION,
		OP_TEAM,
		OP_OTHERTEAM,
		OP_MARINESFIELD,
		OP_OPFORFIELD,
		OP_MENUISOPEN,
		OP_WRITINGDATA,
		OP_INLOBBY,
		OP_INGAMELOBBY,
		OP_INPRIVATEPARTY,
		OP_INPRIVATEPARTYHOST,
		OP_PRIVATEPARTYHOSTINLOBBY,
		OP_ALONEINPARTY,
		OP_ADSJAVELIN,
		OP_WEAPLOCKBLINK,
		OP_WEAPATTACKTOP,
		OP_WEAPATTACKDIRECT,
		OP_WEAPLOCKING,
		OP_WEAPLOCKED,
		OP_WEAPLOCKTOOCLOSE,
		OP_WEAPLOCKSCREENPOSX,
		OP_WEAPLOCKSCREENPOSY,
		OP_SECONDSASTIME,
		OP_TABLELOOKUP,
		OP_TABLELOOKUPBYROW,
		OP_TABLEGETROWNUM,
		OP_LOCSTRING,
		OP_LOCALVARINT,
		OP_LOCALVARBOOL,
		OP_LOCALVARFLOAT,
		OP_LOCALVARSTRING,
		OP_TIMELEFT,
		OP_SECONDSASCOUNTDOWN,
		OP_GAMEMSGWNDACTIVE,
		OP_GAMETYPENAME,
		OP_GAMETYPE,
		OP_GAMETYPEDESCRIPTION,
		OP_SCOREATRANK,
		OP_SPECTATINGCLIENT,
		OP_SPECTATINGFREE,
		OP_KEYBINDING,
		OP_ACTIONSLOTUSABLE,
		OP_HUDFADE,
		OP_MAXRECOMMENDEDPLAYERS,
		OP_ACCEPTINGINVITE,
		OP_ISINTERMISSION,
		OP_GAMEHOST,
		OP_PARTYISMISSINGMAPPACK,
		OP_PARTYMISSINGMAPPACKERROR,
		OP_ANYNEWMAPPACKS,
		OP_AMISELECTED,
		OP_PARTYSTATUSSTRING,
		OP_ATTACHEDCONTROLLERCOUNT,
		OP_ISSPLITSCREENONLINEPOSSIBLE,
		OP_SPLITSCREENPLAYERCOUNT,
		OP_GETPLAYERDATA,
		OP_GETPLAYERDATASPLITSCREEN,
		OP_GETMATCHRULESDATA,
		OP_GETSAVEDMATCHRULESMETADATA,
		OP_LEVELFOREXPERIENCEMP,
		OP_LEVELFOREXPERIENCESO,
		OP_ISITEMUNLOCKED,
		OP_ISITEMUNLOCKEDSPLITSCREEN,
		OP_ISCARDICONUNLOCKED,
		OP_ISCARDTITLEUNLOCKED,
		OP_ISCARDICONNEW,
		OP_ISCARDTITLENEW,
		OP_ISCARDICONUNLOCKEDSPLITSCREEN,
		OP_ISCARDTITLEUNLOCKEDSPLITSCREEN,
		OP_ISCARDICONNEWSPLITSCREEN,
		OP_ISCARDTITLENEWSPLITSCREEN,
		OP_ISPROFILEITEMUNLOCKED,
		OP_ISPROFILEITEMUNLOCKEDSPLITSCREEN,
		OP_ISPROFILEITEMNEW,
		OP_ISPROFILEITEMNEWSPLITSCREEN,
		OP_DEBUGPRINT,
		OP_GETPLAYERDATAANYBOOLTRUE,
		OP_GETPROFILEANYBOOLTRUE,
		OP_WEAPONCLASSNEW,
		OP_WEAPONNAME,
		OP_ISRELOADING,
		OP_SAVEGAMEAVAILABLE,
		OP_UNLOCKEDITEMCOUNT,
		OP_UNLOCKEDITEMCOUNTSPLITSCREEN,
		OP_UNLOCKEDITEM,
		OP_UNLOCKEDITEMSPLITSCREEN,
		OP_RADARISJAMMED,
		OP_RADARJAMINTENSITY,
		OP_RADARISENABLED,
		OP_ISEMPJAMMED,
		OP_PLAYERADS,
		OP_WEAPONHEATACTIVE,
		OP_WEAPONHEATVALUE,
		OP_WEAPONHEATOVERHEATED,
		OP_GETSPLASHTEXT,
		OP_GETSPLASHDESCRIPTION,
		OP_GETSPLASHMATERIAL,
		OP_SPLASHHASICON,
		OP_SPLASHROWNUM,
		OP_GETFOCUSEDITEMNAME,
		OP_GETFOCUSEDITEMX,
		OP_GETFOCUSEDITEMY,
		OP_GETFOCUSEDITEMWIDTH,
		OP_GETFOCUSEDITEMHEIGHT,
		OP_GETITEMX,
		OP_GETITEMY,
		OP_GETITEMWIDTH,
		OP_GETITEMHEIGHT,
		OP_PLAYLIST,
		OP_SCOREBOARDEXTERNALMUTENOTICE,
		OP_GETCLIENTMATCHDATA,
		OP_GETCLIENTMATCHDATADEF,
		OP_GETMAPNAME,
		OP_GETMAPIMAGE,
		OP_GETMAPCUSTOM,
		OP_GETMIGRATIONSTATUS,
		OP_GETPLAYERCARDINFO,
		OP_ISOFFLINEPROFILESELECTED,
		OP_COOPPLAYER,
		OP_ISCOOP,
		OP_GETPARTYSTATUS,
		OP_GETSEARCHPARAMS,
		OP_GETTIMEPLAYED,
		OP_ISSELECTEDPLAYERFRIEND,
		OP_GETCHARBYINDEX,
		OP_GETPROFILEDATA,
		OP_GETPROFILEDATASPLITSCREEN,
		OP_ISPROFILESIGNEDIN,
		OP_GETWAITPOPUPSTATUS,
		OP_GETNATTYPE,
		OP_GETLOCALIZEDNATTYPE,
		OP_GETADJUSTEDSAFEAREAHORIZONTAL,
		OP_GETADJUSTEDSAFEAREAVERTICAL,
		OP_CONNECTIONINFO,
		OP_OFFLINEPROFILECANSAVE,
		OP_USERWITHOUTOFFLINEPROFILEWARNING,
		OP_ALLSPLITSCREENPROFILESCANSAVE,
		OP_ALLSPLITSCREENPROFILESARESIGNEDIN,
		OP_DOWEHAVEMAPPACK,
		OP_MAYINVITEPLAYERTOPARTY,
		OP_GETPATCHNOTES,
		OP_GETGAMEINFOS,
		OP_COOPREADY,
		OP_VOTECAST,
		OP_VOTEPASSED,
		OP_GETMAPVOTEMAPIMAGE,
		OP_GETMAPVOTEMAPNAME,
		OP_MAPVOTEGAMETYPENAME,
		OP_ISFRIENDINVITABLE,
		OP_ISFRIENDJOINABLE,
		OP_GETSORTEDCHALLENGEINDEX,
		OP_GETSORTEDCHALLENGENAME,
		OP_GETSORTEDCHALLENGECOUNT,
		OP_GETFILTERCHALLENGECOUNT,
		OP_GETFILTERCHALLENGELOCKEDCOUNT,
		OP_GETFILTERCHALLENGECOMPLETECOUNT,
		OP_ISSORTEDCHALLENGETIERED,
		OP_GETCHALLENGEFILTERCACHECOUNT,
		OP_GETCHALLENGEFILTERCACHECOMPLETECOUNT,
		OP_ISCOOPSEARCHING,
		OP_COOPSHOWPUBLICTYPE,
		OP_COOPDISPLAYABLEGROUPNUM,
		OP_COOPHASREQUIREDONLINEFILES,
		OP_GETTEXTWIDTH,
		OP_GETTEXTHEIGHT,
		OP_ISDEVELOPER,
		OP_ISUSINGAIRBURST,
		OP_GETAIRBURSTMETERS,
		OP_GETCROSSHAIRTRACEMETERS,
		OP_GETFACEBOOKSTATUSTEXT,
		OP_ISFACEBOOKLOGGEDIN,
		OP_ISFACEBOOKCHECKING,
		OP_ISFACEBOOKALLOWED,
		OP_GETPRIVATEPARTYSTATUS,
		OP_INCLUDEDINMAPROTATION,
		OP_SELECT,
		OP_ISDEMOPLAYING,
		OP_GETUSERGROUPTEXT,
		OP_GETUSERGROUPCOMMONINTERESTTOTAL,
		OP_ISDEMOFOLLOWCAMERA,
		OP_ISDEMOFREECAMERA,
		OP_ISDEMOCAPTURINGSCREENSHOT,
		OP_ISPARTYHOSTWAITINGONMEMBERS,
		OP_ISPOPUPPARTYMEMBERAWAY,
		OP_ISSELECTEDPARTYMEMBERAWAY,
		OP_GETTIME,
		OP_GAMEENDTIME,
		OP_HASFOCUS,
		OP_MENUHASFOCUS,
		OP_GETDEMOSEGMENTCOUNT,
		OP_GETDEMOSEGMENTINFORMATION,
		OP_ISCLIPMODIFIED,
		OP_ISUSINGMATCHRULESDATA,
		OP_ISGUEST,
		OP_GETFACEBOOKHELPTEXT,
		OP_ISELITECLANALLOWED,
		OP_ISENTITLEMENTSALLOWED,
		OP_ISUSERGROUPSALLOWED,
		OP_ISWAITINGFORONLINESERVICES,
		OP_GETTEXTWIDTHMODCASE,
		OP_GETSAVESCREENTITLE,
		OP_GETSAVESCREENDESCRIPTION,
		OP_GETONLINEVAULTSELECTEDITEMDATA,
		OP_ISONLINEVAULTRESTRICTED,
		OP_ISCONTENTSERVERTASKINPROGRESS,
		OP_GETCONTENTSERVERTASKPROGRESS,
		OP_GETRECENTGAMESSELECTEDITEMDATA,
		OP_GAMETYPENAMEABBREVIATED,
		OP_MAPVOTEGAMETYPENAMEABBREVIATED,
		OP_ISUSERSIGNEDINTOLIVE,
		OP_USERCANPLAYONLINE,
		OP_GETPASTTITLERANK,
		OP_GETFEEDERDATA,
		OP_PARTYCLIENTSUPTODATE,
		OP_TRUNCATETEXTWITHELLIPSIS,
		OP_UISTARTED,
		OP_CANRENDERCLIP,
		OP_GETPREVIEWMAPCUSTOM,
		OP_GETDLCMAPSAVAILABLECOUNT,
		OP_ISUSERSIGNEDIN,
		OP_ISUSINGINTERMISSIONTIMER,
		OP_ISUSINGCUSTOMMAPROTATION,
		OP_MENUISTOPMOST,
		OP_FACEBOOK_ISPLATFROMFRIEND,
		OP_ELITECLAN_ISPLATFROMFRIEND,
		OP_ELITECLAN_ISME,
		OP_ELITECLAN_ISLEADER,
		OP_ISUSERSIGNEDINFORVAULT,
		OP_GETUSINGMATCHRULESDATA,
		OP_CANUSERACCESSONLINEVAULT,
		OP_FRIEND_GETGAMERTAG,
		OP_RECENTPLAYER_GETGAMERTAG,
		OP_LIVEPARTY_GETGAMERTAG,
		OP_FACEBOOK_GETGAMERTAG,
		OP_ELITECLAN_GETGAMERTAG,
		OP_LIVEPARTY_ISME,
		OP_LIVEPARTY_ISLOCAL,
		OP_DOUBLECLICKWASRIGHTCLICK,
		OP_ISDEMOCLIPRECORDING,
		OP_GETINDEXFROMSTRING,
		OP_GETSTRINGWITHOUTINDEX,
		OP_ELITECLAN_GETNAME,
		OP_ELITECLAN_GETHELP,
		OP_ELITECLAN_GETMOTD,
		OP_ELITECLAN_ISMEMBER,
		OP_ELITECLAN_ISEMBLEM_OK,
		OP_FACEBOOK_FRIENDS_SHOW_NEXT,
		OP_FACEBOOK_FRIENDS_SHOW_PREV,
		OP_GETONLINEVAULTFRIENDGAMERTAG,
		OP_GETOBJECTIVELISTHEIGHT,
		OP_ISCLIENTDEMOENABLED,
		OP_ISUSERSIGNEDINTODEMONWARE,
		OP_CUSTOMCLASSISRESTRICTED,
		OP_WEAPONISRESTRICTED,
		OP_ANYSPLITSCREENPROFILESARESIGNEDIN,
		OP_ISGUESTSPLITSCREEN,
		OP_ISITEMUNLOCKEDBYCLIENT,
		OP_ISANYUSERSIGNEDINTOLIVE,
		OP_GETPASTTITLEPRESTIGE,
		OP_SPLITSCREENACTIVEGAMEPADCOUNT,
		OP_SHOWFRIENDPLAYERCARD,
		OP_GETFRIENDPLAYERCARDPRESENCE,
		OP_SHOWRECENTPLAYERGROUPICON,
		OP_GETWRAPPEDTEXTHEIGHT,
		OP_CANCLIENTSAVE,
		OP_GETGAMEINVITESCOUNT,
		OP_ISSPLITSCREENGAMERLIVEENABLED,
		OP_SO_COOPSHOWCOMMONGROUPICON,
		OP_STRIPCOLORSFROMSTRING,
		OP_DEPRECATED,
		OP_ISCHALLENGEPERIODIC,
		OP_GETCHALLENGEDATA,
		OP_ISELITEAPPPRESENT,
		OP_ELITECLAN_SELECTEDISME,
		OP_ENOUGHSTORAGESPACEFORCLIENTDEMO,
		OP_ISUSERSIGNEDINFORCOMMERCE,
		OP_GETFACEBOOKMENUTEXT,
		OP_GETFACEBOOKISPOSTING,
		OP_MEETPLAYER_ISPLATFORMFRIEND,
		OP_ISSELECTEDPLAYERGUEST,
		OP_GETSPLITSCREENCONTROLLERCLIENTNUM,
		OP_ISCLIENTDEMOENABLEDSPLITSCREEN,
		OP_ITEMCANTAKEFOCUS,
		OP_GETTIMESINCELASTDOUBLECLICK,
		OP_ISSERVERLISTREFRESHING,
		OP_ISRECIPENAMEVALID,
		OP_RECIPEEXISTS,
		OP_GETFACEBOOKOPTIONSHELPTEXT,
		OP_DOWEHAVEALLAVAILABLEMAPPACKS,
		OP_ISTHERENEWELITEITEMS,
		OP_ISPAYINGSUBSCRIBER,
		OP_LOCALUSER_ISMISSINGMAPPACK,
		OP_LOCALUSER_MISSINGMAPPACKERROR,
		OP_GETFIRSTSPECOPSDLCMAP,
		OP_LOCALUSER_MISSINGMAPNAME,
		OP_SHOWSTORENEW,
		OP_COMMERCE_GETSTATUS,
		OP_ISMANIFESTDOWNLOADED,
		OP_AREALLITEMSUNLOCKED,
		OP_COMMERCE_GETSTATUSCOND,
		OP_DOWEHAVEMISSINGOWNEDCONTENT
	};

	struct UIFunctionList
	{
		int totalFunctions;
		x86::pointer<x86::pointer<Statement_s>> functions;
	};

	struct StaticDvar
	{
		x86::pointer<void> dvar;
		x86::pointer<char> dvarName;
	};

	struct StaticDvarList
	{
		int numStaticDvars;
		x86::pointer<x86::pointer<StaticDvar>> staticDvars;
	};

	struct StringList
	{
		int totalStrings;
		x86::pointer<x86::pointer<const char>> strings;
	};

	struct ExpressionSupportingData
	{
		UIFunctionList uifunctions;
		StaticDvarList staticDvarList;
		StringList uiStrings;
	};

	enum expDataType
	{
		VAL_INT = 0x0,
		VAL_FLOAT = 0x1,
		VAL_STRING = 0x2,
		NUM_INTERNAL_DATATYPES = 0x3,
		VAL_FUNCTION = 0x3,
		NUM_DATATYPES = 0x4,
	};

	struct ExpressionString
	{
		x86::pointer<const char> string;
	};

	union operandInternalDataUnion
	{
		int intVal;
		float floatVal;
		ExpressionString stringVal;
		x86::pointer<Statement_s> function;
	};

	struct Operand
	{
		expDataType dataType;
		operandInternalDataUnion internals;
	};

	union entryInternalData
	{
		operationEnum op;
		Operand operand;
	};

	struct expressionEntry
	{
		int type;
		entryInternalData data;
	};

	struct ExpressionPersistentState
	{
		int flags;
		int playerDataKey[4];
		int lastExecuteTime[4];
		Operand lastResult[4];
	};

	struct Statement_s
	{
		int numEntries;
		x86::pointer<expressionEntry> entries;
		x86::pointer<ExpressionSupportingData> supportingData;
		ExpressionPersistentState persistentState;
	};

	struct SetLocalVarData
	{
		x86::pointer<const char> localVarName;
		x86::pointer<Statement_s> expression;
	};

	struct ConditionalScript
	{
		x86::pointer<MenuEventHandlerSet> eventHandlerSet;
		x86::pointer<Statement_s> eventExpression; // loads this first
	};

	union EventData
	{
		x86::pointer<const char> unconditionalScript;
		x86::pointer<ConditionalScript> conditionalScript;
		x86::pointer<MenuEventHandlerSet> elseScript;
		x86::pointer<SetLocalVarData> setLocalVarData;
	};

	struct MenuEventHandler
	{
		EventData eventData;
		EventType eventType;
	};

	struct MenuEventHandlerSet
	{
		int eventHandlerCount;
		x86::pointer<x86::pointer<MenuEventHandler>> eventHandlers;
	};

	struct ItemKeyHandler
	{
		int key;
		x86::pointer<MenuEventHandlerSet> action;
		x86::pointer<ItemKeyHandler> next;
	};

	struct rectDef_s
	{
		float x;
		float y;
		float w;
		float h;
		char horzAlign;
		char vertAlign;
	};

	struct windowDef_t
	{
		x86::pointer<const char> name;
		rectDef_s rect;
		rectDef_s rectClient;
		x86::pointer<const char> group;
		int style;
		int border;
		int ownerDraw;
		int ownerDrawFlags;
		float borderSize;
		int staticFlags;
		int dynamicFlags;
		int nextTime;
		float foreColor[4];
		float backColor[4];
		float borderColor[4];
		float outlineColor[4];
		float disableColor[4];
		x86::pointer<Material> background;
	};

	enum ItemFloatExpressionTarget
	{
		ITEM_FLOATEXP_TGT_RECT_X = 0x0,
		ITEM_FLOATEXP_TGT_RECT_Y = 0x1,
		ITEM_FLOATEXP_TGT_RECT_W = 0x2,
		ITEM_FLOATEXP_TGT_RECT_H = 0x3,
		ITEM_FLOATEXP_TGT_FORECOLOR_R = 0x4,
		ITEM_FLOATEXP_TGT_FORECOLOR_G = 0x5,
		ITEM_FLOATEXP_TGT_FORECOLOR_B = 0x6,
		ITEM_FLOATEXP_TGT_FORECOLOR_RGB = 0x7,
		ITEM_FLOATEXP_TGT_FORECOLOR_A = 0x8,
		ITEM_FLOATEXP_TGT_GLOWCOLOR_R = 0x9,
		ITEM_FLOATEXP_TGT_GLOWCOLOR_G = 0xA,
		ITEM_FLOATEXP_TGT_GLOWCOLOR_B = 0xB,
		ITEM_FLOATEXP_TGT_GLOWCOLOR_RGB = 0xC,
		ITEM_FLOATEXP_TGT_GLOWCOLOR_A = 0xD,
		ITEM_FLOATEXP_TGT_BACKCOLOR_R = 0xE,
		ITEM_FLOATEXP_TGT_BACKCOLOR_G = 0xF,
		ITEM_FLOATEXP_TGT_BACKCOLOR_B = 0x10,
		ITEM_FLOATEXP_TGT_BACKCOLOR_RGB = 0x11,
		ITEM_FLOATEXP_TGT_BACKCOLOR_A = 0x12,
		ITEM_FLOATEXP_TGT__COUNT = 0x13,
	};

	struct ItemFloatExpression
	{
		ItemFloatExpressionTarget target;
		x86::pointer<Statement_s> expression;
	};

	struct editFieldDef_s
	{
		float minVal;
		float maxVal;
		float stepVal;
		float range;
		int maxChars;
		int maxCharsGotoNext;
		int maxPaintChars;
		int paintOffset;
	};

	struct multiDef_s
	{
		x86::pointer<const char> dvarList[32];
		x86::pointer<const char> dvarStr[32];
		float dvarValue[32];
		int count;
		int strDef;
	};

	struct columnInfo_s
	{
		int xpos;
		int ypos;
		int width;
		int height;
		int maxChars;
		int alignment;
	};

	struct listBoxDef_s
	{
		int mousePos; // is this correct?
		int startPos[1];
		int endPos[1];
		int drawPadding;
		float elementWidth;
		float elementHeight;
		int elementStyle;
		int numColumns;
		columnInfo_s columnInfo[16];
		x86::pointer<MenuEventHandlerSet> onDoubleClick;
		int notselectable;
		int noScrollBars;
		int usePaging;
		float selectBorder[4];
		x86::pointer<Material> selectIcon;
		x86::pointer<Statement_s> elementHeightExp;
	};

	struct newsTickerDef_s
	{
		int feedId;
		int speed;
		int spacing;
	};

	struct textScrollDef_s
	{
		int startTime;
	};

	union itemDefData_t
	{
		x86::pointer<listBoxDef_s> listBox;
		x86::pointer<editFieldDef_s> editField;
		x86::pointer<multiDef_s> multi;
		x86::pointer<const char> enumDvarName;
		x86::pointer<newsTickerDef_s> ticker;
		x86::pointer<textScrollDef_s> scroll;
		x86::pointer<void> data;
	};

	struct itemDef_t
	{
		windowDef_t window;
		rectDef_s textRect;
		int type;
		int dataType;
		int alignment;
		int fontEnum;
		int textAlignMode;

		union
		{
			float floatArray1[3];

			struct
			{
				float textAlignX;
				float textAlignY;
				float textScale;
			};
		};

		int textStyle;
		int gameMsgWindowIndex;
		int gameMsgWindowMode;
		x86::pointer<const char> text;
		int itemFlags;
		x86::pointer<menuDef_t> parent;
		x86::pointer<MenuEventHandlerSet> mouseEnterText;
		x86::pointer<MenuEventHandlerSet> mouseExitText;
		x86::pointer<MenuEventHandlerSet> mouseEnter;
		x86::pointer<MenuEventHandlerSet> mouseExit;
		x86::pointer<MenuEventHandlerSet> action;
		x86::pointer<MenuEventHandlerSet> accept;
		x86::pointer<MenuEventHandlerSet> onFocus;
		x86::pointer<MenuEventHandlerSet> hasFocus;
		x86::pointer<MenuEventHandlerSet> leaveFocus;
		x86::pointer<const char> dvar;
		x86::pointer<const char> dvarTest;
		x86::pointer<ItemKeyHandler> onKey;
		x86::pointer<const char> enableDvar;
		x86::pointer<const char> localVar;
		int dvarFlags;
		x86::pointer<snd_alias_list_t> focusSound;
		float special;
		int cursorPos;
		itemDefData_t typeData;
		int floatExpressionCount;
		x86::pointer<ItemFloatExpression> floatExpressions;
		x86::pointer<Statement_s> visibleExp;
		x86::pointer<Statement_s> disabledExp;
		x86::pointer<Statement_s> textExp;
		x86::pointer<Statement_s> materialExp;
		float glowColor[4];
		bool decayActive;
		int fxBirthTime;
		int fxLetterTime;
		int fxDecayStartTime;
		int fxDecayDuration;
		int lastSoundPlayedTime;
		x86::pointer<Statement_s> textAlignYExp;
	};

	struct menuTransition
	{
		int transitionType;
		int startTime;
		float startVal;
		float endVal;
		float time;
		int endTriggerType;
	};

	struct menuData_t
	{
		int fullScreen;
		int fadeCycle;

		union
		{
			float floatArray1[4];

			struct
			{
				float fadeClamp;
				float fadeAmount;
				float fadeInAmount;
				float blurWorld;
			};
		};

		x86::pointer<MenuEventHandlerSet> onOpen;
		x86::pointer<MenuEventHandlerSet> onCloseRequest;
		x86::pointer<MenuEventHandlerSet> onClose;
		x86::pointer<MenuEventHandlerSet> onESC;
		x86::pointer<MenuEventHandlerSet> onFocusDueToClose;
		x86::pointer<ItemKeyHandler> onKey;
		x86::pointer<Statement_s> visibleExp;
		x86::pointer<const char> allowedBinding;
		x86::pointer<const char> soundName;
		float focusColor[4];
		x86::pointer<Statement_s> rectXExp;
		x86::pointer<Statement_s> rectYExp;
		x86::pointer<Statement_s> rectWExp;
		x86::pointer<Statement_s> rectHExp;
		x86::pointer<Statement_s> openSoundExp;
		x86::pointer<Statement_s> closeSoundExp;
		x86::pointer<Statement_s> soundLoopExp;
		int cursorItem;
		menuTransition scaleTransition;
		menuTransition alphaTransition;
		menuTransition xTransition;
		menuTransition yTransition;
		x86::pointer<ExpressionSupportingData> expressionData;
		char priority;
	};

	struct menuDef_t
	{
		x86::pointer<menuData_t> data;
		windowDef_t window;
		int itemCount;
		x86::pointer<x86::pointer<itemDef_t>> items;
	};

	struct MenuList
	{
		x86::pointer<const char> name;
		int menuCount;
		x86::pointer<x86::pointer<menuDef_t>> menus;
	};

	struct TracerDef
	{
		x86::pointer<const char> name;
		x86::pointer<Material> material;
		unsigned int drawInterval;
		float speed;
		float beamLength;
		float beamWidth;
		float screwRadius;
		float screwDist;
		float colors[5][4];
	};

	enum AttachmentType
	{
		ATTACHMENT_SCOPE = 0x0,
		ATTACHMENT_UNDERBARREL = 0x1,
		ATTACHMENT_OTHER = 0x2,

		ATTACHMENT_COUNT
	};

	enum weapType_t
	{
		WEAPTYPE_NONE = 0x0,
		WEAPTYPE_BULLET = 0x1,
		WEAPTYPE_GRENADE = 0x2,
		WEAPTYPE_PROJECTILE = 0x3,
		WEAPTYPE_RIOTSHIELD = 0x4,

		WEAPTYPE_COUNT
	};

	enum weapClass_t
	{
		WEAPCLASS_RIFLE = 0x0,
		WEAPCLASS_SNIPER = 0x1,
		WEAPCLASS_MG = 0x2,
		WEAPCLASS_SMG = 0x3,
		WEAPCLASS_SPREAD = 0x4,
		WEAPCLASS_PISTOL = 0x5,
		WEAPCLASS_GRENADE = 0x6,
		WEAPCLASS_ROCKETLAUNCHER = 0x7,
		WEAPCLASS_TURRET = 0x8,
		WEAPCLASS_THROWINGKNIFE = 0x9,
		WEAPCLASS_NON_PLAYER = 0xA,
		WEAPCLASS_ITEM = 0xB,

		WEAPCLASS_COUNT
	};

	enum PenetrateType
	{
		PENETRATE_TYPE_NONE = 0x0,
		PENETRATE_TYPE_SMALL = 0x1,
		PENETRATE_TYPE_MEDIUM = 0x2,
		PENETRATE_TYPE_LARGE = 0x3,

		PENETRATE_TYPE_COUNT
	};

	enum ImpactType
	{
		IMPACT_TYPE_NONE = 0x0,
		IMPACT_TYPE_BULLET_SMALL = 0x1,
		IMPACT_TYPE_BULLET_LARGE = 0x2,
		IMPACT_TYPE_BULLET_AP = 0x3,
		IMPACT_TYPE_BULLET_EXPLODE = 0x4,
		IMPACT_TYPE_SHOTGUN = 0x5,
		IMPACT_TYPE_SHOTGUN_EXPLODE = 0x6,
		IMPACT_TYPE_GRENADE_BOUNCE = 0x7,
		IMPACT_TYPE_GRENADE_EXPLODE = 0x8,
		IMPACT_TYPE_ROCKET_EXPLODE = 0x9,
		IMPACT_TYPE_PROJECTILE_DUD = 0xA,

		IMPACT_TYPE_COUNT
	};

	enum weapFireType_t
	{
		WEAPON_FIRETYPE_FULLAUTO = 0x0,
		WEAPON_FIRETYPE_SINGLESHOT = 0x1,
		WEAPON_FIRETYPE_BURSTFIRE2 = 0x2,
		WEAPON_FIRETYPE_BURSTFIRE3 = 0x3,
		WEAPON_FIRETYPE_BURSTFIRE4 = 0x4,
		WEAPON_FIRETYPE_DOUBLEBARREL = 0x5,

		WEAPON_FIRETYPE_COUNT
	};

	struct AttAmmoGeneral
	{
		PenetrateType penetrateType;
		float penetrateMultiplier;
		ImpactType impactType;
		weapFireType_t fireType;
		x86::pointer<TracerDef> tracerType;
		bool rifleBullet;
		bool armorPiercing;
	};

	struct AttSight
	{
		bool aimDownSight;
		bool adsFire;
		bool rechamberWhileAds;
		bool noAdsWhenMagEmpty;
		bool canHoldBreath;
		bool canVariableZoom;
		bool hideRailWithThisScope;
	};

	struct AttReload
	{
		bool noPartialReload;
		bool segmentedReload;
	};

	struct AttAddOns
	{
		bool motionTracker;
		bool silenced;
	};

	struct AttGeneral
	{
		bool boltAction;
		bool inheritsPerks;
		float enemyCrosshairRange;
		x86::pointer<Material> reticleCenter;
		x86::pointer<Material> reticleSide;
		int reticleCenterSize;
		int reticleSideSize;
		float moveSpeedScale;
		float adsMoveSpeedScale;
	};

	struct AttAimAssist
	{
		float autoAimRange;
		float aimAssistRange;
		float aimAssistRangeAds;
	};

	struct AttAmmunition
	{
		int maxAmmo;
		int startAmmo;
		int clipSize;
		int shotCount;
		int reloadAmmoAdd;
		int reloadStartAdd;
	};

	struct AttDamage
	{
		int damage;
		int minDamage;
		int meleeDamage;
		float maxDamageRange;
		float minDamageRange;
		int playerDamage;
		int minPlayerDamage;
	};

	struct AttLocationDamage
	{
		float locNone;
		float locHelmet;
		float locHead;
		float locNeck;
		float locTorsoUpper;
		float locTorsoLower;
		float locRightArmUpper;
		float locRightArmLower;
		float locRightHand;
		float locLeftArmUpper;
		float locLeftArmLower;
		float locLeftHand;
		float locRightLegUpper;
		float locRightLegLower;
		float locRightFoot;
		float locLeftLegUpper;
		float locLeftLegLower;
		float locLeftFoot;
		float locGun;
	};

	struct AttIdleSettings
	{
		float hipIdleAmount;
		float hipIdleSpeed;
		float idleCrouchFactor;
		float idleProneFactor;
		float adsIdleLerpStartTime;
		float adsIdleLerpTime;
	};

	struct AttADSSettings
	{
		float adsSpread;
		float adsAimPitch;
		float adsTransInTime;
		float adsTransOutTime;
		int adsReloadTransTime;
		float adsCrosshairInFrac;
		float adsCrosshairOutFrac;
		float adsZoomFov;
		float adsZoomInFrac;
		float adsZoomOutFrac;
		float adsBobFactor;
		float adsViewBobMult;
		float adsViewErrorMin;
		float adsViewErrorMax;
	};

	struct AttHipSpread
	{
		float hipSpreadStandMin;
		float hipSpreadDuckedMin;
		float hipSpreadProneMin;
		float hipSpreadMax;
		float hipSpreadDuckedMax;
		float hipSpreadProneMax;
		float hipSpreadFireAdd;
		float hipSpreadTurnAdd;
		float hipSpreadMoveAdd;
		float hipSpreadDecayRate;
		float hipSpreadDuckedDecay;
		float hipSpreadProneDecay;
	};

	struct AttGunKick
	{
		int hipGunKickReducedKickBullets;
		float hipGunKickReducedKickPercent;
		float hipGunKickPitchMin;
		float hipGunKickPitchMax;
		float hipGunKickYawMin;
		float hipGunKickYawMax;
		float hipGunKickAccel;
		float hipGunKickSpeedMax;
		float hipGunKickSpeedDecay;
		float hipGunKickStaticDecay;
		int adsGunKickReducedKickBullets;
		float adsGunKickReducedKickPercent;
		float adsGunKickPitchMin;
		float adsGunKickPitchMax;
		float adsGunKickYawMin;
		float adsGunKickYawMax;
		float adsGunKickAccel;
		float adsGunKickSpeedMax;
		float adsGunKickSpeedDecay;
		float adsGunKickStaticDecay;
	};

	struct AttViewKick
	{
		float hipViewKickPitchMin;
		float hipViewKickPitchMax;
		float hipViewKickYawMin;
		float hipViewKickYawMax;
		float hipViewKickCenterSpeed;
		float adsViewKickPitchMin;
		float adsViewKickPitchMax;
		float adsViewKickYawMin;
		float adsViewKickYawMax;
		float adsViewKickCenterSpeed;
	};

	enum weapOverlayReticle_t
	{
		WEAPOVERLAYRETICLE_NONE = 0x0,
		WEAPOVERLAYRETICLE_CROSSHAIR = 0x1,

		WEAPOVERLAYRETICLE_COUNT
	};

	struct ADSOverlay
	{
		x86::pointer<Material> shader;
		x86::pointer<Material> shaderLowRes;
		x86::pointer<Material> shaderEMP;
		x86::pointer<Material> shaderEMPLowRes;
		weapOverlayReticle_t reticle;
		float width;
		float height;
		float widthSplitscreen;
		float heightSplitscreen;
	};

	struct AttADSOverlay
	{
		ADSOverlay overlay;
		bool thermalScope;
	};

	enum weaponIconRatioType_t
	{
		WEAPON_ICON_RATIO_1TO1 = 0x0,
		WEAPON_ICON_RATIO_2TO1 = 0x1,
		WEAPON_ICON_RATIO_4TO1 = 0x2,

		WEAPON_ICON_RATIO_COUNT
	};

	enum ammoCounterClipType_t
	{
		AMMO_COUNTER_CLIP_NONE = 0x0,
		AMMO_COUNTER_CLIP_MAGAZINE = 0x1,
		AMMO_COUNTER_CLIP_SHORTMAGAZINE = 0x2,
		AMMO_COUNTER_CLIP_SHOTGUN = 0x3,
		AMMO_COUNTER_CLIP_ROCKET = 0x4,
		AMMO_COUNTER_CLIP_BELTFED = 0x5,
		AMMO_COUNTER_CLIP_ALTWEAPON = 0x6,

		AMMO_COUNTER_CLIP_COUNT
	};

	struct AttUI
	{
		x86::pointer<Material> dpadIcon;
		x86::pointer<Material> ammoCounterIcon;
		weaponIconRatioType_t dpadIconRatio;
		weaponIconRatioType_t ammoCounterIconRatio;
		ammoCounterClipType_t ammoCounterClip;
	};

	struct AttRumbles
	{
		x86::pointer<const char> fireRumble;
		x86::pointer<const char> meleeImpactRumble;
	};

	enum weapProjExposion_t
	{
		WEAPPROJEXP_GRENADE = 0x0,
		WEAPPROJEXP_ROCKET = 0x1,
		WEAPPROJEXP_FLASHBANG = 0x2,
		WEAPPROJEXP_NONE = 0x3,
		WEAPPROJEXP_DUD = 0x4,
		WEAPPROJEXP_SMOKE = 0x5,
		WEAPPROJEXP_HEAVY = 0x6,

		WEAPPROJEXP_COUNT
	};

	struct snd_alias_list_name
	{
		x86::pointer<const char> soundName;
	};

	union SndAliasCustom
	{
		x86::pointer<snd_alias_list_name> name;
		x86::pointer<snd_alias_list_t> sound;
	};

	struct AttProjectile
	{
		int explosionRadius;
		int explosionInnerDamage;
		int explosionOuterDamage;
		float damageConeAngle;
		int projectileSpeed;
		int projectileSpeedUp;
		int projectileActivateDist;
		float projectileLifetime;
		x86::pointer<XModel> projectileModel;
		weapProjExposion_t projExplosionType;
		x86::pointer<FxEffectDef> projExplosionEffect;
		bool projExplosionEffectForceNormalUp;
		SndAliasCustom projExplosionSound;
		x86::pointer<FxEffectDef> projDudEffect;
		SndAliasCustom projDudSound;
		bool projImpactExplode;
		float destabilizationRateTime;
		float destabilizationCurvatureMax;
		int destabilizeDistance;
		x86::pointer<FxEffectDef> projTrailEffect;
		int projIgnitionDelay;
		x86::pointer<FxEffectDef> projIgnitionEffect;
		SndAliasCustom projIgnitionSound;
	};

	typedef __declspec(align(4)) AttSight AttSight4;

	struct WeaponAttachment
	{
		x86::pointer<const char> szInternalName;
		x86::pointer<const char> szDisplayName;
		AttachmentType type;
		weapType_t weaponType;
		weapClass_t weapClass;
		x86::pointer<x86::pointer<XModel>> worldModels;
		x86::pointer<x86::pointer<XModel>> viewModels;
		x86::pointer<x86::pointer<XModel>> reticleViewModels;
		x86::pointer<AttAmmoGeneral> ammoGeneral;
		x86::pointer<AttSight4> sight;
		x86::pointer<AttReload> reload;
		x86::pointer<AttAddOns> addOns;
		x86::pointer<AttGeneral> general;
		x86::pointer<AttAimAssist> aimAssist;
		x86::pointer<AttAmmunition> ammunition;
		x86::pointer<AttDamage> damage;
		x86::pointer<AttLocationDamage> locationDamage;
		x86::pointer<AttIdleSettings> idleSettings;
		x86::pointer<AttADSSettings> adsSettings;
		x86::pointer<AttADSSettings> adsSettingsMain;
		x86::pointer<AttHipSpread> hipSpread;
		x86::pointer<AttGunKick> gunKick;
		x86::pointer<AttViewKick> viewKick;
		x86::pointer<AttADSOverlay> adsOverlay;
		x86::pointer<AttUI> ui;
		x86::pointer<AttRumbles> rumbles;
		x86::pointer<AttProjectile> projectile;
		float ammunitionScale;
		float damageScale;
		float damageScaleMin;
		float stateTimersScale;
		float fireTimersScale;
		float idleSettingsScale;
		float adsSettingsScale;
		float adsSettingsScaleMain;
		float hipSpreadScale;
		float gunKickScale;
		float viewKickScale;
		float viewCenterScale;
		int loadIndex;
		bool hideIronSightsWithThisAttachment;
		bool shareAmmoWithAlt;
	};

	enum weapInventoryType_t
	{
		WEAPINVENTORY_PRIMARY = 0x0,
		WEAPINVENTORY_OFFHAND = 0x1,
		WEAPINVENTORY_ITEM = 0x2,
		WEAPINVENTORY_ALTMODE = 0x3,
		WEAPINVENTORY_EXCLUSIVE = 0x4,
		WEAPINVENTORY_SCAVENGER = 0x5,

		WEAPINVENTORY_COUNT
	};

	enum OffhandClass
	{
		OFFHAND_CLASS_NONE = 0x0,
		OFFHAND_CLASS_FRAG_GRENADE = 0x1,
		OFFHAND_CLASS_SMOKE_GRENADE = 0x2,
		OFFHAND_CLASS_FLASH_GRENADE = 0x3,
		OFFHAND_CLASS_THROWINGKNIFE = 0x4,
		OFFHAND_CLASS_OTHER = 0x5,

		OFFHAND_CLASS_COUNT
	};

	enum weapStance_t
	{
		WEAPSTANCE_STAND = 0x0,
		WEAPSTANCE_DUCK = 0x1,
		WEAPSTANCE_PRONE = 0x2,

		WEAPSTANCE_COUNT
	};

	enum activeReticleType_t
	{
		VEH_ACTIVE_RETICLE_NONE = 0x0,
		VEH_ACTIVE_RETICLE_PIP_ON_A_STICK = 0x1,
		VEH_ACTIVE_RETICLE_BOUNCING_DIAMOND = 0x2,

		VEH_ACTIVE_RETICLE_COUNT
	};

	struct StateTimers
	{
		int iFireDelay;
		int iMeleeDelay;
		int meleeChargeDelay;
		int iDetonateDelay;
		int iRechamberTime;
		int rechamberTimeOneHanded;
		int iRechamberBoltTime;
		int iHoldFireTime;
		int iDetonateTime;
		int iMeleeTime;
		int meleeChargeTime;
		int iReloadTime;
		int reloadShowRocketTime;
		int iReloadEmptyTime;
		int iReloadAddTime;
		int iReloadStartTime;
		int iReloadStartAddTime;
		int iReloadEndTime;
		int iDropTime;
		int iRaiseTime;
		int iAltDropTime;
		int quickDropTime;
		int quickRaiseTime;
		int iBreachRaiseTime;
		int iEmptyRaiseTime;
		int iEmptyDropTime;
		int sprintInTime;
		int sprintLoopTime;
		int sprintOutTime;
		int stunnedTimeBegin;
		int stunnedTimeLoop;
		int stunnedTimeEnd;
		int nightVisionWearTime;
		int nightVisionWearTimeFadeOutEnd;
		int nightVisionWearTimePowerUp;
		int nightVisionRemoveTime;
		int nightVisionRemoveTimePowerDown;
		int nightVisionRemoveTimeFadeInStart;
		int fuseTime;
		int aiFuseTime;
		int blastFrontTime;
		int blastRightTime;
		int blastBackTime;
		int blastLeftTime;
		int raiseInterruptableTime;
		int firstRaiseInterruptableTime;
		int reloadInterruptableTime;
		int reloadEmptyInterruptableTime;
		int fireInterruptableTime;
	};

	enum WeapOverlayInterface_t
	{
		WEAPOVERLAYINTERFACE_NONE = 0x0,
		WEAPOVERLAYINTERFACE_JAVELIN = 0x1,
		WEAPOVERLAYINTERFACE_TURRETSCOPE = 0x2,

		WEAPOVERLAYINTERFACE_COUNT
	};

	enum WeapStickinessType
	{
		WEAPSTICKINESS_NONE = 0x0,
		WEAPSTICKINESS_ALL = 0x1,
		WEAPSTICKINESS_ALL_ORIENT = 0x2,
		WEAPSTICKINESS_GROUND = 0x3,
		WEAPSTICKINESS_GROUND_WITH_YAW = 0x4,
		WEAPSTICKINESS_KNIFE = 0x5,

		WEAPSTICKINESS_COUNT
	};

	enum guidedMissileType_t
	{
		MISSILE_GUIDANCE_NONE = 0x0,
		MISSILE_GUIDANCE_SIDEWINDER = 0x1,
		MISSILE_GUIDANCE_HELLFIRE = 0x2,
		MISSILE_GUIDANCE_JAVELIN = 0x3,

		MISSILE_GUIDANCE_COUNT
	};

	enum weapAnimFiles_t : unsigned int
	{
		WEAP_ANIM_ROOT = 0x0,
		WEAP_ANIM_IDLE = 0x1,
		WEAP_ANIM_EMPTY_IDLE = 0x2,
		WEAP_ANIM_FIRE = 0x3,
		WEAP_ANIM_HOLD_FIRE = 0x4,
		WEAP_ANIM_LASTSHOT = 0x5,
		WEAP_ANIM_RECHAMBER = 0x6,
		WEAP_ANIM_MELEE = 0x7,
		WEAP_ANIM_MELEE_CHARGE = 0x8,
		WEAP_ANIM_RELOAD = 0x9,
		WEAP_ANIM_RELOAD_EMPTY = 0xA,
		WEAP_ANIM_RELOAD_START = 0xB,
		WEAP_ANIM_RELOAD_END = 0xC,
		WEAP_ANIM_RAISE = 0xD,
		WEAP_ANIM_FIRST_RAISE = 0xE,
		WEAP_ANIM_BREACH_RAISE = 0xF,
		WEAP_ANIM_DROP = 0x10,
		WEAP_ANIM_ALT_RAISE = 0x11,
		WEAP_ANIM_ALT_DROP = 0x12,
		WEAP_ANIM_QUICK_RAISE = 0x13,
		WEAP_ANIM_QUICK_DROP = 0x14,
		WEAP_ANIM_EMPTY_RAISE = 0x15,
		WEAP_ANIM_EMPTY_DROP = 0x16,
		WEAP_ANIM_SPRINT_IN = 0x17,
		WEAP_ANIM_SPRINT_LOOP = 0x18,
		WEAP_ANIM_SPRINT_OUT = 0x19,
		WEAP_ANIM_STUNNED_START = 0x1A,
		WEAP_ANIM_STUNNED_LOOP = 0x1B,
		WEAP_ANIM_STUNNED_END = 0x1C,
		WEAP_ANIM_DETONATE = 0x1D,
		WEAP_ANIM_NIGHTVISION_WEAR = 0x1E,
		WEAP_ANIM_NIGHTVISION_REMOVE = 0x1F,
		WEAP_ANIM_ADS_FIRE = 0x20,
		WEAP_ANIM_ADS_LASTSHOT = 0x21,
		WEAP_ANIM_ADS_RECHAMBER = 0x22,
		WEAP_ANIM_BLAST_FRONT = 0x23,
		WEAP_ANIM_BLAST_RIGHT = 0x24,
		WEAP_ANIM_BLAST_BACK = 0x25,
		WEAP_ANIM_BLAST_LEFT = 0x26,
		WEAP_ANIM_ADS_UP = 0x27,
		WEAP_ANIM_ADS_DOWN = 0x28,
		WEAP_ALT_ANIM_ADJUST = 0x29,

		WEAP_ANIM_COUNT
	};

	enum hitLocation_t
	{
		HITLOC_NONE = 0x0,
		HITLOC_HELMET = 0x1,
		HITLOC_HEAD = 0x2,
		HITLOC_NECK = 0x3,
		HITLOC_TORSO_UPR = 0x4,
		HITLOC_TORSO_LWR = 0x5,
		HITLOC_R_ARM_UPR = 0x6,
		HITLOC_L_ARM_UPR = 0x7,
		HITLOC_R_ARM_LWR = 0x8,
		HITLOC_L_ARM_LWR = 0x9,
		HITLOC_R_HAND = 0xA,
		HITLOC_L_HAND = 0xB,
		HITLOC_R_LEG_UPR = 0xC,
		HITLOC_L_LEG_UPR = 0xD,
		HITLOC_R_LEG_LWR = 0xE,
		HITLOC_L_LEG_LWR = 0xF,
		HITLOC_R_FOOT = 0x10,
		HITLOC_L_FOOT = 0x11,
		HITLOC_GUN = 0x12,
		HITLOC_SHIELD = 0x13,

		HITLOC_COUNT
	};

	enum materialSurfType_t
	{
		SURF_TYPE_DEFAULT,
		SURF_TYPE_BARK,
		SURF_TYPE_BRICK,
		SURF_TYPE_CARPET,
		SURF_TYPE_CLOTH,
		SURF_TYPE_CONCRETE,
		SURF_TYPE_DIRT,
		SURF_TYPE_FLESH,
		SURF_TYPE_FOLIAGE,
		SURF_TYPE_GLASS,
		SURF_TYPE_GRASS,
		SURF_TYPE_GRAVEL,
		SURF_TYPE_ICE,
		SURF_TYPE_METAL,
		SURF_TYPE_MUD,
		SURF_TYPE_PAPER,
		SURF_TYPE_PLASTER,
		SURF_TYPE_ROCK,
		SURF_TYPE_SAND,
		SURF_TYPE_SNOW,
		SURF_TYPE_WATER,
		SURF_TYPE_WOOD,
		SURF_TYPE_ASPHALT,
		SURF_TYPE_CERAMIC,
		SURF_TYPE_PLASTIC,
		SURF_TYPE_RUBBER,
		SURF_TYPE_CUSHION,
		SURF_TYPE_FRUIT,
		SURF_TYPE_PAINTED_METAL,
		SURF_TYPE_RIOT_SHIELD,
		SURF_TYPE_SLUSH,

		SURF_TYPE_COUNT
	};

	enum playerAnimType_t : int
	{
		PLAYERANIMTYPE_NONE = 0,
		PLAYERANIMTYPE_OTHER = 1,
		PLAYERANIMTYPE_PISTOL = 2,
		PLAYERANIMTYPE_SMG = 3,
		PLAYERANIMTYPE_AUTORIFLE = 4,
		PLAYERANIMTYPE_MG = 5,
		PLAYERANIMTYPE_SNIPER = 6,
		PLAYERANIMTYPE_ROCKETLAUNCHER = 7,
		PLAYERANIMTYPE_EXPLOSIVE = 8,
		PLAYERANIMTYPE_GRENADE = 9,
		PLAYERANIMTYPE_TURRET = 10,
		PLAYERANIMTYPE_C4 = 11,
		PLAYERANIMTYPE_M203 = 12,
		PLAYERANIMTYPE_HOLD = 13,
		PLAYERANIMTYPE_BRIEFCASE = 14,
		PLAYERANIMTYPE_RIOTSHIELD = 15,
		PLAYERANIMTYPE_LAPTOP = 16,
		PLAYERANIMTYPE_THROWINGKNIFE = 17,
		PLAYERANIMTYPE_NUM
	};

	struct WeaponDef
	{
		x86::pointer<const char> szOverlayName;
		x86::pointer<x86::pointer<XModel>> gunXModel;
		x86::pointer<XModel> handXModel;
		x86::pointer<x86::pointer<const char>> szXAnimsRightHanded;
		x86::pointer<x86::pointer<const char>> szXAnimsLeftHanded;
		x86::pointer<const char> szModeName;
		x86::pointer<scr_string_t> notetrackSoundMapKeys;
		x86::pointer<scr_string_t> notetrackSoundMapValues;
		x86::pointer<scr_string_t> notetrackRumbleMapKeys;
		x86::pointer<scr_string_t> notetrackRumbleMapValues;
		playerAnimType_t playerAnimType;
		weapType_t weapType;
		weapClass_t weapClass;
		PenetrateType penetrateType;
		weapInventoryType_t inventoryType;
		weapFireType_t fireType;
		OffhandClass offhandClass;
		weapStance_t stance;
		x86::pointer<FxEffectDef> viewFlashEffect;
		x86::pointer<FxEffectDef> worldFlashEffect;
		SndAliasCustom pickupSound;
		SndAliasCustom pickupSoundPlayer;
		SndAliasCustom ammoPickupSound;
		SndAliasCustom ammoPickupSoundPlayer;
		SndAliasCustom projectileSound;
		SndAliasCustom pullbackSound;
		SndAliasCustom pullbackSoundPlayer;
		SndAliasCustom fireSound;
		SndAliasCustom fireSoundPlayer;
		SndAliasCustom fireSoundPlayerAkimbo;
		SndAliasCustom fireLoopSound;
		SndAliasCustom fireLoopSoundPlayer;
		SndAliasCustom fireStopSound;
		SndAliasCustom fireStopSoundPlayer;
		SndAliasCustom fireLastSound;
		SndAliasCustom fireLastSoundPlayer;
		SndAliasCustom emptyFireSound;
		SndAliasCustom emptyFireSoundPlayer;
		SndAliasCustom meleeSwipeSound;
		SndAliasCustom meleeSwipeSoundPlayer;
		SndAliasCustom meleeHitSound;
		SndAliasCustom meleeMissSound;
		SndAliasCustom rechamberSound;
		SndAliasCustom rechamberSoundPlayer;
		SndAliasCustom reloadSound;
		SndAliasCustom reloadSoundPlayer;
		SndAliasCustom reloadEmptySound;
		SndAliasCustom reloadEmptySoundPlayer;
		SndAliasCustom reloadStartSound;
		SndAliasCustom reloadStartSoundPlayer;
		SndAliasCustom reloadEndSound;
		SndAliasCustom reloadEndSoundPlayer;
		SndAliasCustom detonateSound;
		SndAliasCustom detonateSoundPlayer;
		SndAliasCustom nightVisionWearSound;
		SndAliasCustom nightVisionWearSoundPlayer;
		SndAliasCustom nightVisionRemoveSound;
		SndAliasCustom nightVisionRemoveSoundPlayer;
		SndAliasCustom altSwitchSound;
		SndAliasCustom altSwitchSoundPlayer;
		SndAliasCustom raiseSound;
		SndAliasCustom raiseSoundPlayer;
		SndAliasCustom firstRaiseSound;
		SndAliasCustom firstRaiseSoundPlayer;
		SndAliasCustom putawaySound;
		SndAliasCustom putawaySoundPlayer;
		SndAliasCustom scanSound;
		SndAliasCustom changeVariableZoomSound;
		x86::pointer<SndAliasCustom> bounceSound;
		x86::pointer<SndAliasCustom> rollingSound;
		x86::pointer<FxEffectDef> viewShellEjectEffect;
		x86::pointer<FxEffectDef> worldShellEjectEffect;
		x86::pointer<FxEffectDef> viewLastShotEjectEffect;
		x86::pointer<FxEffectDef> worldLastShotEjectEffect;
		x86::pointer<Material> reticleCenter;
		x86::pointer<Material> reticleSide;
		int iReticleCenterSize;
		int iReticleSideSize;
		int iReticleMinOfs;
		activeReticleType_t activeReticleType;
		float vStandMove[3];
		float vStandRot[3];
		float strafeMove[3];
		float strafeRot[3];
		float vDuckedOfs[3];
		float vDuckedMove[3];
		float vDuckedRot[3];
		float vProneOfs[3];
		float vProneMove[3];
		float vProneRot[3];
		float fPosMoveRate;
		float fPosProneMoveRate;
		float fStandMoveMinSpeed;
		float fDuckedMoveMinSpeed;
		float fProneMoveMinSpeed;
		float fPosRotRate;
		float fPosProneRotRate;
		float fStandRotMinSpeed;
		float fDuckedRotMinSpeed;
		float fProneRotMinSpeed;
		x86::pointer<x86::pointer<XModel>> worldModel;
		x86::pointer<XModel> worldClipModel;
		x86::pointer<XModel> rocketModel;
		x86::pointer<XModel> knifeModel;
		x86::pointer<XModel> worldKnifeModel;
		x86::pointer<Material> hudIcon;
		weaponIconRatioType_t hudIconRatio;
		x86::pointer<Material> pickupIcon;
		weaponIconRatioType_t pickupIconRatio;
		x86::pointer<Material> ammoCounterIcon;
		weaponIconRatioType_t ammoCounterIconRatio;
		ammoCounterClipType_t ammoCounterClip;
		int iStartAmmo;
		x86::pointer<const char> szAmmoName;
		int iAmmoIndex;
		x86::pointer<const char> szClipName;
		int iClipIndex;
		int iMaxAmmo;
		int shotCount;
		x86::pointer<const char> szSharedAmmoCapName;
		int iSharedAmmoCapIndex;
		int iSharedAmmoCap;
		int damage;
		int playerDamage;
		int iMeleeDamage;
		int iDamageType;
		StateTimers stateTimers;
		StateTimers akimboStateTimers;
		float autoAimRange;
		float aimAssistRange;
		float aimAssistRangeAds;
		float aimPadding;
		float enemyCrosshairRange;
		float moveSpeedScale;
		float adsMoveSpeedScale;
		float sprintDurationScale;
		float fAdsZoomInFrac;
		float fAdsZoomOutFrac;
		ADSOverlay overlay;
		WeapOverlayInterface_t overlayInterface;
		float fAdsBobFactor;
		float fAdsViewBobMult;
		float fHipSpreadStandMin;
		float fHipSpreadDuckedMin;
		float fHipSpreadProneMin;
		float hipSpreadStandMax;
		float hipSpreadDuckedMax;
		float hipSpreadProneMax;
		float fHipSpreadDecayRate;
		float fHipSpreadFireAdd;
		float fHipSpreadTurnAdd;
		float fHipSpreadMoveAdd;
		float fHipSpreadDuckedDecay;
		float fHipSpreadProneDecay;
		float fHipReticleSidePos;
		float fAdsIdleAmount;
		float fHipIdleAmount;
		float adsIdleSpeed;
		float hipIdleSpeed;
		float fIdleCrouchFactor;
		float fIdleProneFactor;
		float fGunMaxPitch;
		float fGunMaxYaw;
		float adsIdleLerpStartTime;
		float adsIdleLerpTime;
		float swayMaxAngle;
		float swayLerpSpeed;
		float swayPitchScale;
		float swayYawScale;
		float swayHorizScale;
		float swayVertScale;
		float swayShellShockScale;
		float adsSwayMaxAngle;
		float adsSwayLerpSpeed;
		float adsSwayPitchScale;
		float adsSwayYawScale;
		float adsSwayHorizScale;
		float adsSwayVertScale;
		float adsViewErrorMin;
		float adsViewErrorMax;
		x86::pointer<PhysCollmap> physCollmap;
		float dualWieldViewModelOffset;
		weaponIconRatioType_t killIconRatio;
		int iReloadAmmoAdd;
		int iReloadStartAdd;
		int ammoDropStockMin;
		int ammoDropClipPercentMin;
		int ammoDropClipPercentMax;
		int iExplosionRadius;
		int iExplosionRadiusMin;
		int iExplosionInnerDamage;
		int iExplosionOuterDamage;
		float damageConeAngle;
		float bulletExplDmgMult;
		float bulletExplRadiusMult;
		int iProjectileSpeed;
		int iProjectileSpeedUp;
		int iProjectileSpeedForward;
		int iProjectileActivateDist;
		float projLifetime;
		float timeToAccelerate;
		float projectileCurvature;
		x86::pointer<XModel> projectileModel;
		weapProjExposion_t projExplosion;
		x86::pointer<FxEffectDef> projExplosionEffect;
		x86::pointer<FxEffectDef> projDudEffect;
		SndAliasCustom projExplosionSound;
		SndAliasCustom projDudSound;
		WeapStickinessType stickiness;
		float lowAmmoWarningThreshold;
		float ricochetChance;
		bool riotShieldEnableDamage;
		int riotShieldHealth;
		float riotShieldDamageMult;
		x86::pointer<float> parallelBounce;
		x86::pointer<float> perpendicularBounce;
		x86::pointer<FxEffectDef> projTrailEffect;
		x86::pointer<FxEffectDef> projBeaconEffect;
		float vProjectileColor[3];
		guidedMissileType_t guidedMissileType;
		float maxSteeringAccel;
		int projIgnitionDelay;
		x86::pointer<FxEffectDef> projIgnitionEffect;
		SndAliasCustom projIgnitionSound;
		float fAdsAimPitch;
		float fAdsCrosshairInFrac;
		float fAdsCrosshairOutFrac;
		int adsGunKickReducedKickBullets;
		float adsGunKickReducedKickPercent;
		float fAdsGunKickPitchMin;
		float fAdsGunKickPitchMax;
		float fAdsGunKickYawMin;
		float fAdsGunKickYawMax;
		float fAdsGunKickAccel;
		float fAdsGunKickSpeedMax;
		float fAdsGunKickSpeedDecay;
		float fAdsGunKickStaticDecay;
		float fAdsViewKickPitchMin;
		float fAdsViewKickPitchMax;
		float fAdsViewKickYawMin;
		float fAdsViewKickYawMax;
		float fAdsViewScatterMin;
		float fAdsViewScatterMax;
		float fAdsSpread;
		int hipGunKickReducedKickBullets;
		float hipGunKickReducedKickPercent;
		float fHipGunKickPitchMin;
		float fHipGunKickPitchMax;
		float fHipGunKickYawMin;
		float fHipGunKickYawMax;
		float fHipGunKickAccel;
		float fHipGunKickSpeedMax;
		float fHipGunKickSpeedDecay;
		float fHipGunKickStaticDecay;
		float fHipViewKickPitchMin;
		float fHipViewKickPitchMax;
		float fHipViewKickYawMin;
		float fHipViewKickYawMax;
		float fHipViewScatterMin;
		float fHipViewScatterMax;
		float fightDist;
		float maxDist;
		x86::pointer<const char> aiVsAiAccuracyGraphName;
		x86::pointer<const char> aiVsPlayerAccuracyGraphName;
		x86::pointer<vec2_t> originalAiVsAiAccuracyGraphKnots;
		x86::pointer<vec2_t> originalAiVsPlayerAccuracyGraphKnots;
		unsigned short originalAiVsAiAccuracyGraphKnotCount;
		unsigned short originalAiVsPlayerAccuracyGraphKnotCount;
		int iPositionReloadTransTime;
		float leftArc;
		float rightArc;
		float topArc;
		float bottomArc;
		float accuracy;
		float aiSpread;
		float playerSpread;
		float minTurnSpeed[2];
		float maxTurnSpeed[2];
		float pitchConvergenceTime;
		float yawConvergenceTime;
		float suppressTime;
		float maxRange;
		float fAnimHorRotateInc;
		float fPlayerPositionDist;
		x86::pointer<const char> szUseHintString;
		x86::pointer<const char> dropHintString;
		int iUseHintStringIndex;
		int dropHintStringIndex;
		float horizViewJitter;
		float vertViewJitter;
		float scanSpeed;
		float scanAccel;
		int scanPauseTime;
		x86::pointer<const char> szScript;
		float fOOPosAnimLength[2];
		int minDamage;
		int minPlayerDamage;
		float fMaxDamageRange;
		float fMinDamageRange;
		float destabilizationRateTime;
		float destabilizationCurvatureMax;
		int destabilizeDistance;
		x86::pointer<float> locationDamageMultipliers;
		x86::pointer<const char> fireRumble;
		x86::pointer<const char> meleeImpactRumble;
		x86::pointer<TracerDef> tracerType;
		bool turretADSEnabled;
		float turretADSTime;
		float turretFov;
		float turretFovADS;
		float turretScopeZoomRate;
		float turretScopeZoomMin;
		float turretScopeZoomMax;
		float turretOverheatUpRate;
		float turretOverheatDownRate;
		float turretOverheatPenalty;
		SndAliasCustom turretOverheatSound;
		x86::pointer<FxEffectDef> turretOverheatEffect;
		x86::pointer<const char> turretBarrelSpinRumble;
		float turretBarrelSpinSpeed;
		float turretBarrelSpinUpTime;
		float turretBarrelSpinDownTime;
		SndAliasCustom turretBarrelSpinMaxSnd;
		SndAliasCustom turretBarrelSpinUpSnd[4];
		SndAliasCustom turretBarrelSpinDownSnd[4];
		SndAliasCustom missileConeSoundAlias;
		SndAliasCustom missileConeSoundAliasAtBase;
		float missileConeSoundRadiusAtTop;
		float missileConeSoundRadiusAtBase;
		float missileConeSoundHeight;
		float missileConeSoundOriginOffset;
		float missileConeSoundVolumescaleAtCore;
		float missileConeSoundVolumescaleAtEdge;
		float missileConeSoundVolumescaleCoreSize;
		float missileConeSoundPitchAtTop;
		float missileConeSoundPitchAtBottom;
		float missileConeSoundPitchTopSize;
		float missileConeSoundPitchBottomSize;
		float missileConeSoundCrossfadeTopSize;
		float missileConeSoundCrossfadeBottomSize;
		bool sharedAmmo;
		bool lockonSupported;
		bool requireLockonToFire;
		bool isAirburstWeapon;
		bool bigExplosion;
		bool noAdsWhenMagEmpty;
		bool avoidDropCleanup;
		bool inheritsPerks;
		bool crosshairColorChange;
		bool bRifleBullet;
		bool armorPiercing;
		bool bBoltAction;
		bool aimDownSight;
		bool canHoldBreath;
		bool canVariableZoom;
		bool bRechamberWhileAds;
		bool bBulletExplosiveDamage;
		bool bCookOffHold;
		bool bClipOnly;
		bool noAmmoPickup;
		bool adsFireOnly;
		bool cancelAutoHolsterWhenEmpty;
		bool disableSwitchToWhenEmpty;
		bool suppressAmmoReserveDisplay;
		bool laserSightDuringNightvision;
		bool markableViewmodel;
		bool noDualWield;
		bool flipKillIcon;
		bool bNoPartialReload;
		bool bSegmentedReload;
		bool blocksProne;
		bool silenced;
		bool isRollingGrenade;
		bool projExplosionEffectForceNormalUp;
		bool bProjImpactExplode;
		bool stickToPlayers;
		bool stickToVehicles;
		bool stickToTurrets;
		bool hasDetonator;
		bool disableFiring;
		bool timedDetonation;
		bool rotate;
		bool holdButtonToThrow;
		bool freezeMovementWhenFiring;
		bool thermalScope;
		bool altModeSameWeapon;
		bool turretBarrelSpinEnabled;
		bool missileConeSoundEnabled;
		bool missileConeSoundPitchshiftEnabled;
		bool missileConeSoundCrossfadeEnabled;
		bool offhandHoldIsCancelable;
		bool doNotAllowAttachmentsToOverrideSpread;
		scr_string_t stowTag;
		x86::pointer<XModel> stowOffsetModel;
	};

	union WeaponAttachmentCombination
	{
		struct
		{
			// Specifies the index as a number
			// since there can only be one scope
			unsigned short scope : 3;
			// Specifies the index as a number
			// since there can only be one under barrel
			unsigned short underBarrel : 2;
			// Specifies all other attachments as a bit array
			unsigned short other : 4;
		};

		unsigned short fields;
	};

	struct AnimOverrideEntry
	{
		WeaponAttachmentCombination attachment1;
		WeaponAttachmentCombination attachment2;
		x86::pointer<const char> overrideAnim;
		x86::pointer<const char> altmodeAnim;
		weapAnimFiles_t animTreeType;
		int animTime;
		int altTime;
	};

	enum SoundOverrideTypes : unsigned int
	{
		SNDTYPE_NONE = 0x0,
		SNDTYPE_FIRE = 0x1,
		SNDTYPE_PLAYER_FIRE = 0x2,
		SNDTYPE_PLAYER_AKIMBO = 0x3,
		SNDTYPE_PLAYER_LASTSHOT = 0x4,

		SNDTYPE_PLAYER_COUNT
	};

	struct SoundOverrideEntry
	{
		WeaponAttachmentCombination attachment1;
		WeaponAttachmentCombination attachment2;
		SndAliasCustom overrideSound;
		SndAliasCustom altmodeSound;
		SoundOverrideTypes soundType;
	};

	enum FxOverrideTypes : unsigned int
	{
		FXTYPE_NONE = 0x0,
		FXTYPE_VIEW_FLASH = 0x1,
		FXTYPE_WORLD_FLASH = 0x2,
		FXTYPE_VIEW_SHELL_EJECT = 0x3,
		FXTYPE_WORLD_SHELL_EJECT = 0x4,

		FXTYPE_COUNT
	};

	struct FxOverrideEntry
	{
		WeaponAttachmentCombination attachment1;
		WeaponAttachmentCombination attachment2;
		x86::pointer<FxEffectDef> overrideFx;
		x86::pointer<FxEffectDef> altmodeFx;
		FxOverrideTypes fxType;
	};

	struct ReloadStateTimerEntry
	{
		WeaponAttachmentCombination attachment;
		short unused;
		int reloadAddTime;
		int reloadStartAddTime;
	};

	struct NoteTrackToSoundEntry
	{
		WeaponAttachmentCombination attachment;
		short unused;
		x86::pointer<scr_string_t> notetrackSoundMapKeys;
		x86::pointer<scr_string_t> notetrackSoundMapValues;
	};

	struct WeaponCompleteDef
	{
		x86::pointer<const char> szInternalName;
		x86::pointer<WeaponDef> weapDef;
		x86::pointer<const char> szDisplayName;
		x86::pointer<scr_string_t> hideTags;
		x86::pointer<x86::pointer<WeaponAttachment>> scopes;
		x86::pointer<x86::pointer<WeaponAttachment>> underBarrels;
		x86::pointer<x86::pointer<WeaponAttachment>> others;
		x86::pointer<x86::pointer<const char>> szXAnims;
		unsigned int numAnimOverrides;
		x86::pointer<AnimOverrideEntry> animOverrides;
		unsigned int numSoundOverrides;
		x86::pointer<SoundOverrideEntry> soundOverrides;
		unsigned int numFxOverrides;
		x86::pointer<FxOverrideEntry> fxOverrides;
		unsigned int numReloadStateTimerOverrides;
		x86::pointer<ReloadStateTimerEntry> reloadOverrides;
		unsigned int numNotetrackOverrides;
		x86::pointer<NoteTrackToSoundEntry> notetrackOverrides;
		float fAdsZoomFov;
		int iAdsTransInTime;
		int iAdsTransOutTime;
		int iClipSize;
		ImpactType impactType;
		int iFireTime;
		int iFireTimeAkimbo;
		weaponIconRatioType_t dpadIconRatio;
		float penetrateMultiplier;
		float fAdsViewKickCenterSpeed;
		float fHipViewKickCenterSpeed;
		x86::pointer<const char> szAltWeaponName;
		int altWeapon;
		int iAltRaiseTime;
		int iAltRaiseTimeAkimbo;
		x86::pointer<Material> killIcon;
		x86::pointer<Material> dpadIcon;
		int fireAnimLength;
		int fireAnimLengthAkimbo;
		int iFirstRaiseTime;
		int iFirstRaiseTimeAkimbo;
		int ammoDropStockMax;
		float adsDofStart;
		float adsDofEnd;
		uint16_t aiVsAiAccuracyGraphKnotCount;
		uint16_t aiVsPlayerAccuracyGraphKnotCount;
		x86::pointer<vec2_t> aiVsAiAccuracyGraphKnots;
		x86::pointer<vec2_t> aiVsPlayerAccuracyGraphKnots;
		bool motionTracker;
		bool enhanced;
		bool dpadIconShowsAmmo;
	};

	struct ComPrimaryLight
	{
		unsigned char type;
		unsigned char canUseShadowMap;
		unsigned char exponent;
		unsigned char unused;
		float color[3];
		float dir[3];
		float up[3];
		float origin[3];
		float radius;
		float cosHalfFovOuter;
		float cosHalfFovInner;
		float cosHalfFovExpanded;
		float rotationLimit;
		float translationLimit;
		x86::pointer<const char> defName;
	};

	struct ComWorld
	{
		x86::pointer<const char> name;
		int isInUse;
		unsigned int primaryLightCount;
		x86::pointer<ComPrimaryLight> primaryLights;
	};

	struct G_GlassPiece
	{
		unsigned __int16 damageTaken;
		unsigned __int16 collapseTime;
		int lastStateChangeTime;
		char impactDir;
		char impactPos[2];
	};

	struct G_GlassName
	{
		x86::pointer<char> nameStr;
		unsigned __int16 name;
		unsigned __int16 pieceCount;
		x86::pointer<unsigned __int16> pieceIndices;
	};

	struct G_GlassData
	{
		x86::pointer<G_GlassPiece> glassPieces;
		unsigned int pieceCount;
		unsigned __int16 damageToWeaken;
		unsigned __int16 damageToDestroy;
		unsigned int glassNameCount;
		x86::pointer<G_GlassName> glassNames;
		char pad[108];
	};

	struct GlassWorld
	{
		x86::pointer<const char> name;
		x86::pointer<G_GlassData> g_glassData;
	};

	struct GfxImageLoadDef
	{
		char levelCount;
		char pad[3];
		int flags;
		int format;
		int resourceSize;
		char data[1];
	};

	struct Picmip
	{
		char platform[2];
	};

	struct CardMemory
	{
		int platform[2];
	};

	union GfxTexture
	{
		// IDirect3DBaseTexture9* basemap;
		// IDirect3DTexture9* map;
		// IDirect3DVolumeTexture9* volmap;
		// IDirect3DCubeTexture9* cubemap;
		x86::pointer<GfxImageLoadDef> loadDef;
	};

	enum GfxImageCategory : unsigned char
	{
		IMG_CATEGORY_UNKNOWN = 0x0,
		IMG_CATEGORY_AUTO_GENERATED = 0x1,
		IMG_CATEGORY_LIGHTMAP = 0x2,
		IMG_CATEGORY_LOAD_FROM_FILE = 0x3,
		IMG_CATEGORY_RAW = 0x4,
		IMG_CATEGORY_FIRST_UNMANAGED = 0x5,
		IMG_CATEGORY_WATER = 0x5,
		IMG_CATEGORY_RENDERTARGET = 0x6,
		IMG_CATEGORY_TEMP = 0x7,
	};

	enum TextureSemantic : std::uint8_t
	{
		TS_2D = 0x0,
		TS_FUNCTION = 0x1,
		TS_COLOR_MAP = 0x2,
		TS_DETAIL_MAP = 0x3,
		TS_UNUSED_2 = 0x4,
		TS_NORMAL_MAP = 0x5,
		TS_UNUSED_3 = 0x6,
		TS_UNUSED_4 = 0x7,
		TS_SPECULAR_MAP = 0x8,
		TS_UNUSED_5 = 0x9,
		TS_UNUSED_6 = 0xA,
		TS_WATER_MAP = 0xB,
		TS_DISPLACEMENT_MAP = 0xC,
	};

	enum file_image_flags_t
	{
		IMG_FLAG_NOPICMIP = 0x1,
		IMG_FLAG_NOMIPMAPS = 0x2,
		IMG_FLAG_CUBEMAP = 0x4,
		IMG_FLAG_VOLMAP = 0x8,
		IMG_FLAG_STREAMING = 0x10,
		IMG_FLAG_LEGACY_NORMALS = 0x20,
		IMG_FLAG_CLAMP_U = 0x40,
		IMG_FLAG_CLAMP_V = 0x80,
		IMG_FLAG_DYNAMIC = 0x10000,
		IMG_FLAG_RENDER_TARGET = 0x20000,
		IMG_FLAG_SYSTEMMEM = 0x40000,
	};

	struct GfxImage
	{
		GfxTexture texture;
		char mapType;
		char semantic;
		char category;
		unsigned char flags;
		Picmip picmip;
		bool noPicmip;
		char track;
		CardMemory cardMemory;
		unsigned short width;
		unsigned short height;
		unsigned short depth;
		unsigned char levelCount;
		x86::pointer<const char> name;
	};

	struct GfxLightImage
	{
		x86::pointer<GfxImage> image;
		unsigned char samplerState;
	};

	struct GfxLightDef
	{
		x86::pointer<const char> name;
		GfxLightImage attenuation;
		GfxLightImage cucoloris;
		int lmapLookupStart;
	};

	enum MaterialTechniqueType
	{
		// From iw4, looks correct
		TECHNIQUE_DEPTH_PREPASS = 0x0,
		TECHNIQUE_BUILD_FLOAT_Z = 0x1,
		TECHNIQUE_BUILD_SHADOWMAP_DEPTH = 0x2,
		TECHNIQUE_BUILD_SHADOWMAP_COLOR = 0x3,
		TECHNIQUE_UNLIT = 0x4,
		TECHNIQUE_EMISSIVE = 0x5,
		TECHNIQUE_EMISSIVE_DFOG = 0x6,
		TECHNIQUE_EMISSIVE_SHADOW = 0x7,
		TECHNIQUE_EMISSIVE_SHADOW_DFOG = 0x8,
		TECHNIQUE_LIT_BEGIN = 0x9,
		TECHNIQUE_LIT = 0x9,
		TECHNIQUE_LIT_DFOG = 0xA,
		TECHNIQUE_LIT_SUN = 0xB,
		TECHNIQUE_LIT_SUN_DFOG = 0xC,
		TECHNIQUE_LIT_SUN_SHADOW = 0xD,
		TECHNIQUE_LIT_SUN_SHADOW_DFOG = 0xE,
		TECHNIQUE_LIT_SPOT = 0xF,
		TECHNIQUE_LIT_SPOT_DFOG = 0x10,

		// Two unknowns

		TECHNIQUE_LIT_SPOT_SHADOW = 0x13,
		TECHNIQUE_LIT_SPOT_SHADOW_DFOG,
		TECHNIQUE_LIT_OMNI,
		TECHNIQUE_LIT_OMNI_DFOG,
		TECHNIQUE_LIT_OMNI_SHADOW,
		TECHNIQUE_LIT_OMNI_SHADOW_DFOG,
		TECHNIQUE_LIT_INSTANCED,
		TECHNIQUE_LIT_INSTANCED_DFOG,
		TECHNIQUE_LIT_INSTANCED_SUN,
		TECHNIQUE_LIT_INSTANCED_SUN_DFOG,
		TECHNIQUE_LIT_INSTANCED_SUN_SHADOW,
		TECHNIQUE_LIT_INSTANCED_SUN_SHADOW_DFOG,
		TECHNIQUE_LIT_INSTANCED_SPOT,
		TECHNIQUE_LIT_INSTANCED_SPOT_DFOG,
		TECHNIQUE_LIT_INSTANCED_SPOT_SHADOW,
		TECHNIQUE_LIT_INSTANCED_SPOT_SHADOW_DFOG,

		// <2 instance spot shadows here>

		TECHNIQUE_LIT_INSTANCED_OMNI = 0x25,
		TECHNIQUE_LIT_INSTANCED_OMNI_DFOG,
		TECHNIQUE_LIT_INSTANCED_OMNI_SHADOW,
		TECHNIQUE_LIT_INSTANCED_OMNI_SHADOW_DFOG,
		TECHNIQUE_LIGHT_SPOT,
		TECHNIQUE_LIGHT_OMNI,
		TECHNIQUE_LIGHT_SPOT_SHADOW,

		// <1 unknown here>

		TECHNIQUE_FAKELIGHT_NORMAL = 0x2D,
		TECHNIQUE_FAKELIGHT_VIEW = 0x2E,
		TECHNIQUE_SUNLIGHT_PREVIEW = 0x2F,
		TECHNIQUE_CASE_TEXTURE = 0x30,
		TECHNIQUE_WIREFRAME_SOLID = 0x31,
		TECHNIQUE_WIREFRAME_SHADED = 0x32,
		TECHNIQUE_THERMAL = 0x33, // Confirmed
		TECHNIQUE_DEBUG_BUMPMAP = 0x34,
		TECHNIQUE_DEBUG_BUMPMAP_INSTANCED = 0x35,
		//

		TECHNIQUE_COUNT = 0x39,
	};

	enum ShaderCodeConstants : unsigned short
	{
		CONST_SRC_CODE_LIGHT_POSITION,
		CONST_SRC_CODE_LIGHT_DIFFUSE,
		CONST_SRC_CODE_LIGHT_SPECULAR,
		CONST_SRC_CODE_LIGHT_SPOTDIR,
		CONST_SRC_CODE_LIGHT_SPOTFACTORS,
		CONST_SRC_CODE_LIGHT_FALLOFF_PLACEMENT,
		CONST_SRC_CODE_PARTICLE_CLOUD_COLOR,
		CONST_SRC_CODE_GAMETIME,
		CONST_SRC_CODE_EYEOFFSET,
		CONST_SRC_CODE_COLOR_SATURATION_R,
		CONST_SRC_CODE_COLOR_SATURATION_G,
		CONST_SRC_CODE_COLOR_SATURATION_B,
		CONST_SRC_CODE_PIXEL_COST_FRACS,
		CONST_SRC_CODE_PIXEL_COST_DECODE,
		CONST_SRC_CODE_FILTER_TAP_0,
		CONST_SRC_CODE_FILTER_TAP_1,
		CONST_SRC_CODE_FILTER_TAP_2,
		CONST_SRC_CODE_FILTER_TAP_3,
		CONST_SRC_CODE_FILTER_TAP_4,
		CONST_SRC_CODE_FILTER_TAP_5,
		CONST_SRC_CODE_FILTER_TAP_6,
		CONST_SRC_CODE_FILTER_TAP_7,
		CONST_SRC_CODE_COLOR_MATRIX_R,
		CONST_SRC_CODE_COLOR_MATRIX_G,
		CONST_SRC_CODE_COLOR_MATRIX_B,
		CONST_SRC_CODE_UNK1,
		CONST_SRC_CODE_SHADOWMAP_POLYGON_OFFSET,
		CONST_SRC_CODE_RENDER_TARGET_SIZE,
		CONST_SRC_CODE_RENDER_SOURCE_SIZE,
		CONST_SRC_CODE_DOF_EQUATION_VIEWMODEL_AND_FAR_BLUR,
		CONST_SRC_CODE_DOF_EQUATION_SCENE,
		CONST_SRC_CODE_DOF_LERP_SCALE,
		CONST_SRC_CODE_DOF_LERP_BIAS,
		CONST_SRC_CODE_DOF_ROW_DELTA,
		CONST_SRC_CODE_MOTION_MATRIX_X,
		CONST_SRC_CODE_MOTION_MATRIX_Y,
		CONST_SRC_CODE_MOTION_MATRIX_W,
		CONST_SRC_CODE_SHADOWMAP_SWITCH_PARTITION,
		CONST_SRC_CODE_SHADOWMAP_SCALE,
		CONST_SRC_CODE_ZNEAR,
		CONST_SRC_CODE_LIGHTING_LOOKUP_SCALE,
		CONST_SRC_CODE_DEBUG_BUMPMAP,
		CONST_SRC_CODE_MATERIAL_COLOR,
		CONST_SRC_CODE_FOG,
		CONST_SRC_CODE_FOG_COLOR_LINEAR,
		CONST_SRC_CODE_FOG_COLOR_GAMMA,
		CONST_SRC_CODE_FOG_SUN_CONSTS,
		CONST_SRC_CODE_FOG_SUN_COLOR_LINEAR,
		CONST_SRC_CODE_FOG_SUN_COLOR_GAMMA,
		CONST_SRC_CODE_FOG_SUN_DIR,
		CONST_SRC_CODE_GLOW_SETUP,
		CONST_SRC_CODE_GLOW_APPLY,
		CONST_SRC_CODE_COLOR_BIAS,
		CONST_SRC_CODE_COLOR_TINT_BASE,
		CONST_SRC_CODE_COLOR_TINT_DELTA,
		CONST_SRC_CODE_COLOR_TINT_QUADRATIC_DELTA,
		CONST_SRC_CODE_OUTDOOR_FEATHER_PARMS,
		CONST_SRC_CODE_ENVMAP_PARMS,
		CONST_SRC_CODE_SUN_SHADOWMAP_PIXEL_ADJUST,
		CONST_SRC_CODE_SPOT_SHADOWMAP_PIXEL_ADJUST,
		CONST_SRC_CODE_COMPOSITE_FX_DISTORTION,
		CONST_SRC_CODE_POSTFX_FADE_EFFECT,
		CONST_SRC_CODE_VIEWPORT_DIMENSIONS,
		CONST_SRC_CODE_FRAMEBUFFER_READ,
		CONST_SRC_CODE_THERMAL_COLOR_OFFSET,
		CONST_SRC_CODE_PLAYLIST_POPULATION_PARAMS,
		CONST_SRC_CODE_BASE_LIGHTING_COORDS,
		CONST_SRC_CODE_LIGHT_PROBE_AMBIENT,
		CONST_SRC_CODE_NEARPLANE_ORG,
		CONST_SRC_CODE_NEARPLANE_DX,
		CONST_SRC_CODE_NEARPLANE_DY,
		CONST_SRC_CODE_CLIP_SPACE_LOOKUP_SCALE,
		CONST_SRC_CODE_CLIP_SPACE_LOOKUP_OFFSET,
		CONST_SRC_CODE_PARTICLE_CLOUD_MATRIX0,
		CONST_SRC_CODE_PARTICLE_CLOUD_MATRIX1,
		CONST_SRC_CODE_PARTICLE_CLOUD_MATRIX2,
		CONST_SRC_CODE_PARTICLE_CLOUD_SPARK_COLOR0,
		CONST_SRC_CODE_PARTICLE_CLOUD_SPARK_COLOR1,
		CONST_SRC_CODE_PARTICLE_CLOUD_SPARK_COLOR2,
		CONST_SRC_CODE_PARTICLE_FOUNTAIN_PARM0,
		CONST_SRC_CODE_PARTICLE_FOUNTAIN_PARM1,
		CONST_SRC_CODE_DEPTH_FROM_CLIP,
		CONST_SRC_CODE_CODE_MESH_ARG_0,
		CONST_SRC_CODE_CODE_MESH_ARG_1,
		CONST_SRC_CODE_VIEW_MATRIX,
		CONST_SRC_CODE_INVERSE_VIEW_MATRIX,
		CONST_SRC_CODE_TRANSPOSE_VIEW_MATRIX,
		CONST_SRC_CODE_INVERSE_TRANSPOSE_VIEW_MATRIX,
		CONST_SRC_CODE_PROJECTION_MATRIX,
		CONST_SRC_CODE_INVERSE_PROJECTION_MATRIX,
		CONST_SRC_CODE_TRANSPOSE_PROJECTION_MATRIX,
		CONST_SRC_CODE_INVERSE_TRANSPOSE_PROJECTION_MATRIX,
		CONST_SRC_CODE_VIEW_PROJECTION_MATRIX,
		CONST_SRC_CODE_INVERSE_VIEW_PROJECTION_MATRIX,
		CONST_SRC_CODE_TRANSPOSE_VIEW_PROJECTION_MATRIX,
		CONST_SRC_CODE_INVERSE_TRANSPOSE_VIEW_PROJECTION_MATRIX,
		CONST_SRC_CODE_SHADOW_LOOKUP_MATRIX,
		CONST_SRC_CODE_INVERSE_SHADOW_LOOKUP_MATRIX,
		CONST_SRC_CODE_TRANSPOSE_SHADOW_LOOKUP_MATRIX,
		CONST_SRC_CODE_INVERSE_TRANSPOSE_SHADOW_LOOKUP_MATRIX,
		CONST_SRC_CODE_WORLD_OUTDOOR_LOOKUP_MATRIX,
		CONST_SRC_CODE_INVERSE_WORLD_OUTDOOR_LOOKUP_MATRIX,
		CONST_SRC_CODE_TRANSPOSE_WORLD_OUTDOOR_LOOKUP_MATRIX,
		CONST_SRC_CODE_INVERSE_TRANSPOSE_WORLD_OUTDOOR_LOOKUP_MATRIX,
		CONST_SRC_CODE_WORLD_MATRIX0,
		CONST_SRC_CODE_INVERSE_WORLD_MATRIX0,
		CONST_SRC_CODE_TRANSPOSE_WORLD_MATRIX0,
		CONST_SRC_CODE_INVERSE_TRANSPOSE_WORLD_MATRIX0,
		CONST_SRC_CODE_WORLD_VIEW_MATRIX0,
		CONST_SRC_CODE_INVERSE_WORLD_VIEW_MATRIX0,
		CONST_SRC_CODE_TRANSPOSE_WORLD_VIEW_MATRIX0,
		CONST_SRC_CODE_INVERSE_TRANSPOSE_WORLD_VIEW_MATRIX0,
		CONST_SRC_CODE_WORLD_VIEW_PROJECTION_MATRIX0,
		CONST_SRC_CODE_INVERSE_WORLD_VIEW_PROJECTION_MATRIX0,
		CONST_SRC_CODE_TRANSPOSE_WORLD_VIEW_PROJECTION_MATRIX0,
		CONST_SRC_CODE_INVERSE_TRANSPOSE_WORLD_VIEW_PROJECTION_MATRIX0,
		CONST_SRC_CODE_WORLD_MATRIX1,
		CONST_SRC_CODE_INVERSE_WORLD_MATRIX1,
		CONST_SRC_CODE_TRANSPOSE_WORLD_MATRIX1,
		CONST_SRC_CODE_INVERSE_TRANSPOSE_WORLD_MATRIX1,
		CONST_SRC_CODE_WORLD_VIEW_MATRIX1,
		CONST_SRC_CODE_INVERSE_WORLD_VIEW_MATRIX1,
		CONST_SRC_CODE_TRANSPOSE_WORLD_VIEW_MATRIX1,
		CONST_SRC_CODE_INVERSE_TRANSPOSE_WORLD_VIEW_MATRIX1,
		CONST_SRC_CODE_WORLD_VIEW_PROJECTION_MATRIX1,
		CONST_SRC_CODE_INVERSE_WORLD_VIEW_PROJECTION_MATRIX1,
		CONST_SRC_CODE_TRANSPOSE_WORLD_VIEW_PROJECTION_MATRIX1,
		CONST_SRC_CODE_INVERSE_TRANSPOSE_WORLD_VIEW_PROJECTION_MATRIX1,
		CONST_SRC_CODE_WORLD_MATRIX2,
		CONST_SRC_CODE_INVERSE_WORLD_MATRIX2,
		CONST_SRC_CODE_TRANSPOSE_WORLD_MATRIX2,
		CONST_SRC_CODE_INVERSE_TRANSPOSE_WORLD_MATRIX2,
		CONST_SRC_CODE_WORLD_VIEW_MATRIX2,
		CONST_SRC_CODE_INVERSE_WORLD_VIEW_MATRIX2,
		CONST_SRC_CODE_TRANSPOSE_WORLD_VIEW_MATRIX2,
		CONST_SRC_CODE_INVERSE_TRANSPOSE_WORLD_VIEW_MATRIX2,
		CONST_SRC_CODE_WORLD_VIEW_PROJECTION_MATRIX2,
		CONST_SRC_CODE_INVERSE_WORLD_VIEW_PROJECTION_MATRIX2,
		CONST_SRC_CODE_TRANSPOSE_WORLD_VIEW_PROJECTION_MATRIX2,
		CONST_SRC_CODE_INVERSE_TRANSPOSE_WORLD_VIEW_PROJECTION_MATRIX2,
		CONST_SRC_TOTAL_COUNT,
		CONST_SRC_NONE,
	};

	enum MaterialTextureSource
	{
		TEXTURE_SRC_CODE_BLACK,
		TEXTURE_SRC_CODE_WHITE,
		TEXTURE_SRC_CODE_IDENTITY_NORMAL_MAP,
		TEXTURE_SRC_CODE_MODEL_LIGHTING,
		TEXTURE_SRC_CODE_LIGHTMAP_PRIMARY,
		TEXTURE_SRC_CODE_LIGHTMAP_SECONDARY,
		TEXTURE_SRC_CODE_SHADOWMAP_SUN,
		TEXTURE_SRC_CODE_SHADOWMAP_SPOT,
		TEXTURE_SRC_CODE_FEEDBACK,
		TEXTURE_SRC_CODE_RESOLVED_POST_SUN,
		TEXTURE_SRC_CODE_RESOLVED_SCENE,
		TEXTURE_SRC_CODE_POST_EFFECT_0,
		TEXTURE_SRC_CODE_POST_EFFECT_1,
		TEXTURE_SRC_CODE_LIGHT_ATTENUATION,
		TEXTURE_SRC_CODE_LIGHT_CUCOLORIS,
		TEXTURE_SRC_CODE_OUTDOOR,
		TEXTURE_SRC_CODE_FLOATZ,
		TEXTURE_SRC_CODE_PROCESSED_FLOATZ,
		TEXTURE_SRC_CODE_RAW_FLOATZ,
		TEXTURE_SRC_CODE_HALF_PARTICLES,
		TEXTURE_SRC_CODE_HALF_PARTICLES_Z,
		TEXTURE_SRC_CODE_CASE_TEXTURE,
		TEXTURE_SRC_CODE_CINEMATIC_Y,
		TEXTURE_SRC_CODE_CINEMATIC_CR,
		TEXTURE_SRC_CODE_CINEMATIC_CB,
		TEXTURE_SRC_CODE_CINEMATIC_A,
		TEXTURE_SRC_CODE_REFLECTION_PROBE,
		TEXTURE_SRC_CODE_PIP_SCENE,
		TEXTURE_SRC_CODE_COLOR_MANIPULATION,
		TEXTURE_SRC_CODE_STREAMING_LOADING,
		TEXTURE_SRC_CODE_COUNT
	};

	enum MaterialShaderArgumentType
	{
		MTL_ARG_MATERIAL_VERTEX_CONST = 0x0,
		MTL_ARG_LITERAL_VERTEX_CONST = 0x1,
		MTL_ARG_MATERIAL_VERTEX_SAMPLER = 0x2,
		MTL_ARG_MATERIAL_PIXEL_SAMPLER = 0x3,
		MTL_ARG_CODE_PRIM_BEGIN = 0x4,
		MTL_ARG_CODE_VERTEX_CONST = 0x4,
		MTL_ARG_CODE_PIXEL_SAMPLER = 0x5,
		MTL_ARG_CODE_PIXEL_CONST = 0x6,
		MTL_ARG_CODE_PRIM_END = 0x7,
		MTL_ARG_MATERIAL_PIXEL_CONST = 0x7,
		MTL_ARG_LITERAL_PIXEL_CONST = 0x8,
		MTL_ARG_COUNT = 0x9
	};

	struct MaterialArgumentCodeConst
	{
		unsigned short index;
		unsigned char firstRow;
		unsigned char rowCount;
	};

	union MaterialArgumentDef
	{
		x86::pointer<const float[4]> literalConst;
		MaterialArgumentCodeConst codeConst;
		unsigned int codeSampler;
		unsigned int nameHash;
	};

	struct MaterialShaderArgument
	{
		unsigned short type;
		unsigned short dest;
		MaterialArgumentDef u;
	};

	struct GfxPixelShaderLoadDef
	{
		x86::pointer<unsigned int> program;
		unsigned short programSize;
		unsigned short loadForRenderer;
	};

	struct MaterialPixelShaderProgram
	{
		x86::pointer<void> ps;
		GfxPixelShaderLoadDef loadDef;
	};

	struct MaterialPixelShader
	{
		x86::pointer<const char> name;
		MaterialPixelShaderProgram prog;
	};

	struct GfxVertexShaderLoadDef
	{
		x86::pointer<unsigned int> program;
		unsigned short programSize;
		unsigned short loadForRenderer;
	};

	struct MaterialVertexShaderProgram
	{
		x86::pointer<void> vs;
		GfxVertexShaderLoadDef loadDef;
	};

	struct MaterialVertexShader
	{
		x86::pointer<const char> name;
		MaterialVertexShaderProgram prog;
	};

	enum MaterialStreamRoutingSource : unsigned char
	{
		STREAM_SRC_POSITION = 0x0,
		STREAM_SRC_COLOR = 0x1,
		STREAM_SRC_TEXCOORD_0 = 0x2,
		STREAM_SRC_NORMAL = 0x3,
		STREAM_SRC_TANGENT = 0x4,
		STREAM_SRC_OPTIONAL_BEGIN = 0x5,
		STREAM_SRC_PRE_OPTIONAL_BEGIN = 0x4,
		STREAM_SRC_TEXCOORD_1 = 0x5,
		STREAM_SRC_TEXCOORD_2 = 0x6,
		STREAM_SRC_NORMAL_TRANSFORM_0 = 0x7,
		STREAM_SRC_NORMAL_TRANSFORM_1 = 0x8,
		STREAM_SRC_COUNT = 0x9,
	};

	enum MaterialStreamRoutingDestination : unsigned char
	{
		STREAM_DST_POSITION = 0x0,
		STREAM_DST_NORMAL = 0x1,
		STREAM_DST_COLOR_0 = 0x2,
		STREAM_DST_COLOR_1 = 0x3,
		STREAM_DST_DEPTH = 0x4,
		STREAM_DST_TEXCOORD_0 = 0x5,
		STREAM_DST_TEXCOORD_1 = 0x6,
		STREAM_DST_TEXCOORD_2 = 0x7,
		STREAM_DST_TEXCOORD_3 = 0x8,
		STREAM_DST_TEXCOORD_4 = 0x9,
		STREAM_DST_TEXCOORD_5 = 0xA,
		STREAM_DST_TEXCOORD_6 = 0xB,
		STREAM_DST_TEXCOORD_7 = 0xC,
		STREAM_DST_COUNT = 0xD,
	};

	struct MaterialStreamRouting
	{
		MaterialStreamRoutingSource source;
		MaterialStreamRoutingDestination dest;
	};

	union MaterialVertexStreamRouting
	{
		MaterialStreamRouting data[13];
		x86::pointer<void> decl[21];
	};

	struct MaterialVertexDeclaration
	{
		x86::pointer<const char> name;
		unsigned char streamCount;
		bool hasOptionalSource;
		MaterialVertexStreamRouting routing;
	};

	struct MaterialPass
	{
		x86::pointer<MaterialVertexDeclaration> vertexDecl;
		x86::pointer<MaterialVertexShader> vertexShader;
		x86::pointer<MaterialPixelShader> pixelShader;
		unsigned char perPrimArgCount;
		unsigned char perObjArgCount;
		unsigned char stableArgCount;
		unsigned char customSamplerFlags;
		x86::pointer<MaterialShaderArgument> args;
	};

	struct MaterialTechnique
	{
		x86::pointer<const char> name;
		unsigned short flags;
		unsigned short passCount;
		MaterialPass passArray[1];
	};

	struct MaterialTechniqueSet
	{
		x86::pointer<const char> name;
		unsigned char worldVertFormat;
		unsigned char unused[2];
		x86::pointer<MaterialTechniqueSet> remappedTechniqueSet;
		x86::pointer<MaterialTechnique> techniques[54];
	};

	struct GfxDrawSurfFields
	{
		unsigned long long unused : 1;
		unsigned long long primarySortKey : 6;
		unsigned long long surfType : 4;
		unsigned long long viewModelRender : 1;
		unsigned long long sceneLightIndex : 8;
		unsigned long long useHeroLighting : 1;
		unsigned long long prepass : 2;
		unsigned long long materialSortedIndex : 12;
		unsigned long long customIndex : 5;
		unsigned long long hasGfxEntIndex : 1;
		unsigned long long reflectionProbeIndex : 8;
		unsigned long long objectId : 15;
	};

	union GfxDrawSurf
	{
		GfxDrawSurfFields fields;
		unsigned long long packed;
	};

	struct MaterialInfo
	{
		x86::pointer<char> name;
		unsigned char gameFlags;
		unsigned char sortKey;
		unsigned char textureAtlasRowCount;
		unsigned char textureAtlasColumnCount;
		GfxDrawSurf drawSurf;
		unsigned int surfaceTypeBits;
	};

	struct WaterWritable
	{
		float floatTime;
	};

	struct complex_s
	{
		float real;
		float imag;
	};

	struct water_t
	{
		WaterWritable writable;
		x86::pointer<complex_s> H0;
		x86::pointer<float> wTerm;
		int M;
		int N;
		float Lx;
		float Lz;
		float gravity;
		float windvel;
		float winddir[2];
		float amplitude;
		float codeConstant[4];
		x86::pointer<GfxImage> image;
	};

	union MaterialTextureDefInfo
	{
		x86::pointer<GfxImage> image;
		x86::pointer<water_t> water;
	};

	struct MaterialTextureDef
	{
		unsigned int nameHash;
		char nameStart;
		char nameEnd;
		unsigned char samplerState;
		unsigned char semantic;
		MaterialTextureDefInfo u;
	};

	struct MaterialConstantDef
	{
		unsigned int nameHash;
		char name[12];
		float literal[4];
	};

	enum GfxSurfaceStatebitOp0 : unsigned int
	{
		GFXS0_SRCBLEND_RGB_SHIFT = 0x0,
		GFXS0_SRCBLEND_RGB_MASK = 0xF,
		GFXS0_DSTBLEND_RGB_SHIFT = 0x4,
		GFXS0_DSTBLEND_RGB_MASK = 0xF0,
		GFXS0_BLENDOP_RGB_SHIFT = 0x8,
		GFXS0_BLENDOP_RGB_MASK = 0x700,
		GFXS0_BLEND_RGB_MASK = 0x7FF,
		GFXS0_ATEST_DISABLE = 0x800,
		GFXS0_ATEST_GT_0 = 0x1000,
		GFXS0_ATEST_LT_128 = 0x2000,
		GFXS0_ATEST_GE_128 = 0x3000,
		GFXS0_ATEST_MASK = 0x3000,
		GFXS0_CULL_SHIFT = 0xE,
		GFXS0_CULL_NONE = 0x4000,
		GFXS0_CULL_BACK = 0x8000,
		GFXS0_CULL_FRONT = 0xC000,
		GFXS0_CULL_MASK = 0xC000,
		GFXS0_SRCBLEND_ALPHA_SHIFT = 0x10,
		GFXS0_SRCBLEND_ALPHA_MASK = 0xF0000,
		GFXS0_DSTBLEND_ALPHA_SHIFT = 0x14,
		GFXS0_DSTBLEND_ALPHA_MASK = 0xF00000,
		GFXS0_BLENDOP_ALPHA_SHIFT = 0x18,
		GFXS0_BLENDOP_ALPHA_MASK = 0x7000000,
		GFXS0_BLEND_ALPHA_MASK = 0x7FF0000,
		GFXS0_COLORWRITE_RGB = 0x8000000,
		GFXS0_COLORWRITE_ALPHA = 0x10000000,
		GFXS0_COLORWRITE_MASK = 0x18000000,
		GFXS0_GAMMAWRITE = 0x40000000,
		GFXS0_POLYMODE_LINE = 0x80000000
	};

	enum GfxSurfaceStatebitOp1 : unsigned int
	{
		GFXS1_DEPTHWRITE = 0x1,
		GFXS1_DEPTHTEST_DISABLE = 0x2,
		GFXS1_DEPTHTEST_SHIFT = 0x2,
		GFXS1_DEPTHTEST_ALWAYS = 0x0,
		GFXS1_DEPTHTEST_LESS = 0x4,
		GFXS1_DEPTHTEST_EQUAL = 0x8,
		GFXS1_DEPTHTEST_LESSEQUAL = 0xC,
		GFXS1_DEPTHTEST_MASK = 0xC,
		GFXS1_POLYGON_OFFSET_SHIFT = 0x4,
		GFXS1_POLYGON_OFFSET_0 = 0x0,
		GFXS1_POLYGON_OFFSET_1 = 0x10,
		GFXS1_POLYGON_OFFSET_2 = 0x20,
		GFXS1_POLYGON_OFFSET_SHADOWMAP = 0x30,
		GFXS1_POLYGON_OFFSET_MASK = 0x30,
		GFXS1_STENCIL_FRONT_ENABLE = 0x40,
		GFXS1_STENCIL_BACK_ENABLE = 0x80,
		GFXS1_STENCIL_MASK = 0xC0,
		GFXS1_STENCIL_FRONT_PASS_SHIFT = 0x8,
		GFXS1_STENCIL_FRONT_FAIL_SHIFT = 0xB,
		GFXS1_STENCIL_FRONT_ZFAIL_SHIFT = 0xE,
		GFXS1_STENCIL_FRONT_FUNC_SHIFT = 0x11,
		GFXS1_STENCIL_FRONT_MASK = 0xFFF00,
		GFXS1_STENCIL_BACK_PASS_SHIFT = 0x14,
		GFXS1_STENCIL_BACK_FAIL_SHIFT = 0x17,
		GFXS1_STENCIL_BACK_ZFAIL_SHIFT = 0x1A,
		GFXS1_STENCIL_BACK_FUNC_SHIFT = 0x1D,
		GFXS1_STENCIL_BACK_MASK = 0xFFF00000,
		GFXS1_STENCILFUNC_FRONTBACK_MASK = 0xE00E0000,
		GFXS1_STENCILOP_FRONTBACK_MASK = 0x1FF1FF00,
	};

	enum GfxStencilOp
	{
		GFXS_STENCILOP_KEEP = 0x0,
		GFXS_STENCILOP_ZERO = 0x1,
		GFXS_STENCILOP_REPLACE = 0x2,
		GFXS_STENCILOP_INCRSAT = 0x3,
		GFXS_STENCILOP_DECRSAT = 0x4,
		GFXS_STENCILOP_INVERT = 0x5,
		GFXS_STENCILOP_INCR = 0x6,
		GFXS_STENCILOP_DECR = 0x7,

		GFXS_STENCILOP_COUNT,
		GFXS_STENCILOP_MASK = 0x7
	};

	struct GfxStatebitsFlags {
		GfxSurfaceStatebitOp0 loadbit0;
		GfxSurfaceStatebitOp1 loadbit1;
	};

	union GfxStateBits
	{
		GfxStatebitsFlags flags;
		unsigned int loadBits[2];
	};

	enum GfxCameraRegionType : std::uint16_t
	{
		CAMERA_REGION_LIT_OPAQUE = 0x0,
		CAMERA_REGION_LIT_TRANS = 0x1,
		CAMERA_REGION_EMISSIVE = 0x2,
		CAMERA_REGION_DEPTH_HACK = 0x3,
		CAMERA_REGION_COUNT = 0x4,
		CAMERA_REGION_NONE = 0x4,
	};

	struct Material
	{
		union
		{
			x86::pointer<const char> name;
			MaterialInfo info;
		};
		unsigned char stateBitsEntry[54];
		unsigned char textureCount;
		unsigned char constantCount;
		unsigned char stateBitsCount;
		unsigned char stateFlags;
		unsigned char cameraRegion;
		x86::pointer<MaterialTechniqueSet> techniqueSet;
		x86::pointer<MaterialTextureDef> textureTable;
		x86::pointer<MaterialConstantDef> constantTable;
		x86::pointer<GfxStateBits> stateBitsTable;
		x86::pointer<x86::pointer<const char>> subMaterials;
	};

	enum snd_alias_type_t
	{
		SAT_UNKNOWN = 0x0,
		SAT_LOADED = 0x1,
		SAT_STREAMED = 0x2,
		SAT_VOICED = 0x3,
		SAT_COUNT = 0x4
	};

	struct StreamedSound
	{
		x86::pointer<const char> dir;
		x86::pointer<const char> name;
	};

	struct AILSOUNDINFO
	{
		int format;
		x86::pointer<const void> data_ptr;
		unsigned int data_len;
		unsigned int rate;
		int bits;
		int channels;
		unsigned int samples;
		unsigned int block_size;
		x86::pointer<const void> initial_ptr;
	};

	struct MssSound
	{
		AILSOUNDINFO info;
		x86::pointer<char> data;
	};

	struct LoadedSound
	{
		x86::pointer<char> name;
		MssSound sound;
	};

	union SoundFileRef
	{
		x86::pointer<LoadedSound> loadSnd;
		StreamedSound streamSnd;
	};

	struct SoundFile
	{
		unsigned char type;
		unsigned char exists;
		SoundFileRef u;
	};

	struct MSSSpeakerLevels
	{
		int speaker;
		int numLevels;
		float levels[2];
	};

	struct MSSChannelMap
	{
		int speakerCount;
		MSSSpeakerLevels speakers[6];
	};

	struct SpeakerMap
	{
		bool isDefault;
		x86::pointer<const char> name;
		MSSChannelMap channelMaps[2][2];
	};

	struct SndCurve
	{
		x86::pointer<const char> filename;
		unsigned short knotCount;
		float knots[16][2];
	};

	enum SoundVolMod : std::int16_t
	{
		SND_VOLMOD_FOLEY,
		SND_VOLMOD_WPNAI,
		SND_VOLMOD_WPNPLYR,
		SND_VOLMOD_HUD,
		SND_VOLMOD_INTERFACE,
		SND_VOLMOD_INTERFACE_MUSIC,
		SND_VOLMOD_MUSIC,
		SND_VOLMOD_MUSIC_EMITTER,
		SND_VOLMOD_AMBIENCE,
		SND_VOLMOD_AMBIENCE_DIST,
		SND_VOLMOD_ELEMENT,
		SND_VOLMOD_EMITTER,
		SND_VOLMOD_PHYSICS,
		SND_VOLMOD_BODYFALL,
		SND_VOLMOD_FOLEY_PLR,
		SND_VOLMOD_FOLEYMP_PLR,
		SND_VOLMOD_FOLEY_NPC,
		SND_VOLMOD_FOLEYMP_NPC,
		SND_VOLMOD_FOLEY_WPN_PLR,
		SND_VOLMOD_FOLEY_WPN_NPC,
		SND_VOLMOD_FOOTSTEPS_PLR,
		SND_VOLMOD_FOOTSTEPS_NPC,
		SND_VOLMOD_FOOTSTEPMP_PLR,
		SND_VOLMOD_FOOTSTEPMP_NPC,
		SND_VOLMOD_MELEE_PLR,
		SND_VOLMOD_MELEE_NPC,
		SND_VOLMOD_CHATTERAL,
		SND_VOLMOD_CHATTERRAX,
		SND_VOLMOD_REACTIONAL,
		SND_VOLMOD_REACTIONALMAX,
		SND_VOLMOD_VOICEOVER,
		SND_VOLMOD_VOICEOVER_RADIO,
		SND_VOLMOD_VOICEOVER_CRITICAL,
		SND_VOLMOD_VOICEOVER_AMB,
		SND_VOLMOD_DESTRUCT,
		SND_VOLMOD_EXPLOSION,
		SND_VOLMOD_EXPLOSION_GRENADE,
		SND_VOLMOD_EXPLOSION_FLASHBANG,
		SND_VOLMOD_EXPLOSION_ROCKET,
		SND_VOLMOD_EXPLOSION_CAR,
		SND_VOLMOD_IMPACT,
		SND_VOLMOD_IMPACT_PLR,
		SND_VOLMOD_IMPACT_NPC,
		SND_VOLMOD_IMPACTMP,
		SND_VOLMOD_IMPACTMP_PLR,
		SND_VOLMOD_IMPACTMP_NPC,
		SND_VOLMOD_WHIZBY,
		SND_VOLMOD_WHIZBYMP,
		SND_VOLMOD_VEHICLE_PLR,
		SND_VOLMOD_VEHICLE_NPC,
		SND_VOLMOD_VEHICLE_WPN_PLR,
		SND_VOLMOD_VEHICLE_WPN_NPC,
		SND_VOLMOD_VEHICLE,
		SND_VOLMOD_GRENADEBOUNCE,
		SND_VOLMOD_GRENADEBOUNCEMP,
		SND_VOLMOD_SHELLCASINGS,
		SND_VOLMOD_SHELLCASINGSMP,
		SND_VOLMOD_WPN_PLR,
		SND_VOLMOD_WPNMP_PLR,
		SND_VOLMOD_WPN_NPC,
		SND_VOLMOD_WPNMP_NPC,
		SND_VOLMOD_WPN_PROJECTILE,
		SND_VOLMOD_WPNMP_PROJECTILE,
		SND_VOLMOD_NA,
		SND_VOLMOD_MAX,
		SND_VOLMOD_SCRIPTED1,
		SND_VOLMOD_SCRIPTED2,
		SND_VOLMOD_SCRIPTED3,
		SND_VOLMOD_SCRIPTED4,
		SND_VOLMOD_SCRIPTED5,
		SND_VOLMOD_FULLVOLUME,
		SND_VOLMOD_PERKMP_QUIET,
		SND_VOLMOD_LEVEL_AC130,
		SND_VOLMOD_DEFAULT,

		SND_VOLMOD_COUNT,
	};

	enum SoundChannel : std::uint32_t
	{
		SND_CHANNEL_PHYSICS,
		SND_CHANNEL_AMBDIST1,
		SND_CHANNEL_AMBDIST2,
		SND_CHANNEL_ALARM,
		SND_CHANNEL_AUTO,
		SND_CHANNEL_AUTO2,
		SND_CHANNEL_AUTO2D,
		SND_CHANNEL_AUTODOG,
		SND_CHANNEL_EXPLOSIONDIST1,
		SND_CHANNEL_EXPLOSIONDIST2,
		SND_CHANNEL_EXPLOSIVEIMPACT,
		SND_CHANNEL_ELEMENT,
		SND_CHANNEL_ELEMENT_INT,
		SND_CHANNEL_ELEMENT_EXT,
		SND_CHANNEL_BULLETIMPACT,
		SND_CHANNEL_BULLETFLESH1,
		SND_CHANNEL_BULLETFLESH2,
		SND_CHANNEL_BULLETWHIZBY,
		SND_CHANNEL_VEHICLE,
		SND_CHANNEL_VEHICLELIMITED,
		SND_CHANNEL_MENU,
		SND_CHANNEL_BODY,
		SND_CHANNEL_BODY2D,
		SND_CHANNEL_RELOAD,
		SND_CHANNEL_RELOAD2D,
		SND_CHANNEL_ITEM,
		SND_CHANNEL_EXPLOSION1,
		SND_CHANNEL_EXPLOSION2,
		SND_CHANNEL_EXPLOSION3,
		SND_CHANNEL_EXPLOSION4,
		SND_CHANNEL_EXPLOSION5,
		SND_CHANNEL_EFFECTS1,
		SND_CHANNEL_EFFECTS2,
		SND_CHANNEL_EFFECTS3,
		SND_CHANNEL_EFFECTS2D1,
		SND_CHANNEL_EFFECTS2D2,
		SND_CHANNEL_NORESTRICT,
		SND_CHANNEL_NORESTRICT2D,
		SND_CHANNEL_AIRCRAFT,
		SND_CHANNEL_VEHICLE2D,
		SND_CHANNEL_WEAPON_DIST,
		SND_CHANNEL_WEAPON_MID,
		SND_CHANNEL_WEAPON,
		SND_CHANNEL_WEAPON2D,
		SND_CHANNEL_NONSHOCK,
		SND_CHANNEL_NONSHOCK2,
		SND_CHANNEL_GRONDO3D,
		SND_CHANNEL_GRONDO2D,
		SND_CHANNEL_VOICE,
		SND_CHANNEL_LOCAL,
		SND_CHANNEL_LOCAL2,
		SND_CHANNEL_LOCAL3,
		SND_CHANNEL_AMBIENT,
		SND_CHANNEL_HURT,
		SND_CHANNEL_PLAYER1,
		SND_CHANNEL_PLAYER2,
		SND_CHANNEL_MUSIC,
		SND_CHANNEL_MUSICNOPAUSE,
		SND_CHANNEL_MISSION,
		SND_CHANNEL_CRITICAL,
		SND_CHANNEL_ANNOUNCER,
		SND_CHANNEL_SHELLSHOCK,

		SND_CHANNEL_COUNT,
	};

	union SoundAliasFlags
	{
#pragma warning(push)
#pragma warning(disable: 4201)
		struct
		{
			unsigned int looping : 1;		// & 1	/ 0x1			/ 0000 0000 0000 0001
			unsigned int isMaster : 1;		// & 2	/ 0x2			/ 0000 0000 0000 0010
			unsigned int isSlave : 1;		// & 4	/ 0x4			/ 0000 0000 0000 0100
			unsigned int fullDryLevel : 1;	//	& 8	/ 0x8			/ 0000 0000 0000 1000
			unsigned int noWetLevel : 1;	// & 16	/ 0x10			/ 0000 0000 0001 0000
			unsigned int randomLooping : 1;		// & 32	/ 0x20			/ 0000 0000 0010 0000
			unsigned int spatialize : 1;		// & 64	/ 0x40			/ 0000 0000 0100 0000		// CONFIRMED IW4 IW5
			unsigned int type : 2;			// & 384	/ 0x180		/ 0000 0001 1000 0000		// CONFIRMED IW4 IW5
			unsigned int channel : 6;		// & 32256	/ 0x7E00	/ 0111 1110 0000 0000		// CONFIRMED IW4 IW5
		};
#pragma warning(pop)
		unsigned int intValue;
	};

	struct snd_alias_t
	{
		x86::pointer<const char> aliasName;
		x86::pointer<const char> subtitle;
		x86::pointer<const char> secondaryAliasName;
		x86::pointer<const char> chainAliasName;
		x86::pointer<const char> mixerGroup;
		x86::pointer<SoundFile> soundFile;
		int sequence;
		float volMin;
		float volMax;
		int volModIndex;
		float pitchMin;
		float pitchMax;
		float distMin;
		float distMax;
		float velocityMin;
		SoundAliasFlags flags;
		unsigned char masterPriority;
		float masterPercentage;
		float slavePercentage;
		float probability;
		float lfePercentage;
		float centerPercentage;
		int startDelay;
		x86::pointer<SndCurve> volumeFalloffCurve;
		float envelopMin;
		float envelopMax;
		float envelopPercentage;
		x86::pointer<SpeakerMap> speakerMap;
	};

	struct snd_alias_list_t
	{
		x86::pointer<const char> aliasName;
		x86::pointer<snd_alias_t> head;
		int count;
	};

	struct Bounds
	{
		float midPoint[3];
		float halfSize[3];
	};

	struct TriggerModel
	{
		int contents;
		unsigned short hullCount;
		unsigned short firstHull;
	};

	struct TriggerHull
	{
		Bounds bounds;
		int contents;
		unsigned short slabCount;
		unsigned short firstSlab;
	};

	struct TriggerSlab
	{
		float dir[3];
		float midPoint;
		float halfSize;
	};

	struct MapTriggers
	{
		unsigned int count;
		x86::pointer<TriggerModel> models;
		unsigned int hullCount;
		x86::pointer<TriggerHull> hulls;
		unsigned int slabCount;
		x86::pointer<TriggerSlab> slabs;
	};

	struct ClientTriggerAabbNode
	{
		Bounds bounds;
		unsigned short firstChild;
		unsigned short childCount;
	};

	enum clientTriggerType_t : __int32
	{
		CLIENT_TRIGGER_NONE = 0x0,
		CLIENT_TRIGGER_VISIONSET = 0x1,
		CLIENT_TRIGGER_REVERB = 0x2,
	};

	struct ClientTriggers
	{
		MapTriggers trigger;
		unsigned short numClientTriggerNodes;
		x86::pointer<ClientTriggerAabbNode> clientTriggerAabbTree;
		unsigned int triggerStringLength;
		x86::pointer<char> triggerString;
		x86::pointer<unsigned short> triggerStringOffsets;
		x86::pointer<unsigned char> triggerType;
		x86::pointer<vec3_t> origins;
		x86::pointer<float> scriptDelay;
		x86::pointer<short> audioTriggers;
	};

	struct Stage
	{
		x86::pointer<const char> name;
		float origin[3];
		unsigned short triggerIndex;
		unsigned char sunPrimaryLightIndex;
	};

	struct MapEnts
	{
		x86::pointer<const char> name;
		x86::pointer<char> entityString;
		int numEntityChars;
		MapTriggers trigger;
		ClientTriggers clientTrigger;
	};

	struct AddonMapEnts
	{
		x86::pointer<const char> name;
		x86::pointer<char> entityString;
		int numEntityChars;
		MapTriggers trigger;
	};

	struct GfxSky
	{
		int skySurfCount;
		x86::pointer<int> skyStartSurfs;
		x86::pointer<GfxImage> skyImage;
		unsigned char skySamplerState;
	};

	struct cplane_s
	{
		float normal[3];
		float dist;
		unsigned char type;
		unsigned char pad[3];
	};

	struct cbrushside_t
	{
		x86::pointer<cplane_s> plane;
		unsigned short materialNum;
		unsigned char firstAdjacentSideOffset;
		unsigned char edgeCount;
	};

	struct cbrushWrapper_t
	{
		unsigned short numsides;
		unsigned short glassPieceIndex;
		x86::pointer<cbrushside_t> sides;
		x86::pointer<unsigned char> baseAdjacentSide;
		short axialMaterialNum[2][3];
		unsigned char firstAdjacentSideOffsets[2][3];
		unsigned char edgeCount[2][3];
	};

	struct BrushWrapper
	{
		Bounds bounds;
		cbrushWrapper_t brush;
		int totalEdgeCount;
		x86::pointer<cplane_s> planes;
	};

	enum PathNodeErrorCode : __int32
	{
		PNERR_NONE = 0x0,
		PNERR_INSOLID = 0x1,
		PNERR_FLOATING = 0x2,
		PNERR_NOLINK = 0x3,
		PNERR_DUPLICATE = 0x4,
		PNERR_NOSTANCE = 0x5,
		PNERR_INVALIDDOOR = 0x6,
		PNERR_NOANGLES = 0x7,
		PNERR_BADPLACEMENT = 0x8,
		NUM_PATH_NODE_ERRORS = 0x9,
	};

	union $89ADD21C62292EF448FA48DB653E4127
	{
		float minUseDistSq;
		PathNodeErrorCode error;
	};

	struct pathlink_s
	{
		float fDist;
		unsigned __int16 nodeNum;
		unsigned __int8 disconnectCount;
		unsigned __int8 negotiationLink;
		unsigned __int8 flags;
		unsigned __int8 ubBadPlaceCount[3];
	};

	enum nodeType : __int32
	{
		NODE_ERROR = 0x0,
		NODE_PATHNODE = 0x1,
		NODE_COVER_STAND = 0x2,
		NODE_COVER_CROUCH = 0x3,
		NODE_COVER_CROUCH_WINDOW = 0x4,
		NODE_COVER_PRONE = 0x5,
		NODE_COVER_RIGHT = 0x6,
		NODE_COVER_LEFT = 0x7,
		NODE_AMBUSH = 0x8,
		NODE_EXPOSED = 0x9,
		NODE_CONCEALMENT_STAND = 0xA,
		NODE_CONCEALMENT_CROUCH = 0xB,
		NODE_CONCEALMENT_PRONE = 0xC,
		NODE_DOOR = 0xD,
		NODE_DOOR_INTERIOR = 0xE,
		NODE_SCRIPTED = 0xF,
		NODE_NEGOTIATION_BEGIN = 0x10,
		NODE_NEGOTIATION_END = 0x11,
		NODE_TURRET = 0x12,
		NODE_GUARD = 0x13,
		NODE_NUMTYPES = 0x14,
		NODE_DONTLINK = 0x14,
	};

	struct pathnode_constant_t
	{
		nodeType type;
		unsigned __int16 spawnflags;
		unsigned __int16 targetname;
		unsigned __int16 script_linkName;
		unsigned __int16 script_noteworthy;
		unsigned __int16 target;
		unsigned __int16 animscript;
		int animscriptfunc;
		float vOrigin[3];
		float fAngle;
		float forward[2];
		float fRadius;
		$89ADD21C62292EF448FA48DB653E4127 ___u12;
		__int16 wOverlapNode[2];
		unsigned __int16 totalLinkCount;
		x86::pointer<pathlink_s> Links;
	};

	struct SentientHandle
	{
		unsigned __int16 number;
		unsigned __int16 infoIndex;
	};

	struct pathnode_dynamic_t
	{
		SentientHandle pOwner;
		int iFreeTime;
		int iValidTime[3];
		int dangerousNodeTime[3];
		int inPlayerLOSTime;
		__int16 wLinkCount;
		__int16 wOverlapCount;
		__int16 turretEntNumber;
		unsigned __int8 userCount;
		bool hasBadPlaceLink;
	};

	struct pathnode_t;

	union $73F238679C0419BE2C31C6559E8604FC
	{
		float nodeCost;
		int linkIndex;
	};

	struct pathnode_transient_t
	{
		int iSearchFrame;
		x86::pointer<pathnode_t> pNextOpen;
		x86::pointer<pathnode_t> pPrevOpen;
		x86::pointer<pathnode_t> pParent;
		float fCost;
		float fHeuristic;
		$73F238679C0419BE2C31C6559E8604FC ___u6;
	};

	struct pathnode_t
	{
		pathnode_constant_t constant;
		pathnode_dynamic_t dynamic;
		pathnode_transient_t transient;
	};

	struct pathbasenode_t
	{
		float vOrigin[3];
		unsigned int type;
	};

	struct pathnode_tree_t;

	struct pathnode_tree_nodes_t
	{
		int nodeCount;
		x86::pointer<unsigned __int16> nodes;
	};

	union pathnode_tree_info_t
	{
		x86::pointer<pathnode_tree_t> child[2];
		pathnode_tree_nodes_t s;
	};

	struct pathnode_tree_t
	{
		int axis;
		float dist;
		pathnode_tree_info_t u;
	};

	struct PathData
	{
		x86::pointer<const char> name;
		unsigned int nodeCount;
		x86::pointer<pathnode_t> nodes;
		x86::pointer<pathbasenode_t> basenodes;
		unsigned int chainNodeCount;
		x86::pointer<unsigned __int16> chainNodeForNode;
		x86::pointer<unsigned __int16> nodeForChainNode;
		int visBytes;
		x86::pointer<unsigned __int8> pathVis;
		int nodeTreeCount;
		x86::pointer<pathnode_tree_t> nodeTree;
	};

	enum PhysicsGeomType
	{
		PHYS_GEOM_NONE = 0x0,
		PHYS_GEOM_BOX = 0x1,
		PHYS_GEOM_BRUSHMODEL = 0x2,
		PHYS_GEOM_BRUSH = 0x3,
		PHYS_GEOM_COLLMAP = 0x4,
		PHYS_GEOM_CYLINDER = 0x5,
		PHYS_GEOM_CAPSULE = 0x6,
		PHYS_GEOM_GLASS = 0x7,
		PHYS_GEOM_COUNT = 0x8
	};

	struct PhysGeomInfo
	{
		x86::pointer<BrushWrapper> brushWrapper;
		PhysicsGeomType type;
		float orientation[3][3];
		Bounds bounds;
	};

	struct PhysMass
	{
		float centerOfMass[3];
		float momentsOfInertia[3];
		float productsOfInertia[3];
	};

	struct PhysCollmap
	{
		x86::pointer<const char> name;
		unsigned int count;
		x86::pointer<PhysGeomInfo> geoms;
		PhysMass mass;
		Bounds bounds;
	};

	struct GfxWorldDpvsPlanes
	{
		int cellCount;
		x86::pointer<cplane_s> planes;
		x86::pointer<unsigned short> nodes;
		x86::pointer<unsigned int> sceneEntCellBits;
	};

	struct GfxAabbTree
	{
		Bounds bounds;
		unsigned short childCount;
		unsigned short surfaceCount;
		unsigned short startSurfIndex;
		unsigned short surfaceCountNoDecal;
		unsigned short startSurfIndexNoDecal;
		unsigned short smodelIndexCount;
		x86::pointer<unsigned short> smodelIndexes;
		int childrenOffset;
	};

	struct GfxCellTree
	{
		x86::pointer<GfxAabbTree> aabbTree;
	};

	struct GfxCellTreeCount
	{
		int aabbTreeCount;
	};

	struct GfxPortalWritable
	{
		bool isQueued;
		bool isAncestor;
		unsigned char recursionDepth;
		unsigned char hullPointCount;
		x86::pointer<float[2]> hullPoints;
		x86::pointer<struct GfxPortal> queuedParent;
	};

	struct DpvsPlane
	{
		float coeffs[4];
	};

	struct GfxPortal
	{
		GfxPortalWritable writable;
		DpvsPlane plane;
		x86::pointer<vec3_t> vertices;
		unsigned short cellIndex;
		unsigned char vertexCount;
		float hullAxis[2][3];
	};

	struct GfxCell
	{
		Bounds bounds;
		int portalCount;
		x86::pointer<GfxPortal> portals;
		unsigned char reflectionProbeCount;
		x86::pointer<unsigned char> reflectionProbes;
		unsigned char reflectionProbeReferenceCount;
		x86::pointer<unsigned char> reflectionProbeReferences;
	};

	struct GfxReflectionProbe
	{
		float origin[3];
	};

	struct GfxReflectionProbeReferenceOrigin
	{
		float origin[3];
	};

	struct GfxReflectionProbeReference
	{
		unsigned char index;
	};

	struct GfxLightmapArray
	{
		x86::pointer<GfxImage> primary;
		x86::pointer<GfxImage> secondary;
	};

	union PackedUnitVec
	{
		unsigned int packed;
		unsigned char array[4];
	};

	union GfxColor
	{
		unsigned int packed;
		unsigned char array[4];
	};

	struct GfxPackedPlacement
	{
		float origin[3];
		float axis[3][3];
		float scale;
	};

	struct GfxWorldVertex
	{
		float xyz[3];
		float binormalSign;
		GfxColor color;
		float texCoord[2];
		float lmapCoord[2];
		PackedUnitVec normal;
		PackedUnitVec tangent;
	};

	struct GfxWorldVertexData
	{
		x86::pointer<GfxWorldVertex> vertices;
		x86::pointer<void> worldVb;
	};

	struct GfxWorldVertexLayerData
	{
		x86::pointer<unsigned char> data;
		x86::pointer<void> layerVb;
	};

	struct GfxWorldDraw
	{
		unsigned int reflectionProbeCount;
		x86::pointer<x86::pointer<GfxImage>> reflectionProbes;
		x86::pointer<GfxReflectionProbe> reflectionProbeOrigins;
		x86::pointer<GfxTexture> reflectionProbeTextures;
		unsigned int reflectionProbeReferenceCount;
		x86::pointer<GfxReflectionProbeReferenceOrigin> reflectionProbeReferenceOrigins;
		x86::pointer<GfxReflectionProbeReference> reflectionProbeReferences;
		int lightmapCount;
		x86::pointer<GfxLightmapArray> lightmaps;
		x86::pointer<GfxTexture> lightmapPrimaryTextures;
		x86::pointer<GfxTexture> lightmapSecondaryTextures;
		x86::pointer<GfxImage> lightmapOverridePrimary;
		x86::pointer<GfxImage> lightmapOverrideSecondary;
		unsigned int vertexCount;
		GfxWorldVertexData vd;
		unsigned int vertexLayerDataSize;
		GfxWorldVertexLayerData vld;
		unsigned int indexCount;
		x86::pointer<unsigned short> indices;
	};

	struct GfxLightGridEntry
	{
		unsigned short colorsIndex;
		unsigned char primaryLightIndex;
		unsigned char needsTrace;
	};

	struct GfxLightGridColors
	{
		unsigned char rgb[56][3];
	};

	struct GfxLightGrid
	{
		bool hasLightRegions;
		bool useSkyForLowZ;
		unsigned int lastSunPrimaryLightIndex;
		unsigned short mins[3];
		unsigned short maxs[3];
		unsigned int rowAxis;
		unsigned int colAxis;
		x86::pointer<unsigned short> rowDataStart;
		unsigned int rawRowDataSize;
		x86::pointer<unsigned char> rawRowData;
		unsigned int entryCount;
		x86::pointer<GfxLightGridEntry> entries;
		unsigned int colorCount;
		x86::pointer<GfxLightGridColors> colors;
	};

	struct GfxBrushModelWritable
	{
		Bounds bounds;
	};

	struct GfxBrushModel
	{
		GfxBrushModelWritable writable;
		Bounds bounds;
		float radius;
		unsigned short surfaceCount;
		unsigned short startSurfIndex;
		unsigned short surfaceCountNoDecal;
	};

	struct MaterialMemory
	{
		x86::pointer<Material> material;
		int memory;
	};

	struct sunflare_t
	{
		bool hasValidData;
		x86::pointer<Material> spriteMaterial;
		x86::pointer<Material> flareMaterial;
		float spriteSize;
		float flareMinSize;
		float flareMinDot;
		float flareMaxSize;
		float flareMaxDot;
		float flareMaxAlpha;
		int flareFadeInTime;
		int flareFadeOutTime;
		float blindMinDot;
		float blindMaxDot;
		float blindMaxDarken;
		int blindFadeInTime;
		int blindFadeOutTime;
		float glareMinDot;
		float glareMaxDot;
		float glareMaxLighten;
		int glareFadeInTime;
		int glareFadeOutTime;
		float sunFxPosition[3];
	};

	struct BModelDrawInfo
	{
		unsigned short surfId;
	};

	struct GfxSceneDynBrush
	{
		BModelDrawInfo info;
		unsigned short dynEntId;
	};

	struct XModelDrawInfo
	{
		unsigned char hasGfxEntIndex;
		unsigned char lod;
		unsigned short surfId;
	};

	struct GfxSceneDynModel
	{
		XModelDrawInfo info;
		unsigned short dynEntId;
	};

	struct GfxShadowGeometry
	{
		unsigned short  surfaceCount;
		unsigned short  smodelCount;
		x86::pointer<unsigned short> sortedSurfIndex;
		x86::pointer<unsigned short> smodelIndex;
	};

	struct GfxLightRegionAxis
	{
		float dir[3];
		float midPoint;
		float halfSize;
	};

	struct GfxLightRegionHull
	{
		float kdopMidPoint[9];
		float kdopHalfSize[9];
		unsigned int axisCount;
		x86::pointer<GfxLightRegionAxis> axis;
	};

	struct GfxLightRegion
	{
		unsigned int hullCount;
		x86::pointer<GfxLightRegionHull> hulls;
	};

	struct GfxWorldDpvsDynamic
	{
		unsigned int dynEntClientWordCount[2];
		unsigned int dynEntClientCount[2];
		x86::pointer<unsigned int> dynEntCellBits[2];
		x86::pointer<char> dynEntVisData[2][3];
	};

	union PackedTexCoords
	{
		unsigned int packed;
	};

	struct GfxQuantizedNoColorVertex
	{
		short xyz[3];
		short binormalSign;
		PackedUnitVec normal;
		PackedUnitVec tangent;
		PackedTexCoords texCoord;
	};

	struct GfxQuantizedVertex
	{
		short xyz[3];
		short binormalSign;
		PackedUnitVec normal;
		PackedUnitVec tangent;
		PackedTexCoords texCoord;
		GfxColor color;
	};

	struct GfxPackedVertex
	{
		float xyz[3];
		float binormalSign;
		GfxColor color;
		PackedTexCoords texCoord;
		PackedUnitVec normal;
		PackedUnitVec tangent;
	};

	union GfxVertexUnion0
	{
		x86::pointer<GfxQuantizedNoColorVertex> quantizedNoColorVerts0;
		x86::pointer<GfxQuantizedVertex> quantizedVerts0;
		x86::pointer<GfxPackedVertex> packedVerts0;
		x86::pointer<void> verts0;
	};

	struct XSurfaceCollisionAabb
	{
		unsigned short mins[3];
		unsigned short maxs[3];
	};

	struct XSurfaceCollisionNode
	{
		XSurfaceCollisionAabb aabb;
		unsigned short childBeginIndex;
		unsigned short childCount;
	};

	struct XSurfaceCollisionLeaf
	{
		unsigned short triangleBeginIndex;
	};

	struct XSurfaceCollisionTree
	{
		float trans[3];
		float scale[3];
		unsigned int nodeCount;
		x86::pointer<XSurfaceCollisionNode> nodes;
		unsigned int leafCount;
		x86::pointer<XSurfaceCollisionLeaf> leafs;
	};

	struct XRigidVertList
	{
		unsigned short boneOffset;
		unsigned short vertCount;
		unsigned short triOffset;
		unsigned short triCount;
		x86::pointer<XSurfaceCollisionTree> collisionTree;
	};

	struct XSurfaceVertexInfo
	{
		short vertCount[4];
		x86::pointer<unsigned short> vertsBlend;
	};

	enum XSurfaceFlags : std::uint8_t
	{
		SURF_FLAG_VERTCOL_GREY = 0x8,
		SURF_FLAG_VERTCOL_NONE = 0x10,
		SURF_FLAG_QUANTIZED = 0x20,
		SURF_FLAG_SKINNED = 0x40,
	};

	struct XSurface
	{
		unsigned char tileMode;
		unsigned char flags;
		unsigned short vertCount;
		unsigned short triCount;
		char zoneHandle;
		uint16_t baseTriIndex;
		uint16_t baseVertIndex;
		float quantizeScale;
		x86::pointer<unsigned short> triIndices;
		XSurfaceVertexInfo vertInfo;
		GfxVertexUnion0 verts0;
		unsigned int vertListCount;
		x86::pointer<XRigidVertList> vertList;
		int partBits[6];
	};

	enum PhysPresetScaling
	{
		PHYSPRESET_SCALING_LINEAR = 0x0,
		PHYSPRESET_SCALING_QUADRATIC = 0x1,
		PHYSPRESET_SCALING_COUNT = 0x2
	};

	struct PhysPreset
	{
		x86::pointer<const char> name;
		int type;
		float mass;
		float bounce;
		float friction;
		float bulletForceScale;
		float explosiveForceScale;
		x86::pointer<const char> sndAliasPrefix;
		float piecesSpreadFraction;
		float piecesUpwardVelocity;
		float minMomentum;
		float maxMomentum;
		float minPitch;
		float maxPitch;
		PhysPresetScaling volumeType;
		PhysPresetScaling pitchType;
		bool tempDefaultToCylinder;
		bool perSurfaceSndAlias;
	};

	struct XModelSurfs
	{
		x86::pointer<const char> name;
		x86::pointer<XSurface> surfs;
		unsigned short numsurfs;
		int partBits[6];
	};

	struct DObjAnimMat
	{
		float quat[4];
		float trans[3];
		float transWeight;
	};

	struct XModelLodInfo
	{
		float dist;
		unsigned short numsurfs;
		unsigned short surfIndex;
		x86::pointer<XModelSurfs> modelSurfs;
		int partBits[6];
		x86::pointer<XSurface> surfs;
		char lod;
		char smcBaseIndexPlusOne;
		char smcSubIndexMask;
		char smcBucket;
	};

	struct XModelCollTri_s
	{
		float plane[4];
		float svec[4];
		float tvec[4];
	};

	struct XModelCollSurf_s
	{
		x86::pointer<XModelCollTri_s> collTris;
		int numCollTris;
		Bounds bounds;
		int boneIdx;
		int contents;
		int surfFlags;
	};

	struct XBoneInfo
	{
		Bounds bounds;
		float radiusSquared;
	};

	struct XModel
	{
		x86::pointer<const char> name;
		unsigned char numBones;
		unsigned char numRootBones;
		unsigned char numsurfs;
		float scale;
		unsigned int noScalePartBits[6];
		x86::pointer<unsigned short> boneNames;
		x86::pointer<unsigned char> parentList;
		x86::pointer<short[4]> quats;
		x86::pointer<float[3]> trans;
		x86::pointer<unsigned char> partClassification;
		x86::pointer<DObjAnimMat> baseMat;
		x86::pointer<x86::pointer<Material>> materialHandles;
		XModelLodInfo lodInfo[4];
		char maxLoadedLod;
		unsigned char numLods;
		unsigned char collLod;
		unsigned char flags;
		x86::pointer<XModelCollSurf_s> collSurfs;
		int numCollSurfs;
		int contents;
		x86::pointer<XBoneInfo> boneInfo;
		float radius;
		Bounds bounds;
		x86::pointer<unsigned short> invHighMipRadius;
		int memUsage;
		x86::pointer<PhysPreset> physPreset;
		x86::pointer<PhysCollmap> physCollmap;
		float quantization;
	};

	struct GfxStaticModelDrawInst
	{
		GfxPackedPlacement placement;
		x86::pointer<XModel> model;
		unsigned short cullDist;
		unsigned short lightingHandle;
		unsigned char reflectionProbeIndex;
		unsigned char primaryLightIndex;
		unsigned char flags;
		unsigned char firstMtlSkinIndex;
		GfxColor groundLighting;
		unsigned short cacheId[4];
	};

	struct GfxStaticModelInst
	{
		Bounds bounds;
		float lightingOrigin[3];
	};

	struct srfTriangles_t
	{
		unsigned int vertexLayerData;
		unsigned int firstVertex;
		unsigned short vertexCount;
		unsigned short triCount;
		unsigned int baseIndex;
	};

	struct GfxSurfaceLightingAndFlagsFields
	{
		unsigned char lightmapIndex;
		unsigned char reflectionProbeIndex;
		unsigned char primaryLightIndex;
		unsigned char flags;
	};

	union GfxSurfaceLightingAndFlags
	{
		GfxSurfaceLightingAndFlagsFields fields;
		unsigned int packed;
	};

	struct GfxSurface
	{
		srfTriangles_t tris;
		x86::pointer<Material> material;
		GfxSurfaceLightingAndFlags laf;
	};

	struct GfxSurfaceBounds
	{
		Bounds bounds;
	};

	struct GfxWorldDpvsStatic
	{
		unsigned int smodelCount;
		unsigned int staticSurfaceCount;
		unsigned int staticSurfaceCountNoDecal;
		unsigned int litOpaqueSurfsBegin;
		unsigned int litOpaqueSurfsEnd;
		unsigned int litTransSurfsBegin;
		unsigned int litTransSurfsEnd;
		unsigned int shadowCasterSurfsBegin;
		unsigned int shadowCasterSurfsEnd;
		unsigned int emissiveSurfsBegin;
		unsigned int emissiveSurfsEnd;
		unsigned int smodelVisDataCount;
		unsigned int surfaceVisDataCount;
		x86::pointer<char> smodelVisData[3];
		x86::pointer<char> surfaceVisData[3];
		x86::pointer<unsigned short> sortedSurfIndex;
		x86::pointer<GfxStaticModelInst> smodelInsts;
		x86::pointer<GfxSurface> surfaces;
		x86::pointer<GfxSurfaceBounds> surfacesBounds;
		x86::pointer<GfxStaticModelDrawInst> smodelDrawInsts;
		x86::pointer<GfxDrawSurf> surfaceMaterials;
		x86::pointer<unsigned int> surfaceCastsSunShadow;
		volatile int usageCount;
	};

	struct GfxWorld
	{
		x86::pointer<const char> name;
		x86::pointer<const char> baseName;
		int planeCount;
		int nodeCount;
		unsigned int surfaceCount;
		int skyCount;
		x86::pointer<GfxSky> skies;
		unsigned int lastSunPrimaryLightIndex;
		unsigned int primaryLightCount;
		unsigned int sortKeyLitDecal;
		unsigned int sortKeyEffectDecal;
		unsigned int sortKeyEffectAuto;
		unsigned int sortKeyDistortion;
		GfxWorldDpvsPlanes dpvsPlanes;
		x86::pointer<GfxCellTreeCount> aabbTreeCounts;
		x86::pointer<GfxCellTree> aabbTrees;
		x86::pointer<GfxCell> cells;
		GfxWorldDraw draw;
		GfxLightGrid lightGrid;
		int modelCount;
		x86::pointer<GfxBrushModel> models;
		Bounds bounds;
		unsigned int checksum;
		int materialMemoryCount;
		x86::pointer<MaterialMemory> materialMemory;
		sunflare_t sun;
		float outdoorLookupMatrix[4][4];
		x86::pointer<GfxImage> outdoorImage;
		x86::pointer<unsigned int> cellCasterBits;
		x86::pointer<unsigned int> cellHasSunLitSurfsBits;
		x86::pointer<GfxSceneDynModel> sceneDynModel;
		x86::pointer<GfxSceneDynBrush> sceneDynBrush;
		x86::pointer<unsigned int> primaryLightEntityShadowVis;
		x86::pointer<unsigned int> primaryLightDynEntShadowVis[2];
		x86::pointer<unsigned char> nonSunPrimaryLightForModelDynEnt;
		x86::pointer<GfxShadowGeometry> shadowGeom;
		x86::pointer<GfxLightRegion> lightRegion;
		GfxWorldDpvsStatic dpvs;
		GfxWorldDpvsDynamic dpvsDyn;
		unsigned int mapVtxChecksum;
		unsigned int heroOnlyLightCount;
		x86::pointer<struct GfxHeroOnlyLight> heroOnlyLights;
		unsigned char fogTypesAllowed;
	};

	struct GfxHeroOnlyLight
	{
		unsigned char type;
		unsigned char unused[3];
		float color[3];
		float dir[3];
		float up[3];
		float origin[3];
		float radius;
		float cosHalfFovOuter;
		float cosHalfFovInner;
		int exponent;
	};

	struct SModelAabbNode
	{
		Bounds bounds;
		unsigned short firstChild;
		unsigned short childCount;
	};

	enum DynEntityType
	{
		DYNENT_TYPE_INVALID = 0x0,
		DYNENT_TYPE_CLUTTER = 0x1,
		DYNENT_TYPE_DESTRUCT = 0x2,
		DYNENT_TYPE_HINGE = 0x3,
		DYNENT_TYPE_COUNT = 0x4
	};

	struct GfxPlacement
	{
		float quat[4];
		float origin[3];
	};

	struct DynEntityHingeDef
	{
		float axisOrigin[3];
		float axisDir[3];
		bool isLimited;
		float angleMin;
		float angleMax;
		float momentOfInertia;
		float friction;
	};

	struct DynEntityDef
	{
		DynEntityType type;
		GfxPlacement pose;
		x86::pointer<XModel> xModel;
		unsigned short brushModel;
		unsigned short physicsBrushModel;
		x86::pointer<FxEffectDef> destroyFx;
		x86::pointer<PhysPreset> physPreset;
		int health;
		x86::pointer<DynEntityHingeDef> hinge;
		PhysMass mass;
		int contents;
	};

	struct DynEntityPose
	{
		GfxPlacement pose;
		float radius;
	};

	struct Hinge
	{
		float angle;
		float quat[4];
		float angularVel;
		float torqueAccum;
		bool active;
		float autoDisableTimeLeft;
		x86::pointer<DynEntityHingeDef> def;
		x86::pointer<PhysPreset> physPreset;
		float centerOfMassRelToAxisOriginAtAngleZero[3];
	};

	struct DynEntityClient
	{
		int physObjId;
		unsigned short flags;
		unsigned short lightingHandle;
		int health;
		x86::pointer<Hinge> hinge;
	};

	struct DynEntityColl
	{
		unsigned short sector;
		unsigned short nextEntInSector;
		float linkMins[2];
		float linkMaxs[2];
	};

	enum CSurfaceFlags : std::uint32_t
	{
		SURF_FLAG_DEFAULT = 0x00000000,
		SURF_FLAG_BARK = 0x00100000,
		SURF_FLAG_BRICK = 0x00200000,
		SURF_FLAG_CARPET = 0x00300000,
		SURF_FLAG_CLOTH = 0x00400000,
		SURF_FLAG_CONCRETE = 0x00500000,
		SURF_FLAG_DIRT = 0x00600000,
		SURF_FLAG_FLESH = 0x00700000,
		SURF_FLAG_FOLIAGE = 0x00800000,
		SURF_FLAG_GLASS = 0x00900000,
		SURF_FLAG_GRASS = 0x00A00000,
		SURF_FLAG_GRAVEL = 0x00B00000,
		SURF_FLAG_ICE = 0x00C00000,
		SURF_FLAG_METAL = 0x00D00000,
		SURF_FLAG_MUD = 0x00E00000,
		SURF_FLAG_PAPER = 0x00F00000,
		SURF_FLAG_PLASTER = 0x01000000,
		SURF_FLAG_ROCK = 0x01100000,
		SURF_FLAG_SAND = 0x01200000,
		SURF_FLAG_SNOW = 0x01300000,
		SURF_FLAG_WATER = 0x01400000,
		SURF_FLAG_WOOD = 0x01500000,
		SURF_FLAG_ASPHALT = 0x01600000,
		SURF_FLAG_CERAMIC = 0x01700000,
		SURF_FLAG_PLASTIC = 0x01800000,
		SURF_FLAG_RUBBER = 0x01900000,
		SURF_FLAG_CUSHION = 0x01A00000,
		SURF_FLAG_FRUIT = 0x01B00000,
		SURF_FLAG_PAINTEDMETAL = 0x01C00000,
		SURF_FLAG_RIOTSHIELD = 0x01D00000,
		SURF_FLAG_SLUSH = 0x01E00000,
		SURF_FLAG_OPAQUEGLASS = 0x00900000,
		SURF_FLAG_CLIPMISSILE = 0x00000000,
		SURF_FLAG_AI_NOSIGHT = 0x00000000,
		SURF_FLAG_CLIPSHOT = 0x00000000,
		SURF_FLAG_PLAYERCLIP = 0x00000000,
		SURF_FLAG_MONSTERCLIP = 0x00000000,
		SURF_FLAG_AICLIPALLOWDEATH = 0x00000000,
		SURF_FLAG_VEHICLECLIP = 0x00000000,
		SURF_FLAG_ITEMCLIP = 0x00000000,
		SURF_FLAG_NODROP = 0x00000000,
		SURF_FLAG_NONSOLID = 0x00004000,
		SURF_FLAG_DETAIL = 0x00000000,
		SURF_FLAG_STRUCTURAL = 0x00000000,
		SURF_FLAG_PORTAL = 0x80000000,
		SURF_FLAG_CANSHOOTCLIP = 0x00000000,
		SURF_FLAG_ORIGIN = 0x00000000,
		SURF_FLAG_SKY = 0x00000004,
		SURF_FLAG_NOCASTSHADOW = 0x00040000,
		SURF_FLAG_PHYSICSGEOM = 0x00000000,
		SURF_FLAG_LIGHTPORTAL = 0x00000000,
		SURF_FLAG_OUTDOORBOUNDS = 0x00000000,
		SURF_FLAG_SLICK = 0x00000002,
		SURF_FLAG_NOIMPACT = 0x00000010,
		SURF_FLAG_NOMARKS = 0x00000020,
		SURF_FLAG_NOPENETRATE = 0x00000100,
		SURF_FLAG_LADDER = 0x00000008,
		SURF_FLAG_NODAMAGE = 0x00000001,
		SURF_FLAG_MANTLEON = 0x02000000,
		SURF_FLAG_MANTLEOVER = 0x04000000,
		SURF_FLAG_STAIRS = 0x00000200,
		SURF_FLAG_SOFT = 0x00001000,
		SURF_FLAG_NOSTEPS = 0x00002000,
		SURF_FLAG_NODRAW = 0x00000080,
		SURF_FLAG_NOLIGHTMAP = 0x00000400,
		SURF_FLAG_NODLIGHT = 0x00020000,
	};

	struct ClipMaterial
	{
		x86::pointer<const char> name;
		int surfaceFlags;
		int contents;
	};

	struct cLeafBrushNodeLeaf_t
	{
		x86::pointer<unsigned short> brushes;
	};

	struct cLeafBrushNodeChildren_t
	{
		float dist;
		float range;
		unsigned short childOffset[2];
	};

	union cLeafBrushNodeData_t
	{
		cLeafBrushNodeLeaf_t leaf;
		cLeafBrushNodeChildren_t children;
	};

	struct cLeafBrushNode_s
	{
		unsigned char axis;
		short leafBrushCount;
		int contents;
		cLeafBrushNodeData_t data;
	};

	struct cbrush_t
	{
		unsigned short numsides;
		unsigned short glassPieceIndex;
		x86::pointer<cbrushside_t> sides;
		x86::pointer<unsigned char> baseAdjacentSide;
		unsigned short axialMaterialNum[2][3];
		unsigned char firstAdjacentSideOffsets[2][3];
		unsigned char edgeCount[2][3];
	};

	struct ClipInfo
	{
		int planeCount;
		x86::pointer<cplane_s> planes;
		unsigned int numMaterials;
		x86::pointer<ClipMaterial> materials;
		unsigned int numBrushSides;
		x86::pointer<cbrushside_t> brushsides;
		unsigned int numBrushEdges;
		x86::pointer<unsigned char> brushEdges;
		unsigned int leafbrushNodesCount;
		x86::pointer<cLeafBrushNode_s> leafbrushNodes;
		unsigned int numLeafBrushes;
		x86::pointer<unsigned short> leafbrushes;
		unsigned short numBrushes;
		x86::pointer<cbrush_t> brushes;
		x86::pointer<Bounds> brushBounds;
		x86::pointer<int> brushContents;
	};

	struct cStaticModel_s
	{
		x86::pointer<XModel> xmodel;
		float origin[3];
		float invScaledAxis[3][3];
		Bounds absBounds;
	};

	struct cNode_t
	{
		x86::pointer<cplane_s> plane;
		short children[2];
	};

	struct cLeaf_t
	{
		unsigned short firstCollAabbIndex;
		unsigned short collAabbCount;
		int brushContents;
		int terrainContents;
		Bounds bounds;
		int leafBrushNode;
	};

	struct CollisionBorder
	{
		float distEq[3];
		float zBase;
		float zSlope;
		float start;
		float length;
	};

	struct CollisionPartition
	{
		unsigned char triCount;
		unsigned char borderCount;
		unsigned char firstVertSegment;
		int firstTri;
		x86::pointer<CollisionBorder> borders;
	};

	union CollisionAabbTreeIndex
	{
		int firstChildIndex;
		int partitionIndex;
	};

	struct CollisionAabbTree
	{
		float midPoint[3];
		unsigned short materialIndex;
		unsigned short childCount;
		float halfSize[3];
		CollisionAabbTreeIndex u;
	};

	struct cmodel_t
	{
		Bounds bounds;
		float radius;
		x86::pointer<ClipInfo> info;
		cLeaf_t leaf;
	};

	struct clipMap_t
	{
		x86::pointer<const char> name;
		int isInUse;
		ClipInfo info;
		x86::pointer<ClipInfo> pInfo;
		unsigned int numStaticModels;
		x86::pointer<cStaticModel_s> staticModelList;
		unsigned int numNodes;
		x86::pointer<cNode_t> nodes;
		unsigned int numLeafs;
		x86::pointer<cLeaf_t> leafs;
		unsigned int vertCount;
		x86::pointer<vec3_t> verts;
		int triCount;
		x86::pointer<unsigned short> triIndices;
		x86::pointer<unsigned char> triEdgeIsWalkable;
		int borderCount;
		x86::pointer<CollisionBorder> borders;
		int partitionCount;
		x86::pointer<CollisionPartition> partitions;
		int aabbTreeCount;
		x86::pointer<CollisionAabbTree> aabbTrees;
		unsigned int numSubModels;
		x86::pointer<cmodel_t> cmodels;
		x86::pointer<MapEnts> mapEnts;
		x86::pointer<Stage> stages;
		unsigned char stageCount;
		MapTriggers stageTrigger;
		unsigned short smodelNodeCount;
		x86::pointer<SModelAabbNode> smodelNodes;
		unsigned short dynEntCount[2];
		x86::pointer<DynEntityDef> dynEntDefList[2];
		x86::pointer<DynEntityPose> dynEntPoseList[2];
		x86::pointer<DynEntityClient> dynEntClientList[2];
		x86::pointer<DynEntityColl> dynEntCollList[2];
		unsigned int checksum;
		unsigned char padding[20];
	};

	struct FxGlassDef
	{
		float halfThickness;
		float texVecs[2][2];
		GfxColor color;
		x86::pointer<Material> material;
		x86::pointer<Material> materialShattered;
		x86::pointer<PhysPreset> physPreset;
	};

	struct FxSpatialFrame
	{
		float quat[4];
		float origin[3];
	};

	union FxGlassPiecePlace
	{
#pragma warning(push)
#pragma warning(disable: 4201)
		struct
		{
			FxSpatialFrame frame;
			float radius;
		};
#pragma warning(pop)
		unsigned int nextFree;
	};

	struct FxGlassPieceState
	{
		float texCoordOrigin[2];
		unsigned int supportMask;
		unsigned short initIndex;
		unsigned short geoDataStart;
		unsigned char defIndex;
		unsigned char pad[5];
		unsigned char vertCount;
		unsigned char holeDataCount;
		unsigned char crackDataCount;
		unsigned char fanDataCount;
		unsigned short flags;
		float areaX2;
	};

	struct FxGlassPieceDynamics
	{
		int fallTime;
		int physObjId;
		int physJointId;
		float vel[3];
		float avel[3];
	};

	struct FxGlassVertex
	{
		short x;
		short y;
	};

	struct FxGlassHoleHeader
	{
		unsigned short uniqueVertCount;
		unsigned char touchVert;
		unsigned char pad[1];
	};

	struct FxGlassCrackHeader
	{
		unsigned short uniqueVertCount;
		unsigned char beginVertIndex;
		unsigned char endVertIndex;
	};

	union FxGlassGeometryData
	{
		FxGlassVertex vert;
		FxGlassHoleHeader hole;
		FxGlassCrackHeader crack;
		unsigned char asBytes[4];
		short anonymous[2];
	};

	struct FxGlassInitPieceState
	{
		FxSpatialFrame frame;
		float radius;
		float texCoordOrigin[2];
		unsigned int supportMask;
		float areaX2;
		unsigned char defIndex;
		unsigned char vertCount;
		unsigned char fanDataCount;
		unsigned char pad[1];
	};

	struct FxGlassSystem
	{
		int time;
		int prevTime;
		unsigned int defCount;
		unsigned int pieceLimit;
		unsigned int pieceWordCount;
		unsigned int initPieceCount;
		unsigned int cellCount;
		unsigned int activePieceCount;
		unsigned int firstFreePiece;
		unsigned int geoDataLimit;
		unsigned int geoDataCount;
		unsigned int initGeoDataCount;
		x86::pointer<FxGlassDef> defs;
		x86::pointer<FxGlassPiecePlace> piecePlaces;
		x86::pointer<FxGlassPieceState> pieceStates;
		x86::pointer<FxGlassPieceDynamics> pieceDynamics;
		x86::pointer<FxGlassGeometryData> geoData;
		x86::pointer<unsigned int> isInUse;
		x86::pointer<unsigned int> cellBits;
		x86::pointer<unsigned char> visData;
		x86::pointer<float[3]> linkOrg;
		x86::pointer<float> halfThickness;
		x86::pointer<unsigned short> lightingHandles;
		x86::pointer<FxGlassInitPieceState> initPieceStates;
		x86::pointer<FxGlassGeometryData> initGeoData;
		bool needToCompactData;
		unsigned char initCount;
		float effectChanceAccum;
		int lastPieceDeletionTime;
	};

	struct FxWorld
	{
		x86::pointer<const char> name;
		FxGlassSystem glassSys;
	};

	struct FxIntRange
	{
		int base;
		int amplitude;
	};

	struct FxSpawnDefLooping
	{
		int intervalMsec;
		int count;
	};

	struct FxSpawnDefOneShot
	{
		FxIntRange count;
	};

	union FxSpawnDef
	{
		FxSpawnDefLooping looping;
		FxSpawnDefOneShot oneShot;
	};

	struct FxFloatRange
	{
		float base;
		float amplitude;
	};

	enum FxElemAtlasBehavior : std::uint8_t
	{
		FX_ATLAS_START_FIXED = 0x0,
		FX_ATLAS_START_RANDOM = 0x1,
		FX_ATLAS_START_INDEXED = 0x2,
		FX_ATLAS_START_MASK = 0x3,
		FX_ATLAS_PLAY_OVER_LIFE = 0x4,
		FX_ATLAS_LOOP_ONLY_N_TIMES = 0x8,
	};

	struct FxElemAtlas
	{
		unsigned char behavior;
		unsigned char index;
		unsigned char fps;
		unsigned char loopCount;
		unsigned char colIndexBits;
		unsigned char rowIndexBits;
		short entryCount;
	};

	struct FxElemVec3Range
	{
		float base[3];
		float amplitude[3];
	};

	struct FxElemVelStateInFrame
	{
		FxElemVec3Range velocity;
		FxElemVec3Range totalDelta;
	};

	struct FxElemVelStateSample
	{
		FxElemVelStateInFrame local;
		FxElemVelStateInFrame world;
	};

	struct FxElemVisualState
	{
		unsigned char color[4];
		float rotationDelta;
		float rotationTotal;
		float size[2];
		float scale;
	};

	struct FxElemVisStateSample
	{
		FxElemVisualState base;
		FxElemVisualState amplitude;
	};

	struct FxElemMarkVisuals
	{
		x86::pointer<Material> materials[2];
	};

	union FxEffectDefRef
	{
		x86::pointer<FxEffectDef> handle;
		x86::pointer<const char> name;
	};

	union FxElemVisuals
	{
		x86::pointer<const void> anonymous;
		x86::pointer<Material> material;
		x86::pointer<XModel> model;
		FxEffectDefRef effectDef;
		x86::pointer<const char> soundName;
		x86::pointer<GfxLightDef> lightDef;
	};

	union FxElemDefVisuals
	{
		x86::pointer<FxElemMarkVisuals> markArray;
		x86::pointer<FxElemVisuals> array;
		FxElemVisuals instance;
	};

	struct FxTrailVertex
	{
		float pos[2];
		float normal[2];
		float texCoord;
	};

	struct FxTrailDef
	{
		int scrollTimeMsec;
		int repeatDist;
		float invSplitDist;
		float invSplitArcDist;
		float invSplitTime;
		int vertCount;
		x86::pointer<FxTrailVertex> verts;
		int indCount;
		x86::pointer<unsigned short> inds;
	};

	struct FxSparkFountainDef
	{
		float gravity;
		float bounceFrac;
		float bounceRand;
		float sparkSpacing;
		float sparkLength;
		int sparkCount;
		float loopTime;
		float velMin;
		float velMax;
		float velConeFrac;
		float restSpeed;
		float boostTime;
		float boostFactor;
	};

	struct FxSpotLightDef
	{
		float fovInnerFraction;
		float startRadius;
		float endRadius;
		float brightness;
		float maxLength;
		int exponent;
	};

	union FxElemExtendedDefPtr
	{
		x86::pointer<FxTrailDef> trailDef;
		x86::pointer<FxSparkFountainDef> sparkFountainDef;
		x86::pointer<FxSpotLightDef> spotLightDef;
		x86::pointer<char> unknownDef;
	};

	enum FxElemType : char
	{
		FX_ELEM_TYPE_SPRITE_BILLBOARD = 0x0,
		FX_ELEM_TYPE_SPRITE_ORIENTED = 0x1,
		FX_ELEM_TYPE_TAIL = 0x2,
		FX_ELEM_TYPE_TRAIL = 0x3,
		FX_ELEM_TYPE_CLOUD = 0x4,
		FX_ELEM_TYPE_SPARKCLOUD = 0x5,
		FX_ELEM_TYPE_SPARKFOUNTAIN = 0x6,
		FX_ELEM_TYPE_MODEL = 0x7,
		FX_ELEM_TYPE_OMNI_LIGHT = 0x8,
		FX_ELEM_TYPE_SPOT_LIGHT = 0x9,
		FX_ELEM_TYPE_SOUND = 0xA,
		FX_ELEM_TYPE_DECAL = 0xB,
		FX_ELEM_TYPE_RUNNER = 0xC,
		FX_ELEM_TYPE_COUNT = 0xD,
		FX_ELEM_TYPE_LAST_SPRITE = 0x3,
		FX_ELEM_TYPE_LAST_DRAWN = 0x9,
	};

	enum FxElemDefFlags : std::uint32_t
	{
		FX_ELEM_SPAWN_RELATIVE_TO_EFFECT = 0x2,
		FX_ELEM_SPAWN_FRUSTUM_CULL = 0x4,
		FX_ELEM_RUNNER_USES_RAND_ROT = 0x8,
		FX_ELEM_SPAWN_OFFSET_NONE = 0x0,
		FX_ELEM_SPAWN_OFFSET_SPHERE = 0x10,
		FX_ELEM_SPAWN_OFFSET_CYLINDER = 0x20,
		FX_ELEM_SPAWN_OFFSET_MASK = 0x30,
		FX_ELEM_RUN_RELATIVE_TO_WORLD = 0x0,
		FX_ELEM_RUN_RELATIVE_TO_SPAWN = 0x40,
		FX_ELEM_RUN_RELATIVE_TO_EFFECT = 0x80,
		FX_ELEM_RUN_RELATIVE_TO_OFFSET = 0xC0,
		FX_ELEM_RUN_MASK = 0xC0,
		FX_ELEM_USE_COLLISION = 0x100,
		FX_ELEM_DIE_ON_TOUCH = 0x200,
		FX_ELEM_DRAW_PAST_FOG = 0x400,
		FX_ELEM_DRAW_WITH_VIEWMODEL = 0x800,
		FX_ELEM_BLOCK_SIGHT = 0x1000,
		FX_ELEM_DRAW_IN_THERMAL_VIEW_ONLY = 0x2000,
		FX_ELEM_TRAIL_ORIENT_BY_VELOCITY = 0x4000,
		FX_ELEM_EMIT_ORIENT_BY_ELEM = 0x8000,
		FX_ELEM_USE_OCCLUSION_QUERY = 0x10000,
		FX_ELEM_HAS_VELOCITY_GRAPH_LOCAL = 0x1000000,
		FX_ELEM_HAS_VELOCITY_GRAPH_WORLD = 0x2000000,
		FX_ELEM_HAS_GRAVITY = 0x4000000,
		FX_ELEM_USE_MODEL_PHYSICS = 0x8000000,
		FX_ELEM_NONUNIFORM_SCALE = 0x10000000,
		FX_ELEM_CLOUD_SHAPE_CUBE = 0x0,
		FX_ELEM_CLOUD_SHAPE_SPHERE_LARGE = 0x20000000,
		FX_ELEM_CLOUD_SHAPE_SPHERE_MEDIUM = 0x40000000,
		FX_ELEM_CLOUD_SHAPE_SPHERE_SMALL = 0x60000000,
		FX_ELEM_CLOUD_SHAPE_MASK = 0x60000000,
		FX_ELEM_FOUNTAIN_DISABLE_COLLISION = 0x80000000,
	};

	struct FxElemDef
	{
		FxElemDefFlags flags;
		FxSpawnDef spawn;
		FxFloatRange spawnRange;
		FxFloatRange fadeInRange;
		FxFloatRange fadeOutRange;
		float spawnFrustumCullRadius;
		FxIntRange spawnDelayMsec;
		FxIntRange lifeSpanMsec;
		FxFloatRange spawnOrigin[3];
		FxFloatRange spawnOffsetRadius;
		FxFloatRange spawnOffsetHeight;
		FxFloatRange spawnAngles[3];
		FxFloatRange angularVelocity[3];
		FxFloatRange initialRotation;
		FxFloatRange gravity;
		FxFloatRange reflectionFactor;
		FxElemAtlas atlas;
		FxElemType elemType;
		unsigned char visualCount;
		unsigned char velIntervalCount;
		unsigned char visStateIntervalCount;
		x86::pointer<FxElemVelStateSample> velSamples;
		x86::pointer<FxElemVisStateSample> visSamples;
		FxElemDefVisuals visuals;
		Bounds collBounds;
		FxEffectDefRef effectOnImpact;
		FxEffectDefRef effectOnDeath;
		FxEffectDefRef effectEmitted;
		FxFloatRange emitDist;
		FxFloatRange emitDistVariance;
		FxElemExtendedDefPtr extended;
		unsigned char sortOrder;
		unsigned char lightingFrac;
		unsigned char useItemClip;
		unsigned char fadeInfo;
		int randomSeed;
	};

	struct FxEffectDef
	{
		x86::pointer<const char> name;
		int flags;
		int totalSize;
		int msecLoopingLife;
		int elemDefCountLooping;
		int elemDefCountOneShot;
		int elemDefCountEmission;
		float occlusionQueryDepthBias;
		int occlusionQueryFadeIn;
		int occlusionQueryFadeOut;
		FxFloatRange occlusionQueryScaleRange;
		x86::pointer<FxElemDef> elemDefs;
	};

	enum XAnimPartType
	{
		PART_TYPE_NO_QUAT = 0x0,
		PART_TYPE_HALF_QUAT = 0x1,
		PART_TYPE_FULL_QUAT = 0x2,
		PART_TYPE_HALF_QUAT_NO_SIZE = 0x3,
		PART_TYPE_FULL_QUAT_NO_SIZE = 0x4,
		PART_TYPE_SMALL_TRANS = 0x5,
		PART_TYPE_TRANS = 0x6,
		PART_TYPE_TRANS_NO_SIZE = 0x7,
		PART_TYPE_NO_TRANS = 0x8,
		PART_TYPE_ALL = 0x9,
		PART_TYPE_COUNT = 0xA,
	};

	union XAnimIndices
	{
		x86::pointer<char> _1;
		x86::pointer<unsigned short> _2;
		x86::pointer<void> data;
	};

	struct XAnimNotifyInfo
	{
		unsigned short name;
		float time;
	};

	union XAnimDynamicFrames
	{
		x86::pointer<char[3]> _1;
		x86::pointer<unsigned __int16[3]> _2;
	};

	union XAnimDynamicIndicesTrans
	{
		char _1[1];
		unsigned short _2[1];
	};

	struct XAnimPartTransFrames
	{
		float mins[3];
		float size[3];
		XAnimDynamicFrames frames;
		XAnimDynamicIndicesTrans indices;
	};

	union XAnimPartTransData
	{
		XAnimPartTransFrames frames;
		vec3_t frame0;
	};

	struct XAnimPartTrans
	{
		unsigned short size;
		char smallTrans;
		XAnimPartTransData u;
	};

	union XAnimDynamicIndicesQuat2
	{
		char _1[1];
		unsigned short _2[1];
	};

	struct XAnimDeltaPartQuatDataFrames2
	{
		x86::pointer<__int16[2]> frames;
		XAnimDynamicIndicesQuat2 indices;
	};

	union XAnimDeltaPartQuatData2
	{
		XAnimDeltaPartQuatDataFrames2 frames;
		int16_t frame0[2];
	};

	struct XAnimDeltaPartQuat2
	{
		unsigned short size;
		XAnimDeltaPartQuatData2 u;
	};

	union XAnimDynamicIndicesQuat
	{
		char _1[1];
		unsigned short _2[1];
	};

	struct XAnimDeltaPartQuatDataFrames
	{
		x86::pointer<__int16[4]> frames;
		XAnimDynamicIndicesQuat indices;
	};

	union XAnimDeltaPartQuatData
	{
		XAnimDeltaPartQuatDataFrames frames;
		short frame0[4];
	};

	struct XAnimDeltaPartQuat
	{
		unsigned short size;
		XAnimDeltaPartQuatData u;
	};

	struct XAnimDeltaPart
	{
		x86::pointer<XAnimPartTrans> trans;
		x86::pointer<XAnimDeltaPartQuat2> quat2;
		x86::pointer<XAnimDeltaPartQuat> quat;
	};

	struct XAnimParts
	{
		x86::pointer<const char> name;
		unsigned short dataByteCount;
		unsigned short dataShortCount;
		unsigned short dataIntCount;
		unsigned short randomDataByteCount;
		unsigned short randomDataIntCount;
		unsigned short numframes;
		char flags;
		unsigned char boneCount[10];
		char notifyCount;
		char assetType;
		bool isDefault;
		unsigned int randomDataShortCount;
		unsigned int indexCount;
		float framerate;
		float frequency;
		x86::pointer<unsigned short> names;
		x86::pointer<char> dataByte;
		x86::pointer<unsigned short> dataShort;
		x86::pointer<int> dataInt;
		x86::pointer<unsigned short> randomDataShort;
		x86::pointer<char> randomDataByte;
		x86::pointer<int> randomDataInt;
		XAnimIndices indices;
		x86::pointer<XAnimNotifyInfo> notify;
		x86::pointer<XAnimDeltaPart> deltaPart;
	};
}

#pragma warning(pop)
