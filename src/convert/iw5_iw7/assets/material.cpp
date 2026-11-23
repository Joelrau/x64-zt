#include <std_include.hpp>

#include "../include.hpp"
#include "material.hpp"

#include <unordered_set>

namespace zonetool::iw5::converter::iw7
{
	namespace material
	{
		namespace
		{
			namespace iw7_types = zonetool::iw7;

		enum techset_map_type_e
		{
			regular,
			color_tint,
			packed,
			packed_alpha,
			count,
		};

		struct techset_map_s
		{
			std::string techset[techset_map_type_e::count];
		};

		techset_map_s make_techset_map(const std::string& regular, const std::string& color_tint = {},
			const std::string& packed = {}, const std::string& packed_alpha = {})
		{
			techset_map_s map;
			map.techset[techset_map_type_e::regular] = regular;
			map.techset[techset_map_type_e::color_tint] = color_tint;
			map.techset[techset_map_type_e::packed] = packed;
			map.techset[techset_map_type_e::packed_alpha] = packed_alpha;
			return map;
		}

		std::unordered_map<std::string, techset_map_s> mapped_techsets =
		{
			{"mc_l_b0c0",                   {"mo_l_sm_ndw_blend_i0c0s0n0"}},
			{"mc_l_b0c0d0",                 {"mo_l_sm_ndw_blend_i0c0s0n0"}},
			{"mc_l_b0c0d0n0",               {"mo_l_sm_ndw_blend_i0c0s0n0"}},
			{"mc_l_b0c0d0n0s0",             {"mo_l_sm_ndw_blend_i0c0s0n0", "", "", "mo_l_sm_ndw_blend_i0c0s0n0pa0"}},
			{"mc_l_b0c0d0s0",               {"mo_l_sm_ndw_blend_i0c0s0"}},
			{"mc_l_b0c0n0",                 {"mo_l_sm_ndw_blend_i0c0s0n0"}},
			{"mc_l_b0c0n0s0",               {"mo_l_sm_ndw_blend_i0c0s0n0", "", "", "mo_l_sm_ndw_blend_i0c0s0n0pa0"}},
			{"mc_l_b0c0s0",                 {"mo_l_sm_ndw_blend_i0c0s0"}},
			{"mc_l_flag_t0c0",              {"mo_l_sm_atest_i0c0s0n0"}},
			{"mc_l_flag_t0c0n0",            {"mo_l_sm_atest_i0c0s0n0"}},
			{"mc_l_flag_t0c0n0s0",          {"mo_l_sm_atest_i0c0s0n0", "", "", "mo_l_sm_atest_i0c0s0n0pa0"}},
			{"mc_l_flag_t0c0s0",            {"mo_l_sm_atest_i0c0s0"}},
			{"mc_l_r0c0",                   {"mo_l_sm_replace_i0c0s0n0"}},
			{"mc_l_r0c0d0",                 {"mo_l_sm_replace_i0c0s0n0"}},
			{"mc_l_r0c0d0n0",               {"mo_l_sm_replace_i0c0n0d0"}},
			{"mc_l_r0c0d0n0s0",             {"mo_l_sm_replace_i0c0s0n0d0", "", "mo_l_sm_replace_i0c0s0n0d0p0"}},
			{"mc_l_r0c0d0s0",               {"mo_l_sm_replace_i0c0s0"}},
			{"mc_l_r0c0n0",                 {"mo_l_sm_replace_i0c0s0n0"}},
			{"mc_l_r0c0n0s0",               {"mo_l_sm_replace_i0c0s0n0", "", "mo_l_sm_replace_i0c0s0n0p0"}},
			{"mc_l_r0c0s0",                 {"mo_l_sm_replace_i0c0s0"}},
			{"mc_l_scroll_b0c0",            {"mo_l_sm_ndw_blend_i0c0s0n0"}},
			{"mc_l_scroll_b0c0d0",          {"mo_l_sm_ndw_blend_i0c0s0n0"}},
			{"mc_l_scroll_b0c0d0n0",        {"mo_l_sm_ndw_blend_i0c0s0n0"}},
			{"mc_l_scroll_b0c0d0n0s0",      {"mo_l_sm_ndw_blend_i0c0s0n0", "", "", "mo_l_sm_ndw_blend_i0c0s0n0pa0"}},
			{"mc_l_scroll_b0c0d0s0",        {"mo_l_sm_ndw_blend_i0c0s0"}},
			{"mc_l_scroll_b0c0n0",          {"mo_l_sm_ndw_blend_i0c0s0n0"}},
			{"mc_l_scroll_b0c0n0s0",        {"mo_l_sm_ndw_blend_i0c0s0n0", "", "", "mo_l_sm_ndw_blend_i0c0s0n0pa0"}},
			{"mc_l_scroll_b0c0s0",          {"mo_l_sm_ndw_blend_i0c0s0"}},
			{"mc_l_scroll_r0c0",            {"mo_l_sm_replace_i0c0s0n0"}},
			{"mc_l_scroll_r0c0d0",          {"mo_l_sm_replace_i0c0s0n0"}},
			{"mc_l_scroll_r0c0d0n0",        {"mo_l_sm_replace_i0c0n0d0"}},
			{"mc_l_scroll_r0c0d0n0s0",      {"mo_l_sm_replace_i0c0s0n0d0", "", "mo_l_sm_replace_i0c0s0n0d0p0"}},
			{"mc_l_scroll_r0c0d0s0",        {"mo_l_sm_replace_i0c0s0"}},
			{"mc_l_scroll_r0c0n0",          {"mo_l_sm_replace_i0c0s0n0"}},
			{"mc_l_scroll_r0c0n0s0",        {"mo_l_sm_replace_i0c0s0n0", "", "mo_l_sm_replace_i0c0s0n0p0"}},
			{"mc_l_scroll_r0c0s0",          {"mo_l_sm_replace_i0c0s0"}},
			{"mc_l_scroll_t0c0",            {"mo_l_sm_atest_i0c0s0n0"}},
			{"mc_l_scroll_t0c0d0",          {"mo_l_sm_atest_i0c0s0n0"}},
			{"mc_l_scroll_t0c0d0n0",        {"mo_l_sm_atest_i0c0s0n0"}},
			{"mc_l_scroll_t0c0d0n0s0",      {"mo_l_sm_atest_i0c0s0n0", "", "", "mo_l_sm_atest_i0c0s0n0pa0"}},
			{"mc_l_scroll_t0c0d0s0",        {"mo_l_sm_atest_i0c0s0"}},
			{"mc_l_scroll_t0c0n0",          {"mo_l_sm_atest_i0c0s0n0"}},
			{"mc_l_scroll_t0c0n0s0",        {"mo_l_sm_atest_i0c0s0n0", "", "", "mo_l_sm_atest_i0c0s0n0pa0"}},
			{"mc_l_scroll_t0c0s0",          {"mo_l_sm_atest_i0c0s0"}},
			{"mc_l_skin_r0c0",              {"mo_l_sm_replace_i0c0s0n0"}},
			{"mc_l_skin_r0c0d0",            {"mo_l_sm_replace_i0c0s0n0"}},
			{"mc_l_skin_r0c0d0n0",          {"mo_l_sm_replace_i0c0n0d0"}},
			{"mc_l_skin_r0c0d0n0s0",        {"mo_l_sm_replace_i0c0s0n0d0", "", "mo_l_sm_replace_i0c0s0n0d0p0"}},
			{"mc_l_skin_r0c0d0s0",          {"mo_l_sm_replace_i0c0s0"}},
			{"mc_l_skin_r0c0n0",            {"mo_l_sm_replace_i0c0s0n0"}},
			{"mc_l_skin_r0c0n0s0",          {"mo_l_sm_replace_i0c0s0n0", "", "mo_l_sm_replace_i0c0s0n0p0"}},
			{"mc_l_skin_r0c0s0",            {"mo_l_sm_replace_i0c0s0"}},
			{"mc_l_sm_b0c0",                {"mo_l_sm_ndw_blend_i0c0s0n0"}},
			{"mc_l_sm_b0c0d0",              {"mo_l_sm_ndw_blend_i0c0s0n0"}},
			{"mc_l_sm_b0c0d0n0",            {"mo_l_sm_ndw_blend_i0c0s0n0"}},
			{"mc_l_sm_b0c0d0n0s0",          {"mo_l_sm_ndw_blend_i0c0s0n0", "", "", "mo_l_sm_ndw_blend_i0c0s0n0pa0"}},
			{"mc_l_sm_b0c0d0s0",            {"mo_l_sm_ndw_blend_i0c0s0"}},
			{"mc_l_sm_b0c0n0",              {"mo_l_sm_ndw_blend_i0c0s0n0"}},
			{"mc_l_sm_b0c0n0s0",            {"mo_l_sm_ndw_blend_i0c0s0n0", "", "", "mo_l_sm_ndw_blend_i0c0s0n0pa0"}},
			{"mc_l_sm_b0c0n0s0p0",          {"mo_l_sm_ndw_blend_i0c0s0n0", "", "", "mo_l_sm_ndw_blend_i0c0s0n0pa0"}},
			{"mc_l_sm_b0c0p0",              {"mo_l_sm_ndw_blend_i0c0s0n0"}},
			{"mc_l_sm_b0c0s0",              {"mo_l_sm_ndw_blend_i0c0s0"}},
			{"mc_l_sm_flag_t0c0",           {"mo_l_sm_atest_i0c0s0n0"}},
			{"mc_l_sm_flag_t0c0n0",         {"mo_l_sm_atest_i0c0s0n0"}},
			{"mc_l_sm_flag_t0c0n0s0",       {"mo_l_sm_atest_i0c0s0n0", "", "", "mo_l_sm_atest_i0c0s0n0pa0"}},
			{"mc_l_sm_flag_t0c0s0",         {"mo_l_sm_atest_i0c0s0"}},
			{"mc_l_sm_r0c0",                {"mo_l_sm_replace_i0c0s0n0"}},
			{"mc_l_sm_r0c0d0",              {"mo_l_sm_replace_i0c0s0n0"}},
			{"mc_l_sm_r0c0d0n0",            {"mo_l_sm_replace_i0c0n0d0"}},
			{"mc_l_sm_r0c0d0n0s0",          {"mo_l_sm_replace_i0c0s0n0d0", "", "mo_l_sm_replace_i0c0s0n0d0p0"}},
			{"mc_l_sm_r0c0d0s0",            {"mo_l_sm_replace_i0c0s0"}},
			{"mc_l_sm_r0c0n0",              {"mo_l_sm_replace_i0c0s0n0"}},
			{"mc_l_sm_r0c0n0s0",            {"mo_l_sm_replace_i0c0s0n0", "", "mo_l_sm_replace_i0c0s0n0p0"}},
			{"mc_l_sm_r0c0n0s0p0",          {"mo_l_sm_replace_i0c0s0n0", "", "mo_l_sm_replace_i0c0s0n0p0"}},
			{"mc_l_sm_r0c0s0",              {"mo_l_sm_replace_i0c0s0"}},
			{"mc_l_sm_r0c0s0p0",            {"mo_l_sm_replace_i0c0s0"}},
			{"mc_l_sm_scroll_b0c0",         {"mo_l_sm_ndw_blend_i0c0s0n0"}},
			{"mc_l_sm_scroll_b0c0d0",       {"mo_l_sm_ndw_blend_i0c0s0n0"}},
			{"mc_l_sm_scroll_b0c0d0n0",     {"mo_l_sm_ndw_blend_i0c0s0n0"}},
			{"mc_l_sm_scroll_b0c0d0n0s0",   {"mo_l_sm_ndw_blend_i0c0s0n0", "", "", "mo_l_sm_ndw_blend_i0c0s0n0pa0"}},
			{"mc_l_sm_scroll_b0c0d0s0",     {"mo_l_sm_ndw_blend_i0c0s0"}},
			{"mc_l_sm_scroll_b0c0n0",       {"mo_l_sm_ndw_blend_i0c0s0n0"}},
			{"mc_l_sm_scroll_b0c0n0s0",     {"mo_l_sm_ndw_blend_i0c0s0n0", "", "", "mo_l_sm_ndw_blend_i0c0s0n0pa0"}},
			{"mc_l_sm_scroll_b0c0s0",       {"mo_l_sm_ndw_blend_i0c0s0"}},
			{"mc_l_sm_scroll_r0c0",         {"mo_l_sm_replace_i0c0s0n0"}},
			{"mc_l_sm_scroll_r0c0d0",       {"mo_l_sm_replace_i0c0s0n0"}},
			{"mc_l_sm_scroll_r0c0d0n0",     {"mo_l_sm_replace_i0c0n0d0"}},
			{"mc_l_sm_scroll_r0c0d0n0s0",   {"mo_l_sm_replace_i0c0s0n0d0", "", "mo_l_sm_replace_i0c0s0n0d0p0"}},
			{"mc_l_sm_scroll_r0c0d0s0",     {"mo_l_sm_replace_i0c0s0"}},
			{"mc_l_sm_scroll_r0c0n0",       {"mo_l_sm_replace_i0c0s0n0"}},
			{"mc_l_sm_scroll_r0c0n0s0",     {"mo_l_sm_replace_i0c0s0n0", "", "mo_l_sm_replace_i0c0s0n0p0"}},
			{"mc_l_sm_scroll_r0c0s0",       {"mo_l_sm_replace_i0c0s0"}},
			{"mc_l_sm_scroll_t0c0",         {"mo_l_sm_atest_i0c0s0n0"}},
			{"mc_l_sm_scroll_t0c0d0",       {"mo_l_sm_atest_i0c0s0n0"}},
			{"mc_l_sm_scroll_t0c0d0n0",     {"mo_l_sm_atest_i0c0s0n0"}},
			{"mc_l_sm_scroll_t0c0d0n0s0",   {"mo_l_sm_atest_i0c0s0n0", "", "", "mo_l_sm_atest_i0c0s0n0pa0"}},
			{"mc_l_sm_scroll_t0c0d0s0",     {"mo_l_sm_atest_i0c0s0"}},
			{"mc_l_sm_scroll_t0c0n0",       {"mo_l_sm_atest_i0c0s0n0"}},
			{"mc_l_sm_scroll_t0c0n0s0",     {"mo_l_sm_atest_i0c0s0n0", "", "", "mo_l_sm_atest_i0c0s0n0pa0"}},
			{"mc_l_sm_scroll_t0c0s0",       {"mo_l_sm_atest_i0c0s0"}},
			{"mc_l_sm_skin_r0c0",           {"mo_l_sm_replace_i0c0s0n0"}},
			{"mc_l_sm_skin_r0c0d0",         {"mo_l_sm_replace_i0c0s0n0"}},
			{"mc_l_sm_skin_r0c0d0n0",       {"mo_l_sm_replace_i0c0n0d0"}},
			{"mc_l_sm_skin_r0c0d0n0s0",     {"mo_l_sm_replace_i0c0s0n0d0", "", "mo_l_sm_replace_i0c0s0n0d0p0"}},
			{"mc_l_sm_skin_r0c0d0s0",       {"mo_l_sm_replace_i0c0s0"}},
			{"mc_l_sm_skin_r0c0n0",         {"mo_l_sm_replace_i0c0s0n0"}},
			{"mc_l_sm_skin_r0c0n0s0",       {"mo_l_sm_replace_i0c0s0n0", "", "mo_l_sm_replace_i0c0s0n0p0"}},
			{"mc_l_sm_skin_r0c0s0",         {"mo_l_sm_replace_i0c0s0"}},
			{"mc_l_sm_t0c0",                {"mo_l_sm_atest_i0c0s0n0"}},
			{"mc_l_sm_t0c0d0",              {"mo_l_sm_atest_i0c0s0n0"}},
			{"mc_l_sm_t0c0d0n0",            {"mo_l_sm_atest_i0c0s0n0"}},
			{"mc_l_sm_t0c0d0n0s0",          {"mo_l_sm_atest_i0c0s0n0", "", "", "mo_l_sm_atest_i0c0s0n0pa0"}},
			{"mc_l_sm_t0c0d0s0",            {"mo_l_sm_atest_i0c0s0"}},
			{"mc_l_sm_t0c0n0",              {"mo_l_sm_atest_i0c0s0n0"}},
			{"mc_l_sm_t0c0n0s0",            {"mo_l_sm_atest_i0c0s0n0", "", "", "mo_l_sm_atest_i0c0s0n0pa0"}},
			{"mc_l_sm_t0c0s0",              {"mo_l_sm_atest_i0c0s0"}},
			{"mc_l_t0c0",                   {"mo_l_sm_atest_i0c0s0n0"}},
			{"mc_l_t0c0d0",                 {"mo_l_sm_atest_i0c0s0n0"}},
			{"mc_l_t0c0d0n0",               {"mo_l_sm_atest_i0c0s0n0"}},
			{"mc_l_t0c0d0n0s0",             {"mo_l_sm_atest_i0c0s0n0", "", "", "mo_l_sm_atest_i0c0s0n0pa0"}},
			{"mc_l_t0c0d0s0",               {"mo_l_sm_atest_i0c0s0"}},
			{"mc_l_t0c0n0",                 {"mo_l_sm_atest_i0c0s0n0"}},
			{"mc_l_t0c0n0s0",               {"mo_l_sm_atest_i0c0s0n0", "", "", "mo_l_sm_atest_i0c0s0n0pa0"}},
			{"mc_l_t0c0s0",                 {"mo_l_sm_atest_i0c0s0"}},
			{"wc_l_b0c0",                   {"wc_l_sm_ndw_blend_i0c0s0n0"}},
			{"wc_l_b0c0d0",                 {"wc_l_sm_ndw_blend_i0c0s0n0"}},
			{"wc_l_b0c0d0n0",               {"wc_l_sm_ndw_blend_i0c0s0n0"}},
			{"wc_l_b0c0d0n0s0",             {"wc_l_sm_ndw_blend_i0c0s0n0", "", "", "wc_l_sm_ndw_blend_i0c0s0n0pa0"}},
			{"wc_l_b0c0d0s0",               {"wc_l_sm_ndw_blend_i0c0s0"}},
			{"wc_l_b0c0n0",                 {"wc_l_sm_ndw_blend_i0c0s0n0"}},
			{"wc_l_b0c0n0s0",               {"wc_l_sm_ndw_blend_i0c0s0n0", "", "", "wc_l_sm_ndw_blend_i0c0s0n0pa0"}},
			{"wc_l_b0c0s0",                 {"wc_l_sm_ndw_blend_i0c0s0"}},
			{"wc_l_flag_t0c0",              {"wc_l_sm_atest_i0c0"}},
			{"wc_l_flag_t0c0n0",            {"wc_l_sm_atest_i0c0"}},
			{"wc_l_flag_t0c0n0s0",          {"wc_l_sm_atest_i0c0s0"}},
			{"wc_l_flag_t0c0s0",            {"wc_l_sm_atest_i0c0s0"}},
			{"wc_l_r0c0",                   {"wc_l_sm_replace_i0c0s0n0"}},
			{"wc_l_r0c0d0",                 {"wc_l_sm_replace_i0c0s0n0"}},
			{"wc_l_r0c0d0n0",               {"wc_l_sm_replace_i0c0s0n0"}},
			{"wc_l_r0c0d0n0s0",             {"wc_l_sm_replace_i0c0s0n0", "", "wc_l_sm_replace_i0c0s0n0p0"}},
			{"wc_l_r0c0d0s0",               {"wc_l_sm_replace_i0c0s0"}},
			{"wc_l_r0c0n0",                 {"wc_l_sm_replace_i0c0s0n0"}},
			{"wc_l_r0c0n0s0",               {"wc_l_sm_replace_i0c0s0n0", "", "wc_l_sm_replace_i0c0s0n0p0"}},
			{"wc_l_r0c0s0",                 {"wc_l_sm_replace_i0c0s0"}},
			{"wc_l_scroll_b0c0",            {"wc_l_sm_ndw_blend_i0c0s0n0"}},
			{"wc_l_scroll_b0c0d0",          {"wc_l_sm_ndw_blend_i0c0s0n0"}},
			{"wc_l_scroll_b0c0d0n0",        {"wc_l_sm_ndw_blend_i0c0s0n0"}},
			{"wc_l_scroll_b0c0d0n0s0",      {"wc_l_sm_ndw_blend_i0c0s0n0", "", "", "wc_l_sm_ndw_blend_i0c0s0n0pa0"}},
			{"wc_l_scroll_b0c0d0s0",        {"wc_l_sm_ndw_blend_i0c0s0"}},
			{"wc_l_scroll_b0c0n0",          {"wc_l_sm_ndw_blend_i0c0s0n0"}},
			{"wc_l_scroll_b0c0n0s0",        {"wc_l_sm_ndw_blend_i0c0s0n0", "", "", "wc_l_sm_ndw_blend_i0c0s0n0pa0"}},
			{"wc_l_scroll_b0c0s0",          {"wc_l_sm_ndw_blend_i0c0s0"}},
			{"wc_l_scroll_r0c0",            {"wc_l_sm_replace_i0c0s0n0"}},
			{"wc_l_scroll_r0c0d0",          {"wc_l_sm_replace_i0c0s0n0"}},
			{"wc_l_scroll_r0c0d0n0",        {"wc_l_sm_replace_i0c0s0n0"}},
			{"wc_l_scroll_r0c0d0n0s0",      {"wc_l_sm_replace_i0c0s0n0", "", "wc_l_sm_replace_i0c0s0n0p0"}},
			{"wc_l_scroll_r0c0d0s0",        {"wc_l_sm_replace_i0c0s0"}},
			{"wc_l_scroll_r0c0n0",          {"wc_l_sm_replace_i0c0s0n0"}},
			{"wc_l_scroll_r0c0n0s0",        {"wc_l_sm_replace_i0c0s0n0", "", "wc_l_sm_replace_i0c0s0n0p0"}},
			{"wc_l_scroll_r0c0s0",          {"wc_l_sm_replace_i0c0s0"}},
			{"wc_l_scroll_t0c0",            {"wc_l_sm_atest_i0c0"}},
			{"wc_l_scroll_t0c0d0",          {"wc_l_sm_atest_i0c0"}},
			{"wc_l_scroll_t0c0d0n0",        {"wc_l_sm_atest_i0c0"}},
			{"wc_l_scroll_t0c0d0n0s0",      {"wc_l_sm_atest_i0c0s0"}},
			{"wc_l_scroll_t0c0d0s0",        {"wc_l_sm_atest_i0c0s0"}},
			{"wc_l_scroll_t0c0n0",          {"wc_l_sm_atest_i0c0"}},
			{"wc_l_scroll_t0c0n0s0",        {"wc_l_sm_atest_i0c0s0"}},
			{"wc_l_scroll_t0c0s0",          {"wc_l_sm_atest_i0c0s0"}},
			{"wc_l_skin_r0c0",              {"wc_l_sm_replace_i0c0s0n0"}},
			{"wc_l_skin_r0c0d0",            {"wc_l_sm_replace_i0c0s0n0"}},
			{"wc_l_skin_r0c0d0n0",          {"wc_l_sm_replace_i0c0s0n0"}},
			{"wc_l_skin_r0c0d0n0s0",        {"wc_l_sm_replace_i0c0s0n0", "", "wc_l_sm_replace_i0c0s0n0p0"}},
			{"wc_l_skin_r0c0d0s0",          {"wc_l_sm_replace_i0c0s0"}},
			{"wc_l_skin_r0c0n0",            {"wc_l_sm_replace_i0c0s0n0"}},
			{"wc_l_skin_r0c0n0s0",          {"wc_l_sm_replace_i0c0s0n0", "", "wc_l_sm_replace_i0c0s0n0p0"}},
			{"wc_l_skin_r0c0s0",            {"wc_l_sm_replace_i0c0s0"}},
			{"wc_l_sm_b0c0",                {"wc_l_sm_ndw_blend_i0c0s0n0"}},
			{"wc_l_sm_b0c0d0",              {"wc_l_sm_ndw_blend_i0c0s0n0"}},
			{"wc_l_sm_b0c0d0n0",            {"wc_l_sm_ndw_blend_i0c0s0n0"}},
			{"wc_l_sm_b0c0d0n0s0",          {"wc_l_sm_ndw_blend_i0c0s0n0", "", "", "wc_l_sm_ndw_blend_i0c0s0n0pa0"}},
			{"wc_l_sm_b0c0d0s0",            {"wc_l_sm_ndw_blend_i0c0s0"}},
			{"wc_l_sm_b0c0n0",              {"wc_l_sm_ndw_blend_i0c0s0n0"}},
			{"wc_l_sm_b0c0n0s0",            {"wc_l_sm_ndw_blend_i0c0s0n0", "", "", "wc_l_sm_ndw_blend_i0c0s0n0pa0"}},
			{"wc_l_sm_b0c0s0",              {"wc_l_sm_ndw_blend_i0c0s0"}},
			{"wc_l_sm_flag_t0c0",           {"wc_l_sm_atest_i0c0"}},
			{"wc_l_sm_flag_t0c0n0",         {"wc_l_sm_atest_i0c0"}},
			{"wc_l_sm_flag_t0c0n0s0",       {"wc_l_sm_atest_i0c0s0"}},
			{"wc_l_sm_flag_t0c0s0",         {"wc_l_sm_atest_i0c0s0"}},
			{"wc_l_sm_r0c0",                {"wc_l_sm_replace_i0c0s0n0"}},
			{"wc_l_sm_r0c0d0",              {"wc_l_sm_replace_i0c0s0n0"}},
			{"wc_l_sm_r0c0d0n0",            {"wc_l_sm_replace_i0c0s0n0"}},
			{"wc_l_sm_r0c0d0n0s0",          {"wc_l_sm_replace_i0c0s0n0", "", "wc_l_sm_replace_i0c0s0n0p0"}},
			{"wc_l_sm_r0c0d0s0",            {"wc_l_sm_replace_i0c0s0"}},
			{"wc_l_sm_r0c0n0",              {"wc_l_sm_replace_i0c0s0n0"}},
			{"wc_l_sm_r0c0n0s0",            {"wc_l_sm_replace_i0c0s0n0", "", "wc_l_sm_replace_i0c0s0n0p0"}},
			{"wc_l_sm_r0c0s0",              {"wc_l_sm_replace_i0c0s0"}},
			{"wc_l_sm_scroll_b0c0",         {"wc_l_sm_ndw_blend_i0c0s0n0"}},
			{"wc_l_sm_scroll_b0c0d0",       {"wc_l_sm_ndw_blend_i0c0s0n0"}},
			{"wc_l_sm_scroll_b0c0d0n0",     {"wc_l_sm_ndw_blend_i0c0s0n0"}},
			{"wc_l_sm_scroll_b0c0d0n0s0",   {"wc_l_sm_ndw_blend_i0c0s0n0", "", "", "wc_l_sm_ndw_blend_i0c0s0n0pa0"}},
			{"wc_l_sm_scroll_b0c0d0s0",     {"wc_l_sm_ndw_blend_i0c0s0"}},
			{"wc_l_sm_scroll_b0c0n0",       {"wc_l_sm_ndw_blend_i0c0s0n0"}},
			{"wc_l_sm_scroll_b0c0n0s0",     {"wc_l_sm_ndw_blend_i0c0s0n0", "", "", "wc_l_sm_ndw_blend_i0c0s0n0pa0"}},
			{"wc_l_sm_scroll_b0c0s0",       {"wc_l_sm_ndw_blend_i0c0s0"}},
			{"wc_l_sm_scroll_r0c0",         {"wc_l_sm_replace_i0c0s0n0"}},
			{"wc_l_sm_scroll_r0c0d0",       {"wc_l_sm_replace_i0c0s0n0"}},
			{"wc_l_sm_scroll_r0c0d0n0",     {"wc_l_sm_replace_i0c0s0n0"}},
			{"wc_l_sm_scroll_r0c0d0n0s0",   {"wc_l_sm_replace_i0c0s0n0", "", "wc_l_sm_replace_i0c0s0n0p0"}},
			{"wc_l_sm_scroll_r0c0d0s0",     {"wc_l_sm_replace_i0c0s0"}},
			{"wc_l_sm_scroll_r0c0n0",       {"wc_l_sm_replace_i0c0s0n0"}},
			{"wc_l_sm_scroll_r0c0n0s0",     {"wc_l_sm_replace_i0c0s0n0", "", "wc_l_sm_replace_i0c0s0n0p0"}},
			{"wc_l_sm_scroll_r0c0s0",       {"wc_l_sm_replace_i0c0s0"}},
			{"wc_l_sm_scroll_t0c0",         {"wc_l_sm_atest_i0c0"}},
			{"wc_l_sm_scroll_t0c0d0",       {"wc_l_sm_atest_i0c0"}},
			{"wc_l_sm_scroll_t0c0d0n0",     {"wc_l_sm_atest_i0c0"}},
			{"wc_l_sm_scroll_t0c0d0n0s0",   {"wc_l_sm_atest_i0c0s0"}},
			{"wc_l_sm_scroll_t0c0d0s0",     {"wc_l_sm_atest_i0c0s0"}},
			{"wc_l_sm_scroll_t0c0n0",       {"wc_l_sm_atest_i0c0"}},
			{"wc_l_sm_scroll_t0c0n0s0",     {"wc_l_sm_atest_i0c0s0"}},
			{"wc_l_sm_scroll_t0c0s0",       {"wc_l_sm_atest_i0c0s0"}},
			{"wc_l_sm_skin_r0c0",           {"wc_l_sm_replace_i0c0s0n0"}},
			{"wc_l_sm_skin_r0c0d0",         {"wc_l_sm_replace_i0c0s0n0"}},
			{"wc_l_sm_skin_r0c0d0n0",       {"wc_l_sm_replace_i0c0s0n0"}},
			{"wc_l_sm_skin_r0c0d0n0s0",     {"wc_l_sm_replace_i0c0s0n0", "", "wc_l_sm_replace_i0c0s0n0p0"}},
			{"wc_l_sm_skin_r0c0d0s0",       {"wc_l_sm_replace_i0c0s0"}},
			{"wc_l_sm_skin_r0c0n0",         {"wc_l_sm_replace_i0c0s0n0"}},
			{"wc_l_sm_skin_r0c0n0s0",       {"wc_l_sm_replace_i0c0s0n0", "", "wc_l_sm_replace_i0c0s0n0p0"}},
			{"wc_l_sm_skin_r0c0s0",         {"wc_l_sm_replace_i0c0s0"}},
			{"wc_l_sm_t0c0",                {"wc_l_sm_atest_i0c0"}},
			{"wc_l_sm_t0c0d0",              {"wc_l_sm_atest_i0c0"}},
			{"wc_l_sm_t0c0d0n0",            {"wc_l_sm_atest_i0c0"}},
			{"wc_l_sm_t0c0d0n0s0",          {"wc_l_sm_atest_i0c0s0"}},
			{"wc_l_sm_t0c0d0s0",            {"wc_l_sm_atest_i0c0s0"}},
			{"wc_l_sm_t0c0n0",              {"wc_l_sm_atest_i0c0"}},
			{"wc_l_sm_t0c0n0s0",            {"wc_l_sm_atest_i0c0s0"}},
			{"wc_l_sm_t0c0s0",              {"wc_l_sm_atest_i0c0s0"}},
			{"wc_l_t0c0",                   {"wc_l_sm_atest_i0c0"}},
			{"wc_l_t0c0d0",                 {"wc_l_sm_atest_i0c0"}},
			{"wc_l_t0c0d0n0",               {"wc_l_sm_atest_i0c0"}},
			{"wc_l_t0c0d0n0s0",             {"wc_l_sm_atest_i0c0s0"}},
			{"wc_l_t0c0d0s0",               {"wc_l_sm_atest_i0c0s0"}},
			{"wc_l_t0c0n0",                 {"wc_l_sm_atest_i0c0"}},
			{"wc_l_t0c0n0s0",               {"wc_l_sm_atest_i0c0s0"}},
			{"wc_l_t0c0s0",                 {"wc_l_sm_atest_i0c0s0"}},

			{"mc_ambient_r0c0",             {"mo_l_sm_replace_i0c0"}},
			{"mc_ambient_t0c0",             {"mo_l_sm_atest_i0c0"}},
			{"wc_ambient_r0c0",             {"wc_l_sm_replace_i0c0"}},
			{"wc_ambient_t0c0",             {"wc_l_sm_atest_i0c0"}},
			{"mc_effect_falloff_add_nofog",				make_techset_map("eq_effect_falloff_add_lin_nofog_ndw_nocast")},
			{"mc_effect_zfeather_falloff_add_nofog",	make_techset_map("eq_effect_zfeather_falloff_add_lin_nofog_ndw_nocast")},
			{"mc_effect_zfeather_falloff_add_nofog_eyeoffset",	make_techset_map("eq_effect_zfeather_falloff_add_lin_nofog_eyeoffset_ndw_nocast")},
			{"mc_effect_zfeather_falloff_add_lin_nofog_eyeoffset", make_techset_map("eq_effect_zfeather_falloff_add_lin_nofog_eyeoffset_ndw_nocast")},
			
			{"mc_unlit",								make_techset_map("mo_unlit_replace_lin", "mo_unlit_replace_lin_ct")},
			{"mc_unlit_replace",						make_techset_map("mo_unlit_replace_lin", "mo_unlit_replace_lin_ct")},
			{"mc_unlit_replace_lin",                 make_techset_map("mo_unlit_replace_lin", "mo_unlit_replace_lin_ct")},
			{"mc_unlit_replace_lin_nocast",          make_techset_map("mo_unlit_replace_lin_nocast")},
			{"mc_unlit_add",						make_techset_map("mo_unlit_add_lin_ndw", "mo_unlit_add_lin_ct_ndw")},
			{"mc_unlit_alphatest",						make_techset_map("mo_unlit_atest_lin_ct", "mo_unlit_atest_lin_ct")},
			{"mc_unlit_blend_lin",                   make_techset_map("mo_unlit_blend_lin_ndw")},
			{"mc_unlit_screen_lin_ua",              make_techset_map("mo_unlit_blend_lin_ndw_ua")},
			{"mc_threed_cinematic",                 make_techset_map("mo_threed_cinematic|_evc")},
			{"mc_ambient_t0c0_nocast",              make_techset_map("mo_l_sm_atest_i0c0s0n0_nocast")},
			{"m_l_sm_b0c0s0_sat",                   make_techset_map("m_l_sm_ndw_blend_i0c0s0")},
			{"m_l_sm_b0c0q0n0s0_sat",               make_techset_map("m_l_sm_ndw_blend_i0c0s0n0")},
			
			{"wc_unlit",							make_techset_map("wc_unlit_replace_lin", "wc_unlit_replace_lin_ct")},
			{"wc_unlit_add",							make_techset_map("wc_unlit_add_lin_ndw", "wc_unlit_add_lin_ct_ndw")},
			{"wc_unlit_multiply",						make_techset_map("wc_unlit_multiply_lin_ndw")},
			{"wc_unlit_falloff_add",					make_techset_map("wc_unlit_falloff_add_lin_ndw", "wc_unlit_falloff_add_lin_ct_ndw")},
			{"wc_unlit_add_lin",                    make_techset_map("wc_unlit_add_lin_ndw")},
			{"wc_unlit_add_lin_ua",                 make_techset_map("wc_unlit_add_lin_ndw_ua")},
			{"wc_unlit_blend_lin_ua",               make_techset_map("wc_unlit_blend_lin_ndw_ua")},
			{"wc_unlit_blend_lin_custom_objective", make_techset_map("wc_unlit_blend_lin_ndw_custom_objective")},
			{"wc_unlit_distfalloff_replace",        make_techset_map("wc_unlit_blend_lin_ndw")},
			{"wc_unlit_multiply_lin",               make_techset_map("wc_unlit_multiply_lin_ndw")},
			{"wc_unlit_replace_lin",                make_techset_map("wc_unlit_replace_lin", "wc_unlit_replace_lin_ct")},
			{"wc_unlit_replace_lin_nocast",         make_techset_map("wc_unlit_replace_lin", "wc_unlit_replace_lin_ct_nocast")},
			{"wc_unlit_replace_lin_ua",             make_techset_map("wc_unlit_replace_lin_ct_ua")},
			{"wc_distortion_scale",                 make_techset_map("wc_distortion_scale")},
			{"wc_distortion_scale_ua",              make_techset_map("wc_distortion_scale_ua")},
			{"wc_distortion_scale_zfeather",        make_techset_map("wc_distortion_scale")},
			{"wc_distortion_scale_ua_zfeather",     make_techset_map("wc_distortion_scale_ua")},
			{"wc_l_sm_t0c0_nocast",                 make_techset_map("wc_l_sm_atest_i0c0_nocast")},
			{"wc_l_t0c0_nocast",                    make_techset_map("wc_l_sm_atest_i0c0_nocast")},
			{"tools_b0c0",                          make_techset_map("tools_b0c0")},

			{"mc_shadowcaster",							make_techset_map("m_shadowcaster")},
			{"mc_shadowcaster_atest",					make_techset_map("m_shadowcaster")},
			{"wc_shadowcaster",							make_techset_map("w_shadowcaster")},

			{"distortion_scale_zfeather",				make_techset_map("eq_distortion_scale_zfeather")},

			{"wc_sky",									make_techset_map("w_sky")},

			{"2d",										make_techset_map("2d", "2d|_ct")},

			{"effect",									make_techset_map("eq_effect_blend_lin_ndw_nocast")},
			{"effect_nofog",							make_techset_map("eq_effect_blend_lin_nofog_ndw_nocast")},
			{"effect_zfeather_add",						make_techset_map("eq_effect_zfeather_add_lin_ct_ndw_nocast")},
			{"effect_zfeather_add_nofog_eyeoffset",		make_techset_map("eq_effect_zfeather_add_lin_nofog_eyeoffset_ndw_nocast")},
			{"effect_zfeather_add_nofog",				make_techset_map("eq_effect_zfeather_add_lin_nofog_ndw_nocast")},

			{"distortion_scale",						make_techset_map("eq_distortion_scale")},
			{"distortion_scale_zfeather",				make_techset_map("eq_distortion_scale_zfeather")},
		};

