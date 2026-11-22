#include <std_include.hpp>
#include "menu.hpp"

namespace zonetool::iw3
{
	namespace
	{
		void read_window(zone_reader32& reader, windowDef_t& window)
		{
			reader.read_string(window.name);
			reader.read_string(window.group);
			reader.read_asset(ASSET_TYPE_MATERIAL, window.background);
		}

		void read_key_handlers(zone_reader32& reader, x86::pointer<ItemKeyHandler>& first)
		{
			auto* field = &first;
			while (const auto handler = reader.read_single(*field, 3))
			{
				reader.read_string(handler->action);
				field = &handler->next;
			}
		}

		void read_statement(zone_reader32& reader, statement_s& statement)
		{
			const auto entries = reader.read_array(statement.entries, 3, statement.numEntries);
			if (!entries)
			{
				return;
			}

			for (auto i = 0; i < statement.numEntries; i++)
			{
				const auto entry = reader.read_single(entries[i], 3);
				if (entry && entry->type == EET_OPERAND && entry->data.operand.dataType == VAL_STRING)
				{
					reader.read_string(entry->data.operand.internals.stringVal);
				}
			}
		}

		void read_item_data(zone_reader32& reader, itemDef_s* item)
		{
			auto& data = item->typeData;
			switch (item->type)
			{
			case ITEM_TYPE_LISTBOX:
				if (const auto list_box = reader.read_single(data.listBox, 3))
				{
					reader.read_string(list_box->onDoubleClick);
					reader.read_asset(ASSET_TYPE_MATERIAL, list_box->selectIcon);
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
				reader.read_single(data.editField, 3);
				break;
			case ITEM_TYPE_MULTI:
				if (const auto multi = reader.read_single(data.multi, 3))
				{
					for (auto& name : multi->dvarList)
					{
						reader.read_string(name);
					}

					for (auto& name : multi->dvarStr)
					{
						reader.read_string(name);
					}
				}
				break;
			case ITEM_TYPE_DVARENUM:
				reader.read_string(data.enumDvarName);
				break;
			default:
				break;
			}
		}

		void read_item(zone_reader32& reader, itemDef_s* item)
		{
			read_window(reader, item->window);
			reader.read_string(item->text);
			reader.read_string(item->mouseEnterText);
			reader.read_string(item->mouseExitText);
			reader.read_string(item->mouseEnter);
			reader.read_string(item->mouseExit);
			reader.read_string(item->action);
			reader.read_string(item->onAccept);
			reader.read_string(item->onFocus);
			reader.read_string(item->leaveFocus);
			reader.read_string(item->dvar);
			reader.read_string(item->dvarTest);
			read_key_handlers(reader, item->onKey);
			reader.read_string(item->enableDvar);
			reader.read_asset(ASSET_TYPE_SOUND, item->focusSound);
			read_item_data(reader, item);
			read_statement(reader, item->visibleExp);
			read_statement(reader, item->textExp);
			read_statement(reader, item->materialExp);
			read_statement(reader, item->rectXExp);
			read_statement(reader, item->rectYExp);
			read_statement(reader, item->rectWExp);
			read_statement(reader, item->rectHExp);
			read_statement(reader, item->forecolorAExp);
		}
	}

	void menu::read(zone_reader32& reader, menuDef_t* asset)
	{
		reader.push_stream(XFILE_BLOCK_VIRTUAL);
		read_window(reader, asset->window);
		reader.read_string(asset->font);
		reader.read_string(asset->onOpen);
		reader.read_string(asset->onClose);
		reader.read_string(asset->onESC);
		read_key_handlers(reader, asset->onKey);
		read_statement(reader, asset->visibleExp);
		reader.read_string(asset->allowedBinding);
		reader.read_string(asset->soundName);
		read_statement(reader, asset->rectXExp);
		read_statement(reader, asset->rectYExp);

		if (const auto items = reader.read_array(asset->items, 3, asset->itemCount))
		{
			for (auto i = 0; i < asset->itemCount; i++)
			{
				if (const auto item = reader.read_single(items[i], 3))
				{
					read_item(reader, item);
				}
			}
		}

		reader.pop_stream();
	}
}
