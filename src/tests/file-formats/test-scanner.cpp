#include "ff/textscanner.h"
using namespace kl::literals;

void test_line_column() {
  kl::TextScanner sc("   \n   x     ");
  sc.skipWhitespace(kl::NewLineHandling::Keep);
  CHECKST(sc.line() == 1);
  CHECKST(sc.column() == 4);
  CHECKST(sc.read_char().character == '\n');
  CHECKST(sc.line() == 2);
  CHECKST(sc.column() == 1);
  sc.rewind();
  CHECKST(sc.line() == 1);
  CHECKST(sc.column() == 1);
  sc.skipWhitespace(kl::NewLineHandling::Skip);
  CHECKST(sc.line() == 2 && sc.column() == 4);
  CHECKST(sc.read_char().character == 'x');
  CHECKST(sc.line() == 2 && sc.column() == 5);
  sc.skipWhitespace();
  CHECKST(sc.line() == 2 && sc.column() == 10);

  kl::log("SCANNER line/column [OK]");
}

void test_scanner_whitespace() {
  kl::TextScanner scanner1("     a");
  scanner1.skipWhitespace();
  CHECKST(scanner1.read_char().character == 'a');

  kl::TextScanner scanner2("     ");
  scanner2.skipWhitespace();
  CHECKST(scanner2.empty());

  kl::TextScanner scanner3("x     ");
  scanner3.skipWhitespace();
  CHECKST(scanner3.read_char().character == 'x');
  scanner3.skipWhitespace();
  CHECKST(scanner3.empty());

  kl::TextScanner scanner4("   \n   x     ");
  scanner4.skipWhitespace(kl::NewLineHandling::Keep);
  CHECKST(scanner4.read_char().character == '\n');
  scanner4.rewind();
  scanner4.skipWhitespace(kl::NewLineHandling::Skip);
  CHECKST(scanner4.read_char().character == 'x');
  scanner4.rewind();
  scanner4.skipWhitespace();
  CHECKST(scanner4.read_char().character == 'x');

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
  CHECKST(scanner4.read_char().character == ' ');
  CHECKST(scanner4.read_char().character == 't');
  CHECKST(scanner4.read_char().character == 'h');
  CHECKST(scanner4.read_char().character == 'a');

  kl::TextScanner scannerEmpty("\"\"");
  CHECKST(scannerEmpty.readQuotedString() == ""_t);

  kl::TextScanner scannerEsc("\"This text contains escaped characters like:\\n- newlines\\n-\\ttabs\"");
  CHECKST(scannerEsc.readQuotedString() == "This text contains escaped characters like:\n- newlines\n-\ttabs"_t);

  EXPECTEX<kl::ParsingError>([]() { kl::TextScanner("\"").readQuotedString(); });

  kl::log("SCANNER Quoted [OK]");
}

void test_read_word() {
  kl::TextScanner scanner1("This word");
  CHECKST(scanner1.readWord() == "This");
  CHECKST(scanner1.readWord() == "");
  CHECKST(scanner1.read_char().character == ' ');
  CHECKST(scanner1.readWord() == "word");
  CHECKST(scanner1.empty());
  kl::TextScanner scanner2("Thisword");
  CHECKST(scanner2.readWord() == "Thisword");
  CHECKST(scanner2.empty());

  kl::TextScanner scanner3("This_long012_ISword indeed");
  CHECKST(scanner3.readWord() == "This_long012_ISword");
  CHECKST(scanner3.read_char().character == ' ');
  CHECKST(scanner3.readWord() == "indeed");
  CHECKST(scanner3.empty());

  kl::TextScanner scanner4("This_long\\012_ISword indeed");
  CHECKST(scanner4.readWord() == "This_long");

  kl::TextScanner scanner5("This_long\0_12_ISword indeed");
  CHECKST(scanner5.readWord() == "This_long");

  kl::log("SCANNER Word [OK]");
}