		std::unordered_map<std::string, techset_map_s> mapped_techsets_effect_vertlit =
		{
			{"effect",									make_techset_map("ev_effect_blend_lin_ct_ndw_nocast")},
		};

		enum MaterialType : std::uint8_t
		{
			MTL_TYPE_DEFAULT = 0x0,
			MTL_TYPE_MODEL = 1,
			MLT_TYPE_MODEL_UNK2 = 2,
			MTL_TYPE_MODEL_UNK3 = 3,
			MTL_TYPE_MODEL_UNK4 = 4,
			MTL_TYPE_MODEL_UNK5 = 5,
			MTL_TYPE_MODEL_UNK6 = 6,
			MTL_TYPE_MODEL_UNK7 = 7,
			MTL_TYPE_MODEL_UNK8 = 8,
			MTL_TYPE_MODEL_UNK9 = 9,
			MTL_TYPE_MODEL_UNK10 = 10,
			MTL_TYPE_MODEL_UNK11 = 11,
			MTL_TYPE_MODEL_UNK12 = 12,
			MTL_TYPE_MODEL_UNK13 = 13,
			MTL_TYPE_MODEL_UNK14 = 14,
			MTL_TYPE_MODEL_UNK15 = 15,
			MTL_TYPE_MODEL_LMAP = 16,
			MTL_TYPE_MODEL_LMAP_VERTCOL = 17,
			MTL_TYPE_MODEL_UNK18 = 18,
			MTL_TYPE_MODEL_UNK19 = 19,
			MTL_TYPE_MODEL_UNK20 = 20,
			MTL_TYPE_MODEL_UNK21 = 21,
			MTL_TYPE_MODEL_IMPACT = 22,
			MTL_TYPE_MODEL_SELFVIS = 23,
			MTL_TYPE_MODEL_VERTCOL_SELFVIS = 24,
			MTL_TYPE_MODEL_UNK25 = 25,
			MTL_TYPE_MODEL_UNK26 = 26,
			MTL_TYPE_MODEL_UNK27 = 27,
			MTL_TYPE_MODEL_SECONDUV_SELFVIS = 28,
			MTL_TYPE_MODEL_SECONDUV_VERTCOL_SELFVIS = 29,
			MTL_TYPE_MODEL_UNK30 = 30,
			MTL_TYPE_MODEL_UNK31 = 31,
			MTL_TYPE_MODEL_UNK32 = 32,
			MTL_TYPE_MODEL_UNK33 = 33,
			MTL_TYPE_MODEL_UNK34 = 34,
			MTL_TYPE_MODEL_UNK35 = 35,
			MTL_TYPE_MODEL_UNK36 = 36,
			MTL_TYPE_MODEL_UNK37 = 37,
			MTL_TYPE_MODEL_UNK38 = 38,
			MTL_TYPE_MODEL_UNK39 = 39,
			MTL_TYPE_MODEL_UNK40 = 40,
			MTL_TYPE_MODEL_UNK41 = 41,
			MTL_TYPE_MODEL_UNK42 = 42,
			MTL_TYPE_MODEL_UNK43 = 43,
			MTL_TYPE_MODEL_SUBDIV = 44,
			MTL_TYPE_MODEL_UNK45 = 45,
			MTL_TYPE_MODEL_UNK46 = 46,
			MTL_TYPE_MODEL_UNK47 = 47,
			MTL_TYPE_MODEL_UNK48 = 48,
			MTL_TYPE_MODEL_UNK49 = 49,
			MTL_TYPE_MODEL_UNK50 = 50,
			MTL_TYPE_MODEL_UNK51 = 51,
			MTL_TYPE_MODEL_UNK52 = 52,
			MTL_TYPE_MODEL_SUBDIV_VERTCOL_SELFVIS = 53,
			MTL_TYPE_MODEL_UNK54 = 54,
			MTL_TYPE_MODEL_UNK55 = 55,
			MTL_TYPE_MODEL_UNK56 = 56,
			MTL_TYPE_MODEL_SUBDIV_SELFVIS = 57,
			MTL_TYPE_MODEL_UNK58 = 58,
			MTL_TYPE_MODEL_UNK59 = 59,
			MTL_TYPE_MODEL_UNK60 = 60,
			MTL_TYPE_MODEL_UNK61 = 61,
			MTL_TYPE_MODEL_UNK62 = 62,
			MTL_TYPE_MODEL_UNK63 = 63,
			MTL_TYPE_MODEL_UNK64 = 64,
			MTL_TYPE_MODEL_UNK65 = 65,
			MTL_TYPE_MODEL_UNK66 = 66,
			MTL_TYPE_MODEL_UNK67 = 67,
			MTL_TYPE_WORLD = 68,
			MTL_TYPE_WORLD_VERTCOL = 69,
			MTL_TYPE_WORLD_IMPACT = 70,
			MTL_TYPE_EFFECT_LMAP = 71,
			MTL_TYPE_EFFECT_VERTLIT = 72,
			MTL_TYPE_EFFECT_QUAD = 73,
		};

