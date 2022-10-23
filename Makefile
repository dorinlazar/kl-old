.PHONY: executables makedirs
all: makedirs executables
build/ff/textscanner.o: src/ff/textscanner.cpp src/ff/textscanner.h src/kl/kl.h src/kl/klds.h src/kl/kltext.h
	g++ -Wall -Wextra -c -std=c++20 -g3 -O2 -flto -Werror -o build/ff/textscanner.o src/ff/textscanner.cpp -Isrc -Isrc/ff -Isrc/kl
build/ff/poorconfig.o: src/ff/poorconfig.cpp src/ff/poorconfig.h src/ff/textscanner.h src/kl/kl.h src/kl/klds.h src/kl/klexcept.h src/kl/klfs.h src/kl/kltext.h src/kl/kltime.h src/kl/klvalue.h
	g++ -Wall -Wextra -c -std=c++20 -g3 -O2 -flto -Werror -o build/ff/poorconfig.o src/ff/poorconfig.cpp -Isrc -Isrc/ff -Isrc/kl
build/ff/codescanner.o: src/ff/codescanner.cpp src/ff/codescanner.h src/ff/textscanner.h src/kl/kl.h src/kl/klds.h src/kl/klexcept.h src/kl/klfs.h src/kl/kltext.h src/kl/kltime.h
	g++ -Wall -Wextra -c -std=c++20 -g3 -O2 -flto -Werror -o build/ff/codescanner.o src/ff/codescanner.cpp -Isrc -Isrc/ff -Isrc/kl
build/genmakefile/klb-genmakefile.o: src/genmakefile/klb-genmakefile.cpp src/ff/textscanner.h src/ff/poorconfig.h src/ff/codescanner.h src/kl/kltext.h src/kl/klfs.h src/kl/klexcept.h src/kl/kl.h src/kl/klds.h src/kl/kltime.h src/kl/klprocess.h src/kl/klvalue.h src/klb/modulecollection.h src/klb/systemflags.h src/klb/klbsettings.h src/klb/klbfs.h src/klb/moduleitem.h src/klb/klbexecutor.h src/klb/module.h src/klb/klbtoolchain.h
	g++ -Wall -Wextra -c -std=c++20 -g3 -O2 -flto -Werror -o build/genmakefile/klb-genmakefile.o src/genmakefile/klb-genmakefile.cpp -Isrc -Isrc/ff -Isrc/genmakefile -Isrc/kl -Isrc/klb
build/kl/kltext.o: src/kl/kltext.cpp src/kl/kl.h src/kl/klds.h src/kl/kltext.h
	g++ -Wall -Wextra -c -std=c++20 -g3 -O2 -flto -Werror -o build/kl/kltext.o src/kl/kltext.cpp -Isrc -Isrc/kl
build/kl/klfs.o: src/kl/klfs.cpp src/kl/kl.h src/kl/klds.h src/kl/klexcept.h src/kl/klfs.h src/kl/kltext.h src/kl/kltime.h
	g++ -Wall -Wextra -c -std=c++20 -g3 -O2 -flto -Werror -o build/kl/klfs.o src/kl/klfs.cpp -Isrc -Isrc/kl
build/kl/klexcept.o: src/kl/klexcept.cpp src/kl/kl.h src/kl/klds.h src/kl/klexcept.h src/kl/kltext.h
	g++ -Wall -Wextra -c -std=c++20 -g3 -O2 -flto -Werror -o build/kl/klexcept.o src/kl/klexcept.cpp -Isrc -Isrc/kl
build/kl/klio.o: src/kl/klio.cpp src/kl/kl.h src/kl/klds.h src/kl/klexcept.h src/kl/klio.h src/kl/kltext.h
	g++ -Wall -Wextra -c -std=c++20 -g3 -O2 -flto -Werror -o build/kl/klio.o src/kl/klio.cpp -Isrc -Isrc/kl
