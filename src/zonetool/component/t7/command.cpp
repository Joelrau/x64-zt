#include <std_include.hpp>
#include "loader/component_loader.hpp"

#include "command.hpp"
#include "scheduler.hpp"

#include "game/t7/game.hpp"
#include "game/t7/dvars.hpp"

#include <utils/hook.hpp>
#include <utils/string.hpp>

namespace t7
{
	namespace command
	{
		namespace
		{
			void update_whitelist_stub()
			{
				game::cmd_function_s* current_function = game::cmd_functions;
				while (current_function)
				{
					current_function->autoComplete = 1;
					current_function = current_function->next;
				}
			}
		}

		namespace
		{
			std::unordered_map<std::string, std::function<void(params&)>> handlers;

			void main_handler()
			{
				params params = {};

				const auto command = utils::string::to_lower(params[0]);
				if (handlers.find(command) != handlers.end())
				{
					handlers[command](params);
				}
			}
		}

		params::params()
			: nesting_(game::Cmd_Args()->nesting)
		{
		}

		int params::size() const
		{
			return game::Cmd_Args()->argc[this->nesting_];
		}

		const char* params::get(const int index) const
		{
			if (index >= this->size())
			{
				return "";
			}

			return game::Cmd_Args()->argv[this->nesting_][index];
		}

		std::string params::join(const int index) const
		{
			std::string result = {};

			for (auto i = index; i < this->size(); i++)
			{
				if (i > index) result.append(" ");
				result.append(this->get(i));
			}
			return result;
		}

		std::vector<std::string> params::get_all() const
		{
			std::vector<std::string> params_;
			for (auto i = 0; i < this->size(); i++)
			{
				params_.push_back(this->get(i));
			}
			return params_;
		}

		void add_raw(const char* name, void (*callback)())
		{
			auto& allocator = *utils::memory::get_allocator();
			auto* cmd_function = allocator.allocate<game::cmd_function_s>();
			const auto* cmd_string = allocator.duplicate_string(name);

			game::Cmd_AddCommandInternal(cmd_string, callback, utils::memory::get_allocator()->allocate<game::cmd_function_s>());
			cmd_function->autoComplete = 1;
		}

		void add(const char* name, const std::function<void(const params&)>& callback)
		{
			const auto command = utils::string::to_lower(name);

			if (handlers.find(command) == handlers.end())
				add_raw(name, main_handler);

			handlers[command] = callback;
		}

		void add(const char* name, const std::function<void()>& callback)
		{
			add(name, [callback](const params&)
			{
				callback();
			});
		}

		void execute(std::string command, const bool sync)
		{
			command += "\n";

			if (sync)
			{
				game::Cmd_ExecuteSingleCommand(0, game::CONTROLLER_INDEX_FIRST, command.data(), 0);
			}
			else
			{
				game::Cbuf_AddText(0, command.data());
			}
		}

		class component final : public component_interface
		{
			void post_unpack() override
			{
				// Disable whitelist
				utils::hook::jump(0x1404F9CD0, update_whitelist_stub);
			}
		};
	}
}

REGISTER_COMPONENT_T7(t7::command::component)
