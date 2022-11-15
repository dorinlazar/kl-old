#include "kl/kltext.h"
#include <gtest/gtest.h>
using namespace kl;
using namespace std::literals;
using namespace kl::literals;

TEST(kltext, test_text_construction) {
  Text txt("Hello world");
  auto t2 = Text(txt, 4, 20);
  EXPECT_EQ(t2.toString(), "o world"s);
  EXPECT_EQ(t2.size(), 7);

  Text tnull(nullptr, 2, 4);
  EXPECT_EQ(tnull.size(), 0);
  EXPECT_EQ(tnull.toString(), ""s);
  auto tn2 = Text(tnull, 2, 4);
  EXPECT_EQ(tn2.size(), 0);
  EXPECT_EQ(tn2.toString(), ""s);

  Text tempty(""s);
  EXPECT_EQ(tempty.size(), 0);
  EXPECT_EQ(tempty.toString(), ""s);

  Text t1("Hello World", 2);
  EXPECT_EQ(t1.size(), 2);
  EXPECT_EQ(t1.toString(), "He"s);

  Text t3("Hello World\0", 12);
  EXPECT_EQ(t3.size(), 12);
  EXPECT_EQ(t3[11], '\0');

  Text t4("Hello World\0Other World", 23);
  EXPECT_EQ(t4.size(), 23);
  EXPECT_EQ(t4[11], '\0');
  EXPECT_EQ(t4.subpos(12, 24), "Other World");
}

TEST(kltext, test_trimming) {
  Text txt(" \t hello   \n ");
  EXPECT_EQ(txt.trim().toString(), "hello"s);
  EXPECT_EQ(txt.trim_left().toString(), "hello   \n "s);
  EXPECT_EQ(txt.trim_right().toString(), " \t hello"s);

  Text txt2("hello");
  EXPECT_EQ(txt2.trim().toString(), "hello"s);
  EXPECT_EQ(txt2.trim_left().toString(), "hello"s);
  EXPECT_EQ(txt2.trim_right().toString(), "hello"s);

  Text txt3;
  EXPECT_EQ(txt3.trim().toString(), ""s);
  EXPECT_EQ(txt3.trim_left().toString(), ""s);
  EXPECT_EQ(txt3.trim_right().toString(), ""s);
}

TEST(kltext, test_starts_with) {
  Text txt("Hello");
  EXPECT_TRUE(txt.starts_with(""));
  EXPECT_TRUE(txt.starts_with("H"));
  EXPECT_TRUE(txt.starts_with("Hell"));
  EXPECT_TRUE(txt.starts_with("Hello"));
  EXPECT_FALSE(txt.starts_with("Hello world"));
  EXPECT_FALSE(txt.starts_with("world"));

  EXPECT_TRUE(txt.starts_with(""_t));
  EXPECT_TRUE(txt.starts_with("H"_t));
  EXPECT_TRUE(txt.starts_with("Hell"_t));
  EXPECT_TRUE(txt.starts_with("Hello"_t));
  EXPECT_FALSE(txt.starts_with("Hello world"_t));
  EXPECT_FALSE(txt.starts_with("world"_t));
}

TEST(kltext, test_ends_with) {
  Text txt("Hello");
  EXPECT_TRUE(txt.endsWith(""));
  EXPECT_TRUE(txt.endsWith("o"));
  EXPECT_TRUE(txt.endsWith("ello"));
  EXPECT_TRUE(txt.endsWith("Hello"));
  EXPECT_FALSE(txt.endsWith("Hello world"));
  EXPECT_FALSE(txt.endsWith("world"));
}

