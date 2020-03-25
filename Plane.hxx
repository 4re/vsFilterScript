#pragma once
#include "Range.hxx"

template<typename PixelType, typename PolicyType>
struct Plane final {
	self(Width, 0_size);
	self(Height, 0_size);
	self(Canvas, std::vector<PixelType*>{});
	self(PaddingPolicy, PolicyType{});
	struct Proxy final {
		self(State, static_cast<Plane*>(nullptr));
		self(y, 0_ptrdiff);
		self(yOffset, 0_ptrdiff);
		self(xOffset, 0_ptrdiff);
		auto operator[](auto x) {
			auto yAbsolute = y + yOffset;
			auto xAbsolute = x + xOffset;
			if (xAbsolute < 0 || yAbsolute < 0 || xAbsolute >= State->Width || yAbsolute >= State->Height)
				return State->PaddingPolicy(State->Canvas.data(), State->Width, State->Height, yAbsolute, xAbsolute);
			else
				return State->Canvas[yAbsolute][xAbsolute];
		}
	};
	struct Offset final {
		self(State, static_cast<Plane*>(nullptr));
		self(yOffset, 0_ptrdiff);
		self(xOffset, 0_ptrdiff);
		auto operator[](auto y) {
			return Proxy{ .State = State, .y = y, .yOffset = yOffset, .xOffset = xOffset };
		}
		auto GetCoordinates() {
			return std::array{ yOffset, xOffset };
		}
		auto View(auto y, auto x) {
			return Offset{ .State = State, .yOffset = yOffset + y, .xOffset = xOffset + x };
		}
	};
	Plane() = default;
	Plane(auto Pointer, auto Width, auto Height, auto&& PaddingPolicy) {
		this->Width = Width;
		this->Height = Height;
		this->PaddingPolicy = Forward(PaddingPolicy);
		auto Origin = reinterpret_cast<PixelType*>(Pointer);
		for (auto y : Range{ Height })
			Canvas.push_back(Origin + y * Width);
	}
	auto operator[](auto y) {
		if constexpr (std::is_const_v<PixelType>)
			return Proxy{ .State = this, .y = y };
		else
			return Canvas[y];
	}
	auto View(auto y, auto x) {
		return Offset{ .State = this, .yOffset = y, .xOffset = x };
	}
};