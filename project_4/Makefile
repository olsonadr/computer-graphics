# Variables
SRCDIR = .
SRCFILE = proj.cpp
OUTDIR = .
OUTFILE = proj
CCFLAGS = -Linclude -Iinclude -lm

# Add windows compiler options and run command
ifeq ($(OS),Windows_NT)
	SRCDIR := $(SRCDIR:/=\\)\\
	OUTDIR := $(OUTDIR:/=\\)\\
	OUTFILE = $(OUTFILE).exe
	RUNCMD = $(OUTDIR)$(OUTFILE)
    CCFLAGS += -lopengl32 -lGLU32 -lfreeglut -lglew32
# Add linux compiler options and run command
else
	SRCDIR := $(SRCDIR)/
	OUTDIR := $(OUTDIR)/
	RUNCMD = $(OUTDIR)$(OUTFILE)
	CCFLAGS += -lGL -lGLU -lglut /usr/lib64/libglut.so
endif

build: proj.cpp
	g++ -o $(OUTDIR)$(OUTFILE) $(SRCDIR)$(SRCFILE) $(CCFLAGS)

run: build
	$(RUNCMD)

clean:
	rm $(OUTDIR)$(OUTFILE) -f