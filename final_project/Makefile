# Variables
SRCDIR = ./src
SRCFILES = proj.cpp glslprogram.cpp lightinghelpers.cpp loadobj.cpp osusphere.cpp Icosphere.cpp cubemap.cpp bmp.cpp
OUTDIR = .
OUTFILE = proj
CCFLAGS = -g -Linclude -Iinclude -I"include/ImageMagick-6" -L"include/ImageMagick-6" -lm
WIN_CC_FLAGS = -lopengl32 -lGLU32 -lfreeglut -lglew32
LINUX_CC_FLAGS = -lGL -lGLU -lglut -lGLEW /usr/lib64/libglut.so

# Helpers
SPACE := $(subst ,, )

# Add windows compiler options and run command
ifeq ($(OS),Windows_NT)
	SRCDIR := $(SRCDIR:/=\\)\\
	OUTDIR := $(OUTDIR:/=\\)\\
	SRCPATHS := $(SPACE)$(subst $(SPACE), $(SPACE)$(SRCDIR), $(SRCFILES))
	OUTFILE = $(OUTFILE).exe
	RUNCMD = $(OUTDIR)$(OUTFILE)
    CCFLAGS += $(WIN_CC_FLAGS)
# Add linux compiler options and run command
else
	SRCDIR := $(SRCDIR)/
	OUTDIR := $(OUTDIR)/
	SRCPATHS := $(SPACE)$(subst $(SPACE), $(SRCDIR), $(SRCFILES))
	RUNCMD = $(OUTDIR)$(OUTFILE)
	CCFLAGS += $(LINUX_CC_FLAGS)
endif

build: $(SRCPATHS)
	g++ -o $(OUTDIR)$(OUTFILE) $(CCFLAGS) $(SRCPATHS)

run: build
	$(RUNCMD)

clean:
	rm $(OUTDIR)$(OUTFILE) -f


#-Wwrite-strings