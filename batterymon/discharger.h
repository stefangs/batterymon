

class Discharger {
 public:
   void loop();

 private:
   enum State {idle, discharging} state;
}