#pragma once

#include <string>
#include <vector>
#include <jaffarCommon/exceptions.hpp>
#include <jaffarCommon/serializers/contiguous.hpp>
#include <jaffarCommon/deserializers/contiguous.hpp>
#include "../a2600HawkInstanceBase.hpp"

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
  }

  virtual bool loadROMImpl(const std::string &romFilePath) override
  {
    
    return true;
  }

  void initializeVideoOutput() override
  {
  }

  void finalizeVideoOutput() override
  {
  }

  void enableRendering() override
  {
  }

  void disableRendering() override
  {
  }

  void serializeState(jaffarCommon::serializer::Base& s) const override
  {
  }

  void deserializeState(jaffarCommon::deserializer::Base& d) override
  {
  }

  size_t getStateSizeImpl() const override
  {
    return 0;
  }

  uint8_t* getWorkRamPointer() const override
  {
    return nullptr;
  }

  void updateRenderer() override
  {
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

  std::string getCoreName() const override { return "Quicker libA2600Hawk"; }


  void advanceStateImpl(libA2600Hawk::Controller controller) override
  {
    const auto controller1 = controller.getController1Code();
  }

  private:

  std::string _romData;
  const uint8_t* _ram;
};

} // namespace libA2600Hawk