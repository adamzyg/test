#include <stdlib.h>
#include <stdio.h>
#include <clish/plugin.h>
#include <clish/shell.h>
#include <clish/pargv.h>

int clish_plugin_explugin(const clish_command_t *cmd, clish_pargv_t *argv);

CLISH_PLUGIN_SYM(explugin_fn)
{
	int rc = -1;
	printf("explugin: Hello world\n");
	
	/*get command and argv from klish*/
	clish_context_t *cli_context = (clish_context_t *)clish_context;
	clish_pargv_t *argv = clish_context__get_pargv(cli_context);
	const clish_command_t *cmd = clish_context__get_cmd(cli_context);
	
	/*reslove and transfer command*/
	rc = clish_plugin_explugin(cmd, argv);
	return rc;
}

CLISH_PLUGIN_FINI(explugin_fini)
{
    printf("Enter into CLISH_PLUGIN_FINI\n");
	
	return 0;
}

CLISH_PLUGIN_INIT(explugin)
{
    printf("Enter into CLISH_PLUGIN_INIT\n");
	clish_plugin_add_fini(plugin, explugin_fini);
	clish_plugin_add_sym(plugin, explugin_fn, "hello");

	return 0;
}

int clish_plugin_explugin(const clish_command_t *cmd, clish_pargv_t *argv)
{
    int rc = 0;
    int i = 0;
    int argc = 0;
	clish_parg_t *arg;
	const char *cmdname;
	const char *pargvname;
	const char *pargvvalue;

    cmdname= clish_command__get_name(cmd);
	if(!cmdname){
		fprintf(stderr, "command name is null\n");
		return -1;
	}
    printf("cmdname: #%s#\n", cmdname);

	argc = clish_pargv__get_count(argv);
    for (i = 0; i < argc; i++) {
        arg = clish_pargv__get_parg(argv, i);
        if (!arg) {
            fprintf(stderr, "argv[%d] is null\n", i);
            return -1;
        }

        pargvname = clish_parg__get_name(arg);
        if (!pargvname) {
            fprintf(stderr, "argv[%d] name is null\n", i);
            return -1;
        }
        pargvvalue = clish_parg__get_value(arg);

        printf("argv[%d]: name: #%s# value: #%s#\n", i, pargvname, pargvvalue);
    }
    return rc;
}