TEST(kltext, test_comparisons) {
  Text hello("hello");
  Text hfllo("hfllo");
  Text empty;

  EXPECT_EQ(hello, "hello");
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

  EXPECT_EQ(hello, "hello"s);
  EXPECT_TRUE(hello < "hfllo"s);
  EXPECT_TRUE(hello <= "hfllo"s);
  EXPECT_TRUE(hello != "hfllo"s);
  EXPECT_TRUE(hello >= "hallo"s);
  EXPECT_TRUE(hello >= "ahllo"s);
  EXPECT_TRUE(hello > "hallo"s);
  EXPECT_TRUE(hello > "hell"s);
  EXPECT_TRUE(hello < "hello world"s);
  EXPECT_TRUE(hello > ""s);

  EXPECT_EQ(hello, "hello"_t);
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
  EXPECT_EQ(get<0>(txt.splitPos(0)), "");
  EXPECT_EQ(get<1>(txt.splitPos(0)), "hello");
  EXPECT_EQ(get<0>(txt.splitPos(1)), "h");
  EXPECT_EQ(get<1>(txt.splitPos(1)), "ello");
  EXPECT_EQ(get<0>(txt.splitPos(2)), "he");
  EXPECT_EQ(get<1>(txt.splitPos(2)), "llo");
  EXPECT_EQ(get<0>(txt.splitPos(-2)), "hel");
  EXPECT_EQ(get<1>(txt.splitPos(-2)), "lo");
  EXPECT_EQ(get<0>(txt.splitPos(-1)), "hell");
  EXPECT_EQ(get<1>(txt.splitPos(-1)), "o");

  EXPECT_EQ(get<0>(txt.splitPos(-5)), "");
  EXPECT_EQ(get<1>(txt.splitPos(-5)), "hello");
  EXPECT_EQ(get<0>(txt.splitPos(-10)), "");
  EXPECT_EQ(get<1>(txt.splitPos(-10)), "hello");

  EXPECT_EQ(get<0>(txt.splitPos(5)), "hello");
  EXPECT_EQ(get<1>(txt.splitPos(5)), "");
  EXPECT_EQ(get<0>(txt.splitPos(15)), "hello");
  EXPECT_EQ(get<1>(txt.splitPos(15)), "");
  EXPECT_EQ(get<0>(""_t.splitPos(15)), "");
  EXPECT_EQ(get<1>(""_t.splitPos(15)), "");
}

TEST(kltext, test_contains) {
  Text t("hello");
  EXPECT_TRUE(t.contains('h'));
  EXPECT_TRUE(t.contains('e'));
  EXPECT_TRUE(t.contains('o'));
  EXPECT_TRUE(t.contains('l'));
  EXPECT_FALSE(t.contains('a'));
  EXPECT_FALSE(t.contains('\0'));
  EXPECT_FALSE(""_t.contains('\0'));
}

TEST(kltext, test_skip) {
  Text t("hello");
  EXPECT_EQ(t.skip("hel"), "o");
  EXPECT_EQ(t.skip("el"), "hello");
  EXPECT_EQ(t.skip(""), "hello");
  EXPECT_EQ(t.skip("abcdefghijklmnopqrstuvwxyz"), "");
  EXPECT_EQ(""_t.skip("abcdefghijklmnopqrstuvwxyz"), "");

  EXPECT_EQ(t.skip(2), "llo"_t);
  EXPECT_EQ(t.skip(1), "ello"_t);
  EXPECT_EQ(t.skip(4), "o"_t);
  EXPECT_EQ(t.skip(5), ""_t);
  EXPECT_EQ(t.skip(10), ""_t);
  EXPECT_EQ(t.skip(100), ""_t);
}

TEST(kltext, test_pos) {
  Text t("hello world");
  EXPECT_EQ(t.pos('l'), 2);
  EXPECT_EQ(t.pos('l', 2), 3);
  EXPECT_EQ(t.pos('l', 3), 9);
  EXPECT_EQ(t.pos('d'), 10);
  EXPECT_EQ(t.pos(' '), 5);
  EXPECT_FALSE(t.pos('o', 3).has_value());
  EXPECT_FALSE(t.pos('k').has_value());

  EXPECT_FALSE(""_t.pos('x').has_value());

  EXPECT_EQ(t.pos("l"_t), 2);
  EXPECT_EQ(t.pos("ll"_t), 2);
  EXPECT_EQ(t.pos("l"_t, 2), 3);
  EXPECT_EQ(t.pos("l"_t, 3), 9);
  EXPECT_EQ(t.pos("o w"_t), 4);
  EXPECT_EQ(t.pos("d"_t), 10);
  EXPECT_FALSE(t.pos("o W"_t).has_value());
  EXPECT_FALSE(t.pos(""_t).has_value());
  EXPECT_FALSE(t.pos("hello world!"_t).has_value());
  EXPECT_EQ(t.pos("hello w"_t), 0);
  EXPECT_EQ(t.pos("hello world"_t), 0);

  EXPECT_FALSE(""_t.pos("x"_t).has_value());
  EXPECT_FALSE(""_t.pos(""_t).has_value());
}

