#define _XOPEN_SOURCE 500

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <dirent.h>
#include <sys/stat.h>
#include <unistd.h>
#include <pwd.h>
#include <grp.h>
#include <time.h>

#include "lf.h"

struct FileName fn[] = {
    { "Trash", "\uf1f8" }, { "atom", "\ue764" },
    { "bashprofile", "\ue615" }, { "bashrc", "\uf489" },
    { "gitattributes", "\uf1d3" }, { "gitconfig", "\uf1d3" },
    { "github", "\uf408" }, { "gitignore", "\uf1d3" },
    { "gitmodules", "\uf1d3" }, { "rvm", "\ue21e" },
    { "vimrc", "\ue62b" }, { "vscode", "\ue70c" },
    { "zshrc", "\uf489" }, { "git", "\uf1d3" },
    { "go.mod", "\ue626" }, { "go.sum", "\ue626" },
    { "gradle", "\ue256" },

    { "gruntfile.coffee", "\ue611" }, { "gruntfile.js", "\ue611" },
    { "gruntfile.ls", "\ue611" }, { "gulpfile.coffee", "\ue610" },
    { "gulpfile.js", "\ue610" }, { "gulpfile.ls", "\ue610" },
    { "hidden", "\uf023" }, { "include", "\ue5fc" },
    { "lib", "\uf121" }, { "localized", "\uf179" },
    { "Makefile", "\uf489" }, { "node_modules", "\ue718" },
    { "npmignore", "\ue71e" }, { "PKGBUILD", "\uf303" },
    { "rubydoc", "\ue73b" }, { "yarn.lock", "\ue718" },

    { "ai", "\ue7b4" }, { "android", "\ue70e" },
    { "apk", "\ue70e" }, { "apple", "\uf179" },
    { "avi", "\uf03d" }, { "avif", "\uf1c5" },
    { "avro", "\ue60b" }, { "awk", "\uf489" },
    { "bash_history", "\uf489" }, { "bash_profile", "\uf489" },
    { "bmp", "\uf1c5" }, { "bz", "\uf410" },
    { "bz2", "\uf410" }, { "c", "\ue61e" },
    { "c++", "\ue61d" }, { "cab", "\ue70f" },
    { "cc", "\ue61d" }, { "cfg", "\ue615" },
    { "cpp", "\ue61d" }, { "cxx", "\ue61d" },
    { "cs", "\uf81a" }, { "csh", "\uf489" },
    { "css", "\ue749" }, { "d", "\ue7af" },
    { "dart", "\ue798" }, { "deb", "\ue77d" },
    { "fish", "\uf489" }, { "flac", "\uf001" },
    { "flv", "\uf03d" }, { "go", "\ue626" },
    { "gz", "\uf410" }, { "h", "\uf0fd" },
    { "hpp", "\uf0fd" }, { "hs", "\ue777" },
    { "html", "\uf13b" }, { "hxx", "\uf0fd" },
    { "iso", "\ue271" }, { "py", "\ue606" },
    { "r", "\uf25d" }, { "rar", "\uf410" },
    { "razor", "\uf1fa" }, { "rb", "\ue21e" },
    { "rs", "\ue7a8" }, { "rss", "\uf09e" },
    { "ru", "\ue21e" }, { "scala", "\ue737" },
    { "sh", "\uf489" }, { "so", "\uf17c" },
    { "sql", "\uf1c0" }, { "sqlite3", "\ue7c4" },
    { "svg", "\uf1c5" }, { "swift", "\ue755" },
    { "tar", "\uf410" }, { "toml", "\ue615" },
    { "torrent", "\ue275" }, { "ts", "\ue628" },
    { "txt", "\uf15c" }, { "zshrc", "\uf489" },
    { "zst", "\uf410" }
};

static void xerr(int ret, const char *err)
{
    perror(err);
    exit(ret);
}

static void *xcalloc(size_t nmemb, size_t n)
{
    void *ptr;

    ptr = calloc(nmemb, n);

    if (ptr == NULL)
        xerr(EXIT_FAILURE, "calloc()");

    return ptr;
}

static char *scat(char *dst, const char *src)
{
	char *ret;

    ret = dst;

    while (*dst) ++dst;
    while (*src != '\0')
        *dst++ = *src++;

    return ret;
}