		std::string prefixes[] =
		{
			"w",
			"wc",
			"mo",
			"ev",
			"eq",
			"particle",
		};

		std::uint8_t prefixes_types[] =
		{
			MTL_TYPE_WORLD,
			MTL_TYPE_WORLD_VERTCOL,
			MTL_TYPE_MODEL_SELFVIS,
			MTL_TYPE_EFFECT_VERTLIT,
			MTL_TYPE_EFFECT_QUAD,
			MTL_TYPE_EFFECT_LMAP,
		};

		std::string material_prefix_for(const std::string& prefix)
		{
			return prefix == "particle" ? "el" : prefix;
		}

		std::string canonical_iw5_techset(std::string techset)
		{
			for (int i = 0; i < 8; i++)
			{
				if (mapped_techsets.contains(techset) || mapped_techsets_effect_vertlit.contains(techset))
				{
					return techset;
				}
				if (techset.ends_with("_sat"))
				{
					techset.erase(techset.size() - 4);
				}
				else if (techset.ends_with("_nocast"))
				{
					techset.erase(techset.size() - 7);
				}
				else if (techset.ends_with("_custom_growing_ice_cracks"))
				{
					techset.erase(techset.size() - 26);
				}
				else if (techset.ends_with("p0") && techset.find("_l_sm_") != std::string::npos)
				{
					techset.erase(techset.size() - 2);
				}
				else if (const auto heat = techset.find("_l_sm_heat_"); heat != std::string::npos)
				{
					techset.replace(heat, 11, "_l_sm_");
				}
				else if (const auto detail_normal = techset.find("q0");
					detail_normal != std::string::npos && techset.find("_l_sm_") != std::string::npos)
				{
					techset.erase(detail_normal, 2);
				}
				else if (const auto du_dm = techset.find("_l_sm_du_dm_"); du_dm != std::string::npos)
				{
					techset.replace(du_dm, 12, "_l_sm_");
				}
				else if (techset.starts_with("wc_l_sm_ua_"))
				{
					techset.erase(7, 3);
				}
				else
				{
					break;
				}
			}
			return techset;
		}

