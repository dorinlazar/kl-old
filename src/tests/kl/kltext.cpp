#include "kl/kltext.h"
#include <gtest/gtest.h>
using namespace kl;
using namespace std::literals;
using namespace kl::literals;

TEST(kltext, test_text_construction) {
  Text txt("Hello world");
  auto t2 = Text(txt, 4, 20);
  EXPECT_TRUE(t2.toString() == "o world"s);
  EXPECT_TRUE(t2.size() == 7);

  Text tnull(nullptr, 2, 4);
  EXPECT_TRUE(tnull.size() == 0);
  EXPECT_TRUE(tnull.toString() == ""s);
  auto tn2 = Text(tnull, 2, 4);
  EXPECT_TRUE(tn2.size() == 0);
  EXPECT_TRUE(tn2.toString() == ""s);

  Text tempty(""s);
  EXPECT_TRUE(tempty.size() == 0);
  EXPECT_TRUE(tempty.toString() == ""s);

  Text t1("Hello World", 2);
  EXPECT_TRUE(t1.size() == 2);
  EXPECT_TRUE(t1.toString() == "He"s);

  Text t3("Hello World\0", 12);
  EXPECT_TRUE(t3.size() == 12);
  EXPECT_TRUE(t3[11] == '\0');

  Text t4("Hello World\0Other World", 23);
  EXPECT_TRUE(t4.size() == 23);
  EXPECT_TRUE(t4[11] == '\0');
  EXPECT_TRUE(t4.subpos(12, 24) == "Other World");
}

TEST(kltext, test_trimming) {
  Text txt(" \t hello   \n ");
  EXPECT_TRUE(txt.trim().toString() == "hello"s);
  EXPECT_TRUE(txt.trimLeft().toString() == "hello   \n "s);
  EXPECT_TRUE(txt.trimRight().toString() == " \t hello"s);

  Text txt2("hello");
  EXPECT_TRUE(txt2.trim().toString() == "hello"s);
  EXPECT_TRUE(txt2.trimLeft().toString() == "hello"s);
  EXPECT_TRUE(txt2.trimRight().toString() == "hello"s);

  Text txt3;
  EXPECT_TRUE(txt3.trim().toString() == ""s);
  EXPECT_TRUE(txt3.trimLeft().toString() == ""s);
  EXPECT_TRUE(txt3.trimRight().toString() == ""s);
}

TEST(kltext, test_starts_with) {
  Text txt("Hello");
  EXPECT_TRUE(txt.startsWith(""));
  EXPECT_TRUE(txt.startsWith("H"));
  EXPECT_TRUE(txt.startsWith("Hell"));
  EXPECT_TRUE(txt.startsWith("Hello"));
  EXPECT_TRUE(!txt.startsWith("Hello world"));
  EXPECT_TRUE(!txt.startsWith("world"));

  EXPECT_TRUE(txt.startsWith(""_t));
  EXPECT_TRUE(txt.startsWith("H"_t));
  EXPECT_TRUE(txt.startsWith("Hell"_t));
  EXPECT_TRUE(txt.startsWith("Hello"_t));
  EXPECT_TRUE(!txt.startsWith("Hello world"_t));
  EXPECT_TRUE(!txt.startsWith("world"_t));
}

TEST(kltext, test_ends_with) {
  Text txt("Hello");
  EXPECT_TRUE(txt.endsWith(""));
  EXPECT_TRUE(txt.endsWith("o"));
  EXPECT_TRUE(txt.endsWith("ello"));
  EXPECT_TRUE(txt.endsWith("Hello"));
  EXPECT_TRUE(!txt.endsWith("Hello world"));
  EXPECT_TRUE(!txt.endsWith("world"));
}

