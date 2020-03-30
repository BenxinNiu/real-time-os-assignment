/*
 * Copyright 2018, 2020 Jonathan Anderson
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include <errno.h>
#include <stdio.h>


#include "assign4.h"


// Some hard-coded inode numbers for initial backed files:
#define    ROOT_DIR    1
#define    ASSIGN_DIR    2
#define    USERNAME_FILE    3
#define    FEATURE_FILE    4
#define    INITIAL_FILE_COUNT    100
#define MAX_FILE_NAME_LENGTH 100

struct metadata {
    fuse_ino_t ino;
    char name[MAX_FILE_NAME_LENGTH];
    fuse_ino_t children[INITIAL_FILE_COUNT];
    int child_count;
    // flag for keeping record in trash bin
    bool in_trash;
    bool recovered;
};

int current_ava_ino;
struct stat *file_stats;
struct metadata *metadata_stats;

static const char UsernameContent[] = "bn2645_practicing_social_distancing\n";

static const char FeatureImplContent[] = "- directory listing\n";

static const int AllRead = S_IRUSR | S_IRGRP | S_IROTH;
static const int AllExec = S_IXUSR | S_IXGRP | S_IXOTH;
static const int All = S_IRWXU | S_IRWXG | S_IRWXO;

static struct metadata *get_metadata(fuse_ino_t ino) {
    for (int i = 1; i <= INITIAL_FILE_COUNT; i++) {
        if (metadata_stats[i].ino == ino) {
            return &(metadata_stats[i]);
        }
    }
    return NULL;
}

static void
assign4_init(void *userdata, struct fuse_conn_info *conn) {
    struct backing_file *backing = userdata;
    fprintf(stderr, "%s '%s'\n", __func__, backing->bf_path);
    printf("\n\nsocial distancing drives people to learn file system "
           "and keeps everyone safe\n\n");
    /*
     * This function should do some setup (e.g., open the backing file or
     * mmap(2) some memory) and prepare any metadata that you need.
     */

    file_stats = calloc(INITIAL_FILE_COUNT + 1, sizeof(*file_stats));
    metadata_stats = calloc(INITIAL_FILE_COUNT + 1, sizeof(*metadata_stats));

    file_stats[ROOT_DIR].st_ino = ROOT_DIR;
    file_stats[ROOT_DIR].st_mode = S_IFDIR | All | AllExec;
    file_stats[ROOT_DIR].st_nlink = 1;

    metadata_stats[ROOT_DIR].ino = ROOT_DIR;
    strcpy(metadata_stats[ROOT_DIR].name, "");
    metadata_stats[ROOT_DIR].child_count = 1;
    metadata_stats[ROOT_DIR].children[0] = ASSIGN_DIR;

    file_stats[ASSIGN_DIR].st_ino = ASSIGN_DIR;
    file_stats[ASSIGN_DIR].st_mode = S_IFDIR | All | AllExec;
    file_stats[ASSIGN_DIR].st_nlink = 1;

    metadata_stats[ASSIGN_DIR].ino = ASSIGN_DIR;
    strcpy(metadata_stats[ASSIGN_DIR].name, "assignment");
    metadata_stats[ASSIGN_DIR].child_count = 0;
    metadata_stats[ASSIGN_DIR].child_count = 2;
    metadata_stats[ASSIGN_DIR].children[0] = USERNAME_FILE;
    metadata_stats[ASSIGN_DIR].children[1] = FEATURE_FILE;

    file_stats[USERNAME_FILE].st_ino = USERNAME_FILE;
    file_stats[USERNAME_FILE].st_mode = S_IFREG | AllRead;
    file_stats[USERNAME_FILE].st_size = sizeof(UsernameContent);
    file_stats[USERNAME_FILE].st_nlink = 1;

    metadata_stats[USERNAME_FILE].ino = USERNAME_FILE;
    strcpy(metadata_stats[USERNAME_FILE].name, "username");
    metadata_stats[USERNAME_FILE].child_count = 0;

    file_stats[FEATURE_FILE].st_ino = FEATURE_FILE;
    file_stats[FEATURE_FILE].st_mode = S_IFREG | AllRead;
    file_stats[FEATURE_FILE].st_size = sizeof(FeatureImplContent);
    file_stats[FEATURE_FILE].st_nlink = 1;

    metadata_stats[FEATURE_FILE].ino = FEATURE_FILE;
//    metadata_stats[FEATURE_FILE].name = "features";
    strcpy(metadata_stats[FEATURE_FILE].name, "features");
    metadata_stats[FEATURE_FILE].child_count = 0;

    current_ava_ino = FEATURE_FILE + 1;
}

