.PHONY: executables makedirs
all: makedirs executables
build/tests/klvalue.o: src/tests/klvalue.cpp src/kl/kltime.h src/kl/klfs.h src/kl/kl.h src/kl/klexcept.h src/kl/klds.h src/kl/kltext.h src/kl/klvalue.h src/ff/textscanner.h
	g++ -Wall -Wextra -c -std=c++20 -g3 -O2 -flto -Werror -o build/tests/klvalue.o src/tests/klvalue.cpp -Isrc -Isrc/tests -Isrc/ff -Isrc/kl
build/tests/file-formats/test-poorconfig.o: src/tests/file-formats/test-poorconfig.cpp src/kl/kltime.h src/kl/klfs.h src/kl/kl.h src/ff/textscanner.h src/kl/klvalue.h src/kl/klexcept.h src/kl/kltext.h src/kl/klds.h src/ff/poorconfig.h
	g++ -Wall -Wextra -c -std=c++20 -g3 -O2 -flto -Werror -o build/tests/file-formats/test-poorconfig.o src/tests/file-formats/test-poorconfig.cpp -Isrc -Isrc/ff -Isrc/tests/file-formats -Isrc/kl
build/klblog/klblog.o: src/klblog/klblog.cpp src/kl/kltime.h src/ff/textscanner.h src/kl/klvalue.h src/kl/klexcept.h src/kl/kltext.h src/kl/klds.h src/ff/poorconfig.h src/kl/klfs.h src/kl/kl.h src/klblog/settings.h
	g++ -Wall -Wextra -c -std=c++20 -g3 -O2 -flto -Werror -o build/klblog/klblog.o src/klblog/klblog.cpp -Isrc -Isrc/klblog -Isrc/ff -Isrc/kl
build/klblog/settings.o: src/klblog/settings.cpp src/klblog/settings.h src/kl/klds.h src/kl/kltext.h src/kl/kl.h
	g++ -Wall -Wextra -c -std=c++20 -g3 -O2 -flto -Werror -o build/klblog/settings.o src/klblog/settings.cpp -Isrc -Isrc/kl -Isrc/klblog
build/tests/file-formats/test-markwrite.o: src/tests/file-formats/test-markwrite.cpp src/kl/klfs.h src/kl/kl.h src/kl/kltime.h src/ff/poorconfig.h src/kl/klds.h src/kl/kltext.h src/kl/klexcept.h src/kl/klvalue.h src/ff/textscanner.h src/ff/mw/markwrite.h
	g++ -Wall -Wextra -c -std=c++20 -g3 -O2 -flto -Werror -o build/tests/file-formats/test-markwrite.o src/tests/file-formats/test-markwrite.cpp -Isrc -Isrc/ff -Isrc/tests/file-formats -Isrc/ff/mw -Isrc/kl
build/tests/kldate.o: src/tests/kldate.cpp src/kl/kl.h src/kl/kltime.h src/kl/kltext.h src/kl/klexcept.h src/kl/klds.h src/tests/timecounter.h src/ff/textscanner.h
	g++ -Wall -Wextra -c -std=c++20 -g3 -O2 -flto -Werror -o build/tests/kldate.o src/tests/kldate.cpp -Isrc -Isrc/ff -Isrc/tests -Isrc/kl
build/ff/mw/markwrite.o: src/ff/mw/markwrite.cpp src/ff/mw/markwrite.h src/kl/kltime.h src/kl/klexcept.h src/ff/poorconfig.h src/kl/klvalue.h src/ff/textscanner.h src/kl/kl.h src/kl/klds.h src/kl/kltext.h
	g++ -Wall -Wextra -c -std=c++20 -g3 -O2 -flto -Werror -o build/ff/mw/markwrite.o src/ff/mw/markwrite.cpp -Isrc -Isrc/ff -Isrc/kl -Isrc/ff/mw
