#pragma once

#include <string>
#include <vector>
#include <SDL.h>
#include <jaffarCommon/exceptions.hpp>
#include <jaffarCommon/file.hpp>
#include <jaffarCommon/serializers/contiguous.hpp>
#include <jaffarCommon/deserializers/contiguous.hpp>
#include "../a2600HawkInstanceBase.hpp"
#include "Atari2600Hawk.h"
#include "Atari2600Controller.h"
#include "Atari2600MemoryDomain.h"

namespace libA2600Hawk
{

class EmuInstance : public EmuInstanceBase
{
 public:

 EmuInstance() : EmuInstanceBase()
 {
 }

 ~EmuInstance()
 {
 }

  virtual void initialize() override
  {
	_settings.ShowBG = true;
	_settings.ShowPlayer1 = true;
	_settings.ShowPlayer2 = true;
	_settings.ShowMissle1 = true;
	_settings.ShowMissle2 = true;
	_settings.ShowBall = true;
	_settings.ShowPlayfield = true;
	_settings.SECAMColors = true;
	_settings.NTSCTopLine = 24;
	_settings.NTSCBottomLine = 248;
	_settings.PALTopLine = 24;
	_settings.PALBottomLine = 296;
	_settings.BackgroundColor = 0x000000; 

	_syncSettings.Port1 = Atari2600ControllerTypes::Joystick;
	_syncSettings.Port2 = Atari2600ControllerTypes::Unplugged;
	_syncSettings.BW = false;
	_syncSettings.LeftDifficulty = true;
	_syncSettings.RightDifficulty = true;
	_syncSettings.FastScBios = false;

  _hawkController = Atari2600Controller_Create();
  }

  virtual bool loadROMImpl(const std::string &romFilePath) override
  {
    std::string romData;
    jaffarCommon::file::loadStringFromFile(romData, romFilePath);
   _a2600 = Atari2600Hawk_Create((uint8_t*)romData.data(), romData.size(), &_settings, &_syncSettings);
   _ramDomain = Atari2600Hawk_GetMemoryDomain(_a2600, MainRAM);

    return true;
  }

  void initializeVideoOutput() override
  {
    // Opening rendering window
    SDL_SetMainReady();

   _videoBufferHeight = Atari2600Hawk_GetBufferHeight(_a2600);
   _videoBufferWeight = Atari2600Hawk_GetBufferWidth(_a2600);
   _videoBufferSize = _videoBufferHeight * _videoBufferWeight * sizeof(uint32_t);
   _videoBuffer = malloc(_videoBufferSize);

    // We can only call SDL_InitSubSystem once
    if (!SDL_WasInit(SDL_INIT_VIDEO))
      if (SDL_InitSubSystem(SDL_INIT_VIDEO) != 0) JAFFAR_THROW_LOGIC("Failed to initialize video: %s", SDL_GetError());

    m_window = SDL_CreateWindow("JaffarPlus", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, _videoBufferWeight, _videoBufferHeight, SDL_WINDOW_RESIZABLE);
    if (m_window == nullptr) JAFFAR_THROW_LOGIC("Coult not open SDL window");

    // Creating SDL renderer
    if (!(m_renderer = SDL_CreateRenderer(m_window, -1, SDL_RENDERER_ACCELERATED))) JAFFAR_THROW_RUNTIME("Coult not create SDL renderer in NES emulator");
    if (!(m_tex = SDL_CreateTexture(m_renderer, SDL_PIXELFORMAT_ARGB8888, SDL_TEXTUREACCESS_STREAMING, _videoBufferWeight, _videoBufferHeight)))
      JAFFAR_THROW_RUNTIME("Coult not create SDL texture in NES emulator");
    SDL_SetRenderDrawColor(m_renderer, 0, 0, 0, 255);
  }

  void finalizeVideoOutput() override
  {
    if (m_tex) SDL_DestroyTexture(m_tex);
    if (m_renderer) SDL_DestroyRenderer(m_renderer);
    if (m_window) SDL_DestroyWindow(m_window);
    free(_videoBuffer);
  }

  void enableRendering() override
  {
    _doRendering = true;
  }

  void disableRendering() override
  {
    _doRendering = false;
  }