static void
assign4_destroy(void *userdata) {
    struct backing_file *backing = userdata;
    fprintf(stderr, "*** %s %d\n", __func__, backing->bf_fd);

    free(file_stats);
    free(metadata_stats);
    file_stats = NULL;
    metadata_stats= NULL;
}


/* https://github.com/libfuse/libfuse/blob/fuse_2_9_bugfix/include/fuse_lowlevel.h#L801 */
static void
assign4_create(fuse_req_t req, fuse_ino_t parent, const char *name,
               mode_t mode, struct fuse_file_info *fi) {
    fprintf(stderr, "%s parent=%zu name='%s' mode=%d\n", __func__,
            parent, name, mode);

    /*
     * Create and open a file.
     *
     * Respond by calling fuse_reply_err() if there's an error, or else
     * fuse_reply_create(), passing it information in a fuse_entry_param:
     *
     * https://github.com/libfuse/libfuse/blob/fuse_2_9_bugfix/include/fuse_lowlevel.h#L68
     *
     * This is the meaning of the "Valid replies" comment at
     * https://github.com/libfuse/libfuse/blob/fuse_2_9_bugfix/include/fuse_lowlevel.h#L791
     */

    fuse_reply_err(req, ENOSYS);
}

/* https://github.com/libfuse/libfuse/blob/fuse_2_9_bugfix/include/fuse_lowlevel.h#L256 */
static void
assign4_getattr(fuse_req_t req, fuse_ino_t ino, struct fuse_file_info *fip) {
    fprintf(stderr, "%s ino=%zu\n", __func__, ino);

    // make sure the requested inode does not exceed file count
    if (ino > INITIAL_FILE_COUNT) {
        fuse_reply_err(req, ENOENT);
        return;
    }

    int result = fuse_reply_attr(req, file_stats + ino, 1);
    if (result != 0) {
        fprintf(stderr, "Failed to send attr reply\n");
    }
}

/* https://github.com/libfuse/libfuse/blob/fuse_2_9_bugfix/include/fuse_lowlevel.h#L205 */
static void
assign4_lookup(fuse_req_t req, fuse_ino_t parent, const char *name) {
    fprintf(stderr, "%s parent=%zu name='%s'\n", __func__,
            parent, name);

    struct fuse_entry_param dirent;

    struct metadata *parent_metadata = get_metadata(parent);
    if (parent_metadata == NULL) {
        fuse_reply_err(req, ENOENT);
        return;
    }
    for (int i = 0; i < parent_metadata->child_count; i++) {
        struct metadata *child = get_metadata(parent_metadata->children[i]);
        if (child != NULL && strcmp(name, child->name) == 0) {
            dirent.ino = child->ino;
            dirent.attr = file_stats[child->ino];
            dirent.generation = 1;
            // Assume that these values are always valid for 1s:
            dirent.attr_timeout = 1;
            dirent.entry_timeout = 1;

            int result = fuse_reply_entry(req, &dirent);
            if (result != 0) {
                fprintf(stderr, "Failed to send dirent reply\n");
            }
            return;
        }
    }
    fuse_reply_err(req, ENOENT);
}

/* https://github.com/libfuse/libfuse/blob/fuse_2_9_bugfix/include/fuse_lowlevel.h#L367 */
static void
assign4_rmdir(fuse_req_t req, fuse_ino_t parent, const char *name) {
    fprintf(stderr, "%s parent=%zu name='%s'\n", __func__, parent, name);
    struct metadata* parent_metadata = get_metadata(parent);

    for (int i=0; i < parent_metadata->child_count; i++) {
        struct metadata* child = get_metadata(parent_metadata->children[i]);

        if(child != NULL && !child->in_trash && strcmp(name, child->name) == 0){
            child->in_trash = true;
            child->recovered = false;
            fuse_reply_err(req, 0);
            return;
        }
    }
    // file not found
    fuse_reply_err(req, ENOENT);
}

