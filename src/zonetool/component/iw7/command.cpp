#include <std_include.hpp>
#include "loader/component_loader.hpp"
#include "command.hpp"

#include "game/iw7/game.hpp"

#include <utils/hook.hpp>
#include <utils/string.hpp>
#include <utils/memory.hpp>

namespace iw7
{
	namespace command
	{
		namespace
		{
			std::unordered_map<std::string, std::function<void(params&)>> handlers;
			std::unordered_map<std::string, std::function<void(int, params_sv&)>> handlers_sv;

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
			: nesting_(game::cmd_args->nesting)
		{
		}

		int params::size() const
		{
			return game::cmd_args->argc[this->nesting_];
		}

		const char* params::get(const int index) const
		{
			if (index >= this->size())
			{
				return "";
			}

			return game::cmd_args->argv[this->nesting_][index];
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

		params_sv::params_sv()
			: nesting_(game::sv_cmd_args->nesting)
		{
		}

		int params_sv::size() const
		{
			return game::sv_cmd_args->argc[this->nesting_];
		}

		const char* params_sv::get(const int index) const
		{
			if (index >= this->size())
			{
				return "";
			}

			return game::sv_cmd_args->argv[this->nesting_][index];
		}

		std::string params_sv::join(const int index) const
		{
			std::string result = {};

			for (auto i = index; i < this->size(); i++)
			{
				if (i > index) result.append(" ");
				result.append(this->get(i));
			}
			return result;
		}

		std::vector<std::string> params_sv::get_all() const
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
			game::Cmd_AddCommandInternal(name, callback, utils::memory::get_allocator()->allocate<game::cmd_function_s>());
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

		void add_sv(const char* name, std::function<void(int, const params_sv&)> callback)
		{
			// doing this so the sv command would show up in the console
			add_raw(name, nullptr);

			const auto command = utils::string::to_lower(name);

			if (handlers_sv.find(command) == handlers_sv.end())
				handlers_sv[command] = std::move(callback);
		}

		void execute(std::string command, const bool sync)
		{
			command += "\n";

			if (sync)
			{
				game::Cmd_ExecuteSingleCommand(0, 0, command.data());
			}
			else
			{
				game::Cbuf_AddText(0, command.data());
			}
		}

		class component final : public component_interface
		{
		};
	}
}

REGISTER_COMPONENT_IW7(iw7::command::component)