build/kl/kl.o: src/kl/kl.cpp 
	g++ -Wall -Wextra -c -std=c++20 -g3 -O2 -flto -Werror -o build/kl/kl.o src/kl/kl.cpp -Isrc
build/kl/klds.o: src/kl/klds.cpp 
	g++ -Wall -Wextra -c -std=c++20 -g3 -O2 -flto -Werror -o build/kl/klds.o src/kl/klds.cpp -Isrc
build/kl/klnet.o: src/kl/klnet.cpp src/kl/kl.h src/kl/klds.h src/kl/klexcept.h src/kl/klio.h src/kl/klnet.h src/kl/kltext.h src/kl/kltime.h
	g++ -Wall -Wextra -c -std=c++20 -g3 -O2 -flto -Werror -o build/kl/klnet.o src/kl/klnet.cpp -Isrc -Isrc/kl
build/kl/kltime.o: src/kl/kltime.cpp src/ff/textscanner.h src/kl/kl.h src/kl/klds.h src/kl/klexcept.h src/kl/kltext.h src/kl/kltime.h
	g++ -Wall -Wextra -c -std=c++20 -g3 -O2 -flto -Werror -o build/kl/kltime.o src/kl/kltime.cpp -Isrc -Isrc/ff -Isrc/kl
build/kl/klprocess.o: src/kl/klprocess.cpp src/kl/kl.h src/kl/klds.h src/kl/klexcept.h src/kl/klfs.h src/kl/klprocess.h src/kl/kltext.h src/kl/kltime.h
	g++ -Wall -Wextra -c -std=c++20 -g3 -O2 -flto -Werror -o build/kl/klprocess.o src/kl/klprocess.cpp -Isrc -Isrc/kl
build/kl/klvalue.o: src/kl/klvalue.cpp src/kl/kl.h src/kl/klds.h src/kl/klexcept.h src/kl/klfs.h src/kl/kltext.h src/kl/kltime.h src/kl/klvalue.h
	g++ -Wall -Wextra -c -std=c++20 -g3 -O2 -flto -Werror -o build/kl/klvalue.o src/kl/klvalue.cpp -Isrc -Isrc/kl
build/klb/modulecollection.o: src/klb/modulecollection.cpp src/ff/codescanner.h src/kl/kl.h src/kl/klds.h src/kl/klexcept.h src/kl/klfs.h src/kl/kltext.h src/kl/kltime.h src/kl/klvalue.h src/klb/klbfs.h src/klb/klbsettings.h src/klb/module.h src/klb/modulecollection.h src/klb/moduleitem.h src/klb/systemflags.h
	g++ -Wall -Wextra -c -std=c++20 -g3 -O2 -flto -Werror -o build/klb/modulecollection.o src/klb/modulecollection.cpp -Isrc -Isrc/ff -Isrc/kl -Isrc/klb
build/klb/systemflags.o: src/klb/systemflags.cpp src/kl/kl.h src/kl/klds.h src/kl/klexcept.h src/kl/klfs.h src/kl/kltext.h src/kl/kltime.h src/kl/klvalue.h src/klb/klbsettings.h src/klb/systemflags.h
	g++ -Wall -Wextra -c -std=c++20 -g3 -O2 -flto -Werror -o build/klb/systemflags.o src/klb/systemflags.cpp -Isrc -Isrc/kl -Isrc/klb
build/klb/klbsettings.o: src/klb/klbsettings.cpp src/ff/poorconfig.h src/ff/textscanner.h src/kl/kl.h src/kl/klds.h src/kl/klexcept.h src/kl/klfs.h src/kl/kltext.h src/kl/kltime.h src/kl/klvalue.h src/klb/klbsettings.h src/klb/systemflags.h
	g++ -Wall -Wextra -c -std=c++20 -g3 -O2 -flto -Werror -o build/klb/klbsettings.o src/klb/klbsettings.cpp -Isrc -Isrc/ff -Isrc/kl -Isrc/klb
