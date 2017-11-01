/* simple.c

   Simple libftdi usage example

   This program is distributed under the GPL, version 2
*/

#include <stdio.h>
#include <stdlib.h>
#include <getopt.h>
#include <ftdi.h>
#include <ftdi_i.h>


char *ChipType[] = {
    "AM",
    "BM",
    "2232C",
    "R",
    "2232H",
    "4232H",
    "232H",
    "230X",
};

int main(int argc, char **argv)
{
    int ret;
    int opt;
    int vid = 0;
    int pid = 0;
    struct ftdi_context *ftdi;
    struct ftdi_version_info version;

    while ((opt = getopt(argc, argv, "v:p:h")) != -1)
    {
        switch (opt)
        {
            case 'v':   /* VID */
                vid = strtoul(optarg, NULL, 0);
                break;
            case 'p':   /* PID */
                pid = strtoul(optarg, NULL, 0);
                break;
            case 'h':   /* Help */
            default:
                fprintf(stderr, "usage: %s [options]\n", *argv);
                fprintf(stderr, "\t-v <number> Search for device with VID == number\n");
                fprintf(stderr, "\t-p <number> Search for device with PID == number\n");
                return EXIT_SUCCESS;
        }
    }

    if ((ftdi = ftdi_new()) == 0)
    {
        fprintf(stderr, "ftdi_new failed\n");
        return EXIT_FAILURE;
    }

    version = ftdi_get_library_version();
    printf("Initialized libftdi %s (major: %d, minor: %d, micro: %d, snapshot ver: %s)\n",
        version.version_str, version.major, version.minor, version.micro,
        version.snapshot_str);

    if ((ret = ftdi_usb_open(ftdi, vid, pid)) < 0)
    {
        fprintf(stderr, "unable to open ftdi device: %d (%s)\n", ret, ftdi_get_error_string(ftdi));
        ftdi_free(ftdi);
        return EXIT_FAILURE;
    }

    fprintf(stdout, "Type=%s, Size=%d\n",
        ChipType[ftdi->type], ((ftdi->eeprom) ? (ftdi->eeprom->size) : 0) );

    // Read out FTDIChip-ID of R type chips
    if (ftdi->type == TYPE_R)
    {
        unsigned int chipid;
        printf("ftdi_read_chipid: %d\n", ftdi_read_chipid(ftdi, &chipid));
        printf("FTDI chipid: %X\n", chipid);
    }

    if ((ret = ftdi_usb_close(ftdi)) < 0)
    {
        fprintf(stderr, "unable to close ftdi device: %d (%s)\n", ret, ftdi_get_error_string(ftdi));
        ftdi_free(ftdi);
        return EXIT_FAILURE;
    }

    ftdi_free(ftdi);

    return EXIT_SUCCESS;
}
