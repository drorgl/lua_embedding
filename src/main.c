#include <stdint.h>
#include <time.h>
#include <math.h>

#include <stdlib.h>

#include <lua.h>
#include <lualib.h>
#include <lauxlib.h>

void time_c_function()
{
    clock_t start, end;
    double cpu_time_used;

    start = clock();

    size_t iterations = 1000000;

    for (uint32_t i = 0; i < iterations; i++)
    {
        float a = i;
        float b = i;
        float value = (powf(a, 2) / sin(2 * M_PI / b)) - a / 2;
        (void)value;
    }

    end = clock();
    cpu_time_used = ((double)(end - start));
    printf("c %zu iterations took %f ms, %f per iteration, %f calcs per ms\r\n", iterations, cpu_time_used, cpu_time_used / iterations, iterations / cpu_time_used);
}

static void l_message(const char *pname, const char *msg)
{
    if (pname)
        lua_writestringerror("%s: ", pname);
    lua_writestringerror("%s\n", msg);
}

int main(int argc, char **argv)
{
    time_c_function();
    printf("creating runtime\r\n");
    int status, result;
    lua_State *L = luaL_newstate(); /* create state */
    if (L == NULL)
    {
        l_message(argv[0], "cannot create state: not enough memory");
        return EXIT_FAILURE;
    }

    printf("setting libs\r\n");
    luaL_openlibs(L);

    printf("evaluating expression\r\n");
    char *code = "function transform(a,b)\r\n \
return (a^2/math.sin(2*math.pi/b))-(a/2)\r\n \
end\r\n";

    // Here we load the string and use lua_pcall for run the code
    if (luaL_loadstring(L, code) == LUA_OK)
    {
        if (lua_pcall(L, 0, 0, 0) == LUA_OK)
        {
            // If it was executed successfully we remove the code from the stack
            lua_pop(L, lua_gettop(L));
        }
    }

    printf("calling function\r\n");
    clock_t start, end;
    double cpu_time_used;

    start = clock();

    size_t iterations = 1000000;

    for (size_t i = 1; i < iterations; i++)
    {
        lua_getglobal(L, "transform");
        lua_pushnumber(L, i);
        lua_pushnumber(L, i);
        lua_call(L, 2, 1);

        double res = lua_tonumber(L, -1);
        (void)(res);
        // printf("val %f\r\n", res);

        lua_pop(L, 1);
    }
    end = clock();
    cpu_time_used = ((double)(end - start));
    printf("lua %zu iterations took %f ms, %f per iteration, %f calcs per ms\r\n", iterations, cpu_time_used, cpu_time_used / iterations, iterations / cpu_time_used);

    lua_close(L);
    return EXIT_SUCCESS;
}

void app_main()
{
    main(0, NULL);
}