build/klb/klbfs.o: src/klb/klbfs.cpp src/kl/kl.h src/kl/klds.h src/kl/klexcept.h src/kl/klfs.h src/kl/kltext.h src/kl/kltime.h src/kl/klvalue.h src/klb/klbfs.h src/klb/klbsettings.h src/klb/systemflags.h
	g++ -Wall -Wextra -c -std=c++20 -g3 -O2 -flto -Werror -o build/klb/klbfs.o src/klb/klbfs.cpp -Isrc -Isrc/kl -Isrc/klb
build/klb/moduleitem.o: src/klb/moduleitem.cpp src/kl/kl.h src/kl/klds.h src/kl/klexcept.h src/kl/klfs.h src/kl/kltext.h src/kl/kltime.h src/kl/klvalue.h src/klb/klbsettings.h src/klb/moduleitem.h src/klb/systemflags.h
	g++ -Wall -Wextra -c -std=c++20 -g3 -O2 -flto -Werror -o build/klb/moduleitem.o src/klb/moduleitem.cpp -Isrc -Isrc/kl -Isrc/klb
build/klb/klbexecutor.o: src/klb/klbexecutor.cpp src/kl/kl.h src/kl/klds.h src/kl/klexcept.h src/kl/klfs.h src/kl/klprocess.h src/kl/kltext.h src/kl/kltime.h src/kl/klvalue.h src/klb/klbexecutor.h src/klb/klbfs.h src/klb/klbsettings.h src/klb/klbtoolchain.h src/klb/module.h src/klb/modulecollection.h src/klb/moduleitem.h src/klb/systemflags.h
	g++ -Wall -Wextra -c -std=c++20 -g3 -O2 -flto -Werror -o build/klb/klbexecutor.o src/klb/klbexecutor.cpp -Isrc -Isrc/kl -Isrc/klb
build/klb/klb.o: src/klb/klb.cpp src/ff/textscanner.h src/ff/poorconfig.h src/ff/codescanner.h src/kl/kltext.h src/kl/klfs.h src/kl/klexcept.h src/kl/kl.h src/kl/klds.h src/kl/kltime.h src/kl/klprocess.h src/kl/klvalue.h src/klb/modulecollection.h src/klb/systemflags.h src/klb/klbsettings.h src/klb/klbfs.h src/klb/moduleitem.h src/klb/klbexecutor.h src/klb/module.h src/klb/klbtoolchain.h
	g++ -Wall -Wextra -c -std=c++20 -g3 -O2 -flto -Werror -o build/klb/klb.o src/klb/klb.cpp -Isrc -Isrc/ff -Isrc/kl -Isrc/klb
build/klb/module.o: src/klb/module.cpp src/ff/codescanner.h src/kl/kl.h src/kl/klds.h src/kl/klexcept.h src/kl/klfs.h src/kl/kltext.h src/kl/kltime.h src/kl/klvalue.h src/klb/klbfs.h src/klb/klbsettings.h src/klb/module.h src/klb/modulecollection.h src/klb/moduleitem.h src/klb/systemflags.h
	g++ -Wall -Wextra -c -std=c++20 -g3 -O2 -flto -Werror -o build/klb/module.o src/klb/module.cpp -Isrc -Isrc/ff -Isrc/kl -Isrc/klb
build/klb/klbtoolchain.o: src/klb/klbtoolchain.cpp src/kl/kl.h src/kl/klds.h src/kl/klexcept.h src/kl/klfs.h src/kl/klprocess.h src/kl/kltext.h src/kl/kltime.h src/kl/klvalue.h src/klb/klbsettings.h src/klb/klbtoolchain.h src/klb/systemflags.h
	g++ -Wall -Wextra -c -std=c++20 -g3 -O2 -flto -Werror -o build/klb/klbtoolchain.o src/klb/klbtoolchain.cpp -Isrc -Isrc/kl -Isrc/klb
