cc_library(
    name = "connector",
    srcs = glob(
        ["libmariadb/*.c"],
        exclude = ["libmariadb/bmove_upp.c"],
    ) + [
        #        "libmariadb/secure/openssl.c",
        ":ma_client_plugin_c",
        "plugins/auth/my_auth.c",
        "plugins/auth/old_password.c",
        "plugins/pvio/pvio_socket.c",
    ],
    hdrs = glob(["include/**/*.h"]) + [
        ":mariadb_version_h",
        ":ma_config_h",
    ],
    copts = [
        "-DLIBMARIADB",
        "-Iinclude",
        "-DTHREAD",
        #"-DHAVE_OPENSSL",
        #"-DHAVE_TLS",
        "-Dlibmariadb_EXPORTS",
    ],
    strip_include_prefix = "include",
    visibility = ["//visibility:public"],
    #    deps = [
    #        "@boringssl//:crypto",
    #        "@boringssl//:ssl",
    #    ],
)

genrule(
    name = "ma_client_plugin_c",
    outs = ["libmariadb/ma_client_plugin.c"],
    cmd = """
  cat <<EOF >$@
/* Copyright (C) 2010 - 2012 Sergei Golubchik and Monty Program Ab
                 2015-2016 MariaDB Corporation AB

   This library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Library General Public
   License as published by the Free Software Foundation; either
   version 2 of the License, or (at your option) any later version.
   
   This library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Library General Public License for more details.
   
   You should have received a copy of the GNU Library General Public
   License along with this library; if not see <http://www.gnu.org/licenses>
   or write to the Free Software Foundation, Inc., 
   51 Franklin St., Fifth Floor, Boston, MA 02110, USA */

#if _MSC_VER
/* Silence warnings about variable 'unused' being used. */
#define FORCE_INIT_OF_VARS 1
#endif

#include <ma_global.h>
#include <ma_sys.h>
#include <ma_common.h> 
#include <ma_string.h>
#include <ma_pthread.h>

#include "errmsg.h"
#include <mysql/client_plugin.h>

struct st_client_plugin_int {
  struct st_client_plugin_int *next;
  void   *dlhandle;
  struct st_mysql_client_plugin *plugin;
};

static my_bool initialized= 0;
static MA_MEM_ROOT mem_root;

static uint valid_plugins[][2]= {
  {MYSQL_CLIENT_AUTHENTICATION_PLUGIN, MYSQL_CLIENT_AUTHENTICATION_PLUGIN_INTERFACE_VERSION},
  {MARIADB_CLIENT_PVIO_PLUGIN, MARIADB_CLIENT_PVIO_PLUGIN_INTERFACE_VERSION},
  {MARIADB_CLIENT_TRACE_PLUGIN, MARIADB_CLIENT_TRACE_PLUGIN_INTERFACE_VERSION},
  {MARIADB_CLIENT_REMOTEIO_PLUGIN, MARIADB_CLIENT_REMOTEIO_PLUGIN_INTERFACE_VERSION},
  {MARIADB_CLIENT_CONNECTION_PLUGIN, MARIADB_CLIENT_CONNECTION_PLUGIN_INTERFACE_VERSION},
  {0, 0}
};

struct st_client_plugin_int *plugin_list[MYSQL_CLIENT_MAX_PLUGINS + MARIADB_CLIENT_MAX_PLUGINS];
#ifdef THREAD
static pthread_mutex_t LOCK_load_client_plugin;
#endif

 extern struct st_mysql_client_plugin mysql_native_password_client_plugin;
 extern struct st_mysql_client_plugin mysql_old_password_client_plugin;
 extern struct st_mysql_client_plugin pvio_socket_client_plugin;

struct st_mysql_client_plugin *mysql_client_builtins[]=
{

     (struct st_mysql_client_plugin *)&mysql_native_password_client_plugin,
   (struct st_mysql_client_plugin *)&mysql_old_password_client_plugin,
   (struct st_mysql_client_plugin *)&pvio_socket_client_plugin,

  0
};
static int is_not_initialized(MYSQL *mysql, const char *name)
{
  if (initialized)
    return 0;

  my_set_error(mysql, CR_AUTH_PLUGIN_CANNOT_LOAD,
               SQLSTATE_UNKNOWN, ER(CR_AUTH_PLUGIN_CANNOT_LOAD),
               name, "not initialized");
  return 1;
}
static int get_plugin_nr(uint type)
{
  uint i= 0;
  for(; valid_plugins[i][1]; i++)
    if (valid_plugins[i][0] == type)
      return i;
  return -1;
}
static const char *check_plugin_version(struct st_mysql_client_plugin *plugin, unsigned int version)
{
  if (plugin->interface_version < version ||
      (plugin->interface_version >> 8) > (version >> 8))
    return "Incompatible client plugin interface";
  return 0;
}
static struct st_mysql_client_plugin *find_plugin(const char *name, int type)
{
  struct st_client_plugin_int *p;
  int plugin_nr= get_plugin_nr(type);

  DBUG_ASSERT(initialized);
  if (plugin_nr == -1)
    return 0;

  if (!name)
    return plugin_list[plugin_nr]->plugin;

  for (p= plugin_list[plugin_nr]; p; p= p->next)
  {
    if (strcmp(p->plugin->name, name) == 0)
      return p->plugin;
  }
  return NULL;
}
static struct st_mysql_client_plugin *
add_plugin(MYSQL *mysql, struct st_mysql_client_plugin *plugin, void *dlhandle,
           int argc, va_list args)
{
  const char *errmsg;
  struct st_client_plugin_int plugin_int, *p;
  char errbuf[1024];
  int plugin_nr;

  DBUG_ASSERT(initialized);

  plugin_int.plugin= plugin;
  plugin_int.dlhandle= dlhandle;

  if ((plugin_nr= get_plugin_nr(plugin->type)) == -1)
  {
    errmsg= "Unknown client plugin type";
    goto err1;
  }
  if ((errmsg= check_plugin_version(plugin, valid_plugins[plugin_nr][1])))
    goto err1;

  /* Call the plugin initialization function, if any */
  if (plugin->init && plugin->init(errbuf, sizeof(errbuf), argc, args))
  {
    errmsg= errbuf;
    goto err1;
  }

  p= (struct st_client_plugin_int *)
    ma_memdup_root(&mem_root, (char *)&plugin_int, sizeof(plugin_int));

  if (!p)
  {
    errmsg= "Out of memory";
    goto err2;
  }

#ifdef THREAD
  safe_mutex_assert_owner(&LOCK_load_client_plugin);
#endif

  p->next= plugin_list[plugin_nr];
  plugin_list[plugin_nr]= p;

  return plugin;

err2:
  if (plugin->deinit)
    plugin->deinit();
err1:
  my_set_error(mysql, CR_AUTH_PLUGIN_CANNOT_LOAD, SQLSTATE_UNKNOWN,
               ER(CR_AUTH_PLUGIN_CANNOT_LOAD), plugin->name, errmsg);
  if (dlhandle)
    (void)dlclose(dlhandle);
  return NULL;
}
static void load_env_plugins(MYSQL *mysql)
{
  char *plugs, *free_env, *s= getenv("LIBMYSQL_PLUGINS");

  /* no plugins to load */
  if (!s)
    return;

  free_env= plugs= strdup(s);

  do {
    if ((s= strchr(plugs, ';')))
      *s= '\\0';
    mysql_load_plugin(mysql, plugs, -1, 0);
    plugs= s + 1;
  } while (s);

  free(free_env);
}
int mysql_client_plugin_init()
{
  MYSQL mysql;
  struct st_mysql_client_plugin **builtin;
  va_list unused;
  LINT_INIT_STRUCT(unused);

  if (initialized)
    return 0;

  memset(&mysql, 0, sizeof(mysql)); /* dummy mysql for set_mysql_extended_error */

  pthread_mutex_init(&LOCK_load_client_plugin, MY_MUTEX_INIT_SLOW);
  ma_init_alloc_root(&mem_root, 128, 128);

  memset(&plugin_list, 0, sizeof(plugin_list));

  initialized= 1;

  pthread_mutex_lock(&LOCK_load_client_plugin);
  for (builtin= mysql_client_builtins; *builtin; builtin++)
    add_plugin(&mysql, *builtin, 0, 0, unused);

  pthread_mutex_unlock(&LOCK_load_client_plugin);

  load_env_plugins(&mysql);

  return 0;
}
void mysql_client_plugin_deinit()
{
  int i;
  struct st_client_plugin_int *p;

  if (!initialized)
    return;

  for (i=0; i < MYSQL_CLIENT_MAX_PLUGINS; i++)
    for (p= plugin_list[i]; p; p= p->next)
    {
      if (p->plugin->deinit)
        p->plugin->deinit();
      if (p->dlhandle)
        (void)dlclose(p->dlhandle);
    }

  memset(&plugin_list, 0, sizeof(plugin_list));
  initialized= 0;
  ma_free_root(&mem_root, MYF(0));
  pthread_mutex_destroy(&LOCK_load_client_plugin);
}
struct st_mysql_client_plugin * STDCALL
mysql_client_register_plugin(MYSQL *mysql,
                             struct st_mysql_client_plugin *plugin)
{
  va_list unused;
  LINT_INIT_STRUCT(unused);

  if (is_not_initialized(mysql, plugin->name))
    return NULL;

  pthread_mutex_lock(&LOCK_load_client_plugin);

  /* make sure the plugin wasn't loaded meanwhile */
  if (find_plugin(plugin->name, plugin->type))
  {
    my_set_error(mysql, CR_AUTH_PLUGIN_CANNOT_LOAD,
                 SQLSTATE_UNKNOWN, ER(CR_AUTH_PLUGIN_CANNOT_LOAD),
                 plugin->name, "it is already loaded");
    plugin= NULL;
  }
  else
    plugin= add_plugin(mysql, plugin, 0, 0, unused);

  pthread_mutex_unlock(&LOCK_load_client_plugin);
  return plugin;
}
struct st_mysql_client_plugin * STDCALL
mysql_load_plugin_v(MYSQL *mysql, const char *name, int type,
                    int argc, va_list args)
{
  const char *errmsg;
#ifdef _WIN32
  char errbuf[1024];
#endif
  char dlpath[FN_REFLEN+1];
  void *sym, *dlhandle;
  struct st_mysql_client_plugin *plugin;
  char *env_plugin_dir= getenv("MARIADB_PLUGIN_DIR");

  CLEAR_CLIENT_ERROR(mysql);
  if (is_not_initialized(mysql, name))
    return NULL;

  pthread_mutex_lock(&LOCK_load_client_plugin);

  /* make sure the plugin wasn't loaded meanwhile */
  if (type >= 0 && find_plugin(name, type))
  {
    errmsg= "it is already loaded";
    goto err;
  }

  /* Compile dll path */
  snprintf(dlpath, sizeof(dlpath) - 1, "%s/%s%s",
           mysql->options.extension && mysql->options.extension->plugin_dir ?
           mysql->options.extension->plugin_dir : (env_plugin_dir) ? env_plugin_dir :
           MARIADB_PLUGINDIR, name, SO_EXT);

  /* Open new dll handle */
  if (!(dlhandle= dlopen((const char *)dlpath, RTLD_NOW)))
  {
#ifdef _WIN32
   char winmsg[255];
   size_t len;
   winmsg[0] = 0;
   FormatMessage(FORMAT_MESSAGE_FROM_SYSTEM,
                 NULL,
                 GetLastError(),
                 MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
                 winmsg, 255, NULL);
   len= strlen(winmsg);
   while (len > 0 && (winmsg[len - 1] == '\n' || winmsg[len - 1] == '\r'))
     len--;
   if (len)
     winmsg[len] = 0;
   snprintf(errbuf, sizeof(errbuf), "%s Library path is '%s'", winmsg, dlpath);
   errmsg= errbuf;
#else
    errmsg= dlerror();
#endif
    goto err;
  }


  if (!(sym= dlsym(dlhandle, plugin_declarations_sym)))
  {
    errmsg= "not a plugin";
    (void)dlclose(dlhandle);
    goto err;
  }

  plugin= (struct st_mysql_client_plugin*)sym;

  if (type >=0 && type != plugin->type)
  {
    errmsg= "type mismatch";
    goto err;
  }

  if (strcmp(name, plugin->name))
  {
    errmsg= "name mismatch";
    goto err;
  }

  if (type < 0 && find_plugin(name, plugin->type))
  {
    errmsg= "it is already loaded";
    goto err;
  }

  plugin= add_plugin(mysql, plugin, dlhandle, argc, args);

  pthread_mutex_unlock(&LOCK_load_client_plugin);

  return plugin;

err:
  pthread_mutex_unlock(&LOCK_load_client_plugin);
  my_set_error(mysql, CR_AUTH_PLUGIN_CANNOT_LOAD, SQLSTATE_UNKNOWN,
               ER(CR_AUTH_PLUGIN_CANNOT_LOAD), name, errmsg);
  return NULL;
}
struct st_mysql_client_plugin * STDCALL
mysql_load_plugin(MYSQL *mysql, const char *name, int type, int argc, ...)
{
  struct st_mysql_client_plugin *p;
  va_list args;
  va_start(args, argc);
  p= mysql_load_plugin_v(mysql, name, type, argc, args);
  va_end(args);
  return p;
}
struct st_mysql_client_plugin * STDCALL
mysql_client_find_plugin(MYSQL *mysql, const char *name, int type)
{
  struct st_mysql_client_plugin *p;
  int plugin_nr= get_plugin_nr(type);

  if (is_not_initialized(mysql, name))
    return NULL;

  if (plugin_nr == -1)
  {
    my_set_error(mysql, CR_AUTH_PLUGIN_CANNOT_LOAD, SQLSTATE_UNKNOWN,
                 ER(CR_AUTH_PLUGIN_CANNOT_LOAD), name, "invalid type");
  }

  if ((p= find_plugin(name, type)))
    return p;

  /* not found, load it */
  return mysql_load_plugin(mysql, name, type, 0);
}
EOF""",
)

