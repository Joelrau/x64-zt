#include <std_include.hpp>

#include "havok_schema.hpp"

namespace zonetool::iw7::converter::iw8::havok
{
	namespace
	{
		std::vector<::havok::tagfile::type_info> make_schema()
		{
			std::vector<::havok::tagfile::type_info> types;
			types.reserve(188);
			types.emplace_back();
			types.push_back({"HavokPhysicsShapeList", {}, 0u, 0x7u, 0u, 1u, 152u, 8u, 0x2du, 0u, 10u, 0x0a0590d1u, true, {{"shapes", 32u, 0u, 2u}, {"shapeIndices", 32u, 16u, 3u}, {"shapeNames", 32u, 32u, 4u}, {"vertCounts", 32u, 48u, 3u}, {"triCounts", 32u, 64u, 3u}, {"minMaxes", 32u, 80u, 5u}, {"reserved", 32u, 96u, 6u}, {"shapeTagData", 32u, 104u, 7u}, {"shapeContents", 32u, 120u, 3u}, {"convexCounts", 32u, 136u, 3u}}});
			types.push_back({"hkArray", {{"tT", 8u}, {"tAllocator", 9u}}, 0u, 0x8u, 8u, 0u, 16u, 8u, 0x2bu, 0u, 3u, 0xc0f2ac9fu, true, {{"m_data", 34u, 0u, 10u}, {"m_size", 34u, 8u, 6u}, {"m_capacityAndFlags", 34u, 12u, 6u}}});
			types.push_back({"hkArray", {{"tT", 6u}, {"tAllocator", 9u}}, 0u, 0x8u, 6u, 0u, 16u, 8u, 0x2bu, 0u, 3u, 0x36440be9u, true, {{"m_data", 34u, 0u, 11u}, {"m_size", 34u, 8u, 6u}, {"m_capacityAndFlags", 34u, 12u, 6u}}});
			types.push_back({"hkArray", {{"tT", 12u}, {"tAllocator", 9u}}, 0u, 0x8u, 12u, 0u, 16u, 8u, 0x2bu, 0u, 3u, 0x102a4d0du, true, {{"m_data", 34u, 0u, 13u}, {"m_size", 34u, 8u, 6u}, {"m_capacityAndFlags", 34u, 12u, 6u}}});
			types.push_back({"hkArray", {{"tT", 14u}, {"tAllocator", 9u}}, 0u, 0x8u, 14u, 0u, 16u, 8u, 0x2bu, 0u, 3u, 0x535604cfu, true, {{"m_data", 34u, 0u, 15u}, {"m_size", 34u, 8u, 6u}, {"m_capacityAndFlags", 34u, 12u, 6u}}});
			types.push_back({"int", {}, 0u, 0x8204u, 0u, 0u, 4u, 4u, 0x9u, 0u, 0u, 0xf2226812u, true, {}});
			types.push_back({"hkArray", {{"tT", 16u}, {"tAllocator", 9u}}, 0u, 0x8u, 16u, 0u, 16u, 8u, 0x2bu, 0u, 3u, 0x12205293u, true, {{"m_data", 34u, 0u, 17u}, {"m_size", 34u, 8u, 6u}, {"m_capacityAndFlags", 34u, 12u, 6u}}});
			types.push_back({"T*", {{"tT", 18u}}, 0u, 0x6u, 18u, 0u, 8u, 8u, 0x1bu, 0u, 0u, 0x00897122u, true, {}});
			types.push_back({"hkContainerHeapAllocator", {}, 0u, 0x7u, 0u, 0u, 1u, 1u, 0x39u, 16u, 0u, 0u, false, {}});
			types.push_back({"T*", {{"tT", 8u}}, 0u, 0x6u, 8u, 0u, 8u, 8u, 0x1bu, 64u, 0u, 0u, false, {}});
			types.push_back({"T*", {{"tT", 6u}}, 0u, 0x6u, 6u, 0u, 8u, 8u, 0x1bu, 64u, 0u, 0u, false, {}});
			types.push_back({"hkStringPtr", {}, 0u, 0x83u, 0u, 0u, 8u, 8u, 0x29u, 0u, 1u, 0xa1909ef9u, true, {{"stringAndFlag", 36u, 0u, 19u}}});
			types.push_back({"T*", {{"tT", 12u}}, 0u, 0x6u, 12u, 0u, 8u, 8u, 0x1bu, 64u, 0u, 0u, false, {}});
			types.push_back({"hkVector4", {}, 20u, 0x0u, 0u, 0u, 0u, 0u, 0x0u, 0u, 0u, 0xc2b7e409u, true, {}});
			types.push_back({"T*", {{"tT", 14u}}, 0u, 0x6u, 14u, 0u, 8u, 8u, 0x1bu, 64u, 0u, 0u, false, {}});
			types.push_back({"HavokPhysicsShapeList::ShapeTagData", {}, 0u, 0x7u, 0u, 1u, 24u, 8u, 0x2du, 0u, 4u, 0xd6f2ebcfu, true, {{"collisionFilterInfo", 32u, 0u, 21u}, {"materialCRC", 32u, 4u, 6u}, {"materialId", 32u, 8u, 22u}, {"userData", 32u, 16u, 23u}}});
			types.push_back({"T*", {{"tT", 16u}}, 0u, 0x6u, 16u, 0u, 8u, 8u, 0x1bu, 64u, 0u, 0u, false, {}});
			types.push_back({"hknpShape", {}, 24u, 0x7u, 0u, 4u, 56u, 8u, 0x2du, 0u, 7u, 0u, false, {{"flags", 34u, 24u, 25u}, {"type", 34u, 26u, 26u}, {"numShapeKeyBits", 34u, 27u, 27u}, {"dispatchType", 32u, 28u, 28u}, {"convexRadius", 32u, 32u, 29u}, {"userData", 32u, 40u, 23u}, {"properties", 34u, 48u, 30u}}});
			types.push_back({"char*", {}, 0u, 0x3u, 0u, 0u, 8u, 8u, 0x9u, 0u, 0u, 0u, false, {}});
			types.push_back({"hkVector4f", {}, 0u, 0x428u, 31u, 0u, 16u, 16u, 0xbu, 0u, 0u, 0xb54aa116u, true, {}});
			types.push_back({"hkUint32", {}, 32u, 0x0u, 0u, 0u, 0u, 0u, 0x0u, 0u, 0u, 0x664be534u, true, {}});
			types.push_back({"unsigned short", {}, 0u, 0x4004u, 0u, 0u, 2u, 2u, 0x9u, 0u, 0u, 0u, false, {}});
			types.push_back({"hkUint64", {}, 33u, 0x0u, 0u, 0u, 0u, 0u, 0x0u, 0u, 0u, 0u, false, {}});
			types.push_back({"hkReferencedObject", {}, 34u, 0x7u, 0u, 3u, 24u, 8u, 0x2du, 0u, 3u, 0u, false, {{"propertyBag", 36u, 8u, 35u}, {"memSizeAndFlags", 37u, 16u, 36u}, {"refCount", 37u, 18u, 36u}}});
			types.push_back({"hkFlags", {{"tBITS", 37u}, {"tSTORAGE", 36u}}, 0u, 0x4004u, 0u, 0u, 2u, 2u, 0x29u, 0u, 1u, 0u, false, {{"storage", 34u, 0u, 36u}}});
			types.push_back({"hkEnum", {{"tENUM", 38u}, {"tSTORAGE", 27u}}, 0u, 0x2004u, 0u, 0u, 1u, 1u, 0x29u, 0u, 1u, 0u, false, {{"storage", 36u, 0u, 27u}}});
			types.push_back({"hkUint8", {}, 39u, 0x0u, 0u, 0u, 0u, 0u, 0x0u, 0u, 0u, 0xddd4337bu, true, {}});
			types.push_back({"hkEnum", {{"tENUM", 40u}, {"tSTORAGE", 27u}}, 0u, 0x2004u, 0u, 0u, 1u, 1u, 0x29u, 0u, 1u, 0u, false, {{"storage", 36u, 0u, 27u}}});
			types.push_back({"hkReal", {}, 31u, 0x0u, 0u, 0u, 0u, 0u, 0x0u, 0u, 0u, 0u, false, {}});
			types.push_back({"hkRefPtr", {{"tTYPE", 41u}}, 0u, 0x6u, 41u, 0u, 8u, 8u, 0x2bu, 0u, 1u, 0u, false, {{"ptr", 36u, 0u, 42u}}});
			types.push_back({"float", {}, 0u, 0x174605u, 0u, 0u, 4u, 4u, 0x9u, 0u, 0u, 0u, false, {}});
			types.push_back({"unsigned int", {}, 0u, 0x8004u, 0u, 0u, 4u, 4u, 0x9u, 0u, 0u, 0x94e0eb06u, true, {}});
			types.push_back({"unsigned long long", {}, 0u, 0x10004u, 0u, 0u, 8u, 8u, 0x9u, 0u, 0u, 0x20760f81u, true, {}});
			types.push_back({"hkBaseObject", {}, 0u, 0x7u, 0u, 0u, 8u, 8u, 0x39u, 144u, 0u, 0u, false, {}});
			types.push_back({"hkPropertyBag", {}, 0u, 0x8u, 43u, 0u, 8u, 8u, 0x2bu, 0u, 1u, 0u, false, {{"bag", 35u, 0u, 44u}}});
			types.push_back({"hkUint16", {}, 22u, 0x0u, 0u, 0u, 0u, 0u, 0x0u, 0u, 0u, 0xf9194383u, true, {}});
			types.push_back({"hknpShape::FlagsEnum", {}, 0u, 0x8204u, 0u, 0u, 4u, 4u, 0x9u, 0u, 0u, 0u, false, {}});
			types.push_back({"hknpShapeType::Enum", {}, 0u, 0x8204u, 0u, 0u, 4u, 4u, 0x9u, 0u, 0u, 0u, false, {}});
			types.push_back({"unsigned char", {}, 0u, 0x2004u, 0u, 0u, 1u, 1u, 0x9u, 0u, 0u, 0u, false, {}});
			types.push_back({"hknpCollisionDispatchType::Enum", {}, 0u, 0x8204u, 0u, 0u, 4u, 4u, 0x9u, 0u, 0u, 0u, false, {}});
			types.push_back({"hkRefCountedProperties", {}, 24u, 0x7u, 0u, 2u, 40u, 8u, 0x2du, 0u, 1u, 0u, false, {{"entries", 34u, 24u, 45u}}});
			types.push_back({"T*", {{"tT", 41u}}, 0u, 0x6u, 41u, 0u, 8u, 8u, 0x1bu, 0u, 0u, 0u, false, {}});
			types.push_back({"hkDefaultPropertyBag", {}, 0u, 0x7u, 0u, 1u, 72u, 8u, 0x2du, 0u, 3u, 0u, false, {{"propertyMap", 34u, 0u, 48u}, {"transientPropertyMap", 35u, 32u, 48u}, {"locked", 33u, 64u, 49u}}});
			types.push_back({"T*", {{"tT", 43u}}, 0u, 0x6u, 43u, 0u, 8u, 8u, 0x1bu, 64u, 0u, 0u, false, {}});
			types.push_back({"hkArray", {{"tT", 46u}, {"tAllocator", 9u}}, 0u, 0x8u, 46u, 0u, 16u, 8u, 0x2bu, 0u, 3u, 0u, false, {{"m_data", 34u, 0u, 47u}, {"m_size", 34u, 8u, 6u}, {"m_capacityAndFlags", 34u, 12u, 6u}}});
			types.push_back({"hkRefCountedProperties::Entry", {}, 0u, 0x7u, 0u, 0u, 16u, 8u, 0x29u, 0u, 3u, 0u, false, {{"object", 32u, 0u, 50u}, {"key", 32u, 8u, 36u}, {"flags", 32u, 10u, 36u}}});
			types.push_back({"T*", {{"tT", 46u}}, 0u, 0x6u, 46u, 0u, 8u, 8u, 0x1bu, 64u, 0u, 0u, false, {}});
			types.push_back({"hkHashMap", {{"tKEY", 52u}, {"tVALUE", 53u}}, 51u, 0x7u, 0u, 0u, 32u, 8u, 0x39u, 16u, 0u, 0u, false, {}});
			types.push_back({"bool", {}, 0u, 0x2002u, 0u, 0u, 1u, 1u, 0x9u, 0u, 0u, 0u, false, {}});
			types.push_back({"hkRefPtr", {{"tTYPE", 24u}}, 0u, 0x6u, 24u, 0u, 8u, 8u, 0x2bu, 0u, 1u, 0x5a986a75u, true, {{"ptr", 36u, 0u, 54u}}});
			types.push_back({"hkHashBase", {{"tITEM", 55u}}, 0u, 0x7u, 0u, 0u, 32u, 8u, 0x29u, 0u, 2u, 0u, false, {{"items", 34u, 0u, 56u}, {"index", 35u, 16u, 57u}}});
			types.push_back({"hkPropertyId", {}, 0u, 0x7u, 0u, 0u, 8u, 8u, 0x29u, 0u, 196609u, 0u, false, {{"desc", 37u, 0u, 58u}}});
			types.push_back({"hkReflect::Any", {}, 0u, 0x8u, 0u, 0u, 32u, 16u, 0x2bu, 0u, 3u, 0u, false, {{"type", 36u, 0u, 59u}, {"status", 36u, 8u, 39u}, {"buf", 36u, 16u, 60u}}});
			types.push_back({"T*", {{"tT", 24u}}, 0u, 0x6u, 24u, 0u, 8u, 8u, 0x1bu, 0u, 0u, 0u, false, {}});
			types.push_back({"hkHashMapDetail::MapTuple", {{"tKEY", 52u}, {"tVALUE", 53u}}, 61u, 0x7u, 0u, 0u, 48u, 16u, 0x39u, 16u, 0u, 0u, false, {}});
			types.push_back({"hkArray", {{"tT", 55u}, {"tAllocator", 9u}}, 0u, 0x8u, 55u, 0u, 16u, 8u, 0x2bu, 0u, 3u, 0u, false, {{"m_data", 34u, 0u, 62u}, {"m_size", 34u, 8u, 6u}, {"m_capacityAndFlags", 34u, 12u, 6u}}});
			types.push_back({"hkHashMapDetail::Index", {}, 0u, 0x7u, 0u, 0u, 16u, 8u, 0x29u, 0u, 2u, 0u, false, {{"entries", 32u, 0u, 63u}, {"hashMod", 32u, 8u, 6u}}});
			types.push_back({"hkPtrAndInt", {{"tPTYPE", 64u}, {"tITYPE", 32u}, {"vMASK", 1u}}, 0u, 0x7u, 0u, 0u, 8u, 8u, 0x39u, 1u, 1u, 0u, false, {{"ptrAndInt", 36u, 0u, 65u}}});
			types.push_back({"hkReflect::QualifiedType", {{"tTYPE", 66u}}, 0u, 0x6u, 66u, 0u, 8u, 8u, 0x2bu, 0u, 1u, 0u, false, {{"type", 36u, 0u, 67u}}});
			types.push_back({"T[N]", {{"tT", 68u}, {"vN", 4u}}, 0u, 0x428u, 68u, 0u, 16u, 4u, 0xbu, 0u, 0u, 0u, false, {}});
			types.push_back({"hkTuple", {{"tT0", 52u}, {"tT1", 53u}, {"tT2", 69u}, {"tT3", 69u}, {"tT4", 69u}, {"tT5", 69u}, {"tT6", 69u}, {"tT7", 69u}}, 0u, 0x7u, 0u, 0u, 48u, 16u, 0x29u, 0u, 2u, 0u, false, {{"0", 32u, 0u, 52u}, {"1", 32u, 16u, 53u}}});
			types.push_back({"T*", {{"tT", 55u}}, 0u, 0x6u, 55u, 0u, 8u, 8u, 0x1bu, 64u, 0u, 0u, false, {}});
			types.push_back({"T*", {{"tT", 69u}}, 0u, 0x6u, 69u, 0u, 8u, 8u, 0x1bu, 64u, 0u, 0u, false, {}});
			types.push_back({"hkPropertyDesc", {}, 0u, 0x7u, 0u, 0u, 24u, 8u, 0x29u, 0u, 3u, 0u, false, {{"type", 32u, 0u, 67u}, {"name", 32u, 8u, 70u}, {"flags", 32u, 16u, 71u}}});
			types.push_back({"T*", {{"tT", 64u}}, 0u, 0x6u, 64u, 0u, 8u, 8u, 0x1bu, 64u, 0u, 0u, false, {}});
			types.push_back({"hkReflect::Type", {}, 0u, 0x1u, 0u, 0u, 0u, 0u, 0x9u, 0u, 0u, 0u, false, {}});
			types.push_back({"T*", {{"tT", 66u}}, 0u, 0x6u, 66u, 0u, 8u, 8u, 0x1bu, 0u, 0u, 0u, false, {}});
			types.push_back({"hkUintReal", {}, 32u, 0x0u, 0u, 0u, 0u, 0u, 0x0u, 0u, 0u, 0u, false, {}});
			types.push_back({"void", {}, 0u, 0x0u, 0u, 0u, 0u, 0u, 0x19u, 1u, 0u, 0u, false, {}});
			types.push_back({"const char*", {}, 0u, 0x83u, 0u, 0u, 8u, 8u, 0x9u, 0u, 0u, 0u, false, {}});
			types.push_back({"hkFlags", {{"tBITS", 72u}, {"tSTORAGE", 21u}}, 0u, 0x8004u, 0u, 0u, 4u, 4u, 0x29u, 0u, 1u, 0u, false, {{"storage", 34u, 0u, 21u}}});
			types.push_back({"hkPropertyFlags::Enum", {}, 0u, 0x8204u, 0u, 0u, 4u, 4u, 0x9u, 0u, 0u, 0u, false, {}});
			types.push_back({"char", {}, 0u, 0x2004u, 0u, 0u, 1u, 1u, 0x9u, 0u, 0u, 0xf96fee69u, true, {}});
			types.push_back({"hknpCompressedMeshShape", {}, 75u, 0x7u, 0u, 7u, 128u, 8u, 0x2du, 0u, 5u, 0x87986deeu, true, {{"data", 34u, 72u, 50u}, {"triangleIsInterior", 34u, 80u, 76u}, {"numTriangles", 35u, 104u, 6u}, {"numConvexShapes", 35u, 108u, 6u}, {"externShapes", 34u, 112u, 77u}}});
			types.push_back({"hknpCompositeShape", {}, 18u, 0x7u, 0u, 2u, 72u, 8u, 0x2du, 0u, 2u, 0u, false, {{"shapeTagCodecInfo", 32u, 56u, 21u}, {"materialTable", 32u, 64u, 50u}}});
			types.push_back({"hkBitField", {}, 78u, 0x7u, 0u, 2u, 24u, 8u, 0x2du, 0u, 0u, 0u, false, {}});
			types.push_back({"hkArray", {{"tT", 79u}, {"tAllocator", 9u}}, 0u, 0x8u, 79u, 0u, 16u, 8u, 0x2bu, 0u, 3u, 0x49e94f1fu, true, {{"m_data", 34u, 0u, 80u}, {"m_size", 34u, 8u, 6u}, {"m_capacityAndFlags", 34u, 12u, 6u}}});
			types.push_back({"hkBitFieldBase", {{"tStorage", 81u}}, 0u, 0x7u, 0u, 0u, 24u, 8u, 0x29u, 0u, 1u, 0u, false, {{"storage", 34u, 0u, 81u}}});
			types.push_back({"hknpShapeInstance", {}, 0u, 0x7u, 0u, 3u, 112u, 16u, 0x2du, 0u, 9u, 0x0b7c6921u, true, {{"transform", 34u, 0u, 82u}, {"scale", 34u, 64u, 14u}, {"shape", 34u, 80u, 83u}, {"shapeTag", 34u, 88u, 36u}, {"destructionTag", 34u, 90u, 36u}, {"isEmpty", 34u, 92u, 27u}, {"nextEmptyElement", 34u, 96u, 21u}, {"instanceId", 35u, 100u, 84u}, {"parentShape", 35u, 104u, 85u}}});
			types.push_back({"T*", {{"tT", 79u}}, 0u, 0x6u, 79u, 0u, 8u, 8u, 0x1bu, 64u, 0u, 0u, false, {}});
			types.push_back({"hkBitFieldStorage", {{"tStorage", 86u}}, 0u, 0x7u, 0u, 0u, 24u, 8u, 0x29u, 0u, 2u, 0u, false, {{"words", 32u, 0u, 86u}, {"numBits", 32u, 16u, 6u}}});
			types.push_back({"hkTransform", {}, 87u, 0x0u, 0u, 0u, 0u, 0u, 0x0u, 0u, 0u, 0u, false, {}});
			types.push_back({"hkRefPtr", {{"tTYPE", 18u}}, 0u, 0x6u, 18u, 0u, 8u, 8u, 0x2bu, 0u, 1u, 0x5fb1f945u, true, {{"ptr", 36u, 0u, 8u}}});
			types.push_back({"hknpShapeInstanceId", {}, 88u, 0x0u, 0u, 1u, 0u, 0u, 0x4u, 0u, 0u, 0u, false, {}});
			types.push_back({"T*", {{"tT", 89u}}, 0u, 0x6u, 89u, 0u, 8u, 8u, 0x1bu, 0u, 0u, 0u, false, {}});
			types.push_back({"hkArray", {{"tT", 21u}, {"tAllocator", 9u}}, 0u, 0x8u, 21u, 0u, 16u, 8u, 0x2bu, 0u, 3u, 0x4223c021u, true, {{"m_data", 34u, 0u, 90u}, {"m_size", 34u, 8u, 6u}, {"m_capacityAndFlags", 34u, 12u, 6u}}});
			types.push_back({"hkTransformf", {}, 0u, 0x1028u, 31u, 0u, 64u, 16u, 0x2bu, 0u, 2u, 0u, false, {{"rotation", 34u, 0u, 91u}, {"translation", 34u, 48u, 20u}}});
			types.push_back({"hkHandle", {{"tTYPE", 36u}, {"vINVALID_VALUE", 65535u}}, 0u, 0x7u, 0u, 0u, 2u, 2u, 0x29u, 0u, 1u, 0u, false, {{"value", 34u, 0u, 36u}}});
			types.push_back({"hknpCompoundShape", {}, 75u, 0x7u, 0u, 6u, 288u, 16u, 0x2du, 0u, 8u, 0u, false, {{"instances", 34u, 72u, 92u}, {"instanceVelocities", 34u, 96u, 93u}, {"aabb", 34u, 112u, 94u}, {"boundingRadius", 34u, 144u, 29u}, {"isMutable", 32u, 148u, 95u}, {"estimatedNumShapeKeys", 34u, 152u, 6u}, {"mutationSignals", 35u, 160u, 96u}, {"boundingVolumeData", 34u, 176u, 97u}}});
			types.push_back({"T*", {{"tT", 21u}}, 0u, 0x6u, 21u, 0u, 8u, 8u, 0x1bu, 64u, 0u, 0u, false, {}});
			types.push_back({"hkRotationf", {}, 98u, 0x0u, 0u, 0u, 0u, 0u, 0x0u, 0u, 0u, 0u, false, {}});
			types.push_back({"hkFreeListArray", {{"tVALUE_TYPE", 79u}, {"vGROWTH", 8u}}, 0u, 0x7u, 0u, 2u, 24u, 8u, 0x2du, 0u, 3u, 0u, false, {{"elements", 34u, 0u, 99u}, {"firstFree", 34u, 16u, 100u}, {"numAllocated", 34u, 20u, 21u}}});
			types.push_back({"hkArray", {{"tT", 101u}, {"tAllocator", 9u}}, 0u, 0x8u, 101u, 0u, 16u, 8u, 0x2bu, 0u, 3u, 0u, false, {{"m_data", 34u, 0u, 102u}, {"m_size", 34u, 8u, 6u}, {"m_capacityAndFlags", 34u, 12u, 6u}}});
			types.push_back({"hkAabb", {}, 0u, 0x7u, 0u, 0u, 32u, 16u, 0x29u, 0u, 2u, 0u, false, {{"min", 32u, 0u, 14u}, {"max", 32u, 16u, 14u}}});
			types.push_back({"hkBool", {}, 0u, 0x2002u, 0u, 0u, 1u, 1u, 0x29u, 0u, 1u, 0u, false, {{"bool", 36u, 0u, 73u}}});
			types.push_back({"hknpShapeSignals", {}, 0u, 0x7u, 0u, 0u, 16u, 8u, 0x29u, 0u, 2u, 0u, false, {{"shapeMutated", 33u, 0u, 103u}, {"shapeDestroyed", 33u, 8u, 103u}}});
			types.push_back({"hknpCompoundShapeData", {}, 24u, 0x7u, 0u, 2u, 112u, 8u, 0x2du, 0u, 3u, 0u, false, {{"aabbTree", 32u, 24u, 104u}, {"simdTree", 32u, 64u, 105u}, {"type", 32u, 104u, 106u}}});
			types.push_back({"hkRotationImpl", {{"tFT", 31u}}, 0u, 0xc28u, 31u, 0u, 48u, 16u, 0xbu, 0u, 0u, 0u, false, {}});
			types.push_back({"hkArray", {{"tT", 107u}, {"tAllocator", 9u}}, 0u, 0x8u, 107u, 0u, 16u, 8u, 0x2bu, 0u, 3u, 0u, false, {{"m_data", 34u, 0u, 108u}, {"m_size", 34u, 8u, 6u}, {"m_capacityAndFlags", 34u, 12u, 6u}}});
			types.push_back({"hkInt32", {}, 6u, 0x0u, 0u, 0u, 0u, 0u, 0x0u, 0u, 0u, 0u, false, {}});
			types.push_back({"hknpCompoundShape::VelocityInfo", {}, 0u, 0x7u, 0u, 0u, 32u, 16u, 0x29u, 0u, 2u, 0u, false, {{"linearVelocity", 32u, 0u, 14u}, {"angularVelocity", 32u, 16u, 14u}}});
			types.push_back({"T*", {{"tT", 101u}}, 0u, 0x6u, 101u, 0u, 8u, 8u, 0x1bu, 64u, 0u, 0u, false, {}});
			types.push_back({"T*", {{"tT", 109u}}, 0u, 0x6u, 109u, 0u, 8u, 8u, 0x1bu, 64u, 0u, 0u, false, {}});
			types.push_back({"hknpCompoundShapeCdDynamicTree", {}, 110u, 0x7u, 0u, 0u, 40u, 8u, 0x29u, 0u, 0u, 0u, false, {}});
			types.push_back({"hknpCompoundShapeSimdTree", {}, 111u, 0x7u, 0u, 0u, 40u, 8u, 0x29u, 0u, 1u, 0u, false, {{"points", 32u, 24u, 5u}}});
			types.push_back({"hkEnum", {{"tENUM", 112u}, {"tSTORAGE", 27u}}, 0u, 0x2004u, 0u, 0u, 1u, 1u, 0x29u, 0u, 1u, 0u, false, {{"storage", 36u, 0u, 27u}}});
			types.push_back({"hkFreeListArrayElement", {{"tVALUE_TYPE", 79u}}, 79u, 0x0u, 0u, 0u, 0u, 0u, 0x0u, 0u, 0u, 0u, false, {}});
			types.push_back({"T*", {{"tT", 107u}}, 0u, 0x6u, 107u, 0u, 8u, 8u, 0x1bu, 64u, 0u, 0u, false, {}});
			types.push_back({"hkReflect::Detail::Opaque", {}, 0u, 0x1u, 0u, 0u, 0u, 0u, 0x9u, 0u, 0u, 0u, false, {}});
			types.push_back({"hkcdDynamicTree::DefaultTree32", {}, 113u, 0x0u, 0u, 0u, 0u, 0u, 0x0u, 0u, 0u, 0u, false, {}});
			types.push_back({"hkcdSimdTree", {}, 0u, 0x7u, 0u, 3u, 24u, 8u, 0x2du, 0u, 2u, 0u, false, {{"nodes", 32u, 0u, 114u}, {"isCompact", 32u, 16u, 49u}}});
			types.push_back({"hknpCompoundShapeBoundingVolumeType::Enum", {}, 0u, 0x8204u, 0u, 0u, 4u, 4u, 0x9u, 0u, 0u, 0u, false, {}});
			types.push_back({"hkcdDynamicTree::Tree", {{"tSTORAGE", 115u}}, 115u, 0x7u, 0u, 0u, 40u, 8u, 0x29u, 0u, 3u, 0u, false, {{"numLeaves", 32u, 24u, 21u}, {"path", 32u, 28u, 21u}, {"root", 32u, 32u, 22u}}});
			types.push_back({"hkArray", {{"tT", 116u}, {"tAllocator", 9u}}, 0u, 0x8u, 116u, 0u, 16u, 8u, 0x2bu, 0u, 3u, 0x28099114u, true, {{"m_data", 34u, 0u, 117u}, {"m_size", 34u, 8u, 6u}, {"m_capacityAndFlags", 34u, 12u, 6u}}});
			types.push_back({"hkcdDynamicTree::DynamicStorage16", {}, 118u, 0x0u, 0u, 0u, 0u, 0u, 0x0u, 0u, 0u, 0u, false, {}});
			types.push_back({"hkcdSimdTree::Node", {}, 119u, 0x7u, 0u, 2u, 128u, 16u, 0x2du, 0u, 3u, 0x8c79e5ceu, true, {{"data", 32u, 96u, 120u}, {"isLeaf", 32u, 112u, 49u}, {"isActive", 33u, 113u, 49u}}});
			types.push_back({"T*", {{"tT", 116u}}, 0u, 0x6u, 116u, 0u, 8u, 8u, 0x1bu, 64u, 0u, 0u, false, {}});
			types.push_back({"hkcdDynamicTree::DefaultDynamicStorage", {{"tCODEC", 122u}}, 121u, 0x7u, 0u, 0u, 24u, 8u, 0x39u, 16u, 0u, 0u, false, {}});
			types.push_back({"hkcdFourAabb", {}, 0u, 0x7u, 0u, 0u, 96u, 16u, 0x29u, 0u, 6u, 0u, false, {{"lx", 32u, 0u, 14u}, {"hx", 32u, 16u, 14u}, {"ly", 32u, 32u, 14u}, {"hy", 32u, 48u, 14u}, {"lz", 32u, 64u, 14u}, {"hz", 32u, 80u, 14u}}});
			types.push_back({"T[N]", {{"tT", 21u}, {"vN", 4u}}, 0u, 0x428u, 21u, 0u, 16u, 4u, 0xbu, 0u, 0u, 0u, false, {}});
			types.push_back({"hkcdDynamicTree::DynamicStorage", {{"vGROWTH", 0u}, {"tMETRIC", 123u}, {"tCODEC", 122u}}, 123u, 0x7u, 0u, 0u, 24u, 8u, 0x29u, 0u, 2u, 0u, false, {{"nodes", 32u, 0u, 124u}, {"firstFree", 32u, 16u, 22u}}});
			types.push_back({"hkcdDynamicTree::Codec32", {}, 0u, 0x7u, 0u, 0u, 32u, 16u, 0x29u, 0u, 1u, 0u, false, {{"aabb", 34u, 0u, 94u}}});
			types.push_back({"hkcdDynamicTree::AnisotropicMetric", {}, 0u, 0x7u, 0u, 0u, 1u, 1u, 0x39u, 16u, 0u, 0u, false, {}});
			types.push_back({"hkArray", {{"tT", 122u}, {"tAllocator", 9u}}, 0u, 0x8u, 122u, 0u, 16u, 8u, 0x2bu, 0u, 3u, 0u, false, {{"m_data", 34u, 0u, 125u}, {"m_size", 34u, 8u, 6u}, {"m_capacityAndFlags", 34u, 12u, 6u}}});
			types.push_back({"T*", {{"tT", 122u}}, 0u, 0x6u, 122u, 0u, 8u, 8u, 0x1bu, 64u, 0u, 0u, false, {}});
			types.push_back({"hknpCompressedMeshShapeData", {}, 24u, 0x7u, 0u, 2u, 272u, 16u, 0x2du, 0u, 4u, 0x3578a812u, true, {{"meshTree", 32u, 32u, 127u}, {"simdTree", 32u, 192u, 111u}, {"connectivity", 32u, 216u, 128u}, {"hasSimdTree", 32u, 264u, 49u}}});
			types.push_back({"hknpCompressedMeshShapeTree", {}, 129u, 0x7u, 0u, 0u, 160u, 16u, 0x29u, 0u, 0u, 0u, false, {}});
			types.push_back({"hkcdStaticMeshTree::Connectivity", {}, 0u, 0x7u, 0u, 0u, 48u, 8u, 0x29u, 0u, 3u, 0u, false, {{"headers", 32u, 0u, 130u}, {"localLinks", 32u, 16u, 131u}, {"globalLinks", 32u, 32u, 132u}}});
			types.push_back({"hkcdDefaultStaticMeshTree", {}, 133u, 0x7u, 0u, 0u, 160u, 16u, 0x29u, 0u, 3u, 0u, false, {{"packedVertices", 32u, 112u, 134u}, {"sharedVertices", 32u, 128u, 135u}, {"primitiveDataRuns", 32u, 144u, 136u}}});
			types.push_back({"hkArray", {{"tT", 137u}, {"tAllocator", 9u}}, 0u, 0x8u, 137u, 0u, 16u, 8u, 0x2bu, 0u, 3u, 0u, false, {{"m_data", 34u, 0u, 138u}, {"m_size", 34u, 8u, 6u}, {"m_capacityAndFlags", 34u, 12u, 6u}}});
			types.push_back({"hkArray", {{"tT", 27u}, {"tAllocator", 9u}}, 0u, 0x8u, 27u, 0u, 16u, 8u, 0x2bu, 0u, 3u, 0u, false, {{"m_data", 34u, 0u, 139u}, {"m_size", 34u, 8u, 6u}, {"m_capacityAndFlags", 34u, 12u, 6u}}});
			types.push_back({"hkArray", {{"tT", 140u}, {"tAllocator", 9u}}, 0u, 0x8u, 140u, 0u, 16u, 8u, 0x2bu, 0u, 3u, 0u, false, {{"m_data", 34u, 0u, 141u}, {"m_size", 34u, 8u, 6u}, {"m_capacityAndFlags", 34u, 12u, 6u}}});
			types.push_back({"hkcdStaticMeshTree::Base", {}, 142u, 0x7u, 0u, 0u, 112u, 16u, 0x29u, 0u, 7u, 0u, false, {{"numPrimitiveKeys", 32u, 48u, 6u}, {"bitsPerKey", 32u, 52u, 6u}, {"maxKeyValue", 32u, 56u, 21u}, {"primitiveStoresIsFlatConvex", 32u, 60u, 27u}, {"sections", 32u, 64u, 143u}, {"primitives", 32u, 80u, 144u}, {"sharedVerticesIndex", 32u, 96u, 145u}}});
			types.push_back({"hkArray", {{"tT", 32u}, {"tAllocator", 9u}}, 0u, 0x8u, 32u, 0u, 16u, 8u, 0x2bu, 0u, 3u, 0x7e15a107u, true, {{"m_data", 34u, 0u, 146u}, {"m_size", 34u, 8u, 6u}, {"m_capacityAndFlags", 34u, 12u, 6u}}});
			types.push_back({"hkArray", {{"tT", 33u}, {"tAllocator", 9u}}, 0u, 0x8u, 33u, 0u, 16u, 8u, 0x2bu, 0u, 3u, 0xd04b35ecu, true, {{"m_data", 34u, 0u, 147u}, {"m_size", 34u, 8u, 6u}, {"m_capacityAndFlags", 34u, 12u, 6u}}});
			types.push_back({"hkArray", {{"tT", 148u}, {"tAllocator", 9u}}, 0u, 0x8u, 148u, 0u, 16u, 8u, 0x2bu, 0u, 3u, 0x402b9beeu, true, {{"m_data", 34u, 0u, 149u}, {"m_size", 34u, 8u, 6u}, {"m_capacityAndFlags", 34u, 12u, 6u}}});
			types.push_back({"hkcdStaticMeshTree::Connectivity::SectionHeader", {}, 0u, 0x7u, 0u, 0u, 8u, 4u, 0x29u, 0u, 2u, 0u, false, {{"baseLocal", 32u, 0u, 21u}, {"baseGlobal", 32u, 4u, 21u}}});
			types.push_back({"T*", {{"tT", 137u}}, 0u, 0x6u, 137u, 0u, 8u, 8u, 0x1bu, 64u, 0u, 0u, false, {}});
			types.push_back({"T*", {{"tT", 27u}}, 0u, 0x6u, 27u, 0u, 8u, 8u, 0x1bu, 64u, 0u, 0u, false, {}});
			types.push_back({"hkHandle", {{"tTYPE", 21u}, {"vINVALID_VALUE", 4294967295u}}, 0u, 0x7u, 0u, 0u, 4u, 4u, 0x29u, 0u, 1u, 0u, false, {{"value", 34u, 0u, 21u}}});
			types.push_back({"T*", {{"tT", 140u}}, 0u, 0x6u, 140u, 0u, 8u, 8u, 0x1bu, 64u, 0u, 0u, false, {}});
			types.push_back({"hkcdStaticTree::Aabb5BytesTree", {}, 150u, 0x0u, 0u, 0u, 0u, 0u, 0x4u, 0u, 0u, 0u, false, {}});
			types.push_back({"hkArray", {{"tT", 151u}, {"tAllocator", 9u}}, 0u, 0x8u, 151u, 0u, 16u, 8u, 0x2bu, 0u, 3u, 0x10a1aa51u, true, {{"m_data", 34u, 0u, 152u}, {"m_size", 34u, 8u, 6u}, {"m_capacityAndFlags", 34u, 12u, 6u}}});
			types.push_back({"hkArray", {{"tT", 153u}, {"tAllocator", 9u}}, 0u, 0x8u, 153u, 0u, 16u, 8u, 0x2bu, 0u, 3u, 0xe7f5de94u, true, {{"m_data", 34u, 0u, 154u}, {"m_size", 34u, 8u, 6u}, {"m_capacityAndFlags", 34u, 12u, 6u}}});
			types.push_back({"hkArray", {{"tT", 36u}, {"tAllocator", 9u}}, 0u, 0x8u, 36u, 0u, 16u, 8u, 0x2bu, 0u, 3u, 0xcc83466eu, true, {{"m_data", 34u, 0u, 155u}, {"m_size", 34u, 8u, 6u}, {"m_capacityAndFlags", 34u, 12u, 6u}}});
			types.push_back({"T*", {{"tT", 32u}}, 0u, 0x6u, 32u, 0u, 8u, 8u, 0x1bu, 64u, 0u, 0u, false, {}});
			types.push_back({"T*", {{"tT", 33u}}, 0u, 0x6u, 33u, 0u, 8u, 8u, 0x1bu, 64u, 0u, 0u, false, {}});
			types.push_back({"hkcdDefaultStaticMeshTree::PrimitiveDataRun", {}, 0u, 0x7u, 0u, 0u, 4u, 2u, 0x29u, 0u, 3u, 0xc01f50e6u, true, {{"value", 32u, 0u, 36u}, {"index", 32u, 2u, 27u}, {"count", 32u, 3u, 27u}}});
			types.push_back({"T*", {{"tT", 148u}}, 0u, 0x6u, 148u, 0u, 8u, 8u, 0x1bu, 64u, 0u, 0u, false, {}});
			types.push_back({"hkcdStaticTree::AabbTree", {{"tNODE", 157u}}, 156u, 0x7u, 0u, 0u, 48u, 16u, 0x29u, 0u, 2u, 0u, false, {{"nodes", 32u, 0u, 158u}, {"domain", 32u, 16u, 94u}}});
			types.push_back({"hkcdStaticMeshTree::Section", {}, 159u, 0x7u, 0u, 0u, 96u, 16u, 0x29u, 0u, 12u, 0x0fc0f973u, true, {{"codecParms", 32u, 48u, 160u}, {"firstPackedVertexIndex", 32u, 72u, 21u}, {"firstSharedVertexIndex", 32u, 76u, 21u}, {"firstPrimitiveIndex", 32u, 80u, 21u}, {"firstDataRunIndex", 32u, 84u, 21u}, {"numPackedVertices", 32u, 88u, 27u}, {"numPrimitives", 32u, 89u, 27u}, {"numDataRuns", 32u, 90u, 27u}, {"page", 32u, 91u, 27u}, {"leafIndex", 32u, 92u, 36u}, {"layerData", 32u, 94u, 27u}, {"flags", 32u, 95u, 27u}}});
			types.push_back({"T*", {{"tT", 151u}}, 0u, 0x6u, 151u, 0u, 8u, 8u, 0x1bu, 64u, 0u, 0u, false, {}});
			types.push_back({"hkcdStaticMeshTree::Primitive", {}, 0u, 0x7u, 0u, 0u, 4u, 1u, 0x29u, 0u, 1u, 0x384dd2c0u, true, {{"indices", 32u, 0u, 161u}}});
			types.push_back({"T*", {{"tT", 153u}}, 0u, 0x6u, 153u, 0u, 8u, 8u, 0x1bu, 64u, 0u, 0u, false, {}});
			types.push_back({"T*", {{"tT", 36u}}, 0u, 0x6u, 36u, 0u, 8u, 8u, 0x1bu, 64u, 0u, 0u, false, {}});
			types.push_back({"hkcdStaticTree::AabbTreeBase", {}, 0u, 0x7u, 0u, 0u, 1u, 1u, 0x39u, 16u, 0u, 0u, false, {}});
			types.push_back({"hkcdCompressedAabbCodecs::Aabb5BytesCodec", {}, 162u, 0x7u, 0u, 0u, 5u, 1u, 0x29u, 0u, 2u, 0x624ba1e8u, true, {{"hiData", 32u, 3u, 27u}, {"loData", 32u, 4u, 27u}}});
			types.push_back({"hkArray", {{"tT", 157u}, {"tAllocator", 9u}}, 0u, 0x8u, 157u, 0u, 16u, 8u, 0x2bu, 0u, 3u, 0x6a61ef37u, true, {{"m_data", 34u, 0u, 163u}, {"m_size", 34u, 8u, 6u}, {"m_capacityAndFlags", 34u, 12u, 6u}}});
			types.push_back({"hkcdStaticTree::Aabb4BytesTree", {}, 164u, 0x0u, 0u, 0u, 0u, 0u, 0x4u, 0u, 0u, 0u, false, {}});
			types.push_back({"T[N]", {{"tT", 29u}, {"vN", 6u}}, 0u, 0x628u, 29u, 0u, 24u, 4u, 0xbu, 0u, 0u, 0u, false, {}});
			types.push_back({"T[N]", {{"tT", 27u}, {"vN", 4u}}, 0u, 0x428u, 27u, 0u, 4u, 1u, 0xbu, 0u, 0u, 0u, false, {}});
			types.push_back({"hkcdCompressedAabbCodecs::CompressedAabbCodec", {}, 165u, 0x7u, 0u, 0u, 3u, 1u, 0x29u, 0u, 1u, 0u, false, {{"xyz", 32u, 0u, 166u}}});
			types.push_back({"T*", {{"tT", 157u}}, 0u, 0x6u, 157u, 0u, 8u, 8u, 0x1bu, 64u, 0u, 0u, false, {}});
			types.push_back({"hkcdStaticTree::AabbTree", {{"tNODE", 167u}}, 156u, 0x7u, 0u, 0u, 48u, 16u, 0x29u, 0u, 2u, 0u, false, {{"nodes", 32u, 0u, 168u}, {"domain", 32u, 16u, 94u}}});
			types.push_back({"hkcdCompressedAabbCodecs::AabbCodecBase", {}, 0u, 0x7u, 0u, 0u, 1u, 1u, 0x39u, 16u, 0u, 0u, false, {}});
			types.push_back({"T[N]", {{"tT", 27u}, {"vN", 3u}}, 0u, 0x328u, 27u, 0u, 3u, 1u, 0xbu, 0u, 0u, 0u, false, {}});
			types.push_back({"hkcdCompressedAabbCodecs::Aabb4BytesCodec", {}, 162u, 0x7u, 0u, 0u, 4u, 1u, 0x29u, 0u, 1u, 0xaf5dc778u, true, {{"data", 32u, 3u, 27u}}});
			types.push_back({"hkArray", {{"tT", 167u}, {"tAllocator", 9u}}, 0u, 0x8u, 167u, 0u, 16u, 8u, 0x2bu, 0u, 3u, 0x6b1c9ff2u, true, {{"m_data", 34u, 0u, 169u}, {"m_size", 34u, 8u, 6u}, {"m_capacityAndFlags", 34u, 12u, 6u}}});
			types.push_back({"T*", {{"tT", 167u}}, 0u, 0x6u, 167u, 0u, 8u, 8u, 0x1bu, 64u, 0u, 0u, false, {}});
			types.push_back({"hknpBoxShape", {}, 171u, 0x7u, 0u, 0u, 160u, 16u, 0x29u, 0u, 1u, 0xe70f3817u, true, {{"obb", 34u, 96u, 87u}}});
			types.push_back({"hknpConvexPolytopeShape", {}, 172u, 0x7u, 0u, 4u, 88u, 8u, 0x2du, 0u, 4u, 0xf1bd170du, true, {{"planes", 34u, 64u, 173u}, {"faces", 34u, 68u, 174u}, {"indices", 34u, 72u, 175u}, {"connectivity", 34u, 80u, 176u}}});
			types.push_back({"hknpConvexShape", {}, 18u, 0x7u, 0u, 2u, 64u, 8u, 0x2du, 0u, 2u, 0u, false, {{"maxAllowedPenetration", 34u, 56u, 177u}, {"vertices", 34u, 58u, 178u}}});
			types.push_back({"hkRelArray", {{"tT", 14u}}, 0u, 0x8u, 14u, 0u, 4u, 2u, 0x2bu, 0u, 2u, 0x0f6842f4u, true, {{"size", 34u, 0u, 36u}, {"offset", 34u, 2u, 36u}}});
			types.push_back({"hkRelArray", {{"tT", 179u}}, 0u, 0x8u, 179u, 0u, 4u, 2u, 0x2bu, 0u, 2u, 0x025fb763u, true, {{"size", 34u, 0u, 36u}, {"offset", 34u, 2u, 36u}}});
			types.push_back({"hkRelArray", {{"tT", 27u}}, 0u, 0x8u, 27u, 0u, 4u, 2u, 0x2bu, 0u, 2u, 0x452b9f92u, true, {{"size", 34u, 0u, 36u}, {"offset", 34u, 2u, 36u}}});
			types.push_back({"hkRefPtr", {{"tTYPE", 180u}}, 0u, 0x6u, 180u, 0u, 8u, 8u, 0x2bu, 0u, 1u, 0x21a464bdu, true, {{"ptr", 36u, 0u, 181u}}});
			types.push_back({"hkHalf16", {}, 0u, 0x74605u, 0u, 0u, 2u, 2u, 0x29u, 0u, 1u, 0u, false, {{"value", 36u, 0u, 182u}}});
			types.push_back({"hkRelArray", {{"tT", 20u}}, 0u, 0x8u, 20u, 0u, 4u, 2u, 0x2bu, 0u, 2u, 0x2b264726u, true, {{"size", 34u, 0u, 36u}, {"offset", 34u, 2u, 36u}}});
			types.push_back({"hknpConvexPolytopeShape::Face", {}, 0u, 0x7u, 0u, 0u, 4u, 2u, 0x29u, 0u, 3u, 0xeaad8c84u, true, {{"firstIndex", 32u, 0u, 36u}, {"numIndices", 32u, 2u, 27u}, {"minHalfAngle", 32u, 3u, 27u}}});
			types.push_back({"hknpConvexPolytopeShape::Connectivity", {}, 24u, 0x7u, 0u, 0u, 56u, 8u, 0x29u, 0u, 2u, 0xa12ddce7u, true, {{"vertexEdges", 32u, 24u, 183u}, {"faceLinks", 32u, 40u, 183u}}});
			types.push_back({"T*", {{"tT", 180u}}, 0u, 0x6u, 180u, 0u, 8u, 8u, 0x1bu, 0u, 0u, 0u, false, {}});
			types.push_back({"hkInt16", {}, 184u, 0x0u, 0u, 0u, 0u, 0u, 0x0u, 0u, 0u, 0u, false, {}});
			types.push_back({"hkArray", {{"tT", 185u}, {"tAllocator", 9u}}, 0u, 0x8u, 185u, 0u, 16u, 8u, 0x2bu, 0u, 3u, 0x9b6a2731u, true, {{"m_data", 34u, 0u, 186u}, {"m_size", 34u, 8u, 6u}, {"m_capacityAndFlags", 34u, 12u, 6u}}});
			types.push_back({"short", {}, 0u, 0x4204u, 0u, 0u, 2u, 2u, 0x9u, 0u, 0u, 0u, false, {}});
			types.push_back({"hknpConvexPolytopeShape::Connectivity::Edge", {}, 0u, 0x7u, 0u, 0u, 4u, 2u, 0x29u, 0u, 3u, 0xa816b6f7u, true, {{"faceIndex", 32u, 0u, 36u}, {"edgeIndex", 32u, 2u, 27u}, {"padding", 33u, 3u, 187u}}});
			types.push_back({"T*", {{"tT", 185u}}, 0u, 0x6u, 185u, 0u, 8u, 8u, 0x1bu, 64u, 0u, 0u, false, {}});
			types.push_back({"T[N]", {{"tT", 27u}, {"vN", 1u}}, 0u, 0x128u, 27u, 0u, 1u, 1u, 0xbu, 0u, 0u, 0u, false, {}});
			return types;
		}

