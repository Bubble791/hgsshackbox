%.NCGR: %.png
	$(GFX) $< $@ $(GFX_FLAGS)

%.NCLR: %.png
	$(GFX) $< $@ $(GFX_FLAGS)

NARC_FILE := graphic/bottom_bg.NCGR \
			graphic/top_bg.NCGR \
			graphic/top_bg.NCLR \

hackBox.narc : $(NARC_FILE)
