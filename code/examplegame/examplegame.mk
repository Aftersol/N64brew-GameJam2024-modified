ASSETS_LIST += \
	filesystem/examplegame/super_chicken.xm64 \
	filesystem/examplegame/mash.wav64 \

filesystem/examplegame/mash.wav64: AUDIOCONV_FLAGS += --wav-compress 1,3 --wav-mono --wav-resample 16000

filesystem/examplegame/super_chicken.xm64: AUDIOCONV_FLAGS += --xm-8bit