TEST(kltext, test_comparisons) {
  Text hello("hello");
  Text hfllo("hfllo");
  Text empty;

  EXPECT_TRUE(hello == "hello");
  EXPECT_TRUE(hello < "hfllo");
  EXPECT_TRUE(hello <= "hfllo");
  EXPECT_TRUE(hello != "hfllo");
  EXPECT_TRUE(hello >= "hallo");
  EXPECT_TRUE(hello >= "ahllo");
  EXPECT_TRUE(hello > "hallo");
  EXPECT_TRUE(hello > "hell");
  EXPECT_TRUE(hello < "hello world");
  EXPECT_TRUE(hello > "");
  EXPECT_TRUE(hello > nullptr);

  EXPECT_TRUE(hello == "hello"s);
  EXPECT_TRUE(hello < "hfllo"s);
  EXPECT_TRUE(hello <= "hfllo"s);
  EXPECT_TRUE(hello != "hfllo"s);
  EXPECT_TRUE(hello >= "hallo"s);
  EXPECT_TRUE(hello >= "ahllo"s);
  EXPECT_TRUE(hello > "hallo"s);
  EXPECT_TRUE(hello > "hell"s);
  EXPECT_TRUE(hello < "hello world"s);
  EXPECT_TRUE(hello > ""s);

  EXPECT_TRUE(hello == "hello"_t);
  EXPECT_TRUE(hello < "hfllo"_t);
  EXPECT_TRUE(hello <= "hfllo"_t);
  EXPECT_TRUE(hello != "hfllo"_t);
  EXPECT_TRUE(hello >= "hallo"_t);
  EXPECT_TRUE(hello >= "ahllo"_t);
  EXPECT_TRUE(hello > "hallo"_t);
  EXPECT_TRUE(hello > "hell"_t);
  EXPECT_TRUE(hello < "hello world"_t);
  EXPECT_TRUE(hello > ""_t);
}

TEST(kltext, test_split) {
  Text txt("hello");
  EXPECT_TRUE(get<0>(txt.splitPos(0)) == "");
  EXPECT_TRUE(get<1>(txt.splitPos(0)) == "hello");
  EXPECT_TRUE(get<0>(txt.splitPos(1)) == "h");
  EXPECT_TRUE(get<1>(txt.splitPos(1)) == "ello");
  EXPECT_TRUE(get<0>(txt.splitPos(2)) == "he");
  EXPECT_TRUE(get<1>(txt.splitPos(2)) == "llo");
  EXPECT_TRUE(get<0>(txt.splitPos(-2)) == "hel");
  EXPECT_TRUE(get<1>(txt.splitPos(-2)) == "lo");
  EXPECT_TRUE(get<0>(txt.splitPos(-1)) == "hell");
  EXPECT_TRUE(get<1>(txt.splitPos(-1)) == "o");

  EXPECT_TRUE(get<0>(txt.splitPos(-5)) == "");
  EXPECT_TRUE(get<1>(txt.splitPos(-5)) == "hello");
  EXPECT_TRUE(get<0>(txt.splitPos(-10)) == "");
  EXPECT_TRUE(get<1>(txt.splitPos(-10)) == "hello");

  EXPECT_TRUE(get<0>(txt.splitPos(5)) == "hello");
  EXPECT_TRUE(get<1>(txt.splitPos(5)) == "");
  EXPECT_TRUE(get<0>(txt.splitPos(15)) == "hello");
  EXPECT_TRUE(get<1>(txt.splitPos(15)) == "");
  EXPECT_TRUE(get<0>(""_t.splitPos(15)) == "");
  EXPECT_TRUE(get<1>(""_t.splitPos(15)) == "");
}

TEST(kltext, test_contains) {
  Text t("hello");
  EXPECT_TRUE(t.contains('h'));
  EXPECT_TRUE(t.contains('e'));
  EXPECT_TRUE(t.contains('o'));
  EXPECT_TRUE(t.contains('l'));
  EXPECT_TRUE(!t.contains('a'));
  EXPECT_TRUE(!t.contains('\0'));
  EXPECT_TRUE(!""_t.contains('\0'));
}

