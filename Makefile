.PHONY: executables makedirs
all: makedirs executables
build/ff/poorconfig.o: src/ff/poorconfig.cpp src/ff/poorconfig.h src/ff/textscanner.h src/kl/kl.h src/kl/klds.h src/kl/klexcept.h src/kl/klfs.h src/kl/kltext.h src/kl/kltime.h src/kl/klvalue.h
	g++ -c src/ff/poorconfig.cpp -o build/ff/poorconfig.o -Isrc -Isrc/ff -Isrc/kl -std=c++23 -g3 -O2 -flto -Werror -Wall -Wextra

build/ff/codescanner.o: src/ff/codescanner.cpp src/ff/codescanner.h src/ff/textscanner.h src/kl/kl.h src/kl/klds.h src/kl/klexcept.h src/kl/klfs.h src/kl/kltext.h src/kl/kltime.h
	g++ -c src/ff/codescanner.cpp -o build/ff/codescanner.o -Isrc -Isrc/ff -Isrc/kl -std=c++23 -g3 -O2 -flto -Werror -Wall -Wextra

build/ff/textscanner.o: src/ff/textscanner.cpp src/ff/textscanner.h src/kl/kl.h src/kl/klds.h src/kl/kltext.h
	g++ -c src/ff/textscanner.cpp -o build/ff/textscanner.o -Isrc -Isrc/ff -Isrc/kl -std=c++23 -g3 -O2 -flto -Werror -Wall -Wextra

build/genmakefile/klb-genmakefile.o: src/genmakefile/klb-genmakefile.cpp src/ff/poorconfig.h src/ff/codescanner.h src/ff/textscanner.h src/kl/klfs.h src/kl/klprocess.h src/kl/kl.h src/kl/kltime.h src/kl/klexcept.h src/kl/kltext.h src/kl/klds.h src/kl/klvalue.h src/klb/modulecollection.h src/klb/systemflags.h src/klb/klbfs.h src/klb/module.h src/klb/moduleitem.h src/klb/klbtoolchain.h src/klb/klbsettings.h src/klb/klbexecutor.h
	g++ -c src/genmakefile/klb-genmakefile.cpp -o build/genmakefile/klb-genmakefile.o -Isrc -Isrc/ff -Isrc/genmakefile -Isrc/kl -Isrc/klb -std=c++23 -g3 -O2 -flto -Werror -Wall -Wextra

build/kl/klfs.o: src/kl/klfs.cpp src/kl/kl.h src/kl/klds.h src/kl/klexcept.h src/kl/klfs.h src/kl/kltext.h src/kl/kltime.h
	g++ -c src/kl/klfs.cpp -o build/kl/klfs.o -Isrc -Isrc/kl -std=c++23 -g3 -O2 -flto -Werror -Wall -Wextra

build/kl/klprocess.o: src/kl/klprocess.cpp src/kl/kl.h src/kl/klds.h src/kl/klexcept.h src/kl/klfs.h src/kl/klprocess.h src/kl/kltext.h src/kl/kltime.h
	g++ -c src/kl/klprocess.cpp -o build/kl/klprocess.o -Isrc -Isrc/kl -std=c++23 -g3 -O2 -flto -Werror -Wall -Wextra

build/kl/kl.o: src/kl/kl.cpp 
	g++ -c src/kl/kl.cpp -o build/kl/kl.o -Isrc -std=c++23 -g3 -O2 -flto -Werror -Wall -Wextra

build/kl/kltime.o: src/kl/kltime.cpp src/ff/textscanner.h src/kl/kl.h src/kl/klds.h src/kl/klexcept.h src/kl/kltext.h src/kl/kltime.h
	g++ -c src/kl/kltime.cpp -o build/kl/kltime.o -Isrc -Isrc/ff -Isrc/kl -std=c++23 -g3 -O2 -flto -Werror -Wall -Wextra

build/kl/klexcept.o: src/kl/klexcept.cpp src/kl/kl.h src/kl/klds.h src/kl/klexcept.h src/kl/kltext.h
	g++ -c src/kl/klexcept.cpp -o build/kl/klexcept.o -Isrc -Isrc/kl -std=c++23 -g3 -O2 -flto -Werror -Wall -Wextra

