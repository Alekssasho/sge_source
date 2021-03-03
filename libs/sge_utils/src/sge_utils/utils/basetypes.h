#pragma once

namespace sge {

struct Object {
  public:
	Object() = default;
};

struct Noncopyable : public Object {
  public:
	Noncopyable(const Noncopyable&) = delete;
	Noncopyable& operator=(const Noncopyable&) = delete;

  protected:
	inline Noncopyable() = default;
};

struct NoncopyableMovable : public Object {
  public:
	NoncopyableMovable(const NoncopyableMovable&) = delete;
	NoncopyableMovable& operator=(const NoncopyableMovable&) = delete;

	inline NoncopyableMovable(NoncopyableMovable&&) = default;
	inline NoncopyableMovable& operator=(NoncopyableMovable&&) = default;

  protected:
	inline NoncopyableMovable() = default;
};

struct Polymorphic : public Object {
  public:
	Polymorphic() = default;
	Polymorphic(const Polymorphic& copyFrom) = default;
	Polymorphic(Polymorphic&& moveFrom) = default;
	Polymorphic& operator=(const Polymorphic& copyFrom) = default;
	Polymorphic& operator=(Polymorphic&& moveFrom) = default;
	virtual ~Polymorphic() = default;
};

} // namespace sge
