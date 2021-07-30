#include "kl/kltext.h"
using namespace kl;
using namespace std::literals;

void test_text_construction() {
  Text txt("Hello world");
  auto t2 = Text(txt, 4, 20);
  CHECK(t2.toString() == "o world"s, "Text(txt, 4, 20)");
  CHECK(t2.size() == 7);

  Text tnull(nullptr, 2, 4);
  CHECK(tnull.size() == 0);
  CHECK(tnull.toString() == ""s, "null is empty");
  auto tn2 = Text(tnull, 2, 4);
  CHECK(tn2.size() == 0);
  CHECK(tn2.toString() == ""s, "null is empty");

  Text tempty(""s);
  CHECK(tempty.size() == 0);
  CHECK(tempty.toString() == ""s);

  Text t1("Hello World", 2);
  CHECK(t1.size() == 2);
  CHECK(t1.toString() == "He"s);

  Text t3("Hello World", 200);
  CHECK(t3.size() == 11);
  CHECK(t3.toString() == "Hello World"s);

  log("Text construction: [OK]");
}

void test_trimming() {
  Text txt(" \t hello   \n ");
  CHECK(txt.trim().toString() == "hello"s);
  CHECK(txt.trimLeft().toString() == "hello   \n "s);
  CHECK(txt.trimRight().toString() == " \t hello"s);

  Text txt2("hello");
  CHECK(txt2.trim().toString() == "hello"s);
  CHECK(txt2.trimLeft().toString() == "hello"s);
  CHECK(txt2.trimRight().toString() == "hello"s);

  Text txt3;
  CHECK(txt3.trim().toString() == ""s);
  CHECK(txt3.trimLeft().toString() == ""s);
  CHECK(txt3.trimRight().toString() == ""s);

  log("trimming tests [OK]");
}

void test_starts_with() {
  Text txt("Hello");
  CHECKST(txt.startsWith(""));
  CHECKST(txt.startsWith("H"));
  CHECKST(txt.startsWith("Hell"));
  CHECKST(txt.startsWith("Hello"));
  CHECKST(!txt.startsWith("Hello world"));
  CHECKST(!txt.startsWith("world"));

  CHECKST(txt.startsWith(""_t));
  CHECKST(txt.startsWith("H"_t));
  CHECKST(txt.startsWith("Hell"_t));
  CHECKST(txt.startsWith("Hello"_t));
  CHECKST(!txt.startsWith("Hello world"_t));
  CHECKST(!txt.startsWith("world"_t));

  log("starts_with test [OK]");
}

void test_ends_with() {
  Text txt("Hello");
  CHECKST(txt.endsWith(""));
  CHECKST(txt.endsWith("o"));
  CHECKST(txt.endsWith("ello"));
  CHECKST(txt.endsWith("Hello"));
  CHECKST(!txt.endsWith("Hello world"));
  CHECKST(!txt.endsWith("world"));

  log("ends_with test [OK]");
}

void test_comparisons() {
  Text hello("hello");
  Text hfllo("hfllo");
  Text empty;

  CHECKST(hello == "hello");
  CHECKST(hello < "hfllo");
  CHECKST(hello <= "hfllo");
  CHECKST(hello != "hfllo");
  CHECKST(hello >= "hallo");
  CHECKST(hello >= "ahllo");
  CHECKST(hello > "hallo");
  CHECKST(hello > "hell");
  CHECKST(hello < "hello world");
  CHECKST(hello > "");
  CHECKST(hello > nullptr);

  CHECKST(hello == "hello"s);
  CHECKST(hello < "hfllo"s);
  CHECKST(hello <= "hfllo"s);
  CHECKST(hello != "hfllo"s);
  CHECKST(hello >= "hallo"s);
  CHECKST(hello >= "ahllo"s);
  CHECKST(hello > "hallo"s);
  CHECKST(hello > "hell"s);
  CHECKST(hello < "hello world"s);
  CHECKST(hello > ""s);

  CHECKST(hello == "hello"_t);
  CHECKST(hello < "hfllo"_t);
  CHECKST(hello <= "hfllo"_t);
  CHECKST(hello != "hfllo"_t);
  CHECKST(hello >= "hallo"_t);
  CHECKST(hello >= "ahllo"_t);
  CHECKST(hello > "hallo"_t);
  CHECKST(hello > "hell"_t);
  CHECKST(hello < "hello world"_t);
  CHECKST(hello > ""_t);

  log("Text comparisons [OK]");
}

