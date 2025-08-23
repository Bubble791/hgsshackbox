%.NCGR: %.png
	$(GFX) $< $@ $(GFX_FLAGS)

%.NCLR: %.png
	$(GFX) $< $@ $(GFX_FLAGS)

%.NSCR: %.bin
	$(GFX) $< $@ $(GFX_FLAGS)

NARC_FILE := 

hackBox.narc : $(NARC_FILE)