build/kl/klnet.o: src/kl/klnet.cpp src/kl/klnet.h src/kl/kltime.h src/kl/klio.h src/kl/klexcept.h src/kl/kltext.h src/kl/klds.h src/kl/kl.h
	g++ -Wall -Wextra -c -std=c++20 -g3 -O2 -flto -Werror -o build/kl/klnet.o src/kl/klnet.cpp -Isrc -Isrc/kl
build/ff/textscanner.o: src/ff/textscanner.cpp src/ff/textscanner.h src/kl/kltext.h src/kl/klds.h src/kl/kl.h
	g++ -Wall -Wextra -c -std=c++20 -g3 -O2 -flto -Werror -o build/ff/textscanner.o src/ff/textscanner.cpp -Isrc -Isrc/kl -Isrc/ff
build/kl/kltime.o: src/kl/kltime.cpp src/kl/kltime.h src/kl/klexcept.h src/ff/textscanner.h src/kl/kltext.h src/kl/klds.h src/kl/kl.h
	g++ -Wall -Wextra -c -std=c++20 -g3 -O2 -flto -Werror -o build/kl/kltime.o src/kl/kltime.cpp -Isrc -Isrc/ff -Isrc/kl
build/kl/klprocess.o: src/kl/klprocess.cpp src/kl/klfs.h src/kl/kl.h src/kl/klprocess.h src/kl/kltime.h src/kl/klds.h src/kl/kltext.h src/kl/klexcept.h
	g++ -Wall -Wextra -c -std=c++20 -g3 -O2 -flto -Werror -o build/kl/klprocess.o src/kl/klprocess.cpp -Isrc -Isrc/kl
build/kl/klfs.o: src/kl/klfs.cpp src/kl/klfs.h src/kl/kl.h src/kl/kltime.h src/kl/klds.h src/kl/kltext.h src/kl/klexcept.h
	g++ -Wall -Wextra -c -std=c++20 -g3 -O2 -flto -Werror -o build/kl/klfs.o src/kl/klfs.cpp -Isrc -Isrc/kl
build/tests/klfs.o: src/tests/klfs.cpp src/ff/textscanner.h src/kl/klexcept.h src/kl/kltext.h src/kl/klds.h src/kl/kltime.h src/kl/kl.h src/kl/klfs.h
	g++ -Wall -Wextra -c -std=c++20 -g3 -O2 -flto -Werror -o build/tests/klfs.o src/tests/klfs.cpp -Isrc -Isrc/tests -Isrc/kl -Isrc/ff
build/klb/klbfs.o: src/klb/klbfs.cpp src/klb/klbsettings.h src/klb/systemflags.h src/kl/kltext.h src/kl/klds.h src/kl/kl.h src/kl/klfs.h src/kl/klvalue.h src/kl/klexcept.h src/klb/klbfs.h src/kl/kltime.h
	g++ -Wall -Wextra -c -std=c++20 -g3 -O2 -flto -Werror -o build/klb/klbfs.o src/klb/klbfs.cpp -Isrc -Isrc/kl -Isrc/klb
build/tests/file-formats/test-scanner.o: src/tests/file-formats/test-scanner.cpp src/kl/kl.h src/kl/klds.h src/kl/kltext.h src/ff/textscanner.h
	g++ -Wall -Wextra -c -std=c++20 -g3 -O2 -flto -Werror -o build/tests/file-formats/test-scanner.o src/tests/file-formats/test-scanner.cpp -Isrc -Isrc/ff -Isrc/tests/file-formats -Isrc/kl
build/tests/kltext.o: src/tests/kltext.cpp src/kl/kl.h src/kl/klds.h src/kl/kltext.h
	g++ -Wall -Wextra -c -std=c++20 -g3 -O2 -flto -Werror -o build/tests/kltext.o src/tests/kltext.cpp -Isrc -Isrc/tests -Isrc/kl