build/klblog/klblog.o: src/klblog/klblog.cpp src/ff/textscanner.h src/ff/poorconfig.h src/kl/kltext.h src/kl/klfs.h src/kl/klexcept.h src/kl/kl.h src/kl/klds.h src/kl/kltime.h src/kl/klvalue.h src/klblog/systemsettings.hpp
	g++ -Wall -Wextra -c -std=c++20 -g3 -O2 -flto -Werror -o build/klblog/klblog.o src/klblog/klblog.cpp -Isrc -Isrc/ff -Isrc/kl -Isrc/klblog
build/klblog/systemsettings.o: src/klblog/systemsettings.cpp src/kl/kl.h src/kl/klds.h src/kl/kltext.h src/klblog/systemsettings.hpp
	g++ -Wall -Wextra -c -std=c++20 -g3 -O2 -flto -Werror -o build/klblog/systemsettings.o src/klblog/systemsettings.cpp -Isrc -Isrc/kl -Isrc/klblog
build/tests/klfs-lsrec.o: src/tests/klfs-lsrec.cpp src/ff/textscanner.h src/kl/kltext.h src/kl/klfs.h src/kl/klexcept.h src/kl/kl.h src/kl/klds.h src/kl/kltime.h
	g++ -Wall -Wextra -c -std=c++20 -g3 -O2 -flto -Werror -o build/tests/klfs-lsrec.o src/tests/klfs-lsrec.cpp -Isrc -Isrc/ff -Isrc/kl -Isrc/tests
build/tests/klio.o: src/tests/klio.cpp src/kl/kltext.h src/kl/klexcept.h src/kl/klio.h src/kl/kl.h src/kl/klds.h
	g++ -Wall -Wextra -c -std=c++20 -g3 -O2 -flto -Werror -o build/tests/klio.o src/tests/klio.cpp -Isrc -Isrc/kl -Isrc/tests
build/tests/klnet.o: src/tests/klnet.cpp src/ff/textscanner.h src/kl/kltext.h src/kl/klexcept.h src/kl/klio.h src/kl/kl.h src/kl/klds.h src/kl/klnet.h src/kl/kltime.h
	g++ -Wall -Wextra -c -std=c++20 -g3 -O2 -flto -Werror -o build/tests/klnet.o src/tests/klnet.cpp -Isrc -Isrc/ff -Isrc/kl -Isrc/tests
build/tests/process-test.o: src/tests/process-test.cpp src/ff/textscanner.h src/kl/kltext.h src/kl/klfs.h src/kl/klexcept.h src/kl/kl.h src/kl/klds.h src/kl/kltime.h src/kl/klprocess.h
	g++ -Wall -Wextra -c -std=c++20 -g3 -O2 -flto -Werror -o build/tests/process-test.o src/tests/process-test.cpp -Isrc -Isrc/ff -Isrc/kl -Isrc/tests
build/ff/mw/markwrite.o: src/ff/mw/markwrite.cpp src/ff/mw/markwrite.h src/ff/poorconfig.h src/ff/textscanner.h src/kl/kl.h src/kl/klds.h src/kl/klexcept.h src/kl/kltext.h src/kl/kltime.h src/kl/klvalue.h
	g++ -Wall -Wextra -c -std=c++20 -g3 -O2 -flto -Werror -o build/ff/mw/markwrite.o src/ff/mw/markwrite.cpp -Isrc -Isrc/ff -Isrc/ff/mw -Isrc/kl
build/tests/file-formats/test-markwrite.o: src/tests/file-formats/test-markwrite.cpp src/ff/textscanner.h src/ff/poorconfig.h src/kl/kltext.h src/kl/klfs.h src/kl/klexcept.h src/kl/kl.h src/kl/klds.h src/kl/kltime.h src/kl/klvalue.h src/ff/mw/markwrite.h
	g++ -Wall -Wextra -c -std=c++20 -g3 -O2 -flto -Werror -o build/tests/file-formats/test-markwrite.o src/tests/file-formats/test-markwrite.cpp -Isrc -Isrc/ff -Isrc/ff/mw -Isrc/kl -Isrc/tests/file-formats
