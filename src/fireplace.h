#ifndef Fireplace_h
#define Fireplace_h

class Fireplace
{
  public:
  Fireplace();

  void setOn();
  void setOff();
  void setTarget(int temperature);
  void setTemp(int temperature);
  void setHeating(bool heating);
  void toggle();

  int temp();
  int targetTemp();
  bool heating();
  bool on();

  bool heatingChanged();
  bool onChanged();

  private:
  
  int _temp;
  int _target_temp;
  bool _heating;
  bool _on;

  bool _heatingChanged;
  bool _onChanged;
};

#endif