TEST(kltext, test_split_char) {
  Text t1("My,CSV,text,with,,an,ending,,");
  auto sp1 = t1.splitByChar(',');

  EXPECT_EQ(sp1.size(), 6);
  EXPECT_EQ(sp1[0], "My"_t);
  EXPECT_EQ(sp1[1], "CSV"_t);
  EXPECT_EQ(sp1[2], "text"_t);
  EXPECT_EQ(sp1[3], "with"_t);
  EXPECT_EQ(sp1[4], "an"_t);
  EXPECT_EQ(sp1[5], "ending"_t);

  kl::log("Splitting {}", t1);
  auto sp2 = t1.splitByChar(',', SplitEmpty::Keep);
  EXPECT_EQ(sp2.size(), 9);
  EXPECT_EQ(sp2[0], "My"_t);
  EXPECT_EQ(sp2[1], "CSV"_t);
  EXPECT_EQ(sp2[2], "text"_t);
  EXPECT_EQ(sp2[3], "with"_t);
  EXPECT_EQ(sp2[4], ""_t);
  EXPECT_EQ(sp2[5], "an"_t);
  EXPECT_EQ(sp2[6], "ending"_t);
  EXPECT_EQ(sp2[7], ""_t);
  EXPECT_EQ(sp2[8], ""_t);

  auto sp3 = "Hello World"_t.splitByChar(' ');
  EXPECT_EQ(sp3.size(), 2);
  EXPECT_EQ(sp3[0], "Hello"_t);
  EXPECT_EQ(sp3[1], "World"_t);

  sp3 = "   Hello World"_t.splitByChar(' ');
  EXPECT_EQ(sp3.size(), 2);
  EXPECT_EQ(sp3[0], "Hello"_t);
  EXPECT_EQ(sp3[1], "World"_t);

  sp3 = "   Hello World"_t.splitByChar(' ', SplitEmpty::Keep);
  EXPECT_EQ(sp3.size(), 5);
  EXPECT_EQ(sp3[0], ""_t);
  EXPECT_EQ(sp3[1], ""_t);
  EXPECT_EQ(sp3[2], ""_t);
  EXPECT_EQ(sp3[3], "Hello"_t);
  EXPECT_EQ(sp3[4], "World"_t);

  auto sp4 = "Hello_World"_t.splitByChar(' ');
  EXPECT_EQ(sp4.size(), 1);
  EXPECT_EQ(sp4[0], "Hello_World"_t);
}