build/tests/file-formats/test-scanner.o: src/tests/file-formats/test-scanner.cpp src/ff/textscanner.h src/kl/kltext.h src/kl/kl.h src/kl/klds.h
	g++ -Wall -Wextra -c -std=c++20 -g3 -O2 -flto -Werror -o build/tests/file-formats/test-scanner.o src/tests/file-formats/test-scanner.cpp -Isrc -Isrc/ff -Isrc/kl -Isrc/tests/file-formats
build/tests/file-formats/test-poorconfig.o: src/tests/file-formats/test-poorconfig.cpp src/ff/textscanner.h src/ff/poorconfig.h src/kl/kltext.h src/kl/klfs.h src/kl/klexcept.h src/kl/kl.h src/kl/klds.h src/kl/kltime.h src/kl/klvalue.h
	g++ -Wall -Wextra -c -std=c++20 -g3 -O2 -flto -Werror -o build/tests/file-formats/test-poorconfig.o src/tests/file-formats/test-poorconfig.cpp -Isrc -Isrc/ff -Isrc/kl -Isrc/tests/file-formats
build/tests/kl/kldate-test.o: src/tests/kl/kldate-test.cpp src/kl/kl.h src/kl/klds.h src/kl/klexcept.h src/kl/kltext.h src/kl/kltime.h
	g++ -Wall -Wextra -c -std=c++20 -g3 -O2 -flto -Werror -o build/tests/kl/kldate-test.o src/tests/kl/kldate-test.cpp -Isrc -Isrc/kl -DGTEST_HAS_PTHREAD=1
build/tests/kl/klfs-test.o: src/tests/kl/klfs-test.cpp src/kl/kl.h src/kl/klds.h src/kl/klexcept.h src/kl/klfs.h src/kl/kltext.h src/kl/kltime.h
	g++ -Wall -Wextra -c -std=c++20 -g3 -O2 -flto -Werror -o build/tests/kl/klfs-test.o src/tests/kl/klfs-test.cpp -Isrc -Isrc/kl -DGTEST_HAS_PTHREAD=1
build/tests/kl/klvalue-test.o: src/tests/kl/klvalue-test.cpp src/kl/kl.h src/kl/klds.h src/kl/kltext.h src/kl/klvalue.h
	g++ -Wall -Wextra -c -std=c++20 -g3 -O2 -flto -Werror -o build/tests/kl/klvalue-test.o src/tests/kl/klvalue-test.cpp -Isrc -Isrc/kl -DGTEST_HAS_PTHREAD=1
build/tests/kl/kltest.o: src/tests/kl/kltest.cpp src/ff/textscanner.h src/kl/kltext.h src/kl/klfs.h src/kl/klexcept.h src/kl/kl.h src/kl/klds.h src/kl/kltime.h src/kl/klvalue.h src/tests/kl/kldate-test.h src/tests/kl/klfs-test.h src/tests/kl/klvalue-test.h src/tests/kl/kltext-test.h
	g++ -Wall -Wextra -c -std=c++20 -g3 -O2 -flto -Werror -o build/tests/kl/kltest.o src/tests/kl/kltest.cpp -Isrc -Isrc/ff -Isrc/kl -Isrc/tests/kl -DGTEST_HAS_PTHREAD=1
build/tests/kl/kltext-test.o: src/tests/kl/kltext-test.cpp src/kl/kl.h src/kl/klds.h src/kl/kltext.h
	g++ -Wall -Wextra -c -std=c++20 -g3 -O2 -flto -Werror -o build/tests/kl/kltext-test.o src/tests/kl/kltext-test.cpp -Isrc -Isrc/kl -DGTEST_HAS_PTHREAD=1