build/kl/klnet.o: src/kl/klnet.cpp src/kl/kl.h src/kl/klds.h src/kl/klexcept.h src/kl/klio.h src/kl/klnet.h src/kl/kltext.h src/kl/kltime.h
	g++ -c src/kl/klnet.cpp -o build/kl/klnet.o -Isrc -Isrc/kl -std=c++23 -g3 -O2 -flto -Werror -Wall -Wextra

build/kl/klio.o: src/kl/klio.cpp src/kl/kl.h src/kl/klds.h src/kl/klexcept.h src/kl/klio.h src/kl/kltext.h
	g++ -c src/kl/klio.cpp -o build/kl/klio.o -Isrc -Isrc/kl -std=c++23 -g3 -O2 -flto -Werror -Wall -Wextra

build/kl/kltext.o: src/kl/kltext.cpp src/kl/kl.h src/kl/klds.h src/kl/kltext.h
	g++ -c src/kl/kltext.cpp -o build/kl/kltext.o -Isrc -Isrc/kl -std=c++23 -g3 -O2 -flto -Werror -Wall -Wextra

build/kl/klds.o: src/kl/klds.cpp 
	g++ -c src/kl/klds.cpp -o build/kl/klds.o -Isrc -std=c++23 -g3 -O2 -flto -Werror -Wall -Wextra

build/kl/kltimecounter.o: src/kl/kltimecounter.cpp src/kl/kl.h src/kl/klds.h src/kl/klexcept.h src/kl/kltext.h src/kl/kltime.h src/kl/kltimecounter.h
	g++ -c src/kl/kltimecounter.cpp -o build/kl/kltimecounter.o -Isrc -Isrc/kl -std=c++23 -g3 -O2 -flto -Werror -Wall -Wextra

build/kl/klvalue.o: src/kl/klvalue.cpp src/kl/kl.h src/kl/klds.h src/kl/klexcept.h src/kl/klfs.h src/kl/kltext.h src/kl/kltime.h src/kl/klvalue.h
	g++ -c src/kl/klvalue.cpp -o build/kl/klvalue.o -Isrc -Isrc/kl -std=c++23 -g3 -O2 -flto -Werror -Wall -Wextra

build/klb/modulecollection.o: src/klb/modulecollection.cpp src/ff/codescanner.h src/kl/kl.h src/kl/klds.h src/kl/klexcept.h src/kl/klfs.h src/kl/kltext.h src/kl/kltime.h src/kl/klvalue.h src/klb/klbfs.h src/klb/klbsettings.h src/klb/module.h src/klb/modulecollection.h src/klb/moduleitem.h src/klb/systemflags.h
	g++ -c src/klb/modulecollection.cpp -o build/klb/modulecollection.o -Isrc -Isrc/ff -Isrc/kl -Isrc/klb -std=c++23 -g3 -O2 -flto -Werror -Wall -Wextra

build/klb/systemflags.o: src/klb/systemflags.cpp src/kl/kl.h src/kl/klds.h src/kl/klexcept.h src/kl/klfs.h src/kl/kltext.h src/kl/kltime.h src/kl/klvalue.h src/klb/klbsettings.h src/klb/systemflags.h
	g++ -c src/klb/systemflags.cpp -o build/klb/systemflags.o -Isrc -Isrc/kl -Isrc/klb -std=c++23 -g3 -O2 -flto -Werror -Wall -Wextra

build/klb/klbfs.o: src/klb/klbfs.cpp src/kl/kl.h src/kl/klds.h src/kl/klexcept.h src/kl/klfs.h src/kl/kltext.h src/kl/kltime.h src/kl/klvalue.h src/klb/klbfs.h src/klb/klbsettings.h src/klb/systemflags.h
	g++ -c src/klb/klbfs.cpp -o build/klb/klbfs.o -Isrc -Isrc/kl -Isrc/klb -std=c++23 -g3 -O2 -flto -Werror -Wall -Wextra