		std::string get_packed_techset(const std::string& techset, const bool effect_vertlit)
		{
			const auto mapped_name = canonical_iw5_techset(techset);

			const auto& table = effect_vertlit ? mapped_techsets_effect_vertlit : mapped_techsets;
			const auto it = table.find(mapped_name);
			if (it != table.end() && !it->second.techset[techset_map_type_e::packed].empty())
			{
				return it->second.techset[techset_map_type_e::packed];
			}

			if (effect_vertlit)
			{
				const auto regular = mapped_techsets.find(mapped_name);
				if (regular != mapped_techsets.end())
				{
					return regular->second.techset[techset_map_type_e::packed];
				}
			}

			return {};
		}

		std::string get_packed_alpha_techset(const std::string& techset, const bool effect_vertlit)
		{
			const auto mapped_name = canonical_iw5_techset(techset);

			const auto& table = effect_vertlit ? mapped_techsets_effect_vertlit : mapped_techsets;
			const auto it = table.find(mapped_name);
			if (it != table.end() && !it->second.techset[techset_map_type_e::packed_alpha].empty())
			{
				return it->second.techset[techset_map_type_e::packed_alpha];
			}

			if (effect_vertlit)
			{
				const auto regular = mapped_techsets.find(mapped_name);
				if (regular != mapped_techsets.end())
				{
					return regular->second.techset[techset_map_type_e::packed_alpha];
				}
			}

			return {};
		}

