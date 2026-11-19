#include <std_include.hpp>
#include "menudef.hpp"

namespace zonetool::h1
{
	menuDef_t* menu_def::parse(std::string name, zone_memory* mem)
	{
		return nullptr;
	}

	void menu_def::init(const std::string& name, zone_memory* mem)
	{
		this->name_ = name;

		if (this->referenced())
		{
			this->asset_ = mem->allocate<typename std::remove_reference<decltype(*this->asset_)>::type>();
			this->asset_->window.name = mem->duplicate_string(name);
			return;
		}

		this->asset_ = parse(name, mem);
		if (!this->asset_)
		{
			this->asset_ = db_find_x_asset_header_safe(XAssetType(this->type()), this->name_.data()).menu;
		}
	}

	void menu_def::init(void* ptr, zone_memory* mem)
	{
		this->asset_ = reinterpret_cast<menuDef_t*>(ptr);
		if (!this->asset_)
		{
			this->asset_ = db_find_x_asset_header_safe(XAssetType(this->type()), this->name().data()).menu;
			if (!this->asset_)
			{
				return;
			}
		}
		this->name_ = this->asset_->window.name;
	}

	void menu_def::prepare(zone_buffer* buf, zone_memory* mem)
	{
	}

	void menu_def::load_depending(zone_base* zone)
	{
		auto* data = this->asset_;
		if (data->window.background)
		{
			zone->add_asset_of_type(ASSET_TYPE_MATERIAL, data->window.background->name);
		}
		if (data->data->soundName)
		{
			zone->add_asset_of_type(ASSET_TYPE_SOUND, data->data->soundName);
		}
		if (data->items)
		{
			for (int i = 0; i < data->itemCount; i++)
			{
				if (data->items[i]->window.background)
				{
					zone->add_asset_of_type(ASSET_TYPE_MATERIAL, data->items[i]->window.background->name);
				}
				if (data->items[i]->focusSound)
				{
					zone->add_asset_of_type(ASSET_TYPE_SOUND, data->items[i]->focusSound->name);
				}
				if (data->items[i]->dataType == 6)
				{
					if (data->items[i]->typeData.listBox->selectIcon)
					{
						zone->add_asset_of_type(ASSET_TYPE_MATERIAL, data->items[i]->typeData.listBox->selectIcon->name);
					}
				}
			}
		}
	}

	std::string menu_def::name()
	{
		return this->name_;
	}

	std::int32_t menu_def::type()
	{
		return ASSET_TYPE_MENU;
	}

	void menu_def::write_menu_supporting_data(zone_base* zone, zone_buffer* buf, ExpressionSupportingData* data)
	{
		auto* destSupData = buf->write(data);
		if (data->uifunctions.functions)
		{
			buf->align(7);
			auto** destFuncs = buf->write(data->uifunctions.functions,
				data->uifunctions.totalFunctions);
			auto** dataFuncs = data->uifunctions.functions;
			for (int i = 0; i < data->uifunctions.totalFunctions; i++)
			{
				if (dataFuncs[i])
				{
					buf->align(3);
					write_menu_statement(zone, buf, dataFuncs[i]);
					buf->clear_pointer(&destFuncs[i]);
				}
			}
			buf->clear_pointer(&destSupData->uifunctions.functions);
		}
		if (data->staticDvarList.staticDvars)
		{
			buf->align(7);
			auto** destStaticDvars = buf->write(data->staticDvarList.staticDvars,
				data->staticDvarList.numStaticDvars);
			auto** dataStaticDvars = data->staticDvarList.staticDvars;
			for (int i = 0; i < data->staticDvarList.numStaticDvars; i++)
			{
				if (dataStaticDvars[i])
				{
					buf->align(3);
					auto* destVar = buf->write(dataStaticDvars[i]);
					if (dataStaticDvars[i]->dvarName)
					{
						destVar->dvarName = buf->write_str(dataStaticDvars[i]->dvarName);
					}
					buf->clear_pointer(&destStaticDvars[i]);
				}
			}
			buf->clear_pointer(&destSupData->staticDvarList.staticDvars);
		}
		if (data->uiStrings.strings)
		{
			buf->align(7);
			auto* destStrings = buf->write(data->uiStrings.strings,
				data->uiStrings.totalStrings);
			for (int i = 0; i < data->uiStrings.totalStrings; i++)
			{
				if (data->uiStrings.strings[i])
				{
					destStrings[i] = buf->write_str(data->uiStrings.strings[i]);
				}
			}
			buf->clear_pointer(&destSupData->uiStrings.strings);
		}
	}

