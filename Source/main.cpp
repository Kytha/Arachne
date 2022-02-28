#include <iostream>
#include "Scheduler.h"

void task(void* arg) {

}

int main() {
  Scheduler::Start();
  char x;
  std::cin >> x;
  for(int i = 0; i < 100; i ++)
  {
    Scheduler::CreateTask(task,(void*)nullptr);
  }
  std::cin >> x;
  Scheduler::Stop();
}