TEST(kltext, test_split_text) {
  Text t1("This||is||||||some||text||||");
  auto sp1 = t1.splitByText("||"_t);

  EXPECT_EQ(sp1.size(), 4);
  EXPECT_EQ(sp1[0], "This"_t);
  EXPECT_EQ(sp1[1], "is"_t);
  EXPECT_EQ(sp1[2], "some"_t);
  EXPECT_EQ(sp1[3], "text"_t);

  auto sp2 = t1.splitByText("||"_t, SplitEmpty::Keep);
  EXPECT_EQ(sp2.size(), 8);
  EXPECT_EQ(sp2[0], "This"_t);
  EXPECT_EQ(sp2[1], "is"_t);
  EXPECT_EQ(sp2[2], ""_t);
  EXPECT_EQ(sp2[3], ""_t);
  EXPECT_EQ(sp2[4], "some"_t);
  EXPECT_EQ(sp2[5], "text"_t);
  EXPECT_EQ(sp2[6], ""_t);
  EXPECT_EQ(sp2[7], ""_t);

  auto sp3 = "Hello World"_t.splitByText(" "_t);
  EXPECT_EQ(sp3.size(), 2);
  EXPECT_EQ(sp3[0], "Hello"_t);
  EXPECT_EQ(sp3[1], "World"_t);

  sp3 = "   Hello World"_t.splitByText(" "_t);
  EXPECT_EQ(sp3.size(), 2);
  EXPECT_EQ(sp3[0], "Hello"_t);
  EXPECT_EQ(sp3[1], "World"_t);

  sp3 = "   Hello World "_t.splitByText(" "_t, SplitEmpty::Keep);
  EXPECT_EQ(sp3.size(), 6);
  EXPECT_EQ(sp3[0], ""_t);
  EXPECT_EQ(sp3[1], ""_t);
  EXPECT_EQ(sp3[2], ""_t);
  EXPECT_EQ(sp3[3], "Hello"_t);
  EXPECT_EQ(sp3[4], "World"_t);
  EXPECT_EQ(sp3[5], ""_t);

  auto sp4 = "Hello_World"_t.splitByText(" "_t);
  EXPECT_EQ(sp4.size(), 1);
  EXPECT_EQ(sp4[0], "Hello_World"_t);

  sp4 = "Hello"_t.splitByText("HelloWorld"_t);
  EXPECT_EQ(sp4.size(), 1);
  EXPECT_EQ(sp4[0], "Hello"_t);

  sp4 = "Hello"_t.splitByText("Hello"_t);
  EXPECT_EQ(sp4.size(), 0);

  sp4 = "Hello"_t.splitByText(""_t);
  EXPECT_EQ(sp4.size(), 1);
  EXPECT_EQ(sp4[0], "Hello"_t);

  sp4 = "ttttt"_t.splitByText("tt"_t);
  EXPECT_EQ(sp4.size(), 1);
  EXPECT_EQ(sp4[0], "t"_t);
}