	void menu_def::write_menu_statement(zone_base* zone, zone_buffer* buf, Statement_s* data)
	{
		auto* dest = buf->write(data);
		if (data->entries)
		{
			buf->align(3);
			auto* destEntries = buf->write(data->entries, data->numEntries);
			for (int i = 0; i < data->numEntries; i++)
			{
				if (data->entries[i].type)
				{
					auto* operand = &data->entries[i].data.operand;
					if (operand->dataType == VAL_STRING)
					{
						destEntries[i].data.operand.internals.stringVal.string =
							buf->write_str(operand->internals.stringVal.string);
					}
					else if (operand->dataType == VAL_FUNCTION)
					{
						buf->align(3);
						write_menu_statement(zone, buf, operand->internals.function);
						buf->clear_pointer(&destEntries[i].data.operand.internals.function);
					}
				}
			}
			buf->clear_pointer(&dest->entries);
		}
		if (data->supportingData)
		{
			buf->align(3);
			write_menu_supporting_data(zone, buf, data->supportingData);
			buf->clear_pointer(&dest->supportingData);
		}
	}

	void menu_def::write_menu_eventhandlerset(zone_base* zone, zone_buffer* buf, MenuEventHandlerSet* data)
	{
		auto* dest = buf->write(data);
		if (data->eventHandlers)
		{
			buf->align(7);
			auto* destHandlers = buf->write(data->eventHandlers, data->eventHandlerCount);
			for (int i = 0; i < data->eventHandlerCount; i++)
			{
				if (data->eventHandlers[i])
				{
					buf->align(3);
					auto* destHandler = buf->write(data->eventHandlers[i]);
					auto* dataHandler = data->eventHandlers[i];
					switch (dataHandler->eventType)
					{
					case EVENT_UNCONDITIONAL:
						destHandler->eventData.unconditionalScript = buf->write_str(dataHandler->eventData.unconditionalScript);
						break;
					case EVENT_IF:
						if (dataHandler->eventData.conditionalScript)
						{
							buf->align(3);
							auto* destScript = buf->write(dataHandler->eventData.conditionalScript);
							if (dataHandler->eventData.conditionalScript->eventExpression)
							{
								buf->align(3);
								write_menu_statement(zone, buf, dataHandler->eventData.conditionalScript->eventExpression);
								buf->clear_pointer(&destScript->eventExpression);
							}
							if (dataHandler->eventData.conditionalScript->eventHandlerSet)
							{
								buf->align(3);
								write_menu_eventhandlerset(zone, buf, dataHandler->eventData.conditionalScript->eventHandlerSet);
								buf->clear_pointer(&destScript->eventHandlerSet);
							}
							buf->clear_pointer(&destHandler->eventData.conditionalScript);
						}
						break;
					case EVENT_ELSE:
						if (dataHandler->eventData.elseScript)
						{
							buf->align(3);
							write_menu_eventhandlerset(zone, buf, dataHandler->eventData.elseScript);
							buf->clear_pointer(&destHandler->eventData.elseScript);
						}
						break;
					case EVENT_SET_LOCAL_VAR_BOOL:
					case EVENT_SET_LOCAL_VAR_INT:
					case EVENT_SET_LOCAL_VAR_FLOAT:
					case EVENT_SET_LOCAL_VAR_STRING:
						if (dataHandler->eventData.setLocalVarData)
						{
							buf->align(3);
							auto* destLVD = buf->write(dataHandler->eventData.setLocalVarData);
							if (dataHandler->eventData.setLocalVarData->localVarName)
							{
								destLVD->localVarName = buf->write_str(dataHandler->eventData.setLocalVarData->localVarName);
							}
							if (dataHandler->eventData.setLocalVarData->expression)
							{
								buf->align(3);
								write_menu_statement(zone, buf, dataHandler->eventData.setLocalVarData->expression);
								buf->clear_pointer(&destLVD->expression);
							}

							buf->clear_pointer(&destHandler->eventData.setLocalVarData);
						}
						break;
					}
					buf->clear_pointer(&destHandlers[i]);
				}
			}
			buf->clear_pointer(&dest->eventHandlers);
		}
	}