TEST(kltext, test_skip) {
  Text t("hello");
  EXPECT_TRUE(t.skip("hel") == "o");
  EXPECT_TRUE(t.skip("el") == "hello");
  EXPECT_TRUE(t.skip("") == "hello");
  EXPECT_TRUE(t.skip("abcdefghijklmnopqrstuvwxyz") == "");
  EXPECT_TRUE(""_t.skip("abcdefghijklmnopqrstuvwxyz") == "");

  EXPECT_TRUE(t.skip(2) == "llo"_t);
  EXPECT_TRUE(t.skip(1) == "ello"_t);
  EXPECT_TRUE(t.skip(4) == "o"_t);
  EXPECT_TRUE(t.skip(5) == ""_t);
  EXPECT_TRUE(t.skip(10) == ""_t);
  EXPECT_TRUE(t.skip(100) == ""_t);
}

TEST(kltext, test_pos) {
  Text t("hello world");
  EXPECT_TRUE(t.pos('l') == 2);
  EXPECT_TRUE(t.pos('l', 2) == 3);
  EXPECT_TRUE(t.pos('l', 3) == 9);
  EXPECT_TRUE(t.pos('d') == 10);
  EXPECT_TRUE(t.pos(' ') == 5);
  EXPECT_TRUE(!t.pos('o', 3).has_value());
  EXPECT_TRUE(!t.pos('k').has_value());

  EXPECT_TRUE(!""_t.pos('x').has_value());

  EXPECT_TRUE(t.pos("l"_t) == 2);
  EXPECT_TRUE(t.pos("ll"_t) == 2);
  EXPECT_TRUE(t.pos("l"_t, 2) == 3);
  EXPECT_TRUE(t.pos("l"_t, 3) == 9);
  EXPECT_TRUE(t.pos("o w"_t) == 4);
  EXPECT_TRUE(t.pos("d"_t) == 10);
  EXPECT_TRUE(!t.pos("o W"_t).has_value());
  EXPECT_TRUE(!t.pos(""_t).has_value());
  EXPECT_TRUE(!t.pos("hello world!"_t).has_value());
  EXPECT_TRUE(t.pos("hello w"_t) == 0);
  EXPECT_TRUE(t.pos("hello world"_t) == 0);

  EXPECT_TRUE(!""_t.pos("x"_t).has_value());
  EXPECT_TRUE(!""_t.pos(""_t).has_value());
}

TEST(kltext, test_split_char) {
  Text t1("My,CSV,text,with,,an,ending,,");
  auto sp1 = t1.splitByChar(',');

  EXPECT_TRUE(sp1.size() == 6);
  EXPECT_TRUE(sp1[0] == "My"_t);
  EXPECT_TRUE(sp1[1] == "CSV"_t);
  EXPECT_TRUE(sp1[2] == "text"_t);
  EXPECT_TRUE(sp1[3] == "with"_t);
  EXPECT_TRUE(sp1[4] == "an"_t);
  EXPECT_TRUE(sp1[5] == "ending"_t);

  auto sp2 = t1.splitByChar(',', SplitEmpty::Keep);
  EXPECT_TRUE(sp2.size() == 9);
  EXPECT_TRUE(sp2[0] == "My"_t);
  EXPECT_TRUE(sp2[1] == "CSV"_t);
  EXPECT_TRUE(sp2[2] == "text"_t);
  EXPECT_TRUE(sp2[3] == "with"_t);
  EXPECT_TRUE(sp2[4] == ""_t);
  EXPECT_TRUE(sp2[5] == "an"_t);
  EXPECT_TRUE(sp2[6] == "ending"_t);
  EXPECT_TRUE(sp2[7] == ""_t);
  EXPECT_TRUE(sp2[8] == ""_t);

  auto sp3 = "Hello World"_t.splitByChar(' ');
  EXPECT_TRUE(sp3.size() == 2);
  EXPECT_TRUE(sp3[0] == "Hello"_t);
  EXPECT_TRUE(sp3[1] == "World"_t);

  sp3 = "   Hello World"_t.splitByChar(' ');
  EXPECT_TRUE(sp3.size() == 2);
  EXPECT_TRUE(sp3[0] == "Hello"_t);
  EXPECT_TRUE(sp3[1] == "World"_t);

  sp3 = "   Hello World"_t.splitByChar(' ', SplitEmpty::Keep);
  EXPECT_TRUE(sp3.size() == 5);
  EXPECT_TRUE(sp3[0] == ""_t);
  EXPECT_TRUE(sp3[1] == ""_t);
  EXPECT_TRUE(sp3[2] == ""_t);
  EXPECT_TRUE(sp3[3] == "Hello"_t);
  EXPECT_TRUE(sp3[4] == "World"_t);

  auto sp4 = "Hello_World"_t.splitByChar(' ');
  EXPECT_TRUE(sp4.size() == 1);
  EXPECT_TRUE(sp4[0] == "Hello_World"_t);
}

