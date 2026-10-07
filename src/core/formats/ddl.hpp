#pragma once

namespace zonetool
{
	class zone_memory;

	namespace ddl_structs
	{
		enum DDLType : std::int32_t
		{
			DDL_BYTE_TYPE = 0x0,
			DDL_SHORT_TYPE = 0x1,
			DDL_UINT_TYPE = 0x2,
			DDL_INT_TYPE = 0x3,
			DDL_UINT64_TYPE = 0x4,
			DDL_FLOAT_TYPE = 0x5,
			DDL_FIXEDPOINT_TYPE = 0x6,
			DDL_STRING_TYPE = 0x7,
			DDL_STRUCT_TYPE = 0x8,
			DDL_ENUM_TYPE = 0x9,
			DDL_PAD_TYPE = 0xA,
		};

		enum DDLFlags : std::uint8_t
		{
			DDL_FLAG_DIRTY = 0x0,
			DDL_FLAG_CHECKSUM = 0x1,
			DDL_FLAG_CODE_VERSION = 0x2,
			DDL_FLAG_USER_FLAGS = 0x4,
			DDL_FLAG_NO_PADDING = 0x8,
			DDL_FLAG_RESERVE = 0x10,
			DDL_FLAG_DDL_CHECKSUM = 0x20,
		};

		struct DDLMember
		{
			const char* name;
			int index;
			void* parent;
			int bitSize;
			int limitSize;
			int offset;
			int type;
			int externalIndex;
			unsigned int rangeLimit;
			unsigned int serverDelta;
			unsigned int clientDelta;
			int arraySize;
			int enumIndex;
			int permission;
		};

		struct DDLHash
		{
			unsigned int hash;
			int index;
		};

		struct DDLHashTable
		{
			DDLHash* list;
			int count;
			int max;
		};

		struct DDLStruct
		{
			const char* name;
			int bitSize;
			int memberCount;
			DDLMember* members;
			DDLHashTable hashTableUpper;
			DDLHashTable hashTableLower;
		};

		struct DDLEnum
		{
			const char* name;
			int memberCount;
			const char** members;
			DDLHashTable hashTable;
		};

		struct DDLDef
		{
			char* name;
			unsigned short version;
			unsigned int checksum;
			unsigned char flags;
			int bitSize;
			int byteSize;
			DDLStruct* structList;
			int structCount;
			DDLEnum* enumList;
			int enumCount;
			DDLDef* next;
			int headerBitSize;
			int headerByteSize;
			int reserveSize;
			int userFlagsSize;
			bool paddingUsed;
		};

		struct DDLFile
		{
			char* name;
			DDLDef* ddlDef;
		};
	}

	namespace ddl
	{
		ddl_structs::DDLFile* parse(const std::string& name, zone_memory* mem);
		void dump(ddl_structs::DDLFile* asset);
		void generateHashTables(ddl_structs::DDLDef* def, zone_memory* mem);
	}
}
