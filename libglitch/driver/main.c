#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>

#include "arg.h"
#include "../glitch.h"

void usage(char *argv0, int code) {
    fprintf(stderr,
    "Usage: %s -e <lua script> [-i <input file>] [-o <output file>]\n",
    argv0);
    exit(code);
}

int main(int argc, char *argv[]) {
    int err = 0, fin = -1, fout = -1, fe = -1;
    char *argv0, *in = NULL, *out = NULL, *lua = NULL;

    Image_t src = {.allocator = im_std_allocator};
    Image_t dst = {.allocator = im_std_allocator};

    struct horizon_Script sc = {0};

    ARGBEGIN {
    case 'h':
        usage(argv0, 0);
        break;
    case 'e':
        lua = ARGF();
        break;
    case 'i':
        in = ARGF();
        break;
    case 'o':
        out = ARGF();
        break;
    } ARGEND

    if (!lua) {
        usage(argv0, 1);
    }

    fe = open(lua, O_RDONLY);
    if (fe < 0) {
        perror(argv0);
        err = 1;
        goto done;
    }

    fin = in ? open(in, O_RDONLY) : 0;
    if (fin < 0) {
        perror(argv0);
        err = 1;
        goto done;
    }

    fout = out ? open(out, O_WRONLY|O_CREAT|O_TRUNC, 0644) : 1;
    if (fout < 0) {
        perror(argv0);
        err = 1;
        goto done;
    }

    char buf[4096];
    BufferedReader_t bufr = {
        .rf = fdread,
        .src = &fe,
        .buf = buf,
        .bufsz = sizeof(buf),
    };

    if (horizon_ScriptCompile(&sc, &bufr, rbufread) != 0) {
        fprintf(stderr, "%s: Unable to compile lua script: %s\n", argv0, lua);
        err = 1;
        goto done;
    }

    im_load_defaults();

    if (!im_decode(&src, fdread, &fin)) {
        fprintf(stderr, "%s: Unable to decode '%s'\n", argv0, in ? in : "<stdin>");
        err = 1;
        goto done;
    }

    im_initimg_nrgba64(&dst, src.w, src.h, im_std_allocator);

    if (horizon_Glitch(&dst, &src, &sc) != 0) {
        fprintf(stderr, "%s: Failed to glitch image\n", argv0);
        err = 1;
        goto done;
    }

    if (im_encode(&dst, "farbfeld", fdwrite, &fout) < 0) {
        fprintf(stderr, "%s: Unable to encode '%s' as 'farbfeld'\n", argv0, out ? out : "<stdout>");
        err = 1;
    }

done:
    horizon_ScriptClose(&sc);
    im_xfree(im_std_allocator, src.img);
    im_xfree(im_std_allocator, dst.img);
    if (fe != -1) close(fe);
    if (fin != -1) close(fin);
    if (fout != -1) close(fout);

    return err;
}
