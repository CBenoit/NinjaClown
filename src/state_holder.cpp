#include <spdlog/spdlog.h>

#include <filesystem>

#include <imterm/terminal.hpp>

#include "adapter/adapter.hpp"
#include "model/model.hpp"
#include "state_holder.hpp"
#include "terminal_commands.hpp"
#include "utils/resource_manager.hpp"
#include "view/viewer.hpp"

namespace {
utils::resource_manager configure_resources(const std::filesystem::path &config) {
	utils::resource_manager rm{};
	if (!rm.load_config(config)) {
		spdlog::warn("Failed to load resources from {}", config.generic_string());
	}
	return rm;
}

std::shared_ptr<terminal_commands> build_commands_manager() {
	auto manager = std::make_shared<terminal_commands>();
	spdlog::default_logger()->sinks().push_back(manager);
	return manager;
}
} // namespace

namespace state {
struct pimpl {
	pimpl(holder* holder, const std::filesystem::path &config, const std::filesystem::path &autorun_script) noexcept
        : command_manager{build_commands_manager()}
	    , resources{configure_resources(config)}
        , terminal{*holder, "Terminal", 0, 0, command_manager}
        , model{holder}
        , view{holder}
        , adapter{holder}
	{

	}

    std::shared_ptr<terminal_commands> command_manager;
    utils::resource_manager resources;
	ImTerm::terminal<terminal_commands> terminal;
	model::model model;
	view::viewer view;
	adapter::adapter adapter;

	std::map<std::string, property> properties{};
};
} // namespace state

state::holder::~holder() = default;

state::holder::holder(const std::filesystem::path &config, const std::filesystem::path &autorun_script) noexcept
    : m_pimpl{std::make_unique<pimpl>(this, config, autorun_script)}
{

	using view::viewer;

	m_pimpl->properties.emplace("average_fps", property{&viewer::average_fps, m_pimpl->view});

	m_pimpl->properties.emplace("target_fps",
	                   property::proxy<unsigned int>::from_accessor<viewer>(m_pimpl->view, &viewer::target_fps, &viewer::target_fps));

	m_pimpl->properties.emplace("display_debug_data", property{&viewer::show_debug_data, m_pimpl->view});

	m_pimpl->command_manager->load_commands(m_pimpl->resources);
	if (is_regular_file(autorun_script)) {
		std::ifstream autorun{autorun_script};
		std::string command_line;
		while (getline(autorun, command_line)) {
			m_pimpl->terminal.execute(command_line);
		}
	}
}

void state::holder::run() noexcept {
	m_pimpl->view.run();
}

void state::holder::wait() noexcept {
	m_pimpl->view.wait();
}
utils::resource_manager &state::holder::resources() {
    return m_pimpl->resources;
}
std::map<std::string, state::property> &state::holder::properties() {
	return m_pimpl->properties;
}

ImTerm::terminal<terminal_commands> &state::holder::terminal() noexcept {
	return m_pimpl->terminal;
}

adapter::adapter &state::holder::adapter() noexcept {
	return m_pimpl->adapter;
}

model::model &state::holder::model() noexcept {
	return m_pimpl->model;
}

view::viewer &state::holder::view() noexcept {
	return m_pimpl->view;
}