build/klb/module.o: src/klb/module.cpp src/ff/codescanner.h src/kl/kl.h src/kl/klds.h src/kl/klexcept.h src/kl/klfs.h src/kl/kltext.h src/kl/kltime.h src/kl/klvalue.h src/klb/klbfs.h src/klb/klbsettings.h src/klb/module.h src/klb/modulecollection.h src/klb/moduleitem.h src/klb/systemflags.h
	g++ -c src/klb/module.cpp -o build/klb/module.o -Isrc -Isrc/ff -Isrc/kl -Isrc/klb -std=c++23 -g3 -O2 -flto -Werror -Wall -Wextra

build/klb/moduleitem.o: src/klb/moduleitem.cpp src/kl/kl.h src/kl/klds.h src/kl/klexcept.h src/kl/klfs.h src/kl/kltext.h src/kl/kltime.h src/kl/klvalue.h src/klb/klbsettings.h src/klb/moduleitem.h src/klb/systemflags.h
	g++ -c src/klb/moduleitem.cpp -o build/klb/moduleitem.o -Isrc -Isrc/kl -Isrc/klb -std=c++23 -g3 -O2 -flto -Werror -Wall -Wextra

build/klb/klbtoolchain.o: src/klb/klbtoolchain.cpp src/kl/kl.h src/kl/klds.h src/kl/klexcept.h src/kl/klfs.h src/kl/klprocess.h src/kl/kltext.h src/kl/kltime.h src/kl/klvalue.h src/klb/klbsettings.h src/klb/klbtoolchain.h src/klb/systemflags.h
	g++ -c src/klb/klbtoolchain.cpp -o build/klb/klbtoolchain.o -Isrc -Isrc/kl -Isrc/klb -std=c++23 -g3 -O2 -flto -Werror -Wall -Wextra

build/klb/klbsettings.o: src/klb/klbsettings.cpp src/ff/poorconfig.h src/ff/textscanner.h src/kl/kl.h src/kl/klds.h src/kl/klexcept.h src/kl/klfs.h src/kl/kltext.h src/kl/kltime.h src/kl/klvalue.h src/klb/klbsettings.h src/klb/systemflags.h
	g++ -c src/klb/klbsettings.cpp -o build/klb/klbsettings.o -Isrc -Isrc/ff -Isrc/kl -Isrc/klb -std=c++23 -g3 -O2 -flto -Werror -Wall -Wextra

build/klb/klbexecutor.o: src/klb/klbexecutor.cpp src/kl/kl.h src/kl/klds.h src/kl/klexcept.h src/kl/klfs.h src/kl/klprocess.h src/kl/kltext.h src/kl/kltime.h src/kl/klvalue.h src/klb/klbexecutor.h src/klb/klbfs.h src/klb/klbsettings.h src/klb/klbtoolchain.h src/klb/module.h src/klb/modulecollection.h src/klb/moduleitem.h src/klb/systemflags.h
	g++ -c src/klb/klbexecutor.cpp -o build/klb/klbexecutor.o -Isrc -Isrc/kl -Isrc/klb -std=c++23 -g3 -O2 -flto -Werror -Wall -Wextra

build/klb/klb.o: src/klb/klb.cpp src/ff/poorconfig.h src/ff/codescanner.h src/ff/textscanner.h src/kl/klfs.h src/kl/klprocess.h src/kl/kl.h src/kl/kltime.h src/kl/klexcept.h src/kl/kltext.h src/kl/klds.h src/kl/klvalue.h src/klb/modulecollection.h src/klb/systemflags.h src/klb/klbfs.h src/klb/module.h src/klb/moduleitem.h src/klb/klbtoolchain.h src/klb/klbsettings.h src/klb/klbexecutor.h
	g++ -c src/klb/klb.cpp -o build/klb/klb.o -Isrc -Isrc/ff -Isrc/kl -Isrc/klb -std=c++23 -g3 -O2 -flto -Werror -Wall -Wextra

build/klblog/blogsettings.o: src/klblog/blogsettings.cpp src/ff/poorconfig.h src/ff/textscanner.h src/kl/kl.h src/kl/klds.h src/kl/kltext.h src/kl/klvalue.h src/klblog/blogsettings.hpp
	g++ -c src/klblog/blogsettings.cpp -o build/klblog/blogsettings.o -Isrc -Isrc/ff -Isrc/kl -Isrc/klblog -std=c++23 -g3 -O2 -flto -Werror -Wall -Wextra

