#include "ff/textscanner.h"

void test_scanner_whitespace() {
  kl::TextScanner scanner1("     a");
  scanner1.skipWhitespace();
  CHECKST(scanner1.readChar().character == 'a');

  kl::TextScanner scanner2("     ");
  scanner2.skipWhitespace();
  CHECKST(scanner2.empty());

  kl::TextScanner scanner3("x     ");
  scanner3.skipWhitespace();
  CHECKST(scanner3.readChar().character == 'x');
  scanner3.skipWhitespace();
  CHECKST(scanner3.empty());

  kl::TextScanner scanner4("   \n   x     ");
  scanner4.skipWhitespace(kl::NewLineHandling::Keep);
  CHECKST(scanner4.readChar().character == '\n');
  scanner4.rewind();
  scanner4.skipWhitespace(kl::NewLineHandling::Skip);
  CHECKST(scanner4.readChar().character == 'x');
  scanner4.rewind();
  scanner4.skipWhitespace();
  CHECKST(scanner4.readChar().character == 'x');

  kl::log("SCANNER Whitespace [OK]");
}

void test_read_quoted_string() {
  kl::TextScanner scanner1("    \"This is a quoted string\"");
  scanner1.skipWhitespace();
  CHECKST(scanner1.readQuotedString() == "This is a quoted string");
  CHECKST(scanner1.empty());
  scanner1.rewind();
  EXPECTEX<kl::ParsingError>([&scanner1]() { scanner1.readQuotedString(); });

  kl::TextScanner scanner2("\"This is a quoted string\\\" that has a quote in it\"");
  CHECKST(scanner2.readQuotedString() == "This is a quoted string\" that has a quote in it");
  CHECKST(scanner2.empty());

  kl::TextScanner scanner3("\"This is a quoted string that has no end");
  EXPECTEX<kl::ParsingError>([&scanner3]() { scanner3.readQuotedString(); });

  kl::TextScanner scanner4("\"This is a quoted string\" that has something behind it");
  CHECKST(scanner4.readQuotedString() == "This is a quoted string");
  CHECKST(!scanner4.empty());
  CHECKST(scanner4.readChar().character == ' ');
  CHECKST(scanner4.readChar().character == 't');
  CHECKST(scanner4.readChar().character == 'h');
  CHECKST(scanner4.readChar().character == 'a');

  kl::TextScanner scannerEmpty("\"\"");
  CHECKST(scannerEmpty.readQuotedString() == ""_t);

  EXPECTEX<kl::ParsingError>([]() { kl::TextScanner("\"").readQuotedString(); });

  kl::log("SCANNER Quoted [OK]");
}

int main() {
  test_scanner_whitespace();
  test_read_quoted_string();
}
