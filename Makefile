# Каталоги
IDIR = include
CDIR = src
ODIR = bin
BINDIR = $(ODIR)
UDIR = utils

# Утилитки
MERGEDEP = $(PERL) $(UDIR)/mergedep.pl
# Файл cflow.ignore содержит список функций,
# исключаемых из графов вызовов.
STYLECHECK = astyle --indent=force-tab=4

# Используемые общесистемные утилиты
CC := cc
PERL := perl
VALGRIND := valgrind

DEBUG = yes

# Флаги компиляции
CFLAGS := $(CFLAGS) $(DEFS) $(LABDEFS) -I$(IDIR) -I$(COMMONIDIR) -MD --std="gnu99"
ifdef DEBUG
CFLAGS += -O0 -DDEBUG -ggdb
else
CFLAGS += -O2
endif
CFLAGS += -Wall -Wno-format -Wno-unused -Werror
CFLAGS += -DWORKER_SET_JOIN_ON_EXIT

# Флаги сборки
LDFLAGS += $(shell autoopts-config ldflags)
LDFLAGS += -lresolv

TEST_RUNNER = $(BINDIR)/unittests_runner
PROG = $(BINDIR)/lookup_mx

# По-умолчанию и по make all собираем программу.
# Отдельно отчёт собирается как make report
.PHONY: all
all: $(PROG)

# make it so that no intermediate .o files are ever deleted
.PRECIOUS: %.o $(ODIR)/%.o

# This magic automatically generates makefile dependencies
# for header files included from C source files we compile,
# and keeps those dependencies up-to-date every time we recompile.
# See 'mergedep.pl' for more information.
$(ODIR)/.deps: $(wildcard $(ODIR)/*.d)
	@mkdir -p $(@D)
	@$(MERGEDEP) $@ $^

-include $(ODIR)/.deps

INCLUDES = $(wildcard $(IDIR)/*.h)
CSRC := $(wildcard $(CDIR)/*.c)
OBJS := $(patsubst $(CDIR)/%.c, $(ODIR)/%.o, $(CSRC))
TEST_OBJS := $(filter $(ODIR)/test%.o, $(OBJS))
OBJS := $(filter-out $(TEST_OBJS), $(OBJS))
LIB_OBJS := $(filter-out $(ODIR)/main.o, $(OBJS))
HANDCRAFTED := $(CSRC) $(INCLUDED)

# Файлы latex
TEXS = $(wildcard $(TEXDIR)/*.tex)

$(ODIR)/%.o: $(CDIR)/%.c
	@mkdir -p $(@D)
	$(CC) -c $(CFLAGS) -o $@ $<

$(PROG): $(OBJS)
	$(CC) -o $@ $(LDFLAGS) $^

# Тестирование
.PHONY: tests
tests: test_units test_memory test_style

$(TEST_RUNNER): $(TEST_OBJS) $(LIB_OBJS)
	$(CC) -o $@ $(LDFLAGS) -lcunit $^

.PHONY: test_units
test_units: $(TEST_RUNNER)
	$(TEST_RUNNER) 2>test.log

.PHONY: test_style
test_style: $(HANDCRAFTED)
	@for file in $^; do\
		$(STYLECHECK) < $$file | diff - $$file > /dev/null;\
		if [ $$? != 0 ]; then\
			echo "Style check failed on $$file";\
			exit 1;\
		fi;\
	done;

.PHONY: test_memory
test_memory: $(PROG) $(TEST_RUNNER)
	$(VALGRIND) $(TEST_RUNNER)

.PHONY: clean
clean:
	rm -rfv $(ODIR)/
