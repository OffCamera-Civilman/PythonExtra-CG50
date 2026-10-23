GCC = nspire-gcc
LD = nspire-ld
GENZEHN = genzehn
all: NspireClock_CXII_v0_1.tns
main.o: main.c
	$(GCC) -Wall -W -marm -Os -c $< -o $@
NspireClock_CXII_v0_1.elf: main.o
	$(LD) $< -o $@ -Wl,--gc-sections
NspireClock_CXII_v0_1.tns: NspireClock_CXII_v0_1.elf
	$(GENZEHN) --input $< --output $@.zehn --name "NspireClock Safe Test"
	make-prg $@.zehn $@
	rm -f $@.zehn
clean:
	rm -f *.o *.elf *.tns *.zehn