		std::vector<::havok::tagfile::type_info> make_physics_asset_schema()
		{
			std::vector<::havok::tagfile::type_info> types;
			types.reserve(152);
			types.emplace_back();
			types.push_back({"HavokPhysicsAsset", {}, 0u, 0x7u, 0u, 0u, 224u, 8u, 0x29u, 0u, 14u, 0x8ae1219bu, true, {{"useCategory", 32u, 0u, 2u}, {"physicsSystemData", 32u, 8u, 3u}, {"bodyQualityNameCRCLookup", 32u, 16u, 120u}, {"materialNameCRCLookup", 32u, 32u, 120u}, {"motionPropertiesNameCRCLookup", 32u, 48u, 120u}, {"bodySFXAssetNames", 32u, 64u, 122u}, {"bodyVFXAssetNames", 32u, 80u, 122u}, {"bodyServerUsage", 32u, 96u, 120u}, {"constraintServerUsage", 32u, 112u, 120u}, {"bodyDrivers", 32u, 128u, 120u}, {"simulationCategories", 32u, 144u, 120u}, {"mutable", 32u, 160u, 124u}, {"mutableShapeNames", 32u, 176u, 122u}, {"mutableShapeLookup", 32u, 192u, 126u}}});
			types.push_back({"int", {}, 0u, 0x8204u, 0u, 0u, 4u, 4u, 0x9u, 0u, 0u, 0xf2226812u, true, {}});
			types.push_back({"T*", {{"tT", 4u}}, 0u, 0x6u, 4u, 0u, 8u, 8u, 0x1bu, 0u, 0u, 0x283fa9ecu, true, {}});
			types.push_back({"hknpPhysicsSystemData", {}, 5u, 0x7u, 0u, 3u, 104u, 8u, 0x2du, 0u, 6u, 0x9d844619u, true, {{"materials", 32u, 24u, 39u}, {"motionProperties", 32u, 40u, 61u}, {"bodyCinfos", 32u, 56u, 66u}, {"constraintCinfos", 32u, 72u, 108u}, {"name", 32u, 88u, 41u}, {"microStepMultiplier", 32u, 96u, 45u}}});
			types.push_back({"hkReferencedObject", {}, 6u, 0x7u, 0u, 3u, 24u, 8u, 0x2du, 0u, 3u, 0x9a3a0860u, true, {{"propertyBag", 36u, 8u, 7u}, {"memSizeAndFlags", 37u, 16u, 37u}, {"refCount", 37u, 18u, 37u}}});
			types.push_back({"hkBaseObject", {}, 0u, 0x7u, 0u, 0u, 8u, 8u, 0x39u, 144u, 0u, 0xa27d6612u, true, {}});
			types.push_back({"hkPropertyBag", {}, 0u, 0x8u, 8u, 0u, 8u, 8u, 0x2bu, 0u, 1u, 0xf3f8a311u, true, {{"bag", 35u, 0u, 36u}}});
			types.push_back({"hkDefaultPropertyBag", {}, 0u, 0x7u, 0u, 1u, 72u, 8u, 0x2du, 0u, 3u, 0xa4e13c81u, true, {{"propertyMap", 34u, 0u, 9u}, {"transientPropertyMap", 35u, 32u, 9u}, {"locked", 33u, 64u, 35u}}});
			types.push_back({"hkHashMap", {{"tKEY", 13u}, {"tVALUE", 24u}}, 10u, 0x7u, 0u, 0u, 32u, 8u, 0x39u, 16u, 0u, 0x66f3b28au, true, {}});
			types.push_back({"hkHashBase", {{"tITEM", 11u}}, 0u, 0x7u, 0u, 0u, 32u, 8u, 0x29u, 0u, 2u, 0xcd3c24a6u, true, {{"items", 34u, 0u, 30u}, {"index", 35u, 16u, 33u}}});
			types.push_back({"hkHashMapDetail::MapTuple", {{"tKEY", 13u}, {"tVALUE", 24u}}, 12u, 0x7u, 0u, 0u, 48u, 16u, 0x39u, 16u, 0u, 0xdc5990d9u, true, {}});
			types.push_back({"hkTuple", {{"tT0", 13u}, {"tT1", 24u}, {"tT2", 29u}, {"tT3", 29u}, {"tT4", 29u}, {"tT5", 29u}, {"tT6", 29u}, {"tT7", 29u}}, 0u, 0x7u, 0u, 0u, 48u, 16u, 0x29u, 0u, 2u, 0x8e6f0a41u, true, {{"0", 32u, 0u, 13u}, {"1", 32u, 16u, 24u}}});
			types.push_back({"hkPropertyId", {}, 0u, 0x7u, 0u, 0u, 8u, 8u, 0x29u, 0u, 196609u, 0x3bba4203u, true, {{"desc", 37u, 0u, 14u}}});
			types.push_back({"hkPtrAndInt", {{"tPTYPE", 15u}, {"tITYPE", 22u}, {"vMASK", 1u}}, 0u, 0x7u, 0u, 0u, 8u, 8u, 0x39u, 1u, 1u, 0x00000000u, false, {{"ptrAndInt", 36u, 0u, 23u}}});
			types.push_back({"hkPropertyDesc", {}, 0u, 0x7u, 0u, 0u, 24u, 8u, 0x29u, 0u, 3u, 0x257ed6e3u, true, {{"type", 32u, 0u, 16u}, {"name", 32u, 8u, 18u}, {"flags", 32u, 16u, 19u}}});
			types.push_back({"T*", {{"tT", 17u}}, 0u, 0x6u, 17u, 0u, 8u, 8u, 0x1bu, 0u, 0u, 0xda3f2275u, true, {}});
			types.push_back({"hkReflect::Type", {}, 0u, 0x1u, 0u, 0u, 0u, 0u, 0x9u, 0u, 0u, 0xbe74729cu, true, {}});
			types.push_back({"const char*", {}, 0u, 0x83u, 0u, 0u, 8u, 8u, 0x9u, 0u, 0u, 0x47f55c2eu, true, {}});
			types.push_back({"hkFlags", {{"tBITS", 20u}, {"tSTORAGE", 21u}}, 0u, 0x8004u, 0u, 0u, 4u, 4u, 0x29u, 0u, 1u, 0x44a716a2u, true, {{"storage", 34u, 0u, 21u}}});
			types.push_back({"hkPropertyFlags::Enum", {}, 0u, 0x8204u, 0u, 0u, 4u, 4u, 0x9u, 0u, 0u, 0x54aa7442u, true, {}});
			types.push_back({"hkUint32", {}, 22u, 0x0u, 0u, 0u, 0u, 0u, 0x0u, 0u, 0u, 0x664be534u, true, {}});
			types.push_back({"unsigned int", {}, 0u, 0x8004u, 0u, 0u, 4u, 4u, 0x9u, 0u, 0u, 0x94e0eb06u, true, {}});
			types.push_back({"T*", {{"tT", 15u}}, 0u, 0x6u, 15u, 0u, 8u, 8u, 0x1bu, 64u, 0u, 0xdd100154u, true, {}});
			types.push_back({"hkReflect::Any", {}, 0u, 0x8u, 0u, 0u, 32u, 16u, 0x2bu, 0u, 3u, 0xfd34ff7eu, true, {{"type", 36u, 0u, 25u}, {"status", 36u, 8u, 26u}, {"buf", 36u, 16u, 27u}}});
			types.push_back({"hkReflect::QualifiedType", {{"tTYPE", 17u}}, 0u, 0x6u, 17u, 0u, 8u, 8u, 0x2bu, 0u, 1u, 0xfb96a177u, true, {{"type", 36u, 0u, 16u}}});
			types.push_back({"unsigned char", {}, 0u, 0x2004u, 0u, 0u, 1u, 1u, 0x9u, 0u, 0u, 0x3bcfeb39u, true, {}});
			types.push_back({"T[N]", {{"tT", 28u}, {"vN", 4u}}, 0u, 0x428u, 28u, 0u, 16u, 4u, 0xbu, 0u, 0u, 0xf5946b34u, true, {}});
			types.push_back({"hkUintReal", {}, 22u, 0x0u, 0u, 0u, 0u, 0u, 0x0u, 0u, 0u, 0xf8184e72u, true, {}});
			types.push_back({"void", {}, 0u, 0x0u, 0u, 0u, 0u, 0u, 0x19u, 1u, 0u, 0x00000000u, false, {}});
			types.push_back({"hkArray", {{"tT", 11u}, {"tAllocator", 31u}}, 0u, 0x8u, 11u, 0u, 16u, 8u, 0x2bu, 0u, 3u, 0xb3c5b2bau, true, {{"m_data", 34u, 0u, 32u}, {"m_size", 34u, 8u, 2u}, {"m_capacityAndFlags", 34u, 12u, 2u}}});
			types.push_back({"hkContainerHeapAllocator", {}, 0u, 0x7u, 0u, 0u, 1u, 1u, 0x39u, 16u, 0u, 0x3745716au, true, {}});
			types.push_back({"T*", {{"tT", 11u}}, 0u, 0x6u, 11u, 0u, 8u, 8u, 0x1bu, 64u, 0u, 0x7920e8fdu, true, {}});
			types.push_back({"hkHashMapDetail::Index", {}, 0u, 0x7u, 0u, 0u, 16u, 8u, 0x29u, 0u, 2u, 0x513e8ee6u, true, {{"entries", 32u, 0u, 34u}, {"hashMod", 32u, 8u, 2u}}});
			types.push_back({"T*", {{"tT", 29u}}, 0u, 0x6u, 29u, 0u, 8u, 8u, 0x1bu, 64u, 0u, 0x5e656280u, true, {}});
			types.push_back({"bool", {}, 0u, 0x2002u, 0u, 0u, 1u, 1u, 0x9u, 0u, 0u, 0xfa853d0au, true, {}});
			types.push_back({"T*", {{"tT", 8u}}, 0u, 0x6u, 8u, 0u, 8u, 8u, 0x1bu, 64u, 0u, 0xd3201f50u, true, {}});
			types.push_back({"hkUint16", {}, 38u, 0x0u, 0u, 0u, 0u, 0u, 0x0u, 0u, 0u, 0xf9194383u, true, {}});
			types.push_back({"unsigned short", {}, 0u, 0x4004u, 0u, 0u, 2u, 2u, 0x9u, 0u, 0u, 0xba2556eau, true, {}});
			types.push_back({"hkArray", {{"tT", 40u}, {"tAllocator", 31u}}, 0u, 0x8u, 40u, 0u, 16u, 8u, 0x2bu, 0u, 3u, 0x34889aabu, true, {{"m_data", 34u, 0u, 60u}, {"m_size", 34u, 8u, 2u}, {"m_capacityAndFlags", 34u, 12u, 2u}}});
			types.push_back({"hknpMaterial", {}, 5u, 0x7u, 0u, 5u, 112u, 16u, 0x2du, 0u, 21u, 0xdce1b222u, true, {{"name", 32u, 32u, 41u}, {"isExclusive", 32u, 40u, 21u}, {"flags", 32u, 44u, 2u}, {"triggerType", 32u, 48u, 43u}, {"triggerManifoldTolerance", 32u, 49u, 46u}, {"dynamicFriction", 32u, 50u, 47u}, {"staticFriction", 32u, 52u, 47u}, {"restitution", 32u, 54u, 47u}, {"frictionCombinePolicy", 32u, 56u, 50u}, {"restitutionCombinePolicy", 32u, 57u, 50u}, {"weldingTolerance", 32u, 60u, 52u}, {"maxContactImpulse", 32u, 64u, 52u}, {"fractionOfClippedImpulseToApply", 32u, 68u, 52u}, {"massChangerCategory", 32u, 72u, 54u}, {"massChangerHeavyObjectFactor", 32u, 74u, 47u}, {"softContactForceFactor", 32u, 76u, 47u}, {"softContactDampFactor", 32u, 78u, 47u}, {"softContactSeparationVelocity", 32u, 80u, 46u}, {"surfaceVelocity", 32u, 88u, 56u}, {"disablingCollisionsBetweenCvxCvxDynamicObjectsDistance", 32u, 96u, 47u}, {"userData", 32u, 104u, 58u}}});
			types.push_back({"hkStringPtr", {}, 0u, 0x83u, 0u, 0u, 8u, 8u, 0x29u, 0u, 1u, 0xa1909ef9u, true, {{"stringAndFlag", 36u, 0u, 42u}}});
			types.push_back({"char*", {}, 0u, 0x3u, 0u, 0u, 8u, 8u, 0x9u, 0u, 0u, 0x396952fau, true, {}});
			types.push_back({"hkEnum", {{"tENUM", 44u}, {"tSTORAGE", 45u}}, 0u, 0x2004u, 0u, 0u, 1u, 1u, 0x29u, 0u, 1u, 0x1135fcd1u, true, {{"storage", 36u, 0u, 45u}}});
			types.push_back({"hknpMaterial::TriggerType", {}, 0u, 0x8204u, 0u, 0u, 4u, 4u, 0x9u, 0u, 0u, 0x36e85fb4u, true, {}});
			types.push_back({"hkUint8", {}, 26u, 0x0u, 0u, 0u, 0u, 0u, 0x0u, 0u, 0u, 0xddd4337bu, true, {}});
			types.push_back({"hkUFloat8", {}, 0u, 0x7u, 0u, 0u, 1u, 1u, 0x29u, 0u, 1u, 0x9bcb890bu, true, {{"value", 32u, 0u, 45u}}});
			types.push_back({"hkHalf16", {}, 0u, 0x74605u, 0u, 0u, 2u, 2u, 0x29u, 0u, 1u, 0xa7066325u, true, {{"value", 36u, 0u, 48u}}});
			types.push_back({"hkInt16", {}, 49u, 0x0u, 0u, 0u, 0u, 0u, 0x0u, 0u, 0u, 0x5cc5d8eau, true, {}});
			types.push_back({"short", {}, 0u, 0x4204u, 0u, 0u, 2u, 2u, 0x9u, 0u, 0u, 0x76cc3da5u, true, {}});
			types.push_back({"hkEnum", {{"tENUM", 51u}, {"tSTORAGE", 45u}}, 0u, 0x2004u, 0u, 0u, 1u, 1u, 0x29u, 0u, 1u, 0x39c9e755u, true, {{"storage", 36u, 0u, 45u}}});
			types.push_back({"hknpMaterial::CombinePolicy", {}, 0u, 0x8204u, 0u, 0u, 4u, 4u, 0x9u, 0u, 0u, 0x5fb6450eu, true, {}});
			types.push_back({"hkReal", {}, 53u, 0x0u, 0u, 0u, 0u, 0u, 0x0u, 0u, 0u, 0xe5cd30f2u, true, {}});
			types.push_back({"float", {}, 0u, 0x174605u, 0u, 0u, 4u, 4u, 0x9u, 0u, 0u, 0xbbff8fbcu, true, {}});
			types.push_back({"hkEnum", {{"tENUM", 55u}, {"tSTORAGE", 45u}}, 0u, 0x2004u, 0u, 0u, 1u, 1u, 0x29u, 0u, 1u, 0x013b7620u, true, {{"storage", 36u, 0u, 45u}}});
			types.push_back({"hknpMaterial::MassChangerCategory", {}, 0u, 0x8204u, 0u, 0u, 4u, 4u, 0x9u, 0u, 0u, 0x2c029b68u, true, {}});
			types.push_back({"T*", {{"tT", 57u}}, 0u, 0x6u, 57u, 0u, 8u, 8u, 0x1bu, 0u, 0u, 0x861bc9e5u, true, {}});
			types.push_back({"hknpSurfaceVelocity", {}, 5u, 0x7u, 0u, 0u, 24u, 8u, 0x39u, 3u, 0u, 0x00000000u, false, {}});
			types.push_back({"hkUint64", {}, 59u, 0x0u, 0u, 0u, 0u, 0u, 0x0u, 0u, 0u, 0x7236e109u, true, {}});
			types.push_back({"unsigned long long", {}, 0u, 0x10004u, 0u, 0u, 8u, 8u, 0x9u, 0u, 0u, 0x20760f81u, true, {}});
			types.push_back({"T*", {{"tT", 40u}}, 0u, 0x6u, 40u, 0u, 8u, 8u, 0x1bu, 0u, 0u, 0x74e0cb88u, true, {}});
			types.push_back({"hkArray", {{"tT", 62u}, {"tAllocator", 31u}}, 0u, 0x8u, 62u, 0u, 16u, 8u, 0x2bu, 0u, 3u, 0x861f63a9u, true, {{"m_data", 34u, 0u, 65u}, {"m_size", 34u, 8u, 2u}, {"m_capacityAndFlags", 34u, 12u, 2u}}});
			types.push_back({"hknpMotionProperties", {}, 0u, 0x7u, 0u, 5u, 112u, 16u, 0x2du, 0u, 12u, 0x7928832cu, true, {{"isExclusive", 32u, 0u, 21u}, {"flags", 32u, 4u, 22u}, {"gravityFactor", 32u, 8u, 52u}, {"timeFactor", 32u, 12u, 52u}, {"maxLinearSpeed", 32u, 16u, 52u}, {"maxAngularSpeed", 32u, 20u, 52u}, {"linearDamping", 32u, 24u, 52u}, {"angularDamping", 32u, 28u, 52u}, {"solverStabilizationSpeedThreshold", 32u, 32u, 52u}, {"solverStabilizationSpeedReduction", 32u, 36u, 52u}, {"deactivationSettings", 32u, 40u, 63u}, {"fullCastSettings", 32u, 64u, 64u}}});
			types.push_back({"hknpMotionProperties::DeactivationSettings", {}, 0u, 0x7u, 0u, 0u, 24u, 4u, 0x29u, 0u, 10u, 0x0791fcb1u, true, {{"maxDistSqrd", 32u, 0u, 52u}, {"maxRotSqrd", 32u, 4u, 52u}, {"invBlockSize", 32u, 8u, 52u}, {"pathingUpperThreshold", 32u, 12u, 48u}, {"pathingLowerThreshold", 32u, 14u, 48u}, {"numDeactivationFrequencyPasses", 32u, 16u, 45u}, {"deactivationVelocityScaleSquare", 32u, 17u, 45u}, {"minimumPathingVelocityScaleSquare", 32u, 18u, 45u}, {"spikingVelocityScaleThresholdSquared", 32u, 19u, 45u}, {"minimumSpikingVelocityScaleSquared", 32u, 20u, 45u}}});
			types.push_back({"hknpMotionProperties::FullCastSettings", {}, 0u, 0x7u, 0u, 0u, 40u, 4u, 0x29u, 0u, 10u, 0x0e3cdefcu, true, {{"minSeparation", 32u, 0u, 52u}, {"minExtraSeparation", 32u, 4u, 52u}, {"toiSeparation", 32u, 8u, 52u}, {"toiExtraSeparation", 32u, 12u, 52u}, {"toiAccuracy", 32u, 16u, 52u}, {"relativeSafeDeltaTime", 32u, 20u, 52u}, {"absoluteSafeDeltaTime", 32u, 24u, 52u}, {"keepTime", 32u, 28u, 52u}, {"keepDistance", 32u, 32u, 52u}, {"maxIterations", 32u, 36u, 2u}}});
			types.push_back({"T*", {{"tT", 62u}}, 0u, 0x6u, 62u, 0u, 8u, 8u, 0x1bu, 64u, 0u, 0x481f849eu, true, {}});
			types.push_back({"hkArray", {{"tT", 67u}, {"tAllocator", 31u}}, 0u, 0x8u, 67u, 0u, 16u, 8u, 0x2bu, 0u, 3u, 0x7cdd0523u, true, {{"m_data", 34u, 0u, 107u}, {"m_size", 34u, 8u, 2u}, {"m_capacityAndFlags", 34u, 12u, 2u}}});
			types.push_back({"hknpPhysicsSystemData::bodyCinfoWithAttachment", {}, 68u, 0x7u, 0u, 0u, 192u, 16u, 0x29u, 0u, 1u, 0xb5de5b8du, true, {{"attachedBody", 32u, 176u, 2u}}});
			types.push_back({"hknpBodyCinfo", {}, 0u, 0x7u, 0u, 8u, 176u, 16u, 0x2du, 0u, 22u, 0xf45dd92du, true, {{"shape", 32u, 0u, 69u}, {"flags", 32u, 8u, 2u}, {"collisionCntrl", 32u, 12u, 49u}, {"collisionFilterInfo", 32u, 16u, 21u}, {"materialId", 32u, 20u, 38u}, {"qualityId", 32u, 22u, 26u}, {"name", 32u, 24u, 41u}, {"userData", 32u, 32u, 58u}, {"motionType", 32u, 40u, 26u}, {"position", 32u, 48u, 86u}, {"orientation", 32u, 64u, 88u}, {"linearVelocity", 32u, 80u, 86u}, {"angularVelocity", 32u, 96u, 86u}, {"mass", 32u, 112u, 52u}, {"massDistribution", 32u, 120u, 90u}, {"dragProperties", 32u, 128u, 94u}, {"motionPropertiesId", 32u, 136u, 38u}, {"desiredBodyId", 32u, 140u, 101u}, {"motionId", 32u, 144u, 22u}, {"collisionLookAheadDistance", 32u, 148u, 52u}, {"localFrame", 32u, 152u, 102u}, {"activationPriority", 32u, 160u, 105u}}});
			types.push_back({"hkRefPtr", {{"tTYPE", 70u}}, 0u, 0x6u, 70u, 0u, 8u, 8u, 0x2bu, 0u, 1u, 0x5fb1f945u, true, {{"ptr", 36u, 0u, 85u}}});
			types.push_back({"hknpShape", {}, 5u, 0x7u, 0u, 4u, 56u, 8u, 0x2du, 0u, 7u, 0x9ca3231bu, true, {{"flags", 34u, 24u, 71u}, {"type", 34u, 26u, 73u}, {"numShapeKeyBits", 34u, 27u, 45u}, {"dispatchType", 32u, 28u, 75u}, {"convexRadius", 32u, 32u, 52u}, {"userData", 32u, 40u, 58u}, {"properties", 34u, 48u, 77u}}});
			types.push_back({"hkFlags", {{"tBITS", 72u}, {"tSTORAGE", 37u}}, 0u, 0x4004u, 0u, 0u, 2u, 2u, 0x29u, 0u, 1u, 0x08291487u, true, {{"storage", 34u, 0u, 37u}}});
			types.push_back({"hknpShape::FlagsEnum", {}, 0u, 0x8204u, 0u, 0u, 4u, 4u, 0x9u, 0u, 0u, 0xca5d26feu, true, {}});
			types.push_back({"hkEnum", {{"tENUM", 74u}, {"tSTORAGE", 45u}}, 0u, 0x2004u, 0u, 0u, 1u, 1u, 0x29u, 0u, 1u, 0x4cc43812u, true, {{"storage", 36u, 0u, 45u}}});
			types.push_back({"hknpShapeType::Enum", {}, 0u, 0x8204u, 0u, 0u, 4u, 4u, 0x9u, 0u, 0u, 0x78805d2bu, true, {}});
			types.push_back({"hkEnum", {{"tENUM", 76u}, {"tSTORAGE", 45u}}, 0u, 0x2004u, 0u, 0u, 1u, 1u, 0x29u, 0u, 1u, 0xb846a164u, true, {{"storage", 36u, 0u, 45u}}});
			types.push_back({"hknpCollisionDispatchType::Enum", {}, 0u, 0x8204u, 0u, 0u, 4u, 4u, 0x9u, 0u, 0u, 0x2c9c2ac6u, true, {}});
			types.push_back({"hkRefPtr", {{"tTYPE", 78u}}, 0u, 0x6u, 78u, 0u, 8u, 8u, 0x2bu, 0u, 1u, 0x2f88000fu, true, {{"ptr", 36u, 0u, 84u}}});
			types.push_back({"hkRefCountedProperties", {}, 5u, 0x7u, 0u, 2u, 40u, 8u, 0x2du, 0u, 1u, 0x9d3e5900u, true, {{"entries", 34u, 24u, 79u}}});
			types.push_back({"hkArray", {{"tT", 80u}, {"tAllocator", 31u}}, 0u, 0x8u, 80u, 0u, 16u, 8u, 0x2bu, 0u, 3u, 0xda43f5ffu, true, {{"m_data", 34u, 0u, 83u}, {"m_size", 34u, 8u, 2u}, {"m_capacityAndFlags", 34u, 12u, 2u}}});
			types.push_back({"hkRefCountedProperties::Entry", {}, 0u, 0x7u, 0u, 0u, 16u, 8u, 0x29u, 0u, 3u, 0xf31715cau, true, {{"object", 32u, 0u, 81u}, {"key", 32u, 8u, 37u}, {"flags", 32u, 10u, 37u}}});
			types.push_back({"hkRefPtr", {{"tTYPE", 5u}}, 0u, 0x6u, 5u, 0u, 8u, 8u, 0x2bu, 0u, 1u, 0x5a986a75u, true, {{"ptr", 36u, 0u, 82u}}});
			types.push_back({"T*", {{"tT", 5u}}, 0u, 0x6u, 5u, 0u, 8u, 8u, 0x1bu, 0u, 0u, 0x7a2035d8u, true, {}});
			types.push_back({"T*", {{"tT", 80u}}, 0u, 0x6u, 80u, 0u, 8u, 8u, 0x1bu, 64u, 0u, 0x76e17e8fu, true, {}});
			types.push_back({"T*", {{"tT", 78u}}, 0u, 0x6u, 78u, 0u, 8u, 8u, 0x1bu, 0u, 0u, 0x3adfd99cu, true, {}});
			types.push_back({"T*", {{"tT", 70u}}, 0u, 0x6u, 70u, 0u, 8u, 8u, 0x1bu, 0u, 0u, 0x00897122u, true, {}});
			types.push_back({"hkVector4", {}, 87u, 0x0u, 0u, 0u, 0u, 0u, 0x0u, 0u, 0u, 0xc2b7e409u, true, {}});
			types.push_back({"hkVector4f", {}, 0u, 0x428u, 53u, 0u, 16u, 16u, 0xbu, 0u, 0u, 0xb54aa116u, true, {}});
			types.push_back({"hkQuaternion", {}, 89u, 0x0u, 0u, 0u, 0u, 0u, 0x0u, 0u, 0u, 0x9803b3deu, true, {}});
			types.push_back({"hkQuaternionf", {}, 0u, 0x428u, 53u, 0u, 16u, 16u, 0x2bu, 0u, 1u, 0x532f3f83u, true, {{"vec", 32u, 0u, 87u}}});
			types.push_back({"hkRefPtr", {{"tTYPE", 91u}}, 0u, 0x6u, 91u, 0u, 8u, 8u, 0x2bu, 0u, 1u, 0x8994d182u, true, {{"ptr", 36u, 0u, 93u}}});
			types.push_back({"hknpRefMassDistribution", {}, 5u, 0x7u, 0u, 0u, 80u, 16u, 0x29u, 0u, 1u, 0xddd014b7u, true, {{"massDistribution", 32u, 32u, 92u}}});
			types.push_back({"hknpMassDistribution", {}, 0u, 0x7u, 0u, 0u, 48u, 16u, 0x29u, 0u, 3u, 0xd1661c38u, true, {{"centerOfMassAndVolume", 32u, 0u, 86u}, {"majorAxisSpace", 32u, 16u, 88u}, {"inertiaTensor", 32u, 32u, 86u}}});
			types.push_back({"T*", {{"tT", 91u}}, 0u, 0x6u, 91u, 0u, 8u, 8u, 0x1bu, 0u, 0u, 0x9a5f54a5u, true, {}});
			types.push_back({"hkRefPtr", {{"tTYPE", 95u}}, 0u, 0x6u, 95u, 0u, 8u, 8u, 0x2bu, 0u, 1u, 0x649f65eeu, true, {{"ptr", 36u, 0u, 100u}}});
			types.push_back({"hknpRefDragProperties", {}, 5u, 0x7u, 0u, 0u, 224u, 16u, 0x29u, 0u, 1u, 0x57292ac5u, true, {{"dragProperties", 32u, 32u, 96u}}});
			types.push_back({"hknpDragProperties", {}, 0u, 0x7u, 0u, 0u, 192u, 16u, 0x29u, 0u, 3u, 0x041332abu, true, {{"centerAndOffset", 32u, 0u, 97u}, {"angularEffectsAndArea", 32u, 48u, 98u}, {"armUVs", 32u, 144u, 99u}}});
			types.push_back({"T[N]", {{"tT", 86u}, {"vN", 3u}}, 0u, 0x328u, 86u, 0u, 48u, 16u, 0xbu, 0u, 0u, 0x95a6065au, true, {}});
			types.push_back({"T[N]", {{"tT", 86u}, {"vN", 6u}}, 0u, 0x628u, 86u, 0u, 96u, 16u, 0xbu, 0u, 0u, 0x11290ef5u, true, {}});
			types.push_back({"T[N]", {{"tT", 52u}, {"vN", 12u}}, 0u, 0xc28u, 52u, 0u, 48u, 4u, 0xbu, 0u, 0u, 0x2fc92d91u, true, {}});
			types.push_back({"T*", {{"tT", 95u}}, 0u, 0x6u, 95u, 0u, 8u, 8u, 0x1bu, 0u, 0u, 0x77ea4faeu, true, {}});
			types.push_back({"hknpBodyId", {}, 0u, 0x7u, 0u, 0u, 4u, 4u, 0x29u, 0u, 1u, 0x96199f12u, true, {{"serialAndIndex", 32u, 0u, 21u}}});
			types.push_back({"hkRefPtr", {{"tTYPE", 103u}}, 0u, 0x6u, 103u, 0u, 8u, 8u, 0x2bu, 0u, 1u, 0x2e5185a0u, true, {{"ptr", 36u, 0u, 104u}}});
			types.push_back({"hkLocalFrame", {}, 5u, 0x7u, 0u, 0u, 24u, 8u, 0x39u, 3u, 0u, 0x00000000u, false, {}});
			types.push_back({"T*", {{"tT", 103u}}, 0u, 0x6u, 103u, 0u, 8u, 8u, 0x1bu, 0u, 0u, 0x9d1f8db4u, true, {}});
			types.push_back({"hkInt8", {}, 106u, 0x0u, 0u, 0u, 0u, 0u, 0x0u, 0u, 0u, 0xd38221d2u, true, {}});
			types.push_back({"signed char", {}, 0u, 0x2204u, 0u, 0u, 1u, 1u, 0x9u, 0u, 0u, 0x90e534efu, true, {}});
			types.push_back({"T*", {{"tT", 67u}}, 0u, 0x6u, 67u, 0u, 8u, 8u, 0x1bu, 64u, 0u, 0x7798f00cu, true, {}});
			types.push_back({"hkArray", {{"tT", 109u}, {"tAllocator", 31u}}, 0u, 0x8u, 109u, 0u, 16u, 8u, 0x2bu, 0u, 3u, 0x82d95e37u, true, {{"m_data", 34u, 0u, 119u}, {"m_size", 34u, 8u, 2u}, {"m_capacityAndFlags", 34u, 12u, 2u}}});
			types.push_back({"hknpConstraintCinfo", {}, 0u, 0x7u, 0u, 6u, 40u, 8u, 0x2du, 0u, 7u, 0xa6c5a999u, true, {{"constraintData", 32u, 0u, 110u}, {"bodyA", 32u, 8u, 101u}, {"bodyB", 32u, 12u, 101u}, {"flags", 32u, 16u, 114u}, {"name", 32u, 24u, 41u}, {"desiredConstraintId", 32u, 32u, 116u}, {"constraintGroupId", 32u, 36u, 118u}}});
			types.push_back({"hkRefPtr", {{"tTYPE", 111u}}, 0u, 0x6u, 111u, 0u, 8u, 8u, 0x2bu, 0u, 1u, 0x2ac3a564u, true, {{"ptr", 36u, 0u, 113u}}});
			types.push_back({"hkpConstraintData", {}, 5u, 0x7u, 0u, 0u, 32u, 8u, 0x39u, 3u, 1u, 0x00000000u, false, {{"userData", 32u, 24u, 112u}}});
			types.push_back({"hkUlong", {}, 59u, 0x0u, 0u, 0u, 0u, 0u, 0x0u, 0u, 0u, 0xa4b1fd68u, true, {}});
			types.push_back({"T*", {{"tT", 111u}}, 0u, 0x6u, 111u, 0u, 8u, 8u, 0x1bu, 0u, 0u, 0x9a4ea10au, true, {}});
			types.push_back({"hkFlags", {{"tBITS", 115u}, {"tSTORAGE", 37u}}, 0u, 0x4004u, 0u, 0u, 2u, 2u, 0x29u, 0u, 1u, 0xeaba7a27u, true, {{"storage", 34u, 0u, 37u}}});
			types.push_back({"hknpConstraint::FlagsEnum", {}, 0u, 0x8204u, 0u, 0u, 4u, 4u, 0x9u, 0u, 0u, 0x621c8e47u, true, {}});
			types.push_back({"hknpConstraintId", {}, 117u, 0x0u, 0u, 1u, 0u, 0u, 0x4u, 0u, 0u, 0x1bb73430u, true, {}});
			types.push_back({"hkHandle", {{"tTYPE", 21u}, {"vINVALID_VALUE", 2147483647u}}, 0u, 0x7u, 0u, 0u, 4u, 4u, 0x29u, 0u, 1u, 0xc76e8b15u, true, {{"value", 34u, 0u, 21u}}});
			types.push_back({"hknpConstraintGroupId", {}, 117u, 0x0u, 0u, 1u, 0u, 0u, 0x4u, 0u, 0u, 0x698b216eu, true, {}});
			types.push_back({"T*", {{"tT", 109u}}, 0u, 0x6u, 109u, 0u, 8u, 8u, 0x1bu, 64u, 0u, 0xe8af6e77u, true, {}});
			types.push_back({"hkArray", {{"tT", 2u}, {"tAllocator", 31u}}, 0u, 0x8u, 2u, 0u, 16u, 8u, 0x2bu, 0u, 3u, 0x36440be9u, true, {{"m_data", 34u, 0u, 121u}, {"m_size", 34u, 8u, 2u}, {"m_capacityAndFlags", 34u, 12u, 2u}}});
			types.push_back({"T*", {{"tT", 2u}}, 0u, 0x6u, 2u, 0u, 8u, 8u, 0x1bu, 64u, 0u, 0xb8b4e789u, true, {}});
			types.push_back({"hkArray", {{"tT", 41u}, {"tAllocator", 31u}}, 0u, 0x8u, 41u, 0u, 16u, 8u, 0x2bu, 0u, 3u, 0x102a4d0du, true, {{"m_data", 34u, 0u, 123u}, {"m_size", 34u, 8u, 2u}, {"m_capacityAndFlags", 34u, 12u, 2u}}});
			types.push_back({"T*", {{"tT", 41u}}, 0u, 0x6u, 41u, 0u, 8u, 8u, 0x1bu, 64u, 0u, 0xd18f37e3u, true, {}});
			types.push_back({"hkArray", {{"tT", 35u}, {"tAllocator", 31u}}, 0u, 0x8u, 35u, 0u, 16u, 8u, 0x2bu, 0u, 3u, 0x084292dcu, true, {{"m_data", 34u, 0u, 125u}, {"m_size", 34u, 8u, 2u}, {"m_capacityAndFlags", 34u, 12u, 2u}}});
			types.push_back({"T*", {{"tT", 35u}}, 0u, 0x6u, 35u, 0u, 8u, 8u, 0x1bu, 64u, 0u, 0xe7bd665cu, true, {}});
			types.push_back({"hkHashMap", {{"tKEY", 112u}, {"tVALUE", 2u}}, 127u, 0x7u, 0u, 0u, 32u, 8u, 0x39u, 16u, 0u, 0xebfa8ed3u, true, {}});
			types.push_back({"hkHashBase", {{"tITEM", 128u}}, 0u, 0x7u, 0u, 0u, 32u, 8u, 0x29u, 0u, 2u, 0xbca71d51u, true, {{"items", 34u, 0u, 130u}, {"index", 35u, 16u, 33u}}});
			types.push_back({"hkHashMapDetail::MapTuple", {{"tKEY", 112u}, {"tVALUE", 2u}}, 129u, 0x7u, 0u, 0u, 16u, 8u, 0x39u, 16u, 0u, 0x365be0d3u, true, {}});
			types.push_back({"hkTuple", {{"tT0", 112u}, {"tT1", 2u}, {"tT2", 29u}, {"tT3", 29u}, {"tT4", 29u}, {"tT5", 29u}, {"tT6", 29u}, {"tT7", 29u}}, 0u, 0x7u, 0u, 0u, 16u, 8u, 0x29u, 0u, 2u, 0x02d2acd5u, true, {{"0", 32u, 0u, 112u}, {"1", 32u, 8u, 2u}}});
			types.push_back({"hkArray", {{"tT", 128u}, {"tAllocator", 31u}}, 0u, 0x8u, 128u, 0u, 16u, 8u, 0x2bu, 0u, 3u, 0xbd4732bcu, true, {{"m_data", 34u, 0u, 131u}, {"m_size", 34u, 8u, 2u}, {"m_capacityAndFlags", 34u, 12u, 2u}}});
			types.push_back({"T*", {{"tT", 128u}}, 0u, 0x6u, 128u, 0u, 8u, 8u, 0x1bu, 64u, 0u, 0x3c2d1828u, true, {}});
			types.push_back({"hknpConvexPolytopeShape", {}, 133u, 0x7u, 0u, 4u, 88u, 8u, 0x2du, 0u, 4u, 0xf1bd170du, true, {{"planes", 34u, 64u, 135u}, {"faces", 34u, 68u, 136u}, {"indices", 34u, 72u, 138u}, {"connectivity", 34u, 80u, 139u}}});
			types.push_back({"hknpConvexShape", {}, 70u, 0x7u, 0u, 2u, 64u, 8u, 0x2du, 0u, 2u, 0xba9dedb5u, true, {{"maxAllowedPenetration", 34u, 56u, 47u}, {"vertices", 34u, 58u, 134u}}});
			types.push_back({"hkRelArray", {{"tT", 87u}}, 0u, 0x8u, 87u, 0u, 4u, 2u, 0x2bu, 0u, 2u, 0x2b264726u, true, {{"size", 34u, 0u, 37u}, {"offset", 34u, 2u, 37u}}});
			types.push_back({"hkRelArray", {{"tT", 86u}}, 0u, 0x8u, 86u, 0u, 4u, 2u, 0x2bu, 0u, 2u, 0x0f6842f4u, true, {{"size", 34u, 0u, 37u}, {"offset", 34u, 2u, 37u}}});
			types.push_back({"hkRelArray", {{"tT", 137u}}, 0u, 0x8u, 137u, 0u, 4u, 2u, 0x2bu, 0u, 2u, 0x025fb763u, true, {{"size", 34u, 0u, 37u}, {"offset", 34u, 2u, 37u}}});
			types.push_back({"hknpConvexPolytopeShape::Face", {}, 0u, 0x7u, 0u, 0u, 4u, 2u, 0x29u, 0u, 3u, 0xeaad8c84u, true, {{"firstIndex", 32u, 0u, 37u}, {"numIndices", 32u, 2u, 45u}, {"minHalfAngle", 32u, 3u, 45u}}});
			types.push_back({"hkRelArray", {{"tT", 45u}}, 0u, 0x8u, 45u, 0u, 4u, 2u, 0x2bu, 0u, 2u, 0x452b9f92u, true, {{"size", 34u, 0u, 37u}, {"offset", 34u, 2u, 37u}}});
			types.push_back({"hkRefPtr", {{"tTYPE", 140u}}, 0u, 0x6u, 140u, 0u, 8u, 8u, 0x2bu, 0u, 1u, 0x21a464bdu, true, {{"ptr", 36u, 0u, 145u}}});
			types.push_back({"hknpConvexPolytopeShape::Connectivity", {}, 5u, 0x7u, 0u, 0u, 56u, 8u, 0x29u, 0u, 2u, 0xa12ddce7u, true, {{"vertexEdges", 32u, 24u, 141u}, {"faceLinks", 32u, 40u, 141u}}});
			types.push_back({"hkArray", {{"tT", 142u}, {"tAllocator", 31u}}, 0u, 0x8u, 142u, 0u, 16u, 8u, 0x2bu, 0u, 3u, 0x9b6a2731u, true, {{"m_data", 34u, 0u, 144u}, {"m_size", 34u, 8u, 2u}, {"m_capacityAndFlags", 34u, 12u, 2u}}});
			types.push_back({"hknpConvexPolytopeShape::Connectivity::Edge", {}, 0u, 0x7u, 0u, 0u, 4u, 2u, 0x29u, 0u, 3u, 0xa816b6f7u, true, {{"faceIndex", 32u, 0u, 37u}, {"edgeIndex", 32u, 2u, 45u}, {"padding", 33u, 3u, 143u}}});
			types.push_back({"T[N]", {{"tT", 45u}, {"vN", 1u}}, 0u, 0x128u, 45u, 0u, 1u, 1u, 0xbu, 0u, 0u, 0x00000000u, false, {}});
			types.push_back({"T*", {{"tT", 142u}}, 0u, 0x6u, 142u, 0u, 8u, 8u, 0x1bu, 64u, 0u, 0xce345badu, true, {}});
			types.push_back({"T*", {{"tT", 140u}}, 0u, 0x6u, 140u, 0u, 8u, 8u, 0x1bu, 0u, 0u, 0xb32b6866u, true, {}});
			types.push_back({"hknpShapeMassProperties", {}, 5u, 0x7u, 0u, 0u, 56u, 8u, 0x29u, 0u, 1u, 0xb770ab38u, true, {{"compressedMassProperties", 32u, 24u, 147u}}});
			types.push_back({"hkCompressedMassProperties", {}, 0u, 0x7u, 0u, 1u, 32u, 12296u, 0x2du, 0u, 5u, 0x5f47c2fcu, true, {{"centerOfMass", 32u, 0u, 148u}, {"inertia", 32u, 8u, 148u}, {"majorAxisSpace", 32u, 16u, 150u}, {"mass", 32u, 24u, 52u}, {"volume", 32u, 28u, 52u}}});
			types.push_back({"hkPackedVector3", {}, 0u, 0x7u, 0u, 0u, 8u, 8u, 0x29u, 0u, 1u, 0x75632ecfu, true, {{"values", 32u, 0u, 149u}}});
			types.push_back({"T[N]", {{"tT", 48u}, {"vN", 4u}}, 0u, 0x428u, 48u, 0u, 8u, 2u, 0xbu, 0u, 0u, 0x26d52268u, true, {}});
			types.push_back({"T[N]", {{"tT", 49u}, {"vN", 4u}}, 0u, 0x428u, 49u, 0u, 8u, 2u, 0xbu, 0u, 0u, 0x5288884cu, true, {}});
			types.push_back({"char", {}, 0u, 0x2004u, 0u, 0u, 1u, 1u, 0x9u, 0u, 0u, 0xf96fee69u, true, {}});
			return types;
		}
	}

