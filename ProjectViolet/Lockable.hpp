#pragma once

#include <mutex>
#include <memory>


class Lockable {
public:

	Lockable() { lockObj = std::make_unique<recursive_mutex>(); }
	Lockable(const Lockable&) { lockObj = std::make_unique<recursive_mutex>(); }

	void lock() { lockObj->lock(); }
	void unlock() { lockObj->unlock(); }
	bool try_lock() { return lockObj->try_lock(); }

private:

	std::unique_ptr<recursive_mutex> lockObj;

};