void test_split() {
  Text txt("hello");
  CHECKST(get<0>(txt.splitPos(0)) == "");
  CHECKST(get<1>(txt.splitPos(0)) == "hello");
  CHECKST(get<0>(txt.splitPos(1)) == "h");
  CHECKST(get<1>(txt.splitPos(1)) == "ello");
  CHECKST(get<0>(txt.splitPos(2)) == "he");
  CHECKST(get<1>(txt.splitPos(2)) == "llo");
  CHECKST(get<0>(txt.splitPos(-2)) == "hel");
  CHECKST(get<1>(txt.splitPos(-2)) == "lo");
  CHECKST(get<0>(txt.splitPos(-1)) == "hell");
  CHECKST(get<1>(txt.splitPos(-1)) == "o");

  CHECKST(get<0>(txt.splitPos(-5)) == "");
  CHECKST(get<1>(txt.splitPos(-5)) == "hello");
  CHECKST(get<0>(txt.splitPos(-10)) == "");
  CHECKST(get<1>(txt.splitPos(-10)) == "hello");

  CHECKST(get<0>(txt.splitPos(5)) == "hello");
  CHECKST(get<1>(txt.splitPos(5)) == "");
  CHECKST(get<0>(txt.splitPos(15)) == "hello");
  CHECKST(get<1>(txt.splitPos(15)) == "");
  CHECKST(get<0>(""_t.splitPos(15)) == "");
  CHECKST(get<1>(""_t.splitPos(15)) == "");
  log("Text splits [OK]");
}

void test_contains() {
  Text t("hello");
  CHECKST(t.contains('h'));
  CHECKST(t.contains('e'));
  CHECKST(t.contains('o'));
  CHECKST(t.contains('l'));
  CHECKST(!t.contains('a'));
  CHECKST(!t.contains('\0'));
  CHECKST(!""_t.contains('\0'));
  log("Text contains [OK]");
}

void test_skip() {
  Text t("hello");
  CHECKST(t.skip("hel") == "o");
  CHECKST(t.skip("el") == "hello");
  CHECKST(t.skip("") == "hello");
  CHECKST(t.skip("abcdefghijklmnopqrstuvwxyz") == "");
  CHECKST(""_t.skip("abcdefghijklmnopqrstuvwxyz") == "");

  CHECKST(t.skip(2) == "llo"_t);
  CHECKST(t.skip(1) == "ello"_t);
  CHECKST(t.skip(4) == "o"_t);
  CHECKST(t.skip(5) == ""_t);
  CHECKST(t.skip(10) == ""_t);
  CHECKST(t.skip(100) == ""_t);

  log("Text skips [OK]");
}

void test_pos() {
  Text t("hello world");
  CHECKST(t.pos('l') == 2);
  CHECKST(t.pos('l', 2) == 3);
  CHECKST(t.pos('l', 3) == 9);
  CHECKST(t.pos('d') == 10);
  CHECKST(t.pos(' ') == 5);
  CHECKST(!t.pos('o', 3).has_value());
  CHECKST(!t.pos('k').has_value());

  CHECKST(!""_t.pos('x').has_value());

  CHECKST(t.pos("l"_t) == 2);
  CHECKST(t.pos("ll"_t) == 2);
  CHECKST(t.pos("l"_t, 2) == 3);
  CHECKST(t.pos("l"_t, 3) == 9);
  CHECKST(t.pos("o w"_t) == 4);
  CHECKST(t.pos("d"_t) == 10);
  CHECKST(!t.pos("o W"_t).has_value());
  CHECKST(!t.pos(""_t).has_value());
  CHECKST(!t.pos("hello world!"_t).has_value());
  CHECKST(t.pos("hello w"_t) == 0);
  CHECKST(t.pos("hello world"_t) == 0);

  CHECKST(!""_t.pos("x"_t).has_value());
  CHECKST(!""_t.pos(""_t).has_value());

  log("Text pos [OK]");
}

