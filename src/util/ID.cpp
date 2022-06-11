#include <util/ID.hpp>
#include <atomic>

static std::atomic<id_t> global_id = 0;

id_t new_id(void) {
	return ++global_id;
}
