#include <std_include.hpp>
#include "menudef.hpp"

namespace zonetool::h1
{
	menuDef_t* IMenuDef::parse(std::string name, ZoneMemory* mem)
	{
		return nullptr;
	}

	void IMenuDef::init(const std::string& name, ZoneMemory* mem)
	{
		this->name_ = name;

		if (this->referenced())
		{
			this->asset_ = mem->Alloc<typename std::remove_reference<decltype(*this->asset_)>::type>();
			this->asset_->window.name = mem->StrDup(name);
			return;
		}

		this->asset_ = parse(name, mem);
		if (!this->asset_)
		{
			this->asset_ = DB_FindXAssetHeader_Safe(XAssetType(this->type()), this->name_.data()).menu;
		}
	}

	void IMenuDef::init(void* ptr, ZoneMemory* mem)
	{
		this->asset_ = reinterpret_cast<menuDef_t*>(ptr);
		if (!this->asset_)
		{
			this->asset_ = DB_FindXAssetHeader_Safe(XAssetType(this->type()), this->name().data()).menu;
		}
		this->name_ = this->asset_->window.name;
	}

	void IMenuDef::prepare(ZoneBuffer* buf, ZoneMemory* mem)
	{
	}

	void IMenuDef::load_depending(IZone* zone)
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

	std::string IMenuDef::name()
	{
		return this->name_;
	}

	std::int32_t IMenuDef::type()
	{
		return ASSET_TYPE_MENU;
	}

