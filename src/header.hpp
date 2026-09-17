#pragma once
#include <SDL3/SDL.h>
#include <SDL3/SDL_assert.h>
#include <SDL3/SDL_init.h>
#include <SDL3/SDL_oldnames.h>
#include <SDL3/SDL_rect.h>
#include <SDL3/SDL_render.h>
#include <SDL3/SDL_video.h>
#include <cstdint>
#include <stdfloat>
#include <stddef.h>
#include <stdint.h>
#include <array>
#include <random>
#include <utility>
// #include <print>

namespace RNG{
	inline std::random_device rd;
	inline std::mt19937_64 rngr(rd());
	inline std::uniform_int_distribution<short> uid1(-1,1);
	inline std::uniform_int_distribution<short> uid4(0,3);
	inline std::uniform_int_distribution<short> uid5(0,4);
	inline std::uniform_int_distribution<short> uid8(0,7);
	inline std::uniform_int_distribution<short> uid256(0,255);
	inline std::uniform_real_distribution<double> urd1(-1,1);
	inline std::uniform_real_distribution<double> urd(0.0,1.0);
	inline std::bernoulli_distribution bd(0.5);
}

static constexpr std::array<std::pair<double,double>,4> DirectionStraight{
	{{1.0,0.0},{0.0,1.0},{-1.0,0.0},{0.0,-1.0}}
};
static constexpr std::array<std::pair<double,double>,4> DirectionDiagonal{
	{{1.0,1.0},{-1.0,1.0},{-1.0,-1.0},{1.0,-1.0}}
};
static constexpr std::array<std::pair<double,double>,5> DirectionStraightWCenter{
	{{0.0,0.0},{1.0,0.0},{0.0,1.0},{-1.0,0.0},{0.0,-1.0}}
};
static constexpr std::array<std::pair<double,double>,5> DirectionDiagonalWCenter{
	{{0.0,0.0},{1.0,1.0},{-1.0,1.0},{-1.0,-1.0},{1.0,-1.0}}
};
static constexpr std::array<std::pair<double,double>,8> DirectionStraightDiagonal{
	{{1.0,0.0},{1.0,1.0},{0.0,1.0},{-1.0,1.0},{-1.0,0.0},{-1.0,-1.0},{0.0,-1.0},{1.0,-1.0}}
};

class Canvas
{
	private:
		uint64_t Width;
		uint64_t Height;
		std::string Title;

		SDL_Window* Window;
		SDL_Renderer* Renderer;
	public:
		// Constructors
		Canvas() = default;
		Canvas(const Canvas&) = delete;
		Canvas(Canvas&& other) noexcept : Width(other.Width), Height(other.Height), Title(std::move(other.Title)), Window(std::exchange(other.Window, nullptr)), Renderer(std::exchange(other.Renderer,nullptr)) {}
		explicit Canvas(uint64_t w=900, uint64_t h=600, std::string t="SDL App") : Width(w), Height(h), Title(std::move(t)) {}
		// Assignments
		Canvas& operator=(const Canvas&) = delete;
		Canvas& operator=(Canvas&& other) noexcept
		{
			if((*this)==other)
			{
				return *this;
			}
			else
			{
				Destroy();
				Width=other.Width;
				Height=other.Height;
				Title=std::move(other.Title);
				Window=std::exchange(other.Window,nullptr);
				Renderer=std::exchange(other.Renderer,nullptr);
				return *this;
			}
		}
		// Destructor
		~Canvas() {Destroy();}
		// Comparison
		bool operator==(const Canvas& other) const noexcept
		{
			return (Width==other.Width && Height==other.Height && Title==other.Title && Window==other.Window && Renderer==other.Renderer)?true:false;
		}
		// Destroy()
		void Destroy() noexcept
		{
			if (Renderer)
			{
				SDL_DestroyRenderer(Renderer);
				Renderer = nullptr;
			}
			if (Window)
			{
				SDL_DestroyWindow(Window);
				Window = nullptr;
			}
			SDL_Quit();
		}
		// Create Window
		bool CanvasCreateWindow()
		{
			if (!SDL_Init(SDL_INIT_VIDEO))
			{
				std::printf("SDL Init Error: [SDL] %s\n",SDL_GetError());
				return false;
			}
			if (!SDL_CreateWindowAndRenderer(Title.c_str(), static_cast<int>(Width), static_cast<int>(Height), SDL_WINDOW_RESIZABLE, &Window, &Renderer))
			{
				std::printf("Window Creation Error: [SDL] %s\n",SDL_GetError());
				SDL_Quit();
				return false;
			}
			return true;
		}

		// Getters
		[[nodiscard]] SDL_Renderer* GetRenderer() const noexcept {return Renderer;}
		[[nodiscard]] SDL_Window* GetWindow() const noexcept {return Window;}
};

enum class WalkerMoveStyle
{
	Straight=0,
	Diagonal,
	StraightDiagonal,
	StraightWCenter,
	DiagonalWCenter,
	StraightDiagonalWCenter,
	StraightContinuus,
	DiagonalContinuous,
	StraightDiagonalContinuous
};

