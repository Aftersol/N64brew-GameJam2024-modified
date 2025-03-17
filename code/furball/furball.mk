
ASSETS_LIST += \
	filesystem/furball/ball.t3dm \
	filesystem/furball/target.t3dm \
	filesystem/furball/Untitled.ci4.sprite \
	filesystem/furball/fastladder.t3dm \
	filesystem/furball/ladder.ci8.sprite \
	filesystem/furball/fastcat.t3dm \
	filesystem/furball/wall.t3dm \
	filesystem/furball/stonebricks.i8.sprite \
	filesystem/furball/crystal.xm64 \
	filesystem/furball/fire.wav64 \
	filesystem/furball/hit.wav64
filesystem/furball/fire.wav64: AUDIOCONV_FLAGS += --wav-mono --wav-resample 16000
filesystem/furball/hit.wav64: AUDIOCONV_FLAGS += --wav-mono --wav-resample 16000

filesystem/furball/crystal.xm64: AUDIOCONV_FLAGS += --xm-8bit