static char *human_bytes(struct stat st)
{
    int idx;
    char *buf;
    double base, res;

    buf = calloc(STOR_SIZE, sizeof(char));

    if (st.st_size <= (off_t)0) {
        sprintf(buf, "0.0 B");

        return buf;
    }

    base = log10((double)st.st_size) / log10(UNIT);
    res = round((double)(powf(UNIT, (float)base - floor(base)) * 10.0)) / 10.0;
    idx = floor(base);

    sprintf(buf, "%0.1lf %s", res, suff[idx]);

    return buf;
}

static char *get_extension(const char *file)
{
    char *str;

    str = strrchr(file, '.');

    if (str == NULL)
        return "";

    return str + 1;
}

static char *get_file_perms(struct stat st)
{
    char *ptr;

    ptr = xcalloc(MAX_SIZE, sizeof(char));

    (S_ISCHR(st.st_mode)) ? scat(ptr, "c") : scat(ptr, "-");
    (S_ISLNK(st.st_mode)) ? scat(ptr, "l") : scat(ptr, "-");

    (S_ISDIR(st.st_mode)) ? scat(ptr, "d") : scat(ptr, "-");
    (st.st_mode & S_IRUSR) ? scat(ptr, "r") : scat(ptr, "-");
    (st.st_mode & S_IWUSR) ? scat(ptr, "w") : scat(ptr, "-");
    (st.st_mode & S_IXUSR) ? scat(ptr, "x") : scat(ptr, "-");
    (st.st_mode & S_IRGRP) ? scat(ptr, "r") : scat(ptr, "-");
    (st.st_mode & S_IWGRP) ? scat(ptr, "w") : scat(ptr, "-");
    (st.st_mode & S_IXGRP) ? scat(ptr, "x") : scat(ptr, "-");
    (st.st_mode & S_IROTH) ? scat(ptr, "r") : scat(ptr, "-");
    (st.st_mode & S_IWOTH) ? scat(ptr, "w") : scat(ptr, "-");
    (st.st_mode & S_IXOTH) ? scat(ptr, "x") : scat(ptr, "-");

    return ptr;
}

static char *resolve_symlink(const char *file)
{
    char *ptr;
    ssize_t sz;
    struct stat st;

    ptr = xcalloc(MAX_SIZE, sizeof(char));
    sz = readlink(file, ptr, MAX_SIZE);

    if (sz == -1) {
        free(ptr);
        (void)xerr(EXIT_FAILURE, "readlink()");
    }

    if (stat(file, &st) != 0)
        scat(ptr, RED " (broken)" END);

    return ptr;
}

static char *fmt_unix_time(struct stat st)
{
    char *ptr;
    struct tm tm;

    ptr = xcalloc(MAX_SIZE, sizeof(char));

    localtime_r(&st.st_mtime, &tm);

    if (strftime(ptr, 20, "%a %b %d %Y", &tm) == 0) {
        free(ptr);
        (void)xerr(EXIT_FAILURE, "strftime()");
    }

    return ptr;
}

static char *itox(int i)
{
    /**
     * Since we don't want dynamic allocation using calloc or malloc
     * we'll here use local allocation (which can be used outside function)
    */
    int t;
    char *p;
    static char sto[INT_DIGITS + 2] = {0};

    p = sto + INT_DIGITS + 1;
    t = i;

    do {
        *--p = i >= 0 ? '0' + (i % 10) : '0' - (i % 10);
        i /= 10;
    } while (i != 0);

    t < 0 ? *--p = '-' : ' '; /* For negative values */

    return p;
}