	const std::vector<::havok::tagfile::type_info>& schema()
	{
		static const auto types = make_schema();
		return types;
	}

	const std::vector<::havok::tagfile::type_info>& physics_asset_schema()
	{
		static const auto types = make_physics_asset_schema();
		return types;
	}

	const std::vector<std::uint32_t>& physics_asset_compendium_types()
	{
		static const std::vector<std::uint32_t> types =
		{
			0u, 7u, 5u, 8u, 13u, 21u, 32u, 33u, 47u, 87u, 99u, 108u, 121u, 100u, 110u, 123u,
			2u, 6u, 127u, 128u, 131u, 50u, 59u, 124u, 101u, 111u, 43u, 112u, 125u, 68u, 109u, 3u,
			122u, 30u, 46u, 17u, 48u, 34u, 49u, 22u, 35u, 15u, 27u, 51u, 69u, 26u, 52u, 53u,
			70u, 76u, 54u, 71u, 55u, 72u, 56u, 73u, 57u, 74u, 58u, 31u, 36u, 23u, 37u, 60u,
			61u, 38u, 24u, 39u, 62u, 75u, 88u, 102u, 113u, 103u, 114u, 104u, 115u, 105u, 116u, 126u,
			129u, 132u, 133u, 130u, 117u, 89u, 77u, 90u, 78u, 91u, 79u, 92u, 106u, 93u, 80u, 94u,
			107u, 118u, 119u, 120u, 95u, 64u, 81u, 96u, 97u, 82u, 98u, 40u, 25u, 41u, 63u, 83u,
			20u, 84u, 65u, 85u, 66u, 86u, 67u, 42u, 9u, 14u, 10u, 16u, 11u, 18u, 12u, 19u,
			28u, 44u, 29u, 45u, 1448u, 210u, 211u, 1303u, 1304u, 1305u, 1302u, 1265u, 610u, 611u, 612u, 614u,
			613u, 609u, 1574u, 1055u, 1056u, 741u, 789u, 163u
		};
		return types;
	}
}