build/klblog/systemsettings.o: src/klblog/systemsettings.cpp src/kl/kl.h src/kl/klds.h src/kl/kltext.h src/klblog/systemsettings.hpp
	g++ -c src/klblog/systemsettings.cpp -o build/klblog/systemsettings.o -Isrc -Isrc/kl -Isrc/klblog -std=c++23 -g3 -O2 -flto -Werror -Wall -Wextra

build/klblog/blog.o: src/klblog/blog.cpp src/ff/poorconfig.h src/ff/textscanner.h src/kl/kl.h src/kl/klds.h src/kl/klexcept.h src/kl/klfs.h src/kl/kltext.h src/kl/kltime.h src/kl/klvalue.h src/klblog/blog.hpp src/klblog/blogsettings.hpp src/klblog/systemsettings.hpp
	g++ -c src/klblog/blog.cpp -o build/klblog/blog.o -Isrc -Isrc/ff -Isrc/kl -Isrc/klblog -std=c++23 -g3 -O2 -flto -Werror -Wall -Wextra

build/klblog/klblog.o: src/klblog/klblog.cpp src/ff/poorconfig.h src/ff/textscanner.h src/kl/klfs.h src/kl/kl.h src/kl/kltime.h src/kl/klexcept.h src/kl/kltext.h src/kl/klds.h src/kl/kltimecounter.h src/kl/klvalue.h src/klblog/blogsettings.hpp src/klblog/systemsettings.hpp src/klblog/blog.hpp
	g++ -c src/klblog/klblog.cpp -o build/klblog/klblog.o -Isrc -Isrc/ff -Isrc/kl -Isrc/klblog -std=c++23 -g3 -O2 -flto -Werror -Wall -Wextra

build/tests/klio.o: src/tests/klio.cpp src/kl/kl.h src/kl/klexcept.h src/kl/klio.h src/kl/kltext.h src/kl/klds.h
	g++ -c src/tests/klio.cpp -o build/tests/klio.o -Isrc -Isrc/kl -Isrc/tests -std=c++23 -g3 -O2 -flto -Werror -Wall -Wextra

build/tests/klnet.o: src/tests/klnet.cpp src/ff/textscanner.h src/kl/kl.h src/kl/kltime.h src/kl/klexcept.h src/kl/klnet.h src/kl/klio.h src/kl/kltext.h src/kl/klds.h
	g++ -c src/tests/klnet.cpp -o build/tests/klnet.o -Isrc -Isrc/ff -Isrc/kl -Isrc/tests -std=c++23 -g3 -O2 -flto -Werror -Wall -Wextra

build/tests/klfs-lsrec.o: src/tests/klfs-lsrec.cpp src/ff/textscanner.h src/kl/klfs.h src/kl/kl.h src/kl/kltime.h src/kl/klexcept.h src/kl/kltext.h src/kl/klds.h
	g++ -c src/tests/klfs-lsrec.cpp -o build/tests/klfs-lsrec.o -Isrc -Isrc/ff -Isrc/kl -Isrc/tests -std=c++23 -g3 -O2 -flto -Werror -Wall -Wextra

build/tests/process-test.o: src/tests/process-test.cpp src/ff/textscanner.h src/kl/klfs.h src/kl/klprocess.h src/kl/kl.h src/kl/kltime.h src/kl/klexcept.h src/kl/kltext.h src/kl/klds.h
	g++ -c src/tests/process-test.cpp -o build/tests/process-test.o -Isrc -Isrc/ff -Isrc/kl -Isrc/tests -std=c++23 -g3 -O2 -flto -Werror -Wall -Wextra

build/ff/mw/markwrite.o: src/ff/mw/markwrite.cpp src/ff/mw/markwrite.h src/ff/poorconfig.h src/ff/textscanner.h src/kl/kl.h src/kl/klds.h src/kl/klexcept.h src/kl/kltext.h src/kl/kltime.h src/kl/klvalue.h
	g++ -c src/ff/mw/markwrite.cpp -o build/ff/mw/markwrite.o -Isrc -Isrc/ff -Isrc/ff/mw -Isrc/kl -std=c++23 -g3 -O2 -flto -Werror -Wall -Wextra

