#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>

#include "arg.h"
#include "../glitch.h"

#define PARAMS_MAX  32

static horizon_Param g_params[PARAMS_MAX+1];

void usage(char *argv0, int code) {
    fprintf(stderr,
    "Usage: %s\n"
    "    -e <lua script>\n"
    "    [-p <params>] [-f <output format>] [-i <input file>] [-o <output file>]\n"
    "\n"
    "Notes: The parameters take the form:\n"
    "    \"<d|i>:<k1>:<v1> <d|i>:<k2>:<v2> ...\", where \"d\" are double parameters,\n"
    "    \"i\" are integer parameters, \"kn\" is a string name for a particular\n"
    "    parameter and \"vn\" its value.\n"
    "\n",
    argv0);
    exit(code);
}

void log_errors(void *user, const char *s) {
    (void)user;
    fprintf(stderr, "%s\n", s);
}

int parse_params(char *ps) {
    int i = 0;

    char *st_arg, *st_type;
    char *arg_tok = strtok_r(ps, " ", &st_arg);

    while (arg_tok && i < PARAMS_MAX) {
        // extract type-key-value triplet
        char *type = strtok_r(arg_tok, ":", &st_type); if (!type) { return -1; }
        char *key = strtok_r(NULL, ":", &st_type); if (!key) { return -1; }
        char *value = strtok_r(NULL, ":", &st_type); if (!value) { return -1; }

        // parse value
        if (!strcmp(type, "d")) {
            g_params[i].kind = HORIZON_PARAM_DOUBLE;
            g_params[i].key = key;
            g_params[i].value.k_double = atof(value);
        } else if (!strcmp(type, "i")) {
            g_params[i].kind = HORIZON_PARAM_INT;
            g_params[i].key = key;
            g_params[i].value.k_int = atoi(value);
        } else {
            return -1;
        }

        // next key-value pair
        arg_tok = strtok_r(NULL, " ", &st_arg);
        i++;
    }
    g_params[i].kind = HORIZON_PARAM_END;

    return i;
}

int main(int argc, char *argv[]) {
    int err = 0, fin = -1, fout = -1, fe = -1;
    char *argv0, *in = NULL, *out = NULL, *lua = NULL, *fmt = NULL, *ps = NULL;

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
    case 'f':
        fmt = ARGF();
        break;
    case 'p':
        ps = ARGF();
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

    horizon_ErrorCtx err_ctx = {
        .fn = log_errors,
    };

    if (ps && parse_params(ps) == -1) {
        fprintf(stderr, "%s: Unable to parse params\n", argv0);
        err = 1;
        goto done;
    }

    if (horizon_ScriptCompileCtxParams(ps ? g_params : NULL, &err_ctx, &sc, &bufr, rbufread) != 0) {
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

    if (horizon_GlitchCtx(&err_ctx, &dst, &src, &sc) != 0) {
        fprintf(stderr, "%s: Failed to glitch image\n", argv0);
        err = 1;
        goto done;
    }

    fmt = fmt ? fmt : "farbfeld";

    if (im_encode(&dst, fmt, fdwrite, &fout) < 0) {
        fprintf(stderr, "%s: Unable to encode '%s' as '%s'\n", argv0, out ? out : "<stdout>", fmt);
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