build/genmakefile/klb-genmakefile.exe: build/ff/codescanner.o build/ff/poorconfig.o build/ff/textscanner.o build/genmakefile/klb-genmakefile.o build/kl/klexcept.o build/kl/klfs.o build/kl/klprocess.o build/kl/kltext.o build/kl/kltime.o build/kl/klvalue.o build/klb/klbexecutor.o build/klb/klbfs.o build/klb/klbsettings.o build/klb/klbtoolchain.o build/klb/module.o build/klb/modulecollection.o build/klb/moduleitem.o build/klb/systemflags.o
	g++ -flto=auto -Werror -ltcmalloc -o build/genmakefile/klb-genmakefile.exe build/ff/codescanner.o build/ff/poorconfig.o build/ff/textscanner.o build/genmakefile/klb-genmakefile.o build/kl/klexcept.o build/kl/klfs.o build/kl/klprocess.o build/kl/kltext.o build/kl/kltime.o build/kl/klvalue.o build/klb/klbexecutor.o build/klb/klbfs.o build/klb/klbsettings.o build/klb/klbtoolchain.o build/klb/module.o build/klb/modulecollection.o build/klb/moduleitem.o build/klb/systemflags.o -lfmt
build/klb/klb.exe: build/ff/codescanner.o build/ff/poorconfig.o build/ff/textscanner.o build/kl/klexcept.o build/kl/klfs.o build/kl/klprocess.o build/kl/kltext.o build/kl/kltime.o build/kl/klvalue.o build/klb/klb.o build/klb/klbexecutor.o build/klb/klbfs.o build/klb/klbsettings.o build/klb/klbtoolchain.o build/klb/module.o build/klb/modulecollection.o build/klb/moduleitem.o build/klb/systemflags.o
	g++ -flto=auto -Werror -ltcmalloc -o build/klb/klb.exe build/ff/codescanner.o build/ff/poorconfig.o build/ff/textscanner.o build/kl/klexcept.o build/kl/klfs.o build/kl/klprocess.o build/kl/kltext.o build/kl/kltime.o build/kl/klvalue.o build/klb/klb.o build/klb/klbexecutor.o build/klb/klbfs.o build/klb/klbsettings.o build/klb/klbtoolchain.o build/klb/module.o build/klb/modulecollection.o build/klb/moduleitem.o build/klb/systemflags.o -lfmt
build/klblog/klblog.exe: build/ff/poorconfig.o build/ff/textscanner.o build/kl/klexcept.o build/kl/klfs.o build/kl/kltext.o build/kl/kltime.o build/kl/klvalue.o build/klblog/klblog.o build/klblog/systemsettings.o
	g++ -flto=auto -Werror -ltcmalloc -o build/klblog/klblog.exe build/ff/poorconfig.o build/ff/textscanner.o build/kl/klexcept.o build/kl/klfs.o build/kl/kltext.o build/kl/kltime.o build/kl/klvalue.o build/klblog/klblog.o build/klblog/systemsettings.o -lfmt
build/tests/klfs-lsrec.exe: build/ff/textscanner.o build/kl/klexcept.o build/kl/klfs.o build/kl/kltext.o build/kl/kltime.o build/tests/klfs-lsrec.o
	g++ -flto=auto -Werror -ltcmalloc -o build/tests/klfs-lsrec.exe build/ff/textscanner.o build/kl/klexcept.o build/kl/klfs.o build/kl/kltext.o build/kl/kltime.o build/tests/klfs-lsrec.o -lfmt
build/tests/klio.exe: build/kl/klexcept.o build/kl/klio.o build/kl/kltext.o build/tests/klio.o
	g++ -flto=auto -Werror -ltcmalloc -o build/tests/klio.exe build/kl/klexcept.o build/kl/klio.o build/kl/kltext.o build/tests/klio.o -lfmt