TEST(kltext, test_split_text) {
  Text t1("This||is||||||some||text||||");
  auto sp1 = t1.splitByText("||"_t);

  EXPECT_TRUE(sp1.size() == 4);
  EXPECT_TRUE(sp1[0] == "This"_t);
  EXPECT_TRUE(sp1[1] == "is"_t);
  EXPECT_TRUE(sp1[2] == "some"_t);
  EXPECT_TRUE(sp1[3] == "text"_t);

  auto sp2 = t1.splitByText("||"_t, SplitEmpty::Keep);
  EXPECT_TRUE(sp2.size() == 8);
  EXPECT_TRUE(sp2[0] == "This"_t);
  EXPECT_TRUE(sp2[1] == "is"_t);
  EXPECT_TRUE(sp2[2] == ""_t);
  EXPECT_TRUE(sp2[3] == ""_t);
  EXPECT_TRUE(sp2[4] == "some"_t);
  EXPECT_TRUE(sp2[5] == "text"_t);
  EXPECT_TRUE(sp2[6] == ""_t);
  EXPECT_TRUE(sp2[7] == ""_t);

  auto sp3 = "Hello World"_t.splitByText(" "_t);
  EXPECT_TRUE(sp3.size() == 2);
  EXPECT_TRUE(sp3[0] == "Hello"_t);
  EXPECT_TRUE(sp3[1] == "World"_t);

  sp3 = "   Hello World"_t.splitByText(" "_t);
  EXPECT_TRUE(sp3.size() == 2);
  EXPECT_TRUE(sp3[0] == "Hello"_t);
  EXPECT_TRUE(sp3[1] == "World"_t);

  sp3 = "   Hello World "_t.splitByText(" "_t, SplitEmpty::Keep);
  EXPECT_TRUE(sp3.size() == 6);
  EXPECT_TRUE(sp3[0] == ""_t);
  EXPECT_TRUE(sp3[1] == ""_t);
  EXPECT_TRUE(sp3[2] == ""_t);
  EXPECT_TRUE(sp3[3] == "Hello"_t);
  EXPECT_TRUE(sp3[4] == "World"_t);
  EXPECT_TRUE(sp3[5] == ""_t);

  auto sp4 = "Hello_World"_t.splitByText(" "_t);
  EXPECT_TRUE(sp4.size() == 1);
  EXPECT_TRUE(sp4[0] == "Hello_World"_t);

  sp4 = "Hello"_t.splitByText("HelloWorld"_t);
  EXPECT_TRUE(sp4.size() == 1);
  EXPECT_TRUE(sp4[0] == "Hello"_t);

  sp4 = "Hello"_t.splitByText("Hello"_t);
  EXPECT_TRUE(sp4.size() == 0);

  sp4 = "Hello"_t.splitByText(""_t);
  EXPECT_TRUE(sp4.size() == 1);
  EXPECT_TRUE(sp4[0] == "Hello"_t);

  sp4 = "ttttt"_t.splitByText("tt"_t);
  EXPECT_TRUE(sp4.size() == 1);
  EXPECT_TRUE(sp4[0] == "t"_t);
}