build/kl/klvalue.o: src/kl/klvalue.cpp src/kl/klfs.h src/kl/kl.h src/kl/kltime.h src/kl/klds.h src/kl/kltext.h src/kl/klvalue.h src/kl/klexcept.h
	g++ -Wall -Wextra -c -std=c++20 -g3 -O2 -flto -Werror -o build/kl/klvalue.o src/kl/klvalue.cpp -Isrc -Isrc/kl
build/kl/klio.o: src/kl/klio.cpp src/kl/klio.h src/kl/klexcept.h src/kl/kltext.h src/kl/klds.h src/kl/kl.h
	g++ -Wall -Wextra -c -std=c++20 -g3 -O2 -flto -Werror -o build/kl/klio.o src/kl/klio.cpp -Isrc -Isrc/kl
build/tests/process-test.o: src/tests/process-test.cpp src/ff/textscanner.h src/kl/kltime.h src/kl/klfs.h src/kl/kl.h src/kl/klprocess.h src/kl/klexcept.h src/kl/kltext.h src/kl/klds.h
	g++ -Wall -Wextra -c -std=c++20 -g3 -O2 -flto -Werror -o build/tests/process-test.o src/tests/process-test.cpp -Isrc -Isrc/tests -Isrc/kl -Isrc/ff
build/kl/klexcept.o: src/kl/klexcept.cpp src/kl/klexcept.h src/kl/kltext.h src/kl/klds.h src/kl/kl.h
	g++ -Wall -Wextra -c -std=c++20 -g3 -O2 -flto -Werror -o build/kl/klexcept.o src/kl/klexcept.cpp -Isrc -Isrc/kl
build/tests/klnet.o: src/tests/klnet.cpp src/kl/klio.h src/kl/kl.h src/kl/klexcept.h src/kl/kltext.h src/kl/klds.h src/kl/kltime.h src/kl/klnet.h src/ff/textscanner.h
	g++ -Wall -Wextra -c -std=c++20 -g3 -O2 -flto -Werror -o build/tests/klnet.o src/tests/klnet.cpp -Isrc -Isrc/tests -Isrc/ff -Isrc/kl
build/ff/codescanner.o: src/ff/codescanner.cpp src/ff/textscanner.h src/kl/klfs.h src/kl/kl.h src/kl/kltime.h src/ff/codescanner.h src/kl/klds.h src/kl/kltext.h src/kl/klexcept.h
	g++ -Wall -Wextra -c -std=c++20 -g3 -O2 -flto -Werror -o build/ff/codescanner.o src/ff/codescanner.cpp -Isrc -Isrc/kl -Isrc/ff
build/kl/kltext.o: src/kl/kltext.cpp src/kl/kltext.h src/kl/klds.h src/kl/kl.h
	g++ -Wall -Wextra -c -std=c++20 -g3 -O2 -flto -Werror -o build/kl/kltext.o src/kl/kltext.cpp -Isrc -Isrc/kl
build/tests/klfs-lsrec.o: src/tests/klfs-lsrec.cpp src/ff/textscanner.h src/kl/klexcept.h src/kl/kltext.h src/kl/klds.h src/kl/kltime.h src/kl/kl.h src/kl/klfs.h
	g++ -Wall -Wextra -c -std=c++20 -g3 -O2 -flto -Werror -o build/tests/klfs-lsrec.o src/tests/klfs-lsrec.cpp -Isrc -Isrc/tests -Isrc/kl -Isrc/ff
build/kl/klds.o: src/kl/klds.cpp 
	g++ -Wall -Wextra -c -std=c++20 -g3 -O2 -flto -Werror -o build/kl/klds.o src/kl/klds.cpp -Isrc
build/klb/systemflags.o: src/klb/systemflags.cpp src/klb/klbsettings.h src/klb/systemflags.h src/kl/kltext.h src/kl/klds.h src/kl/kl.h src/kl/klfs.h src/kl/klvalue.h src/kl/klexcept.h src/kl/kltime.h
	g++ -Wall -Wextra -c -std=c++20 -g3 -O2 -flto -Werror -o build/klb/systemflags.o src/klb/systemflags.cpp -Isrc -Isrc/kl -Isrc/klb
