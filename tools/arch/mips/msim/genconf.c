
#include <stdio.h>
#include <stdlib.h>
#include <config.h>

int main(int argc, char *argv[])
{
	FILE *fp;
	char buf[BUFSIZ];
	int i;

	if (argc != 2) {
		fprintf(stderr, "Usage: %s config-filename\n", argv[0]);
		exit(1);
	}

	if ((fp = fopen(argv[1], "w")) == NULL) {
		perror(argv[1]);
		exit(1);
	}

	/* Add CPUs */
	fprintf(fp, "# CPUs\n");
	for (i = 0; i < NR_CPUS; ++i) {
		fprintf(fp, "add dcpu cpu%d\n", i);
	}
	fprintf(fp, "\n");

	/* Low 256M RAM */
	fprintf(fp, "# Low 256MB RAM\n");
	fprintf(fp, "add rwm ram 0x00000000\n");
	fprintf(fp, "ram generic 256M");
	fprintf(fp, "\n");

	/* 1M BIOS */
	fprintf(fp, "# 1M BIOS\n");
	fprintf(fp, "add rom bios 0x1fc00000\n");
	fprintf(fp, "bios generic 1024K\n");
	fprintf(fp, "bios load \"%s\"\n", MSIM_FIRMWARE_BIN);
	fprintf(fp, "\n");

	/* Keyboard */
	fprintf(fp, "# Keyboard\n");
	fprintf(fp, "add dkeyboard kbd0 0x%08x 3\n", MSIM_KBD_PHYSADDR);
	fprintf(fp, "\n");

	/* Printer */
	fprintf(fp, "# Printer\n");
	fprintf(fp, "add dprinter lp0 0x%08x\n", MSIM_LP_PHYSADDR);
	fprintf(fp, "\n");

	/* Disk */
	fprintf(fp, "# Disk\n");
	fprintf(fp, "add ddisk hd0 0x%08x 2\n", MSIM_DISK_PHYSADDR);
	fprintf(fp, "hd0 fmap \"%s\"\n", MSIM_DISK_IMG);
	fprintf(fp, "\n");

	/* Inter-processor Hub */
	fprintf(fp, "# Inter-processor communication device\n");
	fprintf(fp, "add dorder ord0 0x%08x 6\n", MSIM_ORDER_PHYSADDR);
	for (i = 0; i < NR_CPUS; ++i) {
		fprintf(fp, "add rwm mb%d 0x%08x\n", i,
		    MSIM_ORDER_MAILBOX_BASE + i * MSIM_ORDER_MAILBOX_SIZE);
		fprintf(fp, "mb%d generic %d\n", i, MSIM_ORDER_MAILBOX_SIZE);
	}
	fprintf(fp, "\n");

	/* RTC */
	fprintf(fp, "# RTC\n");
	fprintf(fp, "add dtime rtc0 0x%08x\n", MSIM_RTC_PHYSADDR);
	fprintf(fp, "\n");

	fclose(fp);
	return 0;
}