TEST(kltext, test_split_next_char) {
  auto sp1 = "This is some bad text  "_t;
  auto [first, next] = sp1.splitNextChar(' ', SplitDirection::KeepLeft);
  EXPECT_TRUE(first == "This ");
  EXPECT_TRUE(next == "is some bad text  ");
  std::tie(first, next) = next.splitNextChar(' ', SplitDirection::KeepLeft);
  EXPECT_TRUE(first == "is ");
  EXPECT_TRUE(next == "some bad text  ");
  std::tie(first, next) = next.splitNextChar(' ', SplitDirection::KeepLeft);
  EXPECT_TRUE(first == "some ");
  EXPECT_TRUE(next == "bad text  ");
  std::tie(first, next) = next.splitNextChar(' ', SplitDirection::KeepLeft);
  EXPECT_TRUE(first == "bad ");
  EXPECT_TRUE(next == "text  ");
  std::tie(first, next) = next.splitNextChar(' ', SplitDirection::KeepLeft);
  EXPECT_TRUE(first == "text ");
  EXPECT_TRUE(next == " ");
  std::tie(first, next) = next.splitNextChar(' ', SplitDirection::KeepLeft);
  EXPECT_TRUE(first == " ");
  EXPECT_TRUE(next.size() == 0);

  std::tie(first, next) = sp1.splitNextChar(' ', SplitDirection::KeepRight);
  EXPECT_TRUE(first == "This");
  EXPECT_TRUE(next == " is some bad text  ");
  std::tie(first, next) = next.splitNextChar(' ', SplitDirection::KeepRight);
  EXPECT_TRUE(first == "");
  EXPECT_TRUE(next == " is some bad text  ");
  std::tie(first, next) = next.skip(1).splitNextChar(' ', SplitDirection::KeepRight);
  EXPECT_TRUE(first == "is");
  EXPECT_TRUE(next == " some bad text  ");
  std::tie(first, next) = next.skip(1).splitNextChar(' ', SplitDirection::KeepRight);
  EXPECT_TRUE(first == "some");
  EXPECT_TRUE(next == " bad text  ");
  std::tie(first, next) = next.skip(1).splitNextChar(' ', SplitDirection::KeepRight);
  EXPECT_TRUE(first == "bad");
  EXPECT_TRUE(next == " text  ");
  std::tie(first, next) = next.skip(1).splitNextChar(' ', SplitDirection::KeepRight);
  EXPECT_TRUE(first == "text");
  EXPECT_TRUE(next == "  ");
  std::tie(first, next) = next.skip(1).splitNextChar(' ', SplitDirection::KeepRight);
  EXPECT_TRUE(first == "");
  EXPECT_TRUE(next == " ");
  std::tie(first, next) = next.skip(1).splitNextChar(' ', SplitDirection::KeepRight);
  EXPECT_TRUE(first == "");
  EXPECT_TRUE(next == "");

  std::tie(first, next) = sp1.splitNextChar(' ', SplitDirection::Discard);
  EXPECT_TRUE(first == "This");
  EXPECT_TRUE(next == "is some bad text  ");
  std::tie(first, next) = next.splitNextChar(' ', SplitDirection::Discard);
  EXPECT_TRUE(first == "is");
  EXPECT_TRUE(next == "some bad text  ");
  std::tie(first, next) = next.splitNextChar(' ', SplitDirection::Discard);
  EXPECT_TRUE(first == "some");
  EXPECT_TRUE(next == "bad text  ");
  std::tie(first, next) = next.splitNextChar(' ', SplitDirection::Discard);
  EXPECT_TRUE(first == "bad");
  EXPECT_TRUE(next == "text  ");
  std::tie(first, next) = next.splitNextChar(' ', SplitDirection::Discard);
  EXPECT_TRUE(first == "text");
  EXPECT_TRUE(next == " ");
  std::tie(first, next) = next.splitNextChar(' ', SplitDirection::Discard);
  EXPECT_TRUE(first == "");
  EXPECT_TRUE(next == "");

  std::tie(first, next) = ""_t.splitNextChar(' ', SplitDirection::KeepLeft);
  EXPECT_TRUE(first == "");
  EXPECT_TRUE(next == "");

  std::tie(first, next) = ""_t.splitNextChar(' ', SplitDirection::KeepRight);
  EXPECT_TRUE(first == "");
  EXPECT_TRUE(next == "");

  std::tie(first, next) = ""_t.splitNextChar(' ', SplitDirection::Discard);
  EXPECT_TRUE(first == "");
  EXPECT_TRUE(next == "");
}

