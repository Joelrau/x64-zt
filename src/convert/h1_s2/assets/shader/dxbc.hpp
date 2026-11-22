#pragma once

#include <formats/dxbc.hpp>

#include "shader.hpp"

namespace zonetool::h1::converter::s2::shader
{
	using dxbc::float4_type;
	using dxbc::uint4_type;
	using dxbc::signature_element;

	using dxbc::relocate_constant_row;
	using dxbc::redirect_input;
	using dxbc::is_declaration;
	using dxbc::is_input_declaration;
	using dxbc::is_destination;
	using dxbc::is_resource;
	using dxbc::is_sample;
	using dxbc::read_components;
	using dxbc::set_resource_dimension;
	using dxbc::temp_component;
	using dxbc::uint_literal;
	using dxbc::uint_literals;
	using dxbc::find_signature_element;
	using dxbc::declares_resource;
	using dxbc::declares_constant_buffer;
	using dxbc::declares_sampler;
	using dxbc::as_temp;
	using dxbc::writes_operand;
	using dxbc::negate;
	using dxbc::is_input_ps_declaration;
	using dxbc::is_resource_declaration;
	using dxbc::check_single_return;
	using dxbc::check_writes_only_o0;
	using dxbc::relocate_resource_slots;
}
