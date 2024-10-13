#pragma once

// Base controller class
// by eien86

#include <cstdint>
#include <jaffarCommon/exceptions.hpp>
#include <jaffarCommon/json.hpp>
#include <string>
#include <sstream>
#include "Atari2600Controller.h"

namespace jaffar
{

typedef uint16_t port_t;

struct input_t
{
  bool reset = false;
  bool select = false;
  bool power = false;
  bool leftDifficulty = false;
  bool rightDifficulty = false;
  port_t port1 = 0;
  port_t port2 = 0;
};

class InputParser
{
public:

  enum controller_t { none, gamepad };

  InputParser(const nlohmann::json &config)
  {
    // Parsing controller 1 type
    {
      bool isTypeRecognized = false;
      const auto controller1Type = jaffarCommon::json::getString(config, "Controller 1 Type");

      if (controller1Type == "None")    { _controller1Type = controller_t::none; isTypeRecognized = true; }
      if (controller1Type == "Gamepad") { _controller1Type = controller_t::gamepad;  isTypeRecognized = true; }
      
      if (isTypeRecognized == false) JAFFAR_THROW_LOGIC("Controller 1 type not recognized: '%s'\n", controller1Type.c_str()); 
   }

    // Parsing controller 2 type
    {
      bool isTypeRecognized = false;
      const auto controller2Type = jaffarCommon::json::getString(config, "Controller 2 Type");

      if (controller2Type == "None")    { _controller2Type = controller_t::none; isTypeRecognized = true; }
      if (controller2Type == "Gamepad") { _controller2Type = controller_t::gamepad;  isTypeRecognized = true; }
      
      if (isTypeRecognized == false) JAFFAR_THROW_LOGIC("Controller 2 type not recognized: '%s'\n", controller2Type.c_str()); 
    }
  }

  inline input_t parseInputString(const std::string &inputString) const
  {
    // Storage for the input
    input_t input;

    // Converting input into a stream for parsing
    std::istringstream ss(inputString);

    // Start separator
    if (ss.get() != '|') reportBadInputString(inputString);

    // Parsing console inputs
    parseConsoleInputs(input, ss, inputString);
    
    // Parsing controller 1 inputs
    parseControllerInputs(_controller1Type, input.port1, ss, inputString);

    // Parsing controller 2 inputs
    parseControllerInputs(_controller2Type, input.port2, ss, inputString);

    // End separator
    if (ss.get() != '|') reportBadInputString(inputString);

    // If its not the end of the stream, then extra values remain and its invalid
    ss.get();
    if (ss.eof() == false) reportBadInputString(inputString);
    
    // Returning input
    return input;
  };

  inline void setController1Type(const controller_t type) { _controller1Type = type; }
  inline void setController2Type(const controller_t type) { _controller2Type = type; }

  private:

  static inline void reportBadInputString(const std::string &inputString)
  {
    JAFFAR_THROW_LOGIC("Could not decode input string: '%s'\n", inputString.c_str());
  }

  static void parseGamePadInput(uint16_t& code, std::istringstream& ss, const std::string &inputString)
  {
    // Currently read character
    char c;

    // Cleaning code
    code = 0;

    c = ss.get();
    if (c != '.' && c != 'U') reportBadInputString(inputString);
    if (c == 'U') code |= Atari2600PortButtons::Up;

    c = ss.get();
    if (c != '.' && c != 'D') reportBadInputString(inputString);
    if (c == 'D') code |= Atari2600PortButtons::Down;

    c = ss.get();
    if (c != '.' && c != 'L') reportBadInputString(inputString);
    if (c == 'L') code |= Atari2600PortButtons::Left;

    c = ss.get();
    if (c != '.' && c != 'R') reportBadInputString(inputString);
    if (c == 'R') code |= Atari2600PortButtons::Right;

    c = ss.get();
    if (c != '.' && c != 'B') reportBadInputString(inputString);
    if (c == 'B') code |=Atari2600PortButtons::Button; 
  }

  static void parseControllerInputs(const controller_t type, port_t& port, std::istringstream& ss, const std::string &inputString)
  {
    // If no controller assigned then, its port is all zeroes.
    if (type == controller_t::none) { port = 0; return; }

    // Controller separator
    if (ss.get() != '|') reportBadInputString(inputString);

    // Storage for gamepad's code
    uint16_t code = 0;

    // Parsing gamepad code
    parseGamePadInput(code, ss, inputString);

    // Pushing input code into the port
    port = code;
  }

  static void parseConsoleInputs(input_t& input, std::istringstream& ss, const std::string &inputString)
  {
    // Currently read character
    char c;

    c = ss.get();
    if (c != '.' && c != 'r') reportBadInputString(inputString);
    if (c == 'r') input.reset = true;
    if (c == '.') input.reset = false;

    c = ss.get();
    if (c != '.' && c != 's') reportBadInputString(inputString);
    if (c == 's') input.select = true;
    if (c == '.') input.select = false;

    c = ss.get();
    if (c != '.' && c != 'P') reportBadInputString(inputString);
    if (c == 'P') input.power = true;
    if (c == '.') input.power = false;
    
    c = ss.get();
    if (c != '.' && c != 'l') reportBadInputString(inputString);
    if (c == 'l') input.leftDifficulty = true;
    if (c == '.') input.leftDifficulty = false;

    c = ss.get();
    if (c != '.' && c != 'r') reportBadInputString(inputString);
    if (c == 'r') input.rightDifficulty = true;
    if (c == '.') input.rightDifficulty = false;
  }

  controller_t _controller1Type;
  controller_t _controller2Type;
};

} // namespace jaffar