TEST(kltext, test_split_next_line) {
  auto sp1 = "This\nis\n\rsome\r\nbad\ntext\n\n"_t;
  auto [first, next] = sp1.splitNextLine();
  EXPECT_TRUE(first == "This");
  EXPECT_TRUE(next == "is\n\rsome\r\nbad\ntext\n\n");
  std::tie(first, next) = next.splitNextLine();
  EXPECT_TRUE(first == "is");
  EXPECT_TRUE(next == "some\r\nbad\ntext\n\n");
  std::tie(first, next) = next.splitNextLine();
  EXPECT_TRUE(first == "some");
  EXPECT_TRUE(next == "bad\ntext\n\n");
  std::tie(first, next) = next.splitNextLine();
  EXPECT_TRUE(first == "bad");
  EXPECT_TRUE(next == "text\n\n");
  std::tie(first, next) = next.splitNextLine();
  EXPECT_TRUE(first == "text");
  EXPECT_TRUE(next == "\n");
  std::tie(first, next) = next.splitNextLine();
  EXPECT_TRUE(first.size() == 0);
  EXPECT_TRUE(next.size() == 0);

  auto sp2 = "A bad example"_t;
  std::tie(first, next) = sp2.splitNextLine();
  EXPECT_TRUE(first == sp2);
  EXPECT_TRUE(next.size() == 0);

  auto sp3 = "A simple\nexample"_t;
  std::tie(first, next) = sp3.splitNextLine();
  EXPECT_TRUE(first == "A simple");
  EXPECT_TRUE(next == "example");

  std::tie(first, next) = ""_t.splitNextLine();
  EXPECT_TRUE(first == "");
  EXPECT_TRUE(next == "");
}