TEST(kltext, test_split_next_char) {
  auto sp1 = "This is some bad text  "_t;
  auto [first, next] = sp1.splitNextChar(' ', SplitDirection::KeepLeft);
  EXPECT_EQ(first, "This ");
  EXPECT_EQ(next, "is some bad text  ");
  std::tie(first, next) = next.splitNextChar(' ', SplitDirection::KeepLeft);
  EXPECT_EQ(first, "is ");
  EXPECT_EQ(next, "some bad text  ");
  std::tie(first, next) = next.splitNextChar(' ', SplitDirection::KeepLeft);
  EXPECT_EQ(first, "some ");
  EXPECT_EQ(next, "bad text  ");
  std::tie(first, next) = next.splitNextChar(' ', SplitDirection::KeepLeft);
  EXPECT_EQ(first, "bad ");
  EXPECT_EQ(next, "text  ");
  std::tie(first, next) = next.splitNextChar(' ', SplitDirection::KeepLeft);
  EXPECT_EQ(first, "text ");
  EXPECT_EQ(next, " ");
  std::tie(first, next) = next.splitNextChar(' ', SplitDirection::KeepLeft);
  EXPECT_EQ(first, " ");
  EXPECT_EQ(next.size(), 0);

  std::tie(first, next) = sp1.splitNextChar(' ', SplitDirection::KeepRight);
  EXPECT_EQ(first, "This");
  EXPECT_EQ(next, " is some bad text  ");
  std::tie(first, next) = next.splitNextChar(' ', SplitDirection::KeepRight);
  EXPECT_EQ(first, "");
  EXPECT_EQ(next, " is some bad text  ");
  std::tie(first, next) = next.skip(1).splitNextChar(' ', SplitDirection::KeepRight);
  EXPECT_EQ(first, "is");
  EXPECT_EQ(next, " some bad text  ");
  std::tie(first, next) = next.skip(1).splitNextChar(' ', SplitDirection::KeepRight);
  EXPECT_EQ(first, "some");
  EXPECT_EQ(next, " bad text  ");
  std::tie(first, next) = next.skip(1).splitNextChar(' ', SplitDirection::KeepRight);
  EXPECT_EQ(first, "bad");
  EXPECT_EQ(next, " text  ");
  std::tie(first, next) = next.skip(1).splitNextChar(' ', SplitDirection::KeepRight);
  EXPECT_EQ(first, "text");
  EXPECT_EQ(next, "  ");
  std::tie(first, next) = next.skip(1).splitNextChar(' ', SplitDirection::KeepRight);
  EXPECT_EQ(first, "");
  EXPECT_EQ(next, " ");
  std::tie(first, next) = next.skip(1).splitNextChar(' ', SplitDirection::KeepRight);
  EXPECT_EQ(first, "");
  EXPECT_EQ(next, "");

  std::tie(first, next) = sp1.splitNextChar(' ', SplitDirection::Discard);
  EXPECT_EQ(first, "This");
  EXPECT_EQ(next, "is some bad text  ");
  std::tie(first, next) = next.splitNextChar(' ', SplitDirection::Discard);
  EXPECT_EQ(first, "is");
  EXPECT_EQ(next, "some bad text  ");
  std::tie(first, next) = next.splitNextChar(' ', SplitDirection::Discard);
  EXPECT_EQ(first, "some");
  EXPECT_EQ(next, "bad text  ");
  std::tie(first, next) = next.splitNextChar(' ', SplitDirection::Discard);
  EXPECT_EQ(first, "bad");
  EXPECT_EQ(next, "text  ");
  std::tie(first, next) = next.splitNextChar(' ', SplitDirection::Discard);
  EXPECT_EQ(first, "text");
  EXPECT_EQ(next, " ");
  std::tie(first, next) = next.splitNextChar(' ', SplitDirection::Discard);
  EXPECT_EQ(first, "");
  EXPECT_EQ(next, "");

  std::tie(first, next) = ""_t.splitNextChar(' ', SplitDirection::KeepLeft);
  EXPECT_EQ(first, "");
  EXPECT_EQ(next, "");

  std::tie(first, next) = ""_t.splitNextChar(' ', SplitDirection::KeepRight);
  EXPECT_EQ(first, "");
  EXPECT_EQ(next, "");

  std::tie(first, next) = ""_t.splitNextChar(' ', SplitDirection::Discard);
  EXPECT_EQ(first, "");
  EXPECT_EQ(next, "");
}

TEST(kltext, test_split_next_line) {
  auto sp1 = "This\nis\r\nsome\r\nbad\ntext\n\n"_t;
  auto [first, next] = sp1.splitNextLine();
  EXPECT_EQ(first, "This");
  EXPECT_EQ(next, "is\r\nsome\r\nbad\ntext\n\n");
  std::tie(first, next) = next.splitNextLine();
  EXPECT_EQ(first, "is");
  EXPECT_EQ(next, "some\r\nbad\ntext\n\n");
  std::tie(first, next) = next.splitNextLine();
  EXPECT_EQ(first, "some");
  EXPECT_EQ(next, "bad\ntext\n\n");
  std::tie(first, next) = next.splitNextLine();
  EXPECT_EQ(first, "bad");
  EXPECT_EQ(next, "text\n\n");
  std::tie(first, next) = next.splitNextLine();
  EXPECT_EQ(first, "text");
  EXPECT_EQ(next, "\n");
  std::tie(first, next) = next.splitNextLine();
  EXPECT_EQ(first.size(), 0);
  EXPECT_EQ(next.size(), 0);

  auto sp2 = "A bad example"_t;
  std::tie(first, next) = sp2.splitNextLine();
  EXPECT_EQ(first, sp2);
  EXPECT_EQ(next.size(), 0);

  auto sp3 = "A simple\nexample"_t;
  std::tie(first, next) = sp3.splitNextLine();
  EXPECT_EQ(first, "A simple");
  EXPECT_EQ(next, "example");

  std::tie(first, next) = ""_t.splitNextLine();
  EXPECT_EQ(first, "");
  EXPECT_EQ(next, "");
}