build/tests/file-formats/test-markwrite.o: src/tests/file-formats/test-markwrite.cpp src/ff/poorconfig.h src/ff/textscanner.h src/kl/klfs.h src/kl/kl.h src/kl/kltime.h src/kl/klexcept.h src/kl/kltext.h src/kl/klds.h src/kl/klvalue.h src/ff/mw/markwrite.h
	g++ -c src/tests/file-formats/test-markwrite.cpp -o build/tests/file-formats/test-markwrite.o -Isrc -Isrc/ff -Isrc/ff/mw -Isrc/kl -Isrc/tests/file-formats -std=c++23 -g3 -O2 -flto -Werror -Wall -Wextra

build/tests/file-formats/test-poorconfig.o: src/tests/file-formats/test-poorconfig.cpp src/ff/poorconfig.h src/ff/textscanner.h src/kl/klfs.h src/kl/kl.h src/kl/kltime.h src/kl/klexcept.h src/kl/kltext.h src/kl/klds.h src/kl/klvalue.h
	g++ -c src/tests/file-formats/test-poorconfig.cpp -o build/tests/file-formats/test-poorconfig.o -Isrc -Isrc/ff -Isrc/kl -Isrc/tests/file-formats -std=c++23 -g3 -O2 -flto -Werror -Wall -Wextra

build/tests/file-formats/test-scanner.o: src/tests/file-formats/test-scanner.cpp src/ff/textscanner.h src/kl/kl.h src/kl/kltext.h src/kl/klds.h
	g++ -c src/tests/file-formats/test-scanner.cpp -o build/tests/file-formats/test-scanner.o -Isrc -Isrc/ff -Isrc/kl -Isrc/tests/file-formats -std=c++23 -g3 -O2 -flto -Werror -Wall -Wextra

build/tests/kl/klvalue-test.o: src/tests/kl/klvalue-test.cpp src/kl/kl.h src/kl/klds.h src/kl/kltext.h src/kl/klvalue.h
	g++ -c src/tests/kl/klvalue-test.cpp -o build/tests/kl/klvalue-test.o -Isrc -Isrc/kl -DGTEST_HAS_PTHREAD=1 -std=c++23 -g3 -O2 -flto -Werror -Wall -Wextra

build/tests/kl/kldate-test.o: src/tests/kl/kldate-test.cpp src/kl/kl.h src/kl/klds.h src/kl/klexcept.h src/kl/kltext.h src/kl/kltime.h
	g++ -c src/tests/kl/kldate-test.cpp -o build/tests/kl/kldate-test.o -Isrc -Isrc/kl -DGTEST_HAS_PTHREAD=1 -std=c++23 -g3 -O2 -flto -Werror -Wall -Wextra

build/tests/kl/kltext-test.o: src/tests/kl/kltext-test.cpp src/kl/kl.h src/kl/klds.h src/kl/kltext.h
	g++ -c src/tests/kl/kltext-test.cpp -o build/tests/kl/kltext-test.o -Isrc -Isrc/kl -DGTEST_HAS_PTHREAD=1 -std=c++23 -g3 -O2 -flto -Werror -Wall -Wextra

build/tests/kl/klfs-test.o: src/tests/kl/klfs-test.cpp src/kl/kl.h src/kl/klds.h src/kl/klexcept.h src/kl/klfs.h src/kl/kltext.h src/kl/kltime.h
	g++ -c src/tests/kl/klfs-test.cpp -o build/tests/kl/klfs-test.o -Isrc -Isrc/kl -DGTEST_HAS_PTHREAD=1 -std=c++23 -g3 -O2 -flto -Werror -Wall -Wextra

build/tests/kl/kltest.o: src/tests/kl/kltest.cpp src/ff/textscanner.h src/kl/klfs.h src/kl/kl.h src/kl/kltime.h src/kl/klexcept.h src/kl/kltext.h src/kl/klds.h src/kl/klvalue.h src/tests/kl/klvalue-test.h src/tests/kl/kldate-test.h src/tests/kl/kltext-test.h src/tests/kl/klfs-test.h
	g++ -c src/tests/kl/kltest.cpp -o build/tests/kl/kltest.o -Isrc -Isrc/ff -Isrc/kl -Isrc/tests/kl -DGTEST_HAS_PTHREAD=1 -std=c++23 -g3 -O2 -flto -Werror -Wall -Wextra

