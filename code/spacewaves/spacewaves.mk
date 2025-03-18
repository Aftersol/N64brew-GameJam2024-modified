ASSETS_LIST += \
	filesystem/spacewaves/asteroid1.ihq.sprite \
	filesystem/spacewaves/explosion.rgba32.sprite \
	filesystem/spacewaves/galaxy1.rgba16.sprite \
	filesystem/spacewaves/galaxy2.rgba16.sprite \
	filesystem/spacewaves/galaxy3.rgba16.sprite \
	filesystem/spacewaves/planet1.i4.sprite \
	filesystem/spacewaves/planet1_clouds.i4.sprite \
	filesystem/spacewaves/planet1_rings.i4.sprite \
	filesystem/spacewaves/planet2_clouds.ia4.sprite \
	filesystem/spacewaves/planet2.i4.sprite \
	filesystem/spacewaves/spacecraft.ihq.sprite \
	filesystem/spacewaves/spacecraft_engine.rgba32.sprite \
	filesystem/spacewaves/stars1.i4.sprite \
	filesystem/spacewaves/station.ihq.sprite \
    filesystem/spacewaves/lensflare1.i8.sprite \
	filesystem/spacewaves/lensflare2.i8.sprite \
	filesystem/spacewaves/lensflare3.i8.sprite \
    filesystem/spacewaves/bullet.rgba32.sprite \
    filesystem/spacewaves/rocket.rgba16.sprite \
    filesystem/spacewaves/ui.bonus0.ia8.sprite \
    filesystem/spacewaves/ui.bonus1.rgba32.sprite \
    filesystem/spacewaves/ui.bonus2.rgba32.sprite \
    filesystem/spacewaves/ui.bonus3.rgba32.sprite \
    filesystem/spacewaves/ui.bonus4.rgba32.sprite \
    filesystem/spacewaves/ui.crosshair.ia4.sprite \
    filesystem/spacewaves/ui.crosshair2.ia4.sprite \
    filesystem/spacewaves/ui.target.ia8.sprite \
	filesystem/spacewaves/exp3d.rgba32.sprite \
    filesystem/spacewaves/space.t3dm \
    filesystem/spacewaves/planet.t3dm \
	filesystem/spacewaves/enemycraft.t3dm \
    filesystem/spacewaves/asteroid.t3dm \
    filesystem/spacewaves/rocket.t3dm \
	filesystem/spacewaves/machinegun_new.t3dm \
	filesystem/spacewaves/rocketgun.t3dm \
	filesystem/spacewaves/station.t3dm \
	filesystem/spacewaves/explosion3d.t3dm \
    filesystem/spacewaves/biotech.xm64 \
    filesystem/spacewaves/natural.xm64 \
	filesystem/spacewaves/chasingufo.xm64 \
    filesystem/spacewaves/pinkbats.xm64 \
	filesystem/spacewaves/JupiteroidBoldItalic.font64 \
	filesystem/spacewaves/Jupiteroid.font64 \
	filesystem/spacewaves/JupiteroidBoldItalic_half.font64 \
	filesystem/spacewaves/Jupiteroid_half.font64 \
	filesystem/spacewaves/button_click1.wav64 \
    filesystem/spacewaves/button_click2.wav64 \
	filesystem/spacewaves/button_click3.wav64 \
    filesystem/spacewaves/explosion-blast.wav64 \
    filesystem/spacewaves/hit.wav64 \
	filesystem/spacewaves/machinery.wav64 \
	filesystem/spacewaves/pickup_ammo.wav64 \
	filesystem/spacewaves/pickup_shield.wav64 \
	filesystem/spacewaves/reload.wav64 \
    filesystem/spacewaves/shoot.wav64 \
    filesystem/spacewaves/shoot_rocket.wav64 \
	filesystem/spacewaves/use_powerup.wav64 \
    filesystem/spacewaves/use_shield.wav64 \
    filesystem/spacewaves/machinegun_new_01.i4.sprite \
    filesystem/spacewaves/machinegun_new_02.i4.sprite \
    filesystem/spacewaves/machinegun_new_03.ci4.sprite \
    filesystem/spacewaves/machinegun_new_04.ci4.sprite \
    filesystem/spacewaves/intro.m1v \
	filesystem/spacewaves/expansionPak.rgba16.sprite \

# Generate both fonts at the same time form the same source.
# Usage of pattern rules here is a trick:
# https://stackoverflow.com/questions/2973445/gnu-makefile-rule-generating-a-few-targets-from-a-single-source-file
$(FILESYSTEM_DIR)/spacewaves/JupiteroidBoldItalic%font64 $(FILESYSTEM_DIR)/spacewaves/JupiteroidBoldItalic_half%font64: $(ASSETS_DIR)/spacewaves/JupiteroidBoldItalic.ttf
	@mkdir -p $(dir $@)
	@echo "    [SPACEWAVES FONT] $@"
	$(N64_MKFONT) --size 12 -o $(dir $@) "$<"
	mv "$(dir $@)/JupiteroidBoldItalic.font64" $(FILESYSTEM_DIR)/spacewaves/JupiteroidBoldItalic_half.font64
	@echo "    [SPACEWAVES FONT] $@"
	$(N64_MKFONT) --size 16 -o $(dir $@) "$<"
#mv "$(dir $@)/JupiteroidBoldItalic.font64" $(FILESYSTEM_DIR)/spacewaves/JupiteroidBoldItalic.font64

# Generate both fonts at the same time form the same source.
# Usage of pattern rules here is a trick:
# https://stackoverflow.com/questions/2973445/gnu-makefile-rule-generating-a-few-targets-from-a-single-source-file
$(FILESYSTEM_DIR)/spacewaves/Jupiteroid_half%font64 $(FILESYSTEM_DIR)/spacewaves/Jupiteroid%font64: $(ASSETS_DIR)/spacewaves/Jupiteroid.ttf
	@mkdir -p $(dir $@)
	@echo "    [SPACEWAVES FONT] $@"
	$(N64_MKFONT) --size 24 -o $(dir $@) "$<"
	mv "$(dir $@)/Jupiteroid.font64" $(FILESYSTEM_DIR)/spacewaves/Jupiteroid_half.font64
	@echo "    [SPACEWAVES FONT] $@"
	$(N64_MKFONT) --size 32 -o $(dir $@) "$<"
#mv "$(dir $@)/Jupiteroid.font64" $(FILESYSTEM_DIR)/spacewaves/Jupiteroid.font64


$(FILESYSTEM_DIR)/spacewaves/%.m1v: $(ASSETS_DIR)/spacewaves/%.m1v
	@mkdir -p $(dir $@)
	@echo "    [M1V] $@"
	cp "$<" $@

filesystem/spacewaves/machinegun_new_01.i4.sprite: MKSPRITE_FLAGS=--format I8
filesystem/spacewaves/machinegun_new_02.i4.sprite: MKSPRITE_FLAGS=--format I8

$(FILESYSTEM_DIR)/spacewaves/%.xm64: AUDIOCONV_FLAGS += --xm-8bit

$(FILESYSTEM_DIR)/spacewaves/%.wav64: AUDIOCONV_FLAGS += --wav-compress 1
