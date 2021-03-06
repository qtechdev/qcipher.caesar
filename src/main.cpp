#include <cstdio>
#include <iostream>
#include <optional>
#include <string>

#include <qfio/qfio.hpp>
#include <qarg/qarg.hpp>

#include "standard.hpp"
#include "running.hpp"

char caesar(const int c, int count);
char caesar(const int c, int count, bool &changed);
std::string caesar(const std::string &plain, int count);
std::string runningkey(
  const std::string &plain, const std::string &key, const bool reverse
);

int main(int argc, const char *argv[]) {
  qarg::parser args;
  args.add<bool>('h', "display this text");
  args.add<int>('s', "shift value/key", true);
  args.add<bool>('u', "unshift");
  args.add<std::string>('f', "file path");
  args.add<std::string>('t', "text");
  args.add<bool>('r', "use running key");

  try {
    args.parse(argc, argv);
  } catch (std::invalid_argument &e) {
    std::cerr << e.what() << "\n";
    return 1;
  }

  const bool help = *args.get<bool>('h');
  if (help) {
    std::cout << args.help();
    return 0;
  }

  // get text
  std::string plain_text;
  const std::optional<std::string> file = args.get<std::string>('f');
  const std::optional<std::string> text = args.get<std::string>('t');
  if (file) {
    const auto file_text = qfio::read(*file);
    if (!file_text) {
      std::cerr << "could not read file\n";
      return 3;
    } else {
      plain_text = *file_text;
    }
  } else if (text) {
    plain_text = *text;
  } else {
    std::cerr << "missing argument, -f or -t\n";
    return 2;
  }

  std::string cipher_text;
  const bool reverse = *args.get<bool>('u');
  const bool use_running_key = *args.get<bool>('r');

  if (use_running_key) {
    const std::string key = *args.get<std::string>('s');
    cipher_text = runningkey(plain_text, key, reverse);
  } else { /* Standard Caesar Shift*/
    int count = 0;
    const int shift = *args.get<int>('s');
    count = shift;

    if (reverse) {
      count *= -1;
    }

    cipher_text = caesar(plain_text, count);
  }

  std::cout << "Plain Text  : " << plain_text << "\n";
  std::cout << "Cipher Text : " << cipher_text << "\n";

  return 0;
}