build/genmakefile/klb-genmakefile.exe: build/ff/codescanner.o build/ff/poorconfig.o build/ff/textscanner.o build/genmakefile/klb-genmakefile.o build/kl/klexcept.o build/kl/klfs.o build/kl/klprocess.o build/kl/kltext.o build/kl/kltime.o build/kl/klvalue.o build/klb/klbexecutor.o build/klb/klbfs.o build/klb/klbsettings.o build/klb/klbtoolchain.o build/klb/module.o build/klb/modulecollection.o build/klb/moduleitem.o build/klb/systemflags.o
	g++ -o build/genmakefile/klb-genmakefile.exe build/ff/codescanner.o build/ff/poorconfig.o build/ff/textscanner.o build/genmakefile/klb-genmakefile.o build/kl/klexcept.o build/kl/klfs.o build/kl/klprocess.o build/kl/kltext.o build/kl/kltime.o build/kl/klvalue.o build/klb/klbexecutor.o build/klb/klbfs.o build/klb/klbsettings.o build/klb/klbtoolchain.o build/klb/module.o build/klb/modulecollection.o build/klb/moduleitem.o build/klb/systemflags.o -flto=auto -ltcmalloc -lfmt

build/klb/klb.exe: build/ff/codescanner.o build/ff/poorconfig.o build/ff/textscanner.o build/kl/klexcept.o build/kl/klfs.o build/kl/klprocess.o build/kl/kltext.o build/kl/kltime.o build/kl/klvalue.o build/klb/klb.o build/klb/klbexecutor.o build/klb/klbfs.o build/klb/klbsettings.o build/klb/klbtoolchain.o build/klb/module.o build/klb/modulecollection.o build/klb/moduleitem.o build/klb/systemflags.o
	g++ -o build/klb/klb.exe build/ff/codescanner.o build/ff/poorconfig.o build/ff/textscanner.o build/kl/klexcept.o build/kl/klfs.o build/kl/klprocess.o build/kl/kltext.o build/kl/kltime.o build/kl/klvalue.o build/klb/klb.o build/klb/klbexecutor.o build/klb/klbfs.o build/klb/klbsettings.o build/klb/klbtoolchain.o build/klb/module.o build/klb/modulecollection.o build/klb/moduleitem.o build/klb/systemflags.o -flto=auto -ltcmalloc -lfmt

build/klblog/klblog.exe: build/ff/poorconfig.o build/ff/textscanner.o build/kl/klexcept.o build/kl/klfs.o build/kl/kltext.o build/kl/kltime.o build/kl/kltimecounter.o build/kl/klvalue.o build/klblog/blog.o build/klblog/blogsettings.o build/klblog/klblog.o build/klblog/systemsettings.o
	g++ -o build/klblog/klblog.exe build/ff/poorconfig.o build/ff/textscanner.o build/kl/klexcept.o build/kl/klfs.o build/kl/kltext.o build/kl/kltime.o build/kl/kltimecounter.o build/kl/klvalue.o build/klblog/blog.o build/klblog/blogsettings.o build/klblog/klblog.o build/klblog/systemsettings.o -flto=auto -ltcmalloc -lfmt

build/tests/klio.exe: build/kl/klexcept.o build/kl/klio.o build/kl/kltext.o build/tests/klio.o
	g++ -o build/tests/klio.exe build/kl/klexcept.o build/kl/klio.o build/kl/kltext.o build/tests/klio.o -flto=auto -ltcmalloc -lfmt

build/tests/klnet.exe: build/ff/textscanner.o build/kl/klexcept.o build/kl/klio.o build/kl/klnet.o build/kl/kltext.o build/kl/kltime.o build/tests/klnet.o
	g++ -o build/tests/klnet.exe build/ff/textscanner.o build/kl/klexcept.o build/kl/klio.o build/kl/klnet.o build/kl/kltext.o build/kl/kltime.o build/tests/klnet.o -flto=auto -ltcmalloc -lfmt -lssl -lcrypto