TEST(kltext, test_split_lines) {
  auto sp1 = "This\nis\nsome\ntext\n"_t.splitLines();
  EXPECT_TRUE(sp1.size() == 5);
  EXPECT_TRUE(sp1[0] == "This"_t);
  EXPECT_TRUE(sp1[1] == "is"_t);
  EXPECT_TRUE(sp1[2] == "some"_t);
  EXPECT_TRUE(sp1[3] == "text"_t);
  EXPECT_TRUE(sp1[4] == ""_t);

  sp1 = "This\nis\nsome\ntext\n"_t.splitLines(SplitEmpty::Discard);
  EXPECT_TRUE(sp1.size() == 4);
  EXPECT_TRUE(sp1[0] == "This"_t);
  EXPECT_TRUE(sp1[1] == "is"_t);
  EXPECT_TRUE(sp1[2] == "some"_t);
  EXPECT_TRUE(sp1[3] == "text"_t);

  auto sp2 = "\n\nThis\n\nis\n\n\nsome\ntext"_t.splitLines();
  EXPECT_TRUE(sp2.size() == 9);
  EXPECT_TRUE(sp2[0] == ""_t);
  EXPECT_TRUE(sp2[1] == ""_t);
  EXPECT_TRUE(sp2[2] == "This"_t);
  EXPECT_TRUE(sp2[3] == ""_t);
  EXPECT_TRUE(sp2[4] == "is"_t);
  EXPECT_TRUE(sp2[5] == ""_t);
  EXPECT_TRUE(sp2[6] == ""_t);
  EXPECT_TRUE(sp2[7] == "some"_t);
  EXPECT_TRUE(sp2[8] == "text"_t);

  sp2 = "\n\r\n\rThis\n\r\n\ris\n\r\n\r\nsome\ntext"_t.splitLines();
  EXPECT_TRUE(sp2.size() == 9);
  EXPECT_TRUE(sp2[0] == ""_t);
  EXPECT_TRUE(sp2[1] == ""_t);
  EXPECT_TRUE(sp2[2] == "This"_t);
  EXPECT_TRUE(sp2[3] == ""_t);
  EXPECT_TRUE(sp2[4] == "is"_t);
  EXPECT_TRUE(sp2[5] == ""_t);
  EXPECT_TRUE(sp2[6] == ""_t);
  EXPECT_TRUE(sp2[7] == "some"_t);
  EXPECT_TRUE(sp2[8] == "text"_t);

  sp2 = "\r\n\r\nThis\r\n\r\nis\r\n\r\n\r\nsome\r\ntext"_t.splitLines();
  EXPECT_TRUE(sp2.size() == 9);
  EXPECT_TRUE(sp2[0] == ""_t);
  EXPECT_TRUE(sp2[1] == ""_t);
  EXPECT_TRUE(sp2[2] == "This"_t);
  EXPECT_TRUE(sp2[3] == ""_t);
  EXPECT_TRUE(sp2[4] == "is"_t);
  EXPECT_TRUE(sp2[5] == ""_t);
  EXPECT_TRUE(sp2[6] == ""_t);
  EXPECT_TRUE(sp2[7] == "some"_t);
  EXPECT_TRUE(sp2[8] == "text"_t);

  sp2 = "Hello\r\n"_t.splitLines();
  EXPECT_TRUE(sp2.size() == 2);
  EXPECT_TRUE(sp2[0] == "Hello"_t);
  EXPECT_TRUE(sp2[1] == ""_t);

  sp2 = "Hello\r\n"_t.splitLines(SplitEmpty::Discard);
  EXPECT_TRUE(sp2.size() == 1);
  EXPECT_TRUE(sp2[0] == "Hello"_t);

  sp2 = "\r\n\r\nThis\r\n\r\nis\r\n\r\n\r\nsome\r\ntext"_t.splitLines(SplitEmpty::Discard);
  EXPECT_TRUE(sp2.size() == 4);
  EXPECT_TRUE(sp2[0] == "This"_t);
  EXPECT_TRUE(sp2[1] == "is"_t);
  EXPECT_TRUE(sp2[2] == "some"_t);
  EXPECT_TRUE(sp2[3] == "text"_t);

  sp2 = "\n\r\n\rThis\n\r\n\ris\n\r\n\r\nsome\ntext"_t.splitLines(SplitEmpty::Discard);
  EXPECT_TRUE(sp2.size() == 4);
  EXPECT_TRUE(sp2[0] == "This"_t);
  EXPECT_TRUE(sp2[1] == "is"_t);
  EXPECT_TRUE(sp2[2] == "some"_t);
  EXPECT_TRUE(sp2[3] == "text"_t);
}

TEST(kltext, test_last_pos) {
  auto filename = "/123456/89.1234"_t;
  EXPECT_TRUE(filename.lastPos('.') == 10);
  EXPECT_TRUE(filename.lastPos('/') == 7);
  EXPECT_TRUE(filename.lastPos('4') == 14);
  EXPECT_TRUE(filename.lastPos('9') == 9);
  EXPECT_TRUE(!filename.lastPos('+').has_value());
}

