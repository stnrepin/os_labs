#
# Makefile
#

# Name of output document.
TARGET = doc

RM = /bin/rm
XELATEX = /bin/xelatex

# Output directory.
OUT_DIR = out

SOURCE_DOCS = main.tex

# Basic XeLaTex options.
XELATEX_OPTIONS = \
	-interaction=nonstopmode \
	-file-line-error \
	--output-directory=$(OUT_DIR) \

# Rule for Latex -> PDF.
#
$(TARGET):
	$(XELATEX) \
		$(XELATEX_OPTIONS) \
		$(SOURCE_DOCS) \

# Targets.
#
.PHONY: all clean

all: $(TARGET)

clean:
	$(RM) -r $(OUT_DIR)/*.*