		const std::unordered_set<std::string> stock_effect_techsets =
		{
			"eq_effect_add_lin_ct_ndw_nocast",
			"eq_effect_add_lin_ct_nofog_ndw_nocast",
			"eq_effect_add_lin_ndw_nocast",
			"eq_effect_add_lin_nofog_ndw_nocast",
			"eq_effect_blend_lin_ct_ndw_nocast",
			"eq_effect_blend_lin_ndw_nocast",
			"eq_effect_blend_lin_nofog_ndw_nocast",
			"eq_effect_zfeather_add_lin_ct_eyeoffset_ndw_nocast",
			"eq_effect_zfeather_add_lin_ct_ndw_nocast",
			"eq_effect_zfeather_add_lin_ct_nofog_eyeoffset_ndw_nocast",
			"eq_effect_zfeather_add_lin_ct_nofog_ndw_nocast",
			"eq_effect_zfeather_add_lin_eyeoffset_ndw_nocast",
			"eq_effect_zfeather_add_lin_ndw_nocast",
			"eq_effect_zfeather_add_lin_nofog_eyeoffset_ndw_nocast",
			"eq_effect_zfeather_add_lin_nofog_ndw_nocast",
			"eq_effect_zfeather_blend_lin_ct_ndw_nocast",
			"eq_effect_zfeather_blend_lin_ct_nofog_ndw_nocast",
			"eq_effect_zfeather_blend_lin_eyeoffset_ndw_nocast",
			"eq_effect_zfeather_blend_lin_ndw_nocast",
			"eq_effect_zfeather_blend_lin_nofog_eyeoffset_ndw_nocast",
			"eq_effect_zfeather_blend_lin_nofog_ndw_nocast",
			"eq_effect_zfeather_outdoor_blend_lin_ndw_nocast",
			"ev_effect_add_lin_ct_ndw_nocast",
			"ev_effect_add_lin_nofog_ndw_nocast",
			"ev_effect_blend_lin_ct_ndw_nocast",
			"ev_effect_blend_lin_ndw_nocast",
			"ev_effect_blend_lin_nofog_ndw_nocast",
			"ev_effect_zfeather_add_lin_ndw_nocast",
			"ev_effect_zfeather_blend_lin_ct_ndw_nocast",
			"ev_effect_zfeather_blend_lin_ndw_nocast",
			"particle_cloud_add_ct_lin",
			"particle_cloud_add_lin",
			"particle_cloud_blend_ct_lin",
			"particle_cloud_blend_lin",
			"particle_cloud_outdoor_add_lin",
			"particle_cloud_outdoor_blend_lin",
			"particle_cloud_spark_add_ct_lin",
			"particle_cloud_spark_add_lin",
			"particle_cloud_spark_blend_ct_lin",
		};