TEST(kltext, test_sublen_subpos) {
  Text t("0123456789");
  EXPECT_TRUE(t.sublen(0, 2) == "01"_t);
  EXPECT_TRUE(t.sublen(0, 4) == "0123"_t);
  EXPECT_TRUE(t.sublen(2, 4) == "2345"_t);
  EXPECT_TRUE(t.sublen(2, 0) == ""_t);
  EXPECT_TRUE(t.sublen(8, 4) == "89"_t);

  EXPECT_TRUE(t.subpos(0, 2) == "012"_t);
  EXPECT_TRUE(t.subpos(0, 4) == "01234"_t);
  EXPECT_TRUE(t.subpos(2, 4) == "234"_t);
  EXPECT_TRUE(t.subpos(2, 0) == ""_t);
  EXPECT_TRUE(t.subpos(2, 2) == "2"_t);
  EXPECT_TRUE(t.sublen(8, 9) == "89"_t);
  EXPECT_TRUE(t.sublen(8, 10) == "89"_t);
  EXPECT_TRUE(t.sublen(8, 20) == "89"_t);
}

TEST(kltext, test_count) {
  Text t("hello world");
  EXPECT_TRUE(t.count('h') == 1);
  EXPECT_TRUE(t.count('x') == 0);
  EXPECT_TRUE(t.count('e') == 1);
  EXPECT_TRUE(t.count('l') == 3);
  EXPECT_TRUE(t.count('o') == 2);
  EXPECT_TRUE(t.count(' ') == 1);
}

TEST(kltext, test_fill_c_buff) {
  auto t = "123"_t;
  char buffer[100] = "Helloworld";
  t.fill_c_buffer(buffer, 100);
  EXPECT_TRUE("123"s == buffer);
  t.fill_c_buffer(buffer, 3);
  EXPECT_TRUE("12"s == buffer);
  t.fill_c_buffer(buffer, 2);
  EXPECT_TRUE("1"s == buffer);
  t.fill_c_buffer(buffer, 1);
  EXPECT_TRUE(""s == buffer);
  t.fill_c_buffer(buffer, 100);
  EXPECT_TRUE("123"s == buffer);
  t.fill_c_buffer(buffer, 0);
  EXPECT_TRUE("123"s == buffer);
}

TEST(kltext, test_indent) {
  EXPECT_TRUE(""_t.getIndent() == 0);
  EXPECT_TRUE("  "_t.getIndent() == 2);
  EXPECT_TRUE("  xxx"_t.getIndent() == 2);
  EXPECT_TRUE(" x "_t.getIndent() == 1);

  EXPECT_TRUE(!""_t.skipIndent(2).has_value());
  EXPECT_TRUE(""_t.skipIndent(0).has_value());
  EXPECT_TRUE(""_t.skipIndent(0) == ""_t);
  EXPECT_TRUE("  "_t.skipIndent(1).has_value());
  EXPECT_TRUE("  "_t.skipIndent(1) == " "_t);
  EXPECT_TRUE("  "_t.skipIndent(2).has_value());
  EXPECT_TRUE("  "_t.skipIndent(2) == ""_t);
  EXPECT_TRUE(!"  "_t.skipIndent(3).has_value());
  EXPECT_TRUE(!"  "_t.skipIndent(100).has_value());
  EXPECT_TRUE(!"  xxx"_t.skipIndent(3).has_value());
  EXPECT_TRUE("  xxx"_t.skipIndent(2) == "xxx"_t);
  EXPECT_TRUE("  xxx"_t.skipIndent(1) == " xxx"_t);
  EXPECT_TRUE("  xxx"_t.skipIndent(0) == "  xxx"_t);
  EXPECT_TRUE(!" x "_t.skipIndent(3).has_value());
  EXPECT_TRUE(!" x "_t.skipIndent(2).has_value());
  EXPECT_TRUE(" x "_t.skipIndent(1) == "x "_t);
  EXPECT_TRUE(" x "_t.skipIndent(0) == " x "_t);
}