	void menu_def::write_menu_itemkeyhandler(zone_base* zone, zone_buffer* buf, ItemKeyHandler* data)
	{
		auto* dest = buf->write(data);
		if (data->action)
		{
			buf->align(3);
			write_menu_eventhandlerset(zone, buf, data->action);
			buf->clear_pointer(&dest->action);
		}
		if (!data->next)
			return;
		buf->align(3);
		write_menu_itemkeyhandler(zone, buf, data->next);
		buf->clear_pointer(&dest->next);
	}

	void menu_def::write_menu_data(zone_base* zone, zone_buffer* buf, menuData_t* data)
	{
		auto* dest = buf->write(data);
		if (data->expressionData)
		{
			buf->align(3);
			write_menu_supporting_data(zone, buf, data->expressionData);
			buf->clear_pointer(&dest->expressionData);
		}
		if (data->onOpen)
		{
			buf->align(3);
			write_menu_eventhandlerset(zone, buf, data->onOpen);
			buf->clear_pointer(&dest->onOpen);
		}
		if (data->onClose)
		{
			buf->align(3);
			write_menu_eventhandlerset(zone, buf, data->onClose);
			buf->clear_pointer(&dest->onClose);
		}
		if (data->onCloseRequest)
		{
			buf->align(3);
			write_menu_eventhandlerset(zone, buf, data->onCloseRequest);
			buf->clear_pointer(&dest->onCloseRequest);
		}
		if (data->onESC)
		{
			buf->align(3);
			write_menu_eventhandlerset(zone, buf, data->onESC);
			buf->clear_pointer(&dest->onESC);
		}
		if (data->onFocusDueToClose)
		{
			buf->align(3);
			write_menu_eventhandlerset(zone, buf, data->onFocusDueToClose);
			buf->clear_pointer(&dest->onFocusDueToClose);
		}
		if (data->onKey)
		{
			buf->align(3);
			write_menu_itemkeyhandler(zone, buf, data->onKey);
			buf->clear_pointer(&dest->onKey);
		}
		if (data->visibleExp)
		{
			buf->align(3);
			write_menu_statement(zone, buf, data->visibleExp);
			buf->clear_pointer(&dest->visibleExp);
		}
		if (data->allowedBinding)
		{
			dest->allowedBinding = buf->write_str(data->allowedBinding);
		}
		if (data->soundName)
		{
			dest->soundName = buf->write_str(data->soundName);
		}
		if (data->rectXExp)
		{
			buf->align(3);
			write_menu_statement(zone, buf, data->rectXExp);
			buf->clear_pointer(&dest->rectXExp);
		}
		if (data->rectYExp)
		{
			buf->align(3);
			write_menu_statement(zone, buf, data->rectYExp);
			buf->clear_pointer(&dest->rectYExp);
		}
		if (data->rectWExp)
		{
			buf->align(3);
			write_menu_statement(zone, buf, data->rectWExp);
			buf->clear_pointer(&dest->rectWExp);
		}
		if (data->rectHExp)
		{
			buf->align(3);
			write_menu_statement(zone, buf, data->rectHExp);
			buf->clear_pointer(&dest->rectHExp);
		}
		if (data->openSoundExp)
		{
			buf->align(3);
			write_menu_statement(zone, buf, data->openSoundExp);
			buf->clear_pointer(&dest->openSoundExp);
		}
		if (data->closeSoundExp)
		{
			buf->align(3);
			write_menu_statement(zone, buf, data->closeSoundExp);
			buf->clear_pointer(&dest->closeSoundExp);
		}
		if (data->soundLoopExp)
		{
			buf->align(3);
			write_menu_statement(zone, buf, data->soundLoopExp);
			buf->clear_pointer(&dest->soundLoopExp);
		}
	}