build/klb/klb.o: src/klb/klb.cpp src/klb/klbtoolchain.h src/ff/textscanner.h src/ff/poorconfig.h src/klb/klbexecutor.h src/klb/moduleitem.h src/kl/kltime.h src/klb/klbsettings.h src/klb/module.h src/klb/systemflags.h src/kl/klds.h src/kl/kltext.h src/ff/codescanner.h src/kl/klexcept.h src/klb/modulecollection.h src/kl/klprocess.h src/klb/klbfs.h src/kl/klfs.h src/kl/kl.h src/kl/klvalue.h
	g++ -Wall -Wextra -c -std=c++20 -g3 -O2 -flto -Werror -o build/klb/klb.o src/klb/klb.cpp -Isrc -Isrc/kl -Isrc/ff -Isrc/klb
build/tests/timecounter.o: src/tests/timecounter.cpp src/tests/timecounter.h src/kl/klds.h src/kl/kltime.h src/kl/klexcept.h src/kl/kltext.h src/kl/kl.h
	g++ -Wall -Wextra -c -std=c++20 -g3 -O2 -flto -Werror -o build/tests/timecounter.o src/tests/timecounter.cpp -Isrc -Isrc/kl -Isrc/tests
build/kl/kl.o: src/kl/kl.cpp 
	g++ -Wall -Wextra -c -std=c++20 -g3 -O2 -flto -Werror -o build/kl/kl.o src/kl/kl.cpp -Isrc
build/tests/testklio.o: src/tests/testklio.cpp src/kl/kl.h src/kl/klds.h src/kl/kltext.h src/kl/klexcept.h src/kl/klio.h
	g++ -Wall -Wextra -c -std=c++20 -g3 -O2 -flto -Werror -o build/tests/testklio.o src/tests/testklio.cpp -Isrc -Isrc/tests -Isrc/kl
build/klb/klbexecutor.o: src/klb/klbexecutor.cpp src/klb/module.h src/kl/klprocess.h src/klb/klbfs.h src/kl/klexcept.h src/klb/klbexecutor.h src/kl/klvalue.h src/kl/klfs.h src/kl/klds.h src/klb/modulecollection.h src/kl/kltime.h src/kl/kl.h src/klb/moduleitem.h src/kl/kltext.h src/klb/systemflags.h src/klb/klbsettings.h src/klb/klbtoolchain.h
	g++ -Wall -Wextra -c -std=c++20 -g3 -O2 -flto -Werror -o build/klb/klbexecutor.o src/klb/klbexecutor.cpp -Isrc -Isrc/kl -Isrc/klb
build/klb/klbtoolchain.o: src/klb/klbtoolchain.cpp src/klb/klbtoolchain.h src/klb/klbsettings.h src/klb/systemflags.h src/kl/klvalue.h src/kl/klexcept.h src/kl/kltime.h src/kl/klprocess.h src/kl/klds.h src/kl/kltext.h src/kl/klfs.h src/kl/kl.h
	g++ -Wall -Wextra -c -std=c++20 -g3 -O2 -flto -Werror -o build/klb/klbtoolchain.o src/klb/klbtoolchain.cpp -Isrc -Isrc/kl -Isrc/klb
build/ff/poorconfig.o: src/ff/poorconfig.cpp src/ff/textscanner.h src/kl/klfs.h src/kl/kl.h src/kl/kltime.h src/kl/klds.h src/kl/kltext.h src/ff/poorconfig.h src/kl/klvalue.h src/kl/klexcept.h
	g++ -Wall -Wextra -c -std=c++20 -g3 -O2 -flto -Werror -o build/ff/poorconfig.o src/ff/poorconfig.cpp -Isrc -Isrc/kl -Isrc/ff