void test_read_escaped_char() {
  kl::TextScanner scanner1("This_long\\012_ISword indeed");
  CHECKST(scanner1.readWord() == "This_long");
  CHECKST(scanner1.readCharEscaped().character == '\0');

  kl::TextScanner scanner2("\t\\t\\0\\n\n");
  auto ch = scanner2.readCharEscaped();
  CHECKST(ch.character == '\t');
  CHECKST(!ch.escaped);
  ch = scanner2.readCharEscaped();
  CHECKST(ch.character == '\t');
  CHECKST(ch.escaped);
  ch = scanner2.readCharEscaped();
  CHECKST(ch.character == '\0');
  CHECKST(ch.escaped);
  ch = scanner2.readCharEscaped();
  CHECKST(ch.character == '\n');
  CHECKST(ch.escaped);
  ch = scanner2.readCharEscaped();
  CHECKST(ch.character == '\n');
  CHECKST(!ch.escaped);
  CHECKST(scanner2.empty());

  kl::log("SCANNER read escaped [OK]");
}

void test_read_new_line() {
  kl::TextScanner scanner1("A long line\nand all good");
  CHECKST(scanner1.read_line() == "A long line");
  CHECKST(scanner1.read_line() == "and all good");
  CHECKST(scanner1.empty());

  kl::log("SCANNER read new line [OK]");
}

void test_indentation() {
  kl::TextScanner sc1("      text");
  CHECKST(sc1.getIndentationLevel() == 6);
  sc1.read_char();
  CHECKST(sc1.getIndentationLevel() == 5);
  sc1.skipWhitespace();
  CHECKST(sc1.getIndentationLevel() == 0);
  CHECKST(sc1.readWord() == "text");
  CHECKST(sc1.empty());
  CHECKST(sc1.getIndentationLevel() == 0);

  kl::TextScanner sc2("  \n    ");
  CHECKST(sc2.getIndentationLevel() == 2);
  sc2.read_line();
  CHECKST(sc2.getIndentationLevel() == 4);

  kl::log("SCANNER indentation [OK]");
}

void test_remainder() {
  kl::TextScanner scanner1("This word");
  CHECKST(scanner1.remainder() == "This word");
  CHECKST(scanner1.readWord() == "This");
  CHECKST(scanner1.remainder() == " word");
  CHECKST(scanner1.read_char().character == ' ');
  CHECKST(scanner1.remainder() == "word");
  CHECKST(scanner1.readWord() == "word");
  CHECKST(scanner1.empty());
  CHECKST(scanner1.remainder() == ""_t);

  kl::log("SCANNER remainder [OK]");
}

void test_expect() {
  kl::TextScanner sc("This word");
  sc.expect('T');
  sc.expectws('h');
  sc.expect('i');
  EXPECTEX<kl::ParsingError>([&sc]() { sc.expect('k'); });
  sc.expect('s');
  auto location = sc.location();
  sc.expect(' ');
  sc.expect('w');
  sc.restoreLocation(location);
  sc.expectws('w');
  sc.expectws('o');
  sc.expectws('r');
  sc.expect('d');
  CHECKST(sc.empty());
  EXPECTEX<kl::ParsingError>([&sc]() { sc.expect('x'); });

  kl::log("SCANNER expect [OK]");
}

void test_read_until() {
  kl::TextScanner sc("This word");
  CHECKST(sc.readUntil('s') == "Thi"_t);
  CHECKST(sc.topChar() == ' ');
  CHECKST(sc.readUntil(' ') == ""_t);
  CHECKST(sc.readUntil('r') == "wo");

  EXPECTEX<kl::ParsingError>([&sc]() { sc.readUntil('k'); });
  CHECKST(sc.empty());
  sc.rewind();

  EXPECTEX<kl::ParsingError>([&sc]() { sc.readUntil('k'); });
  CHECKST(sc.empty());
  sc.rewind();

  CHECKST(sc.readUntil('d') == "This wor");

  kl::log("SCANNER readUntil [OK]");
}

int main() {
  test_line_column();
  test_scanner_whitespace();
  test_read_quoted_string();
  test_read_word();
  test_read_escaped_char();
  test_read_new_line();
  test_indentation();
  test_remainder();
  test_expect();
  test_read_until();
}