build/tests/klfs-lsrec.exe: build/ff/textscanner.o build/kl/klexcept.o build/kl/klfs.o build/kl/kltext.o build/kl/kltime.o build/tests/klfs-lsrec.o
	g++ -o build/tests/klfs-lsrec.exe build/ff/textscanner.o build/kl/klexcept.o build/kl/klfs.o build/kl/kltext.o build/kl/kltime.o build/tests/klfs-lsrec.o -flto=auto -ltcmalloc -lfmt

build/tests/process-test.exe: build/ff/textscanner.o build/kl/klexcept.o build/kl/klfs.o build/kl/klprocess.o build/kl/kltext.o build/kl/kltime.o build/tests/process-test.o
	g++ -o build/tests/process-test.exe build/ff/textscanner.o build/kl/klexcept.o build/kl/klfs.o build/kl/klprocess.o build/kl/kltext.o build/kl/kltime.o build/tests/process-test.o -flto=auto -ltcmalloc -lfmt

build/tests/file-formats/test-markwrite.exe: build/ff/mw/markwrite.o build/ff/poorconfig.o build/ff/textscanner.o build/kl/klexcept.o build/kl/klfs.o build/kl/kltext.o build/kl/kltime.o build/kl/klvalue.o build/tests/file-formats/test-markwrite.o
	g++ -o build/tests/file-formats/test-markwrite.exe build/ff/mw/markwrite.o build/ff/poorconfig.o build/ff/textscanner.o build/kl/klexcept.o build/kl/klfs.o build/kl/kltext.o build/kl/kltime.o build/kl/klvalue.o build/tests/file-formats/test-markwrite.o -flto=auto -ltcmalloc -lfmt

build/tests/file-formats/test-poorconfig.exe: build/ff/poorconfig.o build/ff/textscanner.o build/kl/klexcept.o build/kl/klfs.o build/kl/kltext.o build/kl/kltime.o build/kl/klvalue.o build/tests/file-formats/test-poorconfig.o
	g++ -o build/tests/file-formats/test-poorconfig.exe build/ff/poorconfig.o build/ff/textscanner.o build/kl/klexcept.o build/kl/klfs.o build/kl/kltext.o build/kl/kltime.o build/kl/klvalue.o build/tests/file-formats/test-poorconfig.o -flto=auto -ltcmalloc -lfmt

build/tests/file-formats/test-scanner.exe: build/ff/textscanner.o build/kl/kltext.o build/tests/file-formats/test-scanner.o
	g++ -o build/tests/file-formats/test-scanner.exe build/ff/textscanner.o build/kl/kltext.o build/tests/file-formats/test-scanner.o -flto=auto -ltcmalloc -lfmt

build/tests/kl/kltest.exe: build/ff/textscanner.o build/kl/klexcept.o build/kl/klfs.o build/kl/kltext.o build/kl/kltime.o build/kl/klvalue.o build/tests/kl/kldate-test.o build/tests/kl/klfs-test.o build/tests/kl/kltest.o build/tests/kl/kltext-test.o build/tests/kl/klvalue-test.o
	g++ -o build/tests/kl/kltest.exe build/ff/textscanner.o build/kl/klexcept.o build/kl/klfs.o build/kl/kltext.o build/kl/kltime.o build/kl/klvalue.o build/tests/kl/kldate-test.o build/tests/kl/klfs-test.o build/tests/kl/kltest.o build/tests/kl/kltext-test.o build/tests/kl/klvalue-test.o -flto=auto -ltcmalloc -lgtest -lfmt

executables: build/genmakefile/klb-genmakefile.exe build/klb/klb.exe build/klblog/klblog.exe build/tests/klio.exe build/tests/klnet.exe build/tests/klfs-lsrec.exe build/tests/process-test.exe build/tests/file-formats/test-markwrite.exe build/tests/file-formats/test-poorconfig.exe build/tests/file-formats/test-scanner.exe build/tests/kl/kltest.exe
makedirs:
	mkdir -p build/ff build/ff/mw build/genmakefile build/kl build/klb build/klblog build/tests build/tests/file-formats build/tests/kl