		std::string map_effect_techset_by_features(const std::string& techset, const bool effect_vertlit, const bool color_tint)
		{
			std::vector<std::string> parts;
			for (std::size_t start = 0, end; start <= techset.size(); start = end + 1)
			{
				end = techset.find('_', start);
				if (end == std::string::npos) end = techset.size();
				parts.emplace_back(techset.substr(start, end - start));
			}

			const auto has = [&](const char* part) { return std::find(parts.begin(), parts.end(), part) != parts.end(); };

			const bool is_cloud = techset.starts_with("particle_cloud");
			if (!is_cloud && !techset.starts_with("effect"))
			{
				return {};
			}

			const bool add = has("add");
			const bool nofog = has("nofog");
			const bool eyeoffset = has("eyeoffset");
			const bool outdoor = has("outdoor");

			if (is_cloud)
			{
				const bool spark = has("spark") || has("sparkf");
				for (const auto tint : { color_tint, false })
				{
					for (const auto keep_variant : { true, false })
					{
						std::string name = "particle_cloud";
						if (keep_variant && spark) name += "_spark";
						else if (keep_variant && outdoor) name += "_outdoor";
						name += (add || spark) ? "_add" : "_blend";
						name += tint ? "_ct_lin" : "_lin";
						if (stock_effect_techsets.contains(name)) return name;
					}
				}
				return {};
			}

			const bool zfeather = has("zfeather");
			const std::string blend = add ? "add" : "blend";

			const char* prefix = effect_vertlit ? "ev" : "eq";
			for (const auto tint : { color_tint, false })
			{
				for (auto drop = 0; drop < 5; drop++)
				{
					const bool use_outdoor = outdoor && drop < 1;
					const bool use_eyeoffset = eyeoffset && drop < 2;
					const bool use_nofog = nofog && drop < 3;
					const bool use_zfeather = zfeather && drop < 4;

					std::string name = std::string(prefix) + "_effect";
					if (use_zfeather) name += "_zfeather";
					if (use_outdoor) name += "_outdoor";
					name += "_" + blend + "_lin";
					if (tint) name += "_ct";
					if (use_nofog) name += "_nofog";
					if (use_eyeoffset) name += "_eyeoffset";
					name += "_ndw_nocast";

					if (stock_effect_techsets.contains(name)) return name;
				}
			}

			return effect_vertlit ? map_effect_techset_by_features(techset, false, color_tint) : std::string{};
		}

		std::string get_mapped_techset(const std::string& techset, const bool effect_vertlit, const bool color_tint)
		{
			const auto mapped_name = canonical_iw5_techset(techset);
			if (!effect_vertlit)
			{
				const auto it = mapped_techsets.find(mapped_name);
				if (it != mapped_techsets.end())
				{
					auto tech = it->second.techset[color_tint ? techset_map_type_e::color_tint : techset_map_type_e::regular];
					return tech.empty() ? it->second.techset[techset_map_type_e::regular] : tech;
				}
			}
			else
			{
				const auto it = mapped_techsets_effect_vertlit.find(mapped_name);
				if (it != mapped_techsets_effect_vertlit.end())
				{
					auto tech = it->second.techset[color_tint ? techset_map_type_e::color_tint : techset_map_type_e::regular];
					return tech.empty() ? it->second.techset[techset_map_type_e::regular] : tech;
				}

				const auto regular = mapped_techsets.find(mapped_name);
				if (regular != mapped_techsets.end())
				{
					auto tech = regular->second.techset[color_tint ? techset_map_type_e::color_tint : techset_map_type_e::regular];
					return tech.empty() ? regular->second.techset[techset_map_type_e::regular] : tech;
				}
			}

			if (auto effect = map_effect_techset_by_features(techset, effect_vertlit, color_tint); !effect.empty())
			{
				return effect;
			}

			return "2d";
		}