static void list_inside(
    char *path, int resl_sym,
    int show_inodes, int show_icons
)
{
    DIR *di;
    size_t i, len;
    struct dirent *dr;
    struct stat st;
    struct passwd *pw;
    struct group *gr;
    char *last, *fut, *gfp, *rs, *hb;
    char buf[MAX_SIZE * 2] = {0};

    di = opendir(path);

    if (di == NULL)
        xerr(EXIT_FAILURE, "Error");

    while ((dr = readdir(di)) != NULL) {
        if (strcmp(dr->d_name, ".") == 0 ||
            strcmp(dr->d_name, "..") == 0)
            continue;

        len = strlen(path);
        last = path + len - 1;

        if (*last == '/')
            sprintf(buf, "%s", path);
        else
            sprintf(buf, "%s/", path);

        scat(buf, dr->d_name);

        if (lstat(buf, &st) == -1) {
            if (di)
                closedir(di);

            xerr(EXIT_FAILURE, "lstat()");
        }

        pw = getpwuid(st.st_uid);
        gr = getgrgid(st.st_gid);
        gfp = get_file_perms(st);
        fut = fmt_unix_time(st);
        hb = human_bytes(st);

        switch (st.st_mode & S_IFMT) {
        case S_IFDIR:
        case S_IFCHR:
        case S_IFBLK:
        case S_IFREG:
            for (i = 0; i < ARRAY_LEN(fn); i++) {
                if (strcmp(dr->d_name, fn[i].ex) == 0 ||
                    strcmp(get_extension(dr->d_name), fn[i].ex) == 0) {
                    fprintf(
                        stdout,
                        show_icons ? "%s%s%s %s %s %s %s %s %s\n" :
                        "%s%s%s %s %s %s %s %s%s\n",
                        show_inodes ? itox(dr->d_ino) : "",
                        show_inodes ? " " : "", gfp,
                        pw->pw_name, gr->gr_name, hb, fut,
                        show_icons ? fn[i].unicode : "", dr->d_name
                    );

                    goto file_end;
                }
            }

            fprintf(stdout,
                show_icons ?
                    "%s%s%s %s %s %s %s %s %s\n" :
                    "%s%s%s %s %s %s %s%s %s\n",
                show_inodes ? itox(dr->d_ino) : "",
                show_inodes ? " " : "", gfp,
                pw->pw_name, gr->gr_name, hb, fut,
                show_icons ? DEFAULT_UNICODE : "", dr->d_name
            );

            file_end:
                /* Move to in next case */
            break;

        case S_IFLNK: /* Symbolic links */
            rs = resolve_symlink(buf);

            for (i = 0; i < ARRAY_LEN(fn); i++) {
                if (strcmp(dr->d_name, fn[i].ex) == 0 ||
                    strcmp(get_extension(dr->d_name), fn[i].ex) == 0) {
                    fprintf(stdout,
                        show_icons ? "%s%s%s %s %s %s %s %s %s%s%s\n" :
                        "%s%s%s %s %s %s %s %s%s%s%s\n",
                        show_inodes ? itox(dr->d_ino) : "",
                        show_inodes ? " " : "", gfp,
                        pw->pw_name, gr->gr_name, hb, fut,
                        show_icons ? fn[i].unicode : "", dr->d_name,
                        resl_sym ? " -> " : "", resl_sym ? rs : ""
                    );

                    goto lnk_end;
                }
            }

            fprintf(stdout,
                show_icons ? "%s%s%s %s %s %s %s %s %s%s%s\n" :
                "%s%s%s %s %s %s %s %s%s%s%s\n",
                show_inodes ? itox(dr->d_ino) : "",
                show_inodes ? " " : "", gfp,
                pw->pw_name, gr->gr_name, hb, fut,
                show_icons ? DEFAULT_UNICODE : "", dr->d_name,
                resl_sym ? " -> " : "", resl_sym ? rs : ""
            );

            lnk_end:
                /* Dropped. As next iteration will allocate again */
                free(rs);
                /* Move to in next case */
            break;        
        }

        /* Release on each iteration */
        free(fut);
        free(gfp);
        free(hb);
        memset(buf, '\0', sizeof(buf));
    }

    closedir(di);
}

static void usage(void)
{
    fprintf(stdout,
        "lf - List files\n"
        "Usage\n"
        "   [-s]    -- Resolve symbolic links\n"
        "   [-n]    -- Print inodes of each files\n"
        "   [-i]    -- Print icons of each files (if available)\n"
    );
}

int main(int argc, char **argv)
{
    if (argc < 2 || (argc < 3 && argv[1][0] == '-')) {
        usage();
        exit(EXIT_SUCCESS);
    }

    int opt;
    int sflag, nflag, iflag;

    sflag = nflag = iflag = FALSE;

    if (argc < 3) {
        list_inside(argv[1], sflag, nflag, iflag);
        exit(EXIT_SUCCESS);
    }

    while ((opt = getopt(argc, argv, "sni")) != -1) {
        switch (opt) {
        case 's':
            sflag = TRUE;
            break;

        case 'n':
            nflag = TRUE;
            break;

        case 'i':
            iflag = TRUE;
            break;

        case 'h':
        default:
            usage();
            exit(EXIT_SUCCESS);
        }
    }

    while (argc--) {
        if (argv[argc][0] == '-')
            continue;

        if (argc == 0)
            break;

        list_inside(argv[argc], sflag, nflag, iflag);
    }

    exit(EXIT_SUCCESS);
}