/* https://github.com/libfuse/libfuse/blob/fuse_2_9_bugfix/include/fuse_lowlevel.h#L332 */
static void
assign4_mkdir(fuse_req_t req, fuse_ino_t parent, const char *name, mode_t mode) {
    fprintf(stderr, "%s parent=%zu name='%s' mode=%d\n", __func__,
            parent, name, mode);
    if (current_ava_ino > INITIAL_FILE_COUNT) {
        fuse_reply_err(req, ENOENT);
    }

    struct fuse_entry_param dirent;

    struct metadata *parent_metadata = get_metadata(parent);
    if(parent_metadata == NULL){
        fuse_reply_err(req, ENOENT);
        return;
    }
    // create new directory
    file_stats[current_ava_ino].st_ino = current_ava_ino;
//    file_stats[current_ava_ino].st_mode = S_IFDIR | AllRead | AllExec;
    file_stats[current_ava_ino].st_mode = mode;
    file_stats[current_ava_ino].st_nlink = 1;

    // set metadata of new directory
//    strcpy(metadata_stats[current_ava_ino].name, name);
//    metadata_stats[current_ava_ino].name = name;
    strcpy(metadata_stats[current_ava_ino].name, name);
    metadata_stats[current_ava_ino].child_count=0;
    metadata_stats[current_ava_ino].ino = current_ava_ino;
    metadata_stats[current_ava_ino].in_trash = false;

    // update metadata of parent
    parent_metadata->children[parent_metadata->child_count] = current_ava_ino;
    parent_metadata->child_count++;

    dirent.ino = current_ava_ino;
    dirent.attr = file_stats[current_ava_ino];

    current_ava_ino++;

    int result = fuse_reply_entry(req, &dirent);
    if (result != 0) {
        fprintf(stderr, "Failed to send dirent reply\n");
    }
}

/* https://github.com/libfuse/libfuse/blob/fuse_2_9_bugfix/include/fuse_lowlevel.h#L317 */
static void
assign4_mknod(fuse_req_t req, fuse_ino_t parent, const char *name,
              mode_t mode, dev_t rdev) {
    fprintf(stderr, "%s parent=%zu name='%s' mode=%d\n", __func__,
            parent, name, mode);
    fuse_reply_err(req, ENOSYS);
}

/* https://github.com/libfuse/libfuse/blob/fuse_2_9_bugfix/include/fuse_lowlevel.h#L444 */
static void
assign4_open(fuse_req_t req, fuse_ino_t ino, struct fuse_file_info *fi) {
    fprintf(stderr, "%s ino=%zu\n", __func__, ino);
    fuse_reply_err(req, ENOSYS);
}

/* https://github.com/libfuse/libfuse/blob/fuse_2_9_bugfix/include/fuse_lowlevel.h#L622 */
static void
assign4_readdir(fuse_req_t req, fuse_ino_t ino, size_t size, off_t off, struct fuse_file_info *fi) {
    fprintf(stderr, "%s ino=%zu size=%zu off=%zu\n", __func__,
            ino, size, off);

    if (off > 2) {
        fuse_reply_buf(req, NULL, 0);
        return;
    }

    struct stat *self = file_stats + ino;
    // not a directory
    if (!S_ISDIR(self->st_mode)) {
        fuse_reply_err(req, ENOTDIR);
        return;
    }

    struct stat *parent = file_stats + ROOT_DIR;

    char buffer[size];
    off_t bytes = 0;
    int next = 0;

    if (off < 1) {
        bytes += fuse_add_direntry(req, buffer + bytes,
                                   sizeof(buffer) - bytes,
                                   ".", self, ++next);
    }

    if (off < 2) {
        bytes += fuse_add_direntry(req, buffer + bytes,
                                   sizeof(buffer) - bytes,
                                   "..", parent, ++next);
    }

    struct metadata* target_metadata = get_metadata(ino);

    // metadata not found for this inode
    if (target_metadata == NULL) {
        fuse_reply_err(req, ENOENT);
        return;
    }
    for(int i=0; i < target_metadata->child_count; i++){
        struct metadata* child = get_metadata(target_metadata->children[i]);
        printf("\n\ninode:%d name: %s\n\n", child->ino, child->name);
        if(child != NULL && !child->in_trash){
            bytes += fuse_add_direntry(req, buffer + bytes,
                                       sizeof(buffer) - bytes,
                                       child->name,
                                       file_stats + child->ino,
                                       ++next);
        }
    }

    int result = fuse_reply_buf(req, buffer, bytes);
    if (result != 0) {
        fprintf(stderr, "Failed to send readdir reply\n");
    }
}