	void IMenuDef::write_menu_supporting_data(IZone* zone, ZoneBuffer* buf, ExpressionSupportingData* data)
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
					ZoneBuffer::clear_pointer(&destFuncs[i]);
				}
			}
			ZoneBuffer::clear_pointer(&destSupData->uifunctions.functions);
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
					ZoneBuffer::clear_pointer(&destStaticDvars[i]);
				}
			}
			ZoneBuffer::clear_pointer(&destSupData->staticDvarList.staticDvars);
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
			ZoneBuffer::clear_pointer(&destSupData->uiStrings.strings);
		}
	}

	void IMenuDef::write_menu_statement(IZone* zone, ZoneBuffer* buf, Statement_s* data)
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
						ZoneBuffer::clear_pointer(&destEntries[i].data.operand.internals.function);
					}
				}
			}
			ZoneBuffer::clear_pointer(&dest->entries);
		}
		if (data->supportingData)
		{
			buf->align(3);
			write_menu_supporting_data(zone, buf, data->supportingData);
			ZoneBuffer::clear_pointer(&dest->supportingData);
		}
	}

	void IMenuDef::write_menu_eventhandlerset(IZone* zone, ZoneBuffer* buf, MenuEventHandlerSet* data)
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
								ZoneBuffer::clear_pointer(&destScript->eventExpression);
							}
							if (dataHandler->eventData.conditionalScript->eventHandlerSet)
							{
								buf->align(3);
								write_menu_eventhandlerset(zone, buf, dataHandler->eventData.conditionalScript->eventHandlerSet);
								ZoneBuffer::clear_pointer(&destScript->eventHandlerSet);
							}
							ZoneBuffer::clear_pointer(&destHandler->eventData.conditionalScript);
						}
						break;
					case EVENT_ELSE:
						if (dataHandler->eventData.elseScript)
						{
							buf->align(3);
							write_menu_eventhandlerset(zone, buf, dataHandler->eventData.elseScript);
							ZoneBuffer::clear_pointer(&destHandler->eventData.elseScript);
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
								ZoneBuffer::clear_pointer(&destLVD->expression);
							}

							ZoneBuffer::clear_pointer(&destHandler->eventData.setLocalVarData);
						}
						break;
					}
					ZoneBuffer::clear_pointer(&destHandlers[i]);
				}
			}
			ZoneBuffer::clear_pointer(&dest->eventHandlers);
		}
	}

	void IMenuDef::write_menu_itemkeyhandler(IZone* zone, ZoneBuffer* buf, ItemKeyHandler* data)
	{
		auto* dest = buf->write(data);
		if (data->action)
		{
			buf->align(3);
			write_menu_eventhandlerset(zone, buf, data->action);
			ZoneBuffer::clear_pointer(&dest->action);
		}
		if (!data->next)
			return;
		buf->align(3);
		write_menu_itemkeyhandler(zone, buf, data->next);
		ZoneBuffer::clear_pointer(&dest->next);
	}

	void IMenuDef::write_menu_data(IZone* zone, ZoneBuffer* buf, menuData_t* data)
	{
		auto* dest = buf->write(data);
		if (data->expressionData)
		{
			buf->align(3);
			write_menu_supporting_data(zone, buf, data->expressionData);
			ZoneBuffer::clear_pointer(&dest->expressionData);
		}
		if (data->onOpen)
		{
			buf->align(3);
			write_menu_eventhandlerset(zone, buf, data->onOpen);
			ZoneBuffer::clear_pointer(&dest->onOpen);
		}
		if (data->onClose)
		{
			buf->align(3);
			write_menu_eventhandlerset(zone, buf, data->onClose);
			ZoneBuffer::clear_pointer(&dest->onClose);
		}
		if (data->onCloseRequest)
		{
			buf->align(3);
			write_menu_eventhandlerset(zone, buf, data->onCloseRequest);
			ZoneBuffer::clear_pointer(&dest->onCloseRequest);
		}
		if (data->onESC)
		{
			buf->align(3);
			write_menu_eventhandlerset(zone, buf, data->onESC);
			ZoneBuffer::clear_pointer(&dest->onESC);
		}
		if (data->onFocusDueToClose)
		{
			buf->align(3);
			write_menu_eventhandlerset(zone, buf, data->onFocusDueToClose);
			ZoneBuffer::clear_pointer(&dest->onFocusDueToClose);
		}
		if (data->onKey)
		{
			buf->align(3);
			write_menu_itemkeyhandler(zone, buf, data->onKey);
			ZoneBuffer::clear_pointer(&dest->onKey);
		}
		if (data->visibleExp)
		{
			buf->align(3);
			write_menu_statement(zone, buf, data->visibleExp);
			ZoneBuffer::clear_pointer(&dest->visibleExp);
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
			ZoneBuffer::clear_pointer(&dest->rectXExp);
		}
		if (data->rectYExp)
		{
			buf->align(3);
			write_menu_statement(zone, buf, data->rectYExp);
			ZoneBuffer::clear_pointer(&dest->rectYExp);
		}
		if (data->rectWExp)
		{
			buf->align(3);
			write_menu_statement(zone, buf, data->rectWExp);
			ZoneBuffer::clear_pointer(&dest->rectWExp);
		}
		if (data->rectHExp)
		{
			buf->align(3);
			write_menu_statement(zone, buf, data->rectHExp);
			ZoneBuffer::clear_pointer(&dest->rectHExp);
		}
		if (data->openSoundExp)
		{
			buf->align(3);
			write_menu_statement(zone, buf, data->openSoundExp);
			ZoneBuffer::clear_pointer(&dest->openSoundExp);
		}
		if (data->closeSoundExp)
		{
			buf->align(3);
			write_menu_statement(zone, buf, data->closeSoundExp);
			ZoneBuffer::clear_pointer(&dest->closeSoundExp);
		}
		if (data->soundLoopExp)
		{
			buf->align(3);
			write_menu_statement(zone, buf, data->soundLoopExp);
			ZoneBuffer::clear_pointer(&dest->soundLoopExp);
		}
	}

	void IMenuDef::write_menu_window(IZone* zone, ZoneBuffer* buf, windowDef_t* data, windowDef_t* dest)
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

	void IMenuDef::write_menu_item_defdata(IZone* zone, ZoneBuffer* buf, itemDefData_t* data, int type)
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
					ZoneBuffer::clear_pointer(&dest_listBox->onDoubleClick);
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
					ZoneBuffer::clear_pointer(&dest_listBox->elementHeightExp);
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
				//ZoneBuffer::clear_pointer(&dest->editField);
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
				//ZoneBuffer::clear_pointer(&dest->multi);
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
				//ZoneBuffer::clear_pointer(&dest->ticker);
			}
			break;
		case 21:
			if (data->scroll)
			{
				buf->align(3);
				/*auto* dest_scroll = */buf->write(data->scroll);
				//ZoneBuffer::clear_pointer(&dest->scroll);
			}
			break;
		}
	}

	void IMenuDef::write_menu_item_floatexpressions(IZone* zone, ZoneBuffer* buf, ItemFloatExpression* data, int count)
	{
		auto* dest = buf->write(data, count);
		for (int i = 0; i < count; i++)
		{
			if (data[i].expression)
			{
				buf->align(3);
				write_menu_statement(zone, buf, data[i].expression);
				ZoneBuffer::clear_pointer(&dest[i].expression);
			}
		}
	}

	void IMenuDef::write_menu_item(IZone* zone, ZoneBuffer* buf, itemDef_t* data)
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
			ZoneBuffer::clear_pointer(&dest->mouseEnterText);
		}
		if (data->mouseExitText)
		{
			buf->align(3);
			write_menu_eventhandlerset(zone, buf, data->mouseExitText);
			ZoneBuffer::clear_pointer(&dest->mouseExitText);
		}
		if (data->mouseEnter)
		{
			buf->align(3);
			write_menu_eventhandlerset(zone, buf, data->mouseEnter);
			ZoneBuffer::clear_pointer(&dest->mouseEnter);
		}
		if (data->mouseExit)
		{
			buf->align(3);
			write_menu_eventhandlerset(zone, buf, data->mouseExit);
			ZoneBuffer::clear_pointer(&dest->mouseExit);
		}
		if (data->action)
		{
			buf->align(3);
			write_menu_eventhandlerset(zone, buf, data->action);
			ZoneBuffer::clear_pointer(&dest->action);
		}
		if (data->accept)
		{
			buf->align(3);
			write_menu_eventhandlerset(zone, buf, data->accept);
			ZoneBuffer::clear_pointer(&dest->accept);
		}
		if (data->onFocus)
		{
			buf->align(3);
			write_menu_eventhandlerset(zone, buf, data->onFocus);
			ZoneBuffer::clear_pointer(&dest->onFocus);
		}
		if (data->hasFocus)
		{
			buf->align(3);
			write_menu_eventhandlerset(zone, buf, data->hasFocus);
			ZoneBuffer::clear_pointer(&dest->hasFocus);
		}
		if (data->leaveFocus)
		{
			buf->align(3);
			write_menu_eventhandlerset(zone, buf, data->leaveFocus);
			ZoneBuffer::clear_pointer(&dest->leaveFocus);
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
			ZoneBuffer::clear_pointer(&dest->onKey);
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
			ZoneBuffer::clear_pointer(&dest->typeData.data);
		}
		if (data->floatExpressions)
		{
			buf->align(3);
			write_menu_item_floatexpressions(zone, buf, data->floatExpressions, data->floatExpressionCount);
			ZoneBuffer::clear_pointer(&dest->floatExpressions);
		}
		if (data->visibleExp)
		{
			buf->align(3);
			write_menu_statement(zone, buf, data->visibleExp);
			ZoneBuffer::clear_pointer(&dest->visibleExp);
		}
		if (data->disabledExp)
		{
			buf->align(3);
			write_menu_statement(zone, buf, data->disabledExp);
			ZoneBuffer::clear_pointer(&dest->disabledExp);
		}
		if (data->textExp)
		{
			buf->align(3);
			write_menu_statement(zone, buf, data->textExp);
			ZoneBuffer::clear_pointer(&dest->textExp);
		}
		if (data->materialExp)
		{
			buf->align(3);
			write_menu_statement(zone, buf, data->materialExp);
			ZoneBuffer::clear_pointer(&dest->materialExp);
		}
		if (data->textAlignYExp)
		{
			buf->align(3);
			write_menu_statement(zone, buf, data->textAlignYExp);
			ZoneBuffer::clear_pointer(&dest->textAlignYExp);
		}
	}

	void IMenuDef::write(IZone* zone, ZoneBuffer* buf)
	{
		auto* data = this->asset_;
		auto* dest = buf->write(data);

		buf->push_stream(3);

		// menudata
		if (data->data)
		{
			buf->align(3);
			write_menu_data(zone, buf, data->data);
			ZoneBuffer::clear_pointer(&dest->data);
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
					ZoneBuffer::clear_pointer(&destItems[i]);
				}
			}
			ZoneBuffer::clear_pointer(&dest->items);
		}

		buf->pop_stream();
	}

	void IMenuDef::dump(menuDef_t* asset)
	{
	}
}