TEST(kltext, test_split_lines) {
  auto sp1 = "This\nis\nsome\ntext\n"_t.splitLines();
  EXPECT_EQ(sp1.size(), 5);
  EXPECT_EQ(sp1[0], "This"_t);
  EXPECT_EQ(sp1[1], "is"_t);
  EXPECT_EQ(sp1[2], "some"_t);
  EXPECT_EQ(sp1[3], "text"_t);
  EXPECT_EQ(sp1[4], ""_t);

  sp1 = "This\nis\nsome\ntext\n"_t.splitLines(SplitEmpty::Discard);
  EXPECT_EQ(sp1.size(), 4);
  EXPECT_EQ(sp1[0], "This"_t);
  EXPECT_EQ(sp1[1], "is"_t);
  EXPECT_EQ(sp1[2], "some"_t);
  EXPECT_EQ(sp1[3], "text"_t);

  auto sp2 = "\n\nThis\n\nis\n\n\nsome\ntext"_t.splitLines();
  EXPECT_EQ(sp2.size(), 9);
  EXPECT_EQ(sp2[0], ""_t);
  EXPECT_EQ(sp2[1], ""_t);
  EXPECT_EQ(sp2[2], "This"_t);
  EXPECT_EQ(sp2[3], ""_t);
  EXPECT_EQ(sp2[4], "is"_t);
  EXPECT_EQ(sp2[5], ""_t);
  EXPECT_EQ(sp2[6], ""_t);
  EXPECT_EQ(sp2[7], "some"_t);
  EXPECT_EQ(sp2[8], "text"_t);

  sp2 = "\r\n\r\nThis\r\n\r\nis\r\n\r\n\nsome\ntext"_t.splitLines();
  EXPECT_EQ(sp2.size(), 9);
  EXPECT_EQ(sp2[0], ""_t);
  EXPECT_EQ(sp2[1], ""_t);
  EXPECT_EQ(sp2[2], "This"_t);
  EXPECT_EQ(sp2[3], ""_t);
  EXPECT_EQ(sp2[4], "is"_t);
  EXPECT_EQ(sp2[5], ""_t);
  EXPECT_EQ(sp2[6], ""_t);
  EXPECT_EQ(sp2[7], "some"_t);
  EXPECT_EQ(sp2[8], "text"_t);

  sp2 = "\r\n\r\nThis\r\n\r\nis\r\n\r\n\r\nsome\r\ntext"_t.splitLines();
  EXPECT_EQ(sp2.size(), 9);
  EXPECT_EQ(sp2[0], ""_t);
  EXPECT_EQ(sp2[1], ""_t);
  EXPECT_EQ(sp2[2], "This"_t);
  EXPECT_EQ(sp2[3], ""_t);
  EXPECT_EQ(sp2[4], "is"_t);
  EXPECT_EQ(sp2[5], ""_t);
  EXPECT_EQ(sp2[6], ""_t);
  EXPECT_EQ(sp2[7], "some"_t);
  EXPECT_EQ(sp2[8], "text"_t);

  sp2 = "Hello\r\n"_t.splitLines();
  EXPECT_EQ(sp2.size(), 2);
  EXPECT_EQ(sp2[0], "Hello"_t);
  EXPECT_EQ(sp2[1], ""_t);

  sp2 = "Hello\r\n"_t.splitLines(SplitEmpty::Discard);
  EXPECT_EQ(sp2.size(), 1);
  EXPECT_EQ(sp2[0], "Hello"_t);

  sp2 = "\r\n\r\nThis\r\n\r\nis\r\n\r\n\r\nsome\r\ntext"_t.splitLines(SplitEmpty::Discard);
  EXPECT_EQ(sp2.size(), 4);
  EXPECT_EQ(sp2[0], "This"_t);
  EXPECT_EQ(sp2[1], "is"_t);
  EXPECT_EQ(sp2[2], "some"_t);
  EXPECT_EQ(sp2[3], "text"_t);
}

