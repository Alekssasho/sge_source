#pragma once

#include <functional>
#include <memory>
#include <mutex>
#include <unordered_map>

#include "basetypes.h"
#include "sge_utils/sge_utils.h"

namespace sge {

/// A class used for managing lifetime of callbacks registered in EventEmitter.
/// When a callback is registered to a list a EventSubscription is created and
/// when this object gets destoryed the callback is unregistered from the EventEmitter.
struct EventSubscription : public NoncopyableMovable {
	EventSubscription() = default;

	EventSubscription(std::function<void()> unsubscribeFn)
	    : unsubscribeFn(std::move(unsubscribeFn)) {}

	~EventSubscription() { unsubscribe(); }

	/// Unregisters the callback for the ownging EventEmitter.
	void unsubscribe() {
		if (unsubscribeFn != nullptr) {
			unsubscribeFn();
			unsubscribeFn = nullptr;
		}
	}

	/// if called the lifetime of the callback will no longer be maintained
	/// and it will get called until the owning EventEmitter exists.
	void abandon() { unsubscribeFn = nullptr; }

	EventSubscription(EventSubscription&& other) noexcept {
		this->unsubscribeFn = std::move(other.unsubscribeFn);
		other.unsubscribeFn = nullptr;
	}

	EventSubscription& operator=(EventSubscription&& other) noexcept {
		this->unsubscribe();
		this->unsubscribeFn = std::move(other.unsubscribeFn);
		other.unsubscribeFn = nullptr;

		return *this;
	}

  private:
	std::function<void()> unsubscribeFn;
};

/// A EventEmitter holds callbacks which are subscribed to it.
/// These callbacks could be invoked.
template <typename... TArgs>
struct EventEmitter : public NoncopyableMovable {
  private:
	struct Internal {
		std::unordered_map<int, std::function<void(TArgs...)>> callbacks;
		int nextFreeId = 0;
		std::mutex dataLock;
	};

	std::shared_ptr<Internal> data;

  public:
	EventEmitter()
	    : data(std::make_shared<Internal>()) {}

	EventEmitter(EventEmitter&& ref) noexcept
	    : data(std::move(ref.data)) {
		ref.data = std::make_shared<Internal>();
	}

	EventEmitter& operator=(EventEmitter&& ref) noexcept {
		data = std::move(ref.data);
		ref.data = std::make_shared<Internal>();
		return *this;
	}

	/// Add a new callback to be called.
	/// @retval a EventSubscription used to manage the lifetime of the
	///         registered callback, can be used to unsubscribe.
	[[nodiscard]] EventSubscription subscribe(std::function<void(TArgs...)> fn) {
		std::lock_guard<std::mutex> g(data->dataLock);
		const int id = data->nextFreeId++;
		sgeAssert(data->callbacks.count(id) == 0);
		data->callbacks[id] = std::move(fn);

		std::weak_ptr<Internal> weakData = data;
		return EventSubscription(
		    // This is the unregister function tall will be held by the EventSubscription
		    // which will get called when EventSubscription gets destroyed.
		    [id, weakData]() -> void {
			    if (std::shared_ptr<Internal> strongData = weakData.lock()) {
				    auto itr = strongData->callbacks.find(id);
				    if_checked(itr != strongData->callbacks.end()) { strongData->callbacks.erase(itr); }
			    }
		    });
	}

	void operator()(TArgs... args) const {
		std::lock_guard<std::mutex> g(data->dataLock); // if you crash here, then this mutex is probably locked for a second time.
		for (auto& callback : data->callbacks) {
			if_checked(callback.second) { callback.second(args...); }
		}
	}

	bool isEmpty() const { return data->callbacks.empty(); }

	void discardAllCallbacks() {
		std::lock_guard<std::mutex> g(data->dataLock);
		data->callbacks.clear();
	}
};

} // namespace sge
