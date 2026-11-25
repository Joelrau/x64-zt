#include <std_include.hpp>
#include "menu.hpp"

namespace zonetool::iw5
{
	namespace
	{
		void read_statement(zone_reader32& reader, x86::pointer<Statement_s>& field);
		void read_handler_set(zone_reader32& reader, x86::pointer<MenuEventHandlerSet>& field);

		void read_supporting_data(zone_reader32& reader, ExpressionSupportingData* data)
		{
			if (const auto functions = reader.read_array(data->uifunctions.functions, 3, data->uifunctions.totalFunctions))
			{
				for (auto i = 0; i < data->uifunctions.totalFunctions; i++)
				{
					read_statement(reader, functions[i]);
				}
			}

			if (const auto dvars = reader.read_array(data->staticDvarList.staticDvars, 3, data->staticDvarList.numStaticDvars))
			{
				for (auto i = 0; i < data->staticDvarList.numStaticDvars; i++)
				{
					if (const auto dvar = reader.read_single(dvars[i], 3))
					{
						reader.read_string(dvar->dvarName);
					}
				}
			}

			reader.read_strings(data->uiStrings.strings, data->uiStrings.totalStrings);
		}

		void read_statement(zone_reader32& reader, x86::pointer<Statement_s>& field)
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
					if (entries[i].type != EET_OPERAND)
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

			if (const auto data = reader.read_single(statement->supportingData, 3))
			{
				read_supporting_data(reader, data);
			}
		}

		void read_handler(zone_reader32& reader, x86::pointer<MenuEventHandler>& field)
		{
			const auto handler = reader.read_single(field, 3);
			if (!handler)
			{
				return;
			}

			switch (handler->eventType)
			{
			case EVENT_UNCONDITIONAL:
				reader.read_string(handler->eventData.unconditionalScript);
				break;
			case EVENT_IF:
				if (const auto script = reader.read_single(handler->eventData.conditionalScript, 3))
				{
					read_statement(reader, script->eventExpression);
					read_handler_set(reader, script->eventHandlerSet);
				}
				break;
			case EVENT_ELSE:
				read_handler_set(reader, handler->eventData.elseScript);
				break;
			case EVENT_SET_LOCAL_VAR_BOOL:
			case EVENT_SET_LOCAL_VAR_INT:
			case EVENT_SET_LOCAL_VAR_FLOAT:
			case EVENT_SET_LOCAL_VAR_STRING:
				if (const auto data = reader.read_single(handler->eventData.setLocalVarData, 3))
				{
					reader.read_string(data->localVarName);
					read_statement(reader, data->expression);
				}
				break;
			default:
				break;
			}
		}

		void read_handler_set(zone_reader32& reader, x86::pointer<MenuEventHandlerSet>& field)
		{
			const auto set = reader.read_single(field, 3);
			if (!set)
			{
				return;
			}

			if (const auto handlers = reader.read_array(set->eventHandlers, 3, set->eventHandlerCount))
			{
				for (auto i = 0; i < set->eventHandlerCount; i++)
				{
					read_handler(reader, handlers[i]);
				}
			}
		}

		void read_key_handler(zone_reader32& reader, x86::pointer<ItemKeyHandler>& field)
		{
			if (const auto handler = reader.read_single(field, 3))
			{
				read_handler_set(reader, handler->action);
				read_key_handler(reader, handler->next);
			}
		}

		void read_window(zone_reader32& reader, windowDef_t& window)
		{
			reader.read_string(window.name);
			reader.read_string(window.group);
			reader.read_asset(ASSET_TYPE_MATERIAL, window.background);
		}

		void read_type_data(zone_reader32& reader, itemDef_t* item)
		{
			switch (item->type)
			{
			case ITEM_TYPE_LISTBOX:
				if (const auto list_box = reader.read_single(item->typeData.listBox, 3))
				{
					read_handler_set(reader, list_box->onDoubleClick);
					reader.read_asset(ASSET_TYPE_MATERIAL, list_box->selectIcon);
					read_statement(reader, list_box->elementHeightExp);
				}
				break;
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
				reader.read_single(item->typeData.editField, 3);
				break;
			case ITEM_TYPE_MULTI:
				if (const auto multi = reader.read_single(item->typeData.multi, 3))
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
				break;
			case ITEM_TYPE_DVARENUM:
				reader.read_string(item->typeData.enumDvarName);
				break;
			case ITEM_TYPE_NEWS_TICKER:
				reader.read_single(item->typeData.ticker, 3);
				break;
			case ITEM_TYPE_TEXT_SCROLL:
				reader.read_single(item->typeData.scroll, 3);
				break;
			default:
				break;
			}
		}

		void read_item(zone_reader32& reader, x86::pointer<itemDef_t>& field)
		{
			const auto item = reader.read_single(field, 3);
			if (!item)
			{
				return;
			}

			read_window(reader, item->window);
			reader.read_string(item->text);
			read_handler_set(reader, item->mouseEnterText);
			read_handler_set(reader, item->mouseExitText);
			read_handler_set(reader, item->mouseEnter);
			read_handler_set(reader, item->mouseExit);
			read_handler_set(reader, item->action);
			read_handler_set(reader, item->accept);
			read_handler_set(reader, item->onFocus);
			read_handler_set(reader, item->hasFocus);
			read_handler_set(reader, item->leaveFocus);
			reader.read_string(item->dvar);
			reader.read_string(item->dvarTest);
			read_key_handler(reader, item->onKey);
			reader.read_string(item->enableDvar);
			reader.read_string(item->localVar);
			reader.read_asset(ASSET_TYPE_SOUND, item->focusSound);
			read_type_data(reader, item);

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

		void read_menu_data(zone_reader32& reader, menuData_t* data)
		{
			if (const auto expression_data = reader.read_single(data->expressionData, 3))
			{
				read_supporting_data(reader, expression_data);
			}

			read_handler_set(reader, data->onOpen);
			read_handler_set(reader, data->onCloseRequest);
			read_handler_set(reader, data->onClose);
			read_handler_set(reader, data->onESC);
			read_handler_set(reader, data->onFocusDueToClose);
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
	}

	void menu::read(zone_reader32& reader, menuDef_t* asset)
	{
		reader.push_stream(XFILE_BLOCK_VIRTUAL);
		if (const auto data = reader.read_single(asset->data, 3))
		{
			read_menu_data(reader, data);
		}

		read_window(reader, asset->window);

		if (const auto items = reader.read_array(asset->items, 3, asset->itemCount))
		{
			for (auto i = 0; i < asset->itemCount; i++)
			{
				read_item(reader, items[i]);
			}
		}

		reader.pop_stream();
	}
}