	void menu_def::write_menu_window(zone_base* zone, zone_buffer* buf, windowDef_t* data, windowDef_t* dest)
	{
		if (data->name)
		{
			dest->name = buf->write_str(data->name);
		}
		if (data->group)
		{
			dest->group = buf->write_str(data->group);
		}
		if (data->background)
		{
			dest->background = reinterpret_cast<Material*>(zone->get_asset_pointer(ASSET_TYPE_MATERIAL, data->background->name));
		}
	}

	void menu_def::write_menu_item_defdata(zone_base* zone, zone_buffer* buf, itemDefData_t* data, int type)
	{
		switch (type)
		{
		case 6:
			if (data->listBox)
			{
				auto* dest_listBox = buf->write(data->listBox);
				if (data->listBox->onDoubleClick)
				{
					buf->align(3);
					write_menu_eventhandlerset(zone, buf, data->listBox->onDoubleClick);
					buf->clear_pointer(&dest_listBox->onDoubleClick);
				}
				if (data->listBox->selectIcon)
				{
					dest_listBox->selectIcon = reinterpret_cast<Material*>(
						zone->get_asset_pointer(ASSET_TYPE_MATERIAL, data->listBox->selectIcon->name));
				}
				if (data->listBox->elementHeightExp)
				{
					buf->align(3);
					write_menu_statement(zone, buf, data->listBox->elementHeightExp);
					buf->clear_pointer(&dest_listBox->elementHeightExp);
				}
			}
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
			if (data->editField)
			{
				buf->align(3);
				/*auto* dest_editField = */buf->write(data->editField);
				//buf->clear_pointer(&dest->editField);
			}
			break;
		case 12:
			if (data->multi)
			{
				buf->align(3);
				auto* dest_multi = buf->write(data->multi);
				for (int i = 0; i < 32; i++)
				{
					if (data->multi->dvarList[i])
					{
						dest_multi->dvarList[i] = buf->write_str(data->multi->dvarList[i]);
					}
				}
				for (int i = 0; i < 32; i++)
				{
					if (data->multi->dvarStr[i])
					{
						dest_multi->dvarStr[i] = buf->write_str(data->multi->dvarStr[i]);
					}
				}
				//buf->clear_pointer(&dest->multi);
			}
			break;
		case 13:
			if (data->enumDvarName)
			{
				buf->write_str(data->enumDvarName);
			}
			break;
		case 20:
			if (data->ticker)
			{
				buf->align(3);
				/*auto* dest_ticker = */buf->write(data->ticker);
				//buf->clear_pointer(&dest->ticker);
			}
			break;
		case 21:
			if (data->scroll)
			{
				buf->align(3);
				/*auto* dest_scroll = */buf->write(data->scroll);
				//buf->clear_pointer(&dest->scroll);
			}
			break;
		}
	}

	void menu_def::write_menu_item_floatexpressions(zone_base* zone, zone_buffer* buf, ItemFloatExpression* data, int count)
	{
		auto* dest = buf->write(data, count);
		for (int i = 0; i < count; i++)
		{
			if (data[i].expression)
			{
				buf->align(3);
				write_menu_statement(zone, buf, data[i].expression);
				buf->clear_pointer(&dest[i].expression);
			}
		}
	}

