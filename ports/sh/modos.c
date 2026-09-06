//---------------------------------------------------------------------------//
//    ____        PythonUltra                                                //
//.-'`_ o `;__,   A community port of MicroPython for CASIO calculators.     //
//.-'` `---`  '   License: MIT (except some files; see LICENSE)              //
//---------------------------------------------------------------------------//
// pe.modos: Small filesystem module with PythonUltra virtual cwd support.

#include "py/runtime.h"
#include "pathutil.h"

#include <dirent.h>
#include <errno.h>
#include <stdio.h>
#include <string.h>
#include <sys/stat.h>
#include <unistd.h>

static void pe_os_check(int result)
{
    if(result < 0)
        mp_raise_OSError(errno);
}

static char const *pe_os_path(mp_obj_t path_in, char *buffer, size_t size)
{
    char const *path = mp_obj_str_get_str(path_in);
    if(pe_path_resolve(path, buffer, size) < 0)
        mp_raise_OSError(errno);
    return buffer;
}

static mp_obj_t pe_os_getcwd(void)
{
    char const *cwd = pe_path_getcwd();
    return mp_obj_new_str(cwd, strlen(cwd));
}
MP_DEFINE_CONST_FUN_OBJ_0(pe_os_getcwd_obj, pe_os_getcwd);

static mp_obj_t pe_os_chdir(mp_obj_t path_in)
{
    pe_os_check(pe_path_chdir(mp_obj_str_get_str(path_in)));
    return mp_const_none;
}
MP_DEFINE_CONST_FUN_OBJ_1(pe_os_chdir_obj, pe_os_chdir);

static mp_obj_t pe_os_listdir(size_t n_args, const mp_obj_t *args)
{
    char resolved[PE_PATH_MAX];
    char const *input = n_args ? mp_obj_str_get_str(args[0]) : ".";
    if(pe_path_resolve(input, resolved, sizeof resolved) < 0)
        mp_raise_OSError(errno);
    DIR *dir = opendir(resolved);
    if(!dir)
        mp_raise_OSError(errno);

    mp_obj_t list = mp_obj_new_list(0, NULL);
    struct dirent *entry;
    while((entry = readdir(dir))) {
        if(!strcmp(entry->d_name, ".") || !strcmp(entry->d_name, ".."))
            continue;
        mp_obj_list_append(list,
            mp_obj_new_str(entry->d_name, strlen(entry->d_name)));
    }

    int close_result = closedir(dir);
    if(close_result < 0)
        mp_raise_OSError(errno);
    return list;
}
MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(pe_os_listdir_obj, 0, 1, pe_os_listdir);

static mp_obj_t pe_os_mkdir(size_t n_args, const mp_obj_t *args)
{
    char path[PE_PATH_MAX];
    mode_t mode = n_args > 1 ? mp_obj_get_int(args[1]) : 0777;
    pe_os_path(args[0], path, sizeof path);
    pe_os_check(mkdir(path, mode));
    return mp_const_none;
}
MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(pe_os_mkdir_obj, 1, 2, pe_os_mkdir);

static mp_obj_t pe_os_remove(mp_obj_t path_in)
{
    char path[PE_PATH_MAX];
    pe_os_path(path_in, path, sizeof path);
    pe_os_check(remove(path));
    return mp_const_none;
}
MP_DEFINE_CONST_FUN_OBJ_1(pe_os_remove_obj, pe_os_remove);

static mp_obj_t pe_os_rename(mp_obj_t old_in, mp_obj_t new_in)
{
    char old_path[PE_PATH_MAX], new_path[PE_PATH_MAX];
    pe_os_path(old_in, old_path, sizeof old_path);
    pe_os_path(new_in, new_path, sizeof new_path);
    pe_os_check(rename(old_path, new_path));
    return mp_const_none;
}
MP_DEFINE_CONST_FUN_OBJ_2(pe_os_rename_obj, pe_os_rename);

static mp_obj_t pe_os_rmdir(mp_obj_t path_in)
{
    char path[PE_PATH_MAX];
    pe_os_path(path_in, path, sizeof path);
    pe_os_check(rmdir(path));
    return mp_const_none;
}
MP_DEFINE_CONST_FUN_OBJ_1(pe_os_rmdir_obj, pe_os_rmdir);

static mp_obj_t pe_os_stat(mp_obj_t path_in)
{
    char path[PE_PATH_MAX];
    struct stat st;
    pe_os_path(path_in, path, sizeof path);
    pe_os_check(stat(path, &st));

    mp_obj_t fields[] = {
        mp_obj_new_int_from_uint(st.st_mode),
        mp_obj_new_int_from_uint(st.st_ino),
        mp_obj_new_int_from_uint(st.st_dev),
        mp_obj_new_int_from_uint(st.st_nlink),
        mp_obj_new_int_from_uint(st.st_uid),
        mp_obj_new_int_from_uint(st.st_gid),
        mp_obj_new_int_from_ll(st.st_size),
        mp_obj_new_int_from_ll(st.st_atime),
        mp_obj_new_int_from_ll(st.st_mtime),
        mp_obj_new_int_from_ll(st.st_ctime),
    };
    return mp_obj_new_tuple(MP_ARRAY_SIZE(fields), fields);
}
MP_DEFINE_CONST_FUN_OBJ_1(pe_os_stat_obj, pe_os_stat);

static const mp_rom_map_elem_t pe_os_globals_table[] = {
    { MP_ROM_QSTR(MP_QSTR___name__), MP_ROM_QSTR(MP_QSTR_os) },
    { MP_ROM_QSTR(MP_QSTR_sep), MP_ROM_QSTR(MP_QSTR__slash_) },
    { MP_ROM_QSTR(MP_QSTR_getcwd), MP_ROM_PTR(&pe_os_getcwd_obj) },
    { MP_ROM_QSTR(MP_QSTR_chdir), MP_ROM_PTR(&pe_os_chdir_obj) },
    { MP_ROM_QSTR(MP_QSTR_listdir), MP_ROM_PTR(&pe_os_listdir_obj) },
    { MP_ROM_QSTR(MP_QSTR_mkdir), MP_ROM_PTR(&pe_os_mkdir_obj) },
    { MP_ROM_QSTR(MP_QSTR_remove), MP_ROM_PTR(&pe_os_remove_obj) },
    { MP_ROM_QSTR(MP_QSTR_unlink), MP_ROM_PTR(&pe_os_remove_obj) },
    { MP_ROM_QSTR(MP_QSTR_rename), MP_ROM_PTR(&pe_os_rename_obj) },
    { MP_ROM_QSTR(MP_QSTR_rmdir), MP_ROM_PTR(&pe_os_rmdir_obj) },
    { MP_ROM_QSTR(MP_QSTR_stat), MP_ROM_PTR(&pe_os_stat_obj) },
};
static MP_DEFINE_CONST_DICT(pe_os_globals, pe_os_globals_table);

const mp_obj_module_t pe_module_os = {
    .base = { &mp_type_module },
    .globals = (mp_obj_dict_t *)&pe_os_globals,
};

MP_REGISTER_MODULE(MP_QSTR_os, pe_module_os);
MP_REGISTER_MODULE(MP_QSTR_uos, pe_module_os);
