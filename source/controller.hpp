#pragma once

// Base controller class
// by eien86

#include <cstdint>
#include <string>
#include <sstream>

namespace libA2600Hawk
{

class Controller
{
public:

  enum controller_t { none, gamepad };

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

  inline bool parseInputString(const std::string& input)
  {
    // Parse valid flag
    bool isValid = true;

    // Converting input into a stream for parsing
    std::istringstream ss(input);

    // Start separator
    if (ss.get() != '|') isValid = false;

    // Parsing console inputs
    isValid &= parseConsoleInputs(_input, ss);
    
    // Parsing controller 1 inputs
    isValid &= parseControllerInputs(_controller1Type, _input.port1, ss);

    // Parsing controller 2 inputs
    isValid &= parseControllerInputs(_controller2Type, _input.port2, ss);

    // End separator
    if (ss.get() != '|') isValid = false;

    // If its not the end of the stream, then extra values remain and its invalid
    ss.get();
    if (ss.eof() == false) isValid = false;
    
    // Returning valid flag 
    return isValid;
  };

  inline void setController1Type(const controller_t type) { _controller1Type = type; }
  inline void setController2Type(const controller_t type) { _controller2Type = type; }

  inline bool getResetButtonState() { return _input.reset; }
  inline bool getSelectButtonState() { return _input.select; }
  inline bool getPowerButtonState() { return _input.power; }
  inline bool getLeftDifficultyState() { return _input.leftDifficulty; }
  inline bool getRightDifficultyState() { return _input.rightDifficulty; }
  inline port_t getController1Code() { return _input.port1; }
  inline port_t getController2Code() { return _input.port2; }

  private:

  static bool parseGamePadInput(uint16_t& code, std::istringstream& ss)
  {
    // Currently read character
    char c;

    // Cleaning code
    code = 0;

    c = ss.get();
    if (c != '.' && c != 'U') return false;
    if (c == 'U') code |= 0b00000001;

    c = ss.get();
    if (c != '.' && c != 'D') return false;
    if (c == 'D') code |= 0b00000010;

    c = ss.get();
    if (c != '.' && c != 'L') return false;
    if (c == 'L') code |= 0b00000100;

    c = ss.get();
    if (c != '.' && c != 'R') return false;
    if (c == 'R') code |= 0b00001000;

    c = ss.get();
    if (c != '.' && c != 'B') return false;
    if (c == 'B') code |= 0b00100000;

    return true;
  }

  static bool parseControllerInputs(const controller_t type, port_t& port, std::istringstream& ss)
  {
    // Parse valid flag
    bool isValid = true; 
 
    // If no controller assigned then, its port is all zeroes.
    if (type == controller_t::none) { port = 0; return true; }

    // Controller separator
    if (ss.get() != '|') isValid = false;

    // Storage for gamepad's code
    uint16_t code = 0;

    // Parsing gamepad code
    isValid &= parseGamePadInput(code, ss);

    // Pushing input code into the port
    port = code;

    // Return valid flag
    return isValid;
  }

  static bool parseConsoleInputs(input_t& input, std::istringstream& ss)
  {
    // Parse valid flag
    bool isValid = true; 

    // Currently read character
    char c;

    c = ss.get();
    if (c != '.' && c != 'r') isValid = false;
    if (c == 'r') input.reset = true;
    if (c == '.') input.reset = false;

    c = ss.get();
    if (c != '.' && c != 's') isValid = false;
    if (c == 's') input.select = true;
    if (c == '.') input.select = false;

    c = ss.get();
    if (c != '.' && c != 'P') isValid = false;
    if (c == 'P') input.power = true;
    if (c == '.') input.power = false;
    
    c = ss.get();
    if (c != '.' && c != 'l') isValid = false;
    if (c == 'l') input.leftDifficulty = true;
    if (c == '.') input.leftDifficulty = false;

    c = ss.get();
    if (c != '.' && c != 'r') isValid = false;
    if (c == 'r') input.rightDifficulty = true;
    if (c == '.') input.rightDifficulty = false;

    // Return valid flag
    return isValid;
  }

  input_t _input;
  controller_t _controller1Type;
  controller_t _controller2Type;
};

} // namespace libA2600Hawk