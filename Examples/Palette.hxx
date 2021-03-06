#pragma once
#include "Interface.vxx"

struct Palette final {
	static constexpr auto Name = "Palette";
	static constexpr auto Parameters = "shades:float[];width:int:opt;height:int:opt;";
	self(Shades, std::vector<double>{});
	self(Width, 640);
	self(Height, 480);
	Palette(auto Arguments) {
		for (auto&& x : Arguments["shades"])
			Shades.push_back(x);
		if (Arguments["width"].Exists())
			Width = Arguments["width"];
		if (Arguments["height"].Exists())
			Height = Arguments["height"];
		if (Width <= 0 || Height <= 0)
			throw RuntimeError{ "spatial dimensions must be positive!" };
	}
	auto RegisterMetadata(auto Core) {
		auto Metadata = VideoInfo{
			.Format = Core.Query(VideoFormats::GrayS),
			.FrameRateNumerator = 30000, .FrameRateDenominator = 1001,
			.Width = Width, .Height = Height,
			.FrameCount = 1
		};
		return std::vector{ Shades.size(), Metadata };
	}
	auto DrawFrame(auto Index, auto Core, auto FrameContext) {
		auto ProcessedFrame = VideoFrame<float>{ Core.AllocateVideoFrame(VideoFormats::GrayS, Width, Height) };
		for (auto y : Range{ Height })
			for (auto x : Range{ Width })
				ProcessedFrame[0][y][x] = Shades[FrameContext.QueryOutputIndex()];
		return ProcessedFrame.Leak();
	}
};