  void serializeState(jaffarCommon::serializer::Base& s) const override
  {
    void* buffer = s.getOutputDataBuffer();
    Atari2600Hawk_SaveStateBinary(_a2600, (uint8_t*)buffer, _stateSize);
    s.pushContiguous(nullptr, _stateSize);
  }

  void deserializeState(jaffarCommon::deserializer::Base& d) override
  {
    Atari2600Hawk_LoadStateBinary(_a2600, (uint8_t*)d.getInputDataBuffer(), _stateSize);
    d.popContiguous(nullptr, _stateSize);
  }

  size_t getStateSizeImpl() const override
  {
    return Atari2600Hawk_SaveStateBinary(_a2600, nullptr, 0);
  }

  uint8_t getWorkRamByte(size_t pos) const override
  {
    return Atari2600MemoryDomain_PeekByte(_ramDomain, pos);
  }

  void updateRenderer() override
  {
    int pitch = 0;
    void* pixels;

    if (SDL_LockTexture(m_tex, nullptr, &pixels, &pitch) < 0) JAFFAR_THROW_RUNTIME("Coult not lock texture");
    memcpy(pixels, _videoBuffer, _videoBufferSize);
    SDL_UnlockTexture(m_tex);

    const SDL_Rect BLIT_RECT = {0, 0, (int)_videoBufferWeight, (int)_videoBufferHeight};
    SDL_RenderClear(m_renderer);
    SDL_RenderCopy(m_renderer, m_tex, &BLIT_RECT, &BLIT_RECT);
    SDL_RenderPresent(m_renderer);
  }

  inline size_t getDifferentialStateSizeImpl() const override { return getStateSizeImpl(); }

  void enableStateBlockImpl(const std::string& block) override
  {
    JAFFAR_THROW_LOGIC("State block name: '%s' not found.", block.c_str());
  }

  void disableStateBlockImpl(const std::string& block) override
  {
    JAFFAR_THROW_LOGIC("State block name: '%s' not found", block.c_str());
  }

  void doSoftReset() override
  {
  }
  
  void doHardReset() override
  {
  }

  void* getVideoBuffer() const { return _videoBuffer; }
  size_t getVideoBufferSize() const { return _videoBufferSize; }

  std::string getCoreName() const override { return "libA2600Hawk"; }


  void advanceStateImpl(libA2600Hawk::Controller controller) override
  {
    Atari2600Inputs hawkInputs;
    hawkInputs.P1Buttons = (Atari2600PortButtons)controller.getController1Code();
    uint32_t consoleButtons = 0;
    if (controller.getPowerButtonState())     consoleButtons |= Atari2600ConsoleButtons::Power;
    if (controller.getResetButtonState())     consoleButtons |= Atari2600ConsoleButtons::Reset;
    if (controller.getSelectButtonState())    consoleButtons |= Atari2600ConsoleButtons::Select;
    if (controller.getLeftDifficultyState())  consoleButtons |= Atari2600ConsoleButtons::ToggleLeftDifficulty;
    if (controller.getRightDifficultyState()) consoleButtons |= Atari2600ConsoleButtons::ToggleRightDifficulty;
    hawkInputs.ConsoleButtons = (Atari2600ConsoleButtons)consoleButtons;

    Atari2600Controller_SetInputs(_hawkController, &hawkInputs);
    Atari2600Hawk_FrameAdvance(_a2600, _hawkController, _doRendering, false);
    if (_doRendering)  Atari2600Hawk_GetVideoBuffer(_a2600, (uint32_t*)_videoBuffer);
  }

  private:

  // Window pointer
  SDL_Window *m_window;

  // Renderer
  SDL_Renderer *m_renderer;

  // SDL Textures
  SDL_Texture *m_tex;
  
  struct Atari2600Settings _settings;
  struct Atari2600SyncSettings _syncSettings;
  struct Atari2600Controller* _hawkController;
  struct Atari2600MemoryDomain* _ramDomain;
  
  Atari2600Hawk* _a2600;

  uint32_t _videoBufferHeight;
  uint32_t _videoBufferWeight;
  void* _videoBuffer;
  size_t _videoBufferSize;

  bool _doRendering = false;
};

} // namespace libA2600Hawk