		std::uint8_t get_material_type_from_techset(std::string techset)
		{
			if (!techset.empty())
			{
				for (auto prefix_idx = 0; prefix_idx < std::size(prefixes); prefix_idx++)
				{
					const auto prefix = prefixes[prefix_idx];
					const auto type = prefixes_types[prefix_idx];

					if (techset.starts_with(prefix + "_"))
					{
						return type;
					}
				}
			}

			return MTL_TYPE_DEFAULT;
		}

		iw7_types::TextureSemantic surf_flags_conversion_table[13]
		{
			iw7_types::TextureSemantic::TS_2D,
			iw7_types::TextureSemantic::TS_FUNCTION,
			iw7_types::TextureSemantic::TS_COLOR_MAP,
			iw7_types::TextureSemantic::TS_DETAIL_MAP,
			iw7_types::TextureSemantic::TS_UNUSED_4,
			iw7_types::TextureSemantic::TS_NORMAL_MAP,
			iw7_types::TextureSemantic::TS_UNUSED_6,
			iw7_types::TextureSemantic::TS_UNUSED_7,
			iw7_types::TextureSemantic::TS_SPECULAR_MAP,
			iw7_types::TextureSemantic::TS_UNUSED_4,
			iw7_types::TextureSemantic::TS_UNUSED_4,
			iw7_types::TextureSemantic::TS_UNUSED_4,
			iw7_types::TextureSemantic::TS_DISPLACEMENT_MAP,
		};

		std::uint8_t convert_semantic(std::uint8_t from)
		{
			return surf_flags_conversion_table[from];
		}

		namespace
		{
			std::string get_IW7_techset(std::string name, std::string matname, bool* result, bool effect_vertlit = false, bool has_ct = false)
			{
				auto iw7_techset = get_mapped_techset(name, effect_vertlit, has_ct);

				*result = true;
				if (name != "2d" && iw7_techset == "2d")
				{
					ZONETOOL_ERROR("Could not find mapped IW7 techset for techset \"%s\" (material: %s)%s",
						name.data(),
						matname.data(),
						effect_vertlit ? " (EFFECT_VERTLIT)" : "");
					*result = false;
				}

				return iw7_techset;
			}

			std::unordered_map<std::uint8_t, std::uint8_t> mapped_sortkeys =
			{
				{0, 2},	
				{1, 2},	
				{2, 3},    
				{3, 4},	
				{4, 5},	
				{5, 6},	
				{6, 7},	
				{7, 8},	
				{8, 9},	
				{9, 10},
				{10, 11},
				{11, 12},
				{12, 13},
				{13, 14},
				{14, 15},
				{15, 16},
				{16, 17},
				{17, 18},
				{19, 20},
				{20, 21},
				{24, 25},  
				{25, 26},  
				{26, 27},
				{27, 28},
				{28, 29},
				{29, 30},
				{30, 31},
				{31, 32},
				{32, 33},
				{33, 34},
				{34, 2},
				{43, 36},
				{44, 36},
				{45, 36},
				{46, 36},
				{47, 30},
				{48, 35},
				{49, 35},
				{50, 35},
				{51, 35},
				{53, 35},
				{54, 41},
			};

			std::unordered_map<std::string, std::uint8_t> mapped_sortkeys_by_techset =
			{
				
			};

			std::uint8_t get_IW7_sortkey(std::uint8_t sortkey, std::string matname, std::string IW7_techset)
			{
				if (mapped_sortkeys_by_techset.find(IW7_techset) != mapped_sortkeys_by_techset.end())
				{
					return mapped_sortkeys_by_techset[IW7_techset];
				}

				if (mapped_sortkeys.contains(sortkey))
				{
					return mapped_sortkeys[sortkey];
				}

				ZONETOOL_ERROR("Could not find mapped IW7 sortkey for sortkey: %d (material: %s)", sortkey, matname.data());

				return sortkey;
			}

			std::unordered_map<std::uint8_t, std::uint8_t> mapped_camera_regions =
			{
				{0, 0},
				{1, 3},
				{2, 4},
				{3, 4},
				{4, 4},
			};

			std::unordered_map<std::string, std::uint8_t> mapped_camera_regions_by_techset =
			{
			};

			std::uint8_t get_IW7_camera_region(std::uint8_t camera_region, std::string matname, std::string IW7_techset)
			{
				if (mapped_camera_regions_by_techset.find(IW7_techset) != mapped_camera_regions_by_techset.end())
				{
					return mapped_camera_regions_by_techset[IW7_techset];
				}

				if (mapped_camera_regions.contains(camera_region))
				{
					return mapped_camera_regions[camera_region];
				}

				ZONETOOL_ERROR("Could not find mapped IW7 camera region for camera region: %d (material: %s)", camera_region, matname.data());

				return camera_region;
			}

			std::unordered_map<std::string, std::uint8_t> mapped_render_flags_by_techset =
			{
				{"2d", 0x1},
			};

			std::int32_t get_render_flags_by_techset(std::string IW7_techset)
			{
				std::int32_t flags = 0;

				if (mapped_render_flags_by_techset.find(IW7_techset) != mapped_render_flags_by_techset.end())
				{
					flags |= mapped_render_flags_by_techset[IW7_techset];
				}

				if (IW7_techset.starts_with("el_") || IW7_techset.starts_with("eq_") || IW7_techset.starts_with("ev_"))
				{
					flags |= 0x1;
				}

				return flags;
			}
		}

			constexpr unsigned int slot_hash_colour = 2695565377;
			constexpr unsigned int slot_hash_normal = 1507003663;
			constexpr unsigned int slot_hash_spec = 887934131;
			constexpr unsigned int slot_hash_detail = 3948059469;

			std::string clean_name(const std::string& name)
			{
				auto new_name = name;
				std::replace(new_name.begin(), new_name.end(), '*', '_');
				return new_name;
			}

			bool has_color_tint(const zonetool::iw5::Material* asset)
			{
				static constexpr float identity[4] = { 1.0f, 1.0f, 1.0f, 1.0f };

				const auto constants = asset->constantTable.get();
				for (auto i = 0; i < asset->constantCount; i++)
				{
					if (constants[i].nameHash == 3054254906u)
					{
						return !std::equal(std::begin(constants[i].literal), std::end(constants[i].literal), std::begin(identity));
					}
				}

				return false;
			}

			struct required_slot
			{
				const char* image;
				std::uint8_t semantic;
				std::uint8_t sampler;
				std::uint8_t last_character;
				std::uint8_t first_character;
				unsigned int hash;
			};

			const required_slot slot_colour{ "$white", 2, 19, 112, 99, 2695565377 };
			const required_slot slot_normal{ "$identitynormalmap", 5, 1, 112, 110, 1507003663 };
			const required_slot slot_spec{ "$zero", 8, 19, 112, 115, 887934131 };
			const required_slot slot_occlusion{ "$zero", 9, 19, 112, 115, 2771134132 };
			const required_slot slot_layer1{ "$black", 2, 19, 49, 99, 3054311504 };
			const required_slot slot_detail{ "$gray", 3, 19, 112, 100, slot_hash_detail };