build/genmakefile/klb-genmakefile.o: src/genmakefile/klb-genmakefile.cpp src/klb/klbtoolchain.h src/ff/textscanner.h src/ff/poorconfig.h src/klb/klbexecutor.h src/klb/klbsettings.h src/klb/module.h src/klb/systemflags.h src/kl/klds.h src/kl/kltext.h src/ff/codescanner.h src/kl/klexcept.h src/klb/modulecollection.h src/kl/klprocess.h src/klb/klbfs.h src/kl/klfs.h src/kl/kl.h src/klb/moduleitem.h src/kl/kltime.h src/kl/klvalue.h
	g++ -Wall -Wextra -c -std=c++20 -g3 -O2 -flto -Werror -o build/genmakefile/klb-genmakefile.o src/genmakefile/klb-genmakefile.cpp -Isrc -Isrc/kl -Isrc/genmakefile -Isrc/ff -Isrc/klb
build/klb/klbsettings.o: src/klb/klbsettings.cpp src/klb/klbsettings.h src/klb/systemflags.h src/kl/kltime.h src/kl/klexcept.h src/ff/poorconfig.h src/kl/klvalue.h src/ff/textscanner.h src/kl/klfs.h src/kl/kl.h src/kl/klds.h src/kl/kltext.h
	g++ -Wall -Wextra -c -std=c++20 -g3 -O2 -flto -Werror -o build/klb/klbsettings.o src/klb/klbsettings.cpp -Isrc -Isrc/kl -Isrc/ff -Isrc/klb
build/klb/module.o: src/klb/module.cpp src/kl/klfs.h src/kl/kltext.h src/kl/klds.h src/ff/codescanner.h src/klb/modulecollection.h src/kl/kl.h src/klb/moduleitem.h src/kl/kltime.h src/klb/klbfs.h src/kl/klexcept.h src/kl/klvalue.h src/klb/systemflags.h src/klb/klbsettings.h src/klb/module.h
	g++ -Wall -Wextra -c -std=c++20 -g3 -O2 -flto -Werror -o build/klb/module.o src/klb/module.cpp -Isrc -Isrc/klb -Isrc/ff -Isrc/kl
build/klb/modulecollection.o: src/klb/modulecollection.cpp src/kl/klfs.h src/kl/kltext.h src/kl/klds.h src/ff/codescanner.h src/klb/modulecollection.h src/kl/kl.h src/klb/moduleitem.h src/kl/kltime.h src/klb/klbfs.h src/kl/klexcept.h src/kl/klvalue.h src/klb/systemflags.h src/klb/klbsettings.h src/klb/module.h
	g++ -Wall -Wextra -c -std=c++20 -g3 -O2 -flto -Werror -o build/klb/modulecollection.o src/klb/modulecollection.cpp -Isrc -Isrc/klb -Isrc/ff -Isrc/kl
build/klb/moduleitem.o: src/klb/moduleitem.cpp src/klb/klbsettings.h src/klb/systemflags.h src/kl/kltext.h src/klb/moduleitem.h src/kl/klds.h src/kl/kl.h src/kl/klfs.h src/kl/klvalue.h src/kl/klexcept.h src/kl/kltime.h
	g++ -Wall -Wextra -c -std=c++20 -g3 -O2 -flto -Werror -o build/klb/moduleitem.o src/klb/moduleitem.cpp -Isrc -Isrc/kl -Isrc/klb
build/tests/klvalue.exe: build/kl/klvalue.o build/ff/textscanner.o build/kl/kltext.o build/kl/klexcept.o build/kl/klfs.o build/kl/kltime.o build/tests/klvalue.o
	g++ -flto -Werror -ltcmalloc -o build/tests/klvalue.exe build/kl/klvalue.o build/ff/textscanner.o build/kl/kltext.o build/kl/klexcept.o build/kl/klfs.o build/kl/kltime.o build/tests/klvalue.o
