



void Discharger::Discharger() : state(idle){
}

void Discharger::loop() {
  switch (state) {
    case idle:
      handleIdle();
      break;
    case discharging:
      handleDischarging()
      break;
  }
}