genrule(
    name = "mariadb_version_h",
    outs = ["include/mariadb_version.h"],
    cmd = """
        cat <<EOF >$@
        #ifndef _mariadb_version_h_
#define _mariadb_version_h_

#ifdef _CUSTOMCONFIG_
#include <custom_conf.h>
#else
#define PROTOCOL_VERSION		10
#define MARIADB_CLIENT_VERSION_STR	"10.3.6"
#define MARIADB_BASE_VERSION		"mariadb-10.3"
#define MARIADB_VERSION_ID		100306
#define MYSQL_VERSION_ID		100306
#define MARIADB_PORT	        	3306
#define MARIADB_UNIX_ADDR               "/tmp/mysql.sock"
#define MYSQL_CONFIG_NAME		"my"

#define MARIADB_PACKAGE_VERSION "3.0.4"
#define MARIADB_PACKAGE_VERSION_ID 30004
#define MARIADB_SYSTEM_TYPE "Linux"
#define MARIADB_MACHINE_TYPE "x86_64"
#define MARIADB_PLUGINDIR "lib/mariadb/plugin"

/* mysqld compile time options */
#ifndef MYSQL_CHARSET
#define MYSQL_CHARSET			""
#endif
#endif

/* Source information */
#define CC_SOURCE_REVISION ""

#endif /* _mariadb_version_h_ */
EOF
        """,
)