void test_split_char() {
  Text t1("My,CSV,text,with,,an,ending,,");
  auto sp1 = t1.splitByChar(',');

  CHECKST(sp1.size() == 6);
  CHECKST(sp1[0] == "My"_t);
  CHECKST(sp1[1] == "CSV"_t);
  CHECKST(sp1[2] == "text"_t);
  CHECKST(sp1[3] == "with"_t);
  CHECKST(sp1[4] == "an"_t);
  CHECKST(sp1[5] == "ending"_t);

  auto sp2 = t1.splitByChar(',', SplitEmpty::Keep);
  CHECKST(sp2.size() == 9);
  CHECKST(sp2[0] == "My"_t);
  CHECKST(sp2[1] == "CSV"_t);
  CHECKST(sp2[2] == "text"_t);
  CHECKST(sp2[3] == "with"_t);
  CHECKST(sp2[4] == ""_t);
  CHECKST(sp2[5] == "an"_t);
  CHECKST(sp2[6] == "ending"_t);
  CHECKST(sp2[7] == ""_t);
  CHECKST(sp2[8] == ""_t);

  auto sp3 = "Hello World"_t.splitByChar(' ');
  CHECKST(sp3.size() == 2);
  CHECKST(sp3[0] == "Hello"_t);
  CHECKST(sp3[1] == "World"_t);

  sp3 = "   Hello World"_t.splitByChar(' ');
  CHECKST(sp3.size() == 2);
  CHECKST(sp3[0] == "Hello"_t);
  CHECKST(sp3[1] == "World"_t);

  sp3 = "   Hello World"_t.splitByChar(' ', SplitEmpty::Keep);
  CHECKST(sp3.size() == 5);
  CHECKST(sp3[0] == ""_t);
  CHECKST(sp3[1] == ""_t);
  CHECKST(sp3[2] == ""_t);
  CHECKST(sp3[3] == "Hello"_t);
  CHECKST(sp3[4] == "World"_t);

  auto sp4 = "Hello_World"_t.splitByChar(' ');
  CHECKST(sp4.size() == 1);
  CHECKST(sp4[0] == "Hello_World"_t);

  log("Split by char [OK]");
}

void test_split_text() {
  Text t1("This||is||||||some||text||||");
  auto sp1 = t1.splitByText("||"_t);

  CHECKST(sp1.size() == 4);
  CHECKST(sp1[0] == "This"_t);
  CHECKST(sp1[1] == "is"_t);
  CHECKST(sp1[2] == "some"_t);
  CHECKST(sp1[3] == "text"_t);

  auto sp2 = t1.splitByText("||"_t, SplitEmpty::Keep);
  CHECKST(sp2.size() == 8);
  CHECKST(sp2[0] == "This"_t);
  CHECKST(sp2[1] == "is"_t);
  CHECKST(sp2[2] == ""_t);
  CHECKST(sp2[3] == ""_t);
  CHECKST(sp2[4] == "some"_t);
  CHECKST(sp2[5] == "text"_t);
  CHECKST(sp2[6] == ""_t);
  CHECKST(sp2[7] == ""_t);

  auto sp3 = "Hello World"_t.splitByText(" "_t);
  CHECKST(sp3.size() == 2);
  CHECKST(sp3[0] == "Hello"_t);
  CHECKST(sp3[1] == "World"_t);

  sp3 = "   Hello World"_t.splitByText(" "_t);
  CHECKST(sp3.size() == 2);
  CHECKST(sp3[0] == "Hello"_t);
  CHECKST(sp3[1] == "World"_t);

  sp3 = "   Hello World "_t.splitByText(" "_t, SplitEmpty::Keep);
  CHECKST(sp3.size() == 6);
  CHECKST(sp3[0] == ""_t);
  CHECKST(sp3[1] == ""_t);
  CHECKST(sp3[2] == ""_t);
  CHECKST(sp3[3] == "Hello"_t);
  CHECKST(sp3[4] == "World"_t);
  CHECKST(sp3[5] == ""_t);

  auto sp4 = "Hello_World"_t.splitByText(" "_t);
  CHECKST(sp4.size() == 1);
  CHECKST(sp4[0] == "Hello_World"_t);

  sp4 = "Hello"_t.splitByText("HelloWorld"_t);
  CHECKST(sp4.size() == 1);
  CHECKST(sp4[0] == "Hello"_t);

  sp4 = "Hello"_t.splitByText("Hello"_t);
  CHECKST(sp4.size() == 0);

  sp4 = "Hello"_t.splitByText(""_t);
  CHECKST(sp4.size() == 1);
  CHECKST(sp4[0] == "Hello"_t);

  sp4 = "ttttt"_t.splitByText("tt"_t);
  CHECKST(sp4.size() == 1);
  CHECKST(sp4[0] == "t"_t);

  log("Split by text [OK]");
}