	void menu_def::write_menu_item(zone_base* zone, zone_buffer* buf, itemDef_t* data)
	{
		auto* dest = buf->write(data);
		write_menu_window(zone, buf, &data->window, &dest->window);
		if (data->text)
		{
			dest->text = buf->write_str(data->text);
		}
		if (data->mouseEnterText)
		{
			buf->align(3);
			write_menu_eventhandlerset(zone, buf, data->mouseEnterText);
			buf->clear_pointer(&dest->mouseEnterText);
		}
		if (data->mouseExitText)
		{
			buf->align(3);
			write_menu_eventhandlerset(zone, buf, data->mouseExitText);
			buf->clear_pointer(&dest->mouseExitText);
		}
		if (data->mouseEnter)
		{
			buf->align(3);
			write_menu_eventhandlerset(zone, buf, data->mouseEnter);
			buf->clear_pointer(&dest->mouseEnter);
		}
		if (data->mouseExit)
		{
			buf->align(3);
			write_menu_eventhandlerset(zone, buf, data->mouseExit);
			buf->clear_pointer(&dest->mouseExit);
		}
		if (data->action)
		{
			buf->align(3);
			write_menu_eventhandlerset(zone, buf, data->action);
			buf->clear_pointer(&dest->action);
		}
		if (data->accept)
		{
			buf->align(3);
			write_menu_eventhandlerset(zone, buf, data->accept);
			buf->clear_pointer(&dest->accept);
		}
		if (data->onFocus)
		{
			buf->align(3);
			write_menu_eventhandlerset(zone, buf, data->onFocus);
			buf->clear_pointer(&dest->onFocus);
		}
		if (data->hasFocus)
		{
			buf->align(3);
			write_menu_eventhandlerset(zone, buf, data->hasFocus);
			buf->clear_pointer(&dest->hasFocus);
		}
		if (data->leaveFocus)
		{
			buf->align(3);
			write_menu_eventhandlerset(zone, buf, data->leaveFocus);
			buf->clear_pointer(&dest->leaveFocus);
		}
		if (data->dvar)
		{
			dest->dvar = buf->write_str(data->dvar);
		}
		if (data->dvarTest)
		{
			dest->dvarTest = buf->write_str(data->dvarTest);
		}
		if (data->onKey)
		{
			buf->align(3);
			write_menu_itemkeyhandler(zone, buf, data->onKey);
			buf->clear_pointer(&dest->onKey);
		}
		if (data->enableDvar)
		{
			dest->enableDvar = buf->write_str(data->enableDvar);
		}
		if (data->localVar)
		{
			dest->localVar = buf->write_str(data->localVar);
		}
		if (data->focusSound)
		{
			dest->focusSound = reinterpret_cast<snd_alias_list_t*>(zone->get_asset_pointer(ASSET_TYPE_SOUND, data->focusSound->name));
		}
		if (data->typeData.data)
		{
			buf->align(3);
			write_menu_item_defdata(zone, buf, &data->typeData, data->type);
			buf->clear_pointer(&dest->typeData.data);
		}
		if (data->floatExpressions)
		{
			buf->align(3);
			write_menu_item_floatexpressions(zone, buf, data->floatExpressions, data->floatExpressionCount);
			buf->clear_pointer(&dest->floatExpressions);
		}
		if (data->visibleExp)
		{
			buf->align(3);
			write_menu_statement(zone, buf, data->visibleExp);
			buf->clear_pointer(&dest->visibleExp);
		}
		if (data->disabledExp)
		{
			buf->align(3);
			write_menu_statement(zone, buf, data->disabledExp);
			buf->clear_pointer(&dest->disabledExp);
		}
		if (data->textExp)
		{
			buf->align(3);
			write_menu_statement(zone, buf, data->textExp);
			buf->clear_pointer(&dest->textExp);
		}
		if (data->materialExp)
		{
			buf->align(3);
			write_menu_statement(zone, buf, data->materialExp);
			buf->clear_pointer(&dest->materialExp);
		}
		if (data->textAlignYExp)
		{
			buf->align(3);
			write_menu_statement(zone, buf, data->textAlignYExp);
			buf->clear_pointer(&dest->textAlignYExp);
		}
	}

	namespace
	{
		void read_statement(zone_reader& reader, Statement_s*& field);
		void read_event_handler_set(zone_reader& reader, MenuEventHandlerSet*& field);

		void read_supporting_data(zone_reader& reader, ExpressionSupportingData*& field)
		{
			const auto data = reader.read_single(field, 3);
			if (!data)
			{
				return;
			}

			if (const auto functions = reader.read_array(data->uifunctions.functions, 7, data->uifunctions.totalFunctions))
			{
				for (auto i = 0; i < data->uifunctions.totalFunctions; i++)
				{
					read_statement(reader, functions[i]);
				}
			}

			if (const auto dvars = reader.read_array(data->staticDvarList.staticDvars, 7, data->staticDvarList.numStaticDvars))
			{
				for (auto i = 0; i < data->staticDvarList.numStaticDvars; i++)
				{
					if (const auto dvar = reader.read_single(dvars[i], 3))
					{
						reader.read_string(dvar->dvarName);
					}
				}
			}

			if (const auto strings = reader.read_array(data->uiStrings.strings, 7, data->uiStrings.totalStrings))
			{
				for (auto i = 0; i < data->uiStrings.totalStrings; i++)
				{
					reader.read_string(strings[i]);
				}
			}
		}