build/tests/klnet.exe: build/ff/textscanner.o build/kl/klexcept.o build/kl/klio.o build/kl/klnet.o build/kl/kltext.o build/kl/kltime.o build/tests/klnet.o
	g++ -flto=auto -Werror -ltcmalloc -o build/tests/klnet.exe build/ff/textscanner.o build/kl/klexcept.o build/kl/klio.o build/kl/klnet.o build/kl/kltext.o build/kl/kltime.o build/tests/klnet.o -lfmt -lssl -lcrypto
build/tests/process-test.exe: build/ff/textscanner.o build/kl/klexcept.o build/kl/klfs.o build/kl/klprocess.o build/kl/kltext.o build/kl/kltime.o build/tests/process-test.o
	g++ -flto=auto -Werror -ltcmalloc -o build/tests/process-test.exe build/ff/textscanner.o build/kl/klexcept.o build/kl/klfs.o build/kl/klprocess.o build/kl/kltext.o build/kl/kltime.o build/tests/process-test.o -lfmt
build/tests/file-formats/test-markwrite.exe: build/ff/mw/markwrite.o build/ff/poorconfig.o build/ff/textscanner.o build/kl/klexcept.o build/kl/klfs.o build/kl/kltext.o build/kl/kltime.o build/kl/klvalue.o build/tests/file-formats/test-markwrite.o
	g++ -flto=auto -Werror -ltcmalloc -o build/tests/file-formats/test-markwrite.exe build/ff/mw/markwrite.o build/ff/poorconfig.o build/ff/textscanner.o build/kl/klexcept.o build/kl/klfs.o build/kl/kltext.o build/kl/kltime.o build/kl/klvalue.o build/tests/file-formats/test-markwrite.o -lfmt
build/tests/file-formats/test-scanner.exe: build/ff/textscanner.o build/kl/kltext.o build/tests/file-formats/test-scanner.o
	g++ -flto=auto -Werror -ltcmalloc -o build/tests/file-formats/test-scanner.exe build/ff/textscanner.o build/kl/kltext.o build/tests/file-formats/test-scanner.o -lfmt
build/tests/file-formats/test-poorconfig.exe: build/ff/poorconfig.o build/ff/textscanner.o build/kl/klexcept.o build/kl/klfs.o build/kl/kltext.o build/kl/kltime.o build/kl/klvalue.o build/tests/file-formats/test-poorconfig.o
	g++ -flto=auto -Werror -ltcmalloc -o build/tests/file-formats/test-poorconfig.exe build/ff/poorconfig.o build/ff/textscanner.o build/kl/klexcept.o build/kl/klfs.o build/kl/kltext.o build/kl/kltime.o build/kl/klvalue.o build/tests/file-formats/test-poorconfig.o -lfmt
build/tests/kl/kltest.exe: build/ff/textscanner.o build/kl/klexcept.o build/kl/klfs.o build/kl/kltext.o build/kl/kltime.o build/kl/klvalue.o build/tests/kl/kldate-test.o build/tests/kl/klfs-test.o build/tests/kl/kltest.o build/tests/kl/kltext-test.o build/tests/kl/klvalue-test.o
	g++ -flto=auto -Werror -ltcmalloc -o build/tests/kl/kltest.exe build/ff/textscanner.o build/kl/klexcept.o build/kl/klfs.o build/kl/kltext.o build/kl/kltime.o build/kl/klvalue.o build/tests/kl/kldate-test.o build/tests/kl/klfs-test.o build/tests/kl/kltest.o build/tests/kl/kltext-test.o build/tests/kl/klvalue-test.o -lgtest -lfmt
executables: build/genmakefile/klb-genmakefile.exe build/klb/klb.exe build/klblog/klblog.exe build/tests/klfs-lsrec.exe build/tests/klio.exe build/tests/klnet.exe build/tests/process-test.exe build/tests/file-formats/test-markwrite.exe build/tests/file-formats/test-scanner.exe build/tests/file-formats/test-poorconfig.exe build/tests/kl/kltest.exe
makedirs:
	mkdir -p build/ff build/ff/mw build/genmakefile build/kl build/klb build/klblog build/tests build/tests/file-formats build/tests/kl