build/tests/file-formats/test-poorconfig.exe: build/ff/poorconfig.o build/kl/klvalue.o build/kl/kltext.o build/ff/textscanner.o build/kl/klexcept.o build/kl/klfs.o build/kl/kltime.o build/tests/file-formats/test-poorconfig.o
	g++ -flto -Werror -ltcmalloc -o build/tests/file-formats/test-poorconfig.exe build/ff/poorconfig.o build/kl/klvalue.o build/kl/kltext.o build/ff/textscanner.o build/kl/klexcept.o build/kl/klfs.o build/kl/kltime.o build/tests/file-formats/test-poorconfig.o
build/klblog/klblog.exe: build/klblog/settings.o build/kl/klfs.o build/kl/klexcept.o build/ff/poorconfig.o build/kl/klvalue.o build/kl/kltext.o build/ff/textscanner.o build/kl/kltime.o build/klblog/klblog.o
	g++ -flto -Werror -ltcmalloc -o build/klblog/klblog.exe build/klblog/settings.o build/kl/klfs.o build/kl/klexcept.o build/ff/poorconfig.o build/kl/klvalue.o build/kl/kltext.o build/ff/textscanner.o build/kl/kltime.o build/klblog/klblog.o
build/tests/file-formats/test-markwrite.exe: build/ff/mw/markwrite.o build/ff/textscanner.o build/kl/kltext.o build/kl/klvalue.o build/ff/poorconfig.o build/kl/kltime.o build/kl/klexcept.o build/kl/klfs.o build/tests/file-formats/test-markwrite.o
	g++ -flto -Werror -ltcmalloc -o build/tests/file-formats/test-markwrite.exe build/ff/mw/markwrite.o build/ff/textscanner.o build/kl/kltext.o build/kl/klvalue.o build/ff/poorconfig.o build/kl/kltime.o build/kl/klexcept.o build/kl/klfs.o build/tests/file-formats/test-markwrite.o
build/tests/kldate.exe: build/tests/timecounter.o build/kl/klexcept.o build/ff/textscanner.o build/kl/kltext.o build/kl/kltime.o build/tests/kldate.o
	g++ -flto -Werror -ltcmalloc -o build/tests/kldate.exe build/tests/timecounter.o build/kl/klexcept.o build/ff/textscanner.o build/kl/kltext.o build/kl/kltime.o build/tests/kldate.o
build/tests/klfs.exe: build/kl/kltime.o build/kl/klfs.o build/kl/klexcept.o build/kl/kltext.o build/ff/textscanner.o build/tests/klfs.o
	g++ -flto -Werror -ltcmalloc -o build/tests/klfs.exe build/kl/kltime.o build/kl/klfs.o build/kl/klexcept.o build/kl/kltext.o build/ff/textscanner.o build/tests/klfs.o
build/tests/file-formats/test-scanner.exe: build/ff/textscanner.o build/kl/kltext.o build/tests/file-formats/test-scanner.o
	g++ -flto -Werror -ltcmalloc -o build/tests/file-formats/test-scanner.exe build/ff/textscanner.o build/kl/kltext.o build/tests/file-formats/test-scanner.o
build/tests/kltext.exe: build/kl/kltext.o build/tests/kltext.o
	g++ -flto -Werror -ltcmalloc -o build/tests/kltext.exe build/kl/kltext.o build/tests/kltext.o
build/tests/process-test.exe: build/kl/klexcept.o build/kl/klfs.o build/kl/kltime.o build/kl/kltext.o build/ff/textscanner.o build/kl/klprocess.o build/tests/process-test.o
	g++ -flto -Werror -ltcmalloc -o build/tests/process-test.exe build/kl/klexcept.o build/kl/klfs.o build/kl/kltime.o build/kl/kltext.o build/ff/textscanner.o build/kl/klprocess.o build/tests/process-test.o
build/tests/klnet.exe: build/kl/klnet.o build/kl/kltime.o build/ff/textscanner.o build/kl/kltext.o build/kl/klexcept.o build/kl/klio.o build/tests/klnet.o
	g++ -flto -Werror -ltcmalloc -o build/tests/klnet.exe build/kl/klnet.o build/kl/kltime.o build/ff/textscanner.o build/kl/kltext.o build/kl/klexcept.o build/kl/klio.o build/tests/klnet.o -lssl -lcrypto
