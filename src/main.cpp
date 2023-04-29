#include <iostream>
#include <thread>

#include "work_thread.h"

int main() {
  hevake::WorkThread worker;

  /// 连续派发10个任务给worker
  for (int i = 0; i < 10; ++i) {
    worker.execute(
      [i] {
        std::cout << "worker: run " << i << std::endl;
      }
    );
  }

  bool all_done = false;
  /// 最后派发任务，让worker将all_done置为true
  worker.execute(
    [&] {
      std::cout << "worker: all done" << std::endl;
      all_done = true;
    }
  );

  std::cout << "main: waiting... " << std::endl;
  while (!all_done)
    std::this_thread::yield();
  std::cout << "main: stop" << std::endl;

  return 0;
}