void test_split_lines() {
  auto sp1 = "This\nis\nsome\ntext\n"_t.splitLines();
  CHECKST(sp1.size() == 5);
  CHECKST(sp1[0] == "This"_t);
  CHECKST(sp1[1] == "is"_t);
  CHECKST(sp1[2] == "some"_t);
  CHECKST(sp1[3] == "text"_t);
  CHECKST(sp1[4] == ""_t);

  sp1 = "This\nis\nsome\ntext\n"_t.splitLines(SplitEmpty::Discard);
  CHECKST(sp1.size() == 4);
  CHECKST(sp1[0] == "This"_t);
  CHECKST(sp1[1] == "is"_t);
  CHECKST(sp1[2] == "some"_t);
  CHECKST(sp1[3] == "text"_t);

  auto sp2 = "\n\nThis\n\nis\n\n\nsome\ntext"_t.splitLines();
  CHECKST(sp2.size() == 9);
  CHECKST(sp2[0] == ""_t);
  CHECKST(sp2[1] == ""_t);
  CHECKST(sp2[2] == "This"_t);
  CHECKST(sp2[3] == ""_t);
  CHECKST(sp2[4] == "is"_t);
  CHECKST(sp2[5] == ""_t);
  CHECKST(sp2[6] == ""_t);
  CHECKST(sp2[7] == "some"_t);
  CHECKST(sp2[8] == "text"_t);

  sp2 = "\n\r\n\rThis\n\r\n\ris\n\r\n\r\nsome\ntext"_t.splitLines();
  CHECKST(sp2.size() == 9);
  CHECKST(sp2[0] == ""_t);
  CHECKST(sp2[1] == ""_t);
  CHECKST(sp2[2] == "This"_t);
  CHECKST(sp2[3] == ""_t);
  CHECKST(sp2[4] == "is"_t);
  CHECKST(sp2[5] == ""_t);
  CHECKST(sp2[6] == ""_t);
  CHECKST(sp2[7] == "some"_t);
  CHECKST(sp2[8] == "text"_t);

  sp2 = "\r\n\r\nThis\r\n\r\nis\r\n\r\n\r\nsome\r\ntext"_t.splitLines();
  CHECKST(sp2.size() == 9);
  CHECKST(sp2[0] == ""_t);
  CHECKST(sp2[1] == ""_t);
  CHECKST(sp2[2] == "This"_t);
  CHECKST(sp2[3] == ""_t);
  CHECKST(sp2[4] == "is"_t);
  CHECKST(sp2[5] == ""_t);
  CHECKST(sp2[6] == ""_t);
  CHECKST(sp2[7] == "some"_t);
  CHECKST(sp2[8] == "text"_t);

  sp2 = "Hello\r\n"_t.splitLines();
  CHECKST(sp2.size() == 2);
  CHECKST(sp2[0] == "Hello"_t);
  CHECKST(sp2[1] == ""_t);

  sp2 = "Hello\r\n"_t.splitLines(SplitEmpty::Discard);
  CHECKST(sp2.size() == 1);
  CHECKST(sp2[0] == "Hello"_t);

  sp2 = "\r\n\r\nThis\r\n\r\nis\r\n\r\n\r\nsome\r\ntext"_t.splitLines(SplitEmpty::Discard);
  CHECKST(sp2.size() == 4);
  CHECKST(sp2[0] == "This"_t);
  CHECKST(sp2[1] == "is"_t);
  CHECKST(sp2[2] == "some"_t);
  CHECKST(sp2[3] == "text"_t);

  sp2 = "\n\r\n\rThis\n\r\n\ris\n\r\n\r\nsome\ntext"_t.splitLines(SplitEmpty::Discard);
  CHECKST(sp2.size() == 4);
  CHECKST(sp2[0] == "This"_t);
  CHECKST(sp2[1] == "is"_t);
  CHECKST(sp2[2] == "some"_t);
  CHECKST(sp2[3] == "text"_t);

  log("Split lines [OK]");
}

