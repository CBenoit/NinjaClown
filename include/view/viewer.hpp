#ifndef NINJACLOWN_VIEW_VIEWER_HPP
#define NINJACLOWN_VIEW_VIEWER_HPP

#include "utils/synchronized.hpp"

#include "view/fps_limiter.hpp"
#include "view/map.hpp"
#include "view/mob.hpp"
#include "view/object.hpp"

namespace sf {
class RenderWindow;
}

namespace adapter {
struct view_handle;
}

namespace view {
class viewer {
public:
	~viewer() {
		stop();
	}

	void run();

	void stop() noexcept;

	void wait() {
		if (m_thread && m_thread->joinable()) {
			m_thread->join();
		}
	}

	// can be called concurrently
	void target_fps(unsigned int fps) noexcept {
		m_fps_limiter.target_fps(fps);
	}

	[[nodiscard]] std::pair<std::size_t, std::size_t> level_size() const noexcept {
		return m_level_size;
	}

	// can be called concurrently
	[[nodiscard]] unsigned int target_fps() const noexcept {
		return m_fps_limiter.target_fps();
	}

	// can be called concurrently
	[[nodiscard]] float average_fps() const noexcept {
		return m_fps_limiter.average_fps();
	}

	[[nodiscard]] unsigned int current_frame() const noexcept {
		return m_fps_limiter.frame_count();
	}

	auto acquire_mobs() noexcept {
		return m_mobs.acquire();
	}

	auto acquire_objects() noexcept {
		return m_objects.acquire();
	}

	auto acquire_map() noexcept {
		return m_map.acquire();
	}

	void set_map(std::vector<std::vector<map::cell>> &&new_map) noexcept {
		auto map = m_map.acquire();
		map->set(std::move(new_map));
		m_level_size = map->level_size();
	}

	std::pair<float, float> to_screen_coords(float x, float y) const noexcept;

	const std::chrono::system_clock::time_point starting_time{std::chrono::system_clock::now()};

	std::atomic_bool show_debug_data{true};

private:
	std::pair<float, float> to_screen_coords_base(float x, float y) const noexcept;

	void do_run() noexcept;

	void do_tooltip(sf::RenderWindow &, adapter::view_handle) noexcept;

	std::pair<std::size_t, std::size_t> m_window_size;

	utils::synchronized<std::vector<object>> m_objects;
	utils::synchronized<std::vector<mob>> m_mobs;
	utils::synchronized<view::map, utils::spinlock> m_map{};
	std::pair<std::size_t, std::size_t> m_level_size{};

	std::unique_ptr<std::thread> m_thread{};
	std::atomic_bool m_running{false};

	fps_limiter m_fps_limiter{};
};
} // namespace view

#endif //NINJACLOWN_VIEW_VIEWER_HPP