TEST(kltext, test_last_pos) {
  auto filename = "/123456/89.1234"_t;
  EXPECT_EQ(filename.lastPos('.'), 10);
  EXPECT_EQ(filename.lastPos('/'), 7);
  EXPECT_EQ(filename.lastPos('4'), 14);
  EXPECT_EQ(filename.lastPos('9'), 9);
  EXPECT_FALSE(filename.lastPos('+').has_value());
}

TEST(kltext, test_sublen_subpos) {
  Text t("0123456789");
  EXPECT_EQ(t.sublen(0, 2), "01"_t);
  EXPECT_EQ(t.sublen(0, 4), "0123"_t);
  EXPECT_EQ(t.sublen(2, 4), "2345"_t);
  EXPECT_EQ(t.sublen(2, 0), ""_t);
  EXPECT_EQ(t.sublen(8, 4), "89"_t);

  EXPECT_EQ(t.subpos(0, 2), "012"_t);
  EXPECT_EQ(t.subpos(0, 4), "01234"_t);
  EXPECT_EQ(t.subpos(2, 4), "234"_t);
  EXPECT_EQ(t.subpos(2, 0), ""_t);
  EXPECT_EQ(t.subpos(2, 2), "2"_t);
  EXPECT_EQ(t.sublen(8, 9), "89"_t);
  EXPECT_EQ(t.sublen(8, 10), "89"_t);
  EXPECT_EQ(t.sublen(8, 20), "89"_t);
}

TEST(kltext, test_count) {
  Text t("hello world");
  EXPECT_EQ(t.count('h'), 1);
  EXPECT_EQ(t.count('x'), 0);
  EXPECT_EQ(t.count('e'), 1);
  EXPECT_EQ(t.count('l'), 3);
  EXPECT_EQ(t.count('o'), 2);
  EXPECT_EQ(t.count(' '), 1);
}

TEST(kltext, test_fill_c_buff) {
  auto t = "123"_t;
  char buffer[100] = "Helloworld";
  t.fill_c_buffer(buffer, 100);
  EXPECT_EQ("123"s, buffer);
  t.fill_c_buffer(buffer, 3);
  EXPECT_EQ("12"s, buffer);
  t.fill_c_buffer(buffer, 2);
  EXPECT_EQ("1"s, buffer);
  t.fill_c_buffer(buffer, 1);
  EXPECT_EQ(""s, buffer);
  t.fill_c_buffer(buffer, 100);
  EXPECT_EQ("123"s, buffer);
  t.fill_c_buffer(buffer, 0);
  EXPECT_EQ("123"s, buffer);
}

TEST(kltext, test_indent) {
  EXPECT_EQ(""_t.getIndent(), 0);
  EXPECT_EQ("  "_t.getIndent(), 2);
  EXPECT_EQ("  xxx"_t.getIndent(), 2);
  EXPECT_EQ(" x "_t.getIndent(), 1);

  EXPECT_FALSE(""_t.skipIndent(2).has_value());
  EXPECT_TRUE(""_t.skipIndent(0).has_value());
  EXPECT_EQ(""_t.skipIndent(0), ""_t);
  EXPECT_TRUE("  "_t.skipIndent(1).has_value());
  EXPECT_EQ("  "_t.skipIndent(1), " "_t);
  EXPECT_TRUE("  "_t.skipIndent(2).has_value());
  EXPECT_EQ("  "_t.skipIndent(2), ""_t);
  EXPECT_FALSE("  "_t.skipIndent(3).has_value());
  EXPECT_FALSE("  "_t.skipIndent(100).has_value());
  EXPECT_FALSE("  xxx"_t.skipIndent(3).has_value());
  EXPECT_EQ("  xxx"_t.skipIndent(2), "xxx"_t);
  EXPECT_EQ("  xxx"_t.skipIndent(1), " xxx"_t);
  EXPECT_EQ("  xxx"_t.skipIndent(0), "  xxx"_t);
  EXPECT_FALSE(" x "_t.skipIndent(3).has_value());
  EXPECT_FALSE(" x "_t.skipIndent(2).has_value());
  EXPECT_EQ(" x "_t.skipIndent(1), "x "_t);
  EXPECT_EQ(" x "_t.skipIndent(0), " x "_t);
}
