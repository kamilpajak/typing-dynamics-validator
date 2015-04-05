#include <linux/input.h>
#include <unistd.h> // read()
#include <fcntl.h>  // open()
#include <ncurses.h>
#include <vector>
#include <string>

// --- STRUCTURES --- //

struct keystroke {
  int keyCode;
  double keyDownTime;
  double keyUpTime;
};

// --- FUNCTIONS --- //

// Keystroke Data
std::vector<keystroke> takeKeystrokes(std::vector<input_event> events) {
  std::vector<keystroke> keystrokes;
  for (unsigned int i = 0; i < events.size() - 1; i++)
    if (events[i].value == 1)
      for (unsigned int j = i + 1; j < events.size(); j++)
        if (events[i].code == events[j].code) {
          double keyDownTime =
              events[i].time.tv_sec + (double)events[i].time.tv_usec / 1000000;
          double keyUpTime =
              events[j].time.tv_sec + (double)events[j].time.tv_usec / 1000000;
          keystroke caughtKeystroke;
          caughtKeystroke.keyCode = events[i].code;
          caughtKeystroke.keyDownTime = keyDownTime;
          caughtKeystroke.keyUpTime = keyUpTime;
          keystrokes.push_back(caughtKeystroke);
          break;
        }
  return keystrokes;
}

// Features
std::vector<int> takeKeyCodes(std::vector<keystroke> keystrokes) {
  std::vector<int> keyCodes;
  for (unsigned int i = 0; i < keystrokes.size(); i++)
    keyCodes.push_back(keystrokes[i].keyCode);
  return keyCodes;
}

std::vector<double> takeDownDownLatencies(std::vector<keystroke> keystrokes) {
  std::vector<double> downDownLatencies;
  for (unsigned int i = 1; i < keystrokes.size(); i++) {
    double downDownLatency =
        keystrokes[i].keyDownTime - keystrokes[i - 1].keyDownTime;
    downDownLatencies.push_back(downDownLatency);
  }
  return downDownLatencies;
}

std::vector<double> takeUpDownLatencies(std::vector<keystroke> keystrokes) {
  std::vector<double> upDownLatencies;
  for (unsigned int i = 1; i < keystrokes.size(); i++) {
    double upDownLatency =
        keystrokes[i].keyDownTime - keystrokes[i - 1].keyUpTime;
    upDownLatencies.push_back(upDownLatency);
  }
  return upDownLatencies;
}

std::vector<double> takeDownUpLatencies(std::vector<keystroke> keystrokes) {
  std::vector<double> downUpLatencies;
  for (unsigned int i = 0; i < keystrokes.size(); i++) {
    double downUpLatency = keystrokes[i].keyUpTime - keystrokes[i].keyDownTime;
    downUpLatencies.push_back(downUpLatency);
  }
  return downUpLatencies;
}

// Template

// Classifier

// Others
const int KEY_RELEASED = 0;
const int KEY_PRESSED = 1;
const int KEY_REPEATED = 2;

const int ENTER_CODE = 28;

void printEvent(input_event event) {
  std::string keyState;
  switch (event.value) {
  case KEY_PRESSED:
    keyState = "PRESSED";
    break;
  case KEY_RELEASED:
    keyState = "RELEASED";
    break;
  }
  printf("%-8s | %3d | %ld.%06ld\n", keyState.c_str(), event.code,
         (long)event.time.tv_sec, (long)event.time.tv_usec);
}

std::vector<input_event> getSample(std::string devicePath) {
  std::vector<input_event> events;
  int fileDescriptor = open(devicePath.c_str(), O_RDONLY);
  struct input_event event;
  while (true) {
    read(fileDescriptor, &event, sizeof(struct input_event));
    if (event.type == EV_KEY) {
      if (event.code != ENTER_CODE)
        events.push_back(event);
      else if (event.value == KEY_PRESSED) {
        close(fileDescriptor);
        fileDescriptor = open(devicePath.c_str(), O_TRUNC);
        break;
      }
    }
  }
  close(fileDescriptor);
  return events;
}

// *** MAIN FUNCTION *** //

int main() {
  std::string devicePath = "/dev/input/by-id/"
                           "usb-Microsft_Microsoft_Wireless_Desktop_Receiver_3."
                           "1-event-kbd";
  std::vector<input_event> sample = getSample(devicePath);
  initscr();
  printw("Hello World!");
  refresh();
  getch();
  endwin();

  return 0;
}