		void read_statement(zone_reader& reader, Statement_s*& field)
		{
			const auto statement = reader.read_single(field, 3);
			if (!statement)
			{
				return;
			}

			if (const auto entries = reader.read_array(statement->entries, 3, statement->numEntries))
			{
				for (auto i = 0; i < statement->numEntries; i++)
				{
					if (!entries[i].type)
					{
						continue;
					}

					auto& operand = entries[i].data.operand;
					if (operand.dataType == VAL_STRING)
					{
						reader.read_string(operand.internals.stringVal.string);
					}
					else if (operand.dataType == VAL_FUNCTION)
					{
						read_statement(reader, operand.internals.function);
					}
				}
			}

			read_supporting_data(reader, statement->supportingData);
		}

		void read_event_handler(zone_reader& reader, MenuEventHandler*& field)
		{
			const auto handler = reader.read_single(field, 3);
			if (!handler)
			{
				return;
			}

			auto& data = handler->eventData;
			switch (handler->eventType)
			{
			case EVENT_UNCONDITIONAL:
				reader.read_string(data.unconditionalScript);
				break;
			case EVENT_IF:
				if (const auto script = reader.read_single(data.conditionalScript, 3))
				{
					read_statement(reader, script->eventExpression);
					read_event_handler_set(reader, script->eventHandlerSet);
				}
				break;
			case EVENT_ELSE:
				read_event_handler_set(reader, data.elseScript);
				break;
			case EVENT_SET_LOCAL_VAR_BOOL:
			case EVENT_SET_LOCAL_VAR_INT:
			case EVENT_SET_LOCAL_VAR_FLOAT:
			case EVENT_SET_LOCAL_VAR_STRING:
				if (const auto local_var = reader.read_single(data.setLocalVarData, 3))
				{
					reader.read_string(local_var->localVarName);
					read_statement(reader, local_var->expression);
				}
				break;
			default:
				break;
			}
		}

		void read_event_handler_set(zone_reader& reader, MenuEventHandlerSet*& field)
		{
			const auto set = reader.read_single(field, 3);
			if (!set)
			{
				return;
			}

			if (const auto handlers = reader.read_array(set->eventHandlers, 7, set->eventHandlerCount))
			{
				for (auto i = 0; i < set->eventHandlerCount; i++)
				{
					read_event_handler(reader, handlers[i]);
				}
			}
		}

		void read_key_handler(zone_reader& reader, ItemKeyHandler*& field)
		{
			if (const auto handler = reader.read_single(field, 3))
			{
				read_event_handler_set(reader, handler->action);
				read_key_handler(reader, handler->next);
			}
		}

		void read_menu_data(zone_reader& reader, menuData_t*& field)
		{
			const auto data = reader.read_single(field, 3);
			if (!data)
			{
				return;
			}

			read_supporting_data(reader, data->expressionData);
			read_event_handler_set(reader, data->onOpen);
			read_event_handler_set(reader, data->onClose);
			read_event_handler_set(reader, data->onCloseRequest);
			read_event_handler_set(reader, data->onESC);
			read_event_handler_set(reader, data->onFocusDueToClose);
			read_key_handler(reader, data->onKey);
			read_statement(reader, data->visibleExp);
			reader.read_string(data->allowedBinding);
			reader.read_string(data->soundName);
			read_statement(reader, data->rectXExp);
			read_statement(reader, data->rectYExp);
			read_statement(reader, data->rectWExp);
			read_statement(reader, data->rectHExp);
			read_statement(reader, data->openSoundExp);
			read_statement(reader, data->closeSoundExp);
			read_statement(reader, data->soundLoopExp);
		}

		void read_window(zone_reader& reader, windowDef_t* window)
		{
			reader.read_string(window->name);
			reader.read_string(window->group);
			reader.read_asset(ASSET_TYPE_MATERIAL, window->background);
		}

