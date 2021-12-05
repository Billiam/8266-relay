#include <Arduino.h>
#include "fireplace.h"

Fireplace::Fireplace() {
  this->_heatingChanged = false;
  this->_onChanged = false;
}

int Fireplace::temp() {
  return _temp;
}

int Fireplace::targetTemp() {
  return _target_temp;
}

bool Fireplace::on() {
  return _on;
}

bool Fireplace::heating() {
  return _heating;
}

void Fireplace::toggle() {
  if (this->_on) {
    this->setOff();
  } else {
    this->setOn();
    this->setHeating(true);
  }
}

void Fireplace::setTarget(int temperature) {
  if (temperature < 55) {
    this->setOff();
    this->_target_temp = 0;
    return;
  }

  if (temperature > 95) {
    return;
  }

  this->_target_temp = temperature;
  this->setOn();
}

void Fireplace::setTemp(int temperature) {
  this->_temp = temperature;
}

void Fireplace::setHeating(bool heating) {
  if (_heating != heating) {
    this->_heatingChanged = true;
  }
  this->_heating = heating;
}

void Fireplace::setOff() {
  if (_on) {
    this->_onChanged = true;
  }

  this->_target_temp = 0;
  this->setHeating(false);
  this->_on = false;
}

void Fireplace::setOn() {
  if (!_on) {
    this->_onChanged = true;
  }
  this->_on = true;
}

bool Fireplace::onChanged() {
  bool returnVal = _onChanged;
  _onChanged = false;
  return returnVal;
}

bool Fireplace::heatingChanged() {
  bool returnVal = _heatingChanged;
  _heatingChanged = false;
  return returnVal;
}