/* https://github.com/libfuse/libfuse/blob/fuse_2_9_bugfix/include/fuse_lowlevel.h#L472 */
static void
assign4_read(fuse_req_t req, fuse_ino_t ino, size_t size,
             off_t off, struct fuse_file_info *fi) {
    fprintf(stderr, "%s ino=%zu size=%zu off=%zu\n", __func__,
            ino, size, off);

    const char *response_data = NULL;
    size_t response_len;
    int err = 0;

    switch (ino) {
        case ROOT_DIR:
        case ASSIGN_DIR:
            err = EISDIR;
            break;

        case USERNAME_FILE:
            if (off >= sizeof(UsernameContent)) {
                response_len = 0;
                break;
            }

            response_data = UsernameContent + off;
            response_len = sizeof(UsernameContent) - off;
            if (response_len > size) {
                response_len = size;
            }
            break;

        case FEATURE_FILE:
            if (off >= sizeof(FeatureImplContent)) {
                response_len = 0;
                break;
            }

            response_data = FeatureImplContent + off;
            response_len = sizeof(FeatureImplContent) - off;
            if (response_len > size) {
                response_len = size;
            }
            break;

        default:
            err = EBADF;
    }
    if (err != 0) {
        fuse_reply_err(req, err);
        return;
    }

    assert(response_data != NULL);
    int result = fuse_reply_buf(req, response_data, response_len);
    if (result != 0) {
        fprintf(stderr, "Failed to send read reply\n");
    }
}

/* https://github.com/libfuse/libfuse/blob/fuse_2_9_bugfix/include/fuse_lowlevel.h#L286 */
static void
assign4_setattr(fuse_req_t req, fuse_ino_t ino, struct stat *attr,
                int to_set, struct fuse_file_info *fi) {
    fprintf(stderr, "%s ino=%zu to_set=%d\n", __func__, ino, to_set);
    fuse_reply_err(req, ENOSYS);
}

/* https://github.com/libfuse/libfuse/blob/fuse_2_9_bugfix/include/fuse_lowlevel.h#L674 */
static void
assign4_statfs(fuse_req_t req, fuse_ino_t ino) {
    fprintf(stderr, "%s ino=%zu\n", __func__, ino);

    /*
     * unsigned long	f_bsize;	/* File system block size */
//    unsigned long	f_frsize;	/* Fundamental file system block size */
//    fsblkcnt_t	f_blocks;	/* Blocks on FS in units of f_frsize */
//    fsblkcnt_t	f_bfree;	/* Free blocks */
//    fsblkcnt_t	f_bavail;	/* Blocks available to non-root */
//    fsfilcnt_t	f_files;	/* Total inodes */
//    fsfilcnt_t	f_ffree;	/* Free inodes */
//    fsfilcnt_t	f_favail;	/* Free inodes for non-root */
//    unsigned long	f_fsid;		/* Filesystem ID */
//    unsigned long	f_flag;		/* Bit mask of values */
//    unsigned long	f_namemax;	/* Max file name length */

    struct statvfs stbuf;
    stbuf.f_files = INITIAL_FILE_COUNT;
    stbuf.f_ffree = INITIAL_FILE_COUNT - current_ava_ino;

    fuse_reply_statfs(req, &stbuf);
//    fuse_reply_err(req, ENOSYS);
}

/* https://github.com/libfuse/libfuse/blob/fuse_2_9_bugfix/include/fuse_lowlevel.h#L350 */
static void
assign4_unlink(fuse_req_t req, fuse_ino_t parent, const char *name) {
    fprintf(stderr, "%s parent=%zu name='%s'\n", __func__, parent, name);
    fuse_reply_err(req, ENOSYS);
}

/* https://github.com/libfuse/libfuse/blob/fuse_2_9_bugfix/include/fuse_lowlevel.h#L498 */
static void
assign4_write(fuse_req_t req, fuse_ino_t ino, const char *buf, size_t size,
              off_t off, struct fuse_file_info *fi) {
    fprintf(stderr, "%s ino=%zu size=%zu off=%zu\n", __func__,
            ino, size, off);
    fuse_reply_err(req, ENOSYS);
}


static struct fuse_lowlevel_ops assign4_ops = {
        .init           = assign4_init,
        .destroy        = assign4_destroy,

        .create         = assign4_create,
        .getattr        = assign4_getattr,
        .lookup         = assign4_lookup,
        .mkdir          = assign4_mkdir,
        .mknod          = assign4_mknod,
//        .open           = assign4_open,
        .read           = assign4_read,
        .readdir        = assign4_readdir,
        .rmdir          = assign4_rmdir,
        .setattr        = assign4_setattr,
        .statfs         = assign4_statfs,
        .unlink         = assign4_unlink,
        .write          = assign4_write,
};


#if !defined(USE_EXAMPLEHI) && !defined(USE_SOLUTION)

struct fuse_lowlevel_ops *
assign4_fuse_ops() {
    return &assign4_ops;
}

#endif