			const std::unordered_map<std::string, std::vector<required_slot>> required_slots =
			{
				{"mo_l_sm_atest_i0c0",                    {slot_colour, slot_normal}},
				{"mo_l_sm_atest_i0c0n0",                  {slot_colour, slot_normal}},
				{"mo_l_sm_atest_i0c0s0",                  {slot_colour, slot_normal, slot_spec}},
				{"mo_l_sm_atest_i0c0s0n0",                {slot_colour, slot_normal, slot_spec}},
				{"mo_l_sm_atest_i0c0s0n0_nocast",         {slot_colour, slot_normal, slot_spec}},
				{"mo_l_sm_ndw_blend_i0c0",                {slot_colour, slot_normal}},
				{"mo_l_sm_ndw_blend_i0c0n0",              {slot_colour, slot_normal}},
				{"mo_l_sm_ndw_blend_i0c0s0",              {slot_colour, slot_normal, slot_spec}},
				{"mo_l_sm_ndw_blend_i0c0s0n0",            {slot_colour, slot_normal, slot_spec}},
				{"mo_l_sm_replace_i0c0",                  {slot_colour, slot_normal}},
				{"mo_l_sm_replace_i0c0n0",                {slot_colour, slot_normal}},
				{"mo_l_sm_replace_i0c0n0d0",              {slot_colour, slot_detail, slot_normal}},
				{"mo_l_sm_replace_i0c0s0",                {slot_colour, slot_normal, slot_spec}},
				{"mo_l_sm_replace_i0c0s0n0",              {slot_colour, slot_normal, slot_spec}},
				{"mo_l_sm_replace_i0c0s0n0d0",            {slot_colour, slot_detail, slot_normal, slot_spec}},
				{"wc_l_sm_atest_i0c0",                    {slot_colour, slot_normal}},
				{"wc_l_sm_atest_i0c0s0",                  {slot_colour, slot_normal, slot_spec}},
				{"wc_l_sm_ndw_blend_i0c0",                {slot_colour, slot_normal}},
				{"wc_l_sm_ndw_blend_i0c0n0",              {slot_colour, slot_normal}},
				{"wc_l_sm_ndw_blend_i0c0s0",              {slot_colour, slot_normal, slot_spec}},
				{"wc_l_sm_ndw_blend_i0c0s0n0",            {slot_colour, slot_normal, slot_spec}},
				{"wc_l_sm_replace_i0c0",                  {slot_colour, slot_normal}},
				{"wc_l_sm_replace_i0c0n0",                {slot_colour, slot_normal}},
				{"wc_l_sm_replace_i0c0s0",                {slot_colour, slot_normal, slot_spec}},
				{"wc_l_sm_replace_i0c0s0n0",              {slot_colour, slot_normal, slot_spec, slot_occlusion}},
				{"w_l_sm_replace_r0c0_m1c1",              {slot_colour, slot_normal, slot_layer1}},
				{"w_l_sm_replace_r0c0nl0_m1c1",           {slot_colour, slot_normal, slot_layer1}},
				{"w_l_sm_blend_b0c0sl0ol0_m1c1",          {slot_colour, slot_normal, slot_spec, slot_occlusion, slot_layer1}},
			};

			void insert_sorted(ordered_json& table, const char* key, const unsigned int hash, ordered_json&& entry)
			{
				auto position = table.size();
				for (std::size_t i = 0; i < table.size(); i++)
				{
					const auto existing = table[i][key].get<std::size_t>();
					if (existing == hash)
					{
						return;
					}
					if (existing > hash)
					{
						position = i;
						break;
					}
				}

				table.insert(table.begin() + position, std::move(entry));
			}

			void add_constant(ordered_json& table, const char* name, const unsigned int hash, const float x, const float y, const float z, const float w)
			{
				ordered_json entry;
				entry["name"] = name;
				entry["nameHash"] = hash;
				entry["literal"] = { x, y, z, w };
				insert_sorted(table, "nameHash", hash, std::move(entry));
			}

			void add_image(ordered_json& table, const required_slot& slot)
			{
				ordered_json entry;
				entry["image"] = slot.image;
				entry["semantic"] = slot.semantic;
				entry["samplerState"] = slot.sampler;
				entry["lastCharacter"] = slot.last_character;
				entry["firstCharacter"] = slot.first_character;
				entry["typeHash"] = slot.hash;
				insert_sorted(table, "typeHash", slot.hash, std::move(entry));
			}
		}

		void dump(zonetool::iw5::Material* asset)
		{
			if (!asset)
			{
				return;
			}

			const std::string name = asset->info.name.get();
			const auto has_ct = has_color_tint(asset);

			const auto path = "materials\\"s + clean_name(name) + ".json"s;
			auto file = zonetool::filesystem::file(path);

			ordered_json matdata;

			std::string techset;
			std::string iw7_techset;
			if (const auto source_techset = asset->techniqueSet.get())
			{
				if (!source_techset->name.get())
				{
					ZONETOOL_ERROR("material %s has a techset without a name (%p)", name.data(), source_techset);
					return;
				}

				techset = source_techset->name.get();

				bool result = false;
				iw7_techset = get_IW7_techset(techset, name, &result, false, has_ct);
				matdata["techniqueSet->original"] = techset;
				matdata["techniqueSet->name"] = iw7_techset;
			}

			auto game_flags = asset->info.gameFlags;
			if (game_flags & 0x40)
			{
				game_flags = static_cast<decltype(game_flags)>((game_flags & ~0x40) | 0x80);
			}

			matdata["gameFlags"] = game_flags;
			matdata["unkFlags"] = 0;
			matdata["sortKey"] = get_IW7_sortkey(asset->info.sortKey, name, iw7_techset);
			matdata["renderFlags"] = get_render_flags_by_techset(iw7_techset);

			matdata["textureAtlasRowCount"] = asset->info.textureAtlasRowCount;
			matdata["textureAtlasColumnCount"] = asset->info.textureAtlasColumnCount;
			matdata["textureAtlasFrameBlend"] = 0;
			matdata["textureAtlasAsArray"] = 0;
			matdata["surfaceTypeBits"] = asset->info.surfaceTypeBits;

			auto state_flags = asset->stateFlags;
			if (iw7_techset.compare(0, 2, "2d") != 0)
			{
				state_flags = static_cast<decltype(state_flags)>(state_flags & ~0x40);
			}

			matdata["stateFlags"] = state_flags;
			matdata["cameraRegion"] = get_IW7_camera_region(asset->cameraRegion, name, iw7_techset);
			matdata["materialType"] = get_material_type_from_techset(iw7_techset);
			matdata["assetFlags"] = 0;

			ordered_json constant_table = ordered_json::array();
			const auto constants = asset->constantTable.get();
			for (auto i = 0; i < asset->constantCount && techset != "2d"; i++)
			{
				std::string constant_name(constants[i].name, strnlen(constants[i].name, sizeof(constants[i].name)));
				if (constants[i].nameHash == 1033475292)
				{
					continue;
				}

				ordered_json entry;
				entry["name"] = constant_name;
				entry["nameHash"] = constants[i].nameHash;
				entry["literal"] = { constants[i].literal[0], constants[i].literal[1], constants[i].literal[2], constants[i].literal[3] };
				constant_table.push_back(entry);
			}

			if (iw7_techset.find("s0") != std::string::npos)
			{
				add_constant(constant_table, "reflectionRa", 3344177073u, 8096.0f, 0.0f, 0.0f, 0.0f);
			}
			if (iw7_techset.find("_lin") != std::string::npos && !iw7_techset.starts_with("particle_cloud_"))
			{
				add_constant(constant_table, "textureAtlas", 1128936273u,
					static_cast<float>(asset->info.textureAtlasColumnCount), static_cast<float>(asset->info.textureAtlasRowCount), 1.0f, 1.0f);
			}
			if (iw7_techset.find("_evc") != std::string::npos)
			{
				add_constant(constant_table, "colorIntensi", 3670886770u, 1.0f, 0.0f, 0.0f, 0.0f);
			}
			if (iw7_techset.find("_ct") != std::string::npos || iw7_techset == "2d" || iw7_techset == "wc_unlit_blend_lin_ndw")
			{
				add_constant(constant_table, "colorTint", 3054254906u, 1.0f, 1.0f, 1.0f, 1.0f);
			}

			matdata["constantTable"] = constant_table;

			ordered_json images = ordered_json::array();
			const auto textures = asset->textureTable.get();
			for (auto i = 0; i < asset->textureCount; i++)
			{
				const auto& texture = textures[i];

				std::string image_name;
				if (texture.semantic == 11)
				{
					const auto water = texture.u.water.get();
					if (water && water->image.get() && water->image.get()->name.get())
					{
						image_name = water->image.get()->name.get();
					}
				}
				else if (texture.u.image.get() && texture.u.image.get()->name.get())
				{
					image_name = texture.u.image.get()->name.get();
				}

				std::uint8_t semantic;
				switch (texture.nameHash)
				{
				case slot_hash_detail: semantic = iw7_types::TS_DETAIL_MAP; break;
				case slot_hash_colour: semantic = iw7_types::TS_COLOR_MAP; break;
				case slot_hash_normal: semantic = iw7_types::TS_NORMAL_MAP; break;
				case slot_hash_spec: semantic = iw7_types::TS_SPECULAR_MAP; break;
				default: semantic = convert_semantic(texture.semantic); break;
				}

				ordered_json image;
				image["image"] = image_name;
				image["semantic"] = semantic;
				image["samplerState"] = texture.samplerState == 11 ? 19 : texture.samplerState;
				image["lastCharacter"] = texture.nameEnd;
				image["firstCharacter"] = texture.nameStart;
				image["typeHash"] = texture.nameHash;
				images.push_back(image);
			}

			const auto required = required_slots.find(iw7_techset);
			if (required != required_slots.end())
			{
				for (const auto& slot : required->second)
				{
					add_image(images, slot);
				}
			}
			else if (iw7_techset.find("n0") != std::string::npos)
			{
				add_image(images, slot_normal);
			}

			matdata["textureTable"] = images;

			const auto json_text = matdata.dump(4, ' ', true, nlohmann::detail::error_handler_t::replace);

			file.open("wb");
			file.write(json_text);
			file.close();
		}
	}
}