build/tests/klfs-lsrec.exe: build/kl/klfs.o build/kl/klexcept.o build/kl/kltext.o build/ff/textscanner.o build/kl/kltime.o build/tests/klfs-lsrec.o
	g++ -flto -Werror -ltcmalloc -o build/tests/klfs-lsrec.exe build/kl/klfs.o build/kl/klexcept.o build/kl/kltext.o build/ff/textscanner.o build/kl/kltime.o build/tests/klfs-lsrec.o
build/klb/klb.exe: build/kl/klvalue.o build/klb/klbfs.o build/kl/klfs.o build/kl/klprocess.o build/klb/klbexecutor.o build/klb/modulecollection.o build/ff/codescanner.o build/klb/module.o build/klb/klb.o build/klb/klbsettings.o build/klb/klbtoolchain.o build/kl/kltext.o build/ff/poorconfig.o build/ff/textscanner.o build/klb/moduleitem.o build/kl/kltime.o build/klb/systemflags.o build/kl/klexcept.o
	g++ -flto -Werror -ltcmalloc -o build/klb/klb.exe build/kl/klvalue.o build/klb/klbfs.o build/kl/klfs.o build/kl/klprocess.o build/klb/klbexecutor.o build/klb/modulecollection.o build/ff/codescanner.o build/klb/module.o build/klb/klb.o build/klb/klbsettings.o build/klb/klbtoolchain.o build/kl/kltext.o build/ff/poorconfig.o build/ff/textscanner.o build/klb/moduleitem.o build/kl/kltime.o build/klb/systemflags.o build/kl/klexcept.o
build/tests/testklio.exe: build/kl/klexcept.o build/kl/kltext.o build/kl/klio.o build/tests/testklio.o
	g++ -flto -Werror -ltcmalloc -o build/tests/testklio.exe build/kl/klexcept.o build/kl/kltext.o build/kl/klio.o build/tests/testklio.o
build/genmakefile/klb-genmakefile.exe: build/kl/klvalue.o build/kl/kltime.o build/klb/klbfs.o build/genmakefile/klb-genmakefile.o build/klb/module.o build/klb/moduleitem.o build/ff/textscanner.o build/klb/klbsettings.o build/klb/klbtoolchain.o build/kl/kltext.o build/ff/poorconfig.o build/klb/klbexecutor.o build/klb/modulecollection.o build/ff/codescanner.o build/kl/klfs.o build/kl/klprocess.o build/klb/systemflags.o build/kl/klexcept.o
	g++ -flto -Werror -ltcmalloc -o build/genmakefile/klb-genmakefile.exe build/kl/klvalue.o build/kl/kltime.o build/klb/klbfs.o build/genmakefile/klb-genmakefile.o build/klb/module.o build/klb/moduleitem.o build/ff/textscanner.o build/klb/klbsettings.o build/klb/klbtoolchain.o build/kl/kltext.o build/ff/poorconfig.o build/klb/klbexecutor.o build/klb/modulecollection.o build/ff/codescanner.o build/kl/klfs.o build/kl/klprocess.o build/klb/systemflags.o build/kl/klexcept.o
executables: build/tests/klvalue.exe build/tests/file-formats/test-poorconfig.exe build/klblog/klblog.exe build/tests/file-formats/test-markwrite.exe build/tests/kldate.exe build/tests/klfs.exe build/tests/file-formats/test-scanner.exe build/tests/kltext.exe build/tests/process-test.exe build/tests/klnet.exe build/tests/klfs-lsrec.exe build/klb/klb.exe build/tests/testklio.exe build/genmakefile/klb-genmakefile.exe
makedirs:
	mkdir -p build/genmakefile build/klb build/kl build/ff build/ff/mw build/klblog build/tests/file-formats build/tests