genrule(
    name = "ma_config_h",
    outs = ["include/ma_config.h"],
    cmd = """
    cat <<EOF >$@
#define HAVE_ALLOCA_H 1
#define HAVE_SETLOCALE 1
#define HAVE_NL_LANGINFO 1
#define HAVE_ARPA_INET_H 1
#define HAVE_CRYPT_H 1
#define HAVE_DIRENT_H 1
#define HAVE_DLFCN_H 1
#define HAVE_EXECINFO_H 1
#define HAVE_FCNTL_H 1
#define HAVE_FENV_H 1
#define HAVE_FLOAT_H 1
#define HAVE_GRP_H 1
#define HAVE_LIMITS_H 1
#define HAVE_MALLOC_H 1
#define HAVE_MEMORY_H 1
#define HAVE_NETINET_IN_H 1
#define HAVE_PATHS_H 1
#define HAVE_PWD_H 1
#define HAVE_SCHED_H 1
#define HAVE_STDDEF_H 1
#define HAVE_STDINT_H 1
#define HAVE_STDLIB_H 1
#define HAVE_STRING_H 1
#define HAVE_STRINGS_H 1
#define HAVE_SYS_IOCTL_H 1
#define HAVE_SYS_IPC_H 1
#define HAVE_SYS_MMAN_H 1
#define HAVE_SYS_PRCTL_H 1
#define HAVE_SYS_SELECT_H 1
#define HAVE_SYS_SHM_H 1
#define HAVE_SYS_SOCKET_H 1
#define HAVE_SYS_STAT_H 1
#define HAVE_SYS_TIMEB_H 1
#define HAVE_SYS_TYPES_H 1
#define HAVE_SYS_UN_H 1
#define HAVE_TERMIO_H 1
#define HAVE_TERMIOS_H 1
#define HAVE_UNISTD_H 1
#define HAVE_UTIME_H 1
#define HAVE_UCONTEXT_H 1
#define HAVE_ACCESS 1
#define HAVE_ALARM 1
#define HAVE_BCMP 1
#define HAVE_BZERO 1
#define HAVE_CLOCK_GETTIME 1
#define HAVE_DLERROR 1
#define HAVE_DLOPEN 1
#define HAVE_FCHMOD 1
#define HAVE_FCNTL 1
#define HAVE_FDATASYNC 1
#define HAVE_FESETROUND 1
#define HAVE_FINITE 1
#define HAVE_FSEEKO 1
#define HAVE_FSYNC 1
#define HAVE_GETADDRINFO 1
#define HAVE_GETCWD 1
#define HAVE_GETHOSTBYADDR_R 1
#define HAVE_GETHOSTBYNAME_R 1
#define HAVE_GETNAMEINFO 1
#define HAVE_GETPAGESIZE 1
#define HAVE_GETPASS 1
#define HAVE_GETPWNAM 1
#define HAVE_GETPWUID 1
#define HAVE_GETRLIMIT 1
#define HAVE_GETRUSAGE 1
#define HAVE_GETWD 1
#define HAVE_GMTIME_R 1
#define HAVE_INITGROUPS 1
#define HAVE_LDIV 1
#define HAVE_LOCALTIME_R 1
#define HAVE_LOG2 1
#define HAVE_LONGJMP 1
#define HAVE_LSTAT 1
#define HAVE_MADVISE 1
#define HAVE_MALLINFO 1
#define HAVE_MEMALIGN 1
#define HAVE_MEMCPY 1
#define HAVE_MEMMOVE 1
#define HAVE_MKSTEMP 1
#define HAVE_MLOCK 1
#define HAVE_MLOCKALL 1
#define HAVE_MMAP 1
#define HAVE_MMAP64 1
#define HAVE_PERROR 1
#define HAVE_POLL 1
#define HAVE_PREAD 1
#define HAVE_PTHREAD_ATTR_GETSTACKSIZE 1
#define HAVE_PTHREAD_ATTR_SETSCHEDPARAM 1
#define HAVE_PTHREAD_ATTR_SETSCOPE 1
#define HAVE_PTHREAD_ATTR_SETSTACKSIZE 1
#define HAVE_PTHREAD_KEY_DELETE 1
#define HAVE_PTHREAD_KILL 1
#define HAVE_PTHREAD_RWLOCK_RDLOCK 1
#define HAVE_PTHREAD_SETSCHEDPARAM 1
#define HAVE_PTHREAD_SIGMASK 1
#define HAVE_READDIR_R 1
#define HAVE_READLINK 1
#define HAVE_REALPATH 1
#define HAVE_RENAME 1
#define HAVE_SCHED_YIELD 1
#define HAVE_SELECT 1
#define HAVE_SIGNAL 1
#define HAVE_SIGACTION 1
#define HAVE_SIGWAIT 1
#define HAVE_SLEEP 1
#define HAVE_SNPRINTF 1
#define HAVE_STPCPY 1
#define HAVE_STRERROR 1
#define HAVE_STRNLEN 1
#define HAVE_STRPBRK 1
#define HAVE_STRSEP 1
#define HAVE_STRSTR 1
#define HAVE_STRTOK_R 1
#define HAVE_STRTOL 1
#define HAVE_STRTOLL 1
#define HAVE_STRTOUL 1
#define HAVE_STRTOULL 1
#define HAVE_VASPRINTF 1
#define HAVE_VSNPRINTF 1

#define SIZEOF_CHAR 1
#if SIZEOF_CHAR
# define HAVE_CHAR 1
#endif

#define SIZEOF_CHARP 8
#if SIZEOF_CHARP
# define HAVE_CHARP 1
#endif

#define SIZEOF_SHORT 2
#if SIZEOF_SHORT
# define HAVE_SHORT 1
#endif

#define SIZEOF_INT 4
#if SIZEOF_INT
# define HAVE_INT 1
#endif

#define SIZEOF_LONG 8
#if SIZEOF_LONG
# define HAVE_LONG 1
#endif

#define SIZEOF_LONG_LONG 8
#if SIZEOF_LONG_LONG
# define HAVE_LONG_LONG 1
#endif

#define SIZEOF_SIGSET_T 128
#if SIZEOF_SIGSET_T
# define HAVE_SIGSET_T 1
#endif

#define SIZEOF_SIZE_T 8
#if SIZEOF_SIZE_T
# define HAVE_SIZE_T 1
#endif

#if SIZEOF_UCHAR
# define HAVE_UCHAR 1
#endif

#define SIZEOF_UINT 4
#if SIZEOF_UINT
# define HAVE_UINT 1
#endif

#define SIZEOF_ULONG 8
#if SIZEOF_ULONG
# define HAVE_ULONG 1
#endif

#if SIZEOF_INT8
# define HAVE_INT8 1
#endif
#if SIZEOF_UINT8
# define HAVE_UINT8 1
#endif

#if SIZEOF_INT16
# define HAVE_INT16 1
#endif
#if SIZEOF_UINT16
# define HAVE_UINT16 1
#endif

#if SIZEOF_INT32
# define HAVE_INT32 1
#endif
#if SIZEOF_UINT32
# define HAVE_UINT32 1
#endif
#if SIZEOF_U_INT32_T
# define HAVE_U_INT32_T 1
#endif

#if SIZEOF_INT64
# define HAVE_INT64 1
#endif
#if SIZEOF_UINT64
# define HAVE_UINT64 1
#endif

#if SIZEOF_SOCKLEN_T
# define HAVE_SOCKLEN_T 1
#endif

#define SOCKET_SIZE_TYPE socklen_t
EOF""",
)