void test_last_pos() {
  auto filename = "/123456/89.1234"_t;
  CHECKST(filename.lastPos('.') == 10);
  CHECKST(filename.lastPos('/') == 7);
  CHECKST(filename.lastPos('4') == 14);
  CHECKST(filename.lastPos('9') == 9);
  CHECKST(!filename.lastPos('+').has_value());
  log("Last position [OK]");
}

void test_sublen_subpos() {
  Text t("0123456789");
  CHECKST(t.sublen(0, 2) == "01"_t);
  CHECKST(t.sublen(0, 4) == "0123"_t);
  CHECKST(t.sublen(2, 4) == "2345"_t);
  CHECKST(t.sublen(2, 0) == ""_t);
  CHECKST(t.sublen(8, 4) == "89"_t);

  CHECKST(t.subpos(0, 2) == "012"_t);
  CHECKST(t.subpos(0, 4) == "01234"_t);
  CHECKST(t.subpos(2, 4) == "234"_t);
  CHECKST(t.subpos(2, 0) == ""_t);
  CHECKST(t.subpos(2, 2) == "2"_t);
  CHECKST(t.sublen(8, 9) == "89"_t);
  CHECKST(t.sublen(8, 10) == "89"_t);
  CHECKST(t.sublen(8, 20) == "89"_t);

  log("sublen/subpos [OK]");
}

void test_count() {
  Text t("hello world");
  CHECKST(t.count('h') == 1);
  CHECKST(t.count('x') == 0);
  CHECKST(t.count('e') == 1);
  CHECKST(t.count('l') == 3);
  CHECKST(t.count('o') == 2);
  CHECKST(t.count(' ') == 1);
  log("count [OK]");
}

void test_fill_c_buff() {
  auto t = "123"_t;
  char buffer[100] = "Helloworld";
  t.fill_c_buffer(buffer, 100);
  CHECKST("123"s == buffer);
  t.fill_c_buffer(buffer, 3);
  CHECKST("12"s == buffer);
  t.fill_c_buffer(buffer, 2);
  CHECKST("1"s == buffer);
  t.fill_c_buffer(buffer, 1);
  CHECKST(""s == buffer);
  t.fill_c_buffer(buffer, 100);
  CHECKST("123"s == buffer);
  t.fill_c_buffer(buffer, 0);
  CHECKST("123"s == buffer);
  log("fill_c_buffer [OK]");
}

int main() {
  test_text_construction();
  test_trimming();
  test_ends_with();
  test_starts_with();
  test_comparisons();
  test_split();
  test_contains();
  test_skip();
  test_pos();
  test_split_char();
  test_split_text();
  test_split_lines();
  test_last_pos();
  test_sublen_subpos();
  test_fill_c_buff();
  test_count();
  return 0;
}