class Color
{
	private:
		unsigned short R;
		unsigned short G;
		unsigned short B;
		unsigned short A;
	public:
		// Constructor
		Color() = default;
		Color(const Color&) = default;
		Color(Color&&) noexcept = default;
		explicit Color(short r, short g, short b, short a) : R(r), G(g), B(b), A(a) {}
		// Assignment
		Color& operator=(const Color&) = default;
		Color& operator=(Color&&) noexcept = default;
		// Destructor
		~Color() = default;
		const unsigned short GetR() const noexcept {return R;}
		const unsigned short GetG() const noexcept {return G;}
		const unsigned short GetB() const noexcept {return B;}
		const unsigned short GetA() const noexcept {return A;}
		
};

class Walker
{
	private:
		Color WalkerColor;
		double X;
		double Y;
		double Size;
		WalkerMoveStyle MoveStyle;
	public:
		// Constructor
		Walker() = default;
		Walker(const Walker&) = default;
		Walker(Walker&&) noexcept = default;
		explicit Walker(Color c=Color(0,255,0,255), double x=0.0, double y = 0.0, double s = 0.0, WalkerMoveStyle wms=WalkerMoveStyle::Straight) : WalkerColor(c), X(x), Y(y), Size(s), MoveStyle(wms) {} 
		// Assignment
		Walker& operator=(const Walker&) = default;
		Walker& operator=(Walker&&) noexcept = default;
		// Destructor
		~Walker() = default;

		// Methods
		// Step
		void Step(uint64_t W, uint64_t H)
		{
			switch(MoveStyle)
			{
				case WalkerMoveStyle::Straight: {
					short rand = RNG::uid4(RNG::rngr);
					auto [dX,dY] = DirectionStraight[rand];
					X += dX; Y += dY;
					break;
				}
				case WalkerMoveStyle::Diagonal: {
					short rand = RNG::uid4(RNG::rngr);
					auto [dX,dY] = DirectionDiagonal[rand];
					X += dX; Y += dY;
					break;
				}
				case WalkerMoveStyle::StraightDiagonal: {
					short rand = RNG::uid8(RNG::rngr);
					auto [dX,dY] = DirectionStraightDiagonal[rand];
					X += dX; Y += dY;
					break;
				}
				case WalkerMoveStyle::StraightWCenter: {
					short rand = RNG::uid5(RNG::rngr);
					auto [dX,dY] = DirectionStraightWCenter[rand];
					X += dX; Y += dY;
					break;
				}
				case WalkerMoveStyle::DiagonalWCenter: {
					short rand = RNG::uid5(RNG::rngr);
					auto [dX,dY] = DirectionDiagonalWCenter[rand];
					X += dX; Y += dY;
					break;
				}
				case WalkerMoveStyle::StraightDiagonalWCenter: {
					X += RNG::uid1(RNG::rngr); Y += RNG::uid1(RNG::rngr);
					break;
				}
				case WalkerMoveStyle::StraightContinuus: {
					if (RNG::bd(RNG::rngr)) X += RNG::urd1(RNG::rngr);
					else          Y += RNG::urd1(RNG::rngr);
					break;
				}
				case WalkerMoveStyle::DiagonalContinuous: {
					double MoveSign = RNG::bd(RNG::rngr)?1.0:-1.0;
					double Move = RNG::urd1(RNG::rngr);
					X += Move; Y += Move*MoveSign;
					break;
				}
				case WalkerMoveStyle::StraightDiagonalContinuous: {
					X += RNG::urd1(RNG::rngr); Y += RNG::urd1(RNG::rngr);
					break;
				}
				default: {
					short rand = RNG::uid4(RNG::rngr);
					auto [dX,dY] = DirectionStraight[rand];
					X += dX; Y += dY;
					break;
				}
			}
			if (X>=W) X -= W; else if (X<=0.0) X += W;
			if (Y>=H) Y -= H; else if (Y<=0.0) Y += H;
		}
		const double GetX() const noexcept {return X;}
		void SetX(double x) noexcept {X=x;} 
		const double GetY() const noexcept {return Y;}
		void SetY(double y) noexcept {Y=y;}
		const unsigned short GetR() const noexcept {return WalkerColor.GetR();}
		const unsigned short GetG() const noexcept {return WalkerColor.GetG();}
		const unsigned short GetB() const noexcept {return WalkerColor.GetB();}
		const unsigned short GetA() const noexcept {return WalkerColor.GetA();}
		const double GetSize() const noexcept {return Size;}
		void SetSize(double s) noexcept {Size=s;}
};

void DrawWalker(SDL_Renderer* renderer, const Walker& walker)
{
	SDL_SetRenderDrawColor(renderer,static_cast<Uint8>(walker.GetR()),static_cast<Uint8>(walker.GetG()),static_cast<Uint8>(walker.GetB()),static_cast<Uint8>(walker.GetA()));
	float x = static_cast<float>(walker.GetX()); float y = static_cast<float>(walker.GetY()); float size = static_cast<float>(walker.GetSize());
	if (size<=1.0f)
	{
		SDL_RenderPoint(renderer,x,y);
	}
	else
	{
		SDL_FRect rect{x-size/2.0f,y-size/2.0f,size,size};
		SDL_RenderFillRect(renderer,&rect);
	}
}