		bool is_edit_field_type(const int type)
		{
			switch (type)
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
				return true;
			default:
				return false;
			}
		}

		void read_type_data(zone_reader& reader, itemDefData_t* data, const int type)
		{
			if (type == 6)
			{
				if (const auto list_box = reader.read_single(data->listBox, 3))
				{
					read_event_handler_set(reader, list_box->onDoubleClick);
					reader.read_asset(ASSET_TYPE_MATERIAL, list_box->selectIcon);
					read_statement(reader, list_box->elementHeightExp);
				}
			}
			else if (is_edit_field_type(type))
			{
				reader.read_single(data->editField, 3);
			}
			else if (type == 12)
			{
				if (const auto multi = reader.read_single(data->multi, 3))
				{
					for (auto& dvar : multi->dvarList)
					{
						reader.read_string(dvar);
					}

					for (auto& dvar : multi->dvarStr)
					{
						reader.read_string(dvar);
					}
				}
			}
			else if (type == 13)
			{
				reader.read_string(data->enumDvarName);
			}
			else if (type == 20)
			{
				reader.read_single(data->ticker, 3);
			}
			else if (type == 21)
			{
				reader.read_single(data->scroll, 3);
			}
		}

		void read_item(zone_reader& reader, itemDef_t*& field)
		{
			const auto item = reader.read_single(field, 3);
			if (!item)
			{
				return;
			}

			read_window(reader, &item->window);
			reader.read_string(item->text);
			read_event_handler_set(reader, item->mouseEnterText);
			read_event_handler_set(reader, item->mouseExitText);
			read_event_handler_set(reader, item->mouseEnter);
			read_event_handler_set(reader, item->mouseExit);
			read_event_handler_set(reader, item->action);
			read_event_handler_set(reader, item->accept);
			read_event_handler_set(reader, item->onFocus);
			read_event_handler_set(reader, item->hasFocus);
			read_event_handler_set(reader, item->leaveFocus);
			reader.read_string(item->dvar);
			reader.read_string(item->dvarTest);
			read_key_handler(reader, item->onKey);
			reader.read_string(item->enableDvar);
			reader.read_string(item->localVar);
			reader.read_asset(ASSET_TYPE_SOUND, item->focusSound);
			read_type_data(reader, &item->typeData, item->type);

			if (const auto expressions = reader.read_array(item->floatExpressions, 3, item->floatExpressionCount))
			{
				for (auto i = 0; i < item->floatExpressionCount; i++)
				{
					read_statement(reader, expressions[i].expression);
				}
			}

			read_statement(reader, item->visibleExp);
			read_statement(reader, item->disabledExp);
			read_statement(reader, item->textExp);
			read_statement(reader, item->materialExp);
			read_statement(reader, item->textAlignYExp);
		}
	}

	void menu_def::read(zone_reader& reader, menuDef_t* asset)
	{
		reader.push_stream(XFILE_BLOCK_VIRTUAL);
		read_menu_data(reader, asset->data);
		read_window(reader, &asset->window);

		if (const auto items = reader.read_array(asset->items, 7, asset->itemCount))
		{
			for (auto i = 0; i < asset->itemCount; i++)
			{
				read_item(reader, items[i]);
			}
		}

		reader.pop_stream();
	}


	void menu_def::write(zone_base* zone, zone_buffer* buf)
	{
		auto* data = this->asset_;
		auto* dest = buf->write(data);

		buf->push_stream(XFILE_BLOCK_VIRTUAL);

		// menudata
		if (data->data)
		{
			buf->align(3);
			write_menu_data(zone, buf, data->data);
			buf->clear_pointer(&dest->data);
		}

		write_menu_window(zone, buf, &data->window, &dest->window);

		// items
		if (data->items)
		{
			buf->align(7);
			auto* destItems = buf->write(data->items, data->itemCount);
			for (int i = 0; i < data->itemCount; i++)
			{
				if (data->items[i])
				{
					buf->align(3);
					write_menu_item(zone, buf, data->items[i]);
					buf->clear_pointer(&destItems[i]);
				}
			}
			buf->clear_pointer(&dest->items);
		}

		buf->pop_stream();
	}

	void menu_def::dump(menuDef_t* asset)
	{
	}
}
